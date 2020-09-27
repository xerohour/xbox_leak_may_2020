//-----------------------------------------------------------------------------
// File: DebugConsole.cpp
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

// global environment
DebugConsole g_DCEnv;
PrintQueue   g_PrintQueue;
static const char szAppName[] = "DebugConsole";
PDMN_SESSION g_dmnSession;


//-----------------------------------------------------------------------------
// Name: InitMainDlg
// Desc: Performs some initialization, caching hwnds, etc.
//-----------------------------------------------------------------------------
void InitMainDlg()
{
    // remember some useful hwnds
    g_DCEnv.hwndOut = GetDlgItem(g_DCEnv.hDlgMain, IDC_RICHEDITCON);
    g_DCEnv.hwndCb = GetDlgItem(g_DCEnv.hDlgMain, IDC_RICHEDITCMD);
    g_DCEnv.hwndCbEdit = GetWindow(g_DCEnv.hwndCb, GW_CHILD);

    // subclass our dropdown listbox to handle return key
    g_DCEnv.wndprocLB = SubclassWindow(g_DCEnv.hwndCbEdit, SubclassedLBProc);

    ShowWindow(g_DCEnv.hDlgMain, SW_SHOWNORMAL);

    // get our render window handle
    g_DCEnv.hwndOut = GetDlgItem(g_DCEnv.hDlgMain, IDC_RICHEDITCON);
    g_DCEnv.hwndCb = GetDlgItem(g_DCEnv.hDlgMain, IDC_RICHEDITCMD);

    // change the font type to courier
    CHARFORMAT cf;

    cf.cbSize = sizeof(CHARFORMAT);
    SendMessage(g_DCEnv.hwndOut, EM_GETCHARFORMAT, 0, (LPARAM)&cf);
    
    cf.dwMask &= ~CFM_COLOR;
    lstrcpy(cf.szFaceName, "courier");
    SendMessage(g_DCEnv.hwndOut, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
}

//-----------------------------------------------------------------------------
// Name: WinMain
// Desc: Entry point for program
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
    LPSTR lpszCmdLine, int nCmdShow)
{
    HACCEL      hAccel;
    MSG         msg = {0};
    WNDCLASS    wndclass;

    // Set up our print queue
    InitializeCriticalSection( &g_PrintQueue.CriticalSection );
    g_PrintQueue.dwNumMessages = 0;

    InitCommonControls();
    // Needed for rich edit controls
    HMODULE hRichEdit = LoadLibrary("Riched32.dll");
    assert(hRichEdit);

    //
    // Set up our window class
    //
    wndclass.style         = 0;
    wndclass.lpfnWndProc   = DlgProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = DLGWINDOWEXTRA;
    wndclass.hInstance     = hInst;
    wndclass.hIcon         = LoadIcon(hInst, szAppName);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockBrush(LTGRAY_BRUSH);
    wndclass.lpszMenuName  = MAKEINTRESOURCE(MENU_DebugConsole);
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    // create our main dialog
    if(!(g_DCEnv.hDlgMain = CreateDialog(hInst, szAppName, 0, NULL)))
    {
        char sz[255];

        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, sz, 255, NULL );
        MessageBeep(0);
        goto err;
    }

    // Init RTF controls, etc.
    InitMainDlg();

    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL));

    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(g_DCEnv.hDlgMain, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

err:
    FreeLibrary( hRichEdit );
    return msg.wParam;
}



