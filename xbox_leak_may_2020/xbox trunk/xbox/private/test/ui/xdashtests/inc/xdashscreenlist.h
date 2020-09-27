/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashScreenList.h

Abstract:
    This object can be used to hold a list of screens.  a screen consists
    of a name, and a list of controls and their destinations.

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/

#ifndef _XDASHSCREENLIST_H_
#define _XDASHSCREENLIST_H_

#include <xtl.h>
#include <stdio.h>
#include <profilep.h>
#include "xdashscreen.h"

extern "C"
{
    ULONG DebugPrint(PCHAR Format, ...);
}

// Maximum number of screen our object can hold
#define MAX_SCREENS         100

// INI Delimiters
#define NAME_TOKEN            L"Name="
#define NAME_TOKEN_A           "Name="
#define CONTROL_TOKEN         L"Control="
#define CONTROL_TOKEN_A        "Control="
#define CONTROL_DEST_TOKEN    L"ControlDest="
#define CONTROL_DEST_TOKEN_A   "ControlDest="
#define COMMENT_CHAR          L';'
#define COMMENT_CHAR_A         ';'

// Amount of characters in a line from the INI file
#define MAX_LINE_SIZE       255

class CXDashScreenList
{

public:
    // Properties

    // Methods
    CXDashScreenList();                     // Default Constructor
    CXDashScreenList( char* iniFileName );  // Constructor which accepts an INI filename
    ~CXDashScreenList();                    // Destructor

    void GetScreensFromINI( char* iniFileName );                                        // Populates the screen list from an INI file

    void AddScreenToList( char* screenName );                                           // Add a screen to our list
    void AddControlToScreen( char* controlName, char* controlDest );                    // Add a control to our current screen

    size_t GetNumScreens() { return m_numScreens; };                                    // Returns the number of screens currently in our list
    char* GetCurrentScreenName();                                                       // Returns the name of the current screen
    char* GetScreenNameByIndex( size_t index );                                         // Returns the name of a screen at a current index
    char* GetControlNameFromScreenByIndex( size_t screenIndex, size_t controlIndex );   // Returns the name of a control at a given screen and control index
    char* GetControlDestFromScreenByIndex( size_t screenIndex, size_t controlIndex );   // Returns the destination of a control at a given screen and control index
    char* GetControlFromCurrentScreenByIndex( size_t index );                           // Returns a control from the current screen specified by the index
    size_t GetNumControlsOnCurrentScreen();                                             // Returns the number of controls on the current screen
    size_t GetNumControlsOnScreenByIndex( size_t index );                               // Returns the number of controls on the screen specified by the index

private:
    // Properties
    CXDashScreen* m_Screens;    // The Screens and their controls
    size_t m_numScreens;        // The number of screens in our list

    char* GetValueFromINI( char* iniFileName, char* iniSection, char* iniKey, size_t bufferSize );   // Returns a value from an INI file, NULL if there was a problem
    void PropertyInit();                                                                             // Used to initialize our member variables.  (Called by constructors)
    size_t NullCharInString( char* targetString, char charToNull );                                  // Used to NULL specific characters from a string

};

#endif // _XDASHSCREENLIST_H_