#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define	DFL_LOG		"/var/log/acclog"	// default log file
#define	LINE_SZ		8192				// line max length
#define	LOG_PORT	10009				// log port

int
main(int argc, char *argv[])
{
	int listen_sock;					// listening socket
	int data_sock;						// tranceiving socket
	FILE *log_fp;						// log file pointer
	char *log_file = DFL_LOG;			// default log file
	struct sockaddr_in svr_addr;		// server socket addr
	char line[LINE_SZ];					// line buffer
	int ln_rd_len;						// read length of line
	int ln_wr_len;						// write length of line

	/*open log file*/
	if ((log_fp = fopen(log_file, "a+")) == NULL) {
		perror("open()");
		exit(-1);
	}

	/*create listening socket*/
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		perror("socket()");
		exit(-1);
	}

	/*stuff listening socket address structure*/
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(LOG_PORT);
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*bind to local listening port and listen*/
	if (bind(listen_sock, (struct sockaddr *)&svr_addr, 
				sizeof(svr_addr)) < 0) {
		perror("bind()");
		exit(-1);
	}

	if (listen(listen_sock, 5) < 0) {
		perror("listen()");
		exit(-1);
	}

	while (1) {
		/*accept new incoming connect request*/
		if ((data_sock = accept(listen_sock, NULL, 0)) < 0) {
			perror("accept()");
			continue;
		}
		
		/*read incoming data*/
		if ((ln_rd_len = read(data_sock, line, LINE_SZ)) < 0) {
			perror("read()");
			continue;
		}

		/*log into log-file and print on console*/
		if ((ln_wr_len = fputs(line, log_fp)) == EOF) {
			perror("fputs()");
			continue;
		}
		printf("%s", line);

		/*close data connection*/
		close(data_sock);
	}
	
	return (0);
}
