///////////////////////////////////////////////////////////////////////////////
//
// Name: MachineCreationScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for machine creation screen
//
// Note: DEPRECATED
//
///////////////////////////////////////////////////////////////////////////////

#include "MachineCreationScreen.h"
#include <OnlineAccounts.h>

///////////////////////////////////////////////////////////////////////////////
//
// Name: Machine account creation screen constructor
//
// Input: pChild - the child screen
//
// Output: None
//
// Description: creates an instance of the machine creation screen
//
///////////////////////////////////////////////////////////////////////////////

CMachineCreationScreen::CMachineCreationScreen( CUserInterfaceScreen* pParent, CUserInterfaceScreen* pChild )
  : CUserInterfaceScreen( pParent, 0xFFFFFFFF, 0xFF000000, 320, 250 ),
    m_aText( "(A) Action", 100, 400, 0xFF00FF00, 0xFF000000 ),
	m_bText( "(B) Back", 520, 400, 0xFFFF0000, 0xFF000000 ),
    m_pChildScreen( pChild ),
	m_state( STATE_VERIFYING )
{
	//
	// set the text
	//

	SetText( "Machine Account Already Exists" );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Machine account creation screen destructor
//
// Input: pChildScreen - the child of this screen
//
// Output: None
//
// Description: destroys an instance of the machine creation screen
//
///////////////////////////////////////////////////////////////////////////////

CMachineCreationScreen::~CMachineCreationScreen( void )
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
// Output: the screen to goto
//
// Description: process input
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CMachineCreationScreen::Update( CUserInterfaceInput* pInput, 
		                        CUserInterfaceAudio* pAudio )
{
	//
	// Setup error codes
	//

	HRESULT	hr = S_OK;
	DWORD dwRet = ERROR_SUCCESS;
	BOOL bHasMachineAccount = TRUE;

	//
	// Check to see if the xbox has a machine account
	//

	if ( ! bHasMachineAccount && STATE_VERIFYING == m_state || 
		 ! bHasMachineAccount && STATE_EXITED == m_state )
	{
		SetText( "Press A to create a new machine account" );
		m_state = STATE_PROMPTED;
	}
	else if ( STATE_EXITED == m_state )
	{
		SetText( "Machine Account Already Exists" );
		m_state = STATE_EXITED;
	}
	else if ( STATE_VERIFYING == m_state )
	{
		m_state = STATE_EXISTS;
	}

	//
	// check if a is pressed
	//

	if ( pInput->APressed() && STATE_PROMPTED == m_state )
	{
		//
		// update the text
		//

		SetText( "Creating machine account..." );
		m_state = STATE_PRESSED;
	}

	//
	// and on next update actualy submit the machine account creation
	//

	else if ( STATE_PRESSED == m_state )
	{

		//
		// populate the EEPROM
		// (Faking the manufacturing process)
		//

		hr = WriteEEPROMDevkitData();
		hr = SUCCEEDED( hr ) ? S_OK : E_WRITE_EEPROM_DATA;
		MY_ASSERT( hr, S_WRITE_EEPROM_DATA );

		//
		// set text as appropriate
		//

		if ( SUCCEEDED( hr ) )
		{
			SetText( "Machine account Created. Press A to continue." );
			m_state = STATE_DONE;
		}
		else
		{
			SetText( "Machine account creation failed" );
			m_state = STATE_DONE;
		}
	}

	else if ( pInput->BPressed() || ( pInput->APressed() && STATE_EXITED == m_state ) )
	{
		m_state = STATE_EXITED;
		return ParentScreen();
	}

	//
	// if we are done, goto the child screen
	//

	else if ( pInput->APressed() && STATE_DONE == m_state || STATE_EXISTS == m_state )
	{
		m_state = STATE_EXITED;
		return m_pChildScreen;
	}

	return NULL;
}
