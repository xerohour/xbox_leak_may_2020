///////////////////////////////////////////////////////////////////////////////
//
// Name: PingScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for ping screen
//
///////////////////////////////////////////////////////////////////////////////

#include "PingScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  ping screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the ping screen
//
///////////////////////////////////////////////////////////////////////////////

CPingScreen::CPingScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the ping screen
	//

	: CUserInterfaceScreen( pParent, 
	                        PING_SCREEN_FG_COLOR, 
							PING_SCREEN_BG_COLOR, 
							PING_SCREEN_X, 
							PING_SCREEN_Y ),

	//
	// the text
	//

	  m_aText( PING_A_TEXT, PING_A_X, PING_A_Y, PING_A_FG_COLOR, PING_A_BG_COLOR ),
      m_bText( PING_B_TEXT, PING_B_X, PING_B_Y, PING_B_FG_COLOR, PING_B_BG_COLOR ),

	//
	// start with no thread
	//

	  m_hThread( NULL ),
	  
	//
	// the final hr value from the ping
	//
	
	 m_hrFinal( 0xFFFFFFFF ),

	 //
	 // the ping text index
	 //

	 m_dwPingTextIndex( 0 ),
	 
	 //
	 // init state of ping
	 //

	 m_bPingOnce( FALSE )

{
	//
	// set the initial text
	//

	SetText( PING_TEXTS[m_dwPingTextIndex] );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );

	//
	// hide the text
	//

	m_aText.Hide( TRUE );
	m_bText.Hide( TRUE );

}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  ping screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the ping screen
//
///////////////////////////////////////////////////////////////////////////////

CPingScreen::~CPingScreen( void )
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
// Name: GeneratePing
//
// Input: None
//
// Output: S_OK if ping completed successfuly (in a DWORD)
//
// Description: tries to see if the user can log onto PartnerNet
//
///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI
CPingScreen::GeneratePing( LPVOID lpVoid )
{
	HRESULT hr = S_OK;
	HRESULT hrTask = S_OK;

	//
	// initiate a quick logon
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = COnlineAccounts::Instance()->QuickLogonStart();
	}

	//
	// end the logon
	//

	hrTask = COnlineAccounts::Instance()->QuickLogonFinish();

	//
	// fire back the final hr.
	//

	return SUCCEEDED( hr ) ? hrTask : hr;

}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateText
//
// Input: None
//
// Output: None
//
// Description: Updates the screen text 
//
///////////////////////////////////////////////////////////////////////////////

void
CPingScreen::UpdateText( void )
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
			// still pinging parterNET
			//

			if ( STILL_ACTIVE == hrThread )
			{
				SetText( PING_TEXTS[++m_dwPingTextIndex % NUM_TEXT_STATES] );
			}

			//
			// if the handle is S_OK, then we pinged without a prob
			//

			else if ( SUCCEEDED( hrThread ) )
			{
				SetText( PING_SUCCEEDED_TEXT );
				m_hrFinal = S_OK;
			}

			//
			// otherwise, we did not successfuly ping
			//

			else
			{
				SetText( PING_FAILED_TEXT );
				m_hrFinal = E_FAIL;
			}
		}

		//
		// some how getexitcode bailed
		//

		else
		{
			SetText( PING_FAILED_TEXT );
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
CPingScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
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
	// if we have pinged, and either a or b is pressed
	//

	if ( TRUE == m_bPingOnce && pInput->APressed() )
	{
		//
		// we can come back to this screen, so reset internal state
		//

		m_bPingOnce = FALSE;
		m_hrFinal = 0xFFFFFFFF;
		m_dwPingTextIndex = 0;

		//
		// hide a again
		//

		m_aText.Hide( TRUE );
		m_bText.Hide( TRUE );

		//
		// reset the text
		//

		SetText( PING_TEXTS[m_dwPingTextIndex] );

		//
		// goto the parent screen
		//

		pRet = ParentScreen();

		//
		// play the back sound
		//

		hr = pAudio->PlaySelectButton();
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
		// we've done the ping
		//

		m_bPingOnce = TRUE;

		//
		// unhide text
		//

		m_aText.Hide( FALSE );
	//	m_bText.Hide( FALSE );

	}

	//
	// if the thread was never created
	//

	else if ( ( NULL == m_hThread || INVALID_HANDLE_VALUE == m_hThread ) && FALSE == m_bPingOnce )
	{
		//
		// set the text index back to 0
		//

		m_dwPingTextIndex = 0;

		//
		// update the text
		//

		SetText( PING_TEXTS[m_dwPingTextIndex] );

		//
		// create the thread
		//

		m_hThread = CreateThread( NULL, 0, &CPingScreen::GeneratePing, NULL, 0, NULL );
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

			SetText( PING_FAILED_TEXT );

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