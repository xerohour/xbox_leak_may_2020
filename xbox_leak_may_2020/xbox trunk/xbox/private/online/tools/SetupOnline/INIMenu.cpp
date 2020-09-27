///////////////////////////////////////////////////////////////////////////////
//
// Name: IniMenu.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the ini menu
//
///////////////////////////////////////////////////////////////////////////////

#include "INIMenu.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: Ini Menu constructor
//
// Input: szTitle - the title of the device menu
//        dwFGColor - the foreground text color
//        dwBGColor - the background text color
//        dwTextX - the x location of the text
//        dwTextY - the y location of the text
//        apUserList - the list of users for this menu
//        ppClipboardUser - the user being copied from another menu
//        pSelectText - the text object used to display select text
//        pAddUserText - the text object used to display add user text
//        pEraseUserText - the text object used to display erase user text
//
// Output: None
//
// Description: creates an instance of the ini menu class
//
///////////////////////////////////////////////////////////////////////////////

CINIMenu::CINIMenu( LPSTR szTitle, 
				    DWORD dwFGColor, 
					DWORD dwBGColor, 
					DWORD dwTextX, 
					DWORD dwTextY, 
					PXONLINEP_USER* apUserList, 
					PXONLINEP_USER* ppClipboardUser,
					CUserInterfaceText* pSelectText,
					CUserInterfaceText* pAddUserText,
					CUserInterfaceText* pEraseUserText )

	//
	// create the parent menu
	//


  : CUserInterfaceMenu( szTitle, 
                        dwFGColor, 
						dwBGColor, 
						dwTextX, 
						dwTextY ),

	m_dwSelected( -1 ),
	m_apUserList( apUserList ),
	m_ppClipboardUser( ppClipboardUser ),
	m_pSelectText( pSelectText ),
	m_pAddUserText( pAddUserText ),
	m_pEraseUserText( pEraseUserText )
{
	//
	// no code here
	//
}


///////////////////////////////////////////////////////////////////////////////
//
// Name: Ini Menu constructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the ini menu class
//
///////////////////////////////////////////////////////////////////////////////

CINIMenu::~CINIMenu( void )
{
	//
	// no code here
	//
}


///////////////////////////////////////////////////////////////////////////////
//
// Name: Ini Menu constructor
//
// Input: pInput - a pointer to the ui input class
//        pAudio - a pointer to the ui audio class
//
// Output: the next screen to display
//
// Description: creates an instance of the ini menu class
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CINIMenu::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;

	CUserInterfaceScreen* pRet = NULL;

	//
	// verify input pointers are good
	//

	hr = pInput && pAudio ? S_OK : E_BAD_ARG;
	MY_ASSERT( hr, S_BAD_ARG );

	if ( ! pInput || ! pAudio )
	{
		return pRet;
	}

	//
	// act on input. if up was pressed and there are multiple
	// items in this menu
	//

	if ( pInput->UpPressed() && HasMultipleItems() )
	{
		//
		// scroll up to next item
		//

		ScrollActiveItemUp();
		hr = pAudio->PlayMenuItem();
	}

	//
	// down was pressed an there are multiple items
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
	// process an a press
	//

	else if ( pInput->APressed() )
	{

		//
		// if nothing is selected on this or any menu
		//

		if ( -1 == m_dwSelected && NULL == *m_ppClipboardUser )
		{
			//
			// set the foregrond and background colors
			//

			SetFGColorActive( INI_MENU_COPY_FG_COLOR );
			SetBGColorActive( INI_MENU_COPY_BG_COLOR );

			//
			// set hte active menu item as highlited
			//

			m_dwSelected = ActiveItemIndex();

			//
			// set the clipboard user
			//

			*m_ppClipboardUser = m_apUserList[m_dwSelected];

			//
			// play the sound
			//

			hr = pAudio->PlaySelectButton();
		}

		//
		// if the current active selection is highlited
		//

		else if ( -1 != m_dwSelected && ActiveItemIndex() == m_dwSelected )
		{

			//
			// reset the font to the defualt for the menu
			//

			SetFGColorActive( FGColor() );
			SetBGColorActive( BGColor() );

			//
			// reest the clipbaord user
			//

			*m_ppClipboardUser = NULL;

			//
			// reset the highlited state
			//

			m_dwSelected = -1;

			//
			// play the audio
			//

			hr = pAudio->PlaySelectButton();
		}

	}

	//
	// decide on the rest of the text
	//

	if ( SUCCEEDED( hr ) )
	{

		//
		// erase the x text, add user text
		//

		m_pEraseUserText->Hide( TRUE );
		m_pAddUserText->Hide( TRUE );

		//
		// current active item is selected
		//

		if ( -1 != m_dwSelected && ActiveItemIndex() == m_dwSelected )
		{
			m_pSelectText->Hide( FALSE );
		}

		//
		// current active item is not selected
		//

		else if ( -1 != m_dwSelected )
		{
			m_pSelectText->Hide( TRUE );
		}
	}

	//
	// get a usabale string if audio playback failed
	//

	hr = SUCCEEDED( hr ) ? S_OK : E_AUDIO_PLAYBACK;
	MY_ASSERT( hr, S_AUDIO_PLAYBACK );

	return pRet;
}