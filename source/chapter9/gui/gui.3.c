/*
 * $file	:
 * $desc	:
 * $author	:	rockins(ybc2084@163.com)
 * $date	:
 * $copyright	:	all copyrights(c) reserved by rockins.
 */

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "gui.h"

HWND	hMainWnd;

static void
ControlNotifyProc(HWND hWnd, int id, int nc, DWORD add_data)
{
	if (id == IDC_QUIT_BTN && nc == BN_CLICKED) {
		PostMessage(hMainWnd, MSG_CLOSE, 0, 0);
//		DestroyMainWindow(hMainWnd);
//		PostQuitMessage(hMainWnd);			// TODO: who is hMainWnd?
	}
}

//
// create controls in main window
//
static int
CreateControls(HWND hMainWnd)
{
	HWND	hPanelStcWnd;
	HWND	hLonStcWnd;
	HWND	hLatStcWnd;
	HWND	hLonEdtWnd;
	HWND	hLatEdtWnd;
	HWND	hLBtnWnd;
	HWND	hRBtnWnd;
	HWND	hUBtnWnd;
	HWND	hDBtnWnd;
	HWND	hZoomOutBtnWnd;
	HWND	hZoomInBtnWnd;
	HWND	hCenterBtnWnd;
	HWND	hQuitBtnWnd;

	hPanelStcWnd = CreateWindow(CTRL_STATIC, "panel", 
			SS_GROUPBOX | WS_CHILD | WS_VISIBLE,
			IDC_PANEL_STC, 10, 10, 180, 430, hMainWnd, 0);

	hLonStcWnd = CreateWindow(CTRL_STATIC, "longitude",
			SS_LEFT | WS_CHILD | WS_VISIBLE,
			IDC_LON_STC, 20, 20, 50, 30, hMainWnd, 0);

	hLatStcWnd = CreateWindow(CTRL_STATIC, "latitude",
			SS_LEFT | WS_CHILD | WS_VISIBLE,
			IDC_LAT_STC, 20, 60, 50, 30, hMainWnd, 0);

	hLonEdtWnd = CreateWindow(CTRL_EDIT, "",
			ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE,
			IDC_LON_EDT, 75, 20, 90, 30, hMainWnd, 0);

	hLatEdtWnd = CreateWindow(CTRL_EDIT, "",
			ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE,
			IDC_LAT_EDT, 75, 60, 90, 30, hMainWnd, 0);

	hUBtnWnd = CreateWindow(CTRL_BUTTON, "U",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_U_BTN, 75, 110, 30, 30, hMainWnd, 0);
	
	hLBtnWnd = CreateWindow(CTRL_BUTTON, "L",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_L_BTN, 40, 145, 30, 30, hMainWnd, 0);

	hCenterBtnWnd = CreateWindow(CTRL_BUTTON, "C",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_CENTER_BTN, 75, 145, 30, 30, hMainWnd, 0);

	hRBtnWnd = CreateWindow(CTRL_BUTTON, "R",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_R_BTN, 110, 145, 30, 30, hMainWnd, 0);

	hDBtnWnd = CreateWindow(CTRL_BUTTON, "D",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_D_BTN, 75, 180, 30, 30, hMainWnd, 0);

	hZoomOutBtnWnd = CreateWindow(CTRL_BUTTON, "Zoom Out",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_DISABLED,
			IDC_ZOOMOUT_BTN, 20, 230, 60, 30, hMainWnd, 0);

	hZoomInBtnWnd = CreateWindow(CTRL_BUTTON, "Zoom In",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_DISABLED,
			IDC_ZOOMIN_BTN, 100, 230, 60, 30, hMainWnd, 0);

	hQuitBtnWnd = CreateWindow(CTRL_BUTTON, "Quit",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_QUIT_BTN, 120, 380, 40, 30, hMainWnd, 0);

#if 0
	int i;
	for (i = IDC_L_BTN; i <= IDC_QUIT_BTN; i++) {
		SetNotificationCallback(GetDlgItem(hMainWnd, i), ControlNotifyProc);
	}
#endif
	SetNotificationCallback(hQuitBtnWnd, ControlNotifyProc);

	return (0);
}

static int
MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case MSG_CREATE:
			CreateControls(hWnd);
			return (0);
#if 0
		case MSG_LBUTTONDOWN:
		{
			int x, y;
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			printf("LButton down:x=%d, y=%d\n", x, y);
			break;
		}
		case MSG_LBUTTONUP:
			break;
#endif
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			break;
		default:
			break;
	}

	return (DefaultMainWinProc(hWnd, message, wParam, lParam));
}

int
MiniGUIMain(int argc, const char *argv[])
{
	MSG		Msg;
	MAINWINCREATE	CreateInfo;

#if defined(_LITE_VERSION) && !(_STAND_ALONE) && 0
    int i;
    const char* layer = NULL;
    RECT max_rect = {0, 0, 0, 0};

    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i + 1];
            break;
        }
    }

    GetLayerInfo (layer, &max_rect, NULL, NULL, NULL);

    if (JoinLayer (layer, argv[0], max_rect.left, max_rect.top,
                            max_rect.left + 640,
                            max_rect.top + 480) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

#ifdef	_LITE_VERSION
	SetDesktopRect(0, 0, 640, 480);
#endif

	CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
	CreateInfo.dwExStyle = WS_EX_NONE;
	CreateInfo.spCaption = "TinyGIS";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = MainWinProc;
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = 640;
	CreateInfo.by = 480;
	CreateInfo.iBkColor = COLOR_lightgray;
	CreateInfo.dwAddData = 0;
	CreateInfo.hHosting = HWND_DESKTOP;

	hMainWnd = CreateMainWindow(&CreateInfo);
	if (hMainWnd == HWND_INVALID)
		return (-1);

	ShowWindow(hMainWnd, SW_SHOWNORMAL);


	while (GetMessage(&Msg, hMainWnd)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MainWindowThreadCleanup(hMainWnd);

	return (0);
}

