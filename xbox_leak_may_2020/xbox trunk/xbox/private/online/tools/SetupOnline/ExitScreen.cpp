///////////////////////////////////////////////////////////////////////////////
//
// Name: ExitScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for exit screen
//
///////////////////////////////////////////////////////////////////////////////

#include "ExitScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  exit screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the exit screen
//
///////////////////////////////////////////////////////////////////////////////

CExitScreen::CExitScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the ping screen
	//

	: CUserInterfaceScreen( pParent, 
	                        EXIT_SCREEN_FG_COLOR, 
							EXIT_SCREEN_BG_COLOR, 
							EXIT_SCREEN_X, 
							EXIT_SCREEN_Y )

{
	//
	// set the initial text
	//

	SetText( "" );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  exit screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the exit screen
//
///////////////////////////////////////////////////////////////////////////////

CExitScreen::~CExitScreen( void )
{
	//
	// No code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input ui
//        pAudio - the audio ui
//
// Output: the next screen
//
// Description: process the input for the screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CExitScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{

	//
	// if update of this screen is called, reboot back
	//

	XLaunchNewImage( NULL, NULL );

	//
	// this really will never be reached
	//

	return NULL;
}