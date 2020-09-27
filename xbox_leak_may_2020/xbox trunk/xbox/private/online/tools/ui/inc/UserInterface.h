///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterface.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the user interface class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>

#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"
#include "UserInterfaceVideo.h"

//
// the user interface class decleration
//

class CUserInterface : public CUserInterfaceInput,
					   public CUserInterfaceAudio,
					   public CUserInterfaceVideo {

public:

	//
	// the singleton pattern entry to the class
	//

	static CUserInterface* Instance( void );

	//
	// External state transition calls
	//

	HRESULT Initialize( CUserInterfaceScreen* pScreen,
						ULONG uWidth, 
						ULONG uHeight,
						DWORD dwClearColor, 
						LPSTR szBackground );

	HRESULT Update( void );

private:

	//
	// Constructor and Destructor. They are protected so no one
	// ever tries to allocate / deallocate the class without
	// going through instance() method
	//

	CUserInterface();
	~CUserInterface();

	//
	// the actual instance pointer of the UI
	//

	static CUserInterface* m_pInterface;
};