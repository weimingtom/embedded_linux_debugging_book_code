#include <stdio.h>
#include <stdlib.h>

class FOO {
	public:
		int bar(int a, int b)
		{
			printf("%s : %d + %d = %d\n",
					__func__, a, b, a+b);
		}
};

int
main(int argc, char *argv[])
{
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);

	FOO *foo = new FOO();
	foo->bar(a, b);
	return (0);
}
