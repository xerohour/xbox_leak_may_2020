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
// Hist: 02.05.01 - Initial creation for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xbdm.h>
#include <stdio.h>

#include "DebugCmd.h"

// our critical section
bool g_fcsInited = false;
CRITICAL_SECTION g_cs;

// command prefix
static const char g_szCmd[] = "XCMD";

// processorproc buffer
static char g_szRemoteBuf[CCH_MAXCMDLEN];

//=========================================================================
// Name: RCmdHelp
// Desc: Builtin command - help
//          Iterates over the list of builtin and application-provided 
//          commands and displays a help string for each one
//=========================================================================
void RCmdHelp(int argc, char *argv[])
{
    for(UINT iCmd = 0; iCmd < g_nDCCmds; iCmd++)
    {
        DCPrintf("%s\t%s\n",
            g_rgDCCmds[iCmd].szCmd,
            g_rgDCCmds[iCmd].szCmdHelp);
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
        for(nIndex = 0; nIndex < g_nDCVars; nIndex++)
        {
			CONST LPCSTR lpstr = g_rgDCVars[nIndex].lpstr;
			LPVOID lpvAddr = g_rgDCVars[nIndex].lpvAddr;
            switch(g_rgDCVars[nIndex].ddt)
            {
            case DC_bool:
                DCPrintf("%s\t= %d\n", lpstr, *(bool *)lpvAddr);
                break;
            case DC_I4:
                DCPrintf("%s\t= %d\n", lpstr, *(INT *)lpvAddr);
                break;
            case DC_I2:
                DCPrintf("%s\t= %d\n", lpstr, *(WORD *)lpvAddr);
                break;
            case DC_R4:
                DCPrintf("%s\t= %0.1f\n", lpstr, *(float *)lpvAddr);
                break;
            case DC_COLOR:
                DCPrintf("%s\t= 0x%.08x\n", lpstr, *(DWORD *)lpvAddr);
                break;
            case DC_FLOAT1:
			{
				FLOAT *pf = (float *)lpvAddr;
				DCPrintf("%s\t= %g\n", lpstr, pf[0]);
                break;
			}
            case DC_FLOAT2:
			{
				FLOAT *pf = (float *)lpvAddr;
                DCPrintf("%s\t= %g %g\n", lpstr, pf[0], pf[1]);
                break;
			}
            case DC_FLOAT3:
			{
				FLOAT *pf = (float *)lpvAddr;
                DCPrintf("%s\t= %g %g %g\n", lpstr, pf[0], pf[1], pf[2]);
                break;
			}
            case DC_FLOAT4:
			{
				FLOAT *pf = (float *)lpvAddr;
                DCPrintf("%s\t= %g %g %g %g\n", lpstr, pf[0], pf[1], pf[2], pf[3]);
                break;
			}
            case DC_MATRIX:
			{
				FLOAT *pf = (float *)lpvAddr;
                DCPrintf("%s\t= %g %g %g %g    %g %g %g %g    %g %g %g %g    %g %g %g %g\n", lpstr,
						 pf[0], pf[1], pf[2], pf[3],
						 pf[4], pf[5], pf[6], pf[7],
						 pf[8], pf[9], pf[10], pf[11],
						 pf[12], pf[13], pf[14], pf[15]);
                break;
			}
            }
        }
    }
    else
    {
        // if the user did a set "foo = 2", skip equals sign
		int iarg = 2;
        if(argv[iarg][0] == '=')
			iarg++;

        // Find the entry for this variable, if we can
        for(nIndex = 0; nIndex < g_nDCVars; nIndex++)
        {
			CONST LPCSTR lpstr = g_rgDCVars[nIndex].lpstr;
            if(!lstrcmpiA(argv[1], lpstr))
            {
                // Set the appropriate type of data
				CHAR *endptr;
				DWORD dwVal;
                LPVOID lpvAddr = g_rgDCVars[nIndex].lpvAddr;
                switch(g_rgDCVars[nIndex].ddt)
                {
                case DC_bool:
				{
					dwVal = (argv[iarg][0] == '0' && argv[iarg][1] == 'x') ?
						strtoul(argv[iarg], &endptr, 16) : atoi(argv[iarg]);
                    *(bool *)lpvAddr = !!dwVal;
                    DCPrintf("set %s = %d\n", lpstr, *(bool *)lpvAddr);
                    break;
				}
                case DC_I4:
				{
					dwVal = (argv[iarg][0] == '0' && argv[iarg][1] == 'x') ?
						strtoul(argv[iarg], &endptr, 16) : atoi(argv[iarg]);
                    *(INT *)lpvAddr = dwVal;
                    DCPrintf("set %s = %d\n", lpstr, *(INT *)lpvAddr);
                    break;
				}
                case DC_I2:
				{
					dwVal = (argv[iarg][0] == '0' && argv[iarg][1] == 'x') ?
						strtoul(argv[iarg], &endptr, 16) : atoi(argv[iarg]);
                    *(WORD *)lpvAddr = (WORD)dwVal;
                    DCPrintf("set %s = %d\n", lpstr, *(WORD *)lpvAddr);
                    break;
				}
                case DC_R4:
				{
                    *(float *)lpvAddr = (float)atof(argv[iarg]);
                    DCPrintf("set %s = %0.1f\n", lpstr, *(float *)lpvAddr);
                    break;
				}
				case DC_COLOR:
				{
					dwVal = (argv[iarg][0] == '0' && argv[iarg][1] == 'x') ?
						strtoul(argv[iarg], &endptr, 16) : atoi(argv[iarg]);
                    *(DWORD *)lpvAddr = dwVal;
					DCPrintf("set %s = 0x%.08x\n", lpstr,
						*(DWORD *)lpvAddr);
					break;
				}
				case DC_FLOAT1:
				{
					FLOAT *pf = (float *)lpvAddr;
					for (INT i = 0; iarg < argc && i < 1; iarg++, i++)
						pf[i] = (float)atof(argv[iarg]);
					DCPrintf("set %s = %g\n", lpstr, pf[0]);
					break;
				}
				case DC_FLOAT2:
				{
					FLOAT *pf = (float *)lpvAddr;
					for (INT i = 0; iarg < argc && i < 2; iarg++, i++)
						pf[i] = (float)atof(argv[iarg]);
					DCPrintf("set %s = %g %g\n", lpstr, pf[0], pf[1]);
					break;
				}
				case DC_FLOAT3:
				{
					FLOAT *pf = (float *)lpvAddr;
					for (INT i = 0; iarg < argc && i < 3; iarg++, i++)
						pf[i] = (float)atof(argv[iarg]);
					DCPrintf("set %s = %g %g %g\n", lpstr,
							 pf[0], pf[1], pf[2]);
					break;
				}
				case DC_FLOAT4:
				{
					FLOAT *pf = (float *)lpvAddr;
					for (INT i = 0; iarg < argc && i < 4; iarg++, i++)
						pf[i] = (float)atof(argv[iarg]);
					DCPrintf("set %s = %g %g %g %g\n", lpstr,
							 pf[0], pf[1], pf[2], pf[3]);
					break;
				}
				case DC_MATRIX:
				{
					FLOAT *pf = (float *)lpvAddr;
					for (INT i = 0; iarg < argc && i < 16; iarg++, i++)
						pf[i] = (float)atof(argv[iarg]);
					DCPrintf("set %s = %g %g %g %g    %g %g %g %g    %g %g %g %g    %g %g %g %g\n", lpstr,
							 pf[0], pf[1], pf[2], pf[3],
							 pf[4], pf[5], pf[6], pf[7],
							 pf[8], pf[9], pf[10], pf[11],
							 pf[12], pf[13], pf[14], pf[15]);
					break;
				}
                }

                // call the notif func if there was one
                if(g_rgDCVars[nIndex].pfnNotifFunc)
                    g_rgDCVars[nIndex].pfnNotifFunc(lpvAddr);

                return;
            }
        }

        DCPrintf("variable '%s' not found\n", argv[1]);
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
// Name: DCPRocessorProc
// Desc: Cmd notification proc
//
//  This is what is called by the Xbox debug monitor to have us process
//  a command.  What we'll actually attempt to do is tell it to make calls
//  to us on a separate thread, so that we can just block until we're 
//  able to process a command.
//=========================================================================
HRESULT __stdcall DCProcessorProc(LPCSTR szCommand, LPSTR szResp,
    DWORD cchResp, PDM_CMDCONT pdmcc)
{

    // skip over prefix
    LPCSTR szCmd = szCommand + lstrlenA(g_szCmd) + 1;
    BOOL   fKnownCommand = FALSE;

    // check if this is the initial connect signal
    if(dbgstrcmpn(szCmd, "__connect__", 11))
    {
        // if so, respond that we're connected
        lstrcpynA(szResp, "Connected.", cchResp);
        return XBDM_NOERR;
    }

    // check to see if the cmd exists
    for (UINT iCmd = 0; iCmd < g_nDCCmds; iCmd++)
    {
        if(dbgstrcmpn(g_rgDCCmds[iCmd].szCmd, szCmd, dbgstrlen(g_rgDCCmds[iCmd].szCmd)))
        {
            // If we find the string, copy it into the command buffer
            // to be examined by the polling function
            fKnownCommand = TRUE;
            break;
        }
    }

    if( fKnownCommand )
    {
        // g_szRemoteBuf needs to be protected by the critical section
        EnterCriticalSection(&g_cs);
        if( g_szRemoteBuf[0] )
        {
            // This means the application has probably stopped polling for debug commands
            dbgstrcpy( szResp, "Cannot execute - previous command still pending" );
        }
        else
        {
            dbgstrcpy( g_szRemoteBuf, szCmd );
        }
        LeaveCriticalSection(&g_cs);
    }
    else
    {
        dbgstrcpy( szResp, "unknown command" );
    }

    return XBDM_NOERR;
}

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
	const int maxargs = 30;
    char *      szArgv[maxargs];
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
        HRESULT hr = DmRegisterCommandProcessor(g_szCmd, DCProcessorProc);
        if(FAILED(hr))
            return false;

        fRegistered = true;
    }

    // If there's nothing waiting, return.
    if( !g_szRemoteBuf[0] )
    {
        return false;
    }

    // 
    // Grab a copy of the command to free up
    // the crtical section
    //
    EnterCriticalSection(&g_cs);

    lstrcpyA( szLocalBuf, g_szRemoteBuf );
    g_szRemoteBuf[0] = 0;

    LeaveCriticalSection(&g_cs);

    // Now process the command
    argc = CmdToArgv(szLocalBuf, szArgv, maxargs);

    // Find the entry in our command list
    for (UINT iCmd = 0; iCmd < g_nDCCmds; iCmd++)
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
// Name: DCPrintf 
// Desc: Asynchronous printf routine that sends the string to the 
//          Remote Xbox Command Client
//=========================================================================

BOOL DCPrintf(LPCSTR szFmt, ...)
{
	extern bool g_bQuiet;
	if (g_bQuiet)
		return true;

    char szBuf[CCH_MAXCMDLEN];
    int cch;
    va_list arglist;

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

