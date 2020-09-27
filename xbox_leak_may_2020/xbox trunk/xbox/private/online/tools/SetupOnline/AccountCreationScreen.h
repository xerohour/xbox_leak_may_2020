///////////////////////////////////////////////////////////////////////////////
//
// Name: AccountCreationScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the account creation screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>
#include <OnlineAccounts.h>

#include "UserInterfaceScreen.h"
#include "UserInterfaceText.h"
#include "PinEntryText.h"

//
// the strings
//

static const LPSTR CREATION_TEXTS[] = { "Creating User on PartnerNet.  ",
                                        "Creating User on PartnerNet . ",
								        "Creating User on PartnerNet  .",
									    "Creating User on PartnerNet . " };

static const LPSTR CREATION_SUCCEEDED_TEXT = "User Creation Succeeded";
static const LPSTR CREATION_ERROR_TEXT = "User Creation Failed";
static const LPSTR CREATION_DISK_FULL_TEXT = "User Creation Failed: Drive Full";
static const LPSTR CREATION_NULL_USER_TEXT = "No Name Entered: Generating Random Name";
static const LPSTR CREATION_USER_EXISTS_TEXT = "User Name Exists: Generated Random Name";
static const LPSTR CREATION_A_TEXT = "(A) OK";
static const LPSTR CREATION_B_TEXT = "(B) Back";

//
// the number of text states
//

static const DWORD NUM_CREATION_TEXT_STATES = 4;

//
// the CREATION screen colors
//

static const DWORD CREATION_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD CREATION_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD CREATION_SCREEN_X = 320;
static const DWORD CREATION_SCREEN_Y = 250;

static const DWORD CREATION_A_X = 500;
static const DWORD CREATION_A_Y = 400;
static const DWORD CREATION_B_X = 140;
static const DWORD CREATION_B_Y = 400;

static const DWORD CREATION_INFO_X = 320;
static const DWORD CREATION_INFO_Y = 320;

//
// the fg / bg color
//

static const DWORD CREATION_A_FG_COLOR = 0xFF00FF00;
static const DWORD CREATION_A_BG_COLOR = 0xFF000000;
static const DWORD CREATION_B_FG_COLOR = 0xFFFF0000;
static const DWORD CREATION_B_BG_COLOR = 0xFF000000;

static const DWORD CREATION_INFO_FG_COLOR = 0xFFFFFFFF;
static const DWORD CREATION_INFO_BG_COLOR = 0xFF000000;

//
// struct to hold thread params for USer account creation
//

struct SUserParams {
	LPBYTE pBytes;
	LPSTR szName;
};

//
// the account creation screen class
//

class CAccountCreationScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CAccountCreationScreen( CUserInterfaceScreen* pParent );

	~CAccountCreationScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

	//
	// set the pin and the name
	//

	void SetPin( LPBYTE pBytes ) { m_params.pBytes = pBytes; };
	void SetName( LPSTR szName ) { m_params.szName = szName; };

private:

	//
	// helper to update text based on thread status
	//

	void UpdateText( void );

	//
	// code to generate create user on partnernet
	//

	static DWORD WINAPI GenerateUser( LPVOID lpParam );

	//
	// handle for the thread
	//

	HANDLE m_hThread;

	//
	// the final hr value
	//

	HRESULT m_hrFinal;

	//
	// ui text
	//

	CUserInterfaceText m_aText;
	CUserInterfaceText m_bText;
	CUserInterfaceText m_infoText;

	//
	// the params
	//

	SUserParams m_params;

	//
	// place to hold the current creation text index
	//

	DWORD m_dwTextIndex;

	//
	// only have 1 creation per time
	//

	BOOL m_bCreationOnce;

	//
	// the current creation state
	//
};