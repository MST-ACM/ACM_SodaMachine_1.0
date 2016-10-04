#ifndef SODAMACHINE
#define SODAMACHINE

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>

using namespace std;

/******************************************************************************\
 * sodaMachine class: Provides the interface between the other programs
 *                    and the soda machine's MCU.
 *
 * Functions:
 *
 * - void serialConnect()
 *       Sets up a connection with the MCU via serial port.
 *
 * - int getSodaInventory()
 *       Returns an int representing inventory. If that number
 *       is displayed in binary, 1 = yes, 0 = no.
 *       Currently returns 0 always because I don't know what the MCU
 *       returns yet.
 *
 * - bool hasSoda( const unsigned short slot )
 *       Checks if a single slot has soda.
 *       Returns true if the slot contains soda, returns false if it doesn't.
 *
 * - int vendSoda( const unsigned short slot )
 *       Vends a soda. Returns 0 if that is successful, 1 if that slot is
 *       empty, and -1 on any other error.
 *
 * - int getButtonInput( time_t timeout )
 *       Returns the number of the first button that is pressed during
 *       the timeout period.
 *       Returns -1 if the timeout expires before an input is read.
 *
 * - inline const bool validSlot ( const short slot )
 *       Returns true if the number is in the interval [0, 7].
 *
 * Variables:
 *
 * - int fileDes
 *       Holds the file descriptor of the serial port connection opened
 *       in serialConnect().
 *
 * - bool initComplete
 *       Holds whether or not a serial connection has been intialized
 *       successfully.
 *
 * - termios oldtio
 *       Holds the old terminal IO settings that are overwritten when
 *       serialConnect makes a new connection.
 *
 * - termios newtio
 *       Holds the new terminal IO settings that serialConnect uses.
 \*****************************************************************************/

class sodaMachine
{
  public:
    sodaMachine();
    ~sodaMachine();
	
    int getSodaInventory();
    int getButtonInput( time_t timeout );
    bool hasSoda( const unsigned short slot );
    int vendSoda( const unsigned short slot );
    
  private:
    void serialConnect();
	  inline bool validSlot ( const short slot ) const
	    { return( slot >= 0 && slot <= 7 ); };
    
    int charToInt( const char input );
    int charToInt( const char msb, const char lsb );
    
    int fileDes;
    bool initComplete;
    
    
	ofstream vendLog;
    termios oldtio;
    termios newtio;
    
};

#endif
