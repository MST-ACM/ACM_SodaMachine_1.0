#include "sodaMachine.h"

#define BAUDRATE B4800
#define DEVICE "/dev/ttyS0"
#define LOG_NAME "log/vendsoda.log"
#define RETURNINVENTORY_CHAR 'S'
#define RETURNBUTTONPRESS_CHAR 'B'

using namespace std;


/* TODOs, FIXMEs, & NOTEs
 *
 * There are also some TODOs and FIXMEs scattered about. Use your editor
 *  to search for TODO and FIXME
 */
 
/* The anatomy of a program performing serial I/O with the help of
 *  termios is as follows:
 *
 * - Open serial device with standard Unix system call open(2)
 * - Configure communication parameters and other interface
 *    properties (line discipline, etc.) with the help of specific
 *    termios functions and data structures.
 * - Use standard Unix system calls read(2) and write(2) for reading from,
 *    and writing to the serial interface. Related system calls like readv(2)
 *    and writev(2) can be used, too. Multiple I/O techniques, like blocking,
 *    non-blocking, asynchronous I/O (select(2) or poll(2)) or 
 *    signal-drive I/O (SIGIO signal) are also possible.
 * - Close device with the standard Unix system call close(2) when done.
 *
 * The majority of these operations use <termios.h>
 * 
 * (source: http://en.wikibooks.org/wiki/Serial_Programming:Unix/termios )
 */

/* Uncomment the line below if you want to compile this class all by itself */
//int main() { return 0; }


/* Default constructor:
 *  - Sets initComplete to false
 *  - Connects to the serial port (which sets initComplete to true on success)
 *  - Initializes the filestream for logging
 */
sodaMachine::sodaMachine()
{
  initComplete = false;
  vendLog.open(LOG_NAME);
  vendLog << "Constructing a sodaMachine object" << endl;
  serialConnect();
}

/* Destructor:
 *  - Loads the old termios settings
 *  - Closes the logging filestream
 *  - Closes the terminal connection
 */
sodaMachine::~sodaMachine()
{
  vendLog << "Deconstructing a sodaMachine object" << endl;
  vendLog.close();
  tcsetattr(fileDes,TCSANOW,&oldtio);
  close(fileDes);
}

/* int sodaMachine::charToInt( const char input )
 *
 * Returns the integer that the input char would be if it was an integer.
 * FIXME: This is the legacy of the previous version of this program, once
 *   the MCU interactions make more sense, this function should no longer
 *   be necessary.
 *
 * Note that the code with this functionality was originally called "HexToInt"
 *   buy I changed that, as it was misleading.
 *
 *  The old code assumed that the values returned by the MCU were ASCII letters
 *    representing hex. I get the impression that the previous programmer
 *    didn't know how hex worked or something.
 *
 *  If the MCU actually returned character values which we were supposed to
 *    interpret as hex, then the old code solved for that... but why send
 *    characters masquerading as hex in the first place?
 *
 *  To future programmers: If the MCU is returning hex values, you don't need
 *    to convert it. It's all binary to the computer anyway. The value was
 *    was always an integer! If the MCU is returning characters ... stop it.
 *    Or at least stop pretending like sending characters is the same as
 *    sending a hex integer.
 */

