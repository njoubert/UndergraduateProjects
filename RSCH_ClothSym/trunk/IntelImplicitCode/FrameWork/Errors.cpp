// Errors.cpp - Created with DR&E AppWizard 1.0B10
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
// Routine for displaying an error message and exiting
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
////////////////////////////////////////////////////////////////////////////////////////

#include "..\DRGShell.h"

// -----------------------------------------------------------------------
// void DisplayErrorMessage( HRESULT hr )
//
// -----------------------------------------------------------------------
void DisplayErrorMessage( HRESULT hr )
{
	char strMsg[512];

	GetErrorMessage( hr, strMsg );

	MessageBox( NULL, strMsg, WINDOWNAME, MB_ICONERROR|MB_OK );
}

void GetErrorMessage( HRESULT hr, char strMsg[512] )
{
	switch( hr )
	{
		case E_OUTOFMEMORY:
			lstrcpy( strMsg, TEXT("Not enough memory!") );
			break;
		case DDERR_OUTOFVIDEOMEMORY:
			lstrcpy( strMsg, TEXT("There was insufficient video memory "
					"to use the hardware device.") );
			break;
		default:
			wsprintf( strMsg, TEXT("An unknown error (code %x) has occured\n\n" ), hr );
			break;
	}
}




