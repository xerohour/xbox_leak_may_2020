/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashControl.h

Abstract:
    This object can be used to set information about a control that
    lives on a screen within the XDash

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/
#ifndef _XDASHCONTROL_H_
#define _XDASHCONTROL_H_

#include <xtl.h>

extern "C"
{
        ULONG DebugPrint(PCHAR Format, ...);
}

class CXDashControl
{

public:
    
    // Properties
    char* m_Name;          // The name of the Control
    char* m_Destination;   // The destination of the Control

    // Methods
    CXDashControl();
    CXDashControl( char* controlName, char* controlDestination );
    ~CXDashControl();

    void UpdateControl( char* controlName, char* controlDestination );    // Allows the user to set the name and destination of the current control

    char* GetControlName() { return m_Name; };                             // Returns the name of the control
    char* GetControlDest() { return m_Destination; };                      // Returns the destination of the control
private:

};

#endif // _XDASHCONTROL_H_