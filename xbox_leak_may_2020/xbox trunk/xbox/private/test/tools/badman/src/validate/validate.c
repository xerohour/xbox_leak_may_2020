/*************************************************************************
Module: Validate.C
Author: JohnMil
Date:	2-10-92

Copyright (c) 1992 Microsoft Corporation

This file contains miscellaneous files for the use of the API return
code Validation functions.
*************************************************************************/
#include <setcln.h>
#include <stdarg.h>
#include <xlog.h>

/**************************************************************************
ValPrintf is a substitute for printf which will print to the console, since
the Validation functions are in a DLL and can't print to it normally
***************************************************************************/


int ValPrintf(HANDLE hConOut, char *Format, ...)
{
  char buff[512];
  va_list vaList;
  DWORD i;

  va_start(vaList,Format);
  i=vsprintf(buff,Format,vaList);
  va_end(vaList);

  return((int) WriteFile(hConOut,buff,i,&i,NULL));
}




/***************************************************************************
ErrorPrint is a standard format error printer which will print to the
screen and to the log file, if appropriate.
***************************************************************************/

void ErrorPrint(HANDLE hConOut,HANDLE hLog,char *Module,
		int CaseNo, char *Desc)
{
  if (hConOut != NULL) {
    ValPrintf(hConOut,"\nERROR: %s [Case %d] -- %s",Module,CaseNo,Desc);
    }

  if (hLog != NULL) {
    xLog(hLog,XLL_FAIL,"\nERROR: %s [Case %d] -- %s",Module,CaseNo,Desc);
    }

  return;
}


BOOL
WINAPI
DllMain(
    HINSTANCE hInstance,
    DWORD fdwReason,
    LPVOID lpvReserved
    )
{
    if ( DLL_PROCESS_ATTACH == fdwReason ) {
        DisableThreadLibraryCalls( hInstance );
    }

    UNREFERENCED_PARAMETER( lpvReserved );
    return TRUE;
}
