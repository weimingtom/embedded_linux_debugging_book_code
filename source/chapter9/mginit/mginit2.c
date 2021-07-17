/*
 * $file	:	mginit.c
 * $desc	:	simplest MiniGUI server
 * $author	:	rockins(ybc2084@163.com)
 * $date	:	Sun Apr 15 17:42:58 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

int
MiniGUIMain (int args, const char* arg[])
{
    MSG msg;

    OnNewDelClient = NULL;
    OnChangeLayer = NULL;

#if _MINIGUI_VERSION_CODE < _VERSION_CODE (1, 3, 0)
    if (!ServerStartup ()) {
        fprintf (stderr, "Can not start the server of MiniGUI-Lite: mginit.\n");
        return 1;
    }
#endif

    if (SetDesktopRect (650, 490, 660, 500) == 0) {
        fprintf (stderr, "Empty desktop rect.\n");
        return 1;
    }

    if (!InitMiniGUIExt ()) {
        fprintf (stderr, "Can not init mgext library.\n");
        return 1;
    }

    while (GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

    MiniGUIExtCleanUp ();

    return 0;
}

