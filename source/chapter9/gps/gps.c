/*
 * $file	:
 * $desc	:
 * $author	:	rockins
 * $date	:
 * $copyright	:	all copyrights(c) reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "gps.h"


// for double float on PXA255, I found that its
// first 4 bytes and second 4 bytes should swaped
// to act as it do in X86 PC platform, so I define
// this macro, when work on X86 PC, this macro
// is defined as empty
#if defined(PXA_XSCALE)
	#define	SWAP_DOUBLE(x)						\
				do {						 	\
					unsigned int *p, t; 		\
					p = (unsigned int *)&x;		\
					t = p[0];					\
					p[0] = p[1];				\
					p[1] = t;					\
				}while(0)
#elif defined(X86_PC)
	#define	SWAP_DOUBLE(x)
#endif

MAPNODE TopLeft;					// topleft benchmark
MAPNODE BottomRight;				// bottomright benchmark
MAPNODE CurrentLocation;			// current location
pthread_mutex_t GPSMutex = PTHREAD_MUTEX_INITIALIZER;	// mutex for access CurrentLocation
int isSampling = 0;					// indicating sampling or not, 0 means not, 1 mean yes

//
// init gps serial port
// success: serial port fd
// fail: -1
//
int
GPS_InitSerialPort(char *gps_port)
{
    int fd = -1;
	int err;
	struct termios termopt;

    fd = open(gps_port, O_RDONLY);
    assert(fd > 0);

	err = tcgetattr(fd, &termopt);
	assert(err == 0);

	err = cfsetispeed(&termopt, GPS_BAUD);
	assert(err == 0);

	err = cfsetospeed(&termopt, GPS_BAUD);
	assert(err == 0);

#if	defined(__USE_BSD)
	cfmakeraw(&termopt);
#endif

	return (fd);
}

//
// read GPS data from serial port,
// gps data stored in gps_data, you
// must ensure gps_data has enough 
// space. return 0 if success, else
// return -1
//
int
GPS_ReadGPSData(int gpsfd, char *gps_data)
{
	char gpsline[GPS_LINE];
	int n;

	assert(gpsfd > 0);

	if ((n = read(gpsfd, gpsline, GPS_LINE)) < 0) {
		perror("read()");
		return (-1);
	}
#if	defined(_GPS_DEBUG_)
	printf("%s: %s", __func__, gpsline);
#endif

	assert(n > 0 && n < GPS_LINE);
	strncpy(gps_data, gpsline, n);

	return (0);
}

//
// parse gps data value
// gps_data is raw gps input data string,
// longitude and latitude's units are cent.
// return 0 if success, otherwise -1
//
int
GPS_ParseGPSData(char *gps_data, double *longitude,
								double *latitude)
{
	char *pos;
	char buf[GPS_LINE];
	double t;

	pos = gps_data;
	assert(pos != NULL);
	if (strncmp(pos, GPGGA_HEAD, GPGGA_HEAD_LEN)) {
#if	defined(_GPS_DEBUG_)
		perror(__func__);
#endif
		return (-1);
	}

	// latitude degree
	memset(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LATITUDE_DEGREE_OFF;
	assert(pos != NULL);
	strncpy(buf, pos, GPGGA_LATITUDE_DEGREE_LEN);
	buf[GPGGA_LATITUDE_DEGREE_LEN] = '\0';
	*latitude = atoi(buf) * 60;

	// latitude minute
	memset(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LATITUDE_MINUTE_OFF;
	assert(pos != NULL);
	strncpy(buf, pos, GPGGA_LATITUDE_MINUTE_LEN);
	buf[GPGGA_LATITUDE_MINUTE_LEN] = '\0';
	t = atof(buf);
	*latitude += t;

	// longitude degree
	memset(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LONGITUDE_DEGREE_OFF;
	assert(pos != NULL);
	strncpy(buf, pos, GPGGA_LONGITUDE_DEGREE_LEN);
	buf[GPGGA_LONGITUDE_DEGREE_LEN] = '\0';
	*longitude = atoi(buf) * 60;

	// longitude minute
	memset(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LONGITUDE_MINUTE_OFF;
	assert(pos != NULL);
	strncpy(buf, pos, GPGGA_LONGITUDE_MINUTE_LEN);
	buf[GPGGA_LONGITUDE_MINUTE_LEN] = '\0';
	*longitude += atof(buf);

#if	defined(_GPS_DEBUG_)
	printf("%s:longitude=%f, latitude=%f\n", __func__, *longitude, *latitude);
#endif

	assert(*longitude > DBL_EPSILON);
	assert(*latitude > DBL_EPSILON);

	return (0);
}


//
// read in benchmark coordinates and gps data as
// reference point, here are topleft and bottomright
// of the map.
// return 0 if success, else return -1
//
int
GPS_ReadBenchmarkPointGPSData(MAPNODE *topleft, 
							MAPNODE *bottomright)
{
	FILE *fp;

	// open benchmark gps data file
	fp = fopen(CHENGDU_GPS_DATA_FILE, "rb");
	if (fp == NULL) {
		printf("read gps benchmark data failed\n");
		return (-1);
	}

	// read topleft point data
	assert(topleft != NULL);
	if (fread(topleft, sizeof(MAPNODE), 1, fp) != 1) {
		printf("read topleft point data failed\n");
		return (-1);
	}

	// read bottomright point data
	assert(bottomright != NULL);
	if (fread(bottomright, sizeof(MAPNODE), 1, fp) != 1) {
		printf("read topleft point data failed\n");
		return (-1);
	}

	// adjust the double float, if need
	SWAP_DOUBLE(topleft->longitude);
	SWAP_DOUBLE(topleft->latitude);
	SWAP_DOUBLE(topleft->x);
	SWAP_DOUBLE(topleft->y);
	SWAP_DOUBLE(bottomright->longitude);
	SWAP_DOUBLE(bottomright->latitude);
	SWAP_DOUBLE(bottomright->x);
	SWAP_DOUBLE(bottomright->y);

#if defined(_GPS_DEBUG_)
	printf("topleft:longitude=%f,latitude=%f,x=%f,y=%f\n",
			topleft->longitude,
			topleft->latitude,
			topleft->x,
			topleft->y);
	printf("bottomright:longitude=%f,latitude=%f,x=%f,y=%f\n",
			bottomright->longitude,
			bottomright->latitude,
			bottomright->x,
			bottomright->y);
#endif

	fclose(fp);

	return (0);
}


//
// calculate normalised cartesian x and y in map
// return 0 if success, else return -1
//
int 
GPS_CalcXY(	double longitude, double latitude,
			double *x, double *y)
{
	// if less than DBL_EPSILON,there must be error
	if ((fabs(longitude) < DBL_EPSILON) || (fabs(latitude) < DBL_EPSILON))
		return (-1);

	// longitude & latitude should be non-negtive
 	if ((longitude < -DBL_EPSILON) || (latitude < -DBL_EPSILON))
		return (-1);

	// longitude & latitude should be in the range of supported rectangle,
	// longitude of Earth is increasing from west to east,
	// latitude of Earth is increasing from the equator(³àµÀ) to north or south
	if ((longitude < TopLeft.longitude) || (longitude > BottomRight.longitude))
		return (-1);
	if ((latitude < BottomRight.latitude) || (latitude > TopLeft.latitude))
		return (-1);

	// now do real calculation work
	*x = (longitude - TopLeft.longitude) / (BottomRight.longitude - TopLeft.longitude);
	*y = (latitude - TopLeft.latitude) / (BottomRight.latitude - TopLeft.latitude);

#if defined(_GPS_DEBUG_)
	printf("%s:longitude=%f, latitude=%f, x=%f, y=%f\n",
			__func__, longitude, latitude, *x, *y);
#endif

	assert(*x > DBL_EPSILON);
	assert(*y > DBL_EPSILON);

	return (0);
}

//
// get current location's normalised cartesian x and y
// return 0 if success, else return -1
//
int 
GPS_GetCurrentXY(double *x, double *y)
{
	if (!isSampling)
		return (-1);

	// lock, then access CurrentLocation
	pthread_mutex_lock(&GPSMutex);

	*x = CurrentLocation.x;
	*y = CurrentLocation.y;

	// finish access CurrentLocation, so unlock
	pthread_mutex_unlock(&GPSMutex);

	assert(*x > DBL_EPSILON);
	assert(*y > DBL_EPSILON);

	return (0);
}

//
// main thread for GPS data sampling
//
void *GPS_SampleThread(void *args)
{
	int gps_fd;
	char gps_data[GPS_LINE];
	int err;

	// open GPS device's serial port
	gps_fd = GPS_InitSerialPort(GPS_PORT);

	// read in benchmark point's GPS data
	err = GPS_ReadBenchmarkPointGPSData(&TopLeft, &BottomRight);
	assert(err == 0);


	while (1) {

		// sample GPS data
		err = GPS_ReadGPSData(gps_fd, gps_data);
		assert(err == 0);

		// lock, then access CurrentLocation
		pthread_mutex_lock(&GPSMutex);

		// parse GPS data
		err = GPS_ParseGPSData(gps_data, &CurrentLocation.longitude,
									&CurrentLocation.latitude);
		assert(err == 0);

		// calculate x and y
		err = GPS_CalcXY(CurrentLocation.longitude, CurrentLocation.latitude,
				&CurrentLocation.x, &CurrentLocation.y);
		assert(err == 0);

		// finish access CurrentLocation, so unlock
		pthread_mutex_unlock(&GPSMutex);

		// indicating sampling has begin
		if (isSampling == 0)
			isSampling = 1;

		// block a short time
		sleep(GPS_SAMPLE_INTERVAL);
	}
}

//
// start sampling process
// return 0 if success, else return -1
//
int 
GPS_StartSampleProcess(void)
{
	int err = 0;
	pthread_t gps_thread;

//	err = GPS_ReadBenchmarkPointGPSData(&TopLeft, &BottomRight);
//	assert(err == 0);

	err = pthread_create(&gps_thread, NULL, GPS_SampleThread, NULL);
	if (err) {
		printf("%s:%d:%s: failed\n", __FILE__, __LINE__, __func__);
		return (err);
	}

#if	defined(_GPS_DEBUG_)
	printf("create GPS sample thread success\n");
#endif

	return (err);
}

