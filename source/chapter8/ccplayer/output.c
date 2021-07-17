/**************************************************************************
 * 	output by sdl (C) 2008 Rockins Chen
 *
 * 	$Id: output.c 11 2008-03-09 15:37:46Z rockins $
 *************************************************************************/
#include <stdlib.h>
#include <pthread.h>
#include <xvid.h>
#include <assert.h>
#include "common.h"
#include "event.h"
#include "output.h"

/*
 * output thread
 * @arg		should be cast to output_t*(output object)
 */
static void* Out_Thread_Func(void *arg)
{
	int			 rc;
	output_t	*out = (output_t *)arg;
	EvtType		 et;
	
	/* decoded frame's rect */
	SDL_Rect	 rgb_frm_rect;	
	
	/* screen's drawable rect */
	SDL_Rect	 scr_drw_rect;	

	/* SDL window manager event */
	SDL_Event	 wm_evt;		

	while (1) {
		/*
		 * first notify decode thread that output has finished, 
		 * then wait for decode thread's notification
		 */
		out->out_to_dec_evt->set_type(out->out_to_dec_evt,
									  EVT_TYPE_PREV_FRAME_OUTPUTED);
		out->out_to_dec_evt->sgnl(out->out_to_dec_evt);

		out->dec_to_out_evt->wait(out->dec_to_out_evt);
		et = out->dec_to_out_evt->get_type(out->dec_to_out_evt);

		/*
		 * if no more frame is available, exit
		 */
		if (et == EVT_TYPE_NO_FRAME_AVAIL) {
			out->pt_exit_code = PT_EXIT_NORMAL;
			pthread_exit((void *)&out->pt_exit_code);
			return (NULL);
		}

		/*
		 * retrieve frame parameter
		 */
		out->width 		= out->dec_to_out_evt->extra->frm_width;
		out->height 	= out->dec_to_out_evt->extra->frm_height;
		out->colorspace	= out->dec_to_out_evt->extra->frm_colorspace;
		out->bpp 		= DFL_SCR_BPP;
		out->frame 		= out->ppbm->get_pp_buf(out->ppbm);

		/*
		 * create surface for the frame
		 */
		out->sdl->decoded_rgb_frm = SDL_CreateRGBSurfaceFrom(
				out->frame, 
				out->width,
				out->height, 
				out->bpp,
				out->width * out->bpp / 8,
				out->sdl->Rmask,
				out->sdl->Gmask,
				out->sdl->Bmask,
				out->sdl->Amask
				);

		/* 
		 * set parameter for bliting rgb_frm to screen 
		 */
		rgb_frm_rect.x = 0;
		rgb_frm_rect.y = 0;
		rgb_frm_rect.w = out->width;
		rgb_frm_rect.h = out->height;

		scr_drw_rect.x = (out->sdl->scr_wid - out->width) / 2;
		scr_drw_rect.y = (out->sdl->scr_hgt - out->height) / 2;
		scr_drw_rect.w = out->width;
		scr_drw_rect.h = out->height;

		/*
		 * blit decoded rgb frame to screen and update screen
		 */
		rc = SDL_BlitSurface(out->sdl->decoded_rgb_frm,
							 &rgb_frm_rect,
							 out->sdl->screen,
							 &scr_drw_rect);
		if (rc == -1) {
			ERROR("blit frame to screen failed");
			out->pt_exit_code = PT_EXIT_ABNORMAL;
			goto out;
		}

		SDL_UpdateRects(out->sdl->screen, 1, &scr_drw_rect);

		/*
		 * delay a short time. 
		 * FIXME: currently this delay time is a magic number. in
		 * future version it should be a runtime value ^_^
		 */
		SDL_Delay(20);

		/*
		 * after outputing, release the decoded RGB frame
		 */
		if (out->sdl->decoded_rgb_frm)
			SDL_FreeSurface(out->sdl->decoded_rgb_frm);

		/*
		 * check whether window event happened
		 */
		if (SDL_PollEvent(&wm_evt)) {
			switch (wm_evt.type) {
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					if (wm_evt.key.keysym.sym == SDLK_ESCAPE) {
						/*
						 * Esc key be pressed
						 * action: prompt and exit
						 */
						printf("Esc key pressed, prepare to exit\n");
						out->pt_exit_code = PT_EXIT_NORMAL;
						goto out;
					}
					break;
				default:
					break;
			}
		}
	}

out:
	/*
	 * nofity decode part that outputing is aborted
	 */
	out->out_to_dec_evt->set_type(out->out_to_dec_evt,
								  EVT_TYPE_OUTPUT_ABORT);
	out->out_to_dec_evt->sgnl(out->out_to_dec_evt);

	pthread_exit((void *)&out->pt_exit_code);
	return (NULL);
}

