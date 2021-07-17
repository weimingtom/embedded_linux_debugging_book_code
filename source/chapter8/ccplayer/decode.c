/**************************************************************************
 *	decode xvid	(C) 2008 Rockins Chen
 *
 * Decode MPEG-4 bitstream. MPEG-4 bitstream is acquired 
 * and demuxed from AVI file. Once decoded out a frame,
 * notify outputing thread by event object.
 *
 * Bug report:Rockins Chen<ybc2084@163.com>
 *
 * All Copyrights reserved.
 * 
 * $Id: decode.c 9 2008-02-17 08:09:01Z rockins $
 * 
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "xvid.h"
#include "decode.h"


/**
 * main body of decoding process, wrap xvid's
 * decode interface.
 * @dec		decoding object
 */
static void xvid_dec_wrap(decode_t *dec)
{
	int					 used_bytes;
	int					 avai_bytes;

	/* outputing thread's notification type */
	EvtType				 et;			

	/* xvid structure for decoding */
	xvid_dec_stats_t	 dec_stats;
	xvid_dec_frame_t   	 dec_frame;

	do {

		/*
		 * clear & seting xvid decoding object
		 */
		memset(&dec_stats, 0, sizeof(xvid_dec_stats_t));
		memset(&dec_frame, 0, sizeof(xvid_dec_frame_t));
		dec_stats.version = XVID_VERSION;
		dec_frame.version = XVID_VERSION;
		
		/*
		 * frame setting
		 */
		dec_frame.general 			= 0;
		dec_frame.bitstream 		= dec->in_buf;
		dec_frame.length			= dec->in_buf_used;
		dec_frame.output.plane[0]	= dec->ppbm->get_pp_buf(dec->ppbm);
		dec_frame.output.stride[0] 	= dec->width * dec->depth;
		dec_frame.output.csp 		= XVID_CSP_ARGB;

		/*
		 * decoding till consume out all input stream.
		 */
		used_bytes = 0;
		avai_bytes = dec->in_buf_used;

		used_bytes = xvid_decore(dec->dec_handle, XVID_DEC_DECODE, 
								&dec_frame, &dec_stats);

		/*
		 * in case of two situations:
		 * 	1. do not consume anything
		 * 	2. go too far
		 */
		if (used_bytes == 0)
			break;
		else if (used_bytes < 0) {
			/*
			 * some error must occur
			 */
			ERROR("we go too far");
			break;
		}

		/*
		 * use dec_stats.type to determine output type(according to xvid.h):
		 * 		-1		VOL 
		 * 		0		NOTHING
		 * 		1...4	IVOP/PVOP/BVOP/SVOP
		 */
		switch (dec_stats.type) {
			case XVID_TYPE_VOL:
				/*
				 * FIXME: beyond max dimension is an error now;
				 * should be fixed in future. i.e. future version
				 * shall be able to accept variable dimension, 
				 * rather than hardcode a maximum limitation
				 */
				if (dec_stats.data.vol.width > MAX_VID_WID ||
					dec_stats.data.vol.height > MAX_VID_HGT) {
					ERROR("beyond allowed maximum dimension");
					dec->pt_exit_code = PT_EXIT_ABNORMAL;
					pthread_exit(&dec->pt_exit_code);
				}

				/*
				 * if do not exceed maximum dimension, then just
				 * update decoding object's parameter.
				 */
				dec->width 	= dec_stats.data.vol.width;
				dec->height	= dec_stats.data.vol.height;
				break;
			case XVID_TYPE_NOTHING:
				/*
				 * do nothing
				 */
				break;
			case XVID_TYPE_IVOP:
			case XVID_TYPE_PVOP:
			case XVID_TYPE_BVOP:
			case XVID_TYPE_SVOP:
				/*
				 * notify outputing (e.g. display) part. 
				 * NOTICE: must first wait output's notification
				 */

				dec->out_to_dec_evt->wait(dec->out_to_dec_evt);
				et = dec->out_to_dec_evt->get_type(dec->out_to_dec_evt);

				if (et == EVT_TYPE_PREV_FRAME_OUTPUTED) {
					/*
					 * previous frame has been outputed
					 */

					/*
					 * set extra info: width, height, colorspace.
					 * for decoded frame's dimension may be changed,
					 * it's desired to send event with this extra info
					 */
					dec->dec_to_out_evt->lock(dec->dec_to_out_evt);
					dec->dec_to_out_evt->extra->frm_width 		= dec->width;
					dec->dec_to_out_evt->extra->frm_height 		= dec->height;
					dec->dec_to_out_evt->extra->frm_colorspace 	= dec->colorspace;
					dec->dec_to_out_evt->unlock(dec->dec_to_out_evt);

					/*
					 * set notification type and signal outputing part
					 */
					dec->dec_to_out_evt->set_type(dec->dec_to_out_evt, 
												  EVT_TYPE_NEW_FRAME_READY);
					dec->dec_to_out_evt->sgnl(dec->dec_to_out_evt);
				} else if (et == EVT_TYPE_OUTPUT_ABORT) {
					/*
					 * outputing thread has been aborted, then
					 * decoding thread should exit preamaturely.
					 */

					dec->pt_exit_code = PT_EXIT_ABNORMAL;
					pthread_exit(&dec->pt_exit_code);
				} else {
					/* unknown event type */
					ERROR("decode part:unknown event type");
					dec->pt_exit_code = PT_EXIT_ABNORMAL;
					pthread_exit(&dec->pt_exit_code);
				}
				break;
			default:
				/* unknown type */
				ERROR("unknown xvid type");
				break;
		}

		if (used_bytes > 0 && used_bytes <= avai_bytes) {
			/*
			 * move inputing bitstream forwardly
			 */

			avai_bytes -= used_bytes;
			memmove(dec->in_buf, dec->in_buf + used_bytes, avai_bytes);
			dec->in_buf_used = avai_bytes;
		}

	} while (dec->in_buf_used > MIN_INPUT_BYTES);
}

