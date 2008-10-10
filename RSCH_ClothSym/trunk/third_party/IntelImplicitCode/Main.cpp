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
//  Implementation of Winmain and message handler. Some stuff related to the embossing 
//  demo is in here (help text, etc)
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
///////////////////////////////////////////////////////////////////////////////////////

#include "DRGShell.h"
#include "main.h"

#pragma comment(lib,"user32")
#pragma comment(lib,"gdi32")
#pragma comment(lib,"ddraw")
#pragma comment(lib,"dxguid")
#pragma comment(lib,"d3dxof") //we use one of the DXframeworks functions for loading .x files
							  // this will be replaced by a derived Loader class in the future
#pragma comment(lib,"winmm")

///////////
// Globals
HINSTANCE g_hInst = NULL;       // Instance of this app
HWND g_hWnd  = NULL;			// Handle of the window

int g_iDeviceNum = -1;
int g_iDevices;
int g_iModes;

// Frame rate stuff 
int g_iFrameCount = 1;
DWORD g_dwLastTime = 0;
DWORD g_dwCurTime = 0;
DWORD g_dwFps = 0;

bool g_bActive;				// flag to track whether app is active
bool g_bFullscreen;			// flag for toggling full-screen off and on
bool g_bShowSampleHelp;
bool g_bPauseAnimation;

//globals initing classes
CDRGWrapper *g_pDRGWrapper;		// the DRG wrapper class
CClothSample *g_pClothSample;             // the app class where you stick your app-specific stuff
CDRGTexture *g_pCFontTexture;	// instance of the texture class that will hold the font texture
								// used by the text class below.
CDRGText *g_pCMainText;			// instance of the text class that will hold all the strings printed
								// by main.cpp on every frame (for now, that is FPS)
CDRGText *g_pCTitleText;			// instance of the text class that will hold all the strings printed
								// by main.cpp on every frame (for now, that is FPS)
CDRGText *g_pCHelpText;			// instance of the text class that will hold all the strings printed
								// by main.cpp on every frame (for now, that is FPS)



// Functions

