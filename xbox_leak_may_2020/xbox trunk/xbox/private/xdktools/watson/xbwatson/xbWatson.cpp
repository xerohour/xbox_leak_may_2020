// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbWatson.cpp
// Contents: Contains the windows entry point for the xbWatson executable.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <xboxverp.h>


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_hwndEdit       -- Edit control that contains the current log dump
HWND g_hwndEdit;

// g_hwnd           -- The app's main window
HWND g_hwnd;

// g_hInstance      -- Handle to this instance of xbWatson.
HINSTANCE g_hInstance;

// g_szTargetXbox   -- Name of the Xbox that we're watching.
char g_szTargetXbox[MAX_PATH];

// g_fLimitText     -- if 'true', then buffer length in text control is limited to 25000 lines.
bool g_fLimitText = false;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AboutDlgProc
// Purpose:   Message handler for About dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szBuf[1024];

	switch (message)
	{
	case WM_INITDIALOG:
        // Get the build number from the resource file
        sprintf(szBuf, "Microsoft (R) xbWatson\nBuild %d\nCopyright (C) Microsoft Corp.",
                VER_PRODUCTBUILD);
        SetDlgItemText(hDlg, IDC_ABOUTTEXT, szBuf);
		return true;

	case WM_COMMAND:
		EndDialog(hDlg, 0);
		return false;
	}
    return false;
}

WNDPROC g_editproc;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  EditProc
// Purpose:   Subclassed window procedure for the main edit control.  We subclass it so that we
//            can disallow keypresses inside it.
// Arguments: hWnd          -- Handle to the control's window
//            message       -- The message sent to the control box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK EditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CHAR:
        // Override the character so that it doesn't get displayed

        // Beep to inform the user of the invalid action
        MessageBeep(0);

        // Return zero to indicate that we handled the message
        return 0;
        
    default:
        // Pass all other messages to the base window procedure.
        return CallWindowProc(g_editproc, hWnd, message, wParam, lParam);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WndProc
