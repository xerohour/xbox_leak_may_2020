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
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _DEBUGCMD_H_
#define _DEBUGCMD_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <xbdm.h>

/////////////////////////////////////////////////////////////////////////////
// Commands
/////////////////////////////////////////////////////////////////////////////

void RCmdHelp(int argc, char *argv[]);          // Help command
void RCmdSet(int argc, char *argv[]);           // Set command
// Add prototypes for your app-defined commands here
void RCmdAddConnection(int argc, char *argv[]);
void RCmdRemoveConnection(int argc, char *argv[]);
void RCmdInitializeExperimenter(int argc, char *argv[]);    // Initializes for Xbox Experimenter
void RCmdUninitializeExperimenter(int argc, char *argv[]);    // Initializes for Xbox Experimenter
void RCmdLoad(int argc, char *argv[]);          // Loads the segment
void RCmdUnload(int argc, char *argv[]);        // Unloads the segment
void RCmdPlay(int argc, char *argv[]);          // Plays the segment
void RCmdStop(int argc, char *argv[]);          // Stops the segment
void RCmdSearchDirectory(int argc, char *argv[]);// Sets the search directory
void RCmdClearCache(int argc, char *argv[]);    // Clears the loader's cache
void RCmdPanic(int argc, char *argv[]);			// Stop everything
void RCmdCreateAudiopath(int argc, char *argv[]);// Creates the audiopath
void RCmdSetDefaultAudiopath(int argc, char *argv[]);// Sets the default audiopath
void RCmdReleaseAudiopath(int argc, char *argv[]);// Releases the audiopath

void RCmdScriptInit(int argc, char *argv[]);// Initializes a script
void RCmdScriptRelease(int argc, char *argv[]);// Releases a script
void RCmdScriptEnumVariable(int argc, char *argv[]);// Enumerates a variable
void RCmdScriptGetVariableVariant(int argc, char *argv[]);// Gets a variable
void RCmdScriptSetVariableVariant(int argc, char *argv[]);// Sets a variable
void RCmdScriptEnumRoutine(int argc, char *argv[]);// Enumerates a routine
void RCmdScriptCallRoutine(int argc, char *argv[]);// Calls a routine

HRESULT RCmdSynthPlayBuffer(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Plays a buffer of MIDI events
HRESULT __stdcall SynthReceiveBuffer(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse);

HRESULT RCmdSynthDownload(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Downloads an instrument
HRESULT __stdcall SynthReceiveDownload(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse);

HRESULT RCmdSynthUnload(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Unloads an instrument
HRESULT RCmdSynthGetTime(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Get the current time
HRESULT RCmdSynthSetChannelGroups(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Set the number of channel groups
HRESULT RCmdSynthSetDefaultAudioPath(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Set the default audiopath to perform on
HRESULT RCmdSynthInitialize(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Set the number of channel groups
HRESULT RCmdSynthUninitialize(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);// Set the number of channel groups

void RCmdWaveBankPlayEntry(int argc, char **argv);
void RCmdWaveBankStopAll(int argc, char **argv);

void RCmdChange( void * );                      // Notified on changes


#define CCH_MAXCMDLEN       256
typedef void (*RCMDSHANDLER)(int argc, char *argv[]);

// Command definition structure
typedef struct _DCCMDDEF
{
    LPSTR           szCmd;                      // Name of command
    RCMDSHANDLER    pfnHandler;                 // Handler function
    LPSTR           szCmdHelp;                  // Description of command
} DCCMDDEF;

typedef HRESULT (*RCMDSHANDLERNOW)(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);

// Command definition structure
typedef struct _DCCMDDEFNOW
{
    LPSTR           szCmd;                      // Name of command
    RCMDSHANDLERNOW pfnHandler;                 // Handler function
    LPSTR           szCmdHelp;                  // Description of command
} DCCMDDEFNOW;

extern const DCCMDDEF g_rgDCCmds[];             // List of app-defined commands
extern const DCCMDDEFNOW g_rgDCCmdsNow[];       // List of app-defined commands that should be processed immediately


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

#endif // _DEBUGCMD_H_
