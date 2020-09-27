//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       main.cpp
//
//--------------------------------------------------------------------------

#include "errmsgs.h"
#include "errsay.h"

char *pszDesiredNumber, *pszDesiredName, *pszErrorToFind;
DWORD dwDesiredNumber;
BOOL fFoundError = FALSE;



//*****************************************************************************
void Usage ()
// Informs the user of how to use this program
//*****************************************************************************
{
    printf( "Usage: errsay ERROR_ID\n" );
    printf( "     ERROR_ID can be either a number or a symbolic name.\n" );
    printf( "     If hex, ERROR_ID must begin with a leading \"0x\".\n" );
    printf( "Examples of each case:\n" );
    printf( "     errsay 23                                 [DECIMAL ID]\n" );
    printf( "     errsay 0x8009001F                         [HEX ID]\n" );
    printf( "     errsay ERROR_ATOMIC_LOCKS_NOT_SUPPORTED   [SYMBOLIC NAME]\n\n" );

    exit( 0 );
}

//*****************************************************************************
void NoErrorIDGiven()
// Inform the user that the error ID was not understood
//*****************************************************************************
{
    printf( "Unable to interpret error ID \"%s\".\n\n", pszErrorToFind );
    Usage();    
}

//*****************************************************************************
void NoErrorFound()
// Indicate to user that no corresponding error was found for the ID supplied
//*****************************************************************************
{
    printf( "The error %s (0x%X) was not found.\n\n", pszErrorToFind, StringToDWORD( pszErrorToFind ) );
    Usage();    
}



//*****************************************************************************
BOOL PrintErrorMessage( ERRORMESSAGE pEMessage )
// Indicate to user the correponding error(s) for the ID supplied
//*****************************************************************************
{
    if( fFoundError )
    {
        printf( "   [SYMBOL MULTIPLY DEFINED]\n" );
    }
        
    printf( "Error Number: %lu (0x%X)\n", pEMessage.num, pEMessage.num ); 
    printf( "Symbolic Name: %s\n", pEMessage.name );
    printf( "Text description: %s\n", pEMessage.desc );
    fFoundError = TRUE;

    return( TRUE );
}

//*****************************************************************************
DWORD StringToDWORD( char * pszStringToDWORD )
// Utility function to render a decimal string to a DWORD for later hexification
//*****************************************************************************
{
    DWORD dwTmpErrorNumber;
    
    if( 0 == ( sscanf( pszStringToDWORD, "%lu", &dwTmpErrorNumber ) ) )
    {
        return ( 0 );
    }    
    
    return( dwTmpErrorNumber );
}

//*****************************************************************************
BOOL GetErrorNumber()
// Take the input string, which should be either a hex or a dec number,
// and return a dec string
//*****************************************************************************
{
    DWORD dwTmpErrorNumber = 0;
    

    //This can be either hex or a digit
    //If this is a digit, there will be no Ox
    // This second search must belong in FindErrorNumber
    

    pszDesiredNumber = pszErrorToFind;

    if( 2 < strlen( pszErrorToFind ) )
    {
        if( ( 'x' == pszErrorToFind[1] ) || ( 'X' == pszErrorToFind[1] ) )
        {
            for( int iPos = 0, iLen =( strlen( pszErrorToFind ) ); iPos < iLen; iPos++ )
            {
                //This magically solves part of the scode mystery later!
                pszErrorToFind[iPos] = (char)tolower( pszErrorToFind[iPos] ); 
            }

            //Temporarily make the decimal string (which is hex) a  real hex num...
            if( 0 == ( sscanf( pszErrorToFind, "%x", &dwTmpErrorNumber ) ) )
            {
                return( FALSE );
            }


            //Make the new hex number a decimal string for seeking with
            if( sprintf( pszDesiredNumber, "%lu", dwTmpErrorNumber ) )
            {
                //We're returning now because we've handled the hex number
                return( TRUE );
            } 
            else
            {
                return( FALSE );
            }
        }
        else
        {
            pszDesiredNumber = pszErrorToFind;            
        }
    }

    else
    {        
        pszDesiredNumber = pszErrorToFind;
    }

    
    return( TRUE );
}
        
//*****************************************************************************
BOOL GetErrorName()
// We're searching by string - we're okay with what we have
//*****************************************************************************
{
    pszDesiredName = pszErrorToFind;

    return( TRUE );
}

