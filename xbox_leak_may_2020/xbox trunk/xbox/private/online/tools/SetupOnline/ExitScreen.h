///////////////////////////////////////////////////////////////////////////////
//
// Name: Exit.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the exit screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>
#include <OnlineAccounts.h>

#include "UserInterfaceScreen.h"

//
// the ping screen colors
//

static const DWORD EXIT_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD EXIT_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD EXIT_SCREEN_X = 320;
static const DWORD EXIT_SCREEN_Y = 250;

//
// the ping screen class
//

class CExitScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CExitScreen( CUserInterfaceScreen* pParent );

	~CExitScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

};