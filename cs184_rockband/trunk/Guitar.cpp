#include "Guitar.h"


Guitar::Guitar() {
	socketd = -1;
	invalidate();
}

Guitar::~Guitar() {
	disconnect();
}

void Guitar::disconnect() {
	if (isConnected())
		close(socketd);	
}

/**
 * Attempts to open the serial connection and establish Arduino communication.
 */
bool Guitar::connect(std::string filed) {
	
	if (isConnected())
		return false;
	
	socketd = open(filed.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (socketd == -1)
		return false;
	else
		fcntl(socketd, F_SETFL, 0);
	
    struct termios options;

    tcgetattr(socketd, &options);				//Get the current options for the port...

    cfsetispeed(&options, B9600);			//Set the baud rates to 9600...
    cfsetospeed(&options, B9600);			//Set the baud rates to 9600...

    options.c_cflag |= (CLOCAL | CREAD); 	// Enable the receiver and set local mode...
    options.c_cflag &= ~CSIZE; 				// Mask the character size bits
    options.c_cflag |= CS8;    				// Select 8 data bits
    

    tcsetattr(socketd, TCSANOW, &options); 		//Set the new options for the port...
	
	return true;
}

/**
 * True if connected to Arduino.
 */
bool Guitar::isConnected() {
	return (socketd != -1);
}

/**
 * Returns the current state as a Byte to send to Arduino.
 */
unsigned char Guitar::toByte() {
	unsigned char out = 0;
    if (keyG)
      out += 1;
    if (keyR)
      out += 2;
    if (keyY)
      out += 4;
    if (keyB)
      out += 8;
    if (keyO)
      out += 16;
    if (keyPickDown)
      out += 32;
    if (keyPickUp)
      out += 64;  
    return out;
}

bool Guitar::writeState() {
	if (!isConnected())
		return false;
	unsigned char out = toByte();
	int n = write(socketd, &out, 1);
    if (n < 0)
      return false;
    return true;	
}

void Guitar::invalidate() {
	keyG = keyR = keyY = keyB = keyO = keyPickDown = keyPickUp = false;
}
