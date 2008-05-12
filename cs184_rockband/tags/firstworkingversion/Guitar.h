#ifndef GUITAR_H_
#define GUITAR_H_

#include <string>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/**
 * The Guitar class for CS184 Rock Band Project.
 * 
 * This class provides the interface to the hardware, and maintains the Guitar's state.
 * 
 */
class Guitar {
public:
	Guitar();
	virtual ~Guitar();
	void disconnect();
	bool connect(std::string filed);
	bool isConnected();
	bool writeState();
	void invalidate();
	bool keyG, keyR, keyY, keyB, keyO, keyPickDown, keyPickUp; 	//Stores current state of buttons
	
private:
	int socketd;
	unsigned char toByte();
	
};

#endif /*GUITAR_H_*/
