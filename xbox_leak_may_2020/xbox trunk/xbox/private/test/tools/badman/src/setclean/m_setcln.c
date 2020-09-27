/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    m_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'M'. It also contains setup and cleanup
    funtions beginning with 'LPM'. For more information, please refer
    to BadMan.Doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and removed not needed cases

--*/


#include <setcln.h>
#include <m_cases.bmh>
#include <memmgt.h>


LPMEMORYSTATUS LPMEMORYSTATUSSetup(int CaseNo, LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    HANDLE hMem;
    LPMEMORYSTATUS lpmsMem;
    DWORD dwSize;
    
    
    dwSize = sizeof(MEMORYSTATUS);
    
    switch(CaseNo) {
    case LPMEMORYSTATUS_NULL:
        lpmsMem = (LPMEMORYSTATUS) NULL;
        break;
    case LPMEMORYSTATUS_BUFFER_TOO_SMALL:
        dwSize = sizeof(DWORD);
    case LPMEMORYSTATUS_GOOD:
        hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, dwSize);
        *SCInfo = (LPVOID) hMem;
        lpmsMem = (LPMEMORYSTATUS) GlobalLock(hMem);
        if (lpmsMem == NULL) {
            ErrorPrint(hConOut,hLog,"LPMEMORYSTATUSSetup",CaseNo,"Unable to allocate buffer");
        }
        lpmsMem->dwLength = sizeof(MEMORYSTATUS);
        break;
    default:
        ErrorPrint(hConOut,hLog,"LPMEMORYSTATUSSetup",CaseNo,"Unknown Case");
        lpmsMem = NULL;
        break;
    }
    
    return(lpmsMem);
}


void LPMEMORYSTATUSCleanup(int CaseNo, LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
    case LPMEMORYSTATUS_BUFFER_TOO_SMALL:
    case LPMEMORYSTATUS_GOOD:
        GlobalFree((HANDLE) *SCInfo);
        break;
    default:
        break;
    }
}
