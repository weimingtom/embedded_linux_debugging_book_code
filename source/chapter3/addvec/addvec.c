/*
 * DESC: 
 * 		x and y are two array which have
 * 		same dimension. then add them
 * 		together, result in z array, which
 * 		also has the same dimension as x,y
 */

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	int i, n;
	int *x_p;		// x_p is the x array
	int *y_p;		// y_p is the y array
	int *z_p;		// z_p is the result z array

	printf("please input vector size:");
	scanf("%d", &n);

	/*
	 * allocate space
	 */
	x_p = (int *)malloc(n * sizeof(int));
	y_p = (int *)malloc(n * sizeof(int));
	z_p = (int *)malloc(n * sizeof(int));

	/*
	 * input data
	 */
	printf("please input x array(seperate in SPACE):");
	for (i = 0; i<n; i++) {
		scanf("%d", x_p+i);
	}

	printf("please input y array(seperate in SPACE):");
	for (i = 0; i<n; i++) {
		scanf("%d", y_p+i);
	}

	/*
	 * add the x and y vector
	 */
	for (i = 0; i<n; i++) {
		*(z_p + i) = *(x_p + i) + *(y_p + i);
	}
	
	/*
	 * output the result z array
	 */
	printf("Ok, the result z array is:");
	for (i = 0; i<n; i++) {
		printf("%d ", *(z_p + i));
	}
	printf("\n");

	return (0);
}
