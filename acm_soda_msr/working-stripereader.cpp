#include <string>
#include <iostream>
#include <strings.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0

using namespace std;

/* getlrc: calculates LRC of commands, which is computed by xor of all bytes */
char getlrc(string buf) {
	char lrc='\x60';
	for(int i=0; i<buf.size(); i++) {
		lrc ^= buf[i];
	}
	return lrc;
}

/* send: send command string on serial FD */
void send(int fd, string in) {
	char lrc;
	const char ETX='\x03';
  const int BUFFER_SIZE = 256;
  char input_buffer[BUFFER_SIZE];

	char buf[BUFFER_SIZE];
	int size=2;
	char *inptr;
	int nbytes;
	bzero(input_buffer, BUFFER_SIZE);

	buf[0] = '\x60';
	buf[1] = '\x00';

	if(in.size()+2 >= BUFFER_SIZE) {
		// command too long, abort.
		return;
	}
	for(int i=0; i<in.size(); i++) {
		buf[size] = in[i];
		size++;
	}

	// calculate the LRC, append the ETX character
	lrc=getlrc(in);
	buf[size] = lrc;
	size++;
	buf[size] = ETX;
	size++;

#if DEBUG
	cout << size << endl;
	for(int i=0; i<size; i++) {
		cout << hex << (int)buf[i] << " ";
	}
	cout << dec << endl;
#endif

	// send and wait for an exorbinantly long time
	write(fd, buf, size);
	usleep(1000000);
#if DEBUG
	// read and show the serial buffer
	read(fd, input_buffer, BUFFER_SIZE);
	for(int i=0; i<BUFFER_SIZE; i++) {
		cout << hex << (int)input_buffer[i] << dec << " ";
		if(input_buffer[i] == '\x03') {
			i=BUFFER_SIZE;
		}
	}
	cout << endl;
#endif
	// flush anything remaining in buffers
	tcflush(fd, TCIOFLUSH);
}

/* main: program loop: set up reader, read data */
int main(int argc, char *argv[]) {
  string buf;
  const int BUFFER_SIZE = 256;
  char input_buffer[BUFFER_SIZE];
	int fd;
	struct termios options;
	char *inptr;
	int nbytes;

	/* open the port */
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	fcntl(fd, F_SETFL, 0);

	/* get the current options */
	tcgetattr(fd, &options);

	/* set raw input, 1 second timeout */
	cfsetispeed(&options, B38400);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10;

	// Set Reader Option, LED Controlled by Host, No MSR Data Envelope,
	// Standard Decoder
	buf="\x04\x53\x11\x01\x10";
	send(fd, buf);

	// Set Track Selection, Track 2 Only
  buf="\x04\x53\x13\x01\x32";
	send(fd, buf);

	// Set MSR Reading Enable
	buf="\x04\x53\x1a\x01\x31";
	send(fd, buf);

	// Set LED Command, LED OFF
	buf="\x02\x6c\x30";
	send(fd, buf);

	while(1) {
		bzero(input_buffer, BUFFER_SIZE);

		if(read(fd, input_buffer, sizeof(input_buffer))) {
			if(input_buffer[0] != ';' || input_buffer[strlen(input_buffer)-2] != '?')
				continue;
			cout << input_buffer << endl;

			// Set MSR Reading Disable
			buf="\x04\x53\x1a\x01\x30";
			send(fd, buf);

			// Set LED Command, LED Amber
			buf="\x02\x6c\x33";
			send(fd, buf);

			// Query account here
#if DEBUG
			for(int i=0; i<BUFFER_SIZE; i++) {
				cout << hex << (int)input_buffer[i] << dec << " ";
				if(input_buffer[i] == '\x03') {
					i=BUFFER_SIZE;
				}
			}
			cout << endl;
#endif

			if (1) { // Account OK
				// Set LED Command, LED Green
				buf="\x02\x6c\x31";
				send(fd, buf);
			}
			else { // Account invalid/NSF
				// Set LED Command, LED Red
				buf="\x02\x6c\x32";
				send(fd, buf);
			}
			usleep(1000000);

			// Set LED Command, LED Off
			buf="\x02\x6c\x30";
			send(fd, buf);

			// Set MSR Reading Enable
			buf="\x04\x53\x1a\x01\x31";
			send(fd, buf);
		}
	}

	close(fd);

	return 0;
}