/**
 * decoding thread
 * @arg		should be cast to decode_t *(decode object)
 */
static void* Dec_Thread_Func(void *arg)
{
	int			 		 rc;
	int			 		 vblk_size;
	decode_t			*dec = (decode_t *)arg;

	/*
	 * init & parse avi
	 */
	dec->avi->init(dec->avi);
	dec->avi->parse(dec->avi);

	/*
	 * goto first video block
	 */
	dec->avi->goto_first_video_block(dec->avi);

	/*
	 * decode loop
	 */
	while (1) {
		vblk_size = dec->avi->get_curr_video_block_size(dec->avi);
		if (vblk_size < 0) {
			/*
			 * clearly, it's illegal that video block size < 0.
			 * XXX: currently only jump out decoding loop.
			 */
			ERROR("video block size < 0");
			dec->pt_exit_code = PT_EXIT_ABNORMAL;
			goto out;
		}
		
		/*
		 * if input video block larger than input buffer,
		 * then enlarge input buffer.
		 */
		if (dec->in_buf_used + vblk_size > dec->in_buf_sz) {
			dec->in_buf = realloc(dec->in_buf, dec->in_buf_used + vblk_size);
			if (dec->in_buf) {
				dec->in_buf_sz = dec->in_buf_used + vblk_size;
			} else {
				/* enlarge input buffer failed */
				ERROR("enlarge decoding input buffer failed");
				dec->pt_exit_code = PT_EXIT_ABNORMAL;
				goto out;
			}
		}

		/*
		 * get video block. NOTICE: data remain in input buffer
		 * should be skipped, in other word, reserved.
		 */
		rc = dec->avi->get_curr_video_block(
								dec->avi, 
								(char *)(dec->in_buf + dec->in_buf_used), 
								vblk_size);
		if (rc < 0) {
			ERROR("get video block failed");
			dec->pt_exit_code = PT_EXIT_ABNORMAL;
			goto out;
		}

		dec->in_buf_used += vblk_size;

		/*
		 * actual decoding work is done here
		 */
		xvid_dec_wrap(dec);

		/*
		 * goto next video block
		 */
		rc = dec->avi->goto_next_video_block(dec->avi);
		if (!rc) {
			/*
			 * reach input file's end. 
			 * action: prompt and exit
			 */

			printf("reach input file's end\n");
			dec->pt_exit_code = PT_EXIT_NORMAL;
			goto out;
		}
	}
	
out:
 	/* clear extra info */
	dec->dec_to_out_evt->lock(dec->dec_to_out_evt);
	dec->dec_to_out_evt->extra->frm_width = 0;
	dec->dec_to_out_evt->extra->frm_height = 0;
	dec->dec_to_out_evt->extra->frm_colorspace = 0;
	dec->dec_to_out_evt->unlock(dec->dec_to_out_evt);
	
	/*
	 * notify outputing part
	 */
	dec->dec_to_out_evt->set_type(dec->dec_to_out_evt,
								  EVT_TYPE_NO_FRAME_AVAIL);
	dec->dec_to_out_evt->sgnl(dec->dec_to_out_evt);

	/*
	 * reset pingpong buffer to NULL
	 */
	dec->ppbm->reset_curr(dec->ppbm);

	/* exit */
	pthread_exit((void *)&dec->pt_exit_code);
}

