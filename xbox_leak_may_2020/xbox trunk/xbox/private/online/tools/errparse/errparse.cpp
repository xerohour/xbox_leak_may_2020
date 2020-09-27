//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       errparse.cpp
//
//--------------------------------------------------------------------------

#include "errparse.h"

struct NODE *pCapturedError, *pFirstCapturedError, *pCurrent;

//************************************************************************
char * FindDesiredWord( const char * pDesiredWord, char szCurFileLine[] )
// Find a keyword within our output file.  If we run out of file, we return
// false, otherwise we continue until we find the word.
//************************************************************************
{
	size_t i = 0;
	char * pFirstChar;  
	
    //While we still have lines in the file
	while( NULL != fgets( szCurFileLine, MAX_LINE, pErrorsFile ) )
	{
    
        
        //Check to see if current line contains desired keyphrase
	    if( NULL != ( pFirstChar = strstr( szCurFileLine, pDesiredWord ) ) )
        {
            //Point past the end of the keyphrase in the line
	        pFirstChar += strlen( pDesiredWord );
            //Ensure we're not pointing to end of line
            for( ;; )
            {
                if( EOF == *pFirstChar )
                {
                    return( NULL );
                }

                if( '\0' == *pFirstChar )
                {
                    //Get next line, point to it
                    if( NULL == fgets( szCurFileLine, MAX_LINE, pErrorsFile ) )
                    {
                        return( NULL );
                    }
                    pFirstChar = szCurFileLine;
                }
    	    
                //Skip to the next word/number for processing
                if( isalnum( *pFirstChar ) )
                {
                    if( 0 == strcmp( pDesiredWord, "MessageId:" ) ) 
                    {
                        char *pPtr;

                        pPtr = strstr( pFirstChar, "SymbolicName=" );
                        if( pPtr )
                        {
                            pFirstChar = pPtr + strlen( "SymbolicName=" );
                        }
                    }
                    //Return pointer to the first char of the word they're looking for
                    return( pFirstChar );  
                }
                pFirstChar++;
            }
            
            return( pFirstChar );
        }
	}
	//The file we were looking at did not contain the word we sought
    return( NULL );
}

