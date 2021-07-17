/*
 * $file	:
 * $desc	:
 * $author	:	rockins
 * $date	:
 * $copyright	:	all copyrights(c) reserved.
 */

#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "gps.h"

#define	_GPS_DEBUG_

//
// init gps serial port
// success: serial port fd
// fail: -1
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

	cfmakeraw(&termopt);

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

	if ((n = read(gpsfd, gpsline, GPS_LINE)) < 0) {
		perror("read()");
		return (-1);
	}
#if	defined(_GPS_DEBUG_)
	printf("%s: %s", __func__, gpsline);
#endif

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
GPS_ParseGPSData(char *gps_data, double *latitude,
								double *longitude)
{
	char *pos;
	char buf[GPS_LINE];

	pos = gps_data;
	if (strncmp(pos, GPGGA_HEAD, GPGGA_HEAD_LEN)) {
#if	defined(_GPS_DEBUG_)
		perror(__func__);
#endif
		return (-1);
	}

	// latitude degree
	memcpy(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LATITUDE_DEGREE_OFF;
	strncpy(buf, pos, GPGGA_LATITUDE_DEGREE_LEN);
	buf[GPGGA_LATITUDE_DEGREE_LEN] = '\0';
	*latitude = atoi(buf) * 60;

	// latitude minute
	memcpy(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LATITUDE_MINUTE_OFF;
	strncpy(buf, pos, GPGGA_LATITUDE_MINUTE_LEN);
	buf[GPGGA_LATITUDE_MINUTE_LEN] = '\0';
	*latitude += atof(buf);

	// longitude degree
	memcpy(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LONGITUDE_DEGREE_OFF;
	strncpy(buf, pos, GPGGA_LONGITUDE_DEGREE_LEN);
	buf[GPGGA_LONGITUDE_DEGREE_LEN] = '\0';
	*longitude = atoi(buf) * 60;

	// longitude minute
	memcpy(buf, 0, GPS_LINE);
	pos = gps_data + GPGGA_LONGITUDE_MINUTE_OFF;
	strncpy(buf, pos, GPGGA_LONGITUDE_MINUTE_LEN);
	buf[GPGGA_LONGITUDE_MINUTE_LEN] = '\0';
	*longitude += atof(buf);

#if	defined(_GPS_DEBUG_)
	printf("latitude=%f, longitude=%f", *latitude, *longitude);
#endif
	return (0);
}
