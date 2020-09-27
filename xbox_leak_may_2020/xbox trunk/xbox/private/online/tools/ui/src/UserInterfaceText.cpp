///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceText.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the user interface text class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceText.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: constructor
//
// Input: szText - the string for the text object
//        dwX - the X location for the text
//        dwY - the Y location for the text
//        dwFGColor - the foreground color for the text
//        dwBGColor - the background color for the text
//
// Output: None
//
// Description: creates a ui text object
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceText::CUserInterfaceText( LPSTR szText, 
										DWORD dwX, 
										DWORD dwY, 
										DWORD dwFGColor, 
										DWORD dwBGColor )

	: m_szText( szText ),
	  m_dwX( dwX ),
	  m_dwY( dwY ),
	  m_dwFGColor( dwFGColor ),
	  m_dwBGColor( dwBGColor ),
	  m_bHide( FALSE )
{
	//
	// No code here
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
// Description: destroys a UI Text object
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceText::~CUserInterfaceText( void )
{
	//
	// No code here
	//
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
CUserInterfaceText::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	return S_OK;
}