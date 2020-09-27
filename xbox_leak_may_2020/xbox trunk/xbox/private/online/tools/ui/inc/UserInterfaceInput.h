///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceInput.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the Input part of the UI
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>

//
// the number of joysticks supported by the input class
//

static const DWORD NUM_JOYSTICKS = 4;

//
// the input user interface class decleration
//

class CUserInterfaceInput {

public:

	//
	// Constructor and destructor for UI input
	//

	CUserInterfaceInput( void );
	~CUserInterfaceInput( void );

	//
	// methods to init and update input
	//

	HRESULT Initialize( void );
	HRESULT Update( void );

	//
	// button state accessors
	//

	BOOL UpPressed          ( void ) { return m_bUpPressed;          };
	BOOL DownPressed        ( void ) { return m_bDownPressed;        };
	BOOL LeftPressed        ( void ) { return m_bLeftPressed;        };
	BOOL RightPressed       ( void ) { return m_bRightPressed;       };
	BOOL APressed           ( void ) { return m_bAPressed;           };
	BOOL BPressed           ( void ) { return m_bBPressed;           };
	BOOL XPressed           ( void ) { return m_bXPressed;           };
	BOOL YPressed           ( void ) { return m_bYPressed;           };
	BOOL BlackPressed       ( void ) { return m_bBlackPressed;       };
	BOOL WhitePressed       ( void ) { return m_bWhitePressed;       };
	BOOL BackPressed        ( void ) { return m_bBackPressed;        };
	BOOL StartPressed       ( void ) { return m_bStartPressed;       };
	BOOL LeftTriggerPressed ( void ) { return m_bLeftTriggerPressed; };
	BOOL RightTriggerPressed( void ) { return m_bRightTriggerPressed;};
	BOOL LeftThumbPressed   ( void ) { return m_bLeftThumbPressed;   };
	BOOL RightThumbPressed  ( void ) { return m_bRightThumbPressed;  };

private:

	//
	// button state modifiers
	// 

	void UpdateButtonState( PXINPUT_STATE pInputState );
	void ResetButtonState( void );

	//
	// the state of the buttons
	//

	struct {

		unsigned m_bUpPressed           : 1;
		unsigned m_bDownPressed         : 1;
		unsigned m_bLeftPressed         : 1;
		unsigned m_bRightPressed        : 1;
		unsigned m_bAPressed            : 1;
		unsigned m_bBPressed            : 1;
		unsigned m_bXPressed            : 1;
		unsigned m_bYPressed            : 1;
		unsigned m_bBlackPressed        : 1;
		unsigned m_bWhitePressed        : 1;
		unsigned m_bBackPressed         : 1;
		unsigned m_bStartPressed        : 1;
		unsigned m_bLeftTriggerPressed  : 1;
		unsigned m_bRightTriggerPressed : 1;
		unsigned m_bLeftThumbPressed    : 1;
		unsigned m_bRightThumbPressed   : 1;
	};

	//
	// the handles to the joysticks
	//

	HANDLE m_ahInput[NUM_JOYSTICKS];

	//
	// the old input state
	//

	XINPUT_STATE m_oldInputState;

};