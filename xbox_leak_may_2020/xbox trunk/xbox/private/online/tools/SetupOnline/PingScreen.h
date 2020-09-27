///////////////////////////////////////////////////////////////////////////////
//
// Name: PingScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the ping screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>
#include <OnlineAccounts.h>

#include "UserInterfaceScreen.h"
#include "UserInterfaceText.h"

//
// the strings
//

static const LPSTR PING_TEXTS[] = { "Pinging PartnerNet.  ",
                                    "Pinging PartnerNet . ",
								    "Pinging PartnerNet  .",
									"Pinging PartnerNet . " };

static const LPSTR PING_SUCCEEDED_TEXT = "Ping Succeeded";
static const LPSTR PING_FAILED_TEXT = "Ping Failed";
static const LPSTR PING_A_TEXT = "(A) OK";
static const LPSTR PING_B_TEXT = "(B) Back";

//
// the ping screen colors
//

static const DWORD PING_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD PING_SCREEN_BG_COLOR = 0xFF000000;

//
// the number of text states
//

static const DWORD NUM_TEXT_STATES = 4;

//
// the x and y location of text
//

static const DWORD PING_SCREEN_X = 320;
static const DWORD PING_SCREEN_Y = 250;

static const DWORD PING_A_X = 500;
static const DWORD PING_A_Y = 400;
static const DWORD PING_B_X = 140;
static const DWORD PING_B_Y = 400;

//
// the fg/ bg color
//

static const DWORD PING_A_FG_COLOR = 0xFF00FF00;
static const DWORD PING_A_BG_COLOR = 0xFF000000;
static const DWORD PING_B_FG_COLOR = 0xFFFF0000;
static const DWORD PING_B_BG_COLOR = 0xFF000000;

//
// the ping screen class
//

class CPingScreen : public CUserInterfaceScreen {

public:

	//
	// constructor / destructor
	//

	CPingScreen( CUserInterfaceScreen* pParent );

	~CPingScreen( void );

	//
	// overloaded user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// helper ot update text based on thread status
	//

	void UpdateText( void );

	//
	// code to generate ping to partenrnet
	//

	static DWORD WINAPI GeneratePing( LPVOID lpParam );

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

	//
	// place to hold the current ping text index
	//

	DWORD m_dwPingTextIndex;

	//
	// only have 1 ping per time
	//

	BOOL m_bPingOnce;

};