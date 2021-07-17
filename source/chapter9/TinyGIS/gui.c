/*
 * $file	:	gui.c
 * $desc	:	implement user interface
 * $author	:	rockins(ybc2084@163.com)
 * $date	:	Thu Apr  5 02:12:47 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 */

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "gps.h"
#include "gui.h"

HWND		hMainWnd;							// main window
RASTER_MAP 	* pMap = NULL;						// global unique map

//
// shift button(left, right, up, down, center) notification
//
static void
ShiftBtnNotifyProc(HWND hWnd, int id, int nc, DWORD add_data)
{
	// center
	if (id == IDC_CENTER_BTN && nc == BN_CLICKED) {
		pMap->lx = (pMap->tw - pMap->dw) * 0.5f;
		pMap->ty = (pMap->th - pMap->dh) * 0.5f;
	}

	// left
	if (id == IDC_L_BTN && nc == BN_CLICKED) {
		if ((signed int)pMap->lx - 10 > 0)
			pMap->lx -= 10;
		else
			pMap->lx = 0;
	}

	// right
	if (id == IDC_R_BTN && nc == BN_CLICKED) {
		if (pMap->lx + pMap->dw + 10 < pMap->tw)
			pMap->lx += 10;
		else
			pMap->lx = pMap->tw - pMap->dw;
	}

	// up
	if (id == IDC_U_BTN && nc == BN_CLICKED) {
		if ((signed int)pMap->ty - 10 > 0)
			pMap->ty -= 10;
		else
			pMap->ty = 0;
	}

	// down
	if (id == IDC_D_BTN && nc == BN_CLICKED) {
		if (pMap->ty + pMap->dh + 10 < pMap->th)
			pMap->ty += 10;
		else
			pMap->ty = pMap->th - pMap->dh;
	}

	// re-paint
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

//
// unload raster map
//
static void
Unloadmap(RASTER_MAP * map)
{
	UnloadBitmap(&map->map);
}

//
// install timer
//
static BOOL
InstallTimer(HWND hWnd)
{
#ifdef	_TIMER_UNIT_10MS
	return SetTimer(hWnd, IDC_GPS_TIMER, GPS_SAMPLE_TIME_UNIT);
#else
	return SetTimer(hWnd, IDC_GPS_TIMER, 1000/GPS_SAMPLE_TIME_UNIT);
#endif
}

//
// uninstall timer
//
static BOOL
UninstallTimer(HWND hWnd)
{
	return KillTimer(hWnd, IDC_GPS_TIMER);
}

//
// update vehicle's location
// try best to put vehicle in center of view field
//
static int
UpdateVehicle(double x, double y)
{
	pMap->vx = pMap->tw * x;
	pMap->vy = pMap->th * y;

	if ((pMap->vx - pMap->dw / 2 > 0))
		pMap->lx = pMap->vx - pMap->dw / 2;
	else
		pMap->lx = 0;

	if ((pMap->vy - pMap->dh / 2 > 0))
		pMap->ty = pMap->vy - pMap->dh / 2;
	else
		pMap->ty = 0;

	return (0);
}


//
// main window event handle process
// note: the main window is fixed, cannot changing size of position,
// this is done by capturing MSG_SIZECHANGING message
//
static int
MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case MSG_CREATE:
		{
			CreateControls(hWnd);
			GPS_StartSampleProcess();
			InstallTimer(hWnd);
			return (0);
		}
		case MSG_CLOSE:
		{
			UninstallTimer(hWnd);
			GPS_StopSampleProcess();
			DestroyMainWindow(hWnd);
			DestroyAllControls(hWnd);
			PostQuitMessage(hWnd);
			break;
		}
		case MSG_SIZECHANGING:
		{
			const RECT * rcExpect = (RECT *)wParam;
			RECT * rcResult = (RECT *)lParam;

			rcResult->left = 0;
			rcResult->top = 0;
			rcResult->right = 640;
			rcResult->bottom = 480;
			return (0);
		}
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
				pMap->vx = pMap->tw / 2;
				pMap->vy = pMap->th / 2;
			}
			FillBoxWithBitmapPart(hdc, 215, 30, pMap->dw, pMap->dh, 
					0, 0, &pMap->map, pMap->lx, pMap->ty);				// draw map
			SetBrushColor(hdc, PIXEL_red);								// set brush
			FillCircle(hdc, 215 + (pMap->vx - pMap->lx),
						30 + (pMap->vy - pMap->ty), 10);				// draw vehicle
			
			EndPaint(hWnd, hdc);
			break;
		}
		case MSG_TIMER:
		{
			double 	x = 0.5f, y = 0.5f;
			double 	lon = 0.0f, lat = 0.0f;
			char	lon_buff[64];
			char	lat_buff[64];

			if ((int)wParam == IDC_GPS_TIMER) {
				GPS_GetCurrentLonLat(&lon, &lat);
				GPS_GetCurrentXY(&x, &y);
#if defined(_GUI_DEBUG_)
				printf("MSG_TIMER: lon=%f, lat=%f, x=%f, y=%f\n", 
						lon, lat, x, y);
#endif
				memset(lon_buff, 0, 64);
				memset(lat_buff, 0, 64);
				snprintf(lon_buff, 64, "%.4f", lon);
				snprintf(lat_buff, 64, "%.4f", lat);
				strcat(lon_buff, "'");
				strcat(lat_buff, "'");

				SetDlgItemText(hWnd, IDC_LON_EDT, lon_buff);
				SetDlgItemText(hWnd, IDC_LAT_EDT, lat_buff);

				// update vehicle's position and re-painting
				UpdateVehicle(x, y);
				SendMessage(hWnd, MSG_PAINT, 0, 0);
			}
			break;
		}
		default:
			break;
	}

	return (DefaultMainWinProc(hWnd, message, wParam, lParam));
}

int
MiniGUIMain(int argc, const char *argv[])
{
	MSG				Msg;
	MAINWINCREATE	CreateInfo;

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

