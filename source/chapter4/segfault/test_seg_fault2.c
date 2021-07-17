/* $Id: test_seg_fault2.c
 * $Author: rockins
 * $Date: Oct 11,2006
 * $Detail: intend to Illustrate the segment-fault caused by  
 * programming error's reason
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef	BUFSIZ
#undef	BUFSIZ
#define	BUFSIZ	64
#endif

int main(int argc, char *argv[])
{
    unsigned char	*p;
    unsigned char	*q;

    p = (unsigned char *)malloc(BUFSIZ*sizeof(unsigned char));
    strcpy(p, "abcde");
    free(p);

    /*...maybe thousands of lines...*/

    q = (unsigned char *)malloc(BUFSIZ*sizeof(unsigned char));
    strcpy(q, "opqrst");
    
    if (p != NULL)
	strcpy(p, "uvwxyz");

    printf("p = %s, q = %s\n", p, q);

    return (0);
}
