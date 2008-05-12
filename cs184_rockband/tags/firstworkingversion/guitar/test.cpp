
#include <iostream>
#include "Guitar.h"

using namespace std;

int main() {
	Guitar guitar;
	guitar.connect("/dev/ttyUSB0");
	if (!guitar.isConnected()) {
		cout << "Error! Could not connect to guitar!" << endl;
		exit(1);	
	}
	cout << "Connected to Guitar" << endl;
	
	guitar.invalidate();
	guitar.keyG = true;
	guitar.writeState();
	sleep(1);
	
	guitar.invalidate();
	guitar.keyR = true;
	guitar.writeState();
	sleep(1);
	
	guitar.invalidate();
	guitar.keyY = true;
	guitar.writeState();
	sleep(1);
	
	guitar.invalidate();
	guitar.keyB = true;
	guitar.writeState();
	sleep(1);
	
	guitar.invalidate();
	guitar.keyO = true;
	guitar.writeState();
	sleep(1);
	
	guitar.invalidate();
	guitar.writeState();
	
	
	guitar.disconnect();
	cout << "Disconnected from Guitar" << endl;
}
