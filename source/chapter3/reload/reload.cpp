#include <stdio.h>
#include <stdlib.h>

double
multiple(double a, double b, double c)
{
	return (a * b * c);
}

double
multiple(double a, double b)
{
	return (a * b);
}

int
main(int argc, char ** argv)
{
	double a, b, c;

	if (argc == 4) {
		a = atof(argv[1]);
		b = atof(argv[2]);
		c = atof(argv[3]);
		printf("result=%f\n", multiple(a, b, c));
	} else {
		a = atof(argv[1]);
		b = atof(argv[2]);
		printf("result=%f\n", multiple(a, b));
	}

	return (0);
}
