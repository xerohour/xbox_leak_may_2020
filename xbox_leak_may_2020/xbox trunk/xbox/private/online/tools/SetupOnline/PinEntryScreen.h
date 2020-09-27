///////////////////////////////////////////////////////////////////////////////
//
// Name: PinEntryScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the Pin Entry screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>
#include <OnlineAccounts.h>

#include "UserInterfaceScreen.h"

//
// the text
// 

static const LPSTR PIN_ENTRY_TITLE = "Please Input New PIN";
static const LPSTR PIN_ENTRY_A_TEXT = "(A) Select";
static const LPSTR PIN_ENTRY_B_BACK_TEXT = "(B) Back";
static const LPSTR PIN_ENTRY_B_ERASE_TEXT = "(B) Erase";

//
// the pin entry screen colors
//

static const DWORD PIN_ENTRY_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD PIN_ENTRY_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD PIN_ENTRY_SCREEN_X = 320;
static const DWORD PIN_ENTRY_SCREEN_Y = 140;

static const DWORD PIN_ENTRY_A_X = 500;
static const DWORD PIN_ENTRY_A_Y = 400;
static const DWORD PIN_ENTRY_B_X = 140;
static const DWORD PIN_ENTRY_B_Y = 400;

static const DWORD PIN_X = 320;
static const DWORD PIN_Y = 200;

//
// the fg/ bg color
//

static const DWORD PIN_ENTRY_A_FG_COLOR = 0xFF00FF00;
static const DWORD PIN_ENTRY_A_BG_COLOR = 0xFF000000;
static const DWORD PIN_ENTRY_B_FG_COLOR = 0xFFFF0000;
static const DWORD PIN_ENTRY_B_BG_COLOR = 0xFF000000;

static const DWORD PIN_FG_COLOR = 0xFFFFFFFF;
static const DWORD PIN_BG_COLOR = 0xFF000000;

//
// the pin entry screen class
//

class CPinEntryScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CPinEntryScreen( CUserInterfaceScreen* pParent );

	~CPinEntryScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

	//
	// get the PIN
	//

	LPBYTE GetPin( void ) { return m_aPINByte; };

private:

	//
	// init function
	//

	HRESULT Init( void );

	//
	// update the text
	//

	HRESULT UpdateText( void );

	//
	// ui text
	//

	CUserInterfaceText m_aText;
	CUserInterfaceText m_bText;
	CUserInterfaceText m_pinText;

	//
	// the hnadle use for pin input
	//

	XPININPUTHANDLE m_handle;

	//
	// the input state
	//

    XINPUT_STATE m_inputState;

	//
	// the actual pin
	//

    BYTE m_aPINByte[4];

	//
	// the string
	//

	CHAR m_szStr[64];

	//
	// the current char index
	//

	DWORD m_dwCurIndex;

	//
	// init flag
	//

	BOOL m_bInit;
};