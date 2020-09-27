///////////////////////////////////////////////////////////////////////////////
//
// Name: MachineRemovalScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the machine account removal screen
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>

#include "UserInterfaceScreen.h"
#include "UserInterfaceText.h"

//
// the strings
//

static const LPSTR CREATION_PROMPT_STRING = "Press A to erase machine account";
static const LPSTR COMPLETION_PROMPT_STRING = "Machine account erased. Press A to continue";
static const LPSTR MACHINE_A_TEXT = "(A) Action";
static const LPSTR MACHINE_B_TEXT = "(B) Go Back";

//
// the machine account screen colors
//

static const DWORD MACHINE_ACCOUNT_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD MACHINE_ACCOUNT_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD MACHINE_ACCOUNT_SCREEN_X = 320;
static const DWORD MACHINE_ACCOUNT_SCREEN_Y = 250;

static const DWORD MACHINE_A_X = 100;
static const DWORD MACHINE_A_Y = 400;
static const DWORD MACHINE_B_X = 520;
static const DWORD MACHINE_B_Y = 400;

//
// the fg/ bg color
//

static const DWORD MACHINE_A_FG_COLOR = 0xFF00FF00;
static const DWORD MACHINE_A_BG_COLOR = 0xFF000000;
static const DWORD MACHINE_B_FG_COLOR = 0xFFFF0000;
static const DWORD MACHINE_B_BG_COLOR = 0xFF000000;

//
// the machine removal screen ui
//

class CMachineRemovalScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / desctructor
	//

	CMachineRemovalScreen( CUserInterfaceScreen* pParent );

	~CMachineRemovalScreen( void );

	//
	// the update input method
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:


	//
	// ui text
	//

	CUserInterfaceText m_aText;
	CUserInterfaceText m_bText;

	//
	// member to hold wether machine account removal is complete
	//

	BOOL m_bComplete;
};