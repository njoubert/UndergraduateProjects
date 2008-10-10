// DRGShell.h - Created with DR&E AppWizard 1.0B10
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
//  This is the main include file for the DRG Shell classes. 
//
// ------------------------------------------------------------------------------------
//
// Authors: Dean Macri, Kim Pallister - Intel Developer Relations & Engineering Group
//
//////////////////////////////////////////////////////////////////////// 
#define STRICT
#define D3D_OVERLOADS 1
#include <windows.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <d3d.h>
#include <dxfile.h> //added for dx file stuff

#include <stdio.h>
#include <math.h>

#include "DataIO\DataStream.h"
#include "DataIO\FileStream.h"
#include "DataIO\ImageLoader.h"
#include "DataIO\BMPLoader.h"


#include "FrameWork\DRGMatrix.h"
#include "FrameWork\DRGWrapper.h"
#include "FrameWork\DRGTexture.h"
#include "FrameWork\DRGText.h"

#include "Objects\DRGShader.h"

#include "Objects\DRGObject.h"
#include "Objects\DRGObjectPlane.h"

#include "Physics.h"

#include "Configuration.h"

#include "ClothObject.h"

#include "Resource.h"
#include "ClothSample.h"


//
// Some error handling macros
//
#define HANDLE_ERROR( hr, result ) if( FAILED( hr ) ) return (result)
#define ON_ERROR_GOTO( hr, dest ) if( FAILED( hr ) ) goto dest

#define SAFE_RELEASE( x ) do { if( (x) ) { x->Release(); x = NULL; } } while (0)
#define SAFE_DELETE( x ) do { if( (x) ) { delete x; x = NULL;} } while (0)
#define SAFE_DELETE_ARRAY( x ) do { if( (x) ) { delete [] x; x = NULL;} } while (0)

// Defines
#define	CLASSNAME	"ClothSample_Class"	// Used in Init()
#define WINDOWNAME	"Intel Cloth Sample"

//
// Error handling routine located in Errors.cpp
//
void DisplayErrorMessage( HRESULT hr );
void GetErrorMessage( HRESULT hr, char szMessage[512] );
void MonoDebug(char * str);