// Handles all the windows messages we might receive
long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
	int i, iID;

    // Handle messages
    switch (message) 
	{
		// We'll place a message handler here for keypresses until we add some DIrectInput stuff
		case WM_KEYDOWN:			
			switch(wParam) 
			{
				case 'H':
					g_bShowSampleHelp = !g_bShowSampleHelp;
					break;
				case 'X':	//place your code here for what to do if X is pressed (exit for now).
					DestroyWindow( hWnd );
					break;
				case 'F':	// Toggle between full screen and windowed
                    if( g_bActive && g_pDRGWrapper && g_pClothSample )
					{
						g_bActive = false;
						g_bFullscreen = !g_bFullscreen;
						hr = g_pDRGWrapper->RefreshMode( g_bFullscreen );
						if( FAILED( hr ) )
						{
							DisplayErrorMessage( hr );
							DestroyWindow( hWnd );
						}
						else if( g_pDRGWrapper->Initialized() )
						{
                            hr = g_pClothSample->TargetChanged();
							g_pCFontTexture->Restore(g_pDRGWrapper->m_pD3DDevice7);

							if( FAILED( hr ) )
							{
								DisplayErrorMessage( hr );
								DestroyWindow( hWnd );
							}
							else
								g_bActive = true;
						}
					}
					break;
				case 'P':
					{
						g_bPauseAnimation = !g_bPauseAnimation;
					}
					break;
				
			}
			//now that keypress handled here, pass to the app class
			g_pClothSample->KeyDown(wParam);
			break;		
		case WM_KEYUP:
			//In this file, we aren't using any key releases, but we'll call 
			// the KeyUp in the app class in case the user needs it
			g_pClothSample->KeyUp(wParam);
			break;
		//We're not using the mouse here, but we'll pass it to the app
		case WM_MOUSEMOVE:
			if( (g_pDRGWrapper) && (g_pDRGWrapper->Initialized()) )
				g_pClothSample->MouseMove(wParam,lParam);
			break;

		case WM_LBUTTONDOWN:
			if( (g_pDRGWrapper) && (g_pDRGWrapper->Initialized()) )
				g_pClothSample->MouseLeftDown(wParam,lParam);
			break;
		
		case WM_LBUTTONUP:
			if( (g_pDRGWrapper) && (g_pDRGWrapper->Initialized()) )
				g_pClothSample->MouseLeftUp(wParam,lParam);
			break;

		case WM_RBUTTONDOWN:
			if( (g_pDRGWrapper) && (g_pDRGWrapper->Initialized()) )
				g_pClothSample->MouseRightDown(wParam,lParam);
			break;
		
		case WM_RBUTTONUP:
			if( (g_pDRGWrapper) && (g_pDRGWrapper->Initialized()) )
				g_pClothSample->MouseRightUp(wParam,lParam);
			break;

		case WM_COMMAND:
			//
			// Because the menus are dynamically created, we have to check
			// the devices and modes individually
			//
			iID = LOWORD( wParam );

			// If the FILE->EXIT menu option was chosen then exit the program
			if( iID == IDM_FILE_EXIT)
			{
				DestroyWindow( hWnd );
			}
			// This next selection is for the device selection
			if( (iID >= IDM_DEVICE_DEVICE0) && (iID < IDM_DEVICE_DEVICE0 + g_iDevices ) )
			{
				if( iID != g_iDeviceNum + IDM_DEVICE_DEVICE0 )
				{
					g_iDeviceNum = iID - IDM_DEVICE_DEVICE0;

					HMENU hMenu = GetMenu( g_hWnd );
					hMenu = GetSubMenu( hMenu, 1 );
					for( i=0; i<g_iDevices; i++ )
						CheckMenuItem( hMenu, IDM_DEVICE_DEVICE0+i, MF_BYCOMMAND | ((g_iDeviceNum == i) ? MF_CHECKED : MF_UNCHECKED ) );
                    if( g_bActive && g_pDRGWrapper && g_pClothSample )
					{
						g_bActive = false;
						hr = g_pDRGWrapper->m_pD3D7->EvictManagedTextures();

						hr = g_pDRGWrapper->ChangeDevice( g_iDeviceNum );
						if( FAILED( hr ) )
						{
							DisplayErrorMessage( hr );
							DestroyWindow( hWnd );
						}
						else if( g_pDRGWrapper->Initialized() )
						{
                            hr = g_pClothSample->TargetChanged();
							g_pCFontTexture->Restore(g_pDRGWrapper->m_pD3DDevice7);

							if( FAILED( hr ) )
							{
								DisplayErrorMessage( hr );
								DestroyWindow( hWnd );
							}
							else
								g_bActive = true;
						}
					}
				}
			}

			// This next section handles the Mode menu selections
			if ((iID >= IDM_MODE_WINDOWED) &&(iID <= IDM_MODE_WINDOWED + g_iModes))
			{
				if( g_bActive && g_pDRGWrapper && g_pClothSample )
				{
				
					//if we were fullscreen, and switching to windowed
					if (((iID == IDM_MODE_WINDOWED) || (iID == IDM_MODE_WINDOWED+1))  && g_bFullscreen)
					{
						g_bActive = false;

						g_bFullscreen = false;
					
						hr = g_pDRGWrapper->RefreshMode(g_bFullscreen);

						if( FAILED( hr ) )
						{
							DisplayErrorMessage( hr );
							DestroyWindow( hWnd );
						}
						else
							g_bActive = true;
					}
						
					//if we are switching to a full screen mode
					if ((iID > IDM_MODE_WINDOWED+1) &&(iID <= IDM_MODE_WINDOWED + g_iModes))
					{
						g_bActive = false;
						g_bFullscreen = true;

						D3DFSMode	*pTempmode = g_pDRGWrapper->GetMode(g_iDeviceNum,(iID - IDM_MODE_WINDOWED -1));

						hr = g_pDRGWrapper->InitMode(g_iDeviceNum,g_bFullscreen,
							pTempmode->dwWidth,
							pTempmode->dwHeight,
							pTempmode->dwBpp,
							g_pDRGWrapper->m_dwBackBufferCount,
							g_pDRGWrapper->m_bZBuffer);
						if( FAILED( hr ) )
						{
							DisplayErrorMessage( hr );
							DestroyWindow( hWnd );
						}
						else
							g_bActive = true;


					}

					//
					if ( g_pDRGWrapper->Initialized() )
					{
                        hr = g_pClothSample->TargetChanged();
						g_pCFontTexture->Restore(g_pDRGWrapper->m_pD3DDevice7);

						if( FAILED( hr ) )
						{
							DisplayErrorMessage( hr );
							DestroyWindow( hWnd );
						}						
					}
				}
			}
			
			
			break;


			//
			// Don't let the window get too small
			//
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

		case WM_ACTIVATEAPP:
			// Determine whether app is being activated or not
			g_bActive = wParam ? true : false;

			if (g_bActive) 
			{
				// Hide the cursor if we are active
				//while (ShowCursor(FALSE) > 0) { };
			} 
			else 
			{
				//ShowCursor(TRUE);                
			}
			break;

		case WM_MOVE:
			if( g_bActive && g_pDRGWrapper )
			{
				g_pDRGWrapper->MoveWindow();
			}
			break;
		case WM_SIZE:
			if( (wParam == SIZE_MAXHIDE) || (wParam == SIZE_MINIMIZED ) )
				g_bActive = false;
			else
				g_bActive = true;

            if( g_bActive && g_pDRGWrapper && g_pClothSample && !g_bFullscreen )
			{
				g_bActive = false;
				//if the device wasn't trashed doing a fullscreen->window change
				if (g_pDRGWrapper->m_pD3D7)
					hr = g_pDRGWrapper->m_pD3D7->EvictManagedTextures();

				hr = g_pDRGWrapper->RefreshMode( false );
				if( FAILED( hr ) )
				{
					DisplayErrorMessage( hr );
					DestroyWindow( hWnd );
				}
				else if( g_pDRGWrapper->Initialized() )
				{
                    hr = g_pClothSample->TargetChanged();

					if( FAILED( hr ) )
					{
						DisplayErrorMessage( hr );
						DestroyWindow( hWnd );
					}
					else
					{
						g_bActive = true;
						g_pCFontTexture->Restore(g_pDRGWrapper->m_pD3DDevice7);

					}

				}
			}
			break;

		case WM_CLOSE:
			DestroyWindow(g_hWnd);
			break;
		case WM_DESTROY:
			//
			// Destroy the stuff we created
			//
            if( g_pClothSample ) 
			{
                g_pClothSample->DestroyWorld();
                delete g_pClothSample;
                g_pClothSample = NULL;
			}
			
			if( g_pDRGWrapper )
			{
				delete g_pDRGWrapper;
				g_pDRGWrapper = NULL;
			}

            if( g_pCFontTexture )
			{
				delete g_pCFontTexture;
				g_pCFontTexture = NULL;
			}
			


			// And finally get out for good.
			PostQuitMessage(0);
			break;
		// are there other messaged we should be handling?
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
}


