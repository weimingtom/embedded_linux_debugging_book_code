/*
 * $file	:	gps.h
 * $desc	:	gps definition
 * $author	:	rockins
 * $date	:	Sun Feb 11 10:29:38 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 */

#ifndef	_GPS_H_
#define	_GPS_H_


#ifndef	_GPS_DEBUG_
//#define	_GPS_DEBUG_						// for debugging purpose
#endif


#include <termios.h>						// for B9600
#include <pthread.h>						// for thread supporting

#define	GPS_PORT			"/dev/ttyS1"	// gps data input device
#define	GPS_BAUD			B4800			// gps baud rate
#define	GPS_LINE			1024			// line max characters
#define	GPS_SAMPLE_INTERVAL	2				// sampling interval: 2 seconds

#define	GPGGA_HEAD			"$GPGGA"		// head of GGA
#define	GPGGA_HEAD_LEN		6				// length of "$GPGGA"

#define	GPGGA_LATITUDE_DEGREE_OFF	14		// offset of latitude degree
#define	GPGGA_LATITUDE_DEGREE_LEN	2		// length of latitude degree
#define	GPGGA_LATITUDE_MINUTE_OFF	16		// offset of latitude minute
#define	GPGGA_LATITUDE_MINUTE_LEN	7		// length of latitude minute

#define	GPGGA_LONGITUDE_DEGREE_OFF	26		// offset of longitude degree
#define	GPGGA_LONGITUDE_DEGREE_LEN	3		// length of longitude degree
#define	GPGGA_LONGITUDE_MINUTE_OFF	29		// offset of longitude minute
#define	GPGGA_LONGITUDE_MINUTE_LEN	7		// length of longitude minute

#define	CHENGDU_GPS_DATA_FILE	"res/chengdu_gps.dat"	// gps ref point data

// node coordinates in map
typedef struct tagMAPNODE {
	double longitude;					// in minute
	double latitude;					// in minute
	double x;							// normalised cartesian x
	double y;							// normalised cartesian y
}MAPNODE;

extern MAPNODE TopLeft;					// topleft point
extern MAPNODE BottomRight;				// bottomright point
extern MAPNODE CurrentLocation;			// current location
extern pthread_mutex_t GPSMutex;		// mutex for accessing CurrentLocation
extern int isSampling;					// flag indicating sampling or not

// supporting function, or may called internal function
int GPS_InitSerialPort(char *gps_port);
int GPS_ReadBenchmarkPointGPSData(MAPNODE *topleft, 
								MAPNODE *bottomright);
int GPS_ReadGPSData(int gpsfd, char *gps_data);
int GPS_ParseGPSData(char *gps_data, double *longitude,
									double *latitude);
int GPS_CalcXY(	double longitude, double latitude,
				double *x, double *y);
void *GPS_SampleThread(void *args);

// okay, following two function are everything we need
int GPS_StartSampleProcess(void);
int GPS_StopSampleProcess(void);
int GPS_GetCurrentLonLat(double *lon, double *lat);
int GPS_GetCurrentXY(double *x, double *y);

#endif	//_GPS_H_
