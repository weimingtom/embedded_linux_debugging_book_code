/* $Id: test_seg_fault3.c
 * $Author: rockins
 * $Date: Oct 11,2006
 * $Detail: intend to exploit the memory-leak problems
 */
#include <stdio.h>
#include <stdlib.h>

#define	BUFF_SIZE	1024

int main(int argc, char *argv[])
{
    unsigned char	*p;
    unsigned int	time = 0;

    while (1) {
	p = (unsigned char *)malloc(BUFF_SIZE * sizeof(unsigned char));
    	strcpy(p, "abcde");
	printf("%d:%s\n", time++, p);
    }

    return (0);
}
