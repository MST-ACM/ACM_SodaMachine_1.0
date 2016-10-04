/* sodaDaemon.cpp
 *
 * Spawns a daemon process that uses pipes to fetch vending information
 *  and send it to the microcontroller. Can only vend a soda. Cannot
 *  receive any data from the MCU
 *
 */

#include <fstream>    // stream functions
#include <cstdlib>    // atoi(), exit()
#include <sys/stat.h> // chmod()
#include <unistd.h>   // fork()

#include "sodaMachine.h"

#define PIPE_IN_NAME "pipes/vendsodain"
#define PIPE_OUT_NAME "pipes/vendsodaout"
#define LOG_NAME "log/vendsoda.log"

using namespace std;

int main()
{
  char slotChoice[256];
  fstream vendPipeIn;
  fstream vendPipeOut;

  //pid_t processID, sessionID;
  
  bool vendSuccess;
  
  /* Spawn the daemon process and kill the parent
   *
   * If successful, fork():
   *  - returns 0 to the child process
   *  - returns the process ID of the newly created child to the parent
   *
   * If unsuccessful, fork():
   *  - fails to create a child process
   *  - returns a value of -1 (to the parent)
   *  - sets errno 
   *
   * TODO: There is no error checking implemented at the moment. This is the bare
   *  minimum to spawn a child process then kill the parent.
   * For error checking: instead of pid != 0, use pid < 0 and pid > 0 to
   *  separate the two cases.
   */
  if (fork() != 0)
    exit(0);
  
  /* Set up file IO options
   *
   * Error checking:
   * - umask(): has no error condition
   * - setsid():
   *   - If succesful, returns a session ID
   *   - If unsuccessful, returns -1  & sets errno
   * - chdir():
   *   - If successful, returns 0
   *   - If unsuccessful, returns -1 & sets errno
   *
   * Standard IO locations are closed for security reasons, since this
   *  process is now a daemon and needs no IO.
   */
  umask(0);
  
  setsid(); 
  
  chdir("/");
  
  // TODO: close stdin/stdout/stderr or redirect them; for security reasons

  /* Delete old pipes if they exist:
   *
   * If successful: remove() returns 0
   * If unsuccessful: remove() returns -1 and sets errno
   *  - returns -1 if the file does not exist!
   */
  remove(PIPE_IN_NAME);
  remove(PIPE_OUT_NAME);


  /* Making pipes with the following permissions:
   *  - S_IRWXU - Gives read, write, search, and execute permissions
   *     to the file owner -- the user who started this daemon.
   *
   * using chmod() to change the mode of the pipe:
   *  - S_IRWXU - Same as above (FIXME: pointlessly redundant?)
   *  - S_IROTH - Read permission for users other than the file owner.
   *
   * If successful: mkfifo() and chmod() return 0
   * If unsuccessful: mkfifo() and chmod() return -1 & set errno
   */
  mkfifo(PIPE_IN_NAME, S_IRWXU);
  chmod(PIPE_IN_NAME, S_IRWXU|S_IWOTH);

  mkfifo(PIPE_OUT_NAME, S_IRWXU);
  chmod(PIPE_OUT_NAME, S_IRWXU|S_IROTH);
  
  
  // create a connection with the microcontroller
  sodaMachine acmSoda;


  /* Main loop:
   *
   * - Opens a stream to PIPE_IN_NAME
   *   - fstream::in: only inputs allowed from this stream
   * - Fetches slotChoice from this stream
   *
   * - Sends slotChoice to acmSoda.vendSoda
   *
   * - Opens a stream to PIPE_OUT_NAME
   *   - fstream::out: only outputs allowed to this stream
   * - Writes the return value of vendSoda to this stream
   *
   * - Closes the streams
   */
  while(1)
  {
    /* Fetch slot choice */
    vendPipeIn.open(PIPE_IN_NAME, fstream::in);
    vendPipeIn.getline(slotChoice, 256);

    /* Vend the can */
    vendSuccess = acmSoda.vendSoda(atoi(slotChoice));
    
    /* Write the result of the vend operation */
    
    vendPipeOut.open(PIPE_OUT_NAME, fstream::out);
    vendPipeOut << vendSuccess;

    /* Close streams */
    vendPipeIn.close();
    vendPipeOut.flush();
    vendPipeOut.close();
    
    // TODO: figure out how long to sleep()
  }
  return 0;
}