const char HEXTABLE[16] =
{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
 
 int sodaMachine::charToInt( const char input )
{
  for( short i = 0; i < 16; i++)
    if( HEXTABLE[i] == input )
      return i;
  return -1;
}

/* int sodaMachine::charToInt( const char msb, const char lsb )
 *
 * Same as function above, just processes two characters.
 */
int sodaMachine::charToInt( const char msb, const char lsb )
{
  short msbInt = charToInt( msb );
  short lsbInt = charToInt( lsb );
  
  return (msbInt == -1 || lsbInt == -1) ? (-1) : (msbInt * 16 + lsbInt);
}

/* sodaMachine::serialConnect(): Opens a serial connection to the MCU
 *   Establishes a connection to the serial port. Currently that location
 *    is defined by DEVICE, and there is currently no option to change this
 *    without recompiling.
 *   If this succeeds, initComplete is set to true.
 */
void sodaMachine::serialConnect()
{
  vendLog << "sodaMachine::serialConnect() called" << endl;
  
  /* Opening a file at DEVICE
   *  The following options will be set:
   *  - O_RDWR: Opens the port for reading and writing
   *  - O_NOCTTY: The port never becomes the controlling
   *      terminal of the process.
   *
   * Then check for errors:
   *  - If successful, open() returns a file descriptor.
   *  - If unsuccessful, open() returns -1 and errno is set
   */
  
  vendLog << "opening a file at DEVICE" << endl;
  
  fileDes = open(DEVICE, O_RDWR | O_NOCTTY );
  
  if (fileDes < 0)
  {
    vendLog << "sodaMachine::serialConnect():"
	        << "Error opening connection to DEVICE, exiting. " << endl;
    exit(EXIT_FAILURE);
  }

  /* Saving the previos port settings in "oldtio"
   *
   * Then check for errors:
   *  - If successful, tcgetattr() returns 0.
   *  - If unsuccessful, tcgetattr() returns -1 and errno is set.
   */
  
  vendLog << "sodaMachine::serialConnect(): Saving old port settings." << endl;
  if( tcgetattr( fileDes, &oldtio ) != 0 )
  {
    vendLog << "sodaMachine::serialConnect():"
	        << "Error saving old port settings, exiting. " << endl;
    exit(EXIT_FAILURE);
  }
  
  /* Setting up a new termios struct "newtio"
   *
   * Setting "newtio"'s flags to the following settings:
   *  Control flags (c_cflag):
   *   - CS8: Sets character size to 8 bits
   *   - CLOCAL: Ignore modem status lines
   *   - CREAD: Enable receiver
   *  Input flags (c_iflag):
   *   - IGNPAR: Ignore characters with parity errors
   *   - IGNBRK: Ignore break condition
   *  Output flags (c_oflag):
   *   x none currently enabled
   *  Local flags (c_lflag):
   *   x none currently enabled
   *  Special input characters (c_cc):
   *   - VTIME = 0: Don't want to use any character timer
   *   - VMIN = 1: Must read 1 character before a read is satisfied
   */
  
  vendLog << "sodaMachine::serialConnect(): setting up new termios struct."
          << endl;
  
  /* Clearing "newtio", just in case */
  memset( &newtio, 0x00, sizeof(newtio) );
  
  /* Setting option flags */
  newtio.c_cflag = CS8 | CLOCAL | CREAD;  // control flags
  newtio.c_iflag = IGNPAR | IGNBRK;       // input flags
  newtio.c_oflag = 0;                     // output flags
  newtio.c_lflag = 0;                     // local flags

  newtio.c_cc[VTIME]    = 0;  // Special input characters
  newtio.c_cc[VMIN]     = 1;
  
  /* Setting input & output baud rate */
  if( cfsetospeed(&newtio, BAUDRATE) != 0 ||
      cfsetispeed(&newtio, BAUDRATE) != 0 )
  {
    vendLog << "sodaMachine::serialConnect():"
	        << "Error setting baud rate, exiting. " << endl;
    exit(EXIT_FAILURE);
  }
  
  /* Flush unsent data:
   *  - TCIFLUSH: Flush input data that has been recieved by the system but
   *     has not yet been processed.
   *
   * Then check for errors:
   *  - If successful, tcflush() returns 0.
   *  - If unsuccessful, tcflush() returns -1 and errno is set
   */
   
  vendLog << "sodaMachine::serialConnect(): Flushing unsent data" << endl;
  
  if ( tcflush(fileDes, TCIFLUSH) != 0 )
  {
    vendLog << "sodaMachine::serialConnect():"
	        << "Error flushing terminal, exiting. " << endl;
    exit(EXIT_FAILURE);
  }
  
  /* Apply changes:
   *  - TCSANOW: Apply changes immediately
   *
   * Then, check for errors:
   *  - If successsful, tcsetattr() returns 0.
   *  - If unsuccessful, tcsetattr() returns -1 and errno is set.
   */
   
  vendLog << "sodaMachine::serialConnect(): Applying new termios settings"
          << endl;
		  
  if ( tcsetattr(fileDes,TCSANOW,&newtio) != 0 )
  {
    vendLog << "sodaMachine::serialConnect():"
	        << "Error saving new port settings, exiting. " << endl;
    exit(EXIT_FAILURE);
  }
  
  /* Modify the file descriptor to make the reads non-blocking */
  fcntl(fileDes, F_SETFL, O_NONBLOCK);
  
  
  /* Finished setting up new serial port connection. Setting initComplete. */
  
  initComplete = true;
  
  /* TODO: Query the MCU and make sure it is connected */

  vendLog << "sodaMachine::serialConnect(): "
          << "Successfully connected. Returning to calling object" << endl;
  
  return;
}

/* int sodaMachine::getSodaInventory()
 * 
 * Currently just returns 0 because I don't know what the MCU returns,
 *  but still contains the code that serialController used just to keep it
 *  around and make sure it compiles.
 *
 * Queries the MCU to return an integer representing the curent soda inventory
 *
 * I honstly have no idea what the return value means. The MCU needs to be
 *   reprogrammed first.
 *
 */
int sodaMachine::getSodaInventory()
{
  vendLog << "sodaMachine::getSodaInventory(): Function called. "
          << "Asserting initComplete" << endl;
  
  assert( initComplete );
  
  const char COMMAND = RETURNINVENTORY_CHAR;
  int readWriteResult;
  char buf[10];

  /* write():
   * If successful, returns the number of bytes written.
   * If unsuccessful, returns -1 & sets errno
   *
   * The second argument MUST be a pointer! That is why I am defining
   *  the command to use, then using a const variable and reference down here.
   */
  
  vendLog << "sodaMachine::getSodaInventory(): Writing command to serial" << endl;
  
  readWriteResult = write( fileDes, &COMMAND, 1);
  
  if( readWriteResult != 3 )
  {
    vendLog << "sodaMachine::getSodaInventory(): Write returned an unexpected "
	          << "value. Expected 3, received " << readWriteResult << endl
			      << "sodaMachine::getSodaInventory(): Exiting" << endl;
	exit( EXIT_FAILURE );
  }
  
  /* read operation:
   * - clear buf
   * - read the serial into buf
   */
  
  vendLog << "sodaMachine::getSodaInventory(): Reading from serial port" << endl;
  
  memset( buf, 0x00, sizeof(buf) );
  
  readWriteResult = read( fileDes, buf, sizeof(buf) );
  
  if( readWriteResult != 3 )
  {
    vendLog << "sodaMachine::getSodaInventory(): read returned an unexpected "
	          << "value. Expected 3, received " << readWriteResult << endl
			      << "sodaMachine::getSodaInventory(): Exiting" << endl;
	  exit( EXIT_FAILURE );
  }

  
  return charToInt(buf[1], buf[2]);
}

/* bool sodaMachine::hasSoda( short slot )
 *
 * - Uses getSodaInventory and checks if a single slot has a soda
 *    by shifting getSodaInventory's return value by <slot> digits
 *    and ANDing it with 0x01
 * 
 * Returns true if the can is present, false if it is not present
 *  or is the slot was out of range.
 */
bool sodaMachine::hasSoda( const unsigned short slot )
{
  bool returnValue = false;
  
  vendLog << "sodaMachine::hasSoda(): Function called." << endl;
  vendLog << "sodaMachine::hasSoda(): Checking for valid slot & soda." << endl;
  
  if( !validSlot( slot ) )
    vendLog << "sodaMachine::hasSoda(): Soda availability requested for a "
	        << "slot outside of the valid range. Expected [0:7], recieved "
			<< slot << endl;
  else
    returnValue = ( getSodaInventory() >> slot ) & 0x01;

  vendLog << "sodaMachine::hasSoda(): Complete. Returning " << returnValue
          << endl;
    
  return returnValue;
}

/* int sodaMachine::getButtonInput( time_t timout )
 *
 * Summary of instructions:
 *  - write COMMAND to fileDes, check for errors
 *  - spend the rest of the time waiting for a read, check for
 *      errors and return the read value
 *  - Returns number of pressed button if a button is pressed within the
 *      timeout period. Otherwise, returns -1
 */
int sodaMachine::getButtonInput( const time_t timeout = 10 )
{	  
  time_t end_time = time(NULL) + timeout;
  
  const char COMMAND = RETURNBUTTONPRESS_CHAR;
  int pressedButton = -1;
  int readWriteResult;
  
  vendLog << "sodaMachine::getButtonInput(): called with timeout of "
          << timeout << " seconds" << endl;
  if( timeout > 60 || timeout < 1)
  {
    vendLog << "sodaMachine::getButtonInput(): timeout too large. "
	        << "Exiting." << endl;
	exit( EXIT_FAILURE );
  }

  vendLog << "sodaMachine::getButtonInput(): Asserting initComplete" << endl;
  assert( initComplete );
  
  vendLog << "sodaMachine::getButtonInput(): Writing command to serial"
          << endl;
  if ( (readWriteResult = write( fileDes, &COMMAND, 1 ) ) != 1 )
  {
      vendLog << "sodaMachine::getButtonInput(): write() return an unexpected "
	          << "value. Expected 1, recieved " << readWriteResult
			  << ". Exiting" << endl;
	  exit( EXIT_FAILURE );
  }
  
  vendLog << "sodaMachine::getButtonInput(): Entering read loop" << endl;
  while( time( NULL ) < end_time && read( fileDes, &pressedButton, 1) != 1 )
  {
    if( (readWriteResult = read( fileDes, &pressedButton, 1) ) != 1 )
    {
    vendLog << "sodaMachine::getButtonInput(): read returned an unexpected "
	        << "value. Expected 1, received " << readWriteResult << endl
			<< "sodaMachine::serialConnect(): Exiting" << endl;
	exit( EXIT_FAILURE );
    }
  }
  
  if( time( NULL ) >= end_time )
  {
    vendLog << "sodaMachine::getButtonInput(): Loop timed out. Returning "
	        << pressedButton << endl;
  }
  else
  {
    vendLog << "sodaMachine::getButtonInput(): Answer recieved within the "
	        << "timeout period. Returning " << pressedButton << endl;
  }
  
  // FIXME: I need to know more about the 89C51's responses first
  return pressedButton;
}

/* int sodaMachine::vendSoda( short slot )
 *
 * Tells the MCU to vend the can in slot number <slot>
 *
 * Returns 0 if successful, 1 if the stack is empty, and -1 on some other error
 * FIXME: needs error checking for reads and writes
 * FIXME: the MCU probably needs to be sent one character at a time,
 *  this should be rewritten to wait for a confirmation character or something
 */
int sodaMachine::vendSoda( const unsigned short slot )
{
  char CommandBuffer[10];
  int vendResult = -1;
  int readWriteResult;

  vendLog << "sodaMachine::vendSoda(): Function called with input" << slot
          << " Asserting initComplete" << endl;
		  
  assert( initComplete );

  /* Validating slot number */
  
  vendLog << "sodaMachine::vendSoda(): Validating slot number" << endl;
  
  if( !validSlot( slot ) )
    vendLog << "sodaMachine::vendSoda(): Slot value outside of range" << endl;
  if( !hasSoda( slot ) )
  {
    vendResult = 1;
    vendLog << "sodaMachine::vendSoda(): Slot " << slot << " is empty. "
	        << "Set return value to " << vendResult << endl;
  }
  else
  {
    vendLog << "sodaMachine::vendSoda(): Slot is valid & has soda, vending"
	        << endl;

    /* Sending vend signal to machine using a
     *  two-byte string: "V#" where # is the integer "slot"
     *
     * - Inserts "V<slot>" into the command buffer
     * - Writes the command buffer to the serial port
     */

	vendLog << "sodaMachine::vendSoda(): Writing commands to serial" << endl;
    CommandBuffer[0] = 'V';
    CommandBuffer[1] = slot;

    if( (readWriteResult = write( fileDes, &CommandBuffer, 2 ) ) != 2)
	{
      vendLog << "sodaMachine::vendSoda(): Write returned an unexpected "
	          << "value. Expected 2, received " << readWriteResult << endl
			  << "sodaMachine::vendSoda(): Exiting" << endl;
	  exit( EXIT_FAILURE );
	}

    /* Verify that a vend took place:
     *  - Check that the microcontroller sent a 'Y'
     */
    vendLog << "sodaMachine::vendSoda(): Verifying that a vend took place"
	        << endl;
    if( (readWriteResult = read( fileDes, &CommandBuffer[2], 1 ) ) != 1)
	{
      vendLog << "sodaMachine::vendSoda(): Read returned an unexpected "
	          << "value. Expected 1, received " << readWriteResult << endl
			  << "sodaMachine::vendSoda(): Exiting" << endl;
	  exit( EXIT_FAILURE );
	}  
    if ( CommandBuffer[2] == 'Y')
	  vendResult = 0;
  }
  
  vendLog << "sodaMachine::vendSoda(): Reached end of function. Returning "
          << vendResult << endl;
  
  return vendResult;
}