/**
 * init output object
 */
static int out_Init(output_t			*out, 
					event_t 			*pDecToOutEvt,
					event_t 			*pOutToDecEvt,
					pingpong_buf_mgmt_t *pPpbm)
{
	int				  rc;
	SDL_Rect		**modes;
	SDL_PixelFormat	  pixel;

	rc = SDL_Init(SDL_INIT_VIDEO);
	if (rc) {
		ERROR("SDL_Init() failed");
		return (rc);
	}

	/*
	 * allocate & initialize out->sdl
	 */
	out->sdl = malloc(sizeof(struct _sdl));
	assert(out->sdl);
	memset(out->sdl, 0, sizeof(struct _sdl));

	out->sdl->scr_wid	= DFL_SCR_WID;
	out->sdl->scr_hgt	= DFL_SCR_HGT;
	out->sdl->scr_bpp	= DFL_SCR_BPP;
	out->sdl->scr_flg	= DFL_SCR_FLG;

	out->sdl->Amask		= DFL_SDL_AMASK;
	out->sdl->Rmask 	= DFL_SDL_RMASK;
	out->sdl->Gmask 	= DFL_SDL_GMASK;
	out->sdl->Bmask 	= DFL_SDL_BMASK;

	out->sdl->Ashift 	= DFL_SDL_ASHIFT;
	out->sdl->Rshift 	= DFL_SDL_RSHIFT;
	out->sdl->Gshift 	= DFL_SDL_GSHIFT;
	out->sdl->Bshift 	= DFL_SDL_BSHIFT;

	/*
	 * check whether default mode is supported or not
	 */
	memset(&pixel, 0, sizeof(SDL_PixelFormat));
	pixel.palette 		= NULL;
	pixel.BitsPerPixel 	= DFL_SCR_BPP;
	pixel.BytesPerPixel	= DFL_SCR_BPP / 8;
	pixel.Amask 		= DFL_SDL_AMASK;
	pixel.Rmask 		= DFL_SDL_RMASK;
	pixel.Gmask 		= DFL_SDL_GMASK;
	pixel.Bmask 		= DFL_SDL_BMASK;
	pixel.Ashift 		= DFL_SDL_ASHIFT;
	pixel.Rshift 		= DFL_SDL_RSHIFT;
	pixel.Gshift 		= DFL_SDL_GSHIFT;
	pixel.Bshift 		= DFL_SDL_BSHIFT;
	modes = SDL_ListModes(&pixel, DFL_SCR_FLG);
	if (modes) {
		printf("default mode is supported\n");
	} else {
		printf("default mode is not supported\n");
		exit(-1);
	}

	/*
	 * init SDL screen
	 */
	out->sdl->screen = SDL_SetVideoMode(out->sdl->scr_wid,
										out->sdl->scr_hgt,
										out->sdl->scr_bpp,
										out->sdl->scr_flg);

	/*
	 * associate with events and pingpong
	 */
	out->dec_to_out_evt = pDecToOutEvt;
	out->out_to_dec_evt = pOutToDecEvt;
	out->ppbm 			= pPpbm;

	/* sdl supporting for output is now initialized*/
	out->sdl->initialized = 1;

	/* set window caption */
	SDL_WM_SetCaption("ccplayer(v0.1 by rockins)", NULL);

	return (0);
}

/**
 * startup output thread
 * @out		output object
 */
static int out_MainLoop(output_t *out)
{
	int			rc;

	pthread_attr_init(&out->pt_attr);

	rc = pthread_create(&out->pt_id, &out->pt_attr,
			Out_Thread_Func, out);

	return (rc);
}

/**
 * cleanup
 */
static int out_Term(output_t *out)
{
	SDL_Quit();

	if (out->sdl)
		free(out->sdl);

	return (0);
}

/**
 * constructor for output object
 * $ret		output_t*(output object)
 */
output_t* out_new(void)
{
	output_t* out = (output_t *)malloc(sizeof(output_t));
	assert(out != NULL);

	out->Init 		= out_Init;
	out->MainLoop 	= out_MainLoop;
	out->Term 		= out_Term;

	return (out);
}

/**
 * de-constructor for output object
 * @out		output object to be destroyed
 */
void out_del(output_t *out)
{
	if (out) {
		/* call cleanup */
		if (out->Term)
			out->Term(out);
		
		out->Init 		= NULL;
		out->MainLoop 	= NULL;
		out->Term 		= NULL;

		free(out);
	}
}

