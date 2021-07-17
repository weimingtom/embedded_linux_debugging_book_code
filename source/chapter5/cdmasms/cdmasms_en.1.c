/*
 * $file:	cdmasms_en.c
 * $auth:	rockins
 * $desc:	send English SMS via CDMA modem
 * $date:	Sun Jan 21 04:41:20 CST 2007
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define	CTRL_Z	(unsigned char)26	/*finish sms input*/
#define	ESC		(unsigned char)27	/*abort sms input*/
#define	CR		'\r'				/*carriage return*/
#define	NL		'\n'				/*next line*/
#define	ST_REP	"+CDS"				/*sms-status-report*/

#define	SERIAL	"/dev/ttyS1"		/*serial port*/
#define	MAX_SZ	1024				/*buffer size*/
#define	SMS_LEN	140					/*max length of sms*/
#define	RETRY	20					/*retry time*/

int
main(int argc, char *argv[])
{
	int CDMAFd;
	int err = 0;
	
	/*check command-line*/
	if (argc != 3) {
		printf("usage:%s tel sms\n", argv[0]);
		printf("sms must be in English\n");
		exit(-1);
	}

	CDMAFd= CDMAOpenModem(SERIAL);
	if (CDMAFd < 0) {
		printf("open CDMA Modem %s failed!\n", SERIAL);
		return (-1);
	}

	err = CDMASetModemParams(CDMAFd);
	if (err < 0)
		goto fail;

	err = CDMAInitModem(CDMAFd);
	if (err < 0)
		goto fail;

	err = CDMASendSMS(CDMAFd, argv[1], argv[2]);
	if (err < 0)
		goto fail;

	CDMACloseModem(CDMAFd);
	printf("send English(Ascii) SMS success!\n");
	return (0);

fail:
	CDMACloseModem(CDMAFd);
	printf("send English(Ascii) SMS failed!\n");
	return (-1);
}

/*
 * open CDMA Modem device
 * params:
 * 		CDMADevice:	device namde
 * returns:
 * 		non-negtive file descriptor if success,
 * 		else -1
 */
int
CDMAOpenModem(char *CDMADevice)
{
	int fd;

	/*
	 * open CDMA serial port, O_NOCTTY means it's
	 * not a controll tty device, O_NDELAY means
	 * it's not block
	 */
	fd = open(CDMADevice, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		perror("open()");
		return (-1);
	}
	return (fd);
}

/*
 * close modem
 */
int
CDMACloseModem(int CDMAFd)
{
	return (close(CDMAFd));
}

/*
 * establish working settings
 * params:
 * 		CDMAFd:	file descriptor of CDMA modem
 * returns:
 * 		0 if set success, else -1
 * note(default setting):
 * 		speed:		115200 bps
 * 		databit:	8 bits
 * 		parity:		none
 * 		stopbit:	1 bit
 */
int
CDMASetModemParams(int CDMAFd)
{
	struct termios	term_opt;

	/*
	 * get current attributes
	 */
	tcgetattr(CDMAFd, &term_opt);

	/*
	 * set attribute to 8N1
	 */
	term_opt.c_cflag &= ~CSIZE;
	term_opt.c_cflag |= CS8;		/*8 bit databit*/
	term_opt.c_cflag &= ~PARENB;	/*disable parity*/
	term_opt.c_cflag &= ~CSTOPB;	/*set 1 bit stopbit*/

	/*
	 * set in&out serial port speed to 115200 bps
	 */
	cfsetispeed(&term_opt, B115200);
	cfsetospeed(&term_opt, B115200);

	/*
	 * set to raw mode
	 */
	cfmakeraw(&term_opt);

	/*
	 * set reading timeout to 5 seconds
	 */
	term_opt.c_cc[VMIN] = 0;
	term_opt.c_cc[VTIME] = 100;		/*timeout: 10 seconds*/

	/*
	 * make new attr be committed
	 */
	tcsetattr(CDMAFd, TCSANOW, &term_opt);
}

