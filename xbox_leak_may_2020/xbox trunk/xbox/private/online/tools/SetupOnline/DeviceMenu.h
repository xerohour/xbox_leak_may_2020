///////////////////////////////////////////////////////////////////////////////
//
// Name: DeviceMenu.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the device menus
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xonlinep.h>
#include "myError.h"

#include "UserInterfaceMenu.h"
#include "UserInterfaceText.h"

//
// the foreground and background colors for copying of a user
//

static const DWORD DEVICE_MENU_COPY_FG_COLOR = 0xFF00FF00;
static const DWORD DEVICE_MENU_COPY_BG_COLOR = 0xFF000000;

//
// the a text
//

static const LPSTR ACCOUNT_A_SELECT_TEXT = "(A) Select";
static const LPSTR ACCOUNT_A_UNSELECT_TEXT = "(A) Unselect";
static const LPSTR ACCOUNT_A_PASTE_TEXT = "(A) Paste";

//
// the title of the hard disk
//

static const LPSTR HARD_DISK_TITLE = "HARD DISK";

//
// the device menu class
//

class CDeviceMenu : public CUserInterfaceMenu {

public:

	//
	// constructor and descturctor
	//

	CDeviceMenu( LPSTR szTitle, 
		         DWORD dwFGColor, 
				 DWORD dwBGColor, 
				 DWORD dwTextX,
				 DWORD dwTextY,
				 PXONLINEP_USER* apUserList, 
				 PXONLINEP_USER* ppClipboardUser, 
				 CUserInterfaceText* pSelectUserText,
				 CUserInterfaceText* pEraseUserText,
				 CUserInterfaceText* pOverwriteText,
				 PCHAR pchUpdate );

	~CDeviceMenu( void );

	//
	// the overrided update function to process input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput,
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// variable to hold if an item in the menu is selected
	// and also which one it is
	//

	DWORD m_dwSelected;

	//
	// the online user list for the device
	//

	PXONLINEP_USER* m_apUserList;

	//
	// the golbal clipboard user
	//

    PXONLINEP_USER* m_ppClipboardUser;

	//
	// the text for state handling
	//

	CUserInterfaceText* m_pSelectUserText;
	CUserInterfaceText* m_pEraseUserText;
	CUserInterfaceText* m_pOverwriteText;

	//
	// pointer used to notify the screen that a menu needs update
	//

	PCHAR m_pchUpdate;
};
