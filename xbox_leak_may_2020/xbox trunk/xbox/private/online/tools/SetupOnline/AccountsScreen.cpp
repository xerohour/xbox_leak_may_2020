///////////////////////////////////////////////////////////////////////////////
//
// Name: AccountsScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the account screen
//
///////////////////////////////////////////////////////////////////////////////

#include "AccountsScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CAccountsScreen constructor
//
// Input: pParent - the parent screen of the current screen
//
// Output: None
//
// Description: creates an instance of the Account screen
//
///////////////////////////////////////////////////////////////////////////////

CAccountsScreen::CAccountsScreen( CUserInterfaceScreen* pParent )

	//
	// set up the parent user interface object
	//

	: CUserInterfaceScreen( pParent, 
	                        ACCOUNT_SCREEN_FG_COLOR, 
							ACCOUNT_SCREEN_BG_COLOR, 
							ACCOUNT_SCREEN_X, 
							ACCOUNT_SCREEN_Y ),

	//
	// create the ini menu using the first user array list
	//

/*	  m_iniMenu( INI_NAME, 
	             ACCOUNT_SCREEN_FG_COLOR, 
				 ACCOUNT_SCREEN_BG_COLOR, 
				 ACCOUNT_SCREEN_X,
				 ACCOUNT_SCREEN_Y,
				 m_aapUserLists[0], 
				 &m_pClipboardUser,
				 &m_aSelectText,
				 &m_aAddUserText,
				 &m_xText ),
*/
	//
	// create the uiText elements
	//

	  m_aText( ACCOUNT_A_SELECT_TEXT, 
	           ACCOUNT_A_X, 
		       ACCOUNT_A_Y, 
			   ACCOUNT_A_FG_COLOR, 
			   ACCOUNT_A_BG_COLOR ),

      m_xText( ACCOUNT_X_TEXT, 
	           ACCOUNT_X_X, 
			   ACCOUNT_X_Y, 
			   ACCOUNT_X_FG_COLOR, 
			   ACCOUNT_X_BG_COLOR ),

	  m_bText( ACCOUNT_B_TEXT, 
	           ACCOUNT_B_X, 
			   ACCOUNT_B_Y, 
			   ACCOUNT_B_FG_COLOR, 
			   ACCOUNT_B_BG_COLOR ),

	  m_leftText( ACCOUNT_LEFT_TEXT, 
	              ACCOUNT_LEFT_X, 
				  ACCOUNT_LEFT_Y, 
				  ACCOUNT_LEFT_FG_COLOR, 
				  ACCOUNT_LEFT_BG_COLOR ),

	  m_rightText( ACCOUNT_RIGHT_TEXT, 
	               ACCOUNT_RIGHT_X, 
				   ACCOUNT_RIGHT_Y, 
				   ACCOUNT_RIGHT_FG_COLOR, 
				   ACCOUNT_RIGHT_BG_COLOR ),

	  m_overwriteText( ACCOUNT_OVERWRITE_TEXT,
	                   ACCOUNT_OVERWRITE_X,
					   ACCOUNT_OVERWRITE_Y,
					   ACCOUNT_OVERWRITE_FG_COLOR,
					   ACCOUNT_OVERWRITE_BG_COLOR ),


	//
	// init all non array members
	//

	  m_pClipboardUser( NULL ),
	  m_dwMUs( 0 ),
	  m_bInit( FALSE ),
	  m_chUpdate( INVALID_DEVICE_CHAR ),
	  m_dwLastActive( 0 ),
	  m_bJustEntered( TRUE ),
	  m_dwBadMUs( 0 )
{
	//
	// the constructor will do some internal init, but
	// will report errors by setting the screen text
	//

	HRESULT hr = S_OK;

	//
	// zero out the array members
	//

	ZeroMemory( m_aapUserLists, sizeof( m_aapUserLists ) );
	ZeroMemory( m_apDeviceMenus, sizeof( m_apDeviceMenus ) );

	//
	// add the ui text to the screen
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
	AddUIText( &m_xText );
	AddUIText( &m_leftText );
	AddUIText( &m_rightText );
	AddUIText( &m_overwriteText );

	//
	// hide non visible text
	//

	m_xText.Hide( TRUE );
	m_leftText.Hide( TRUE );
	m_rightText.Hide( TRUE );
	m_aText.Hide( TRUE );
	m_overwriteText.Hide( TRUE );

	//
	// Parse the ini file
	//

//	hr = ParseIniFile();

	//
	// the ini file does not have to exist, so assert is not needed
	//

	//
	// if parsing of the ini file failed, report it to the user
	//

//	if ( FAILED( hr ) )
//	{
//		SetText( S_PARSE_INI_FILE );
//		m_aSelectText.Hide( TRUE );
//	}

	SetText( "" );

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

CAccountsScreen::~CAccountsScreen( void )
{
	DWORD dwError = 0;
	HRESULT hr = S_OK;

	//
	// first clean up the menu lists
	//

	CleanupLists();

	//
	// unmount the mus, destroy the list
	//

	for ( DWORD i = 0; i < NUM_DEVICE_MENUS; i++ )
	{
		//
		// delete the menu
		//

		delete m_apDeviceMenus[i];
		m_apDeviceMenus[i] = NULL;

		//
		// if hte mu was mounted, unmount it
		//

		if ( 0 != i && m_dwMUs & ( 1 << i ) )
		{
			//
			// convert from bit flag to port/slot value
			//

			dwError = XUnmountMU( ( i - 1 ) & 0xC, ( i - 1 ) >> 2 );
			hr = 0 == dwError ? S_OK : E_MU_UNMOUNT;

		//	MY_ASSERT( hr, S_MU_UNMOUNT );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ParseIniFile
//
// Input: None
//
// Output: S_OK if ini file was opened and parsed properly
//
// Description: tries to open and parse the user ini file for online
//
///////////////////////////////////////////////////////////////////////////////
/*
HRESULT 
CAccountsScreen::ParseIniFile( void )
{
	HRESULT hr = S_OK;
	FILE* pIniFile = NULL;
	int iVal = 0;
	DWORD dwCount = 0;

	PXONLINEP_USER pNewUser = NULL;

	//
	// open the ini file
	//

	if ( SUCCEEDED( hr ) )
	{
		pIniFile = fopen( INI_FILE_LOCATION, "rb" );
		hr = pIniFile ? S_OK : E_OPEN_INI_FILE;

		//
		// ini file may not exist, so no assert is needed
		//
	}

	//
	// add the ini menu to the menus
	//

	if ( SUCCEEDED( hr ) )
	{
		AddMenu( &m_iniMenu );
	}

	//
	// read the user data out of the ini file
	//

	while ( SUCCEEDED( hr ) )
	{
		//
		// create a new user pointer
		//

		pNewUser = new XONLINEP_USER();

		hr = pNewUser ? S_OK : E_MEMORY_ALLOCATION;
		MY_ASSERT( hr, S_MEMORY_ALLOCATION );

		//
		// read the data out of the ini
		//

		if ( SUCCEEDED( hr ) )
		{
			hr = ReadUserData( pIniFile, pNewUser );
			
			if ( E_OUTOFMEMORY == hr )
			{
				hr = S_OK;
				break;
			}
		}

		MY_ASSERT( hr, S_PARSE_INI_FILE );
		
		//
		// Add the user to the ini user list
		//

		if ( SUCCEEDED( hr ) )
		{
			m_aapUserLists[0][dwCount] = pNewUser;
			m_iniMenu.AddItem( pNewUser->name, NULL );
			
		}

		dwCount++;

	}

	//
	// close the ini file
	//

	
	if ( pIniFile )
	{
		iVal = fclose( pIniFile );
		pIniFile = NULL;
		hr = 0 == iVal ? S_OK : E_CLOSE_INI_FILE;

		MY_ASSERT( hr, S_CLOSE_INI_FILE );
	}

	return hr;
}
*/
///////////////////////////////////////////////////////////////////////////////
//
// Name: CleanupList
//
// Input: dwIndex - the index of the list to clean
//
// Output: None
//
// Description: runs through the specified list, and cleans it up
//
///////////////////////////////////////////////////////////////////////////////

void
CAccountsScreen::CleanupList( DWORD dwIndex )
{
	for ( DWORD j = 0; j < XONLINE_MAX_STORED_ONLINE_USERS; j++ )
	{
		//
		// only delete the list if it exists
		//

		if ( m_aapUserLists[dwIndex][j] )
		{
			delete m_aapUserLists[dwIndex][j];
			m_aapUserLists[dwIndex][j] = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: CleanupLists
//
// Input: None
//
// Output: None
//
// Description: runs through each of the lists, and cleans them up
//
///////////////////////////////////////////////////////////////////////////////

void 
CAccountsScreen::CleanupLists( void )
{
	for ( DWORD i = 0; i < NUM_MENUS; i++ )
	{
		CleanupList( i );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: PopulateDriveList
//
// Input: chDrive - a char representing a drive to mount and get data from
//
// Output: S_OK if drive mounting and list insertion completed without error,
//         S_FALSE if drive could not be mounted
//         E_XXXX if an error occured
//
// Description: tries to mount the given drive, and populates the user list
//              with the user data on the drive
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CAccountsScreen::PopulateDriveList( CHAR chDrive )
{
	HRESULT hr = S_OK;
	PXONLINEP_USER pUsers = NULL;
	PXONLINEP_USER pUser = NULL;
	DWORD dwPort = 0;
	DWORD dwSlot = 0;
	DWORD dwUsers = 0;
	CHAR chRetDrive = '\0';
	DWORD dwRet = 0;
	DWORD dwMask = 0;

	CHAR * szName = NULL;

	//
	// decide the index into the master array
	//

	DWORD dwUserIndex = 0 == chDrive ? 1 : 2 + chDrive - 'F';
	DWORD dwDeviceIndex = 0;

	//
	// if this is not the hard disk, mount the drive
	//

	if ( SUCCEEDED( hr ) && 0 != chDrive )
	{
		//
		// decide which port / slot cooresponds to the passed in drive
		//

		switch ( chDrive )
		{
			case 'F': 
				dwPort = XDEVICE_PORT0;
				dwSlot = XDEVICE_TOP_SLOT;
				dwMask = 1 << 0;
				break;

			case 'G': 
				dwPort = XDEVICE_PORT0;
				dwSlot = XDEVICE_BOTTOM_SLOT;
				dwMask = 1 << 4;
				break;

			case 'H': 
				dwPort = XDEVICE_PORT1;
				dwSlot = XDEVICE_TOP_SLOT;
				dwMask = 1 << 1;
				break;

			case 'I': 
				dwPort = XDEVICE_PORT1;
				dwSlot = XDEVICE_BOTTOM_SLOT;
				dwMask = 1 << 5;
				break;

			case 'J': 
				dwPort = XDEVICE_PORT2;
				dwSlot = XDEVICE_TOP_SLOT;
				dwMask = 1 << 2;
				break;

			case 'K': 
				dwPort = XDEVICE_PORT2;
				dwSlot = XDEVICE_BOTTOM_SLOT;
				dwMask = 1 << 6;
				break;

			case 'L': 
				dwPort = XDEVICE_PORT3;
				dwSlot = XDEVICE_TOP_SLOT;
				dwMask = 1 << 3;
				break;

			case 'M': 
				dwPort = XDEVICE_PORT3;
				dwSlot = XDEVICE_BOTTOM_SLOT;
				dwMask = 1 << 7;
				break;
		}

		//
		// only mount if not mounted
		//

		if ( ! ( m_dwMUs & dwMask ) )
		{
			dwRet = XMountMU( dwPort, dwSlot, &chRetDrive );

			//
			// if the drive could not be mounted, short circuit the function
			//

			if ( ERROR_DEVICE_NOT_CONNECTED  == dwRet )
			{
				return S_FALSE;
			}

			//
			// make sure mounting of the mu succeeded
			//

			hr = ERROR_SUCCESS == dwRet ? S_OK : E_MU_MOUNT;

			//
			// save the mounted MU Id
			//

			if ( SUCCEEDED( hr ) )
			{
				m_dwMUs |= dwMask;
			}

			//
			// save the bad MU id
			//

			else
			{
				m_dwBadMUs |= dwMask;
			}

			//
			// make sure we got back the expected drive letter
			//

			if ( SUCCEEDED( hr ) )
			{
				hr = toupper( chDrive ) == toupper( chRetDrive ) ? S_OK : E_MU_MOUNT;
			}

		}

		//
		// calcualte the device index
		//
	
		dwDeviceIndex = ( ( dwSlot << 2 ) | dwPort ) + 1;
	}

	//
	// Allocate an array of online users
	//

	if ( SUCCEEDED( hr ) )
	{
		pUsers = new XONLINEP_USER[XONLINE_MAX_STORED_ONLINE_USERS];

		hr = pUsers ? S_OK : E_MEMORY_ALLOCATION;
		MY_ASSERT( hr, S_MEMORY_ALLOCATION );
	}

	//
	// Get the users from the drives
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// Get Users from HD
		//

		if ( 0 == chDrive )
		{

			hr = _XOnlineGetUsersFromHD( (PXONLINE_USER) pUsers, &dwUsers );
			MY_ASSERT( hr, S_GET_USERS_FAILED );

			//
			// make sure we got a sane amount of users
			//

			if ( SUCCEEDED( hr ) )
			{
				hr = dwUsers <= XONLINE_MAX_HD_ONLINE_USERS ? S_OK : E_INVALID_USER_COUNT; 
				MY_ASSERT( hr, S_INVALID_USER_COUNT );
			}
		}

		//
		// otherwise, get them from MU
		//

		else
		{
			hr = _XOnlineGetUserFromMU( dwPort, dwSlot, (PXONLINE_USER) pUsers );

			//
			// only 1 user on an MU
			//

			if ( SUCCEEDED( hr ) )
			{
				dwUsers = 1;
			}

			//
			// no users on the MU
			//

			else if ( XONLINE_E_NO_USER == hr )
			{
				hr = S_OK;
				dwUsers = 0;
			}
			
			//
			// otherwise, it's an errpr
			//

			else
			{
				MY_ASSERT( hr, S_GET_USERS_FAILED );
			}
		}
	}

	//
	// allocate the menu for each user
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// delete the user before allocating a new one
		//

		delete m_apDeviceMenus[dwDeviceIndex];

		m_apDeviceMenus[dwDeviceIndex] = new CDeviceMenu( MU_NAMES[dwDeviceIndex], 
			                                              FGColor(), 
														  BGColor(), 
														  ACCOUNT_SCREEN_X,
														  ACCOUNT_SCREEN_Y,
														  m_aapUserLists[dwDeviceIndex + 1], 
														  &m_pClipboardUser, 
														  &m_aText,
														  &m_xText,
														  &m_overwriteText,
														  &m_chUpdate );

		hr = m_apDeviceMenus[dwDeviceIndex] ? S_OK : E_MEMORY_ALLOCATION;
		MY_ASSERT( hr, S_MEMORY_ALLOCATION );
	}

	//
	// rebuild the menus
	//

	RebuildMenus();

	//
	// insert the users into the list
	//

	for ( DWORD i = 0; i < dwUsers && SUCCEEDED( hr ); i++ )
	{
		//
		// allocate 1 user
		//

		if ( SUCCEEDED( hr ) )
		{
			pUser = new XONLINEP_USER();
			hr = pUser ? S_OK : E_MEMORY_ALLOCATION;
			MY_ASSERT( hr, S_MEMORY_ALLOCATION );
		}

		//
		// copy the data out of the user array
		//

		if ( SUCCEEDED( hr ) )
		{
			CopyMemory( pUser, pUsers + i, sizeof( XONLINEP_USER ) );
		}

		//
		// insert the user into the list
		//

		if ( SUCCEEDED( hr ) )
		{
			//
			// 
			//

			m_aapUserLists[dwDeviceIndex+1][i] = pUser;

			//
			// create a copy of the name 
			//

			szName = new CHAR[XONLINE_USERNAME_SIZE + 2];

			//
			// make sure the name string was allocated
			//

			if ( NULL == szName )
			{
				hr = E_MEMORY_ALLOCATION;
				MY_ASSERT( hr, S_MEMORY_ALLOCATION );
			}

			//
			// make the name online friendly
			//

			if ( SUCCEEDED( hr ) )
			{
				sprintf( szName, "%s.%s", pUser->name, pUser->kingdom ); 

				m_apDeviceMenus[dwDeviceIndex]->AddItem( szName, NULL );
			}
		}
	}

	//
	// destroy the user array
	//

	delete [] pUsers;
	pUsers = NULL;

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Rebuild Menus
//
// Input: None
//
// Output: None
//
// Description: rebuilds the menus
//
///////////////////////////////////////////////////////////////////////////////

void
CAccountsScreen::RebuildMenus( void )
{
	//
	// save off the last active menu index
	//

	m_dwLastActive = ActiveMenuIndex();

	//
	// rebuild the list so menus are in correct order
	//

	RemoveMenus();

	//
	// add the ini menu back
	//

//	AddMenu( &m_iniMenu );

	//
	// add the hard disk and mus back
	//

	for ( DWORD dw = 0; dw < 5; dw++ )
	{
		//
		// menus are added like this to preserve letter order
		//

		if ( NULL != m_apDeviceMenus[dw] )
		{
			AddMenu( m_apDeviceMenus[dw] );
		}

		//
		// make sure we do not add the same menu twice
		//

		if ( NULL != m_apDeviceMenus[dw+4] && 0 != dw )
		{
			AddMenu( m_apDeviceMenus[dw+4] );
		}
	}

	//
	// make the last active menu the currently active menu after the
	// menu recreation
	//

	for ( DWORD i = 0; i < m_dwLastActive; i++ )
	{
		ScrollActiveMenuDown();
	}

	//
	// erase the clipboard
	//

	m_pClipboardUser = NULL;

	//
	// set the a text as select
	//

	m_aText.SetText( ACCOUNT_A_SELECT_TEXT );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: PopulateDriveLists
//
// Input: None
//
// Output: S_OK if drive mounting completed without error
//
// Description: runs through each of the valid drives, mounts them, and
//              populates the cooresponding user list if possible
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CAccountsScreen::PopulateDriveLists( void )
{
	HRESULT hr = S_OK;

	//
	// the drive letters of the valid drives where user data can live
	//

	static const CHAR chDrives[] = { 0, 
									 'F',
									 'G',
									 'H',
									 'I',
									 'J',
									 'K',
									 'L',
									 'M' };


	
	//
	// go through each drive, and populate it's list
	//

	for ( DWORD i = 0; i < NUM_DEVICE_MENUS && SUCCEEDED( hr ); i++ )
	{
		hr = PopulateDriveList( chDrives[i] );
//		MY_ASSERT( hr, S_POPULATE_DRIVE_LIST );
	}

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateLists
//
// Input: None
//
// Output: S_OK if updating the lists succeeded, E_XXXX otherwise
//
// Description: checks for mu insertions / deletions and acts on them
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CAccountsScreen::UpdateLists( void )
{	
	HRESULT hr = S_OK;
	DWORD dwInsertions = 0;
	DWORD dwRemovals = 0;
	DWORD dwError = 0;
	DWORD dwPort = 0;
	DWORD dwSlot = 0;
	DWORD dwMask = 1;

	CHAR chDrive = '\255';

	//
	// get the changed mu state
	//

	if ( XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwInsertions, &dwRemovals ) )
	{
		//
		// strip off useless bits
		//

		dwRemovals &= 0x000F000F;
		dwInsertions &= 0x000F000F;

		//
		// handle insertions / removals
		//

		for ( DWORD i = 0;
			  i < 8 && SUCCEEDED( hr );
			  i++,
			  dwRemovals >>= 1,
			  dwInsertions >>= 1 )
		{
			//
			// handle removals
			//

			if ( dwRemovals & 0x1 && !( m_dwBadMUs & ( 1 << i ) ) && SUCCEEDED( hr ) )
			{
				//
				// unmount the mu
				//

				dwError = XUnmountMU( dwPort, dwSlot );
				hr = ERROR_SUCCESS == dwError ? S_OK : E_MU_UNMOUNT;
			//	MY_ASSERT( hr, S_MU_UNMOUNT );

				if ( SUCCEEDED( hr ) )
				{

					//
					// clear out the flag
					//

					m_dwMUs &= ~( 1 << i );

					//
					// empty out the list
					//

					CleanupList( i + 2 );

					//
					// remove the mu from the menus
					//

					delete m_apDeviceMenus[i + 1];
					m_apDeviceMenus[i + 1] = NULL;

					//
					// rebuild the menus
					//

					RebuildMenus();
				}

			} 
			else if ( m_dwBadMUs & 0x1 )
			{
				m_dwBadMUs &= ~( 1 << i );
			}

			//
			// handle insertions
			//

			if ( dwInsertions & 0x1 && SUCCEEDED( hr ) )
			{
				//
				// decide what the drive letter should be
				//

				chDrive = (CHAR) ( ( dwPort << 1 ) | dwSlot ) + 'F';	

				//
				// update the drive list for that drive
				//

				hr = PopulateDriveList( chDrive );
			//	MY_ASSERT( hr, S_POPULATE_DRIVE_LIST );
			}
		
			//
			// goto the next mask
			//
			
			dwMask <<= 1; 

			//
			// goto the next port
			//

			dwPort++;

			//
			// reset values if after the 3rd mu
			//

			if ( 3 == i )
			{

				//
				// goto the next slot
				//

				dwSlot = ( dwSlot + 1 ) % 2;

				//
				// skip over unsed bits
				//

				dwRemovals >>= 12;
				dwInsertions >>= 12;

				//
				// reset port and slot values
				//

				dwPort = 0;
			}
		}		
	}

	//
	// spit back the hresult
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: InitDevices
//
// Input: None
//
// Output: S_OK if device init succeeded, E_XXXX otherwise
//
// Description: initializes the devices
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CAccountsScreen::InitDevices( void )
{
	HRESULT hr = S_OK;

	//
	// only do this the first time
	//

	if ( SUCCEEDED( hr ) && ! m_bInit )
	{
		SetText( NULL );

		//
		// Populate the drive list
		//

		hr = PopulateDriveLists();
//		MY_ASSERT( hr, S_POPULATE_DRIVE_LIST );	

		//
		// set state to initialized
		//

		m_bInit = TRUE;
		m_bJustEntered = FALSE;

	}

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - a pointer to the user interface input class
//        pAudio - a pointer to the ui audio class
//
// Output: a pointer to the new screen. if this is null, the screen should
//         stay the same
//
// Description: initializes the devices
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen*
CAccountsScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pRet = NULL;

	//
	// make sure params are good
	//

	hr = pInput && pAudio ? S_OK : E_BAD_ARG;
	MY_ASSERT( hr, S_BAD_ARG );

	if ( ! pInput || ! pAudio )
	{
		return pRet;
	}

	//
	// init the devices
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = InitDevices();
//		MY_ASSERT( hr, S_INIT_DEVICES );
	}

	//
	// if we just entered the screen, update the lists
	//

	if ( SUCCEEDED( hr ) && TRUE == m_bJustEntered )
	{
		hr = PopulateDriveLists();
//		MY_ASSERT( hr, S_POPULATE_DRIVE_LIST );

		if ( FAILED( hr ) )
		{
			SetText( S_POPULATE_DRIVE_LIST );
		}

		m_bJustEntered = FALSE;
	}

	//
	// process the input
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// if left was pressed and there are multiple menus
		//

		if ( pInput->LeftPressed() && HasMultipleMenus() )
		{
			//
			// scroll up
			//

			ScrollActiveMenuUp();
			hr = pAudio->PlayMenuItem();
		}

		//
		// if right was pressed and there are multiple menus
		//

		else if ( pInput->RightPressed() && HasMultipleMenus() )
		{

			//
			// scroll down
			//

			ScrollActiveMenuDown();
			hr = pAudio->PlayMenuItem();
		}

		//
		// if b was pressed
		//

		else if ( pInput->BPressed() )
		{
			//
			// switch to the parent screen
			//

			pRet = ParentScreen();
			hr = pAudio->PlayBackButton();

			//
			// we should set up for regen of lists
			//

			m_bJustEntered = TRUE;
		}

		//
		// if there are menus on the screen and no text to display
		//

		else if ( ! NoActiveMenus() && ! GetText() )
		{
			//
			// let the active menu handle the input
			//

			pRet = ActiveMenu()->Update( pInput, pAudio );
		} 

		//
		// if a was pressed when text was written
		//

		else if ( GetText() && pInput->APressed() )
		{
			pRet = ParentScreen();
			hr = pAudio->PlayBackButton();
		}

		//
		// get a usabale string if audio playback failed
		//

		hr = SUCCEEDED( hr ) ? S_OK : E_AUDIO_PLAYBACK;

		if ( FAILED( hr ) )
		{
			SetText( S_AUDIO_PLAYBACK );
		}
	}

	//
	// update the screen text
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// activate the left and right arrows
		//

		m_leftText.Hide( FALSE );
		m_rightText.Hide( FALSE );

		//
		// if we are on the ONLY menu
		//

		if ( FirstActive() && LastActive() )
		{
			m_leftText.Hide( TRUE );
			m_rightText.Hide( TRUE );
		}

		//
		// if we are on the first menu
		//

		else if ( FirstActive() )
		{
			//
			// hide the left indicators
			//

		//	m_leftText.Hide( TRUE );

		}

		//
		// if we are on the last menu
		//

		else if ( LastActive() )
		{
			//
			// hide the right indicators
			//

		//	m_rightText.Hide( TRUE );
		}

		//
		// if there is only one menu
		//

		else if ( ! HasMultipleMenus() )
		{
			//
			// hide both
			//

			m_leftText.Hide( TRUE );
			m_rightText.Hide( TRUE );
		}

		if ( GetText() )
		{
			m_leftText.Hide( TRUE );
			m_rightText.Hide( TRUE );
			m_aText.Hide( TRUE );
		}
	}

	//
	// update the lists
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = UpdateLists();
	//	MY_ASSERT( hr, S_UPDATE_LISTS );

	//	if ( FAILED( hr ) )
	//	{
	//		SetText( S_UPDATE_LISTS );
	//	}
	}

	//
	// if one of the menus has changed state, update the menus
	//

	if ( SUCCEEDED( hr ) && '\255' != m_chUpdate )
	{
		hr = PopulateDriveList( m_chUpdate );
		hr = PopulateDriveLists();
	//	MY_ASSERT( hr, S_POPULATE_DRIVE_LIST );

	//	if ( FAILED( hr ) )
	//	{
	//		SetText( S_POPULATE_DRIVE_LIST );
	//	}

		m_chUpdate = '\255';
	}

	//
	// spit back the hr
	//

	return pRet;
}
