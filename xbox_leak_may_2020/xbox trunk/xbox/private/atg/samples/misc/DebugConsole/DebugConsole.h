//-----------------------------------------------------------------------------
// File: DebugConsole.h
//
// Desc: Remote Xbox Debug Console header
//
// Hist: 1.24.01 - New for March release
//      11.29.01 - Fixed possible hang when disconnecting 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>
#include <malloc.h>

#include <richedit.h>
#include <assert.h>

#include <xboxdbg.h>

#include "resource.h"

#define CMD_PREFIX      "XCMD"

#define NUM_STRINGS 50
#define MAX_STRING_LEN 512
typedef struct 
{
    CRITICAL_SECTION CriticalSection;   // Critical section
    DWORD            dwNumMessages;     // # of messages

    // Array of strings
    COLORREF         aColors[NUM_STRINGS];  // Text color
    CHAR             astrMessages[NUM_STRINGS][MAX_STRING_LEN];
} PrintQueue;

struct DebugConsole
{
    HWND hDlgMain;      // main hwnd
    HWND hwndCb;        // combobox hwnd
    HWND hwndCbEdit;    // combobox edit hwnd
    HWND hwndOut;       // output window

    int ichCaretTab;    // current caret location for tab completion
    int ichTabStart;    // current tab command

    BOOL fConnected;    // Connected to Xbox?
    BOOL fECPConnected; // Connected to ECP in application?
    BOOL fDebugMonitor; // Display debug output?

    PDMN_SESSION   pdmnSession;     // Debug Monitor Session
    PDM_CONNECTION pdmConnection;   // Debug Monitor Connection

    WNDPROC wndprocLB;
};

// global environment
extern DebugConsole       g_DCEnv;
extern PrintQueue         g_PrintQueue;

// prototypes
LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SubclassedLBProc(HWND hDlg, UINT msg,
    WPARAM wParam, LPARAM lParam);

void DisplayError(const char *szApiName, HRESULT hr);

int RtfPrintf(COLORREF rgb, LPCTSTR lpFmt, ...);

int CmdToArgv(char *szCmd, char *szArg[], int maxargs);

#define C_MAXARGVELEMS      10
#define MAX_CMDLEN          256

//
// Command handler prototypes
//
bool RCmdHelp(int argc, char *argv[]);
bool RCmdCls(int argc, char *argv[]);
bool RCmdConnect(int argc, char *argv[]);
bool RCmdDisconnect(int argc, char *argv[]);
bool RCmdQuit(int argc, char *argv[]);
bool RCmdSendFile(int argc, char *argv[]);
bool RCmdGetFile(int argc, char *argv[]);

typedef bool (*RCMDHANDLER)(int argc, char *argv[]);

void EnqueueStringForPrinting( COLORREF rgb, LPCTSTR lpFmt, ... );
void ProcessEnqueuedStrings();

#define DCCMDTAG(_cmd, _cmdhelp, _handler) { _cmd, _cmdhelp, _handler }
static const struct DCCMDS
{
    LPCTSTR     szCmd;
    RCMDHANDLER pfnHandler;
    LPCTSTR     szCmdHelp;
} rgDCCmds[] =
{
    // These are local commands
    DCCMDTAG( "help",               RCmdHelp,       " [CMD]: List commands / usage" ),
    DCCMDTAG( "cls",                RCmdCls,        ": clear the screen" ),
    DCCMDTAG( "connect",            RCmdConnect,    " [server] [portno]: connect to server @ portno" ),
    DCCMDTAG( "disconnect",         RCmdDisconnect, ": Terminate Debug Console session" ),
    DCCMDTAG( "quit",               RCmdQuit,       ": hasta luego" ),
    
    // These are routed through DmSendFile/ReceiveFile to handle file I/O
    DCCMDTAG( "sendfile",           RCmdSendFile,   " <localfile> <remotefile>: Sends a file to Xbox" ),
    DCCMDTAG( "getfile",            RCmdGetFile,    " <remotefile> <localfile>: Gets a file from Xbox" ),

    // These are all sent directly through to the debug monitor and their output will be displayed
    DCCMDTAG( "break",              NULL,           " addr=<address> | \n'Write'/'Read'/'Execute'=<address> size=<DataSize>\n['clear']: Sets/Clears a breakpoint" ),
    DCCMDTAG( "bye",                NULL,           " : Closes connection" ),
    DCCMDTAG( "continue",           NULL,           " thread=<threadid>: resumes execution of a thread which has been stopped" ),
    DCCMDTAG( "delete",             NULL,           " name=<remotefile>: Deletes a file on the Xbox" ),
    DCCMDTAG( "dirlist",            NULL,           " name=<remotedir>: Lists the items in the directory" ),
    DCCMDTAG( "getcontext",         NULL,           " thread=<threadid> 'Control' | 'Int' | 'FP' | 'Full':  Gets the context of the thread" ),
    DCCMDTAG( "getfileattributes",  NULL,           " name=<remotefile>: Gets attributes of a file" ),
    DCCMDTAG( "getmem",             NULL,           " addr=<address> length=<len> Reads memory from the Xbox" ),
    DCCMDTAG( "go",                 NULL,           " : Resumes suspended title threads" ),
    DCCMDTAG( "halt",               NULL,           " thread=<threadid> Breaks a thread" ),
    DCCMDTAG( "isstopped",          NULL,           " thread=<threadid>: Determines if a thread is stopped and why" ),
    DCCMDTAG( "mkdir",              NULL,           " name=<remotedir>: Creates a new directory on the Xbox" ),
    DCCMDTAG( "modlong",            NULL,           " name=<module>: Lists the long name of the module" ),
    DCCMDTAG( "modsections",        NULL,           " name=<module>: Lists the sections in the module" ),
    DCCMDTAG( "modules",            NULL,           " : Lists currently loaded modules" ),
    DCCMDTAG( "reboot",             NULL,           " [warm] [wait]: Reboots the xbox" ),
    DCCMDTAG( "rename",             NULL,           " name=<remotefile> newname=<newname>: Renames a file on the Xbox" ),
    DCCMDTAG( "resume",             NULL,           " thread=<threadid>: Resumes thread execution" ),
    DCCMDTAG( "setcontext",         NULL,           " thread=<threadid> Sets the context of the thread." ),
    DCCMDTAG( "setfileattributes",  NULL,           " <remotefile> <attrs>: Sets attributes of a file" ),
    DCCMDTAG( "setmem",             NULL,           " addr=<address> data=<rawdata>: Sets memory on the Xbox" ),
    DCCMDTAG( "stop",               NULL,           " : Stops the process" ),
    DCCMDTAG( "suspend",            NULL,           " thread=<threadid> Suspends the thread" ),
    DCCMDTAG( "systime",            NULL,           " : gets the system time of the xbox" ),
    DCCMDTAG( "threadinfo",         NULL,           " thread=<threadid>: Gets thread info" ),
    DCCMDTAG( "threads",            NULL,           " : gets the thread list" ),
    DCCMDTAG( "title",              NULL,           " dir=<remotedir> name=<remotexbe> [cmdline=<cmdline>]: Sets title to run" ),
    DCCMDTAG( "xbeinfo",            NULL,           " name=<remotexbe | 'running'>: Gets info on an xbe" ),

};

