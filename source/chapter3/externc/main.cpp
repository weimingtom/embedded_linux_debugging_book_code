#include "test_extern_c.h"
#include <stdio.h>
#include <stdlib.h>

class FOO {
	public:
		int bar(int a, int b)
		{
			printf("result=%i\n", ThisIsTest(a, b));
		}

		int bar(int a)
		{
			printf("result=%i\n", a);
		}
};

int
main(int argc, char **argv)
{
	FOO *foo = new FOO();
	int a = atoi(argv[1]);

	if (argc == 3) { 
		int b = atoi(argv[2]);
		foo->bar(a, b);
	} else {
		foo->bar(a);
	}
	return(0);
}
