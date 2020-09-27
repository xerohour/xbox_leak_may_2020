///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the user interface screen base class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <vector>

using namespace std;

#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"
#include "UserInterfaceMenu.h"
#include "UserInterfaceText.h"

//
// the screen class
//

class CUserInterfaceScreen {

public:

	//
	// constructor and destructor
	//

	CUserInterfaceScreen( CUserInterfaceScreen* pParent, 
		                  DWORD dwFGColor, 
						  DWORD dwBGColor, 
						  DWORD dwTextX, 
						  DWORD dwTextY );

	~CUserInterfaceScreen( void );

	//
	// add a menu to the screen
	//

	void AddMenu( CUserInterfaceMenu* pMenu );

	//
	// add UI text to the screen
	//

	void AddUIText( CUserInterfaceText* pText );

	//
	// get the number of menus
	//

	DWORD NumMenus( void );

	//
	// get the number of ui text objects
	//

	DWORD NumUIText( void );

	//
	// get the menu at the given index
	//

	CUserInterfaceMenu* Menu( DWORD dwIndex );

	//
	// get the ui text at the given index
	//

	CUserInterfaceText* UIText( DWORD dwIndex );

	//
	// update the user input
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

	//
	// set/get the text of the screen
	//

	void SetText( LPSTR szText );
	LPSTR GetText( void );

	//
	// remove the menus from the screen
	//

	void RemoveMenus( void );

	//
	// see if this is the active menu
	//

	BOOL ActiveMenu( DWORD dwMenuID ) { return dwMenuID == m_dwActive; };

	//
	// get the active menu index
	//

	DWORD ActiveMenuIndex( void ) { return m_dwActive; }

	//
	// get the foreground / background color
	//

	DWORD FGColor( void ) { return m_dwFGColor; };
	DWORD BGColor( void ) { return m_dwBGColor; };

	//
	// get the x and y location of the text
	//

	DWORD TextX( void ) { return m_dwTextX; };
	DWORD TextY( void ) { return m_dwTextY; };

protected:

	//
	// helpers for derived classes to query screen info
	// without being able to modify the screen state
	//

	//
	// is there more than 1 menu?
	//

	BOOL HasMultipleMenus( void ) { return NumMenus() > 1; };

	//
	// is the first or last menu active?
	//

	BOOL FirstActive( void ) { return 0 == m_dwActive; };
	BOOL LastActive( void ) { return NumMenus() - 1 == m_dwActive; };

	//
	// index of the first and last menu
	//

	DWORD LastMenu( void ) { return NumMenus() - 1; };
	DWORD FirstMenu( void ) { return 0; };

	//
	// goto the previous/next menu
	//

	DWORD PreviousMenu( void ) { return m_dwActive - 1; };
	DWORD NextMenu( void ) { return m_dwActive + 1; };

	//
	// scroll to the next/previous menu
	//

	void ScrollActiveMenuUp( void ) { m_dwActive = FirstActive() ? LastMenu() : PreviousMenu(); };
	void ScrollActiveMenuDown( void ) { m_dwActive = LastActive() ? FirstMenu() : NextMenu(); };

	//
	// see if there are no menus
	//

	BOOL NoActiveMenus( void ) { return m_menus.empty(); };

	//
	// get the active menu
	//

	CUserInterfaceMenu* ActiveMenu( void ) { return m_menus[m_dwActive]; };
	
	//
	// find the parent screen
	//

	CUserInterfaceScreen* ParentScreen( void ) { return m_pParent; };

private:

	//
	// the list of menus
	//

	vector<CUserInterfaceMenu*> m_menus;

	//
	// the list of layout text
	//

	vector<CUserInterfaceText*> m_uiText;

	//
	// the parent screen
	//

	CUserInterfaceScreen* m_pParent;

	//
	// the text of the screen
	//

	LPSTR m_szText;

	//
	// the active menu index
	//

	DWORD m_dwActive;

	//
	// the foreground/background color
	//

	DWORD m_dwFGColor;
	DWORD m_dwBGColor;

	//
	// the x/y location of the text
	//

	DWORD m_dwTextX;
	DWORD m_dwTextY;

};