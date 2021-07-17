#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define	SVR_ADDR	"202.115.26.224"
#define	LOG_PORT	10009
#define	LINE_SZ		8192

struct acce {
	unsigned int    x;
	unsigned int    y;
};

int
main(int argc, char *argv[])
{
	int             acc_fd;
	struct acce     acc_val;
	int             cli_sock;
	struct sockaddr_in svr_addr;
	char            line[LINE_SZ];
	float           old_acc_x = 0.0f,
					old_acc_y = 0.0f;
	float           new_acc_x = 0.0f,
					new_acc_y = 0.0f;

	if ((acc_fd = open("/dev/acce", O_RDONLY)) < 0) {
		perror("open()");
		exit(-1);
	}

	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(LOG_PORT);
	svr_addr.sin_addr.s_addr = inet_addr(SVR_ADDR);

	while (1) {

		if (read(acc_fd, &acc_val, sizeof(acc_val)) != sizeof(acc_val)) {
			perror("read()");
			exit(-1);
		}

		new_acc_x = (((double) acc_val.x) / 1000 - 0.5) * 8 * 9.8;
		new_acc_y = (((double) acc_val.y) / 1000 - 0.5) * 8 * 9.8;

		if (((new_acc_x - old_acc_x) > 0.01) ||
			((new_acc_y - old_acc_y) > 0.01)) {
			cli_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (cli_sock < 0) {
				perror("socket()");
				exit(-1);
			}

			if (connect(cli_sock, (struct sockaddr *) &svr_addr,
						sizeof(svr_addr)) < 0) {
				perror("connect()");
				exit(-1);
			}

			snprintf(line, LINE_SZ, "acceleration of X=%f,"
					 "acceleration of Y=%f\n", new_acc_x, new_acc_y);
			int len = strnlen(line, LINE_SZ);
			if (write(cli_sock, line, len) != len) {
				perror("write()");
				exit(-1);
			}

			close(cli_sock);
		}
	}
	return (0);
}
