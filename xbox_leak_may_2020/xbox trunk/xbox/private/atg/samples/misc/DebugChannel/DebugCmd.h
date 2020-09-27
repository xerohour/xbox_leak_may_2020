//-----------------------------------------------------------------------------
// File: DebugCmd.H
//
// Desc: Remote Xbox command header
//
// This header file should be included into your application.  It provides 
// function and type definitions.  You will also need to modify the following
// variables:
//
// g_rgDCCmds - This is the list of commands your application provides.  Note
//  that "help" and "set" are provided automatically and can not be overriden.
//  This is currently defined in DebugCmd.cpp
// g_rgDCCMDVars - This is a list of variables that your application exposes.
//  They can be examined and modified by the builtin "set" command.
//  This should be defined in your application source, after including DebugCmd.h
//
// Hist: 02.05.01 - Initial creation for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _DEBUGCMD_H
#define _DEBUGCMD_H

/////////////////////////////////////////////////////////////////////////////
// Commands
/////////////////////////////////////////////////////////////////////////////

void RCmdHelp(int argc, char *argv[]);          // Help command
void RCmdSet(int argc, char *argv[]);           // Set command
// Add prototypes for your app-defined commands here
void RCmdTexture(int argc, char *argv[]);       // Sets the texture
void RCmdSpin(int argc, char *argv[]);          // Sets the spin velocity
void RCmdLightChange( void * );                 // Notified on light changes

#define CCH_MAXCMDLEN       256
typedef void (*RCMDSHANDLER)(int argc, char *argv[]);

// Command definition structure
typedef struct _DCCMDDEF
{
    LPSTR           szCmd;                      // Name of command
    RCMDSHANDLER    pfnHandler;                 // Handler function
    LPSTR           szCmdHelp;                  // Description of command
} DCCMDDEF;

extern const DCCMDDEF g_rgDCCmds[];             // List of app-defined commands


/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

enum DCCMDDATATYPES { SDOS_bool, SDOS_I2, SDOS_I4, SDOS_R4 };
typedef void (*DCCMDSETNOTIF)(LPVOID lpvAddr);

// Variable definition structure
typedef struct _DCCMDVARDEF
{
    LPCSTR          lpstr;                      // Name of variable
    LPVOID          lpvAddr;                    // Address of variable
    DCCMDDATATYPES  ddt;                        // Data type of variable
    DCCMDSETNOTIF   pfnNotifFunc;               // Function to call upon change
} DCCMDVARDEF;

// These should get defined in your application cpp file:
extern const DCCMDVARDEF g_rgDCCMDVars[];       // List of application variables
extern const UINT        g_nVars;               // Number of application vars


/////////////////////////////////////////////////////////////////////////////
// Misc
/////////////////////////////////////////////////////////////////////////////

//
// handle any remote commands that have been sent - this
// should be called periodically by the application
//
BOOL DCHandleCmds();

//
// asynchronous printf - this is used to send
// responses back to the debug console
//
BOOL DCCMDPrintf(LPCSTR szFmt, ...);

#endif // _DEBUGCMD_H