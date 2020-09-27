//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       utils.cpp
//
//--------------------------------------------------------------------------

#include "errparse.h"


//**************************************************************
void Usage() 
//Show the user how to use this program
//**************************************************************
{
    printf( "Usage: errparse ...\n"
            "  FILENAME     Error file(s) to be parsed"
            "Example:\n"
            "    errparse mmserrror.h winerror.h\n\n ");

    exit( 0 );
}

//**************************************************************
void PrintErrorFileHead( DWORD dwErrorCount )
// Prints out header with the total number of error messages.
// Also define struct that error messages will adhere to.
//**************************************************************
{
    
    fprintf( pParsedErrorsFile, "#ifndef ERRMSGS_H\n" );
    fprintf( pParsedErrorsFile, "#define ERRMSGS_H\n\n" );

    fprintf( pParsedErrorsFile, "#include <stdio.h>\n" );
    fprintf( pParsedErrorsFile, "#include <stdlib.h>\n" );
    fprintf( pParsedErrorsFile, "#include <string.h>\n" );
    fprintf( pParsedErrorsFile, "#include <windows.h>\n\n" );

    fprintf( pParsedErrorsFile, "#define MAX_MES %lu\n\n", dwErrorCount );

    fprintf( pParsedErrorsFile, "struct ERRORMESSAGE\n" );
    fprintf( pParsedErrorsFile, "{\n" );
    fprintf( pParsedErrorsFile, "    DWORD num;\n" );
    fprintf( pParsedErrorsFile, "    char * name; \n" );
    fprintf( pParsedErrorsFile, "    char * desc; \n" );
    fprintf( pParsedErrorsFile, "} ;\n\n" );    
    fprintf( pParsedErrorsFile, "ERRORMESSAGE eMessages[MAX_MES] = \n" );
    fprintf( pParsedErrorsFile, "{\n" );
}


//**************************************************************
BOOL PrintErrorMessages()
//Print out all error messages to the output file
//Return false if we fail, main will cleanup
//**************************************************************
{
    PrintErrorFileHead(dwErrorsParsed);

    //Move to the front of the list for printing purposes
    pCapturedError = pFirstCapturedError;
    
    //Error Print Section
    //Continue printing until either we exit or reach the NULL pointer
    // indicating end of list
    for( ;; )
    {
        //Print contents of this element
        if ( !( pCapturedError->ErrorMessage.CPrint( pParsedErrorsFile ) ) )
        {
            return( FALSE );
        }

        //NULL pointer would indicate that the item we are on is the last item in the list
        if( NULL == pCapturedError->pNext )
        {
            break;
        }
        //Move to next element (since there now should be one <g>)
        pCapturedError = pCapturedError->pNext;
    }

    PrintErrorFileTail();

    return( TRUE );
}


//**************************************************************
void PrintErrorFileTail ()
// Closes the struct array in the c file we built
//**************************************************************
{
    fprintf( pParsedErrorsFile, "} ; \n\n\n" );
    fprintf( pParsedErrorsFile, "#endif //ERRMSGS_H\n" );
}



//**************************************************************
BOOL DestroyErrorMessages( )
//Destroy the linked list
//**************************************************************
{    
    NODE pTemp;

    pCapturedError = pFirstCapturedError;
    pTemp.pNext = pCapturedError->pNext;
    
    while( NULL != pCapturedError->pNext )
    {
        pCapturedError->ErrorMessage.~CErrorMessage();
        pCapturedError = pTemp.pNext;
        pTemp.pNext = pCapturedError->pNext;        
    }    
    
    pCapturedError->ErrorMessage.~CErrorMessage();

    return( TRUE );
}



//**************************************************************
void death( char * pError )
//The Little Death function.  Returns error, closes files.
//**************************************************************
{
    printf("%s", pError);
    if( NULL != pParsedErrorsFile )
    {
        (void) fclose( pParsedErrorsFile );
    }
    if( NULL != pErrorsFile )
    {
        (void) fclose( pErrorsFile );
    }
    exit( 1 );
}

//**************************************************************
BOOL OpenErrorFile( char * pFileName )
// Simply opens the error file for output
//**************************************************************
{
       
    //Error check: Show me what we're trying to open
    printf( "File: %s\n", pFileName );
        
    //open error file
    pErrorsFile = fopen( pFileName, "r" );
        
    //ensure fopen succeeded
    return( NULL != pErrorsFile );
}

//**************************************************************
BOOL CreateErrorFile( char * pFileName )
// Open the output file for writing to
//**************************************************************
{
    //Error check: Show me what we're trying to open
    printf( "Output file: %s\n", pFileName );
        
    //open error file
    pParsedErrorsFile = fopen( pFileName, "w" );
        
    //ensure fopen succeeded
    return( NULL != pParsedErrorsFile );
}
