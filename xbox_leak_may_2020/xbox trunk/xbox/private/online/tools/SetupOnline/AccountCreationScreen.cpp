///////////////////////////////////////////////////////////////////////////////
//
// Name: AccountCreationScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for AccountCreation screen
//
///////////////////////////////////////////////////////////////////////////////

#include "AccountCreationScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  account creation screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the account creation screen
//
///////////////////////////////////////////////////////////////////////////////

CAccountCreationScreen::CAccountCreationScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the account creation screen
	//

	: CUserInterfaceScreen( pParent, 
	                        CREATION_SCREEN_FG_COLOR, 
							CREATION_SCREEN_BG_COLOR, 
							CREATION_SCREEN_X, 
							CREATION_SCREEN_Y ),

	//
	// the text
	//

	  m_aText( CREATION_A_TEXT, CREATION_A_X, CREATION_A_Y, CREATION_A_FG_COLOR, CREATION_A_BG_COLOR ),
      m_bText( CREATION_B_TEXT, CREATION_B_X, CREATION_B_Y, CREATION_B_FG_COLOR, CREATION_B_BG_COLOR ),
      m_infoText( "", CREATION_INFO_X, CREATION_INFO_Y, CREATION_INFO_FG_COLOR, CREATION_INFO_BG_COLOR ),

	//
	// start with no thread
	//

	  m_hThread( NULL ),

	//
	// the final hr value from the creation
	//
	
	 m_hrFinal( 0xFFFFFFFF ),

	 //
	 // the creation text index
	 //

	 m_dwTextIndex( 0 ),

	 //
	 // init state of creation
	 //

	 m_bCreationOnce( FALSE )

