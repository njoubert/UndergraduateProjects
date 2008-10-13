#if defined(_MSC_VER)
   #pragma warning(disable:4786 4666)
#endif

//#include "stdafx.h"

#include <windows.h> 

#include <io.h>
#include <fcntl.h>

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <iosfwd>

struct IGStreams {
	//basic_filebuf<char, char_traits<char> > *pbufout, *pbuferr, *pbufin;
	std::filebuf *pbufout, *pbuferr, *pbufin;
	std::ostream *pigout, *pigerr;
	std::istream *pigin;
};

IGStreams igs;

int 
activateConsole() {
  int hCrt, hCrtIn;   

  AllocConsole();   
  hCrt = _open_osfhandle(
          (long) GetStdHandle(STD_OUTPUT_HANDLE),             
	  _O_TEXT
          );
	
  hCrtIn = _open_osfhandle(
          (long) GetStdHandle(STD_INPUT_HANDLE),             
	  _O_TEXT
          );   


	FILE* fho = fdopen(hCrt, "w");
	FILE* fhi = fdopen(hCrtIn, "r");

  igs.pbufout = new std::filebuf(fho/*hCrt*/);
  igs.pigout = new std::ostream(igs.pbufout);
  if(!igs.pigout->good())
    return 0;
	//std::cout = *igs.pigout;
  memcpy((void*)&std::cout, igs.pigout, sizeof(std::ostream));

  igs.pbuferr = new std::filebuf(fho/*hCrt*/);
  igs.pigerr = new std::ostream(igs.pbuferr);
  if(!igs.pigerr->good())
    return 0;
  //std::cerr = *igs.pigerr;
  memcpy((void*)&std::cerr, igs.pigerr, sizeof(std::ostream));

  igs.pbufin = new std::filebuf(fhi/*hCrtIn*/);
  igs.pigin = new std::istream(igs.pbufin);
  if(!igs.pigin->good())
    return 0;
	//std::cin = *igs.pigin;
  memcpy((void*)&std::cin, igs.pigin, sizeof(std::istream));

  return 1;
}

int SetupConsole()
{
    // stuff from knowledge base Q105305 (see that for details)
    // open a console and do the work around to get the console to work in all
    // cases
    int hCrt;
    FILE *hf;
    const COORD screenSz = {80, 5000}; /* size of console buffer */
            
    AllocConsole();
    
    hCrt = _open_osfhandle(
            (long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    if (hCrt!=-1) hf = _fdopen(hCrt, "w");
    if (hf!=0) *stdout = *hf;
    if (hCrt==-1 || hf==0 || 0!=setvbuf(stdout, NULL, _IONBF, 0)) {
            //ShowMessage(MB_ICONINFORMATION,
              //          "unable to mount reroute stdout");
            return FALSE;
    }
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), screenSz);
    
    hCrt = _open_osfhandle(
        (long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    if (hCrt!=-1) hf = _fdopen(hCrt, "w");
    if (hf!=0) *stderr = *hf;
    if (hCrt==-1 || hf==0 || 0!=setvbuf(stderr, NULL, _IONBF, 0)) {
            //ShowMessage(MB_ICONINFORMATION,
            //            "reroute stderr failed in SetupConsole");
            return FALSE;
    }
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), screenSz);
    
    hCrt = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
    if (hCrt!=-1) hf = _fdopen(hCrt, "r");
    if (hf!=0) *stdin = *hf;
    if (hCrt==-1 || hf==0 || 0!=setvbuf(stdin, NULL, _IONBF, 0)) {
            //ShowMessage(MB_ICONINFORMATION,
             //           "reroute stdin failed in SetupConsole");
            return FALSE;
    }
    return TRUE;
}
           
