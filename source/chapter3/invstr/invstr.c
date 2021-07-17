#include <stdio.h>
#include <stdlib.h>

#define	MAXLEN	8192

int
main(int argc, char *argv[])
{
	unsigned char *pStr;
	signed int i, n;

	if (argc < 2) {
		printf("usage:%s <string>\n", argv[0]);
		exit(-1);
	}

	/*
	 * allocate and zero buffer
	 */
	n = strnlen(argv[1], MAXLEN);
	pStr = (unsigned char *)malloc((n+1) * sizeof(unsigned char));
	bzero(pStr, n+1);

	/*
	 * inverse input string and put in pStr
	 */
	for (i = n-1; i >= 0; i--) {
		pStr[n-1-i] = argv[1][i];
	}
	pStr[n] = '\0';
	printf("oldstr:%s\n", argv[1]);
	printf("invstr:%s\n", pStr);
	
	return (0);
}
