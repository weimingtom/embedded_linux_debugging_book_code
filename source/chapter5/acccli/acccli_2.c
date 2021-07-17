#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define	SVR_ADDR	"202.115.26.224"	/*server address*/
#define	LOG_PORT	10009				/*server port*/
#define	LINE_SZ		8192				/*msg buffer size*/

/*
 * used for retrieve acceleration
 * from accelerometer
 */
struct acce {
	unsigned int    x;	/*pulse counter for x axis*/
	unsigned int    y;	/*pulse counter for y axis*/
};

int
main(int argc, char *argv[])
{
	int             acc_fd;				/*file descriptor*/
	struct acce     acc_val;			/*acceleration value*/
	int             cli_sock;			/*client socket*/
	struct sockaddr_in svr_addr;		/*server socket addr*/
	char            line[LINE_SZ];		/*message buffer*/
	float           old_acc_x = 0.0f;	/*old acc x*/
	float			old_acc_y = 0.0f;	/*old acc y*/
	float           new_acc_x = 0.0f;	/*new acc x*/
	float			new_acc_y = 0.0f;	/*new acc y*/
	int				len;				/*sent length*/

	/*open accelerometer*/
	if ((acc_fd = open("/dev/acce", O_RDONLY)) < 0) {
		perror("open()");
		exit(-1);
	}

	/*fill in server socket address structure*/
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(LOG_PORT);
	svr_addr.sin_addr.s_addr = inet_addr(SVR_ADDR);

	while (1) {

		/*read acceleration value*/
		if (read(acc_fd, &acc_val, sizeof(acc_val)) 
				!= sizeof(acc_val)) {
			perror("read()");
			exit(-1);
		}

		/*store old acceleration value*/
		old_acc_x = new_acc_x;
		old_acc_y = new_acc_y;

		/*calculation new acceleration value*/
		new_acc_x = (((double) acc_val.x) / 1000 - 0.5) * 8 * 9.8;
		new_acc_y = (((double) acc_val.y) / 1000 - 0.5) * 8 * 9.8;

		/*if satisfy transfer condition*/
		if ((fabs(new_acc_x - old_acc_x) > 0.1) ||
			(fabs(new_acc_y - old_acc_y) > 0.1)) {

			/*construct message in line,len is the msg's length*/
			snprintf(line, LINE_SZ, "acceleration of X=%f,"
				 "acceleration of Y=%f\n", new_acc_x, new_acc_y);
			len = strnlen(line, LINE_SZ);

			/*construct client socket*/
			cli_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (cli_sock < 0) {
				perror("socket()");
				exit(-1);
			}

			/*connect and write data to the socket pipe*/
			if (connect(cli_sock, (struct sockaddr *) &svr_addr,
						sizeof(svr_addr)) < 0) {
				perror("connect()");
				exit(-1);
			}

			if (write(cli_sock, line, len) != len) {
				perror("write()");
				exit(-1);
			}

			/*close client socket*/
			close(cli_sock);

		}

		/*pause for a second*/
		sleep(1);
	}

	return (0);
}
