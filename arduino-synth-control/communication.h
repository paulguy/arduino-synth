#define SILENT 0
#define SQUARE 1

typedef struct {
	unsigned char mode;
	unsigned short int period;
	unsigned short int duty;
	char lowval;
	char highval;
} chanattr;

int initialize_serial(char *device, struct termios *oldtio);
void restore_serial(int fd, struct termios *oldtio);
int send_serial(int fd, unsigned char channel, chanattr *attrib);
void silence(int fd);
