/*****************************************************
*** rebootcommand.cpp
***
*** CPP file for our XDash CFG Reboot XBox option
*** This option will reboot the XBox when the user
*** selects "A"
*** 
*** by James N. Helm
*** November 3rd, 2000
*** 
*****************************************************/

#include "rebootcommand.h"
#include "videoconstants.h"

extern CXDCItem* g_pCurrentScreen;  // Pointer to the current menu object

// Constructor
CRebootCommand::CRebootCommand()
: CXDCItem()
{
}

// Constructor
CRebootCommand::CRebootCommand( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription )
: CXDCItem( pParent, pwszTitle, pwszDescription )
{
}

// Destructor
CRebootCommand::~CRebootCommand()
{
}


// Draws a menu on to the screen
void CRebootCommand::Action( CXBoxVideo* pScreen )
{
    // Reboot the XBox
    XLaunchNewImage( NULL, NULL );
}

// Handles input for the current menu
void CRebootCommand::HandleInput( enum BUTTONS buttonPressed, bool bFirstPress )
{
    // No input needs to be handled, as the XBox will reboot.
}
