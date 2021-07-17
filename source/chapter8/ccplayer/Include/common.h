/*
 * common definitions (C) 2008 Rockins Chen
 *
 * $Id$
 */

#ifndef	_COMMON_H_
#define	_COMMON_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * maximum video frame dimension and depth
 */
#define	MAX_VID_WID				720
#define	MAX_VID_HGT				576
#define	MAX_VID_BPP				4

/**
 * print error message. 
 * NOTE: @message must be a NULL-terminated string
 */
#define	ERROR(message)	\
		fprintf(stderr, "%s:%d:%s\n", __func__, __LINE__, message)

/*
 * value for pt_exit_code
 */
typedef enum PtExitCodeType_e {
	PT_EXIT_NORMAL					= 0,
	PT_EXIT_ABNORMAL				= -1,
} PtExitCodeType;

/*
 * value for event_t.type
 */
typedef enum EvtType_e {
	EVT_TYPE_NONE					= 0,
	EVT_TYPE_NEW_FRAME_READY		= 1,
	EVT_TYPE_NO_FRAME_AVAIL			= 2,
	EVT_TYPE_PREV_FRAME_OUTPUTED	= 4,
	EVT_TYPE_OUTPUT_ABORT			= 8,
} EvtType;

#ifdef	__cplusplus
}
#endif

#endif //_COMMON_H_
