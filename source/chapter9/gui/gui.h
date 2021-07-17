/*
 * $file	:
 * $desc	:
 * $author	:	rockins(ybc2084@163.com)
 * $date	:
 * $copyright	:	all copyrights(c) reserved by rockins.
 */

#ifndef	_GUI_H_
#define	_GUI_H_

#define	IDC_L_BTN		100						// left shift button
#define	IDC_R_BTN		101						// right shift button
#define	IDC_U_BTN		102						// up shift button
#define	IDC_D_BTN		103						// down shift button

#define	IDC_ZOOMOUT_BTN	104						// zoom out(fang da) button
#define	IDC_ZOOMIN_BTN	105						// zoom in(suo xiao) button
#define	IDC_CENTER_BTN	106						// concentrate to center button
#define	IDC_QUIT_BTN	107						// quit button

#define	IDC_PANEL_STC	200						// panel for other widgets
#define	IDC_MAPPANEL_STC	201					// panel for map
#define	IDC_LON_STC		202						// longitude static text box
#define	IDC_LAT_STC		203						// latitude static text box

#define	IDC_LON_EDT		300						// longitude edit
#define	IDC_LAT_EDT		301						// latitude edit

//#define	IDC_MAP_BMP		400						// raster map bitmap

typedef struct _RASTER_MAP
{
#define	MAP_FILE_LEN	100						// map file path len
	char	*bmpname;							// .bmp file name
	BITMAP	map;								// map object
	unsigned int tw, th;						// total width and height
	unsigned int lx, ty;						// left x and top y
	unsigned int dw, dh;						// display width and height
}RASTER_MAP;

#endif
