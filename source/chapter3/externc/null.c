#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void err_sys(char *errstr);

int
main(int argc,char **argv)
{
	int n, buffsize;
	char *buff;
	
	if(argc != 2) {
		printf("Usage:%s #n\n",argv[0]);
		exit(1);
	}
	
	buffsize = atoi(argv[1]);
	if ((buff = malloc(buffsize)) == NULL)
		err_sys("malloc memory error.");
	
	while ((n = read(STDIN_FILENO,buff,buffsize)) > 0)
		if (write(STDOUT_FILENO,buff,n) != n)
			err_sys("write error.");
	
	if(n < 0)
		err_sys("read error.");
	exit(0);
}

void
err_sys(char *errstr)
{
	fprintf(stderr, "%s\n", errstr);
}