int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	BOOL bGotMessage;
    MSG msg;
	HRESULT hr;
	
    // Set global handle
    g_hInst = hInst;
	
	// Initialize the window required by Windows
    if (!InitWindow(hInst, nCmdShow)) 
	{
		MessageBox( NULL, "Couldn't create a window!", WINDOWNAME, MB_ICONEXCLAMATION | MB_OK );
		return 1;
	}

	//
	// Allocate a CDRGWrapper -- the class' constructor will enumerate all the D3D devices available
	//
	g_pDRGWrapper = new CDRGWrapper( g_hWnd );

	//
	// Add the available devices to the device menu
	//
	g_iDevices = g_pDRGWrapper->GetNumDevices();
	int i;
	D3DDEVICEINFO *pDevice;
	MENUITEMINFO mii;

	HMENU hMenu = GetMenu( g_hWnd );
	hMenu = GetSubMenu( hMenu, 1 ); // Get the second sub-menu (Assumes File then Device)
	DeleteMenu( hMenu, IDM_DEVICE_DEVICE0, MF_BYCOMMAND ); // Delete the first menu item

	memset( &mii, 0, sizeof( MENUITEMINFO ) );
	mii.cbSize = sizeof( MENUITEMINFO );
	mii.fMask = MIIM_CHECKMARKS | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.fState = MFS_ENABLED | MFS_UNCHECKED | MFS_UNHILITE;
 
	for( i=0; i<g_iDevices; i++ )
	{
		pDevice = g_pDRGWrapper->GetDevice( i );
		mii.wID = IDM_DEVICE_DEVICE0 + i;
		mii.dwTypeData = pDevice->strDesc;//pMenuDescription;
		InsertMenuItem( hMenu, i, TRUE, &mii );
	}
  
	//
	// Pick the first  device that fits the requirements (HW/SW, AGP) the Appwizard
	// user specified
	//
	
	bool bUseHardware;
		bUseHardware = true;

	bool bRequireAGP;
		bRequireAGP = false;


	if( FAILED( hr = g_pDRGWrapper->GetBestDevice( g_iDeviceNum, bUseHardware, bRequireAGP ) ) )
	{
		//Then fall back to SW renderer
		if (FAILED( hr = g_pDRGWrapper->GetBestDevice( g_iDeviceNum, false, false ) ) )
		{
			DisplayErrorMessage( hr );
			return 0;
		}
	}
	
	//
	// Check off the menu item for the device
	//
	CheckMenuItem( hMenu, IDM_DEVICE_DEVICE0+g_iDeviceNum, MF_BYCOMMAND | MF_CHECKED  );

	//
	// Get the modes available so we can setup the menu -- DON'T delete the
	// pModes pointer returned by this function.  It will be cleaned up by the
	// wrapper class
	//
	D3DFSMode *pModes = g_pDRGWrapper->m_pCurrentDevice->pFSModes;
	g_iModes = (int)g_pDRGWrapper->m_pCurrentDevice->dwNumFSModes;
	
	// Add the modes to the menu
	//
	hMenu = GetMenu( g_hWnd );
	hMenu = GetSubMenu( hMenu, 2 ); // Get the third sub-menu (Assumes File then Device then Screen)
	DeleteMenu( hMenu, IDM_MODE_FULL0, MF_BYCOMMAND ); // Delete the first menu item

	for( i=0; i<g_iModes; i++ )
	{
		mii.dwTypeData = pModes->szDescription;
		mii.wID = IDM_MODE_FULL0 + i;
		InsertMenuItem( hMenu, i+2, TRUE, &mii );
		pModes = pModes->pNext;
	}

	// Initialize the default mode (primary device, Full Screen, 640x480x16, 1 Back buffer, ZBuffer)
	//
	// Some examples of different ways to call init mode:
	//
	//	hr = g_pDRGWrapper->InitMode()  
	//		- all defaults. primary display device, fullscreen, 640x480,1 backbuffer, zbuffer
	//
	//	hr = g_pDRGWrapper->InitMode( int DeviceNumber, Bool Fullscreen,  )  
	//		- specify device, specify window/fs 
	//
	//	hr = g_pDRGWrapper->InitMode( iDeviceNum, bFullscreen, iHres,iVres,iBpp,iBuffers,bHw,false )  
	//	hr = g_pDRGWrapper->InitMode( 1, true, 640,480,16,2,true,true )  ;
	//		- specify device, specify window/fs, res, etc. The last flag tells it to fall
	//		  back to the default mode if the requested mode fails.
	//
	//	hr = g_pDRGWrapper->InitMode( g_iDeviceNum, iMode)  
	//		- specify device and mode number from the list of enumerated modes. Useful with a menu.
	//
	
    g_bFullscreen = false;
	g_bPauseAnimation = false;
	g_bShowSampleHelp = false;


	if( FAILED( hr = g_pDRGWrapper->InitMode(g_iDeviceNum,g_bFullscreen,640,480,16,1,true) ) )
    {
		DisplayErrorMessage( hr );
		return 0;
	}
	//here, I set our fullscreen global, since, depending how InitMode was called, you may not know
	//whether the app is in FS or not (e.g. it may have been forced to FS by selecting a voodoo card).
	g_bFullscreen = g_pDRGWrapper->m_bFullscreen;


	//
	// Initialize the world. This is where the class containing all your apps stuff gets invoked
	//
	char szDir[_MAX_PATH], szConfigFile[_MAX_PATH];
	GetCurrentDirectory( _MAX_PATH, szDir );
	if( szDir[lstrlen( szDir )-1] != '\\' )
		strcat( szDir, "\\" );
	wsprintf( szConfigFile, "%sCLOTH.INI", szDir );
    g_pClothSample = new CClothSample( szConfigFile );
    g_pClothSample->InitWorld( g_pDRGWrapper );

	
	//
	// Initialize the Text class, and a font texture for it.
	// (have to cast the second param to dword because of the overloaded fcn)
	//
	g_pCFontTexture = new CDRGTexture;
	if( g_pCFontTexture->CreateTexture( CBMPLoader( CFileStream( FONTFILENAME ) ), (DWORD)0, DRGTEXTR_TRANSPARENTBLACK) == S_OK )
		g_pCFontTexture->Restore(g_pDRGWrapper->m_pD3DDevice7);

	g_pCMainText = new CDRGText;
	g_pCMainText->Init( g_pDRGWrapper );
	g_pCMainText->SetFontTexture( g_pCFontTexture );

	g_pCTitleText = new CDRGText;
	g_pCTitleText->Init( g_pDRGWrapper );
	g_pCTitleText->SetFontTexture( g_pCFontTexture );

	g_pCHelpText = new CDRGText;
	g_pCHelpText->Init( g_pDRGWrapper );
	g_pCHelpText->SetFontTexture( g_pCFontTexture );



	int mainstringarray[2];
	int titlestringarray[2];
	int helpstringarray[7];

	//This will be the 2 strings for the frame counter. I will keep the length at
	// 3 digits, and then not have to reallocate memory when modifying
	g_pCMainText->CreateString("FPS:", 0.0f, 0.9f, 0.05f, 0.05f, 50, &mainstringarray[0]);
	g_pCMainText->CreateString("XXX", 0.2f, 0.9f, 0.05f, 0.05f, 50, &mainstringarray[1]);

	//The help and title text are store separately, so I can toggle them, making separate calls to draw each
	g_pCTitleText->CreateString("Intel Cloth Sample", 0.0f, 0.02f, 0.04f, 0.06f, 50, &titlestringarray[0]);
	g_pCTitleText->CreateString("(Press H for Help)", 0.0f, 0.8f, 0.02f, 0.03f, 50, &titlestringarray[1]);

    float fYPos = 0.05f;

	g_pCHelpText->CreateString("P - Pause Animation",	0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[0]); fYPos += 0.05f;
	g_pCHelpText->CreateString("W - Toggle Wireframe",	0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[1]); fYPos += 0.05f;
    g_pCHelpText->CreateString("X - Exit",              0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[2]); fYPos += 0.05f;
    g_pCHelpText->CreateString("F - Toggle Fullscreen", 0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[3]); fYPos += 0.05f;
	g_pCHelpText->CreateString("H - Toggle Help",       0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[4]); fYPos += 0.05f;
	g_pCHelpText->CreateString("R - Reset the Cloth Position",       0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[5]); fYPos += 0.05f;
	g_pCHelpText->CreateString("0,1,2,3 - Toggle Corner Constraints",       0.0f, fYPos, 0.02f, 0.03f, 50, &helpstringarray[6]); fYPos += 0.05f;
    
	
	////////////////
	// Main loop! //
	////////////////
	g_dwCurTime = timeGetTime(); // need to make sure our timer doesn't start at some massive number

	PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
    while( msg.message != WM_QUIT ) 
	{ 
		
		// 1. Peek at the Messages	(use GetMessage if we're not active)
		if( g_bActive )
			bGotMessage = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
		else
			bGotMessage = GetMessage( &msg, NULL, 0, 0 );

		if( bGotMessage )  // only process 1 message per frame
		{	
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		
		//update the timer vars
		g_dwLastTime = g_dwCurTime;
		g_dwCurTime = timeGetTime();		
		if( g_pDRGWrapper )
			g_pDRGWrapper->m_fFrameTime = (float)(g_dwCurTime - g_dwLastTime) / 1000.0f;
		else
			g_dwCurTime = timeGetTime();
	
		// Update the FPS counter
		if (g_pDRGWrapper) 
		{
			if (((float)(++g_iFrameCount) * g_pDRGWrapper->m_fFrameTime) >= FPSUPDATETIME) // We update the FPS every ten frames 
			{
				//calculate the instantaneous frame rate and reset the frame counter.
				g_dwFps= (DWORD)(1.0f / g_pDRGWrapper->m_fFrameTime);
				char string[10];
				sprintf( string, "%03d", g_dwFps );
				g_pCMainText->ModifyStringContent(mainstringarray[1],string);

				g_iFrameCount = 0; 
			} 
		}
		

        if( g_bActive && g_pClothSample && g_pDRGWrapper )
		{
			if( g_pDRGWrapper->Initialized() )
			{
				// 3. Do the Application loop. 
				g_pDRGWrapper->Clear( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );
				if (!g_bPauseAnimation)
					g_pClothSample->UpdateWorld(); //Step function to update timers, move objects, etc, the right amount

				g_pDRGWrapper->StartFrame(); 

                g_pClothSample->RenderWorld(); //Application calls drawprim, d3drm render, or whatever.

				g_pCMainText->DrawText(); //

				if (g_bShowSampleHelp)
					g_pCHelpText->DrawText();
				else
					g_pCTitleText->DrawText();


				g_pDRGWrapper->EndFrame(); //Does Endscene and flip/blit
				
			}
		}

	} 

    return msg.wParam;

} //end Winmain



