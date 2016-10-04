/* sodaCommand.cpp
 *
 * Provides an argument-based and command-line-based way to control
 * the main functions of the soda machine.
 *
 * 
 *
 * 
 *
 */

#include <iostream>
#include <unistd.h> // for getopt()
#include "sodaMachine.h"

using namespace std;

void showButtonInput( sodaMachine &acmSoda );
void showSodaInventory( sodaMachine &acmSoda );
void vendSoda( sodaMachine &acmSoda );
void showSingleSoda( sodaMachine &acmSoda );


int main(int argc, char *argv[])
{
  char option;
  int menuChoice;
  sodaMachine acmSoda;
  
  if(argc > 1)
  {  
    while( ( option = getopt(argc, argv, "bivs") ) != -1 )
    {
      switch( option )
      {
        case 'b':
          showButtonInput( acmSoda );
          break;
	    case 'c':
	      showSodaInventory( acmSoda );
          break;
        case 's':
          showSingleSoda( acmSoda );
          break;
        case 'v':
          vendSoda( acmSoda );
          break;
        default:
          cout << "Usage: sodaCommand [OPTIONS]" << endl
               << "     -b                       Read the buttons" << endl
               << "     -i                       Check soda inventory" << endl
               << "     -s                       Check for a single can" << endl
               << "     -v                       Vend a soda" << endl
               << "     -h                       See this message" << endl
               << "Alternately enter no inputs for a menu interface" << endl;
          break;
      }
    }
    
    cout << "Finished reading inputs." << endl;
  }
  else
  {
    while( true )
    {
      cout  <<"* Please enter an Option *" << endl
            <<"1 Read the Buttons"         << endl
            <<"2 Check the soda inventory" << endl
            <<"3 Check for a single can"   << endl
            <<"4 Vend a Soda"              << endl
            <<"5 Exit"                     << endl
            <<"> ";
          
      cin >> menuChoice;
      
      if( menuChoice == 1 )
        showButtonInput( acmSoda );
      else if( menuChoice == 2 )
        showSodaInventory( acmSoda );
      else if( menuChoice == 3 )
        showSingleSoda( acmSoda );
      else if( menuChoice == 4 )
        vendSoda( acmSoda );
      else
        break;
    }
  }
  
  cout << "Exiting." << endl;
  return 0;
}

/* showButtonInput: Terminal interface for sodaMachine::getButtonInput */
void showButtonInput( sodaMachine &acmSoda )
{
  int timeout;
  int buttonPress;
  
  cout << "How long would you like to wait for a choice? (seconds)" << endl
       << "Leave blank for the default timeout of 10 seconds." << endl
	   << "> ";
  
  cin >> timeout;
  
  cout << "Now waiting for user input on the soda machine ..." << endl;
  
  if ( ( buttonPress = acmSoda.getButtonInput(timeout) ) == -1)
  {
    cout << "getButtonInput timed out or the timeout given was outside"
	     << " the valid range of [1, 60] seconds." << endl;
  }
  else
  {
    cout << "getButtonInput returned " << buttonPress << endl;
  }
  
  cout << "Finished getting user soda choice input." << endl;
}


/* showSodaInventory: Terminal interface for sodaMachine::getSodaInventory */
void showSodaInventory( sodaMachine &acmSoda )
{
  int sodaInventory;
  
  cout << "Checking the soda machine's inventory now ... " << endl;
			 
  sodaInventory = acmSoda.getSodaInventory();
      
  cout << "Integer output of getSodaInventory:" << sodaInventory << endl
       << "Formatted for human convenience: ";
      
  for( short i = 0; i < 8; ++i )
  {
    cout << ( ( sodaInventory & 0x01 ) ? 'X' : '0' );
    sodaInventory >>= 1;
  }
  cout << endl;
		
  cout << "Finished checking the machine's inventory." << endl;
}

/* vendSoda: Terminal interface for sodaMachine::vendSoda */
void vendSoda( sodaMachine &acmSoda )
{
  int slot;
  int vendResult;
  
  cout << "Which slot would you like to vend from? (integer)" << endl
       << "> ";
  cin >> slot;
  cout << "Vending a soda ..." << endl;

  vendResult = acmSoda.vendSoda( slot );

  if( vendResult == -1 )
	cout << "vendSoda returned -1: unknown error." << endl;
  else if( vendResult == 0 )
    cout << "vendSoda returned 0: success" << endl;
  else if( vendResult == 1 )
	cout << "vendSoda returned 1: stack is empty" << endl;
  else
    cout << "vendSoda returned an unexpected value." << endl;
	  
  cout << "Finished vending a soda." << endl;
}

/* showSingleSoda: Terminal interface for sodaMachine::hasSoda */
void showSingleSoda( sodaMachine &acmSoda )
{
  int slot;
  
  cout << "What slot would you like to check? (integer)" << endl
       << "> " << endl;
  cin >> slot;
  cout << "Checking slot " << slot << " for soda ..." << endl;
		
  if( acmSoda.hasSoda( slot ) )
    cout << "That slot has soda." << endl;
  else
    cout << "That slot does not have soda, or an error occured." << endl;

  cout << "Finished checking for a single soda." << endl;
}
