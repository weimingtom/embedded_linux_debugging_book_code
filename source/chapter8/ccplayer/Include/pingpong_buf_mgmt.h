/**************************************************************************
 * 	manage ping pong double buffer (C) 2008 Rockins Chen
 *
 * 	$Id: pingpong_buf_mgmt.h 9 2008-02-17 08:09:01Z rockins $
 *
 *************************************************************************/
#ifndef	_PINGPONG_BUF_MGMT_H_
#define	_PINGPONG_BUF_MGMT_H_

#include <stdlib.h>
#include <string.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef	struct _pingpong_buf_mgmt pingpong_buf_mgmt_t;

/**
 * ping pong buffer management object definition
 */
struct _pingpong_buf_mgmt {

	/* flag whether initialized or not */
	int					 initialized;

	/* ping & pong buffer */
	void				*ping;
	void				*pong;

	/* current available buffer: ping or pong */
	void				*curr;

	/* ping/pong buffer's size */
	int					 pingpong_buf_sz;

	/* ping/pong buffer's inuse size */
	int					 pingpong_buf_inuse;

	/* lock for accessing pingpong buffer */
	pthread_mutex_t		 lock;
	pthread_mutexattr_t	 lock_attr;

	/* init ping/pong management object */
	int		(*init)(pingpong_buf_mgmt_t *ppbm, 
					void 				*pPing, 
					void 				*pPong, 
					int 				 buf_sz);

	/* get ping/pong buf, after this function, ping/pong will be swapped */
	void*	(*get_pp_buf)(pingpong_buf_mgmt_t *ppbm);

	/* set ping/pong buf's inuse size(Note: not buf's maximum size */
	int		(*set_pp_buf_inuse_sz)(pingpong_buf_mgmt_t *ppbm, 
								   int 					buf_inuse_sz);

	/* reset curr */
	int		(*reset_curr)(pingpong_buf_mgmt_t *ppbm);

	/* terminate ping pong management routine */
	int		(*term)(pingpong_buf_mgmt_t *ppbm);
};

extern pingpong_buf_mgmt_t*	ppbm_new(void);
extern void 				ppbm_del(pingpong_buf_mgmt_t *ppbm);

#ifdef	__cplusplus
}
#endif

#endif	//_PINGPONG_BUF_MGMT_H_

