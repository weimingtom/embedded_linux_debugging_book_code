/*
 * $file : simple_fork.c
 * $desc : demonstrator a simple multi-process example
 * $author : rockins
 * $date : Sat Feb  3 00:41:44 CST 2007
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
    pid_t pid;


    if (fork() == 0) {
        printf("in child process %d\n", getpid());
    } else {
        printf("in parent process %d\n", getpid());
    }

    return (0);
}
