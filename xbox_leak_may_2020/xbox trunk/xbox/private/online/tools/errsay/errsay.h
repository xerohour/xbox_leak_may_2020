//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       errsay.h
//
//--------------------------------------------------------------------------

#ifndef ERRSAY_H
#define ERRSAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wtypes.h>
#include <windows.h>

#define SCODE_CODE(sc)      ((sc) & 0xFFFF)

extern char *pszDesiredNumber, *pszDesiredName, *pszErrorToFind;
extern DWORD dwDesiredNumber;
extern BOOL fFoundError;

//Function prototypes
DWORD StringToDWORD( char * pszStringToDWORD );
BOOL PrintErrorMessage( ERRORMESSAGE pEMessage );
BOOL GetErrorNumber();
BOOL GetErrorName();
void NoErrorIDGiven();
void NoErrorFound();
BOOL SeekErrorByNumber();
BOOL SeekErrorByName();
void Usage();


#endif // ERRSAY_H