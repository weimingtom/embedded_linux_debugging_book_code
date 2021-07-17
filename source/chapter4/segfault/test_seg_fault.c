/* $Id: test_seg_fault.c
 * $Author: rockins
 * $Date: Oct 11,2006
 * $Detail: intend to Illustrate the segment-fault 
 * programming error's reason
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    unsigned char	*p;

    strcpy(p, "abcde");

    return (0);
}
