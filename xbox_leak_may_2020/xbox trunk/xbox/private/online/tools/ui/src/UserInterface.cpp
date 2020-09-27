///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterface.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the user interface class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterface.h"

//
// Need to init the static members
//

CUserInterface* CUserInterface::m_pInterface = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// Name: Instance
//
// Input: None
//
// Output: a pointer to the instance of the UserInterface
//
// Description: this function is the entry point to the user interface. 
//              it will create the ui pointer if it does not already exist
//              otherwise it will return the existing pointer
//
///////////////////////////////////////////////////////////////////////////////

CUserInterface*
CUserInterface::Instance( void )
{
	//
	// check to see if the ui has been created
	//

	if ( ! m_pInterface )
	{
		//
		// allocate a new UI
		//

		m_pInterface = new CUserInterface();
		ASSERT( m_pInterface || ! "could not create ui" );
	}

	//
	// return the ui pointer
	//

	return m_pInterface;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: CUserInterface
//
// Input: None
//
// Output: an instance of the user interface class
//
// Description: constructs the UI
//
///////////////////////////////////////////////////////////////////////////////

CUserInterface::CUserInterface( void )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ~CUserInterface
//
// Input: None
//
// Output: None
//
// Description: destroys the UI
//
///////////////////////////////////////////////////////////////////////////////

CUserInterface::~CUserInterface( void )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Initialize
//
// Input: pScreen - the root screen for the ui
//        uWidth - the width of the screen
//        uHeight - the height of the screen
//        dwClearColor - the color to clear the screen with
//        szBackground - the name of the background image
//                       can be NULL
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: sets up the User interface
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterface::Initialize( CUserInterfaceScreen* pScreen,
							ULONG uWidth, 
							ULONG uHeight,
							DWORD dwClearColor,
							LPSTR szBackground )
{
	HRESULT hr = S_OK;

	//
	// Initialize Input
	//

	if( SUCCEEDED( hr ) )
	{
		hr = CUserInterfaceInput::Initialize();
		ASSERT( SUCCEEDED( hr ) || !"Could not Init input" );
	}

	//
	// Initialize Audio
	//

	if( SUCCEEDED( hr ) )
	{
		hr = CUserInterfaceAudio::Initialize();
		DbgPrint( "0x%x\n", hr );

		if ( E_FAIL == hr )
		{
			hr = S_OK;
		}
	//	ASSERT( SUCCEEDED( hr ) || !"Could not Init audio" );
	}

	//
	// Initialize Video
	//

	if( SUCCEEDED( hr ) )
	{
		hr = CUserInterfaceVideo::Initialize( pScreen,
											  uWidth, 
											  uHeight,
											  dwClearColor,
											  szBackground );

		ASSERT( SUCCEEDED( hr ) || !"Could not Init video" );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: None
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: updates the User interface for the next frame
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterface::Update( void )
{
	HRESULT hr = S_OK;

	//
	// Process the input
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = CUserInterfaceInput::Update();
		ASSERT( SUCCEEDED( hr ) || ! "Could not process input" );
	}

	//
	// render the scene
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = CUserInterfaceVideo::Update( this, this );
		ASSERT( SUCCEEDED( hr ) || ! "Could not update video" );
	}

	//
	// sleep since we are rendering so quick
	//

	Sleep( 130 );

	//
	// spit back the hresult
	//

	return hr;
}
