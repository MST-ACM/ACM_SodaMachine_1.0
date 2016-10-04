#include <iostream>
#include <termios.h>

using namespace std;

int main()
{
  /* Opening the serial port */
  
  int fileDes = open("/dev/ttyS0", O_RDWR | O_NOCTTY );
  
  if (fileDes < 0)
  {
    perror( "Error establishing connection with open()" );
    exit(1);
  }
  
  if( flock( fileDes, LOCK_EX | LOCK_NB) != 0 )
  {
    perror( "Error locking serial port connection with flock()");
    exit(1);
  }
  
  struct termios termAttribs;
  
  termAttribs.c_cflag = CS8 | CLOCAL | CREAD; // control flags
  termAttribs.c_iflag = IGNPAR | IGNBRK;                 // input flags
  termAttribs.c_oflag = 0;                               // output flags
  termAttribs.c_lflag = 0;                               // local flags

  termAttribs.c_cc[VTIME]    = 0;   // special input
  termAttribs.c_cc[VMIN]     = 1;   //  characters
  
  
  if( cfsetospeed(&termAttribs, BAUDRATE) != 0 || // setting input & output baud
      cfsetispeed(&termAttribs, BAUDRATE) != 0 )
  {
    perror( "Error setting serial port baud rate" );
    exit(1);
  }
  
  if ( tcsetattr(fd,TCSANOW,&newtio) != 0 )
  {
    perror( "Error finalizing serial port attributes" );
    exit(1);
  }
  
  /* Finished opening serial port */
  
  /* Gathering info about the microcontroller reponses */
  
  