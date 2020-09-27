/***************************************************************************
 *
 *  Copyright (C) 1995-1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.c
 *  Content:    Debugger helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/15/00    dereks  Created.
 *
 ***************************************************************************/

#include "debug.h"

#ifdef DEBUG

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "text.h"


/***************************************************************************
 *
 *  dprintf
 *
 *  Description:
 *      Writes a string to the debugger.
 *
 *  Arguments:
 *      LPCSTR [in]: string.
 *      ... [in]: optional string modifiers.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

void dprintf(const char *pszFormat, ...)
{
    char                    szFinal[0x400]  = { 0 };
    char *                  pszString       = szFinal;
    va_list                 va;

#ifdef DPF_LIBRARY

    // Add the library name
    pszString += sprintf(pszString, DPF_LIBRARY ": ");

#endif // DPF_LIBRARY

    // Format the string
    va_start(va, pszFormat);
    pszString += vsprintf(pszString, pszFormat, va);
    va_end(va);

    // Add a carriage-return since OuputDebugString doesn't
    strcpy(pszString, CRLF);

    // Output to the debugger

#ifdef WIN32

    OutputDebugStringA(szFinal);

#else // WIN32

    printf(szFinal);

#endif // WIN32

}


#endif // DEBUG