// Purpose:   This is the application's Windows message notification function.  When Windows wants
//            to tell the app that something has happened (ie mouse button pressed), it does so by
//            calling into WndProc with the message and it's related information.  If this function
//            does not handle the specified message, then it is passed on to the Win32 function
//            'DefWindowProc', which allows Windows to do any handling on it that it wants.
// Arguments: hWnd          -- Handle to the window
//            message       -- The message sent to the window
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
    LOGFONT lf;
    HFONT hfont;
    HMENU hmenu;

	switch (message) 
	{
        case WM_CREATE:
            g_hwndEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL |//;// ES_READONLY |
                                      WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE |
                                      ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                      0, 0, 0, 0, hWnd, (HMENU)IDC_XBWATSON, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            g_editproc = (WNDPROC) SetWindowLong(g_hwndEdit, GWL_WNDPROC, (LONG)EditProc);

            // Set the edit control to accept the maximum number of character
            SendMessage(g_hwndEdit, EM_LIMITTEXT, 0, 0);

            // Create the GDI font object
            lf.lfHeight         = -14;
            lf.lfWidth          = 0;        lf.lfEscapement     = 0;
            lf.lfOrientation    = 0;        lf.lfWeight         = 0;
            lf.lfItalic         = 0;        lf.lfUnderline      = 0;
            lf.lfStrikeOut      = 0;        lf.lfCharSet        = 0;
            lf.lfOutPrecision   = 0;        lf.lfClipPrecision  = 0;
            lf.lfQuality        = 0;        lf.lfPitchAndFamily = 0;
            strcpy(lf.lfFaceName, "Courier New");
            hfont = CreateFontIndirect(&lf);
            SendMessage(g_hwndEdit, WM_SETFONT, (WPARAM)hfont, 0);

            // Initialize Xbox communication.
            if (FAILED(HrInitDM()))
                return FALSE;

            break;

        case WM_SIZE:
            MoveWindow(g_hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;

        case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
            case IDM_ABOUT:
                DialogBox(g_hInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutDlgProc);
                break;
            case IDM_EXIT:
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case IDM_SAVE:
                SaveLogFile(hWnd, g_hwndEdit);
                break;

            // ==== Edit selections: ====

            case ID_EDIT_SELECTALL:
                SelectAllOutput();
                break;

            case ID_EDIT_COPY:
                CopyOutput();
                break;

            case ID_EDIT_CLEAR:
                ClearOutput();
                break;

            case ID_LIMITBUFLEN:
                g_fLimitText = !g_fLimitText;
                hmenu = GetMenu(g_hwnd);
                CheckMenuItem(hmenu, ID_LIMITBUFLEN, g_fLimitText ? MFS_CHECKED : MFS_UNCHECKED);
                break;

#ifdef EDITABLE
            case ID_EDIT_CUT:
                CutOutput();
                break;

            case ID_EDIT_PASTE:
                PasteOutput();
                break;

            case ID_EDIT_DELETE:
                DeleteOutput();
                break;

#endif
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InitInstance
// Purpose:   Register our class and then create and show our window.
// Arguments: hInstance     -- The handle to this instance of the application
//            hPrevInstance -- Obsolete; throwback to Win16 -- ignore
//            szCmdLine     -- Contains the parameter list passed to this application
//            nCmdShow      -- The default visibility of this application (maximized, minimized, etc).
// Return:    true if successfully initialized..
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool InitInstance(int nCmdShow)
{
	WNDCLASSEX wcex;

    // Register the class for the main xbWatson window.
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= LoadIcon(g_hInstance, (LPCTSTR)IDI_XBWATSON);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_XBWATSON;
	wcex.lpszClassName	= "xbWatson";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	RegisterClassEx(&wcex);

    // Create the main xbWatson window.
    // UNDONE-FEATURE: Store and restore last position ran from (?)
    char sz[MAX_PATH + 30];
    sprintf(sz, "xbWatson - Log Window [%s]", g_szTargetXbox);

    g_hwnd = CreateWindow("xbWatson", sz, WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, g_hInstance, NULL);
    if (!g_hwnd)
        return false;

    // Make our window visible and refresh it.
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    return true;
}

bool HandleCmdLine(char *szCmd)
{
    if (*szCmd == NULL)
        return true;

    if ((szCmd[0] == '-' || szCmd[0] == '/') && (szCmd[1] == 'x' || szCmd[1] == 'X'))
    {
        // Get name
        char *pszName = strrchr(&szCmd[2], ' ');
        if (pszName)
            if(SUCCEEDED(DmSetXboxName(pszName+1)))
                return true;
    }

    // If here, then bad cmd line
    char szMsg[1000];
    sprintf(szMsg, "xbWatson.exe.\n\r\nusage: xbWatson [/x xboxname]\r\n"\
            "        /x    Specify Xbox to explore.");

    MessageBox(NULL, szMsg, "Invalid Command Line Option Specified", MB_ICONINFORMATION | MB_OK);
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WinMain
// Purpose:   This is the default entry point into a Windows application.  When the application is
//            started, Windows calls into this function - when this function exits, the application
//            ends.  This function creates the Win32 class that represents this application, sets
//            up the Win32 Window in which the shell will be rendered, and then goes into the Win32
//            message loop that drives the application.
// Arguments: hInstance     -- The handle to this instance of the application
//            hPrevInstance -- Obsolete; throwback to Win16 -- ignore
//            szCmdLine     -- Contains the parameter list passed to this application
//            nCmdShow      -- The default visibility of this application (maximized, minimized, etc).
// Return:    result value.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

    // Keep track of this application's Instance handle.
    g_hInstance = hInstance;

    if (!HandleCmdLine(lpCmdLine))
        return -1;

    // Perform Win32-specific application initialization.
    if (!InitInstance (nCmdShow)) 
        return FALSE;

    // Initialize keyboard accelerators
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_XBWATSON);
    
    // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
    {
		if (!TranslateAccelerator(g_hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
//        for (int i = 0; i < 100; i++)
  //          OutputMsg("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
    }

    // Uninitialize XBDM.
    UninitDM();

	return msg.wParam;
}
