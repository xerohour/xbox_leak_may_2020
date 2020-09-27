/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    Console.c

Abstract:

     console interface (will be made UI-less for X-Box)

Author / ported by:

    John Daly (johndaly) porting date 4/29/2000

Environment:

    X-Box

[Notes:]

    port from the NT sdstress for X-Box testing

Revision History:

    initial port: johndaly
        port from NT

--*/

//
// include files
//

#include "windows.h"

BOOL 
WriteTextConsole (
    char *pszText, 
    WORD Attribute, 
    DWORD LoggedLevel
    )
/*++

Routine Description:

    WriteTextConsole writes some text, in the given attribute, to
    the console.  This function also writes logging information.
    x-box - ignore attribute, and just output debug string and log

Arguments:

    pszText - Pointer to the Text to be written to the console
    Attribute - Contains the foreground/background color of the text
    LoggedLevel - Contains Logging level (based on NTLOG.DLL)

Return Value:

    TRUE is success
    FALSE if not

--*/ 
{

    if((FALSE == LoggedLevel) && (TRUE == bDebug)){
        OutputDebugStringA (pszText);
    } 

    return(1);
}

BOOL PrintError (
    long ErrorCode
    )
/*++

Routine Description:

    This function converts Win32 Error Codes to the appropriate message

Arguments:

    Parameters:  ErrorCode - Contains Win32 ErrorCode

Return Value:

    TRUE if successful
    FALSE if not

--*/ 
{
    char szBuffer[240];

    sprintf (szBuffer,
             "Error code: %ld\n",
             ErrorCode);
    
    return(TRUE);
}

BOOL PrintSDError (
    long ErrorCode
    )
/*++

Routine Description:

    This function displays SdStress specific errors

Arguments:

    ErrorCode - Contains SdStress error code

Return Value:

    TRUE if successful
    FALSE if not

--*/ 
{
    char szBuffer[100];

    switch ( ErrorCode ) {
    case E_CDROMCOPY:
        sprintf(szBuffer, "CDROM Copy failure.\n");
        break;
    case E_COPY:
        sprintf(szBuffer, "Copy failed.\n");
        break;
    case E_TESTFILE:
        sprintf(szBuffer, "Testfile creation failed.\n");
        break;
    case E_MEMORY:
        sprintf(szBuffer, "Memory allocation error.\n");
        break;
    case E_VERIFY:
        sprintf(szBuffer, "Verify failed.\n");
        break;
    case E_VERIFYREAD:
        sprintf(szBuffer, "Verify readback failed.\n");
        break;
    case E_VERIFYCOPY:
        sprintf(szBuffer, "Verify after copy failed.\n");
        break;
    case E_NOHANDLE:
        sprintf(szBuffer, "Unable to acquire handle.\n");
        break;
    case E_DELETE:
        sprintf(szBuffer, "Delete failed.\n");
        break;
    case E_DISKSPACE:
        sprintf(szBuffer, "Out of disk space.\n");
        break;
    case E_ACCESSSdStress:
        sprintf(szBuffer, "Undefined. E\n");
        break;
    case E_NOPOINTER:
        sprintf(szBuffer, "Null pointer assignment.\n");
        break;
    case E_SEEK:
        sprintf(szBuffer, "Seek failed.\n");
        break;
    case E_DEALLOC:
        sprintf (szBuffer, "Memory DE-allocation failed.\n");
        break;
    case E_THREAD:
        sprintf (szBuffer, "Thread creation failed.\n");
        break;
    default:
        sprintf(szBuffer, "Undefined.\n");
        break;
    }

    WriteTextConsole(szBuffer, ERRORCOLOR, XLL_WARN);
    return(TRUE);
}

void 
Debug_Print (
    char *Text
    )
/*++

Routine Description:

    This function will print Debug output on the screen if the switch 
    is selected

Arguments:

    Text - Pointer to buffer containing text to be written

Return Value:

    None    

--*/ 
{
    //
    // Return if the text pointer is NULL
    //

    if ( !Text )
        return;

    if ( bDebug ) {
        WriteTextConsole (Text, DEBUGCOLOR, FALSE);
    }

    return;
}