//-----------------------------------------------------------------------------
// Name: EnqueueStringForPrinting
// Desc: Formats the string and adds it to the print queue
//-----------------------------------------------------------------------------
void EnqueueStringForPrinting( COLORREF rgb, LPCTSTR lpFmt, ... )
{
    assert( g_PrintQueue.dwNumMessages <= NUM_STRINGS );

    // Enter critical section so we don't try to process the list
    EnterCriticalSection( &g_PrintQueue.CriticalSection );

    // If the queue is full, that means the main thread is probably blocked
    // and we'll have to drop this message.  Either allocate more space,
    // or make sure windows messages get processed more frequently on the
    // main thread.
    if( g_PrintQueue.dwNumMessages == NUM_STRINGS )
    {
        LeaveCriticalSection( &g_PrintQueue.CriticalSection );
        return;
    }

    // Print the message into the next slot
    va_list arglist;
    va_start( arglist, lpFmt );
    _vsnprintf( g_PrintQueue.astrMessages[ g_PrintQueue.dwNumMessages ],
                MAX_STRING_LEN,
                lpFmt,
                arglist );
    va_end( arglist );
    g_PrintQueue.aColors[ g_PrintQueue.dwNumMessages++ ] = rgb;

    // Ensure we've got a message posted to process the print queue
    if( g_PrintQueue.dwNumMessages == 1 )
        PostMessage( g_DCEnv.hDlgMain, WM_USER, 0, 0 );

    // Done - now the main thread can safely process the list
    LeaveCriticalSection( &g_PrintQueue.CriticalSection );


}

    

//-----------------------------------------------------------------------------
// Name: ProcessEnqueuedStrings
// Desc: Processes the list of enqueued strings to be printed (put there by
//       our notification handlers)
//-----------------------------------------------------------------------------
void ProcessEnqueuedStrings()
{
    // Enter critical section so we don't try to add anything while
    // we're processing
    EnterCriticalSection( &g_PrintQueue.CriticalSection );

    for( DWORD i = 0; i < g_PrintQueue.dwNumMessages; i++ )
    {
        RtfPrintf( g_PrintQueue.aColors[i], "%s", g_PrintQueue.astrMessages[i] );
    }

    g_PrintQueue.dwNumMessages = 0;

    // Done - now we can safely add to the list
    LeaveCriticalSection( &g_PrintQueue.CriticalSection );
}