// Initializes all that windows junk, creates class then shows main window
BOOL InitWindow(HINSTANCE hInst, int nCmdShow)
{
	WNDCLASS    wndClass;

    // Fill out WNDCLASS info
    wndClass.style              = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc        = WndProc;
    wndClass.cbClsExtra         = 0;
    wndClass.cbWndExtra         = 0;
    wndClass.hInstance          = hInst;
    wndClass.hIcon              = LoadIcon(hInst, "AppWizard");
    wndClass.hCursor            = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground      = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName       = MAKEINTRESOURCE( IDR_MENU1 );
    wndClass.lpszClassName      = CLASSNAME;
    
	// We only want to run one at a time anyway, so just quit if another is running
    if (!RegisterClass(&wndClass)) 
		return FALSE;

    // Create a window
    g_hWnd = CreateWindowEx(WS_EX_APPWINDOW,
                            CLASSNAME, 
                            WINDOWNAME,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
							640, 480,
                            NULL,
                            NULL,
                            hInst,
                            NULL);

    // Return false if window creation failed
    if (!g_hWnd) 
		return FALSE;
    
    // Show the window
    ShowWindow(g_hWnd, SW_SHOWNORMAL);

    // Update the window
    UpdateWindow(g_hWnd);

	return TRUE;
}



