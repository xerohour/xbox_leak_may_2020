///////////////////////////////////////////////////////////////////////////////
//
// Name: DeviceMenu.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for device menus
//
///////////////////////////////////////////////////////////////////////////////

#include "DeviceMenu.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: Device Menu constructor
//
// Input: szTitle - the title of the device menu
//        dwFGColor - the foreground text color
//        dwBGColor - the background text color
//        dwTextX - the x location of the text
//        dwTextY - the y location of the text
//        apUserList - the list of users for this menu
//        ppClipboardUser - the user being copied from another menu
//        pSelectUserText - the a select text
//        pAddUserText - the a add text
//        pEraseUserText - the x erase text
//        pchUpdate - a pointer to the charecter used to tell the
//                    screen to update a menu
//
// Output: None
//
// Description: creates an instance of the device menu class
//
///////////////////////////////////////////////////////////////////////////////

CDeviceMenu::CDeviceMenu( LPSTR szTitle, 
						  DWORD dwFGColor, 
						  DWORD dwBGColor, 
						  DWORD dwTextX,
						  DWORD dwTextY,
						  PXONLINEP_USER* apUserList, 
						  PXONLINEP_USER* ppClipboardUser, 
						  CUserInterfaceText* pSelectUserText,
						  CUserInterfaceText* pEraseUserText,
						  CUserInterfaceText* pOverwriteText,
						  PCHAR pchUpdate )

	//
	// create the parent ui menu
	//

	: CUserInterfaceMenu( szTitle, 
	                      dwFGColor, 
						  dwBGColor, 
						  dwTextX, 
						  dwTextY ),

	  m_apUserList( apUserList ),
	  m_dwSelected( -1 ),
	  m_ppClipboardUser( ppClipboardUser ),
	  m_pSelectUserText( pSelectUserText ),
	  m_pEraseUserText( pEraseUserText ),
	  m_pOverwriteText( pOverwriteText ),
	  m_pchUpdate( pchUpdate )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Device Menu destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the device menu class
//
///////////////////////////////////////////////////////////////////////////////

