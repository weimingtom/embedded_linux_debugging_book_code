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
#include <iconv.h>
#include <unistd.h>

#define	CTRL_Z	"\x00\x1A"			/*finish sms input*/
#define	CTRL_Z_LEN	2				/*length of CTRL_Z*/
#define	ESC		"\x00\x1B"			/*abort sms input*/
#define	ESC_LEN		2				/*length of ESC*/
#define	CR		'\r'				/*carriage return*/
#define	NL		'\n'				/*next line*/
#define	ST_REP	"+CDS"				/*sms-status-report*/

#define	SERIAL	"/dev/ttyS1"		/*serial port*/
#define	MAX_SZ	1024				/*buffer size*/
#define	SMS_LEN	140					/*max length of sms*/
#define	RETRY	20					/*retry time*/

static int CDMAGB2312toUNICODEBIG(char *in, 
		size_t instrlen, char *out, size_t outbufsiz);

int
main(int argc, char *argv[])
{
	int CDMAFd;
	int err = 0;
	/*GB2312 message: welcome to ³Éµç*/
	char message[] = "\x77\x65\x6C\x63\x6F\x6D\x65"
					"\x20\x74\x6F\x20\xB3\xC9\xB5\xE7";
	
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

	err = CDMASendSMS(CDMAFd, argv[1], message);
	if (err < 0)
		goto fail;

	CDMACloseModem(CDMAFd);
	printf("send Chinese(Unicode) SMS success!\n");
	return (0);

fail:
	CDMACloseModem(CDMAFd);
	printf("send Chinese(Unicode) SMS failed!\n");
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
	fd = open(CDMADevice, O_RDWR | O_NOCTTY);
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
		
		if (strstr(cmd_buf, "OK"))
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
	char buf[MAX_SZ];			/*command and response buffer*/
	int len;					/*length written*/
	int cmd_len;				/*AT command's length*/
	char *msg_p;				/*point to msg*/
	int msg_len;				/*msg's length, in bytes*/
	char conv_buf[MAX_SZ];		/*buff for encode conversion*/
	int conv_len;		/*converted msg's length*/
	int retry;			/*retry time*/

	/* AT+CMGF=1\r set in TEXT mode, whereas AT+CMGF=0\r 
	 * is PDU mode, which is not supported by CDMA*/
	memset(buf, 0, MAX_SZ);
	cmd_len = snprintf(buf, MAX_SZ, "AT+CMGF=1\r");
	len = write(CDMAFd, buf, cmd_len);
	if (len != cmd_len) {
		perror("write()");
		return (-1);
	}

	/*set language and encoding: 6,4 means Chinese,unicode*/
	memset(buf, 0, MAX_SZ);
	cmd_len = snprintf(buf, MAX_SZ, "AT+WSCL=6,4\r");
	len = write(CDMAFd, buf, cmd_len);
	if (len != cmd_len) {
		perror("write()");
		return (-1);
	}

	/*convert from GB2312 to UNICODE*/
	msg_len = strnlen(msg, MAX_SZ);
	memset(conv_buf, 0, MAX_SZ);
	if ((conv_len = CDMAGB2312toUNICODEBIG(msg, msg_len,
							conv_buf, MAX_SZ)) < 0) {
		printf("convert encoding failed!\n");
		return (-1);
	}

	if (conv_len> SMS_LEN) {
		printf("too long SMS,SMS must not be more than"
				" 140 English words or 70 Chinese words\n");
		return (-1);
	}

	/*send message*/
	memset(buf, 0, MAX_SZ);
	cmd_len = snprintf(buf, MAX_SZ, "AT+CMGS=\"%s\",%d\r", 
				phone, conv_len);
	msg_p = buf + cmd_len;
	memcpy(msg_p, conv_buf, conv_len);
	msg_p = buf + cmd_len + conv_len;

	/*below is critical area*/
	memcpy(msg_p, CTRL_Z, CTRL_Z_LEN);
	len = write(CDMAFd, buf, cmd_len + conv_len + CTRL_Z_LEN);
	if (len != (cmd_len + conv_len + CTRL_Z_LEN)) {
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
			if (strstr(buf, ST_REP))
				return (0);
		}
	}

	return (-1);
}

/*
 * convert GB2312 to UNICODE
 * params:
 * 		in:		input GB2312 string
 * 		instrlen:	input bytes number
 * 		out:	output UNICODE big-endian string
 * 		outbufsiz:	output buffer's size
 * returns:
 * 		if success return output encoding's
 * 		length in byte; else return -1
 */
static int
CDMAGB2312toUNICODEBIG(char *in, size_t instrlen,
		char *out, size_t outbufsiz)
{
	char *in_p = in;
	char *out_p = out;
	size_t inbytesleft = instrlen;
	size_t outbytesleft = outbufsiz;
	size_t err = 0;
	iconv_t cd;

	cd = iconv_open("UNICODEBIG", "GB2312");
	if (cd == (iconv_t)(-1)) {
		perror("iconv_open()");
		return (-1);
	}

	err = iconv(cd, &in_p, &inbytesleft,
			&out_p, &outbytesleft);
	if (err == (size_t)(-1)) {
		perror("iconv()");
		return (-1);
	}

	err = iconv_close(cd);
	if (err == -1) {
		perror("iconv_close()");
		return (-1);
	}

	return (outbufsiz - outbytesleft);	/*output encode length*/
}
