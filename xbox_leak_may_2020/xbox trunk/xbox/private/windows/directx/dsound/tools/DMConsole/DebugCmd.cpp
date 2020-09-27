//-----------------------------------------------------------------------------
// File: DebugCmd.CPP
//
// Desc: 
//      Helps an application expose functionality through the debug channel
//  to a debug console running on a remote machine.
//      Commands are sent through the debug channel to the debug monitor on
//  the Xbox machine.  The Xbox machine routes it to the command processor,
//  which is linked into the application (this happens on a separate thread
//  from the application's).  When the application polls for commands to be
//  run (usually once per frame), it can then pick up the command to execute
//  and do the appropriate processing.
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xbdm.h>
#include <stdio.h>

#include "DebugCmd.h"

// our critical section
bool g_fcsInited = false;
CRITICAL_SECTION g_cs;

// command prefix
const char *g_szCmd = "XAUD";

// processorproc buffer
#define REMOTEBUF_SIZE 10
static char g_aszRemoteBuf[REMOTEBUF_SIZE][CCH_MAXCMDLEN];
static int g_nRemoteBufWriteSlot;
static int g_nRemoteBufReadSlot;

const DCCMDDEF g_rgDCCmds[] =                   // List of app-defined commands
{
    {"help",      RCmdHelp,      " [CMD]: List commands / usage"},
    {"set",       RCmdSet,       " var [=] <val>: set a variable"},
    // Add additional application defined commands here:
	{"connect",RCmdAddConnection," : Connects an application"},
	{"disconnect",RCmdRemoveConnection," : Disconnects an application"},
    {"initialize",RCmdInitializeExperimenter," : Initializes Xbox Experimenter functions"},
	{"uninitialize",RCmdUninitializeExperimenter," : Uninitializes Xbox Experimenter functions"},
    {"load",      RCmdLoad,      " <segmentID> <filename>: Loads a segment"},
    {"unload",    RCmdUnload,    " <segmentID> : Unloads a segment"},
    {"play",      RCmdPlay,      " <segmentID> <segmentFlags> <audiopathID> <standardAudiopathID>: Plays a segment"},
    {"stop",      RCmdStop,      " <segmentID> <segmentFlags>: Stops a segment"},
	{"searchDirectory",RCmdSearchDirectory, " <searchDirectory>: Sets the search directory"},
	{"clearCache",RCmdClearCache," : Clears the loader's cache"},
	{"panic",	  RCmdPanic,	 " : Stop everything"},
    {"createAudiopath",RCmdCreateAudiopath," <audiopathID> <filename>: Loads and instantiates an audiopath"},
	{"createStandardAudiopath",RCmdCreateAudiopath," <audiopathID> <dwType>: Instantiates a standard audiopath"},
    {"setDefaultAudiopath",RCmdSetDefaultAudiopath," <audiopathID>: Sets a audiopath as the default audiopath"},
    {"releaseAudiopath",RCmdReleaseAudiopath," <audiopathID> : Releases an audiopath"},
	{"Script.Init",RCmdScriptInit," <scriptID> <filename>: Loads and initializes a script"},
	{"Script.Release",RCmdScriptRelease," <scriptID>: Releases a script"},
	{"Script.EnumVariable",RCmdScriptEnumVariable," <scriptID> <variableIndex>: Enumerates a variable"},
	{"Script.GetVariableVariant",RCmdScriptGetVariableVariant," <scriptID> <variableName>: Gets a variable"},
	{"Script.SetVariableVariant",RCmdScriptSetVariableVariant," <scriptID> <variableName>: Sets a variable"},
	{"Script.EnumRoutine",RCmdScriptEnumRoutine," <scriptID> <routineIndex>: Enumerates a routine"},
	{"Script.CallRoutine",RCmdScriptCallRoutine," <scriptID> <routineName>: Calls a routine"},
	{"WaveBank.PlayEntry",RCmdWaveBankPlayEntry," : <file_name> Plays a wave bank entry"},
	{"WaveBank.StopAll",RCmdWaveBankStopAll," : Stops all playing wave bank entries"},
};

const DCCMDDEFNOW g_rgDCCmdsNow[] = // List of app-defined commands that must be processed immediately
{
	{"Synth.Initialize",RCmdSynthInitialize," : Initializes the synth"},
	{"Synth.Uninitialize",RCmdSynthUninitialize," : Uninitializes the synth"},
    {"Synth.PlayBuffer",RCmdSynthPlayBuffer," <rtBuffer> <rtNow> <cbBuffer> : Plays a buffer of MIDI events"},
    {"Synth.Download",RCmdSynthDownload," <dwBufferID> <cbBuffer> : Downloads an instrument buffer"},
    {"Synth.Unload",RCmdSynthUnload," <dwBufferID> : Unloads an instrument buffer"},
    {"Synth.GetTime",RCmdSynthGetTime," : Gets the current time"},
	{"Synth.SetChannelGroups",RCmdSynthSetChannelGroups," : <dwNumChannelGroups> Sets the number of channel groups to use"},
	{"Synth.SetDefaultAudiopath",RCmdSynthSetDefaultAudioPath," : <dwNumAudioPath> Sets the default audiopath to use"},
};

