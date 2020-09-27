/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashScreen.h

Abstract:
    This object can be used to set information about a screen that
    lives within the XDash.  The information includes the Name of the
    screen, and the controls that live on that screen

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/

#ifndef _XDASHSCREEN_H_
#define _XDASHSCREEN_H_

#define NUM_CONTROLS    50

#include <xtl.h>
#include "XDashControl.h"

extern "C"
{
        ULONG DebugPrint(PCHAR Format, ...);
}

class CXDashScreen
{

public:

    // The controls that exist on the screen
    CXDashControl* m_Controls;

    // Methods
    CXDashScreen();                     // Default Constructor
    CXDashScreen( char* screenName );   // Constructor that will allow you to set the screen name
    ~CXDashScreen();                    // Destructor

    size_t GetNumControls() { return m_numControls; };                  // Returns the number of controls the screen contains
    char* GetScreenName() { return m_Name; };                           // Returns a pointer to the name of the screen
    char* GetControlNameByIndex( size_t index );                        // Returns the name of the control with a given index
    char* GetControlDestByIndex( size_t index );                        // Returns the destination of the control with a given index

    void SetScreenName( char* screenName );                             // Sets the name of the current screen
    void AddControl( char* controlName, char* controlDestination );     // Adds a control to the current screen
    
private:
    // Properties
    size_t m_numControls;       // The number of controls that exist on the screen
    char* m_Name;               // The name of the Screen

    // Methods
    void CXDashScreen::PropertyInit();  // Used to initialze member variables (called by our constructors)
};

#endif // _XDASHSCREEN_H_