/**
 * init decoding object
 */
static int dec_Init(decode_t 			*dec, 
					AVI 				*pAvi, 
					event_t 			*pDecToOutEvt, 
					event_t 			*pOutToDecEvt,
					pingpong_buf_mgmt_t	*pPpbm)
{
	int					rc;
	xvid_gbl_init_t		gbl_init;
	xvid_dec_create_t	dec_create;

	/*
	 * setup decoding in/out buffer size.
	 * NOTE: since output buffer is managed by pingpong
	 * object, here didn't allocate space for outputing.
	 */
	dec->in_buf		= malloc(DFL_DEC_IN_BUF_SZ);
	dec->in_buf_sz 	= DFL_DEC_IN_BUF_SZ;
	dec->out_buf_sz	= DFL_DEC_OUT_BUF_SZ;

	/*
	 * set decoder's name: xvid
	 */
	dec->decoder_name = malloc(MAX_LEN_DECODER_NAME);
	strncpy(dec->decoder_name, "xvid", MAX_LEN_DECODER_NAME);

	/*
	 * connect to avi and event object, set output colorspace
	 */
	dec->avi 			= pAvi;
	dec->dec_to_out_evt	= pDecToOutEvt;
	dec->out_to_dec_evt = pOutToDecEvt;
	dec->ppbm 			= pPpbm;
	dec->colorspace 	= XVID_CSP_ARGB;
	dec->depth 			= MAX_VID_BPP;

	/*
	 * clear global init & decode create type
	 */
	memset(&gbl_init, 0, sizeof(xvid_gbl_init_t));
	memset(&dec_create, 0, sizeof(xvid_dec_create_t));

	/*
	 * initialize xvid core
	 */
	gbl_init.version 	= XVID_VERSION;
	gbl_init.cpu_flags 	= 0;
	xvid_global(NULL, 0, &gbl_init, NULL);

	/*
	 * initialize xvid decoder
	 */
	dec_create.version	= XVID_VERSION;
	dec_create.width 	= 0;
	dec_create.height 	= 0;
	rc = xvid_decore(NULL, XVID_DEC_CREATE, &dec_create, NULL);

	/*
	 * setup decoder's handle
	 */
	dec->dec_handle = ((rc < 0) ? NULL : dec_create.handle);
	assert(dec->dec_handle);

	return (rc);
}

/**
 * startup decoding
 * @dec		decode object
 */
static int dec_MainLoop(decode_t *dec)
{
	pthread_attr_init(&dec->pt_attr);

	pthread_create(&dec->pt_id, &dec->pt_attr, 
			Dec_Thread_Func, dec);

	return (0);
}

/**
 * cleanup decode
 * @dec		decode object
 */
static int dec_Term(decode_t *dec)
{
	int	rc = 0;

	/*
	 * destroy decoder's handle
	 */
	if (dec->dec_handle)
		rc = xvid_decore(dec->dec_handle, XVID_DEC_DESTROY, NULL, NULL);

	/*
	 * free buffers, if needed
	 */
	if (dec->decoder_name)	free(dec->decoder_name);
	if (dec->in_buf)		free(dec->in_buf);
	if (dec->out_buf)		free(dec->out_buf);

	return (rc);
}

/**
 * constructor for decoding object
 * $ret		decode_t*(decode object)
 */
decode_t* dec_new(void)
{
	decode_t	*dec;

	dec = malloc(sizeof(decode_t));
	assert(dec);
	memset(dec, 0, sizeof(dec));

	dec->Init 		= dec_Init;
	dec->MainLoop 	= dec_MainLoop;
	dec->Term 		= dec_Term;

	return (dec);
}

/**
 * de-constructor for decode object
 * @dec		decode object
 */
void dec_del(decode_t *dec)
{
	if (dec) {
		/*
		 * call cleanup
		 */
		if (dec->Term)
			dec->Term(dec);

		dec->Init 		= NULL;
		dec->MainLoop 	= NULL;
		dec->Term 		= NULL;

		free(dec);
	}
}

