///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceMenu.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for base menu class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <vector>

using namespace std;

#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"

//
// forward decleration to keep from having circular includes
// due to circular dependency on screen and menu classes
//

class CUserInterfaceScreen;

//
// the menu ui class
//

class CUserInterfaceMenu {

public:

	//
	// constructor and destructor
	//

	CUserInterfaceMenu( LPSTR szTitle, 
		                DWORD dwFGColor, 
						DWORD dwBGColor, 
						DWORD dwTextX, 
						DWORD dwTextY );

	~CUserInterfaceMenu( void );

	//
	// code to add a menu item to the menu
	//

	void AddItem( LPSTR szStr, CUserInterfaceScreen* pScreen );

	//
	// accessor to get the number of items
	//

	DWORD NumItems( void );

	//
	// get the item at the inputted index
	//

	LPSTR Item( DWORD dwIndex );

	//
	// remove all the items from the menu
	//

	void RemoveItems( void );

	//
	// overloaded input handler
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio );

	//
	// get the title of the menu
	//

	LPSTR GetTitle( void ) { return m_szTitle; };

	//
	// find out if the inputted item is the active item
	//

	BOOL ActiveItem( DWORD dwItemID ) { return dwItemID == m_dwActive; };

	//
	// get the active item index
	//

	DWORD ActiveItemIndex( void ) { return m_dwActive; }

	//
	// get the foreground and background color fo the title
	//

	DWORD FGColor( void ) { return m_dwFGColor; };
	DWORD BGColor( void ) { return m_dwBGColor; };

	//
	// get the x and y position of the text
	//

	DWORD TextX( void ) { return m_dwTextX; };
	DWORD TextY( void ) { return m_dwTextY; };

	//
	// set the foreground / background color for the active item
	//

	void SetFGColorActive( DWORD dwColor ) { m_dwFGColors[m_dwActive] = dwColor; };
	void SetBGColorActive( DWORD dwColor ) { m_dwBGColors[m_dwActive] = dwColor; };

	//
	//
	// get the foreground / background color for a specific item

	DWORD FGColor( DWORD dwIndex ) { return m_dwFGColors[dwIndex]; };
	DWORD BGColor( DWORD dwIndex ) { return m_dwBGColors[dwIndex]; };

protected:

	//
	// helpers for derived classes to query menu info
	// without being able to modify the menu state
	//

	//
	// does this menu have more than 1 item?
	//

	BOOL HasMultipleItems( void ) { return NumItems() > 1; };

	//
	// is the first item in the menu the active one?
	//

	BOOL FirstActive( void ) { return 0 == m_dwActive; };

	//
	// is the last item in the menu the active one?
	//

	BOOL LastActive( void ) { return (NumItems() - 1) == m_dwActive; };

	//
	// get the index of the last item
	//

	DWORD LastItem( void ) { return NumItems() - 1; };

	//
	// get hte index of the first item
	//

	DWORD FirstItem( void ) { return 0; };

	//
	// create the index of the previous item and the next item
	//

	DWORD PreviousItem( void ) { return m_dwActive - 1; };
	DWORD NextItem( void ) { return m_dwActive + 1; };

	//
	// scroll the items in the menu up and down
	//

	void ScrollActiveItemUp( void ) { m_dwActive = FirstActive() ? LastItem() : PreviousItem(); };
	void ScrollActiveItemDown( void ) { m_dwActive = LastActive() ? FirstItem() : NextItem(); };

	//
	// check if the active item has a screen also
	//

	BOOL ActiveItemHasScreen( void ) { return ! m_screens.empty() && m_screens[m_dwActive]; };

	//
	// see if the items have screens
	//

	BOOL NoActiveItems( void ) { return m_screens.empty(); };

	//
	// get the screen that is highlited by the active item
	//

	CUserInterfaceScreen* ActiveScreen( void ) { return m_screens[m_dwActive]; };


private:

	//
	// the actual items in the menu
	//

	vector<LPSTR> m_items;

	//
	// the screens that go along with the strings
	//

	vector<CUserInterfaceScreen*> m_screens;

	//
	// the index of hte active item
	//

	DWORD m_dwActive;

	//
	// the fg / bg colors for the items in the list
	//

	vector<DWORD> m_dwFGColors;
	vector<DWORD> m_dwBGColors;

	//
	// th fg / bg color for the menu
	//

	DWORD m_dwFGColor;
	DWORD m_dwBGColor;

	//
	// the title of the menu
	//

	LPSTR m_szTitle;

	//
	// the location of the text for the menu
	//

	DWORD m_dwTextX;
	DWORD m_dwTextY;

};
