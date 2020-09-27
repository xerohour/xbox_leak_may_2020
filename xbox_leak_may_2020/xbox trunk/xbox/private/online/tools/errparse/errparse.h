//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       errparse.h
//
//--------------------------------------------------------------------------

#ifndef ERRPARSE_H
#define ERRPARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_LINE 1024

//extern BOOL fFoundIfDef;
extern char *pErrFileName;
extern char *pParsedErrorsName;
extern FILE *pErrorsFile , *pParsedErrorsFile;
extern DWORD dwErrorsParsed;
extern struct NODE *pCapturedError, *pFirstCapturedError;

//Prototypes for functions
void Usage( ); 
void PrintErrorFileHead( int iErrorCount );
BOOL PrintErrorMessages( );
void PrintErrorFileTail( );
BOOL OpenErrorFile( char * pFileName );
BOOL CreateErrorFile( char * pFileName );
void GetErrors( );
BOOL InsertIntoList( DWORD dwTmpErrorNumber, NODE * pCurrent );
BOOL DestroyErrorMessages( );

void death( char * pError );
            

//**************************************************************
class CErrorMessage
// Class to hold error messages for later spewing
// Basic functions: constructors, deconstructors,
// Members: ErrorNumber, SymbolicName, Description
//**************************************************************
{
public:
    CErrorMessage( DWORD dwErrorNumber = 0, char *pszSymbolicName = NULL, char *pszDescription = NULL);
    ~CErrorMessage();
    BOOL CPrint( FILE * pFile );
    BOOL CSetErrorMessage( DWORD dwErrorNumber, char *pszSymbolicName = NULL, char *pszDescription = NULL);
    DWORD CGetErrorNumber() { return( m_dwErrorNumber ); };
    const char * CGetSymbolicName() { return( m_pszSymbolicName ); };
    const char * CGetDescription() { return( m_pszDescription ); };

private:
    DWORD m_dwErrorNumber;
    char * m_pszSymbolicName;
    char * m_pszDescription;
} ;


//**************************************************************
struct NODE
//Struct for linked list
//**************************************************************
{
    CErrorMessage  ErrorMessage;
    NODE *pNext;
} ;


#endif //ERRPARSE_H