///////////////////////////////////////////////////////////////////////////////
//
// Name: AccountsScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the Account Manipulation Screen
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xonlinep.h>
//#include <OnlineAccounts.h>
#include "myError.h"

#include "UserInterfaceScreen.h"
//#include "INIMenu.h"
#include "DeviceMenu.h"

//
// the number of menus in the screen
//

static const DWORD NUM_MENUS = 10;

//
// the number of device menus in the screen
//

static const DWORD NUM_DEVICE_MENUS = NUM_MENUS - 1;

//
// the name of the ini file menu
//

//static const LPSTR INI_NAME = "INI File";

//
// used to title the menu and report which device menu needs updating
//

static const LPSTR MU_NAMES[] = { "HARD DISK", 
								  "F: (Controller 1, Top Slot)",
								  "H: (Controller 2, Top Slot)",
								  "J: (Controller 3, Top Slot)",
								  "L: (Controller 4, Top Slot)",
								  "G: (Controller 1, Bottom Slot)",	
								  "I: (Controller 2, Bottom Slot)",
								  "K: (Controller 3, Bottom Slot)",
								  "M: (Controller 4, Bottom Slot)" };

//
// the account screen colors
//

static const DWORD ACCOUNT_SCREEN_FG_COLOR = 0xFFFFFFFF;
static const DWORD ACCOUNT_SCREEN_BG_COLOR = 0xFF000000;

//
// the x and y location of text
//

static const DWORD ACCOUNT_SCREEN_X = 320;
static const DWORD ACCOUNT_SCREEN_Y = 250;

//
// the value of an invalid device
//

static const CHAR INVALID_DEVICE_CHAR = '\255';

//
// the number of charecters on 1 line of an ini file
//

//static const DWORD INI_FILE_STRING_SIZE = 256;

//
// the location of the ini file
//

//static const LPSTR INI_FILE_LOCATION = "d:\\users.ini";

//
// the ini file format string
//

//	!#$%&'*+./=?^_`{|}~@-

//static const LPSTR INI_FORMAT_STRING = "%16I64x,%63[a-zA-Z0-9@._-],%16I64x%16I64x,%255[a-zA-Z0-9@._,-]";
//static const LPSTR INI_FORMAT_STRING = "%16I64x,%63[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%16I64x%16I64x,%255[a-zA-Z0-9,]";

//
// ui text stuff
//

static const DWORD ACCOUNT_A_X = 500;
static const DWORD ACCOUNT_A_Y = 400;
static const DWORD ACCOUNT_A_FG_COLOR = 0xFF00FF00;
static const DWORD ACCOUNT_A_BG_COLOR = 0xFF000000;

static const LPSTR ACCOUNT_X_TEXT = "(X) Delete User";
static const DWORD ACCOUNT_X_X = 320;
static const DWORD ACCOUNT_X_Y = 400;
static const DWORD ACCOUNT_X_FG_COLOR = 0xFF0000FF;
static const DWORD ACCOUNT_X_BG_COLOR = 0xFF000000;

static const LPSTR ACCOUNT_B_TEXT = "(B) Back";
static const DWORD ACCOUNT_B_X = 140;
static const DWORD ACCOUNT_B_Y = 400;
static const DWORD ACCOUNT_B_FG_COLOR = 0xFFFF0000;
static const DWORD ACCOUNT_B_BG_COLOR = 0xFF000000;

static const LPSTR ACCOUNT_LEFT_TEXT = "<<";
static const DWORD ACCOUNT_LEFT_X = 130;
static const DWORD ACCOUNT_LEFT_Y = 240;
static const DWORD ACCOUNT_LEFT_FG_COLOR = 0xFFFFFFFF;
static const DWORD ACCOUNT_LEFT_BG_COLOR = 0xFF000000;

