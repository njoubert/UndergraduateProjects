// Main.cpp - Created with DR&E AppWizard 1.0B10
// ------------------------------------------------------------------------------------
// Copyright © 1999 Intel Corporation
// All Rights Reserved
// 
// Permission is granted to use, copy, distribute and prepare derivative works of this 
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  This software is provided "AS IS." 
//
// Intel specifically disclaims all warranties, express or implied, and all liability,
// including consequential and other indirect damages, for the use of this software, 
// including liability for infringement of any proprietary rights, and including the 
// warranties of merchantability and fitness for a particular purpose.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
// ------------------------------------------------------------------------------------
//
//  PURPOSE:
//    
//  defines for what is in main.cpp
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
///////////////////////////////////////////////////////////////////////////////////////
#define FPSUPDATETIME		1.0f					// Number of milliseconds between FPS counter updates


BOOL InitWindow(HINSTANCE hInst, int nCmdShow);

// Handles all the windows messages we might receive
long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);

// Initializes all that windows junk, creates class then shows main window
BOOL InitWindow(HINSTANCE hInst, int nCmdShow);



