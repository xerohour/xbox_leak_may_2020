// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      util.cpp
// Contents:  
// Revisions: 7-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DebugOutput
// Purpose:   Outputs a variable-argument string to the debugger.  Used like
//            printf.
// Arguments: sz            -- Format to use (ie the first parm to printf() )
//            ...           -- [optional] Variable number of arguments.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DebugOutput(char *sz, ...)
{
#ifdef _DEBUG
    char szOut[255];
    va_list args;
    va_start(args, sz);

    vsprintf(szOut, sz, args);
    OutputDebugString(szOut);
    va_end(args);
#endif
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FormatByteString
// Purpose:   
// Arguments: 
// Return:   None 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FormatByteString(ULARGE_INTEGER uliVal, char *szDest)
{
    // Set nPrecision to the number of digits after the decimal point you want.
    int nPrecision = 2;
    double r = (double)uliVal.QuadPart;
	char *rgsz[] = {"Bytes", "KB", "MB", "GB"};
	int iName = 0;

	while(r > 1024.0f && iName < 3)
	{
        r = r / 1024.0f; iName++;
	}

    if (r - (double)((int)r) == 0.0 || uliVal.QuadPart < 1024768)
        sprintf(szDest, "%d %s", (int)r, rgsz[iName]);
    else
        sprintf(szDest, "%0.*f %s", nPrecision, r, rgsz[iName]);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FatalError
// Purpose:   Called when an unrecoverable error has occurred.  Forces exit
//            after notification
// Arguments: dwErr         -- The error that occurred
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FatalError(DWORD dwErr)
{
    char szError[1024], szCaption[1024];

    switch(dwErr)
    {
    case E_OUTOFMEMORY:
        sprintf(szError, "There is not enough memory available to complete the action.\n"\
                         "xbGameDisc will now close.  Please close other open applications"\
                         " and try again.");
        sprintf(szCaption, "Unrecoverable Error: Out of Memory");
        break;
    default:
        assert(false);
        break;
    }

    MessageBox(NULL, szError, szCaption,  MB_ICONEXCLAMATION | MB_OK);
    exit(-1);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MessageBoxResource
// Purpose:   Display a messagebox using string resources
// Arguments: hwnd        -- Handle to the window 'owning' the messagebox
//            dwTextId    -- String resource id of the text string
//            dwCaptionId -- String resource id of the caption string
//            dwFlags     -- Flags for the messagebox
// Return:    Button pressed
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD MessageBoxResource(HWND hwnd, DWORD dwTextId, DWORD dwCaptionId, DWORD dwFlags)
{
    char szText[2048], szCaption[256];
    LoadString(g_hInst, dwTextId,    szText,    2048);
    LoadString(g_hInst, dwCaptionId, szCaption, 256);
    return MessageBox(hwnd, szText, szCaption, dwFlags);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MessageBoxResourceParam
// Purpose:   Display a messagebox using string resources.  Expects one string
//            param for the text string.
// Arguments: hwnd        -- Handle to the window 'owning' the messagebox
//            dwTextId    -- String resource id of the text string
//            szParam     -- String param for text string.
//            dwCaptionId -- String resource id of the caption string
//            dwFlags     -- Flags for the messagebox
// Return:    Button pressed
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD MessageBoxResourceParam(HWND hwnd, DWORD dwTextId, char *szParam, DWORD dwCaptionId, DWORD dwFlags)
{
    char szText[2048], szText2[2048], szCaption[256];
    LoadString(g_hInst, dwTextId,    szText,    2048);
    LoadString(g_hInst, dwCaptionId, szCaption, 256);
    sprintf(szText2, szText, szParam);
    return MessageBox(hwnd, szText2, szCaption, dwFlags);
}


void CenterDialog(HWND hDlg)
{
	int x, y;
    POINT pt = {0};
	RECT rc, parentrc;
	GetWindowRect(hDlg, &rc);
	GetClientRect(g_hwndMain, &parentrc);
    ClientToScreen(g_hwndMain, &pt);
	x = pt.x + (parentrc.right / 2 - (rc.right - rc.left) / 2);
	y = pt.y + (parentrc.bottom / 2 - (rc.bottom - rc.top) / 2);
	SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    BringWindowToTop(hDlg);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SetDlgControlFont
// Purpose:   Sets the font of the specified control in the specified dialog
// Arguments: hwnd                  -- The dialog with the control to set.
//            nControl              -- The control whose font we'll set
//            nHeight               -- Height of the font
//            szFace                -- Face of the font ("Arial", "Courier", etc)
//            nWeight               -- Normal (0) or Bold (FW_BOLD)
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT SetDlgControlFont(HWND hwnd, int nControl, int nHeight, char *szFace,
                          int nWeight)
{
    LOGFONT lf;

    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(hwnd, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    // Create the GDI font object
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = nHeight;
    lf.lfWeight = nWeight;
    lf.lfQuality = NONANTIALIASED_QUALITY;
    strcpy(lf.lfFaceName, szFace);
    HFONT hfont = CreateFontIndirect(&lf);
    
    // Set the control's current font to the newly created font
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)hfont, TRUE);
    
    return S_OK;
}