static const LPSTR ACCOUNT_RIGHT_TEXT = ">>";
static const DWORD ACCOUNT_RIGHT_X = 500;
static const DWORD ACCOUNT_RIGHT_Y = 240;
static const DWORD ACCOUNT_RIGHT_FG_COLOR = 0xFFFFFFFF;
static const DWORD ACCOUNT_RIGHT_BG_COLOR = 0xFF000000;

static const LPSTR ACCOUNT_OVERWRITE_TEXT = "User Account On MU Will Be Overwritten";
static const DWORD ACCOUNT_OVERWRITE_X = 320;
static const DWORD ACCOUNT_OVERWRITE_Y = 320;
static const DWORD ACCOUNT_OVERWRITE_FG_COLOR = 0xFFFFFFFF;
static const DWORD ACCOUNT_OVERWRITE_BG_COLOR = 0xFF000000;

//
// the account screen class
//

class CAccountsScreen : public CUserInterfaceScreen {

public:

	//
	// constructor and descturctor
	//

	CAccountsScreen( CUserInterfaceScreen* pParent );

	~CAccountsScreen( void );

	//
	// overloaded update function
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// helper function to init the mus
	//

	HRESULT InitDevices( void );

	//
	// helper code to parse the ini file
	//

//	HRESULT ParseIniFile( void );

	//
	// Helper code to manipulate the drive lists
	//

	HRESULT PopulateDriveList( CHAR chDrive );
	HRESULT PopulateDriveLists( void );

	//
	// MU handling
	//

	HRESULT UpdateLists( void );

	//
	// code to clean up the lists
	//

	void CleanupList( DWORD dwIndex );
	void CleanupLists( void );

	//
	// rebuild the menus
	//

	void RebuildMenus( void );

	//
	// the user lists for the ini file, the mounted MUs and the HD
	// these are indexed as follows:
	//
	// 0 - ini file
	// 1 - hard disk
	// 2 - controller 1, mu 1
	// 3 - controller 1, mu 2
	// 4 - controller 2, mu 1
	// 5 - controller 2, mu 2
	// 6 - controller 3, mu 1
	// 7 - controller 3, mu 2
	// 8 - controller 4, mu 1
	// 9 - controller 4, mu 2
	//

	PXONLINEP_USER m_aapUserLists[NUM_MENUS][XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// the ui text
	//

	CUserInterfaceText m_bText;
	CUserInterfaceText m_xText;
	CUserInterfaceText m_leftText;
	CUserInterfaceText m_rightText;
	CUserInterfaceText m_aText;
	CUserInterfaceText m_overwriteText;

	//
	// the ini menu. it is differnet than a device because you can
	// not remove users from it
	//

//	CINIMenu m_iniMenu;

	//
	// the other menus. these are:
	//
	// 0 - the hard disk
	// 8 - F:
	// 7 - H:
	// 6 - J:
	// 5 - L:
	// 4 - G:
	// 3 - I:
	// 2 - K:
	// 1 - L:
	//

	CDeviceMenu* m_apDeviceMenus[NUM_DEVICE_MENUS];

	//
	// the bit mask of the mounted mus
	//
	// only the low 8 bits are used. these are (from right to left):
	//
	// bit 0 - F:
	// bit 1 - H:
	// bit 2 - J:
	// bit 3 - L:
	// bit 4 - G:
	// bit 5 - I:
	// bit 6 - K:
	// bit 7 - L:
	//

	DWORD m_dwMUs;

	//
	// a pointer to the online user currently selected on one of the menus
	// for copying / pasting to other menus
	//

	PXONLINEP_USER m_pClipboardUser;

	//
	// flag to guarentee init code only happnes once
	//

	BOOL m_bInit;

	//
	// an indicator that is used to decide of menus need to be updated
	//

	CHAR m_chUpdate;

	//
	// the index of the last active menu, used to decide what menu should
	// be displayed after an MU insertion/ removal
	//

	DWORD m_dwLastActive;

	//
	// check state to see if lists should be updated
	//

	BOOL m_bJustEntered;

	//
	// place to hold a list of bad MU's, so we don't unmount them
	//

	DWORD m_dwBadMUs;
};