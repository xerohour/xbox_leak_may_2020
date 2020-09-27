//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       main.cpp
//
//--------------------------------------------------------------------------

/***************************************************************************
Error File Parser.cpp
By: Zach Robinson (zachd@microsoft.com)
Created: Aug 26, 1996
Purpose:
    To parse a list of files for error codes.  Once parsing has occured,
    the list will be sorted and printed out into a C file.  This file
    will then be used to build a program that will supply information
    about error codes supplied to it.

Incarnation:
    errparse (FILE_LIST)
    where (FILE_LIST) is N number of files that will be parsed to build
    the complete list of applicable error codes.

Requirements:
    At this point, that error files will adhere to the structure
    established by the standard MS C++ file winerror.h.

Output:
    A C file.

**************************************************************************/

#include "errparse.h"

char *pErrFileName = NULL;
char *pParsedErrorsName = "errmsgs.h";
FILE *pErrorsFile , *pParsedErrorsFile;
DWORD dwErrorsParsed = 0;

//**************************************************************
void __cdecl main (int argc, char **argv)
// (see above program description)
//**************************************************************
{

    if (2 > argc )
    {
        Usage();
    }
    
    if( !( CreateErrorFile( pParsedErrorsName ) ) )
    {
        death( ( "Error creating output file.  Program terminated.\n" ) );
    }

    //Loop for each file supplied
    for( int i=1; i<argc; i++ )
    {

        pErrFileName = argv[i];

        if( !( OpenErrorFile( pErrFileName ) ) )
        {
            death( ( "Error opening file.  Program terminated.\n" ) );
        }


        //parse -> build linked list
        //This both gets the errors and sorts them, and is the real meat of the program
        GetErrors();
        
        //close the filehandle
        if( NULL != pErrorsFile )
        {
            (void) fclose( pErrorsFile );
        }
 
    }
    

    if( !( PrintErrorMessages() ) )
    {
        death( ( "Error printing out error messages.  Program terminating.\n" ) );
    }
    

    if( !( DestroyErrorMessages() ) )
    {
        death( ( "Error cleaning up memory.  Program terminating.\n" ) );
    }
    

    //Close the output file now ( all files have been processed & printed )
    if( NULL != pParsedErrorsFile )
    {
        (void) fclose( pParsedErrorsFile );
    }  
    
    return;
}
