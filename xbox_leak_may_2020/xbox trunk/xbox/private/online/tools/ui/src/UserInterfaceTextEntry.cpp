///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceTextEntry.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the user interface text entry class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceTextEntry.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: constructor
//
// Input: dwMaxChars - the maximum number of chars allowed to be inputted
//        dwX - the X location for the text
//        dwY - the Y location for the text
//        dwFGColor - the foreground color for the text
//        dwBGColor - the background color for the text
//
// Output: None
//
// Description: creates a ui text entry object
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceTextEntry::CUserInterfaceTextEntry( DWORD dwMaxChars,
										          DWORD dwX, 
										          DWORD dwY, 
										          DWORD dwFGColor, 
										          DWORD dwBGColor )

	//
	// create the base text UI object
	//

	: CUserInterfaceText( "", dwX, dwY, dwFGColor, dwBGColor ),
	  m_dwMaxChars( dwMaxChars ),
	  m_szInput( NULL ),
	  m_szSelect( NULL ),
	  m_cCurChar( START_CHAR ),
	  m_dwCurIndex( 0 ),
	  m_bInit( FALSE )
{
	ASSERT( 0 != dwMaxChars );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Destructor
//
// Input: None
//
// Output: None
//
// Description: destroys a UI Text entry object
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceTextEntry::~CUserInterfaceTextEntry( void )
{
	//
	// destory the internal string
	//

	if ( NULL != m_szInput )
	{
		delete [] m_szInput;
		m_szInput = NULL;
	}

	//
	// destroy the select string
	//

	if ( NULL != m_szSelect )
	{
		delete [] m_szSelect;
		m_szSelect = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateSelectText
//
// Input: none
//
// Output: none
//
// Description: updates the text buffer
//
///////////////////////////////////////////////////////////////////////////////

void  
CUserInterfaceTextEntry::UpdateSelectText( void )
{
	//
	// concatenante the strings, set the text
	//

	sprintf( m_szSelect, SELECT_FORMAT_STRING, m_szInput, m_cCurChar );

	SetText( m_szSelect );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Init
//
// Input: none
//
// Output: the final HR
//
// Description: init the text entry class
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceTextEntry::Init( void )
{
	HRESULT hr = S_OK;

	//
	// reset the current index
	//

	m_dwCurIndex = 0;

	//
	// kill off the input string if necessary
	//

	if ( m_szInput )
	{
		delete [] m_szInput;
	}

	//
	// kill of the select string
	//

	if ( m_szSelect )
	{
		delete [] m_szSelect;
	}

	//
	// allocate the input string
	//

	if ( SUCCEEDED( hr ) )
	{
		m_szInput = new CHAR[m_dwMaxChars + 1];
		hr = NULL == m_szInput ? E_OUTOFMEMORY : S_OK;
	}

	//
	// allocate the select string
	//

	if ( SUCCEEDED( hr ) )
	{
		m_szSelect = new CHAR[strlen( Text() ) + m_dwMaxChars + MAX_SELECT_CHARS + 1];
		hr = NULL == m_szInput ? E_OUTOFMEMORY : S_OK;
	}

	//
	// zero out the strings
	//

	if ( SUCCEEDED( hr ) )
	{
		ZeroMemory( m_szInput, m_dwMaxChars + 1 );
		ZeroMemory( m_szSelect, strlen( Text() ) + MAX_SELECT_CHARS + m_dwMaxChars + 1 );
	}

	//
	// don't init again
	//

	if ( SUCCEEDED( hr ) )
	{
		m_bInit = TRUE;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input ui class
//        pAudio - the input audio class
//
// Output: the final HR
//
// Description: process input
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceTextEntry::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;

	//
	// only init once
	//

	if ( ! m_bInit )
	{
		hr = Init();
	}

	//
	// if left is pressed, scroll left
	//

	if ( pInput->LeftPressed() )
	{
		if ( --m_cCurChar < START_CHAR )
			m_cCurChar = END_CHAR;

		hr = pAudio->PlayMenuItem();
	}

	//
	// if right is pressed, scroll right
	//

	if ( pInput->RightPressed() )
	{
		if ( ++m_cCurChar > END_CHAR )
			m_cCurChar = START_CHAR;

		hr = pAudio->PlayMenuItem();
	}

	//
	// if A is pressed
	//

	if ( pInput->XPressed() )
	{
		m_szInput[m_dwCurIndex] = m_cCurChar;

		if ( m_dwMaxChars == ++m_dwCurIndex )
		{
			m_dwCurIndex = m_dwMaxChars - 1;
		}

		hr = pAudio->PlayKeyboardStroke();
	}

	//
	// if B is pressed
	//

	if ( pInput->YPressed() )
	{
		if ( 0xFFFFFFFF == --m_dwCurIndex )
		{
			m_dwCurIndex = 0;
		}

		m_szInput[m_dwCurIndex] = '\0';

		hr = pAudio->PlayKeyboardStroke();
	}

	//
	// update the select string
	//
	
	if ( SUCCEEDED( hr ) )
	{
		UpdateSelectText();
	}

	return hr;
}