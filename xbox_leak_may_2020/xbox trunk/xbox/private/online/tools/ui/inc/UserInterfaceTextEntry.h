///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceTextEntry.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the interface for the text entry class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>

#include "UserInterfaceText.h"
#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"

//
// the number of chars in select string
//

static const DWORD MAX_SELECT_CHARS = 9;

//
// the range of chars
//

static const CHAR START_CHAR = 'A';
static const CHAR END_CHAR = 'Z';

//
// the formatting string
//

static const LPSTR SELECT_FORMAT_STRING = "%s <<%c>>";

//
// the actual text class
//

class CUserInterfaceTextEntry : public CUserInterfaceText {

public:

	//
	// constructor and destructor
	//

	CUserInterfaceTextEntry(  DWORD dwMaxChars,
							  DWORD dwX, 
							  DWORD dwY, 
							  DWORD dwFGColor, 
							  DWORD dwBGColor );

	~CUserInterfaceTextEntry( void );

	//
	// Update the UI
	//

	virtual HRESULT Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio );

	//
	// get the inputed text
	//

	LPSTR GetInputText( void ) { return m_szInput; };

private:

	//
	// update the select text
	//

	void UpdateSelectText( void );

	//
	// init the text entry
	//

	HRESULT Init( void );

	//
	// the max number of chars
	//

	DWORD m_dwMaxChars;

	//
	// the inputted text
	//

	LPSTR m_szInput;

	//
	// the rotating letter string
	//

	LPSTR m_szSelect;

	//
	// the current charecter
	//

	CHAR m_cCurChar;

	//
	// current index
	//

	DWORD m_dwCurIndex;

	//
	// flag for previous init
	//

	BOOL m_bInit;
};
