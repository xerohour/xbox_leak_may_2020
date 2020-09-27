///////////////////////////////////////////////////////////////////////////////
//
// Name: GenAccounts.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: code to generate account names from user names and kingdoms
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//
// Name: Usage
//
// Input: None
//
// Output: None
//
// Description: describes how to use the app
//
///////////////////////////////////////////////////////////////////////////////

void Usage( void )
{
	fprintf( stderr, "Usage:   GenAccounts <NAME_FILE> <KINGDOM_FILE>\r\n" );
	fprintf( stderr, "Example: GenAccounts names.txt kingdoms.txt\r\n ");
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: main
//
// Input: argc - the number of args
//        argv - the args
//
// Output: an error code. 0 for success, -XXXXX for failure
//
// Description: actualy does all the work
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{
	//
	// the file pointers
	//

	FILE* pNameFile = NULL;
	FILE* pKingdomFile = NULL;

	//
	// the strings
	//

	char szName[64];
	char szKingdom[64];
	char szDomain[64];
	char* szNew = NULL;

	//
	// integer return values
	//

	int iVal = 0;
	int iRetKingdom = 0;
	int iRetName = 0;

	//
	// count of names and kingdoms
	//

	unsigned int iNameCount = 0;
	unsigned int iKingdomCount = 0;

	//
	// decide if there should be a passcode
	//

	bool bPassCode = true;

	//
	// must have 3 args
	//

	if ( 3 != argc )
	{
		Usage();
		iVal = -1;
	}

	//
	// open the name file
	//

	if ( 0 == iVal )
	{
		pNameFile = fopen( argv[1], "rb" );

		//
		// if hte file could not be opened, say that
		//

		if ( NULL == pNameFile )
		{
			fprintf( stderr, "Could not open Name file: %s\r\n", argv[1] );
			Usage();

			iVal = -2;
		}
	}

	//
	// open the kingdom file
	//

	if ( 0 == iVal )
	{
		pKingdomFile = fopen( argv[2], "rb" );

		//
		// if the file could not be opened, say that
		//

		if ( NULL == pKingdomFile )
		{
			fprintf( stderr, "Could not open Kingdom file: %s\r\n", argv[2] );
			Usage();

			iVal = -3;
		}
	}

	//
	// turn echo off...
	//

	if ( 0 == iVal )
	{
		printf( "@echo off\r\n" );
	}

	//
	// cycle through each kingdom
	//

	while ( EOF != iRetKingdom && 0 == iVal )
	{
		//
		// reset the kingdom string
		//

		memset( szKingdom, 0, sizeof( szKingdom ) );

		//
		// read tje kingdom string from the file
		//

		iRetKingdom = fscanf( pKingdomFile, "%s", szKingdom );

		if ( 0 == iRetKingdom )
		{
			iVal = -4;
		}
		else
		{
			//
			// incrmenet the kingdom count
			//

			iKingdomCount++;
		}

		//
		// cycle through the users
		//

		while ( EOF != iRetName && 0 == iVal )
		{
			//
			// reset the name
			//

			memset( szName, 0, sizeof( szName ) );

			//
			// read the next name
			//

			iRetName = fscanf( pNameFile, "%s", szName );

			if ( 0 == iRetName )
			{
				iVal = -5;
			}
			else
			{
				//
				// find where the domain name begins
				//

				szNew = strpbrk( szName, "@" );

				if ( NULL == szNew )
				{
					iVal = -6;
				}
				else
				{
					//
					// copy the domain
					//

					strcpy( szDomain, szNew );

					//
					// replace the domain with .kingdom@domain
					//

					sprintf( szNew, "_%s%s", szKingdom, szDomain );

					//
					// print the new user name
					//

					if ( bPassCode )
					{
						printf( "AccCmdTool %s 00000001,04020406\r\n", szName );
					}
					else
					{
						printf( "AccCmdTool %s 00000000,00000000\r\n", szName );
					}

					iNameCount++;
				}
			}

			//
			// we want to do this in groups of 4 users
			//

			if ( 4 == iNameCount )
			{
				bPassCode = ! bPassCode;
				iNameCount = 0;
				break;
			}
		}
	}

	//
	// close the name file
	//

	if ( pNameFile )
	{
		fclose( pNameFile );
	}

	//
	// close the kingdom file
	//

	if ( pKingdomFile )
	{
		fclose( pKingdomFile );
	}

	return iVal;
}