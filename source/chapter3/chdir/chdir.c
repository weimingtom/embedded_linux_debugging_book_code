/*
 * $Id: chdir.c
 * $check GDB's cd command
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open("test.txt", O_RDONLY);
    if (fd < 0) {
	perror("error:");
	return (-1);
    }
    printf("open file success\n");
    close(fd);

    return(0);
}
