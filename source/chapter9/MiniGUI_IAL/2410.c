/*
** $Id: 2410.c,v 1.4 2003/11/21 12:15:37 weiym Exp $
**
** 2410.c: Low Level Input Engine for SMDK2410 Dev Board.
** 
** Copyright (C) 2003 Feynman Software.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "tslib.h"

#ifdef _SMDK2410_IAL

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "2410.h"

//#ifndef	_DEBUG
//#define	_DEBUG				// for debugging
//#endif

#ifdef	_DEBUG
#undef	_DEBUG					// for release
#endif

/* for storing data reading from /dev/input/event1 */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;

static unsigned char state [NR_KEYS];
static int mousex = 0;
static int mousey = 0;
static TS_EVENT ts_event;
static struct tsdev *ts;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    if (mousex < 0) mousex = 0;
    if (mousey < 0) mousey = 0;
    if (mousex > 639) mousex = 639;
    if (mousey > 479) mousey = 479;

#ifdef _DEBUG
    printf ("mousex = %d, mousey = %d\n", mousex, mousey);
#endif

    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return ts_event.pressure;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
	struct ts_sample sample;
	int	ret = 0;
	int fd;
    fd_set rfds;
    int e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

	fd = ts_fd(ts);

    if ((which & IAL_MOUSEEVENT) && fd >= 0) {
        FD_SET (fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }
#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 

		// input events is coming
        if (fd > 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            ts_event.x=0;
            ts_event.y=0;
			
			ret = ts_read(ts, &sample, 1);
			if (ret < 0) {
				perror("ts_read()");
				exit(-1);
			}

			ts_event.x = sample.x;
			ts_event.y = sample.y;
			ts_event.pressure = (sample.pressure > 0 ? 4:0);

			if (ts_event.pressure > 0 &&
				(ts_event.x >= 0 && ts_event.x <= 639) &&
				(ts_event.y >= 0 && ts_event.y <= 479)) {
				mousex = ts_event.x;
				mousey = ts_event.y;
			}
	
#ifdef _DEBUG
            if (ts_event.pressure > 0) {
                printf ("mouse down: ts_event.x = %d, ts_event.y = %d, ts_event.pressure = %d\n", 
						ts_event.x, ts_event.y, ts_event.pressure);
            }
#endif
			ret |= IAL_MOUSEEVENT;

			return (ret);
        }

    } 
    else if (e < 0) {
        return -1;
    }

	return (ret);
}

BOOL Init2410Input (INPUT* input, const char* mdev, const char* mtype)
{
	char *ts_device = NULL;

	if ((ts_device = getenv("TSLIB_TSDEVICE")) != NULL) {
		
		// open touch screen event device in blocking mode
		ts = ts_open(ts_device, 0);
	} else {
#ifdef USE_INPUT_API
		ts = ts_open("/dev/input/event0", 0);
#else
		ts = ts_open("/dev/touchscreen/ucb1x00", 0);
#endif
	}

	if (!ts) {
		perror("ts_open()");
		exit(-1);
	}

	if (ts_config(ts)) {
		perror("ts_config()");
		exit(-1);
	}

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    ts_event.x = ts_event.y = ts_event.pressure = 0;
    
    return TRUE;
}

void Term2410Input(void) 
{
    if (ts)
        ts_close(ts);
}

#endif /* _SMDK2410_IAL */

