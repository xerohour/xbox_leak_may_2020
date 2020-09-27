///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceText.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the interface for the text class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <stdio.h>

#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"

//
// the actual text class
//

class CUserInterfaceText {

public:

	//
	// constructor and destructor
	//

	CUserInterfaceText( LPSTR szText, 
		                DWORD dwX, 
						DWORD dwY, 
						DWORD dwFGColor, 
						DWORD dwBGColor );

	~CUserInterfaceText( void );

	//
	// Update the UI Text
	//

	virtual HRESULT Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio );

	//
	// accessors for each member
	//

	LPSTR Text( void ) { return m_szText; };
	DWORD X( void ) { return m_dwX; };
	DWORD Y( void ) { return m_dwY; };
	DWORD FGColor( void ) { return m_dwFGColor; };
	DWORD BGColor( void ) { return m_dwBGColor; };
	void SetFGColor( DWORD dwColor ) { m_dwFGColor = dwColor; };
	void SetBGColor( DWORD dwColor ) { m_dwBGColor = dwColor; };
	void Hide( BOOL bHide ) { m_bHide = bHide; };
	BOOL Hidden( void ) { return m_bHide; };

	//
	// update text
	//

	void SetText( LPSTR szText ) { m_szText = szText; };

private:

	//
	// the string
	//

	LPSTR m_szText;

	//
	// x and y pos
	//

	DWORD m_dwX;
	DWORD m_dwY;

	//
	// the fore- and back-ground colors
	//

	DWORD m_dwFGColor;
	DWORD m_dwBGColor;

	//
	// hidden or not
	//

	BOOL m_bHide;

};
	