/**************************************************************************
 * 	decode MPEG-4 video (C) 2008 Rockins Chen
 *
 * 	$Id: decode.h 9 2008-02-17 08:09:01Z rockins $
 *
 *************************************************************************/
#ifndef	_DECODE_H_
#define	_DECODE_H_

#include <pthread.h>
#include "common.h"
#include "event.h"
#include "libavi.h"
#include "pingpong_buf_mgmt.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * decode object
 */
typedef	struct _decode_t decode_t;

struct _decode_t {

/*
 * maximum length of decode_t.decoder_name
 */
#define	MAX_LEN_DECODER_NAME	16

	char			*decoder_name;		// decoder's name

	unsigned int	 width;				// width of video frame, in pixel
	unsigned int	 height;			// height of video frame, in pixel
	unsigned int	 depth;				// Bytes per pixel
	unsigned int	 colorspace;		// color space
	double			 ratio;				// width / height
	unsigned long	 pts_unit;			// presentation timestamp unit, in ms, reserved

	unsigned char	*in_buf;			// input buff(for input stream)
	unsigned char	*out_buf;			// output buff(for decoded picture)

/*
 * default size of: decode_t.in_buf
 */
#define	DFL_DEC_IN_BUF_SZ		512 * 1024

	int				 in_buf_sz;			// input buff's size, in byte

/*
 * decoding threshold: minimum bytes remain in
 * decode_t.in_buf to correctly decode.
 */
#define	MIN_INPUT_BYTES			1

	int				 in_buf_used;		// used size of input buff, in byte

/*
 * default size of: decode_t.out_buf
 */
#define	DFL_DEC_OUT_BUF_SZ		MAX_VID_WID * MAX_VID_HGT * MAX_VID_BPP	

	int				 out_buf_sz;		// output buff's size, in byte
	int				 out_buf_used;		// used size of output buff, in byte

	void			*dec_handle;		// xvid decoder handle
	AVI				*avi;				// avi object, as inputing tool
	event_t			*dec_to_out_evt;	// event object, sync: dec -> out
	event_t			*out_to_dec_evt;	// event object, sync: out -> dec
	pingpong_buf_mgmt_t	*ppbm;			// pingpong management object
	pthread_t		 pt_id;				// decoding thread id
	pthread_attr_t	 pt_attr;			// decoding thread attribute
	PtExitCodeType	 pt_exit_code;		// decoding thread exit code

	/*
	 * decode object's methods
	 */
	int				(*Init)(decode_t			*dec, 
							AVI 				*pAvi, 
							event_t 			*pDecToOutEvt, 
							event_t 			*pOutToDecEvt,
							pingpong_buf_mgmt_t	*pPpbm);

	int				(*MainLoop)(decode_t *dec);

	int				(*Term)(decode_t *dec);
};

decode_t* dec_new(void);
void dec_del(decode_t *);

#ifdef	__cplusplus
}
#endif

#endif	//_DECODE_H_

