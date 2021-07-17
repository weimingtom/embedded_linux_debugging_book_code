/**************************************************************************
 *
 * output decoded picture (C) 2008 Rockins Chen
 *
 * output decoded picture thru libSDL.
 *
 * $Id: output.h 9 2008-02-17 08:09:01Z rockins $
 *
 *************************************************************************/
#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <SDL.h>
#include "common.h"
#include "pingpong_buf_mgmt.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct _output_t output_t;

/**
 * output object definition
 */
struct _output_t {

	/* outputing object name */
	char			*name;			

	/* width of output, in pixel */
	unsigned int	 width;			

	/* height of output, in pixel */
	unsigned int	 height;		

	/* bits/pixel */
	unsigned int	 bpp;			

	/* colorspace of output */
	unsigned int	 colorspace;	

	/* decoded frame to display */
	unsigned char	*frame;			

	/* SDL object for output object */
	struct _sdl {

		/* flag whether init SDL or not */
		int			 initialized;		

		/* screen to display */
		SDL_Surface	*screen;	

/*
 * default value for scr_*
 */
#define	DFL_SCR_WID			800
#define	DFL_SCR_HGT			600
#define	DFL_SCR_BPP			32
#define	DFL_SCR_FLG			SDL_SWSURFACE

		/* screen width */
		int				scr_wid;	
		
		/* screen height */
		int				scr_hgt;	

		/* screen depth, bits/pixel */
		int				scr_bpp;	

		/* screen flag, for SDL */
		unsigned int	scr_flg;	

/*
 * default color mask and shift for SDL screen
 */
#if SDL_ORDER == SDL_BIG_ENDIAN

# define	DFL_SDL_AMASK		0x00000000
# define	DFL_SDL_RMASK		0x00ff0000
# define	DFL_SDL_GMASK		0x0000ff00
# define	DFL_SDL_BMASK		0x000000ff

# define	DFL_SDL_ASHIFT		24
# define	DFL_SDL_RSHIFT		16
# define	DFL_SDL_GSHIFT		8
# define	DFL_SDL_BSHIFT		0

#else

# define	DFL_SDL_AMASK		0x00000000
# define	DFL_SDL_RMASK		0x0000ff00
# define	DFL_SDL_GMASK		0x00ff0000
# define	DFL_SDL_BMASK		0xff000000

# define	DFL_SDL_ASHIFT		0
# define	DFL_SDL_RSHIFT		8
# define	DFL_SDL_GSHIFT		16
# define	DFL_SDL_BSHIFT		24

#endif
		unsigned int	Amask;
		unsigned int	Rmask;
		unsigned int	Gmask;
		unsigned int	Bmask;

		unsigned char	Ashift;
		unsigned char	Rshift;
		unsigned char	Gshift;
		unsigned char	Bshift;

		/* rgb frame from decoder */
		SDL_Surface	   *decoded_rgb_frm;
	} *sdl;	

	/* events for sync between dec and out */
	event_t			   *dec_to_out_evt;
	event_t			   *out_to_dec_evt;

	/* pingpong mgmt object */
	pingpong_buf_mgmt_t	*ppbm;

	/* output thread's id */
	pthread_t			pt_id;

	/* output thread's attributes */
	pthread_attr_t		pt_attr;

	/* output thread's exit code */
	PtExitCodeType		pt_exit_code;

	/**
	 * output object methods
	 */
	/* init method of output object */
	int		(*Init)(output_t			*out, 
					event_t				*pDecToOutEvt, 
					event_t				*pOutToDecEvt,
					pingpong_buf_mgmt_t	*pPpbm);

	/* mainloop of outputing */
	int		(*MainLoop)(output_t *out);	
	
	/* terminate method of outputing */
	int		(*Term)(output_t *out);	
};

output_t*	out_new(void);
void		out_del(output_t *out);

#ifdef	__cplusplus
}
#endif

#endif //_OUTPUT_H_

