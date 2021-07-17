#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct tagMAPNODE {
	double longitude;	// in minute
	double latitude;	// in minute
	double x;
	double y;
}MAPNODE;

int
main(void)
{
	FILE *fp;
	MAPNODE mapnode;

	fp = fopen("chengdu_gps.dat", "rb");
	if (fp == NULL)
		return (-1);

	if (fread(&mapnode, sizeof(MAPNODE), 1, fp) != 1)
		return (-1);

	printf("topleft:\n");
	printf("longitude = %f, latitude = %f, x = %f, y = %f\n",
			mapnode.longitude,
			mapnode.latitude,
			mapnode.x,
			mapnode.y);

	if (fread(&mapnode, sizeof(MAPNODE), 1, fp) != 1)
		return (-1);

	printf("bottomright:\n");
	printf("longitude = %f, latitude = %f, x = %f, y = %f\n",
			mapnode.longitude,
			mapnode.latitude,
			mapnode.x,
			mapnode.y);

	fclose(fp);
	return (0);
}
