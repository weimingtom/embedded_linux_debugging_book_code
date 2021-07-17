#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gps.h"

int
main(void)
{
	double x;
	double y;
	int i;

	GPS_StartSampleProcess();

	for (i = 0; i < 15; i++) {
		GPS_GetCurrentXY(&x, &y);
		sleep(GPS_SAMPLE_INTERVAL);
	}
}