//*****************************************************************************
BOOL SeekErrorByNumber()
// Seeks by decimal number for error message text, and then prints out the result
//*****************************************************************************
{
    //IF NUM
    for( int i = 0; i < MAX_MES; i++ )
    {
        if( StringToDWORD( pszDesiredNumber ) == eMessages[i].num )
        {
            PrintErrorMessage( eMessages[i] );
        }
    }

    //If we haven't found the error yet, we need to check if it's available as an scode
    if( !( fFoundError ) )
    {
        char * pszFirstChar;
        
        //ERROR: this does not cover scode decimals ... does it need to?

        char szHexErrorToFind[MAX_PATH];
        sprintf( szHexErrorToFind, "0x%X", StringToDWORD( pszErrorToFind ) ); 

        if (NULL != (pszFirstChar = strstr( szHexErrorToFind, "0x8" ) ) )
        {

        //Looks like SCODEs are taken care of elsewhere...
        /* dwDesiredNumber = SCODE_CODE( ( (HRESULT) szHexErrorToFind ) );           
            printf("SCODE found.  Converted to: %lu\n", dwDesiredNumber);
            
            
            sprintf( pszDesiredNumber, "%lu", dwDesiredNumber );
            for( i = 0; i < MAX_MES; i++ )
            {
                if( StringToDWORD( pszDesiredNumber ) == eMessages[i].num )
                {
                    PrintErrorMessage( eMessages[i] );      
                }
            }
*/                
            //Maybe this a Win32 Error Code?
            if (NULL != (pszFirstChar = strstr( szHexErrorToFind, "0x8007" ) ) )
            {

                //offset by "0x8007"
                sprintf( szHexErrorToFind, "%s", pszFirstChar + 6 );

                sscanf( szHexErrorToFind, "%X", &dwDesiredNumber );
                printf("Win32 Error Code found.  Converted to: %lu\n\n", dwDesiredNumber);

                sprintf( pszDesiredNumber, "%lu", dwDesiredNumber );
                for( i = 0; i < MAX_MES; i++ )
                {
                    if( StringToDWORD( pszDesiredNumber ) == eMessages[i].num )
                    {
                        PrintErrorMessage( eMessages[i] );      
                    }
                }
            }
        }
    }

    return( fFoundError );
}

//*****************************************************************************
BOOL SeekErrorByName()
// Seeks by Symbolic Name for error message text, and then print out the result
//*****************************************************************************
{

    for( int i = 0; i < MAX_MES; i++ )
    {        
        //Case insensitive compare to see if we have a weiner
        if( ( strlen( pszDesiredName ) == strlen( eMessages[i].name ) ) && ( 0 == _strnicmp( pszDesiredName, eMessages[i].name, ( strlen( pszDesiredName ) - 1 ) ) ) ) 
        {
            PrintErrorMessage( eMessages[i] );
        }        
    }

    return( fFoundError );
}



//*****************************************************************************
void __cdecl main ( int argc, char **argv )
// See Usage for exact details
// Takes an error ID in the form of either a dec or hex number or a symbolic
// name, and then returns the associated error code(s) as supplied by
// errparse's rendering of error header files.

// Possible future refinements:
//         Multiple arguments accepted & sought for (currently drops all but first)
//         Return/reply with error number as same value as received
//*****************************************************************************
{
    char * pErrText = NULL;

    if( 2 != argc )
    {  
        Usage();
    }

    //Get cmd line arg
    pszErrorToFind = argv[1];
    pszErrorToFind[( strlen( argv[1] ) )] = '\0';

    // Arg can either be symbolic name, hex, or decimal
    //Determine which now & handle appropriately
    if( ( isdigit( pszErrorToFind[0] ) ) || ( '-' == pszErrorToFind[0] ) )
    {
        if ( !( GetErrorNumber() ) )
        {
            NoErrorIDGiven();
        }
    
        if ( !( SeekErrorByNumber() ) )
        {
            NoErrorFound();
        }
    }
    else
    {
        if ( !( GetErrorName() ) )
        {
            NoErrorIDGiven();
        }
        if ( !( SeekErrorByName() ) )
        {
            NoErrorFound();
        }
    }

   exit( 0 );
}
