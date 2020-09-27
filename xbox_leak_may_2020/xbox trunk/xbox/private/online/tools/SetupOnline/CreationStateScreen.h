///////////////////////////////////////////////////////////////////////////////
//
// Name: CreationStateScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the creation state screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>
#include <OnlineAccounts.h>

#include "UserInterfaceScreen.h"
#include "AccountCreationScreen.h"
#include "PinEntryScreen.h"
#include "NameEntryScreen.h"

//
// the creation state screen colors
//

static const DWORD CREATION_STATE_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD CREATION_STATE_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD CREATION_STATE_SCREEN_X = 320;
static const DWORD CREATION_STATE_SCREEN_Y = 250;

//
// the creation state enumeration
//

enum CREATION_STATE 
{
	START_STATE,
	NAME_ENTRY_STATE,
	PIN_ENTRY_STATE,
	USER_CREATION_STATE,
	DONE_STATE
};

//
// the creation state screen class
//

class CCreationStateScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CCreationStateScreen( CUserInterfaceScreen* pParent );

	~CCreationStateScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// the current state
	//

	CREATION_STATE m_creationState;

	//
	// the pointer to the screens
	//

	CNameEntryScreen* m_pNameScreen;
	CPinEntryScreen* m_pPinScreen;
	CAccountCreationScreen* m_pCreationScreen;
};