//-----------------------------------------------------------------------------
// Name: RtfPrintf
// Desc: Writes out a string directly to the console window
//-----------------------------------------------------------------------------
int RtfPrintf( COLORREF rgb, LPCTSTR lpFmt, ...)
{
    int cch;
    char szT[512];
    va_list arglist;
    CHARRANGE cr = { -1, -2 };

    if(rgb != CLR_INVALID)
    {
        // set whatever colors, etc. they want
        CHARFORMAT cf = {0};

        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.dwEffects = 0;
        cf.crTextColor = rgb;

        SendDlgItemMessage(g_DCEnv.hDlgMain, IDC_RICHEDITCON, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }

    // Get our string to print
    va_start( arglist, lpFmt );
    cch = _vsnprintf(szT, sizeof(szT), lpFmt, arglist);
    va_end( arglist );

    // move the selection to the end
    SendDlgItemMessage(g_DCEnv.hDlgMain, IDC_RICHEDITCON, EM_EXSETSEL, 0, (LPARAM)&cr);

    // add the text and scroll it into view
    SendDlgItemMessage(g_DCEnv.hDlgMain, IDC_RICHEDITCON, EM_REPLACESEL, 0, (LONG)(LPSTR)szT);
    SendDlgItemMessage(g_DCEnv.hDlgMain, IDC_RICHEDITCON, EM_SCROLLCARET, 0, 0L);

    return cch;
}


//-----------------------------------------------------------------------------
// Name: RCmdHandle
// Desc: Command parser and dispatcher
//-----------------------------------------------------------------------------
bool RCmdHandle(char *szCmd)
{
    TCHAR szCmdBak[MAX_PATH];
    char *argv[C_MAXARGVELEMS];
    BOOL fBuiltinCommand = FALSE;

    // Make a copy of the original command string
    lstrcpy(szCmdBak, szCmd);

    int argc = CmdToArgv(szCmd, argv, C_MAXARGVELEMS);

    // Nothing but whitespace?
    if(!argv[0][0])
        return true;

    // See if we already have this entry in the command history
    int iIndex = ComboBox_FindStringExact(g_DCEnv.hwndCb, -1, szCmdBak);
    if(iIndex != CB_ERR)
    {
        ComboBox_DeleteString(g_DCEnv.hwndCb, iIndex);
    }
    ComboBox_InsertItemData(g_DCEnv.hwndCb, 0, szCmdBak);

    // limit the # of history items
    if((iIndex = ComboBox_GetCount(g_DCEnv.hwndCb)) > 25)
        ComboBox_DeleteString(g_DCEnv.hwndCb, iIndex - 1);

    // see if the command is recognized
    for(int iCmd = 0; iCmd < sizeof(rgDCCmds) / sizeof(rgDCCmds[0]); iCmd++)
    {
        if(!lstrcmpi(rgDCCmds[iCmd].szCmd, argv[0]))
        {
            // echo command to the window
            RtfPrintf(CLR_INVALID, "%s\n", szCmdBak);

            // If handler was null, that means send it through raw to the XBox
            if( !rgDCCmds[iCmd].pfnHandler )
            {
                fBuiltinCommand = TRUE;
                break;
            }

            // If we called a handler, and it returns false, 
            // then we should send the command remotely as
            // well, otherwise we're done
            if(!rgDCCmds[iCmd].pfnHandler(argc, argv))
                break;

            return true;
        }
    }

    // ok, try to send the command over the wire
    if(g_DCEnv.fConnected)
    {
        char    szRemoteCmd[MAX_PATH + 10];
        char    szResp[MAX_PATH];
        DWORD   cchResp = MAX_PATH;
        HRESULT hr;

        if( !fBuiltinCommand )
        {
            // 
            // App-defined commands need a command
            // prefix to get routed properly
            //
            lstrcpy(szRemoteCmd, CMD_PREFIX "!");
            lstrcat(szRemoteCmd, szCmdBak);

            // echo remote command
            RtfPrintf(CLR_INVALID, "%s\n", szRemoteCmd);
        }
        else
            lstrcpy(szRemoteCmd, szCmdBak);

        // Send the command to the Xbox
        hr = DmSendCommand(g_DCEnv.pdmConnection, szRemoteCmd, szResp, &cchResp);

        if(FAILED(hr))
            DisplayError("DmSendCommand", hr);
        else
        {
            // There are different success codes.  If your app defines
            // a command which needs or returns binary data, You will
            // need to take appropriate action here.
            switch( hr )
            {
            case XBDM_NOERR:
                if(cchResp)
                    RtfPrintf(RGB(0, 0, 255), "%s\n", szResp);
                break;
            case XBDM_MULTIRESPONSE:
                // Multi-line response - loop, looking for end of response
                for( ; ; )
                {
                    DWORD cchResp = sizeof(szResp);

                    hr = DmReceiveSocketLine( g_DCEnv.pdmConnection, szResp, &cchResp );
                    if( FAILED(hr) || szResp[0] == '.' )
                        break;
                    RtfPrintf(RGB( 0, 0, 255), "%s\n", szResp);
                }
                break;
            case XBDM_BINRESPONSE:
                RtfPrintf( RGB( 255, 0, 0 ), "Binary response - can't handle\n");
                break;
            case XBDM_READYFORBIN:
                RtfPrintf( RGB( 0, 0, 255 ), "Waiting for binary data\n");
                break;
            default:
                RtfPrintf( RGB( 255, 0, 0 ), "Unknown success code (%s).\n", szResp );
                break;
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: HandleWmKeyDown
// Desc: Handle a WmKeyDown in our RTF cmd window
//-----------------------------------------------------------------------------
bool HandleWmKeyDown(WPARAM wParam)
{
    bool fhandled = false;

    switch(wParam)
    {
    case VK_RETURN:
        // User hit return in the combo box
        if(ComboBox_GetDroppedState(g_DCEnv.hwndCb))
        {
            ComboBox_ShowDropdown(g_DCEnv.hwndCb, FALSE);
        }
        else
        {
            PostMessage(g_DCEnv.hDlgMain, WM_APP, 0, 0);
            fhandled = true;
        }
        break;
    }

    return fhandled;
}

//-----------------------------------------------------------------------------
// Name: SubclassedLBProc
// Desc: Our subclassed listbox proc
//-----------------------------------------------------------------------------
LRESULT CALLBACK SubclassedLBProc(HWND hDlg, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_KEYDOWN:
        if(HandleWmKeyDown(wParam))
            return 1;
        break;
    case WM_CHAR:
        if(wParam == VK_RETURN)
            return 1;
        break;
    }

    return CallWindowProc(g_DCEnv.wndprocLB, hDlg, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: HandleWmSize
// Desc: Handles a WM_SIZE message by resizing all our child windows to 
//          match the main window
//-----------------------------------------------------------------------------
void HandleWmSize(HWND hDlg, UINT state, int cx, int cy)
{
    if(!cx || !cy)
    {
        RECT rcClient;

        GetClientRect(hDlg, &rcClient);
        cx = rcClient.right;
        cy = rcClient.bottom;
    }

    // If we're big enough, position our child windows
    if(g_DCEnv.hwndCb && cx > 64 && cy > 64)
    {
        RECT rcCmd;

        //
        // Fit the combo box into our window
        //
        GetWindowRect(g_DCEnv.hwndCb, &rcCmd);
        ScreenToClient(hDlg, (LPPOINT)&rcCmd);
        ScreenToClient(hDlg, (LPPOINT)&rcCmd + 1);

        int x = rcCmd.left;
        int dx = cx - 4 - x;
        int dy = rcCmd.bottom - rcCmd.top;
        int y = cy - 4 - dy;

        SetWindowPos(g_DCEnv.hwndCb, NULL, x, y,
            dx, dy, SWP_NOZORDER);

        //
        // Position the "Cmd" label
        //
        RECT rcStaticCmd;
        HWND hStaticCmd = GetDlgItem(g_DCEnv.hDlgMain, IDC_Cmd);

        GetWindowRect(hStaticCmd, &rcStaticCmd);
        ScreenToClient(hDlg, (LPPOINT)&rcStaticCmd);
        ScreenToClient(hDlg, (LPPOINT)&rcStaticCmd + 1);
        SetWindowPos(hStaticCmd, NULL, 4,
            y + (dy - (rcStaticCmd.bottom - rcStaticCmd.top)) / 2 - 1,
            0, 0, SWP_NOSIZE | SWP_NOZORDER);

        RECT rcOut;

        //
        // Position the output window
        //
        GetWindowRect(g_DCEnv.hwndOut, &rcOut);
        ScreenToClient(hDlg, (LPPOINT)&rcOut);

        int dwWidth = cx - rcOut.left - 4;
        int dwHeight = y - rcOut.top - 4;

        SetWindowPos(g_DCEnv.hwndOut, NULL, 0, 0,
            dwWidth, dwHeight, SWP_NOMOVE | SWP_NOZORDER);
    }
}

//-----------------------------------------------------------------------------
// Name: DlgProc
// Desc: Our main dialog proc
//-----------------------------------------------------------------------------
LRESULT CALLBACK DlgProc(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HWND hwndMsg = (HWND)(UINT)lParam;
    WORD wID     = LOWORD(wParam);
    WORD wCode   = HIWORD(wParam);

    switch(message)
    {
    case WM_APP:
    {
        // Take the string from the command window and process it
        char szCmd[MAX_PATH + 3]; // extra room for \r\n
        ComboBox_GetText(g_DCEnv.hwndCb, szCmd, MAX_PATH);

        RCmdHandle(szCmd);

        ComboBox_SetText(g_DCEnv.hwndCb, "");
        break;
    }

    case WM_USER:
        ProcessEnqueuedStrings();
        break;

    case WM_SIZE:
        HANDLE_WM_SIZE(hDlg, wParam, lParam, HandleWmSize);
        break;

    case WM_SYSCOMMAND:
        if(wID == SC_CLOSE)
        {
            PostMessage(hDlg, WM_CLOSE, 0, 0);
            return 0;
        }
        break;

    case WM_CLOSE:
        // Must disconnect boefore closing
        RCmdDisconnect(0, NULL);
        DeleteCriticalSection( &g_PrintQueue.CriticalSection );

        DestroyWindow(hDlg);
        break;

    case WM_DESTROY:
        SubclassWindow(g_DCEnv.hwndCb, g_DCEnv.wndprocLB);
        PostQuitMessage(0);
        return 0;

    case WM_INITMENU:
        // Set up our menus
        CheckMenuItem(  (HMENU)wParam, IDM_DEBUGMONITOR, MF_BYCOMMAND | (g_DCEnv.fDebugMonitor ? MF_CHECKED : MF_UNCHECKED ) );
        EnableMenuItem( (HMENU)wParam, IDM_DEBUGMONITOR, MF_BYCOMMAND | (g_DCEnv.fConnected ? MF_ENABLED : MF_GRAYED ) );
        CheckMenuItem(  (HMENU)wParam, IDM_CONNECTTOBOX, MF_BYCOMMAND | (g_DCEnv.fConnected ? MF_CHECKED : MF_UNCHECKED ) );
        return 0;

    case WM_COMMAND:
        switch(wID)
        {
        case IDM_DEBUGMONITOR:
            g_DCEnv.fDebugMonitor = ! g_DCEnv.fDebugMonitor;
            return 0;

        case IDM_CONNECTTOBOX:
            if( g_DCEnv.fConnected )
            {
                RCmdDisconnect(0, NULL);
            }
            else
            {
                RCmdConnect( 0, NULL );
            }
            return 0;

        case IDM_Exit:
            PostMessage(hDlg, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
    }

    return DefDlgProc(hDlg, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: DisplayError
// Desc: Display friendly error by translating the hr to a message
//-----------------------------------------------------------------------------
void DisplayError(const char *szApiName, HRESULT hr)
{
    char szErr[100];
    if( !FAILED( DmTranslateError(hr, szErr, 100) ) )
    {
        if(szErr)
            RtfPrintf(RGB(255, 0, 0), "%s failed: '%s'\n", szApiName, szErr);
        else
            RtfPrintf(RGB(255, 0, 0), "%s failed: 0x%08lx\n", szApiName, hr);
    }
}

//-----------------------------------------------------------------------------
// Name: CmdToArgv
// Desc: Parse szCmd into argv and return # of args. (modifies szCmd)
//-----------------------------------------------------------------------------
int CmdToArgv(char *szCmd, char *szArg[], int maxargs)
{
    int argc = 0;
    int argcT = 0;
    char *szNil = szCmd + lstrlen(szCmd);

    while(argcT < maxargs)
    {
        // eat whitespace
        while(*szCmd && (*szCmd == ' '))
            szCmd++;

        if(!*szCmd)
        {
            szArg[argcT++] = szNil;
        }
        else
        {
            // find the end of this arg
            char chEnd = (*szCmd == '"' || *szCmd == '\'') ? *szCmd++ : ' ';
            char *szArgEnd = szCmd;
            while(*szArgEnd && (*szArgEnd != chEnd))
                szArgEnd++;

            // record this argument
            szArg[argcT++] = szCmd;
            argc = argcT;

            // move szArg to the next argument (or not, if we hit the end)
            szCmd = *szArgEnd ? szArgEnd + 1 : szArgEnd;
            *szArgEnd = 0;
        }
    }

    return argc;
}

