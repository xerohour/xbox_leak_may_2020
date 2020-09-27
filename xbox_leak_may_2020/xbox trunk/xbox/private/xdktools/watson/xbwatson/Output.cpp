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

// UNDONE-WARN: Not modifying the edit menu based on selection state (not worth the trouble).

extern bool g_fLimitText;

#define MAXLINES 100000
#define LINESTOCUT 2000

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  PasteOutput
// Purpose:   Pastes the contents of the clipboard to the output window.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PasteOutput()
{
    // Tell the edit control "paste what's in the clipboard".
    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_PASTE, 0, 0);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CutOutput
// Purpose:   Cuts the selected text from the output window.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CutOutput()
{
    int iStart, iEnd;

    // Verify that something is selected
    SendMessage(g_hwndEdit, EM_GETSEL, (WPARAM)&iStart, (LPARAM)&iEnd);
    if (iStart == iEnd)
    {
        // No current selection
        MessageBeep(0);
        return;
    }

    // Tell the edit control "cut what's in the clipboard".
    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_CUT, 0, 0);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ClearOutput
// Purpose:   Clears the output window
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ClearOutput()
{
    SetWindowText(g_hwndEdit, "");
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DeleteOutput
// Purpose:   Deletes the selected text from the output window.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DeleteOutput()
{
    // Tell the edit control "delete what's in the clipboard".
    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_CLEAR, 0, 0);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CopyOutput
// Purpose:   Copies the current selection to the clipboard
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CopyOutput()
{
    // Tell the edit control "copy the current selection to the clipboard".
    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_COPY, 0, 0);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SelectAllOutput
// Purpose:   Selects everything in the output window
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SelectAllOutput()
{
    // Tell the edit control "select everything in the output window".
    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, EM_SETSEL, 0, -1);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  OutputMsg
// Purpose:   Utility function - dumps the specified message to the edit control.  This will
//            only work if launched from a command prompt.
// Arguments: szMsg         -- The format of the message (just like 'printf').
//            ...           -- Arguments for the message.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void OutputMsg(char *szMsg, ...)
{
    // szMsgOut         -- Buffer to hold the final formatted string to output.
    char szMsgOut[1024];

    // valist           -- vararg var used to generate final formatted string.
    va_list valist;

    if (g_fLimitText)
    {
        int nLines = SendMessage(g_hwndEdit, EM_GETLINECOUNT, 0, 0);
        if (nLines > MAXLINES)
        {
            // Oop, too much text.  Delete the first chunk to make room
            SetFocus(g_hwndEdit);
        
            // Delete the first 2000 lines
            int nCharsToDelete = SendMessage(g_hwndEdit, EM_LINEINDEX, LINESTOCUT, 0);
            SendMessage(g_hwndEdit, EM_SETSEL, 0, nCharsToDelete);
            SendMessage(g_hwndEdit, WM_CUT, 0, 0);
        }
    }

    // Move the cursor in the edit control to the very end to append the specified text.
    SendMessage(g_hwndEdit, EM_SETSEL, (WPARAM)-2, (LPARAM)-1);
    
    // Create the final formatted string.
    va_start (valist, szMsg);
    vsprintf (szMsgOut, szMsg, valist);
    va_end (valist);

    // Tell the edit control to add the specified string to the end of the text.
    SendMessage(g_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)szMsgOut);
}

