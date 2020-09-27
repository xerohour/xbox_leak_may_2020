///////////////////////////////////////////////////////////////////////////////
//
// Name: PinEntry.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the pin entry class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>

#include "UserInterfaceText.h"

//
// the pin entry class
//

class CPinEntryText : public CUserInterfaceText {

public:

	//
	// constructor and destructor
	//

	CPinEntryText( DWORD dwX, 
				   DWORD dwY, 
				   DWORD dwFGColor, 
				   DWORD dwBGColor );

	~CPinEntryText( void );

	//
	// Update the UI Text
	//

	virtual HRESULT Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio );

	//
	// get the PIN
	//

	LPBYTE GetPin( void ) { return m_aPINByte; };

private:

	//
	// init function
	//

	HRESULT Init( void );

	//
	// update the text
	//

	HRESULT UpdateText( void );

	//
	// the hnadle use for pin input
	//

	XPININPUTHANDLE m_handle;

	//
	// the input state
	//

    XINPUT_STATE m_inputState;

	//
	// the actual pin
	//

    BYTE m_aPINByte[4];

	//
	// the string
	//

	CHAR m_szStr[64];

	//
	// the current char index
	//

	DWORD m_dwCurIndex;

	//
	// init flag
	//

	BOOL m_bInit;

};