/*
 * check if CDMA work correctly?
 * params:
 * 		CDMAFd:	the file descritor of CDMA Modem
 * returns:
 * 		0 if work healthy,else -1
 * note:
 * 		according AT command manual, if send "AT" to DCE and
 * 		it response "OK", then can treat the device is
 * 		working correctly.
 */
int
CDMAInitModem(int CDMAFd)
{
	unsigned char cmd_buf[MAX_SZ];
	int retry;
	int len;
	
	/*flush data received and transimitted*/
	tcflush(CDMAFd, TCIOFLUSH);

	/*issue AT command*/
	memset(cmd_buf, 0, MAX_SZ);
	strncpy(cmd_buf, "AT\r", MAX_SZ);
	len = write(CDMAFd, cmd_buf, strnlen(cmd_buf, MAX_SZ));
	if (len != strnlen(cmd_buf, MAX_SZ)) {
		perror("write()");
		return (-1);
	}

	for (retry = 0; retry < RETRY; retry++) {
		/*wait for 1 second*/
		sleep(1);
		
		memset(cmd_buf, 0, MAX_SZ);
		if ((len = read(CDMAFd, cmd_buf, MAX_SZ)) < 0) {
			perror("read()");
			continue;
		}
		
		if (!strncmp(cmd_buf, "OK", 2))
			return (0);
	}

	return (-1);
}

/*
 * send SMS via the CDMA modem
 * params:
 * 		CDMAFd:	file descriptor
 * 		msg:	message to send
 * returns:
 * 		0 if success, else -1
 * note:
 * 		message must be in TEXT mode, 
 * 		CDMA do not support PDU mode as GSM does.
 * 		tel must be 13xxxxxxxxx format, no +prefix,
 * 		message should be pure English words.
 */
int
CDMASendSMS(int CDMAFd, char *phone, char *msg)
{
	unsigned char buf[MAX_SZ];
	int msg_len;
	int len;
	int retry;

	/*set in TEXT mode, whereas AT+CMGF=0\r is PDU mode*/
	memset(buf, 0, MAX_SZ);
	snprintf(buf, MAX_SZ, "AT+CMGF=1\r");
	len = write(CDMAFd, buf, strnlen(buf, MAX_SZ));
	if (len != strnlen(buf, MAX_SZ)) {
		perror("write()");
		return (-1);
	}

	/*set language and encoding: 1,2 means English,ascii*/
	memset(buf, 0, MAX_SZ);
	snprintf(buf, MAX_SZ, "AT+WSCL=1,2\r");
	len = write(CDMAFd, buf, strnlen(buf, MAX_SZ));
	if (len != strnlen(buf, MAX_SZ)) {
		perror("write()");
		return (-1);
	}

	/*send message*/
	msg_len = strnlen(msg, MAX_SZ);
	if (msg_len > SMS_LEN) {
		printf("too long SMS\n");
		return (-1);
	}

	memset(buf, 0, MAX_SZ);
	snprintf(buf, MAX_SZ, "AT+CMGS=\"%s\",%d\r%s%c", 
				phone, msg_len, msg, CTRL_Z);
	len = write(CDMAFd, buf, strnlen(buf, MAX_SZ));
	if (len != strnlen(buf, MAX_SZ)) {
		perror("write()");
		return (-1);
	}

	/*waiting for acknowledge from the SMS center*/
	for (retry = 0; retry < RETRY; retry++) {
		/*wait for 1 second*/
		sleep(1);

		len = 0;
		memset(buf, 0, MAX_SZ);
		if ((len = read(CDMAFd, buf, MAX_SZ)) > 0) {
			printf("%s\n", buf);

			/*positive acknoledge*/
			if (!strncmp(buf, ST_REP, strlen(ST_REP)))
				return (0);
		}
	}

	return (-1);
}
