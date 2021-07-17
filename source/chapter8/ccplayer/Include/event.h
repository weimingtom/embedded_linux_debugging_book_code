/********************************************************
 *	event.h	(C) 2008 Rockins Chen
 *
 * wrapper for pthread mutex, used for synchronous
 * between decode thread and output thread.
 *
 * bug:Rockins Chen<ybc2084@163.com>
 *
 * All Copyrights reserved.
 *
 * $Id: event.h 9 2008-02-17 08:09:01Z rockins $
 *
 ********************************************************/

#ifndef	_EVENT_H_
#define _EVENT_H_

#include <pthread.h>
#include "common.h"

#ifdef	__cplusplus
extern "C" {
#endif

#if !defined(TRUE) || !defined(FALSE)
#	define	TRUE	1
#	define	FALSE	0
#endif


typedef struct _event_t event_t;

/**
 * event object definition
 */
struct _event_t {

	/** event type */
	EvtType			type;

	/** pthread condition variable denotes an event */
	pthread_cond_t	cond;

	/** event state - happened(TRUE) or not(FALSE) */
	int				state;

	/** pthread mutex to protect cond */
	pthread_mutex_t	mutex;

	/** 
	 * extra info structure: to make output thread know 
	 * decoded frame's info.
	 * FIXME: this mechanism should be replaced by global
	 * parameter setting in future, since this approach
	 * lack of scalability and flexiability.
	 */
	struct _extra_info {
		unsigned int		frm_width;
		unsigned int		frm_height;
		unsigned int		frm_colorspace;
	} *extra;

	/** initial an event */
	int			(*init)(event_t *evt);

	/** waiting for an event */
	int			(*wait)(event_t *evt);

	/** signal an event happened */
	int			(*sgnl)(event_t *evt);

	/** set/get event type */
	int			(*set_type)(event_t *evt, EvtType et);
	EvtType 	(*get_type)(event_t *evt);

	/** lock & unlock event */
	int 		(*lock)(event_t *evt);
	int			(*unlock)(event_t *evt);
};

extern event_t*	evt_new(void);
extern void		evt_del(event_t *evt);

#ifdef	__cplusplus
}
#endif

#endif	//_EVENT_H_

