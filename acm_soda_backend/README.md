ACM Soda Machine - Backend
==========================

Software to operate the soda machine in the Computer Science building lounge.

This code provides the sodaMachine class which handles communication and
control of the soda machine by interacting with an Atmel 89C51 MCU via serial.

This code also provides the sodaCommand program, which uses the sodaMachine
class to control the soda machine and perform a few basic functions like
checking inventory and vending a soda.

Unit tests for the sodaMachine class are currently being implemented.
SodaCommand is not intended to test the class itself, but to give a tool to
play around with the soda machine hardware and control the machine without
bothering with something more complicated.

Currently, the backend is structured like this:

     +------------------+
     |   sodaCommand,   |
     |   sodaDaemon     |
     +------------------+
             |
    +---------------------+
    |  sodaMachine class  |
    +---------------------+
             |
        +----------+
        |   MCU    |      
        +----------+
             |
    +--------------------+
    |   Soda machine     |
    |     hardware       |
    +--------------------+

### Classes

 sodaMachine: Handles all interaction of the rest of the program with the
  soda machine via the MCU. Allows programs to:
     - Check availability of cans: Whole inventory or single cans
	 - Vend a can
	 - Retrieve input from the buttons on the front of the machine

   
### Programs
	
  sodaDaemon: "Front-end of the back-end": reads instructions from pipes and
    uses the sodaMachine class to implement those instructions.
     - Runs as a background process
     - Reads a vending slot from a pipe and sends the corresponding vend
      instruction to the MCU using serialController
	  
  sodaCommand: Controls the soda machine via arguments or a console menu for
    testing or experimentation purposes. NOT meant for testing the software.
	Write unit tests if you want that.
   
  
Note from the previous programmer:
After a hard reboot, ensure the /tmp files are deleted. Then start the daemon.