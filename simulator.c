//=======================================================================================================================
// Written by LAICE software team in Nov, 2013 to make a hardware-in-the-loop simulator
// to receive fake PMT-type signals from the Debian computer
// Latest version: May 2014, v3
// Generates random data for science and housekeeping data
//=======================================================================================================================

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include "math.h"

// Global variables
int fd_port;
int ff_port;

//======================================================================================================================
int set_interface_attribs (int fd_port, int speed, int parity)
{
// Refer for explanation of options: 
// www.cmrr.umn.edu/~strupp/serial.html#3_1

struct termios tty;

	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd_port, &tty) != 0)
	{
		printf ("error %d from tcgetattr\n", errno);
		return -1;
	}

	cfsetospeed (&tty, B115200);	// Output baud rate
	cfsetispeed (&tty, B115200);	// Input baud rate

	// Input options:
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ICRNL | INLCR | INPCK | ISTRIP | IXON);
	// Local options: select raw unprocessed input
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	// Output options:
	tty.c_oflag = 0;
	tty.c_oflag &= ~OPOST;	// Raw output
		
	// Control options:
	tty.c_cflag |= (CLOCAL | CREAD);	// Local line (don't change line owner) | Enable receiver 
	tty.c_cflag &= ~PARENB;				// Disable parity bit
	tty.c_cflag &= ~CSTOPB;				// 1-stop bit (2 otherwise)
	tty.c_cflag &= ~CSIZE; 				// Bit mask for data bits
	tty.c_cflag |= CS8;     			// 8-bit chars

	// Control characters:
	tty.c_cc[VMIN]  = 2;            // read blocks until atleast 1 byte received
	tty.c_cc[VTIME] = 0;            // 0.1 second timeout
	
	if (tcsetattr (fd_port, TCSANOW, &tty) != 0)
	{
		printf ("error %d from tcsetattr\n", errno);
		return -1;
	}
	return 0;
}

//======================================================================================================================

void pmtActions(void)
{
int i, n_write, n_read;
char TX[161];
char RX[161];
struct timeval timestamp;

	printf("Simulator started========================================\n");

char const* const fileName = "makela.txt"; /* should check that argc > 1 */
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[161];
	while(1) {
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
		n_write = write(fd_port, line, strlen(line));   
		//sleep(1);	
		n_read = read(fd_port, RX, n_write); 
printf("%s", line);
		//sleep(5);
    }
}
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

  /*  fclose(file);
	while(1)
	{
		read = getline(&line, &len, fp)
				     //printf("Retrieved line of length %zu :\n", read);
				     printf("%s", line);
				 }
		
		n_write = write(fd_port, TX, strlen(TX));
		printf("SENDING %d of %d bytes: %s\n", n_write, strlen(TX), TX);	
		sleep(1);	
		n_read = read(ff_port, RX, n_write); 
		printf("RECEIVING: %s\n", RX);	
		sleep(5);
	}
fclose(fp);*/
	return;
}

//======================================================================================================================

int main(void)
{
char *portname = "/dev/ttyO3";

		fd_port = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
   	if (fd_port < 0)
   	{
    		printf ("Error %d opening %s: %s", errno, portname, strerror(errno));
    		return;
   	}
   	else printf ("Serial port opened successfully\n");

   	set_interface_attribs (fd_port, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)

	/*	ff_port = open ("/dev/ttyO1", O_RDWR | O_NOCTTY | O_SYNC);
   	if (ff_port < 0)
   	{
    		printf ("Error %d opening %s: %s", errno, portname, strerror(errno));
    		return;
   	}
   	else printf ("Serial port opened successfully\n");

   	set_interface_attribs (ff_port, B115200, 0); 
*/
   	while(1) pmtActions();

		return 0;
}

//======================================================================================================================