//=========================================================================
// Name: RCmdHelp
// Desc: Builtin command - help
//          Iterates over the list of builtin and application-provided 
//          commands and displays a help string for each one
//=========================================================================
void RCmdHelp(int argc, char *argv[])
{
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );

    int iCmd;

    for(iCmd = 0; iCmd < sizeof(g_rgDCCmds) / sizeof(g_rgDCCmds[0]); iCmd++)
    {
        DCCMDPrintf("%s\t%s\n",
            g_rgDCCmds[iCmd].szCmd,
            g_rgDCCmds[iCmd].szCmdHelp);
    }
    for(iCmd = 0; iCmd < sizeof(g_rgDCCmdsNow) / sizeof(g_rgDCCmdsNow[0]); iCmd++)
    {
        DCCMDPrintf("%s\t%s\n",
            g_rgDCCmdsNow[iCmd].szCmd,
            g_rgDCCmdsNow[iCmd].szCmdHelp);
    }
}

//=========================================================================
// Name: RCmdSet
// Desc: Builtin command - set
//          Can set or display values of any variables exposed by the 
//          application to the debug console
//=========================================================================
void RCmdSet(int argc, char *argv[])
{
    UINT nIndex;

    if(argc == 1)
    {
        // if we aren't passed any arguments, then just list all the variables and
        // what their current values are.
        for(nIndex = 0; nIndex < g_nVars; nIndex++)
        {
            switch(g_rgDCCMDVars[nIndex].ddt)
            {
            case SDOS_bool:
                DCCMDPrintf("%s\t= %d\n", g_rgDCCMDVars[nIndex].lpstr,
                    *(bool *)g_rgDCCMDVars[nIndex].lpvAddr);
                break;
            case SDOS_I4:
                DCCMDPrintf("%s\t= %d\n", g_rgDCCMDVars[nIndex].lpstr,
                    *(INT *)g_rgDCCMDVars[nIndex].lpvAddr);
                break;
            case SDOS_I2:
                DCCMDPrintf("%s\t= %d\n", g_rgDCCMDVars[nIndex].lpstr,
                    *(WORD *)g_rgDCCMDVars[nIndex].lpvAddr);
                break;
            case SDOS_R4:
                DCCMDPrintf("%s\t= %0.1f\n", g_rgDCCMDVars[nIndex].lpstr,
                    *(float *)g_rgDCCMDVars[nIndex].lpvAddr);
                break;
            }
        }
    }
    else
    {
        // if the user did a set "foo = 2" move arg3 to arg2
        if(argv[2][0] == '=')
            argv[2] = argv[3];

        // Find the entry for this variable, if we can
        for(nIndex = 0; nIndex < g_nVars; nIndex++)
        {
            if(!lstrcmpiA(argv[1], g_rgDCCMDVars[nIndex].lpstr))
            {
                char *endptr;
                DWORD dwVal = (argv[2][0] == '0' && argv[2][1] == 'x') ?
                    strtoul(argv[2], &endptr, 16) : atoi(argv[2]);

                // Set the appropriate type of data
                LPVOID lpvAddr = g_rgDCCMDVars[nIndex].lpvAddr;
                switch(g_rgDCCMDVars[nIndex].ddt)
                {
                case SDOS_bool:
                    *(bool *)lpvAddr = !!dwVal;
                    DCCMDPrintf("set %s = %d\n", argv[1], *(bool *)lpvAddr);
                    break;
                case SDOS_I4:
                    *(INT *)lpvAddr = dwVal;
                    DCCMDPrintf("set %s = %d\n", argv[1], *(INT *)lpvAddr);
                    break;
                case SDOS_I2:
                    *(WORD *)lpvAddr = (WORD)dwVal;
                    DCCMDPrintf("set %s = %d\n", argv[1], *(WORD *)lpvAddr);
                    break;
                case SDOS_R4:
                    *(float *)lpvAddr = (float)atof(argv[2]);
                    DCCMDPrintf("set %s = %0.1f\n", argv[1], *(float *)lpvAddr);
                    break;
                }

                // call the notif func if there was one
                if(g_rgDCCMDVars[nIndex].pfnNotifFunc)
                    g_rgDCCMDVars[nIndex].pfnNotifFunc(lpvAddr);

                return;
            }
        }

        DCCMDPrintf("variable '%s' not found\n", argv[1]);
    }
}

