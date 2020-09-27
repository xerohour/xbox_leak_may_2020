///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the user interface screen class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceScreen.h"
#include "UserInterfaceMenu.h"
#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: Constructor
//
// Input: pParent - the parent screen for this screen
//        dwFGColor - the fg color
//        dwBGColor - the bg color
//        dwTextX - the x location of text
//        dwTextY - the Y location of text
//
// Output: an instance of the UI Screen
//
// Description: constructs a UI Screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen::CUserInterfaceScreen( CUserInterfaceScreen* pParent, 
										    DWORD dwFGColor, 
											DWORD dwBGColor, 
											DWORD dwTextX, 
											DWORD dwTextY )
	: m_menus(),
	  m_uiText(),
	  m_pParent( pParent ),
	  m_szText( NULL ),
	  m_dwActive( 0 ),
	  m_dwFGColor( dwFGColor ),
	  m_dwBGColor( dwBGColor ),
	  m_dwTextX( dwTextX ),
	  m_dwTextY( dwTextY )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Destructor
//
// Input: None
//
// Output: None
//
// Description: destroys a UI Screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen::~CUserInterfaceScreen( void )
{
	//
	// remove all menus
	//

	RemoveMenus();

	//
	// remove ui text
	//

	while ( ! m_uiText.empty() )
	{
		m_uiText.pop_back();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: SetText
//
// Input: szText - the text to set for the screen
//
// Output: None
//
// Description: updates the text with the new text
//
///////////////////////////////////////////////////////////////////////////////

void 
CUserInterfaceScreen::SetText( LPSTR szText )
{
	m_szText = szText;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: GetText
//
// Input: None
//
// Output: the text held in the Screen
//
// Description: retrieves the text on the screen. Can be NULL
//
///////////////////////////////////////////////////////////////////////////////

LPSTR
CUserInterfaceScreen::GetText( void )
{
	return m_szText;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: AddMenu
//
// Input: pMenu - the menu to add to the screen
//
// Output: None
//
// Description: adds a menu to the current screen
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceScreen::AddMenu( CUserInterfaceMenu* pMenu )
{
	//
	// make sure the pointer is good
	//

	ASSERT( pMenu || !"Menu pointer can not be null" );

	//
	// insert the menu pointer
	//

	if ( pMenu && ! m_szText )
	{
		m_menus.push_back( pMenu );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: AddUIText
//
// Input: pText - the ui text to add to the screen
//
// Output: None
//
// Description: adds ui text to the screen
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceScreen::AddUIText( CUserInterfaceText* pText )
{
	//
	// make sure the pointer is good
	//

	ASSERT( pText || !"UI text pointer can not be null" );

	//
	// insert the pointer in the menu
	//

	if ( pText )
	{
		m_uiText.push_back( pText );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: NumMenus
//
// Input: None
//
// Output: the number of menus on the screen
//
// Description: returns the number of menus associated with the screen
//
///////////////////////////////////////////////////////////////////////////////

DWORD
CUserInterfaceScreen::NumMenus( void )
{
	return m_menus.size();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: NumUIText
//
// Input: None
//
// Output: the number of ui text objects on the screen
//
// Description: returns the number of ui text objects on the screen
//
///////////////////////////////////////////////////////////////////////////////

DWORD
CUserInterfaceScreen::NumUIText( void )
{
	return m_uiText.size();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Menu
//
// Input: dwIndex - the index of the menu
//
// Output: a pointer ot the menu at hte inputted index
//
// Description: gets a menu at the desired index
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceMenu* 
CUserInterfaceScreen::Menu( DWORD dwIndex )
{
	//
	// make sure the index is valid
	//

	ASSERT( dwIndex < NumMenus() || !"index out of range" );

	//
	// get the menu
	//

	if ( dwIndex < NumMenus() )
	{
		return m_menus[dwIndex];
	}
	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UIText
//
// Input: dwIndex - the index of the ui text
//
// Output: a pointer ot the text at hte inputted index
//
// Description: gets a text object at the desired index
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceText*
CUserInterfaceScreen::UIText( DWORD dwIndex )
{
	//
	// make sure the index is valid
	//

	ASSERT( dwIndex < NumUIText() || !"index out of range" );

	//
	// get the ui text
	//

	if ( dwIndex < NumUIText() )
	{
		return m_uiText[dwIndex];
	}
	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: RemoveMenu
//
// Input: None
//
// Output: None
//
// Description: removes the menus from the screen
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceScreen::RemoveMenus( void )
{ 
	//
	// go through the menus until the menu list is empty
	//

	while ( ! m_menus.empty() ) { 
		m_menus.pop_back(); 
	}

	//
	// reset the active menu
	//

	m_dwActive = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input ui class
//        pAudio - the input audio class
//
// Output: the next screen to display
//
// Description: process input
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen*
CUserInterfaceScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pRet = NULL;

	//
	// make sure inputs are good
	//

	ASSERT( pInput && pAudio || !"input can not be null" );

	if ( ! pInput || ! pAudio )
	{
		return pRet;
	}

	//
	// if left pressed and there are multiple menus
	//

	if ( pInput->LeftPressed() && HasMultipleMenus() )
	{

		//
		// scroll up a menu
		//

		ScrollActiveMenuUp();
		hr = pAudio->PlayMenuItem();
	}

	//
	// if right is pressed and there are multiple menus
	//

	else if ( pInput->RightPressed() && HasMultipleMenus() )
	{
		//
		// scroll down a menu
		//

		ScrollActiveMenuDown();
		hr = pAudio->PlayMenuItem();
	}

	//
	// if b is pressed and there is a parent screen
	//

	else if ( pInput->BPressed() && ParentScreen() )
	{
		//
		// goto the parent screen
		//

		pRet = ParentScreen();
		hr = pAudio->PlayBackButton();
	}

	//
	// if a is pressed and no menus exist
	//

	else if ( pInput->APressed() && NoActiveMenus() )
	{
		//
		// this acts as a selection
		//

		hr = pAudio->PlaySelectButton();
	}

	//
	// if x is pressed and no active menus
	//

	else if ( pInput->XPressed() && NoActiveMenus() )
	{
		//
		// this acts as an X selection
		//

		hr = pAudio->PlayKeyboardStroke();
	}

	//
	// otherwise if there are menus and no text to display
	//

	else if ( ! NoActiveMenus() && ! GetText() )
	{

		//
		// let the active menu drive the input
		//

		pRet = ActiveMenu()->Update( pInput, pAudio );
	} 

	ASSERT( SUCCEEDED( hr ) || !"could not play audio file" );

	return pRet;
}
