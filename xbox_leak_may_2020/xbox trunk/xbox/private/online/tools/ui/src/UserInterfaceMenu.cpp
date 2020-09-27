///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceMenu.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the base menu class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceMenu.h"
#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CUserInterfaceMenu constructor
//
// Input: szTitle - the title of the menu
//        dwFGColor - the default text fg color
//        dwBGColor - the defailt text bg color
//        dwTextX - the x location for text 
//        dwTextY - the y location for text
//
// Output: None
//
// Description: creates an instance of menu class
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceMenu::CUserInterfaceMenu( LPSTR szTitle, 
									    DWORD dwFGColor, 
										DWORD dwBGColor, 
										DWORD dwTextX, 
										DWORD dwTextY )
	: m_items(),
	  m_screens(),
	  m_dwActive( 0 ),
	  m_dwFGColor( dwFGColor ),
	  m_dwBGColor( dwBGColor ),
	  m_dwFGColors(),
	  m_dwBGColors(),
	  m_szTitle( szTitle ),
	  m_dwTextX( dwTextX ),
	  m_dwTextY( dwTextY )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: CAccountsScreen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the Account screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceMenu::~CUserInterfaceMenu( void )
{
	//
	// remove the items from the menu
	//

	RemoveItems();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: AddItem
//
// Input: szStr - the string to be displayed for this item
//        pScreen - the screen to goto if this item is selected, can be null
//
// Output: None
//
// Description: adds an item to a menu
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceMenu::AddItem( LPSTR szStr, CUserInterfaceScreen* pScreen )
{
	ASSERT( szStr || !"need to pass in a valid string" );

	//
	// if the string was passed in
	//

	if ( szStr )
	{

		//
		// add the string and the screen to the lists
		//

		m_items.push_back( szStr );
		m_screens.push_back( pScreen );

		//
		// set the colors to the defailt
		//

		m_dwFGColors.push_back( m_dwFGColor );
		m_dwBGColors.push_back( m_dwBGColor );

		//
		// update the x drawing location for later
		//

		if ( NumItems() > 1 )
		{
			m_dwTextY -= 14;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: NumImtes
//
// Input: None
//
// Output: DWORD - any non negative value is possible. 0 means there is nothing
//
// Description: returns the number of items in the menu
//
///////////////////////////////////////////////////////////////////////////////

DWORD
CUserInterfaceMenu::NumItems( void )
{
	return m_items.size();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Item
//
// Input: dwIndex - an index into the array of items
//
// Output: the string at the index
//
// Description: returns the item string at the index provided
//
///////////////////////////////////////////////////////////////////////////////

LPSTR
CUserInterfaceMenu::Item( DWORD dwIndex )
{
	ASSERT( dwIndex < NumItems() || !"index out of range" );

	//
	// as long as the index is in range, return the string
	//

	if ( dwIndex < NumItems() )
	{
		return m_items[dwIndex];
	}

	//
	// otherwise return null
	//

	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: RemoveItems
//
// Input: None
//
// Output: None
//
// Description: removes everything from the menu
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceMenu::RemoveItems( void )
{ 
	//
	// go through the items until they are empty
	//

	while ( ! m_items.empty() ) { 

		//
		// pop the string, the screen
		//

		m_items.pop_back(); 
		m_screens.pop_back();

		//
		// and the colors
		//

		m_dwFGColors.pop_back();
		m_dwBGColors.pop_back();

		//
		// update the y location of text
		//

		if ( NumItems() > 1 )
		{
			m_dwTextY += 14;
		}
	}

	//
	// reset the active value
	//

	m_dwActive = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the ui input class
//        pAudio - the ui audio class
//
// Output: the next screen to display
//
// Description: does the default input handling for a menu
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen*
CUserInterfaceMenu::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;

	CUserInterfaceScreen* pRet = NULL;

	//
	// make sure params are good
	//

	ASSERT( pInput && pAudio || !"Parameters must not be null" );

	if ( ! pInput || ! pAudio )
	{
		return pRet;
	}

	//
	// process the input
	//

	//
	// if up was pressed and there are multiple items
	//

	if ( pInput->UpPressed() && HasMultipleItems() )
	{
		//
		// scroll up
		//

		ScrollActiveItemUp();
		hr = pAudio->PlayMenuItem();
	}

	//
	// down pressed and multiple items
	//

	else if ( pInput->DownPressed() && HasMultipleItems() )
	{
		//
		// scroll down
		//

		ScrollActiveItemDown();
		hr = pAudio->PlayMenuItem();
	}

	//
	// a pressed and no items active
	//

	else if ( pInput->APressed() && NoActiveItems() )
	{
		hr = pAudio->PlaySelectButton();
	} 

	//
	// a pressed an the active item does not have a screen
	//

	else if ( pInput->APressed() && ! ActiveItemHasScreen() )
	{
		hr = pAudio->PlaySelectButton();
	}

	//
	// a pressed and the active item does have a screen
	//

	else if ( pInput->APressed() && ActiveItemHasScreen() )
	{
		pRet = ActiveScreen();
		hr = pAudio->PlaySelectButton();
	}

	//
	// x is pressed and there are active items
	// and the active item has no screen
	//

	else if ( pInput->XPressed() && ! NoActiveItems() && ! ActiveItemHasScreen() )
	{
		hr = pAudio->PlayKeyboardStroke();
	}

	ASSERT( SUCCEEDED( hr ) || !"could not play audio file" );

	return pRet;
}

	
