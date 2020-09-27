///////////////////////////////////////////////////////////////////////////////
//
// Name: NameEntryScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for Name Entry screen
//
///////////////////////////////////////////////////////////////////////////////

#include "NameEntryScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  Name Entry screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the Name Entry screen
//
///////////////////////////////////////////////////////////////////////////////

CNameEntryScreen::CNameEntryScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the Name Entry screen
	//

	: CUserInterfaceScreen( pParent, 
	                        NAME_ENTRY_SCREEN_FG_COLOR, 
							NAME_ENTRY_SCREEN_BG_COLOR, 
							NAME_ENTRY_SCREEN_X, 
							NAME_ENTRY_SCREEN_Y ),

	//
	// the text
	//

	  m_aText( NAME_ENTRY_A_TEXT, NAME_ENTRY_A_X, NAME_ENTRY_A_Y, NAME_ENTRY_A_FG_COLOR, NAME_ENTRY_A_BG_COLOR ),
      m_bText( NAME_ENTRY_B_TEXT, NAME_ENTRY_B_X, NAME_ENTRY_B_Y, NAME_ENTRY_B_FG_COLOR, NAME_ENTRY_B_BG_COLOR ),
      m_nameText( "", NAME_X, NAME_Y, NAME_FG_COLOR, NAME_BG_COLOR ),

    //
	// start the row and index on ok
	//

	  m_dwRowIndex( 0 ),
	  m_dwColIndex( NUM_COLS - 1 ),

	//
	// start the string index at the first char
	//

	  m_dwStrIndex( 0 ),

	//
	// start off in an update necessary state
	//

	  m_bUpdate( TRUE ),

	//
	// init to false
	//

	  m_bInit( FALSE )

{
	ZeroMemory( m_szName, sizeof( m_szName ) );

	//
	// needs to have some kind of text
	//

	SetText( NAME_ENTRY_TITLE );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
	AddUIText( &m_nameText );

	//
	// create the keys
	//

	for ( DWORD dwRow = 0; dwRow < NUM_ROWS; dwRow++ )
	{
		for ( DWORD dwCol = 0; dwCol < NUM_COLS; dwCol++ )
		{
			//
			// calculate the x and y location on the fly for the keys
			//

			m_pKeys[dwRow][dwCol] = new CUserInterfaceText( KEYBOARD_STRINGS[dwRow][dwCol],
				                                            KEY_X + dwCol * HORI_DIST,
															KEY_Y + dwRow * VERT_DIST,
															KEY_FG_COLOR,
															KEY_BG_COLOR );

			//
			// this should never fail
			//

			ASSERT( m_pKeys[dwRow][dwCol] );

			//
			// add the key to the screen
			//

			AddUIText( m_pKeys[dwRow][dwCol] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  Name Entry screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the Name Entry screen
//
///////////////////////////////////////////////////////////////////////////////

CNameEntryScreen::~CNameEntryScreen( void )
{
	for ( DWORD dwRow = 0; dwRow < NUM_ROWS; dwRow++ )
	{
		for ( DWORD dwCol = 0; dwCol < NUM_COLS; dwCol++ )
		{
			delete m_pKeys[dwRow][dwCol];
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
CNameEntryScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	CUserInterfaceScreen* pRet = NULL;
	LPSTR szText = m_pKeys[m_dwRowIndex][m_dwColIndex]->Text();

	//
	// leave screen on b
	//

	if ( pInput->BPressed() )
	{
		pRet = ParentScreen()->Update( pInput, pAudio );
	}

	//
	// if a is pressed, handle it and decide what action to do
	//

	if ( pInput->APressed() )
	{
		//
		// on cancel, go to parent screen
		//

		if ( 0 == strcmp( KEY_CANCEL, szText ) )
		{
			pRet = ParentScreen()->Update( NULL, pAudio );
		}

		//
		// on ok ,goto parent screen
		//

		else if ( 0 == strcmp( KEY_OK, szText ) )
		{
			pRet = ParentScreen()->Update( pInput, pAudio );
		}

		//
		// on back, erase a letter
		//

		else if ( 0 == strcmp( KEY_BACK, szText ) && 0 != m_dwStrIndex )
		{
			m_szName[--m_dwStrIndex] = '\0';
			pAudio->PlayBackButton();
		}

		//
		// on a letter, add it
		//

		else if ( 0 != strcmp( KEY_BACK, szText ) && m_dwStrIndex < XONLINE_MAX_NAME_LENGTH )
		{
			m_szName[m_dwStrIndex++] = *szText;

			pAudio->PlaySelectButton();
		}

		m_nameText.SetText( m_szName );
	}

	//
	// move around the grid
	//

	else if ( pInput->DownPressed() )
	{
		m_dwRowIndex = ++m_dwRowIndex % NUM_ROWS;
		m_bUpdate = TRUE;
	}

	else if ( pInput->UpPressed() )
	{
		m_dwRowIndex = --m_dwRowIndex % NUM_ROWS;
		m_bUpdate = TRUE;
	}

	else if ( pInput->LeftPressed() )
	{
	//	m_dwColIndex = --m_dwColIndex % NUM_COLS;

		if ( 0xFFFFFFFF == --m_dwColIndex )
			m_dwColIndex = NUM_COLS - 1;

		m_bUpdate = TRUE;
	}

	else if ( pInput->RightPressed() )
	{
		m_dwColIndex = ++m_dwColIndex % NUM_COLS;
		m_bUpdate = TRUE;
	}

	//
	// reverse the color on the highlited letter, make the rest normal
	//

	if ( TRUE == m_bUpdate )
	{
		for ( DWORD dwRow = 0; dwRow < NUM_ROWS; dwRow++ )
		{
			for ( DWORD dwCol = 0; dwCol < NUM_COLS; dwCol++ )
			{
				if ( m_dwRowIndex == dwRow && m_dwColIndex == dwCol )
				{
					m_pKeys[dwRow][dwCol]->SetFGColor( BGColor() );
					m_pKeys[dwRow][dwCol]->SetBGColor( FGColor() );
				}
				else
				{
					m_pKeys[dwRow][dwCol]->SetFGColor( FGColor() );
					m_pKeys[dwRow][dwCol]->SetBGColor( BGColor() );			
				}
			}
		}

		m_bUpdate = FALSE;

		//
		// only play the sound if we moved
		//

		if ( TRUE == m_bInit )
		{
			pAudio->PlayKeyboardStroke();
		}
		else
		{	
			m_bInit = TRUE;
		}
	}

	return pRet;
}