//=========================================================================
// Name: CmdToArgv
// Dsec: Parse szCmd into argv and return # of args. (modifies szCmd)
//=========================================================================
int CmdToArgv(char *szCmd, char *szArg[], int maxargs)
{
    int argc = 0;
    int argcT = 0;
    char *szNil = szCmd + lstrlenA(szCmd);

    while(argcT < maxargs)
    {
        // eat whitespace
        while(*szCmd && (*szCmd == ' '))
            szCmd++;

        if(!*szCmd)
        {
            szArg[argcT++] = (char *)szNil;
        }
        else
        {
            // find the end of this arg
            char chEnd = (*szCmd == '"' || *szCmd == '\'') ? *szCmd++ : ' ';
            char *szArgEnd = szCmd;
            while(*szArgEnd && (*szArgEnd != chEnd))
                szArgEnd++;

            // record this bad boy
            szArg[argcT++] = szCmd;
            argc = argcT;

            // move szArg to the next argument (or not)
            szCmd = *szArgEnd ? szArgEnd + 1 : szArgEnd;
            *szArgEnd = 0;
        }
    }

    return argc;
}


//
// Temporary replacement for CRT string funcs, since
// we can't call CRT functions on the debug monitor
// thread right now.
//

//=========================================================================
// Name: dbgstrlen
// Desc: Returns length of sz
//=========================================================================
int dbgstrlen( LPCSTR sz )
{
    LPCSTR szEnd = sz;

    while( *szEnd )
        szEnd++;

    return szEnd - sz;
}

//=========================================================================
// Name: dbglc
// Desc: Returns lowercase of char
//=========================================================================
inline char dbglc( char ch )
{
    if( ch >= 'A' && ch <= 'Z' )
        return ch - ( 'A' - 'a' );
    else
        return ch;
}

//=========================================================================
// Name: dbgstrcmpn
// Desc: String compare.  Returns TRUE if strings are equal up to n 
//       chars or end of string
//=========================================================================
BOOL dbgstrcmpn( LPCSTR sz1, LPCSTR sz2, int n )
{
    while( ( dbglc( *sz1 ) == dbglc( *sz2 ) ) && *sz1 && n > 0 )
    {
        --n;
        ++sz1;
        ++sz2;
    }

    return( n == 0 || dbglc( *sz1 ) == dbglc( *sz2 ) );
}

//=========================================================================
// Name: dbgstrcpy
// Desc: Copies szSrc to szDest and terminates szDest
//=========================================================================
void dbgstrcpy( LPSTR szDest, LPCSTR szSrc )
{
    while( ( *szDest++ = *szSrc++ ) != 0 )
        ;
}
    
//=========================================================================
// Name: DCCmdPRocessorProc
// Desc: Cmd notification proc
//
//  This is what is called by the Xbox debug monitor to have us process
//  a command.  What we'll actually attempt to do is tell it to make calls
//  to us on a separate thread, so that we can just block until we're 
//  able to process a command.
//=========================================================================
extern "C" ULONG _cdecl DbgPrint(PCH Format, ...);
HRESULT __stdcall DCCmdProcessorProc(LPCSTR szCommand, LPSTR szResp,
    DWORD cchResp, PDM_CMDCONT pdmcc)
{
    // skip over prefix
    LPCSTR szCmd = szCommand + lstrlenA(g_szCmd) + 1;
    BOOL   fKnownCommand = FALSE;

    // check if this is the initial connect signal
    if(dbgstrcmpn(szCmd, "__connect__", 11))
    {
        // if so, respond that we're connected
        lstrcpynA(szResp, "Version 0.1 Connected.", cchResp);
        return XBDM_NOERR;
    }

    // check to see if the cmd exists
    for(int iCmd = 0; iCmd < sizeof(g_rgDCCmds) / sizeof(g_rgDCCmds[0]); iCmd++)
    {
        if(dbgstrcmpn(g_rgDCCmds[iCmd].szCmd, szCmd, dbgstrlen(g_rgDCCmds[iCmd].szCmd)))
        {
            // If we find the string, copy it into the command buffer
            // to be examined by the polling function
            fKnownCommand = TRUE;
            break;
        }
    }

	int iCmdNow = -1;
	if( !fKnownCommand )
	{
		// check to see if the cmd exists
		for(iCmdNow = 0; iCmdNow < sizeof(g_rgDCCmdsNow) / sizeof(g_rgDCCmdsNow[0]); iCmdNow++)
		{
			if(dbgstrcmpn(g_rgDCCmdsNow[iCmdNow].szCmd, szCmd, dbgstrlen(g_rgDCCmdsNow[iCmdNow].szCmd)))
			{
				// If we find the string, call its function immediately
				fKnownCommand = TRUE;
				break;
			}
		}
	}

	HRESULT hr = XBDM_NOERR;

    if( fKnownCommand )
    {
		if( iCmdNow != -1 )
		{
			char *      szArgv[10];
			int         argc;
			char        szLocalBuf[CCH_MAXCMDLEN]; // local copy of command
			dbgstrcpy( szLocalBuf, szCmd );

			// Now process the command
			argc = CmdToArgv(szLocalBuf, szArgv, 10);
			hr = g_rgDCCmdsNow[iCmdNow].pfnHandler(argc, szArgv, szResp, cchResp, pdmcc);
		}
		else
		{
			// g_aszRemoteBuf needs to be protected by the critical section
			EnterCriticalSection(&g_cs);

			// If the read and write heads are at the same position, and there is data ther
			if( (g_nRemoteBufWriteSlot == g_nRemoteBufReadSlot)
			&&	g_aszRemoteBuf[g_nRemoteBufWriteSlot][0] )
			{
				// This means the application has probably stopped polling for debug commands
				dbgstrcpy( szResp, "Cannot execute - previous commands still pending" );
			}
			else
			{
				// Otherwise, write out the command
				dbgstrcpy( g_aszRemoteBuf[g_nRemoteBufWriteSlot], szCmd );
				g_nRemoteBufWriteSlot++;
				if( g_nRemoteBufWriteSlot >= REMOTEBUF_SIZE )
				{
					g_nRemoteBufWriteSlot = 0;
				}
			}
			LeaveCriticalSection(&g_cs);
		}
    }
    else
    {
        dbgstrcpy( szResp, "unknown command" );
    }

    return hr;
}
extern void PlayMIDI();

