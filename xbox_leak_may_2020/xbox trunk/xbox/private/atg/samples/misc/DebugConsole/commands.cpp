//-----------------------------------------------------------------------------
// File: commands.cpp
//
// Desc: Remote Xbox Debug Console
//
// Hist: 1.24.01 - New for March release
//      11.29.01 - Fixed possible hang when disconnecting 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "DebugConsole.h"
#include "xboxdbg.h"


//-----------------------------------------------------------------------------
// Name: ExtNotifyFunc
// Desc: Notifier function registered via DmRegisterNotificationProcessor 
//          below.  This is called to return output from the remote ECP
//-----------------------------------------------------------------------------
DWORD __stdcall ExtNotifyFunc(LPCSTR szNotification)
{
    EnqueueStringForPrinting(RGB(0, 0, 255), "%s\n", szNotification);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: HandleDebugString
// Desc: Prints debug output to the output window, if desired
//-----------------------------------------------------------------------------
DWORD __stdcall HandleDebugString( ULONG dwNotification, DWORD dwParam )
{
    if( g_DCEnv.fDebugMonitor )
    {
        PDMN_DEBUGSTR p = (PDMN_DEBUGSTR)dwParam;

        // The string may not be null-terminated, so
        // make a terminated copy for printing
        CHAR * pszTemp = new CHAR[ p->Length + 1 ];
        memcpy( pszTemp, p->String, p->Length * sizeof( CHAR ) );
        pszTemp[ p->Length ] = 0;
        EnqueueStringForPrinting(RGB(0xA0, 0xA0, 0xA0), "Dbg: %s\n", p->String );

        delete[] pszTemp;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name: RCmdHelp
// Desc: Handles the "help" command.  If no args, prints a list of builtin
//          and remote commands (remote only if connected).  If a command
//          is specified, prints detailed help for that command
//-----------------------------------------------------------------------------
bool RCmdHelp(int argc, char *argv[])
{
    int nCmds = sizeof(rgDCCmds) / sizeof(rgDCCmds[0]);

    if(!argv[1][0])
    {
        // No arguments - print out our list of commands, 
        // 3 per line
        for(int iCmd = 0; iCmd < nCmds; iCmd += 3)
        {
            RtfPrintf(CLR_INVALID, "%s\t%s\t%s\n",
                rgDCCmds[iCmd].szCmd,
                (iCmd + 1) < nCmds ? rgDCCmds[iCmd + 1].szCmd : "",
                (iCmd + 2) < nCmds ? rgDCCmds[iCmd + 2].szCmd : "");
        }

        if(g_DCEnv.fConnected && g_DCEnv.fECPConnected)
        {
            RtfPrintf(CLR_INVALID, "Remote Commands:\n");
            return false;   // Pass the command to ECP
        }
    }
    else
    {
        int cch = lstrlen(argv[1]);

        // Print help description for all matches
        for(int iCmd = 0; iCmd < nCmds; iCmd++)
        {
            if(!_strnicmp(rgDCCmds[iCmd].szCmd, argv[1], cch) && rgDCCmds[iCmd].szCmdHelp)
            {
                RtfPrintf(CLR_INVALID, "%s%s\n", rgDCCmds[iCmd].szCmd, rgDCCmds[iCmd].szCmdHelp);
            }
        }
    }
    return true;
 
}

//-----------------------------------------------------------------------------
// Name: RCmdCls
// Desc: Handles the CLS command by clearing the output window
//-----------------------------------------------------------------------------
bool RCmdCls(int argc, char *argv[])
{
    SetWindowText(g_DCEnv.hwndOut, "");
    return true;
}

//-----------------------------------------------------------------------------
// Name: RCmdConnect:
// Desc: Handles the connect command by first opening a connection to the
//          debug monitor on the Xbox and opening a notification session.  
//          Then we set notification handlers for debug output and our ECP.
//          Finally, we send a special __connect__ command to our ECP to
//          let him initialize himself, if an ECP-enabled app is running.
//-----------------------------------------------------------------------------
bool RCmdConnect(int argc, char *argv[])
{
    HRESULT hr;

    // Set the xbox machine name to connect to, if specified
    if(argc && argv[1][0])
    {
        hr = DmSetXboxName(argv[1]);
        if(FAILED(hr))
            DisplayError("DmSetXboxName", hr);
    }

    // Open our connection
    hr = DmOpenConnection(&g_DCEnv.pdmConnection);
    if( FAILED( hr ) )
    {
        DisplayError("DmOpenConnection", hr );
        return true;
    }

    g_DCEnv.fConnected = true;

    // Make sure we'll be able to receive notifications
    hr = DmOpenNotificationSession( 0, &g_DCEnv.pdmnSession );
    if( FAILED( hr ) )
    {
        DisplayError("DmOpenNotificationSession", hr);
        return true;
    }
    hr = DmNotify(g_DCEnv.pdmnSession, DM_DEBUGSTR, HandleDebugString);
    if( FAILED( hr ) )
    {
        DisplayError("DmNotify", hr);
        return true;
    }
    g_DCEnv.fDebugMonitor = FALSE;

    hr = DmRegisterNotificationProcessor(g_DCEnv.pdmnSession, CMD_PREFIX, ExtNotifyFunc);
    if( FAILED( hr ) )
    {
        DisplayError("DmRegisterNotificationProcessor", hr);
        return true;
    }

    // send initial connect command to ECP so it knows we're here
    {
        DWORD cchResp = MAX_PATH;
        char szResp[MAX_PATH];

        hr = DmSendCommand(g_DCEnv.pdmConnection, CMD_PREFIX "!__connect__", szResp, &cchResp);
        if(FAILED(hr))
        {
            RtfPrintf(RGB(255, 0, 0), "Couldn't connect to Application - standard debug commands only\n");
        }
        else {
            g_DCEnv.fECPConnected = TRUE;
            if(cchResp)
                RtfPrintf(RGB(0, 0, 255), "%s\n", szResp);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name: RCmdDisconnect
// Desc: Handles the disconnect command by
//          1) Shut down our notifications
//          2) Close the notification session
//          3) Close the connection
//-----------------------------------------------------------------------------
bool RCmdDisconnect(int argc, char *argv[])
{
    if(g_DCEnv.fConnected)
    {
        RtfPrintf(CLR_INVALID, "Closing connection\n");
        DmNotify(g_DCEnv.pdmnSession, DM_NONE, NULL);
        DmCloseNotificationSession( g_DCEnv.pdmnSession );
        DmCloseConnection(g_DCEnv.pdmConnection);
        g_DCEnv.fConnected = g_DCEnv.fECPConnected = false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name: RCmdQuit
// Desc: Handles the quit command by posting a WM_CLOSE message to our window
//-----------------------------------------------------------------------------
bool RCmdQuit(int argc, char *argv[])
{
    PostMessage(g_DCEnv.hDlgMain, WM_CLOSE, 0, 0);
    return true;
}

//-----------------------------------------------------------------------------
// Name: RCmdSendFile
// Desc: Handles the sendfile command by sending the arguments along to
//          DmSendFile
//-----------------------------------------------------------------------------
bool RCmdSendFile(int argc, char *argv[])
{
    if( argc != 3 )
    {
        return true;
    }

    return !FAILED( DmSendFile( argv[1], argv[2] ) );
}

//-----------------------------------------------------------------------------
// Name: RCmdGetFile
// Desc: Handles the getfile command by sending the arguments along to
//      DmReceiveFile
//-----------------------------------------------------------------------------
bool RCmdGetFile(int argc, char *argv[])
{
    if( argc != 3 )
    {
        return true;
    }

    return !FAILED( DmReceiveFile( argv[2], argv[1] ) );
}

