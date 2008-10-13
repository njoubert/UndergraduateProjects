#ifndef __MYTIMER_H__
#define __MYTIMER_H__

#include "globals.h"
#if defined(_MSC_VER)
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

class CMyTimer{
public:


   #if defined(_MSC_VER)
   DWORD lastTime;
	#else
	struct timeval lastTime;
	#endif

	void Start(); // Start the timer 
	real Stop();  // Return elapsed time since start in seconds

};

#endif