//=========================================================================
// Name: DCHandleCmds
// Desc: Poll routine called by Xbox app
//
//  The application should call this function periodically, usually every
//  frame.  This will check to see if there is a command waiting to be
//  executed, and if so, execute it.
//=========================================================================
BOOL DCHandleCmds()
{
    static BOOL fRegistered = false;
    char *      szArgv[10];
    int         argc;
    char        szLocalBuf[CCH_MAXCMDLEN]; // local copy of command

    // Initialize ourselves when we're first called.
    if(!fRegistered)
    {
        // init our critical section
        if(!g_fcsInited)
        {
            InitializeCriticalSection(&g_cs);
            g_fcsInited = true;
        }

        // register our command handler with the debug monitor
        HRESULT hr = DmRegisterCommandProcessor(g_szCmd, DCCmdProcessorProc);
        if(FAILED(hr))
            return false;

        fRegistered = true;
    }

    // If there's nothing waiting, return.
    if( !g_aszRemoteBuf[g_nRemoteBufReadSlot][0] )
    {
        return false;
    }

    // 
    // Grab a copy of the command to free up
    // the crtical section
    //
    EnterCriticalSection(&g_cs);

    lstrcpyA( szLocalBuf, g_aszRemoteBuf[g_nRemoteBufReadSlot] );
    g_aszRemoteBuf[g_nRemoteBufReadSlot][0] = 0;

	g_nRemoteBufReadSlot++;
	if( g_nRemoteBufReadSlot >= REMOTEBUF_SIZE )
	{
		g_nRemoteBufReadSlot = 0;
	}

    LeaveCriticalSection(&g_cs);

	{
        char szBuf[100+CCH_MAXCMDLEN];
        sprintf( szBuf, "Read command %s\n", szLocalBuf );
        OutputDebugString( szBuf );
	}


    // Now process the command
    argc = CmdToArgv(szLocalBuf, szArgv, 10);

    // Find the entry in our command list
    for(int iCmd = 0; iCmd < sizeof(g_rgDCCmds) / sizeof(g_rgDCCmds[0]); iCmd++)
    {
        if(!lstrcmpiA(g_rgDCCmds[iCmd].szCmd, szArgv[0]))
        {
            g_rgDCCmds[iCmd].pfnHandler(argc, szArgv);
            break;
        }
    }

    return true;
}

//=========================================================================
// Name: DCCMDPrintf 
// Desc: Asynchronous printf routine that sends the string to the 
//          Remote Xbox Command Client
//=========================================================================
BOOL DCCMDPrintf(LPCSTR szFmt, ...)
{
    char szBuf[CCH_MAXCMDLEN];
    int cch;
    va_list arglist;

	// Zero buffer
	ZeroMemory( szBuf, sizeof(char) * CCH_MAXCMDLEN );

    // copy prefix into into buffer
    for(int ich = 0; ich < sizeof(g_szCmd); ich++)
        szBuf[ich] = g_szCmd[ich];
    szBuf[ich - 1] = '!';

    // format arguments
    va_start(arglist, szFmt);
    cch = _vsnprintf(&szBuf[ich], CCH_MAXCMDLEN - ich, szFmt, arglist);
    va_end(arglist);

    // send it out
    DmSendNotificationString(szBuf);
    return true;
}