{	
	//
	// set the initial text
	//

	SetText( CREATION_TEXTS[m_dwTextIndex] );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
	AddUIText( &m_infoText );

	//
	// hide the text
	//

	m_aText.Hide( TRUE );
	m_bText.Hide( TRUE );
	m_infoText.Hide( TRUE );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  account creation screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the account creation screen
//
///////////////////////////////////////////////////////////////////////////////

CAccountCreationScreen::~CAccountCreationScreen( void )
{
	//
	// check if the thread handle exists
	//

	if ( m_hThread && INVALID_HANDLE_VALUE != m_hThread )
	{
		//
		// close it off if it does
		//

		CloseHandle( m_hThread );
		m_hThread = NULL;
	}
	else if ( INVALID_HANDLE_VALUE == m_hThread )
	{
		m_hThread = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: GenerateUser
//
// Input: None
//
// Output: S_OK if user generation completed successfuly (in a DWORD)
//
// Description: tries to see create a user
//
///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI
CAccountCreationScreen::GenerateUser( LPVOID lpVoid )
{
	HRESULT hr = S_OK;

	LPSTR szName = NULL;
	LPBYTE pBytes = NULL;
	XONLINEP_USER user;

	if ( NULL == lpVoid )
	{
		return E_INVALIDARG;
	}

	//
	// zero out the user
	//
	
	ZeroMemory( &user, sizeof( XONLINEP_USER ) );

	//
	// get the inputed text
	//

	if ( SUCCEEDED( hr ) )
	{
		szName = ((SUserParams*) lpVoid)->szName;

		if ( NULL != szName )
		{
			strcpy( user.name, szName );
		}	
	}

	//
	// fill in the pin
	//

	if ( SUCCEEDED( hr ) )
	{
		pBytes = ((SUserParams*) lpVoid)->pBytes;

		if ( NULL != pBytes )
		{
			memcpy( user.pin, pBytes, XONLINE_PIN_LENGTH );

			//
			// set the require pin flag, if a pin exists
			//

			if ( 0 != *pBytes )
			{
				user.dwUserOptions |= XONLINE_USER_OPTION_REQUIRE_PIN;
			}
		}
	}

	//
	// try to create a user
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( &user, 1, FALSE );
	}

	//
	// spit bakc the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateText
//
// Input: None
//
// Output: None
//
// Description: Updates the UI text
//
///////////////////////////////////////////////////////////////////////////////

void
CAccountCreationScreen::UpdateText( void )
{
	HRESULT hrThread = S_OK;

	//
	// only update the text if a thread handle exists
	//

	if ( m_hThread && INVALID_HANDLE_VALUE != m_hThread )
	{
		//
		// see if the thread is terminated yet
		//

		if ( GetExitCodeThread( m_hThread, (LPDWORD) &hrThread ) )
		{
			//
			// if the thread is still alive, then we are
			// still creating user on parterNET
			//

			if ( STILL_ACTIVE == hrThread )
			{
				SetText( CREATION_TEXTS[++m_dwTextIndex % NUM_CREATION_TEXT_STATES] );
			}

			//
			// if a random user name was generated, warn the user
			//

			else if ( S_FALSE == hrThread )
			{
				SetText( CREATION_SUCCEEDED_TEXT );
				m_hrFinal = S_OK;

				m_infoText.SetText( CREATION_USER_EXISTS_TEXT );
				m_infoText.Hide( FALSE );
			}

			//
			// if the handle is S_OK, then we created a user without a prob
			//

			else if ( SUCCEEDED( hrThread ) )
			{
				SetText( CREATION_SUCCEEDED_TEXT );
				m_hrFinal = S_OK;

			}

			//
			// the drive may be full
			//

			else if ( E_OUTOFMEMORY == hrThread )
			{
				SetText( CREATION_DISK_FULL_TEXT );
				m_hrFinal = E_FAIL;

			}

			//
			// otherwise, we did not successfuly create the user
			//

			else
			{
				SetText( CREATION_ERROR_TEXT );
				m_hrFinal = E_FAIL;

			}
		}

		//
		// some how getexitcode bailed
		//

		else
		{
			SetText( S_GENERAL_ERROR );
			m_hrFinal = E_FAIL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input ui
//        pAudio - the audio ui
//
// Output: the next screen
//
// Description: process the input for the screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CAccountCreationScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
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
	// update the text
	//

	UpdateText();

	//
	// if we have created, and an a or b was pressed
	//

	if ( TRUE == m_bCreationOnce && ( pInput->APressed() || pInput->BPressed() ) )
	{
		//
		// we can come back to this screen, so reset internal state
		//

		m_bCreationOnce = FALSE;
		m_hrFinal = 0xFFFFFFFF;
		m_dwTextIndex = 0;

		//
		// hide a again
		//

		m_aText.Hide( TRUE );
		m_bText.Hide( TRUE );

		//
		// reset the text
		//

		SetText( CREATION_TEXTS[m_dwTextIndex] );

		//
		// hide the info text
		//

		m_infoText.Hide( TRUE );

		//
		// goto the parent screen
		//

		pRet = ParentScreen()->Update( pInput, pAudio );

	}

	//
	// if the thread has been created and is completed
	//

	else if ( ( NULL != m_hThread && INVALID_HANDLE_VALUE != m_hThread ) && 
		      ( S_OK == m_hrFinal || E_FAIL == m_hrFinal ) )
	{

		//
		// destroy the handle if it exists
		//

		if ( m_hThread && INVALID_HANDLE_VALUE != m_hThread )
		{
			CloseHandle( m_hThread );
			m_hThread = NULL;
		}

		else if ( INVALID_HANDLE_VALUE == m_hThread )
		{
			m_hThread = NULL;
		}

		//
		// decide which sound to play
		//

		if ( S_OK == m_hrFinal )
		{	
			hr = pAudio->PlaySelectButton();
		}

		else
		{
			hr = pAudio->PlayBackButton();
		}

		//
		// reset the final state
		//

		m_hrFinal = S_OK;

		//
		// we've done the creation
		//

		m_bCreationOnce = TRUE;

		//
		// unhide text
		//

		m_aText.Hide( FALSE );
		m_bText.Hide( FALSE );

	}

	//
	// if the thread was never created
	//

	else if ( ( NULL == m_hThread || INVALID_HANDLE_VALUE == m_hThread ) && FALSE == m_bCreationOnce )
	{
		//
		// set the text index back to 0
		//

		m_dwTextIndex = 0;

		//
		// update the text
		//

		SetText( CREATION_TEXTS[m_dwTextIndex] );

		//
		// if no user name was entered, generate a warning
		//

		if ( NULL == m_params.szName || '\0' == *m_params.szName )
		{
			m_infoText.SetText( CREATION_NULL_USER_TEXT );
			m_infoText.Hide( FALSE );
		}

		//
		// create the thread
		//

		m_hThread = CreateThread( NULL, 0, &CAccountCreationScreen::GenerateUser, (LPVOID) &m_params, 0, NULL );
		hr = 0 != m_hThread ? S_OK : E_MEMORY_ALLOCATION;
		MY_ASSERT( hr, S_MEMORY_ALLOCATION );

		//
		// if the thread could not be created, report it
		//

		if ( ! m_hThread )
		{
			//
			// update the text
			//

			SetText( CREATION_ERROR_TEXT );

			//
			// play the failed sound
			//

			hr = pAudio->PlayBackButton();

			//
			// do not return any screen
			//

			pRet = NULL;
		}
	}

	return pRet;
}