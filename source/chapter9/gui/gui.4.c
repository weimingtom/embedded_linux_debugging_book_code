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

HWND		hMainWnd;							// main window
RASTER_MAP 	* pMap = NULL;						// map

//
// shift button(left, right, up, down, center) notification
//
static void
ShiftBtnNotifyProc(HWND hWnd, int id, int nc, DWORD add_data)
{
	if (id == IDC_CENTER_BTN && nc == BN_CLICKED) {
		pMap->lx = (pMap->tw - pMap->dw) * 0.5f;
		pMap->ty = (pMap->th - pMap->dh) * 0.5f;
	}

	if (id == IDC_L_BTN && nc == BN_CLICKED) {
		if (pMap->lx - 10 > 0)
			pMap->lx -= 10;
		else
			pMap->lx = 0;
	}
	if (id == IDC_R_BTN && nc == BN_CLICKED) {
		if (pMap->lx + pMap->dw + 10 < pMap->tw)
			pMap->lx += 10;
		else
			pMap->lx = pMap->tw - pMap->dw;
	}
	if (id == IDC_U_BTN && nc == BN_CLICKED) {
		if (pMap->ty - 10 > 0)
			pMap->ty -= 10;
		else
			pMap->ty = 0;
	}
	if (id == IDC_D_BTN && nc == BN_CLICKED) {
		if (pMap->ty + pMap->dh + 10 < pMap->th)
			pMap->ty += 10;
		else
			pMap->ty = pMap->th - pMap->dh;
	}

	SendMessage(hMainWnd, MSG_PAINT, 0, 0);
}

//
// quit button notification
//
static void
QuitBtnNotifyProc(HWND hWnd, int id, int nc, DWORD add_data)
{
	if (id == IDC_QUIT_BTN && nc == BN_CLICKED) {
		PostMessage(hMainWnd, MSG_CLOSE, 0, 0);
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
	HWND	hMapPanel;

	hPanelStcWnd = CreateWindow(CTRL_STATIC, "panel", 
			SS_GROUPBOX | WS_CHILD | WS_VISIBLE,
			IDC_PANEL_STC, 10, 10, 180, 430, hMainWnd, 0);

	hLonStcWnd = CreateWindow(CTRL_STATIC, "longitude",
			SS_LEFT | WS_CHILD | WS_VISIBLE,
			IDC_LON_STC, 20, 40, 50, 30, hMainWnd, 0);

	hLatStcWnd = CreateWindow(CTRL_STATIC, "latitude",
			SS_LEFT | WS_CHILD | WS_VISIBLE,
			IDC_LAT_STC, 20, 80, 50, 30, hMainWnd, 0);

	hLonEdtWnd = CreateWindow(CTRL_EDIT, "",
			ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE,
			IDC_LON_EDT, 75, 40, 90, 30, hMainWnd, 0);

	hLatEdtWnd = CreateWindow(CTRL_EDIT, "",
			ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE,
			IDC_LAT_EDT, 75, 80, 90, 30, hMainWnd, 0);

	hUBtnWnd = CreateWindow(CTRL_BUTTON, "U",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_U_BTN, 75, 130, 30, 30, hMainWnd, 0);
	
	hLBtnWnd = CreateWindow(CTRL_BUTTON, "L",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_L_BTN, 40, 165, 30, 30, hMainWnd, 0);

	hCenterBtnWnd = CreateWindow(CTRL_BUTTON, "C",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_CENTER_BTN, 75, 165, 30, 30, hMainWnd, 0);

	hRBtnWnd = CreateWindow(CTRL_BUTTON, "R",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_R_BTN, 110, 165, 30, 30, hMainWnd, 0);

	hDBtnWnd = CreateWindow(CTRL_BUTTON, "D",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_D_BTN, 75, 200, 30, 30, hMainWnd, 0);

	hZoomOutBtnWnd = CreateWindow(CTRL_BUTTON, "Zoom Out",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_DISABLED,
			IDC_ZOOMOUT_BTN, 20, 250, 60, 30, hMainWnd, 0);

	hZoomInBtnWnd = CreateWindow(CTRL_BUTTON, "Zoom In",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_DISABLED,
			IDC_ZOOMIN_BTN, 100, 250, 60, 30, hMainWnd, 0);

	hQuitBtnWnd = CreateWindow(CTRL_BUTTON, "Quit",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			IDC_QUIT_BTN, 120, 380, 40, 30, hMainWnd, 0);

	hMapPanel = CreateWindow(CTRL_STATIC, "map",
			SS_GROUPBOX | WS_CHILD | WS_VISIBLE,
			IDC_MAPPANEL_STC, 200, 10, 430, 430, hMainWnd, 0);

	// set notification callback for essential buttons
	SetNotificationCallback(hLBtnWnd, ShiftBtnNotifyProc);
	SetNotificationCallback(hRBtnWnd, ShiftBtnNotifyProc);
	SetNotificationCallback(hUBtnWnd, ShiftBtnNotifyProc);
	SetNotificationCallback(hDBtnWnd, ShiftBtnNotifyProc);
	SetNotificationCallback(hCenterBtnWnd, ShiftBtnNotifyProc);
	SetNotificationCallback(hQuitBtnWnd, QuitBtnNotifyProc);

	return (0);
}

//
// load raster map
//
static int
LoadMap(HDC hdc, RASTER_MAP * map)
{
	return LoadBitmap(hdc, &map->map, map->bmpname);
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
		case MSG_PAINT:
		{
			HDC hdc = BeginPaint(hWnd);

			// map not loaded, load first
			if (pMap == NULL) {
				pMap = (RASTER_MAP *)malloc(sizeof(RASTER_MAP));
				pMap->bmpname = (char *)malloc(MAP_FILE_LEN * sizeof(char));
				strncpy(pMap->bmpname, "res/chengdu_map.bmp", MAP_FILE_LEN);
				pMap->lx = 0;
				pMap->ty = 0;
				pMap->dw = 400;
				pMap->dh = 400;
				LoadMap(hdc, pMap);
				pMap->tw = pMap->map.bmWidth;
				pMap->th = pMap->map.bmHeight;
			}
			FillBoxWithBitmapPart(hdc, 215, 30, pMap->dw, pMap->dh, 
					0, 0, &pMap->map, pMap->lx, pMap->ty);
			
			EndPaint(hWnd, hdc);
			break;
		}
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

