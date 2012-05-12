#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "communication.h"

int arduino;
struct termios savedtio;

void cleanup(int signal);

int main(int argc, char **argv) {
	struct sigaction inthandler;
	struct timespec timereq;
	chanattr attrib;
	int i;

	arduino = initialize_serial("/dev/ttyUSB0", &savedtio);
	if(arduino == -1) {
		perror("initialize_serial");
		exit(-1);
	}

	inthandler.sa_handler = cleanup;
	sigemptyset(&(inthandler.sa_mask));
	sigaddset(&(inthandler.sa_mask), SIGHUP);
	sigaddset(&(inthandler.sa_mask), SIGINT);
	sigaddset(&(inthandler.sa_mask), SIGQUIT);
	sigaddset(&(inthandler.sa_mask), SIGPIPE);
	sigaddset(&(inthandler.sa_mask), SIGTERM);
	inthandler.sa_flags = 0;
	sigaction(SIGHUP, &inthandler, NULL);
	sigaction(SIGINT, &inthandler, NULL);
	sigaction(SIGQUIT, &inthandler, NULL);
	sigaction(SIGPIPE, &inthandler, NULL);
	sigaction(SIGTERM, &inthandler, NULL);

	attrib.mode = SQUARE;
	for(;;) {
		for(i = 125; i > 64; i -= 4) {
			attrib.lowval = -i;
			attrib.highval = i;
			attrib.period = 200 - i;
			attrib.duty = attrib.period / 2;
			send_serial(arduino, 0, &attrib);
			timereq.tv_sec = 0;
			timereq.tv_nsec = 10000000;
			nanosleep(&timereq, NULL);
		}
		timereq.tv_sec = 0;
		timereq.tv_nsec = 200000000;
		nanosleep(&timereq, NULL);
		for(i = 125; i > 64; i -= 4) {
			attrib.lowval = -i + 64;
			attrib.highval = i - 64;
			attrib.period = 150 - i;
			attrib.duty = 10;
			send_serial(arduino, 1, &attrib);
			timereq.tv_sec = 0;
			timereq.tv_nsec = 10000000;
			nanosleep(&timereq, NULL);
		}
		timereq.tv_sec = 0;
		timereq.tv_nsec = 200000000;
		nanosleep(&timereq, NULL);
	}
	
	silence(arduino);
	restore_serial(arduino, &savedtio);
}

void cleanup(int signal) {
	silence(arduino);

	restore_serial(arduino, &savedtio);

	exit(0);
}
