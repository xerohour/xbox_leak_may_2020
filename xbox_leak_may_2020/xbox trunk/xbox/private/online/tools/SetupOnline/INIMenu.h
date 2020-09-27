///////////////////////////////////////////////////////////////////////////////
//
// Name: INIMenu.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the ini menu
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

static const DWORD INI_MENU_COPY_FG_COLOR = 0xFF00FF00;
static const DWORD INI_MENU_COPY_BG_COLOR = 0xFF000000;

//
// the ini menu class
//

class CINIMenu : public CUserInterfaceMenu {

public:

	//
	// constructor / destructor
	//

	CINIMenu( LPSTR szTitle, 
		      DWORD dwFGColor, 
			  DWORD dwBGColor, 
			  DWORD dwTextX,
			  DWORD dwTextY,
			  PXONLINEP_USER* apUserList, 
			  PXONLINEP_USER* ppClipboardUser,
			  CUserInterfaceText* pSelectText,
			  CUserInterfaceText* pAddUserText,
			  CUserInterfaceText* pEraseUserText );

	~CINIMenu( void );

	//
	// overloaded update input handler
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// member to hold if an item is selected, and which it is
	//

	DWORD m_dwSelected;

	//
	// the user list for the ini menu
	//

	PXONLINEP_USER* m_apUserList;

	//
	// the global clipboard user
	//

    PXONLINEP_USER* m_ppClipboardUser;

	//
	// the a text
	//

	CUserInterfaceText* m_pSelectText;
	CUserInterfaceText* m_pAddUserText; 
	CUserInterfaceText* m_pEraseUserText;
};