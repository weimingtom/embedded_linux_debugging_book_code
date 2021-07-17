#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sig_handle(int signo);

int main(void)
{
	signal(SIGHUP, sig_handle);
	signal(SIGQUIT, sig_handle);
	signal(SIGILL, sig_handle);

	while (1) {
		printf("idle...\n");
		sleep(1);
	}

	return (0);
}

void sig_handle(int signo)
{
	if (signo == SIGHUP) {
		printf("received SIGHUP\n");
	} else if (signo == SIGQUIT) {
		printf("received SIGQUIT\n");
	} else if (signo == SIGILL) {
		printf("received SIGILL\n");
	}
}
