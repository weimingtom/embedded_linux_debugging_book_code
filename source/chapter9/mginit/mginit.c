/*
** $Id: mginit.c,v 1.31 2003/09/04 07:58:39 weiym Exp $
**
** The mginit program of MiniGUI Demostration Envirionment.
**
** Copyright (C) 2001, 2002 Wei Yongming.
** Copyright (C) 2003 Feynman Software.
**
** Create date: 2001/09/05
*/

/*
**  This source is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This software is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
**  MA 02111-1307, USA
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

static void child_wait (int sig)
{
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            printf ("--pid=%d--status=%x--rc=%d---\n", pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

int MiniGUIMain (int args, const char* arg[])
{
    struct sigaction siga;
    int status;
    MSG msg;

    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));

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

    sigaction (SIGCHLD, &siga, NULL);

    while (GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

    MiniGUIExtCleanUp ();

    return 0;
}

