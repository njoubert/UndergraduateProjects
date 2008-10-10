#include "myTimer.h"

void CMyTimer::Start() {
   #ifdef _MSC_VER
   lastTime = GetTickCount();
	#else
	gettimeofday(&lastTime, NULL);
	#endif 	
}

// Return elapsed time since start in seconds
real CMyTimer::Stop() { 
	real dt;
   #ifdef _MSC_VER
      DWORD currentTime = GetTickCount();
		dt = (real)(currentTime - lastTime)*0.001f; 
	#else
		timeval currentTime;
		gettimeofday(&currentTime, NULL);
		dt = (real)((currentTime.tv_sec - lastTime.tv_sec) + 1e-6*(currentTime.tv_usec - lastTime.tv_usec));
	#endif
	return dt;
}
