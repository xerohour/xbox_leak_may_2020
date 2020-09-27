/***************************************************************************
 *
 *  Copyright (C) 2/4/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       strhlp.cpp
 *  Content:    String helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/4/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactgui.h"


/****************************************************************************
 *
 *  FormatStringResource
 *
 *  Description:
 *      Loads and formats a string.
 *
 *  Arguments:
 *      HINSTANCE [in]: resource instance.
 *      UINT [in]: resource identifier.
 *
 *  Returns:  
 *      LPCTSTR: formatted string.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FormatStringResource"

void
FormatStringResource
(
    LPTSTR                  pszText,
    UINT                    nLength,
    UINT                    nResourceId,
    ...
)
{
    TCHAR                   szFormat[0x100];
    va_list                 va;

    if(LoadString(g_hInstance, nResourceId, szFormat, NUMELMS(szFormat)))
    {
        va_start(va, nResourceId);
        _vsntprintf(pszText, nLength, szFormat, va);
        va_end(va);
    }
    else
    {
        DPF_ERROR("String %lu not found", nResourceId);
        *pszText = 0;
    }
}


/****************************************************************************
 *
 *  FormatStringResourceStatic
 *
 *  Description:
 *      Loads and formats a string.
 *
 *  Arguments:
 *      HINSTANCE [in]: resource instance.
 *      UINT [in]: resource identifier.
 *
 *  Returns:  
 *      LPCTSTR: formatted string.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FormatStringResourceStatic"

LPCTSTR
FormatStringResourceStatic
(
    UINT                    nResourceId,
    ...
)
{
    static TCHAR            szFinal[0x400];
    TCHAR                   szFormat[0x100];
    va_list                 va;

    if(LoadString(g_hInstance, nResourceId, szFormat, NUMELMS(szFormat)))
    {
        va_start(va, nResourceId);
        _vsntprintf(szFinal, NUMELMS(szFinal), szFormat, va);
        va_end(va);
    }
    else
    {
        DPF_ERROR("String %lu not found", nResourceId);
        szFormat[0] = 0;
    }
    
    return szFinal;
}


/****************************************************************************
 *
 *  ParseCommandLine
 *
 *  Description:
 *      Parses a command line into a set of double-null-terminated string.
 *
 *  Arguments:
 *      LPCTSTR [in]: command line.
 *      LPCTSTR * [out]: parsed command line.  The caller is responsible for
 *                       freeing this buffer with MEMFREE.
 *      LPUINT [out]: number of arguments in the command line.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ParseCommandLine"

BOOL
ParseCommandLine
(
    LPCTSTR                 pszCmdLine,
    LPTSTR *                ppszParsed,
    LPUINT                  pnArgCount
)
{
    BOOL                    fOpenQuote  = FALSE;
    UINT                    nArgCount   = 1;
    LPTSTR                  pszParsed;
    LPTSTR                  pszDest;

    //
    // Allocate a string large enough to hold the double-NULL terminated
    // version of the arguments
    //

    if(!(pszDest = pszParsed = MEMALLOC(TCHAR, _tcslen(pszCmdLine) + 2)))
    {
        return FALSE;
    }

    //
    // Start coopying parameters
    //

    while(*pszCmdLine)
    {
        //
        // Skip leading whitespace
        //

        while(TEXT(' ') == *pszCmdLine)
        {
            pszCmdLine++;
        }

        //
        // Copy characters up to the next unquoted space
        //

        while(TRUE)
        {
            if(!*pszCmdLine)
            {
                *pszDest++ = 0;
                break;
            }
            else if(TEXT('\"') == *pszCmdLine)
            {
                fOpenQuote = !fOpenQuote;
            }
            else if((TEXT(' ') == *pszCmdLine) && !fOpenQuote)
            {
                nArgCount++;
                *pszDest++ = 0;
                break;
            }
            else
            {
                *pszDest++ = *pszCmdLine;
            }

            pszCmdLine++;
        }
    }

    *pszDest = 0;

    //
    // Fill out parameters
    //

    if(ppszParsed)
    {
        *ppszParsed = pszParsed;
    }
    else
    {
        MEMFREE(pszParsed);
    }

    if(pnArgCount)
    {
        *pnArgCount = nArgCount;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  GetXDKVersion
 *
 *  Description:
 *      Gets the current XDK version.
 *
 *  Arguments:
 *      LPTSTR [out]: version string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "GetXDKVersion"

void 
GetXDKVersion
(
    LPTSTR                  pszString
)
{
    static const DWORD      dwMajor = (VER_PRODUCTVERSION_DW >> 24) & 0xFF;
    static const DWORD      dwMinor = (VER_PRODUCTVERSION_DW >> 16) & 0xFF;
    static const DWORD      dwBuild = VER_PRODUCTVERSION_DW & 0xFFFF;
    
    _stprintf(pszString, TEXT("%lu.%lu.%lu"), dwMajor, dwMinor, dwBuild);
}


/****************************************************************************
 *
 *  FormatNumber
 *
 *  Description:
 *      Converts a number to a string, adding comma-separators.
 *
 *  Arguments:
 *      int [in]: number.
 *      BOOL [in]: signed or unsigned.
 *      BOOL [in]: add commas.
 *      LPTSTR [out]: string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FormatNumber"

void
FormatNumber
(
    int                     nValue,
    BOOL                    fSigned,
    BOOL                    fThousands,
    LPTSTR                  pszString
)
{
    LPSTR                   psz     = pszString;
    UINT                    nDigits = 0;
    UINT                    uValue;
    
    if(nValue)
    {
        if(!fSigned || (nValue > 0))
        {
            uValue = (UINT)nValue;
            
            while(uValue)
            {
                *psz++ = TEXT('0') + (uValue % 10);

                uValue /= 10;

                if(uValue && fThousands)
                {
                    if(2 == (nDigits % 3))
                    {
                        *psz++ = TEXT(',');
                    }
                }

                nDigits++;
            }
        }
        else
        {
            uValue = abs(nValue);
            
            while(uValue)
            {
                *psz++ = TEXT('0') + (uValue % 10);

                uValue /= 10;

                if(uValue && fThousands)
                {
                    if(2 == (nDigits % 3))
                    {
                        *psz++ = TEXT(',');
                    }
                }

                nDigits++;
            }

            *psz++ = TEXT('-');
        }
    }
    else
    {
        *psz++ = TEXT('0');
    }

    *psz = 0;

    _tcsrev(pszString);
}


/****************************************************************************
 *
 *  MsgBoxV
 *
 *  Description:
 *      Displays a message box.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      DWORD [in]: flags.
 *      LPCTSTR [in]: message text.
 *      ... [in]: formatting options.
 *
 *  Returns:  
 *      UINT: button clicked.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "MsgBoxV"

UINT
MsgBoxV
(
    HWND                    hWnd,
    DWORD                   dwFlags,
    LPCTSTR                 pszText,
    va_list                 va
)
{
    TCHAR                   szText[0x400];
    UINT                    nResult;
    HWND                    hWndFocus;

    //
    // Save the current keyboard focus
    //

    hWndFocus = GetFocus();

    //
    // Create the text string
    //

    _vsntprintf(szText, NUMELMS(szText), pszText, va);

    //
    // Display the message
    //

    nResult = MessageBox(hWnd, szText, g_szAppTitle, dwFlags);

    //
    // Restore focus
    //

    SetFocus(hWndFocus);

    return nResult;
}


/****************************************************************************
 *
 *  MsgBox
 *
 *  Description:
 *      Displays a message box.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      DWORD [in]: flags.
 *      LPCTSTR [in]: message text.
 *      ... [in]: formatting options.
 *
 *  Returns:  
 *      UINT: button clicked.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "MsgBox"

UINT
MsgBox
(
    HWND                    hWnd,
    DWORD                   dwFlags,
    LPCTSTR                 pszText,
    ...
)
{
    va_list                 va;
    UINT                    nResult;

    va_start(va, pszText);

    nResult = MsgBoxV(hWnd, dwFlags, pszText, va);

    va_end(va);

    return nResult;
}


/****************************************************************************
 *
 *  MsgBox
 *
 *  Description:
 *      Displays a message box.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      DWORD [in]: flags.
 *      UINT [in]: string resource identifier.
 *      ... [in]: formatting options.
 *
 *  Returns:  
 *      UINT: button clicked.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "MsgBoxResource"

UINT
MsgBoxResource
(
    HWND                    hWnd,
    DWORD                   dwFlags,
    UINT                    nStringResourceId,
    ...
)
{
    TCHAR                   szFormat[0x100];
    va_list                 va;
    UINT                    nResult;

    LoadString(g_hInstance, nStringResourceId, szFormat, NUMELMS(szFormat));
    
    va_start(va, nStringResourceId);
    
    nResult = MsgBoxV(hWnd, dwFlags, szFormat, va);
    
    va_end(va);

    return nResult;
}


