///////////////////////////////////////////////////////////////////////////////
//
// Name: NameEntryScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the Name Entry screen
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

static const LPSTR NAME_ENTRY_TITLE = "Please Enter User Name";
static const LPSTR NAME_ENTRY_A_TEXT = "(A) Select";
static const LPSTR NAME_ENTRY_B_TEXT = "(B) Back";

//
// the Name Entry screen colors
//

static const DWORD NAME_ENTRY_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD NAME_ENTRY_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD NAME_ENTRY_SCREEN_X = 320;
static const DWORD NAME_ENTRY_SCREEN_Y = 140;

static const DWORD NAME_ENTRY_A_X = 500;
static const DWORD NAME_ENTRY_A_Y = 400;
static const DWORD NAME_ENTRY_B_X = 140;
static const DWORD NAME_ENTRY_B_Y = 400;

static const DWORD KEY_X = 130;
static const DWORD KEY_Y = 220;

static const DWORD NAME_X = 320;
static const DWORD NAME_Y = 170;

//
// the fg/ bg color
//

static const DWORD NAME_ENTRY_A_FG_COLOR = 0xFF00FF00;
static const DWORD NAME_ENTRY_A_BG_COLOR = 0xFF000000;
static const DWORD NAME_ENTRY_B_FG_COLOR = 0xFFFF0000;
static const DWORD NAME_ENTRY_B_BG_COLOR = 0xFF000000;

static const DWORD KEY_FG_COLOR = 0xFFFFFFFF;
static const DWORD KEY_BG_COLOR = 0xFF000000;

static const DWORD NAME_FG_COLOR = 0xFFFFFFFF;
static const DWORD NAME_BG_COLOR = 0xFF000000;

//
// the number of rows and columns in the keyboard
//

static const DWORD NUM_ROWS = 4;
static const DWORD NUM_COLS = 10;

//
// the vert and horizontal distance for keys
//

static const DWORD HORI_DIST = 40;
static const DWORD VERT_DIST = 40;

//
// the special keys
//

static const LPSTR KEY_OK = "ok";
static const LPSTR KEY_CANCEL = "cancel";
static const LPSTR KEY_BACK = "<<";

//
// the array of strings
//

static const LPSTR KEYBOARD_STRINGS[NUM_ROWS][NUM_COLS] = 
{
	{ "0", "1", "2", "3", "4", "5", "6", "7", "8", KEY_OK     },
	{ "9", "a", "b", "c", "d", "e", "f", "g", "h", KEY_CANCEL },
	{ "i", "j", "k", "l", "m", "n", "o", "p", "q", "_"        },
	{ "r", "s", "t", "u", "v", "w", "x", "y", "z", KEY_BACK   }
};

//
// the name entry screen class
//

class CNameEntryScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CNameEntryScreen( CUserInterfaceScreen* pParent );

	~CNameEntryScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

	//
	// get the name
	//

	LPSTR GetName( void ) { return m_szName; };

private:

	//
	// ui text
	//

	CUserInterfaceText m_aText;
	CUserInterfaceText m_bText;

	CUserInterfaceText m_nameText;

	//
	// the name
	//

	CHAR m_szName[XONLINE_NAME_SIZE];

	//
	// the index into the string
	//

	DWORD m_dwStrIndex;

	//
	// the row and col index
	//

	DWORD m_dwRowIndex;
	DWORD m_dwColIndex;

	//
	// the actual key text elements
	//

	CUserInterfaceText* m_pKeys[NUM_ROWS][NUM_COLS];

	//
	// keep track of when updates are needed
	//

	BOOL m_bUpdate;

	//
	// init flag
	//

	BOOL m_bInit;
};