//**************************************************************
void GetErrors()
//  The Big Kahuna function
//   In this function we will get all of the error entries from the
//   header files, and then place them sorted-wise into our linked list
//**************************************************************
{
    
    char *MSG_SYM_ID = "MessageId:";
	char *MSG_TXT_ID = "MessageText:";
    //We actually have to seek on the symbolic name, since that is the first
    //nonspace standardly before the identifying num
 
    char szCurFileLine[MAX_LINE + 1];
	char szTmpSymbolicName[MAX_LINE + 1];
    char szTmpErrorNumber[MAX_LINE + 1];
    char szTmpDefinition[MAX_LINE + 1];
    char *pszDesiredWord;
    DWORD dwTmpErrorNumber = 0;
    
	for( ; ; )
    {
 		//GetErrorSymbolicName()
        if( NULL != ( pszDesiredWord = FindDesiredWord( MSG_SYM_ID, szCurFileLine ) ) )
		{
			//We now have a pointer to the first letter of the word we want
            //Now - determine length of string, copy that string to a temp location for building object
            int iWordLen = 1;  
            for( ; __iscsym( pszDesiredWord[( iWordLen - 1 )] ); iWordLen++ )
            {
                ;
            }
            //We are pointing beyond the end of the string at this point, so -1
            strncpy( szTmpSymbolicName, pszDesiredWord, ( iWordLen  ) );
            szTmpSymbolicName[( iWordLen -1 )] = '\0';            
		}
        else
        {
            return;
        }


        //GetErrorText()
        if( NULL != ( pszDesiredWord = FindDesiredWord( MSG_TXT_ID, szCurFileLine ) ) )
		{
			//We now have a pointer to the first letter of the word we want
            //Now - determine length of string, copy that string to a temp location for building object
            int iWordLen = 1;  
            for( ; ( '\n' != pszDesiredWord[( iWordLen - 1 )] ) && ( '\0' != pszDesiredWord[( iWordLen - 1 )] ); iWordLen++)
            {
                ;
            }            

            //iWordLen is now beyond the end of the string
            strncpy( szTmpDefinition, pszDesiredWord, ( iWordLen - 1 ) );
            szTmpDefinition[( iWordLen - 1 )] = '\0';


            //Now we handle for multiple line error descriptions                                   
            //A line that has additional text on it will be a minimum of 4 chars long                                
            while( ( NULL != fgets( szCurFileLine, MAX_LINE, pErrorsFile ) ) && ( 3 < strlen( szCurFileLine ) ) )
            {
                int iLineLen = 0;
                                                                
                for( ; ( '\n' != pszDesiredWord[( iLineLen - 1 )] ) && ( '\0' != pszDesiredWord[( iLineLen - 1 )] ); iLineLen++)
                {
                    ;
                }            
                
                //We need to append a space to the prior string, since we end and begin on alphanum chars
                strcat( szTmpDefinition, " " );
                //iLineLen is now beyond the end of the string
                strncat( szTmpDefinition, pszDesiredWord, ( iLineLen - 1 ) );
                                               
                iWordLen += iLineLen;
        
            }
        }
        else
        {
            return;
        }


        //GetErrorNumber();
        //Symbolic Name should be the first whitespace before the ErrorID Num
        if( NULL != ( pszDesiredWord = FindDesiredWord( szTmpSymbolicName, szCurFileLine ) ) )
		{
			int iWordLen = 1;  
            char * pHresultLoc;

            //We must account for _HRESULT_TYPEDEF as found in Winerror.h
            //Here, we just move the pointer to point past it

            if(NULL != ( pHresultLoc = strstr( pszDesiredWord, "HRESULT" ) ) )
            
            {
                //strlen wasn't getting the ( as part of length
                pszDesiredWord += ( strlen( "HRESULT_TYPEDEF" ) + 2 );
            }

            
            for( ; isalnum( pszDesiredWord[( iWordLen -1 )] ); iWordLen++)
            {
                ;
            }
            //We are beyond the end of the string
            strncpy( szTmpErrorNumber, pszDesiredWord, ( iWordLen - 1 ) );
            
            
            //Ensure that we didn't catch an extra character (such as L...)
            if( !( isdigit( szTmpErrorNumber[( iWordLen - 2 )] ) ) )
            {
                //Decrement to account for extra unwanted letter
                iWordLen--;
            }
            szTmpErrorNumber[( iWordLen - 1 )] = '\0';
  
		}
        else
        {
            return;
        }
       
        //Now that we have the errornumber as a string, we want to convert it to a DWORD
        if( ( 'x' == szTmpErrorNumber[1] ) || ( 'X' == szTmpErrorNumber[1] ) )
        {
            for( int iPos = 0, iLen =( strlen( szTmpErrorNumber ) - 1 ); iPos < iLen; iPos++ )
            {
                szTmpErrorNumber[iPos] = (char)tolower( szTmpErrorNumber[iPos] );
            }
            if ( 0 == ( sscanf( szTmpErrorNumber, "%x", &dwTmpErrorNumber ) ) )
            {
                //death( ( "Could not process error number.  Terminating.\n" ) );
                printf( "Could not process error number.\n" );
                continue;
            }
        }
        else
        {
            if( 0 == ( sscanf( szTmpErrorNumber, "%lu", &dwTmpErrorNumber ) ) )
            {
                //death( ( "Could not process error number.  Terminating.\n" ) );
                printf( "Could not process error number.\n" );
                continue;
            }
        }

//        printf("Adding error 0x%08x name='%s' desc='%s'\n", dwTmpErrorNumber, szTmpSymbolicName, szTmpDefinition);
        //can't add to the list if there is no list yet
        if( 0 == dwErrorsParsed )
        {
            if( NULL == ( pCurrent = new struct NODE ) )
            {
                death( ( "Error creating object. Terminating.\n" ) );
            }
            pCapturedError = pCurrent;
            pCapturedError->ErrorMessage.CSetErrorMessage( dwTmpErrorNumber, szTmpSymbolicName, szTmpDefinition );
            // now we need a pointer to the first item in the list
            pFirstCapturedError = pCapturedError;
            pCapturedError->pNext = NULL;            
            dwErrorsParsed++;
        }
        else
        {
            if( NULL == ( pCurrent = new struct NODE ) )
            {
                death( ( "Error creating object. Terminating.\n" ) );
            }
            pCurrent->ErrorMessage.CSetErrorMessage( dwTmpErrorNumber, szTmpSymbolicName, szTmpDefinition );
            if( !( InsertIntoList( dwTmpErrorNumber, pCurrent ) ) )
            {
                death( ( "Error inserting object into list. Terminating.\n" ) );
            }                   
        }
    }
	return;
}


//**************************************************************
BOOL InsertIntoList( DWORD dwTmpErrorNumber, NODE *pCurrent )
// Inserts an element to the list in sorted order
//**************************************************************
{
    dwErrorsParsed++;

    //we want to put this new node in the right place.
    //in order to do that, we need:
    // 1. to find desired loc - (dwTmpErrorNumber is our index string)
    // 2. to switch the pointers

    pCapturedError = pFirstCapturedError;
    
    do
    {        
        //Special handling if we're at end of list (or if we just started, too)
        if( NULL == pCapturedError->pNext )
        {
            if( dwTmpErrorNumber <= pCapturedError->ErrorMessage.CGetErrorNumber() )
            {
                pCurrent->pNext = pCapturedError;
                pFirstCapturedError = pCurrent;

                return( TRUE );
            }
            else
            {   //We can swap normally
                pCurrent->pNext = NULL;
                pCapturedError->pNext = pCurrent;

                return( TRUE );
            }
        }
        //If we have the same # as the beginning of the list, this is our out
        if( dwTmpErrorNumber <= pFirstCapturedError->ErrorMessage.CGetErrorNumber() )
        {
            pCurrent->pNext = pFirstCapturedError;
            pFirstCapturedError = pCurrent;

            return( TRUE );
        }
        if( dwTmpErrorNumber <= pCapturedError->pNext->ErrorMessage.CGetErrorNumber() )
        {
            break;
        }

        pCapturedError = pCapturedError->pNext;
      
    } while( ( NULL != pCapturedError->pNext ) && ( dwTmpErrorNumber >= pCapturedError->pNext->ErrorMessage.CGetErrorNumber() ) );
        
    pCurrent->pNext = pCapturedError->pNext;
    pCapturedError->pNext = pCurrent;

    return( TRUE );
}
