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
// g_rgDCVars - This is a list of variables that your application exposes.
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

#define CCH_MAXCMDLEN       256
typedef void (*RCMDSHANDLER)(int argc, char *argv[]);

// Command definition structure
typedef struct _DCCMDDEF
{
    LPSTR           szCmd;                      // Name of command
    RCMDSHANDLER    pfnHandler;                 // Handler function
    LPSTR           szCmdHelp;                  // Description of command
} DCCMDDEF;

extern const DCCMDDEF *g_rgDCCmds;				// List of app-defined commands
extern const UINT      g_nDCCmds;				// Number of app-defined commands

/////////////////////////////////////////////////////////////////////////////
// Useful commands for the application to put into the g_rgDCCmds array
/////////////////////////////////////////////////////////////////////////////
void RCmdHelp(int argc, char *argv[]);          // Help command
void RCmdSet(int argc, char *argv[]);           // Set command

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

enum DCDATATYPES { DC_bool,
				   DC_I2,
				   DC_I4,
				   DC_R4,
				   DC_COLOR,
				   DC_FLOAT1,
				   DC_FLOAT2,
				   DC_FLOAT3,
				   DC_FLOAT4,
				   DC_MATRIX,
};
typedef void (*DCSETNOTIF)(LPVOID lpvAddr);

// Variable definition structure
typedef struct _DCVARDEF
{
    LPCSTR          lpstr;                      // Name of variable
    LPVOID          lpvAddr;                    // Address of variable
    DCDATATYPES  ddt;                        // Data type of variable
    DCSETNOTIF   pfnNotifFunc;               // Function to call upon change
} DCVARDEF;

// These should get defined in your application cpp file:
extern const DCVARDEF *g_rgDCVars;       // List of application variables
extern const UINT      g_nDCVars;        // Number of application vars


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
BOOL DCPrintf(LPCSTR szFmt, ...);

#endif // _DEBUGCMD_H
