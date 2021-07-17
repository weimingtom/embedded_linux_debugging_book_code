/****************************************************************************
 * pingpong_buf_mgmt.c	(C)2008 Rockins Chen
 *
 * management ping-pong associate buffer 
 *
 * Author: Rockins Chen<ybc2084@163.com>
 *
 * $Id: pingpong_buf_mgmt.c 9 2008-02-17 08:09:01Z rockins $
 *
 * *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "common.h"
#include "pingpong_buf_mgmt.h"

/** 
 * init ping/pong management object 
 */
static int ppbm_init(pingpong_buf_mgmt_t	*ppbm,
					 void 					*pPing, 
					 void 					*pPong, 
					 int 					 buf_sz)
{
	/*
	 * init mutex lock
	 */
	pthread_mutexattr_init(&ppbm->lock_attr);
	pthread_mutex_init(&ppbm->lock, &ppbm->lock_attr);

	/*
	 * setup ping/pong buffer
	 */
	pthread_mutex_lock(&ppbm->lock);

	ppbm->ping 					= pPing;
	ppbm->pong 					= pPong;

	ppbm->curr 					= ppbm->ping;
	ppbm->pingpong_buf_sz 		= buf_sz;
	ppbm->pingpong_buf_inuse 	= 0;

	ppbm->initialized 			= 1;

	pthread_mutex_unlock(&ppbm->lock);

	return (0);
}


/**
 * get ping/pong buf, after this function, 
 * ping/pong will be swapped 
 */
static void* ppbm_get_pp_buf(pingpong_buf_mgmt_t *ppbm)
{
	void		*curr_of_pp;
	
	pthread_mutex_lock(&ppbm->lock);

	curr_of_pp = ppbm->curr;
	if (ppbm->curr == ppbm->ping)
		ppbm->curr = ppbm->pong;
	else if (ppbm->curr == ppbm->pong)
		ppbm->curr = ppbm->ping;
	else {
		/* there must be some error */
		ERROR("error in swaping pingpong");
	}

	pthread_mutex_unlock(&ppbm->lock);

	return (curr_of_pp);
}

/** 
 * set ping/pong buf's inuse size.
 * Note: not buf's maximum size 
 */
static int ppbm_set_pp_buf_inuse_sz(pingpong_buf_mgmt_t *ppbm, 
									int 				 buf_inuse_sz)
{
	pthread_mutex_lock(&ppbm->lock);

	ppbm->pingpong_buf_inuse = buf_inuse_sz;
	
	pthread_mutex_unlock(&ppbm->lock);

	return (0);
}

/**
 * reset curr 
 */
static int ppbm_reset_curr(pingpong_buf_mgmt_t *ppbm)
{
	pthread_mutex_lock(&ppbm->lock);

	ppbm->curr = NULL;

	pthread_mutex_unlock(&ppbm->lock);

	return (0);
}

/**
 * terminate ping pong management routine 
 */
static int ppbm_term(pingpong_buf_mgmt_t *ppbm)
{
	pthread_mutex_destroy(&ppbm->lock);
	pthread_mutexattr_destroy(&ppbm->lock_attr);

	return (0);
}


/**
 * constructor for pingpong
 * $ret		pingpong_buf_mgmt_t*(pingpong object)
 */
pingpong_buf_mgmt_t* ppbm_new(void)
{
	pingpong_buf_mgmt_t		*ppbm;

	ppbm = malloc(sizeof(pingpong_buf_mgmt_t));
	assert(ppbm);

	memset(ppbm, 0, sizeof(pingpong_buf_mgmt_t));

	ppbm->init 					= ppbm_init;
	ppbm->get_pp_buf 			= ppbm_get_pp_buf;
	ppbm->set_pp_buf_inuse_sz	= ppbm_set_pp_buf_inuse_sz;
	ppbm->reset_curr 			= ppbm_reset_curr;
	ppbm->term 					= ppbm_term;

	return (ppbm);
}

/**
 * de-constructor for pingpong object
 * @ppbm	pingpong object
 */
void ppbm_del(pingpong_buf_mgmt_t *ppbm)
{
	if (ppbm) {
		if (ppbm->term)
			ppbm->term(ppbm);

		ppbm->init 					= NULL;
		ppbm->get_pp_buf 			= NULL;
		ppbm->set_pp_buf_inuse_sz	= NULL;
		ppbm->reset_curr			= NULL;
		ppbm->term 					= NULL;

		free(ppbm);
	}
}

