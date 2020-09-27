///////////////////////////////////////////////////////////////////////////////
//
// Name: MachineCreationScreen.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the machine account creation screen
//
// Note: DEPRECATED
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <xtl.h>
#include "myError.h"
#include <xonlinep.h>

#include "UserInterfaceScreen.h"
#include "UserInterfaceText.h"

enum SCREEN_STATE {
	STATE_VERIFYING,
	STATE_PROMPTED,
	STATE_PRESSED,
	STATE_EXISTS,
	STATE_DONE,
	STATE_EXITED
};


//
// the machine creation class
//

class CMachineCreationScreen : public CUserInterfaceScreen {

public:

	//
	// constructor and destructor
	//

	CMachineCreationScreen( CUserInterfaceScreen* pParent, CUserInterfaceScreen* pChild );
	~CMachineCreationScreen( void );

	//
	// the update input method
	//

	virtual CUserInterfaceScreen* Update( CUserInterfaceInput* pInput, 
		                                  CUserInterfaceAudio* pAudio );

private:

	//
	// the child screen
	//

	CUserInterfaceScreen* m_pChildScreen;

	//
	// ui text
	//

	CUserInterfaceText m_aText;
	CUserInterfaceText m_bText;

	//
	// state variables
	//

	SCREEN_STATE m_state;
};