CDeviceMenu::~CDeviceMenu( void )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the ui input class
//        pAudio - the ui audio class
//
// Output: a pointer to the next active screen
//
// Description: process the input for this menu
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CDeviceMenu::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;

	CUserInterfaceScreen* pRet = NULL;
	LPSTR szTitle = NULL;

	//
	// create an empty user to clear out the MU
	//

	XONLINE_USER emptyUser;
	ZeroMemory( &emptyUser, sizeof( XONLINE_USER ) );

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
	// if up was pressed and there are multiple items
	//

	if ( pInput->UpPressed() && HasMultipleItems() )
	{
		//
		// scroll up an item
		//

		ScrollActiveItemUp();
		hr = pAudio->PlayMenuItem();
	}

	//
	// if down was pressed and there are multiple items
	//

	else if ( pInput->DownPressed() && HasMultipleItems() )
	{
		//
		// scroll down an item
		//

		ScrollActiveItemDown();
		hr = pAudio->PlayMenuItem();
	}

	//
	// if a was pressed
	//

	else if ( pInput->APressed() )
	{
		//
		// nothing on this menu is selected,
		// nothing on any menu is selected
		// and there are items on this menu
		//

		if ( -1 == m_dwSelected && 
			 NULL == *m_ppClipboardUser && 
			 NumItems() > 0 )
		{
			//
			// set the foreground and background colors
			// to the chosen color
			//

			SetFGColorActive( DEVICE_MENU_COPY_FG_COLOR );
			SetBGColorActive( DEVICE_MENU_COPY_BG_COLOR );

			//
			// mark the active one as chosen
			//

			m_dwSelected = ActiveItemIndex();

			//
			// set the screen's chosen user as the chosen one
			//

			*m_ppClipboardUser = m_apUserList[m_dwSelected];

			//
			// play audio
			// 

			hr = pAudio->PlaySelectButton();
		}

		//
		// nothing on this menu is selected,
		// but there is an item selected on another menu
		//

		else if ( -1 == m_dwSelected && *m_ppClipboardUser&& NumItems() < XONLINE_MAX_HD_ONLINE_USERS )
		{
			//
			// get the title of the device
			//

			szTitle = GetTitle();

			//
			// if this is the hard disk, then the drive we want is '\0'
			//

			if ( 0 == strcmp( HARD_DISK_TITLE, szTitle ) )
			{
				szTitle = "";

				//
				// add the clipboard user as the cached user to the HD
				//
				
				hr = _XOnlineAddUserToHD( (PXONLINE_USER) *m_ppClipboardUser );
			}
			else
			{
				//
				// add the clipboard user as the cached user to the MU
				//

				hr = _XOnlineSetUserInMU( *szTitle, (PXONLINE_USER) *m_ppClipboardUser );
			}

			//
			// verify the result
			//

			hr = SUCCEEDED( hr ) || E_OUTOFMEMORY == hr || NTE_EXISTS == hr ? S_OK : hr;
			MY_ASSERT( hr, S_ADD_CACHED_USER );

			//
			// remove the items from the menu
			//

			RemoveItems();

			//
			// notify the screen that there is a device update
			//

			*m_pchUpdate = *szTitle;

			//
			// erase the clipboard user
			//

			*m_ppClipboardUser = NULL;

			//
			// play the audio
			//

			hr = pAudio->PlaySelectButton();
		}

		//
		// the current item is selected
		//

		else if ( -1 != m_dwSelected && ActiveItemIndex() == m_dwSelected )
		{
			//
			// reset it to the menus default colors
			//

			SetFGColorActive( FGColor() );
			SetBGColorActive( BGColor() );

			//
			// cancel the copy user
			//

			*m_ppClipboardUser = NULL;

			//
			// reset the selected state
			//

			m_dwSelected = -1;

			//
			// play the audio
			//

			hr = pAudio->PlaySelectButton();
		}
	}

	//
	// x is pressed, the menu has items and the items do not 
	// point to screens
	//

	else if ( pInput->XPressed() && 
		      ! NoActiveItems() && 
			  ! ActiveItemHasScreen() )
	{
		//
		// no items are selected on the menu
		//

		if ( -1 == m_dwSelected )
		{
			//
			// select the current item
			//

			m_dwSelected = ActiveItemIndex();

			//
			// get hte title
			//

			szTitle = GetTitle();

			//
			// remove the current user from the device
			//

			if ( 0 == strcmp( HARD_DISK_TITLE, szTitle ) )
			{
				szTitle = "";

				//
				// remove the user from the hd
				//

				hr = _XOnlineRemoveUserFromHD( (PXONLINE_USER) m_apUserList[m_dwSelected] );
			}
			else
			{
				//
				// remove the user from the MU
				//

				hr = _XOnlineSetUserInMU( *szTitle, &emptyUser );
			}


			MY_ASSERT( hr, S_REMOVE_CACHED_USER );

			//
			// reset the user list
			//

			m_apUserList[m_dwSelected] = NULL;

			//
			// remove items from the list
			//

			RemoveItems();

			//
			// tell the screen to update the list
			//

			*m_pchUpdate = *szTitle;

			//
			// play the audio
			//

			hr = pAudio->PlaySelectButton();
		}
	}

	//
	// decide on text
	//

	if ( SUCCEEDED( hr ) )
	{

		//
		// hide the MU warning
		//

		m_pOverwriteText->Hide( TRUE );

		//
		// nothing in list, nothing on clipboard, nothing selected
		//

		if ( 0 == NumItems() && NULL == *m_ppClipboardUser && -1 == m_dwSelected )
		{
			m_pSelectUserText->Hide( TRUE );
		//	m_pAddUserText->Hide( TRUE );
			m_pEraseUserText->Hide( TRUE );
		}

		//
		// nothing in list, somethin on clipboard, nothing selected
		//

		if ( 0 == NumItems() && *m_ppClipboardUser && -1 == m_dwSelected )
		{
			m_pSelectUserText->SetText( ACCOUNT_A_PASTE_TEXT );
			m_pSelectUserText->Hide( FALSE );
		//	m_pAddUserText->Hide( FALSE );
			m_pEraseUserText->Hide( TRUE );
		}

		//
		// something in list, nothing on clipboard, nothing selected
		//

		if ( NumItems() > 0 && NULL == *m_ppClipboardUser && -1 == m_dwSelected )
		{
			m_pSelectUserText->SetText( ACCOUNT_A_SELECT_TEXT );
			m_pSelectUserText->Hide( FALSE );
		//	m_pAddUserText->Hide( TRUE );
			m_pEraseUserText->Hide( FALSE );
		}

		//
		// something in list, something on clipboard, something selected,
		// selected item is active
		//

		if ( NumItems() > 0 && *m_ppClipboardUser && ActiveItemIndex() == m_dwSelected )
		{
			m_pSelectUserText->SetText( ACCOUNT_A_UNSELECT_TEXT );
			m_pSelectUserText->Hide( FALSE );
		//	m_pAddUserText->Hide( TRUE );
			m_pEraseUserText->Hide( TRUE );
		}

		//
		// something in list, something on clipboard, something selected,
		// selected item is not active
		//

		if ( NumItems() > 0 && *m_ppClipboardUser && ActiveItemIndex() != m_dwSelected )
		{
			m_pSelectUserText->Hide( TRUE );
		//	m_pAddUserText->Hide( TRUE );
			m_pEraseUserText->Hide( TRUE );
		}

		//
		// something in list, something on clipboard, currently an MU selected
		//

		if ( NumItems() > 0 && *m_ppClipboardUser && 0 != strcmp( HARD_DISK_TITLE, GetTitle() ) ) 
		{
			m_pOverwriteText->Hide( FALSE );
		}

		//
		// something in list, something on clipboard, currently an mu is selected, selected
		// item is on current MU
		//

		if ( NumItems() > 0 && 
			 *m_ppClipboardUser && 
			 0 != strcmp( HARD_DISK_TITLE, GetTitle() ) &&
			 ActiveItemIndex() == m_dwSelected ) 
		{
			m_pOverwriteText->Hide( TRUE );
		}

		//
		// something in list, something on clipboard, nothing selected
		//

		if ( NumItems() > 0 && *m_ppClipboardUser && -1 == m_dwSelected )
		{
			if ( NumItems() < XONLINE_MAX_HD_ONLINE_USERS )
			{
				m_pSelectUserText->SetText( ACCOUNT_A_PASTE_TEXT );
				m_pSelectUserText->Hide( FALSE );
		//		m_pAddUserText->Hide( FALSE );
			}
			else
			{
				m_pSelectUserText->Hide( TRUE );
			//	m_pAddUserText->Hide( TRUE );
			}

			m_pEraseUserText->Hide( FALSE );
		}

	}

	//
	// get a usabale string if audio playback failed
	//

	hr = SUCCEEDED( hr ) ? S_OK : E_AUDIO_PLAYBACK;
	MY_ASSERT( hr, S_AUDIO_PLAYBACK );

	return pRet;
}