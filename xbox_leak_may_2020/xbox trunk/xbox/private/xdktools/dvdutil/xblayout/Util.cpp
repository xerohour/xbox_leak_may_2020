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
// Function:  DumpLayer
// Purpose:   Temp debug function
// Arguments: pol           -- List to dump
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DumpLayer(CObjList *pol)
{
#ifdef _DEBUG1
    char *rgszType[] = {"Sec", "BE", "VolDesc", "File", "Folder", "Group"};
    CObject *poCur = pol->GetOutside();
    int iLayer = 0;
    while (poCur)
    {
        char sz[1024];
        if (!(iLayer%4))
            OutputDebugString("\r\n");
        iLayer++;
        sprintf(sz, "[%s] '%s' =%d=", rgszType[poCur->GetType()],
                poCur->m_szName, poCur->m_dwLSN);
        if (poCur->m_pog)
            strcat(sz, " {g}");
        strcat(sz, " -->  ");
        OutputDebugString(sz);
        poCur = poCur->m_poInner;
    }
    OutputDebugString("NULL\r\n"); 
#endif
}

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
void FormatByteString(DWORD dwVal, char *szDest)
{
    // Set nPrecision to the number of digits after the decimal point you want.
    int nPrecision = 2;
    float r = (float)dwVal;
	char *rgsz[] = {"Bytes", "KB", "MB", "GB"};
	int iName = 0;

	while(r > 1024.0f && iName < 3)
	{
        r /= 1024.0f; iName++;
	}

    if (r - (float)((int)r) == 0.0f)
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
                         "xbLayout will now close.  Please close other open applications"\
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
    LoadString(g_hinst, dwTextId,    szText,    2048);
    LoadString(g_hinst, dwCaptionId, szCaption, 256);
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
    LoadString(g_hinst, dwTextId,    szText,    2048);
    LoadString(g_hinst, dwCaptionId, szCaption, 256);
    sprintf(szText2, szText, szParam);
    return MessageBox(hwnd, szText2, szCaption, dwFlags);
}
