#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "communication.h"

chanattr stop;

int initialize_serial(char *device, struct termios *oldtio) {
	int fd;
	struct termios newtio;

	fd = open(device, O_RDWR | O_NOCTTY ); 
	if (fd < 0) {
		return(-1);
	}

	tcgetattr(fd,oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD; 
	newtio.c_iflag = IGNPAR | IXON | IXOFF;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	stop.mode = SILENT;
	stop.period = 0;
	stop.duty = 0;

	return(fd);
}

void restore_serial(int fd, struct termios *oldtio) {
	tcsetattr(fd, TCSANOW, oldtio);
	close(fd);
}

void send_serial(int fd, unsigned char channel, chanattr *attrib) {
	write(fd, &channel, 1);
	write(fd, &(attrib->mode), 1);
	write(fd, &((unsigned char *)&(attrib->period))[1], 1);
	write(fd, &((unsigned char *)&(attrib->period))[0], 1);
	write(fd, &((unsigned char *)&(attrib->duty))[1], 1);
	write(fd, &((unsigned char *)&(attrib->duty))[0], 1);
	write(fd, &(attrib->lowval), 1);
	write(fd, &(attrib->highval), 1);
}

void silence(int fd) {
	send_serial(fd, 0, &stop);
	send_serial(fd, 1, &stop);
	send_serial(fd, 2, &stop);
	send_serial(fd, 3, &stop);
}
