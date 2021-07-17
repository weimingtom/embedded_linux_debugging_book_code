/********************************************************
 *	event.h	(C) 2008 Rockins Chen
 *
 * Wrapper for pthread synchronous facility, 
 * used for synchronizing between decoding 
 * thread and outputing thread.
 *
 * Bug report:Rockins Chen<ybc2084@163.com>
 *
 * All Copyrights reserved.
 *
 * $Id: event.c 11 2008-03-09 15:37:46Z rockins $
 *
 ********************************************************/
#include <stdlib.h>
#include <assert.h>
#include "common.h"
#include "event.h"

/**
 * init event object
 * @evt		event_t*(event object)
 */
static int evt_init(event_t *evt)
{
	evt->extra = malloc(sizeof(struct _extra_info));
	assert(evt->extra);

	return (0);
}

/**
 * wait event to be true
 * @evt		event_t*(event object)
 */
static int evt_wait(event_t *evt)
{
	pthread_mutex_lock(&evt->mutex);

	while (!evt->state)
		pthread_cond_wait(&evt->cond, &evt->mutex);
	evt->state = FALSE;

	pthread_mutex_unlock(&evt->mutex);

	return (0);
}

/**
 * signal an event happened
 * @evt		event_t*(event object)
 */
static int evt_sgnl(event_t *evt)
{
	pthread_mutex_lock(&evt->mutex);

	evt->state = TRUE;
	pthread_cond_signal(&evt->cond);

	pthread_mutex_unlock(&evt->mutex);

	return (0);
}

/**
 * set event type
 * @evt		event_t*(event object)
 * @et		event type
 */
static int evt_set_type(event_t *evt, EvtType et)
{
	pthread_mutex_lock(&evt->mutex);

	evt->type = et;

	pthread_mutex_unlock(&evt->mutex);

	return (0);
}

/**
 * get event type
 * @evt		event_t*(event object)
 * $ret		event type
 */
static EvtType evt_get_type(event_t *evt)
{
	EvtType		et;

	pthread_mutex_lock(&evt->mutex);

	et = evt->type;

	pthread_mutex_unlock(&evt->mutex);

	return (et);
}

/**
 * lock event object
 * @evt		event_t*(event object)
 */
int evt_lock(event_t *evt)
{
	pthread_mutex_lock(&evt->mutex);

	return (0);
}

/**
 * unlock event object
 * @evt		event_t*(event object)
 */
int evt_unlock(event_t *evt)
{
	pthread_mutex_unlock(&evt->mutex);

	return (0);
}

/**
 * constructor of event object
 * $ret		event_t*(event object)
 */
event_t* evt_new(void)
{
	event_t *event;

	event = malloc(sizeof(event_t));
	pthread_mutex_init(&event->mutex, NULL);
	pthread_cond_init(&event->cond, NULL);
	event->init 	= evt_init;
	event->wait 	= evt_wait;
	event->sgnl 	= evt_sgnl;
	event->set_type	= evt_set_type;
	event->get_type = evt_get_type;
	event->lock 	= evt_lock;
	event->unlock 	= evt_unlock;

	return (event);
}

/**
 * de-constructor of event object
 * @evt		event_t*(event object)
 */
void evt_del(event_t *evt)
{
	pthread_mutex_destroy(&evt->mutex);
	pthread_cond_destroy(&evt->cond);
	evt->init 		= NULL;
	evt->wait 		= NULL;
	evt->sgnl 		= NULL;
	evt->set_type	= NULL;
	evt->get_type	= NULL;
	evt->lock 		= NULL;
	evt->unlock 	= NULL;
	free(evt);
}

