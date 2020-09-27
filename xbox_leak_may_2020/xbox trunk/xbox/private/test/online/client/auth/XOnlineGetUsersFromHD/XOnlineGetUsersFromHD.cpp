/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineGetUsersFromHD.cpp
 *  Content:    XOnlineGetUsersFromHD tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineGetUsersFromHD
 *
 ****************************************************************************/

#include "XOnlineGetUsersFromHD.h"
#include <XOnlineAuthUtil.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 * The Global list of drives
 *
 ****************************************************************************/

static const CHAR DRIVES[] = { 
	0, 
	'F', 
	'G', 
	'H', 
	'I', 
	'J', 
	'K', 
	'L', 
	'M' 
};

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;

/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_VALID_COUNT 
 *
	Description:
		Go through each mountable drive (MU and hard disk), 
		and retrieve each user profile. Do this for every 
		possible number of users per device

	Input: 
		chDrive: 'a' thru 'z'
		pUsers: valid pointer
		pcUsers: valid pointer

	System State:
		Users exist on drive: TRUE
		Users on drive valid: TRUE 
		Number of users on drive: 0 thru MAX_STORED_USERS
		MU at drive exists: TRUE

	Verify: 
		Return value is S_OK
		Count of users is correct
		user data is valid

 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_VALID_COUNT ( void )
{
    HRESULT             hr              = S_OK;
	DWORD				dwNumUsers		= 0;

	XONLINEP_USER aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER users[XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// cycle through each drive
	//

	for ( DWORD i = 0; i < NUMELEMS( DRIVES ) && SUCCEEDED( hr ); i++ )
	{

		//
		// cycle through N users
		//

		for ( DWORD j = 0; j <= XONLINE_MAX_STORED_ONLINE_USERS && SUCCEEDED( hr ); j++ )
		{

			//
			// zero out the users
			//

			ZeroMemory( aUsers, sizeof( aUsers ) );

			//
			// remove all users on the drive
			//

			CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( aUsers, j ) );

			//
			// if this drive is not mounted, then goto next drive
			//

			if ( 0x80070002 == hr )
			{
				hr = S_OK;
				break;
			}

			//
			// Get the users on the drive
			//

			CHECKRUN( XOnlineGetUsers( users, &dwNumUsers ) );
		
			//
			// check to make sure there is only j user
			//

			if ( SUCCEEDED( hr ) )
			{
				hr = j == dwNumUsers ? S_OK : E_FAIL;
			}
		}
	}

    //
    // Return the Hresult
    //

    return hr;
}


/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_NULL_DRIVE_VALID_USER_VALID_COUNT
 *
	Description:
		Make sure API will fail correctly when a bogus drive value is passed in

	Input: 
		chDrive: '\255'
		pUsers: valid pointer
		pcUsers: valid pointer

	System State:
		Users exist on drive: N/A
		Users on drive valid: N/A
		Number of users on drive: N/A
		MU at drive exists: FALSE

	Verify: 
		Return value is not S_OK

 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetUsersFromHD_NULL_DRIVE_VALID_USER_VALID_COUNT( void )
{
    HRESULT             hr              = S_OK;
	DWORD				dwNumUsers		= 0;
	XONLINE_USER aUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_NULL_COUNT
 *
	Description:
		Make sure API will fail correctly when a NULL user pointer is passed in

	Input: 
		chDrive: 'c'
		pUsers: valid pointer
		pcUsers: NULL

	System State:
		Users exist on drive: TRUE
		Users on drive valid: TRUE
		Number of users on drive: 1
		MU at drive exists: TRUE

	Verify: 
			Return value is not S_OK

 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_NULL_COUNT( void )
{
    HRESULT             hr              = S_OK;

	XONLINE_USER aUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// call add cached user with junk
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineGetUsers( aUsers, NULL ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_VALID_DRIVE_NULL_USER_VALID_COUNT
 *
	Description:
		Make sure API will fail correctly when a NULL user pointer is passed in

	Input: 
		chDrive: 'c'
		pUsers: NULL
		pcUsers: valid pointer

	System State:
		Users exist on drive: TRUE
		Users on drive valid: TRUE
		Number of users on drive: 1
		MU at drive exists: TRUE

	Verify: 
			Return value is not S_OK

 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetUsersFromHD_VALID_DRIVE_NULL_USER_VALID_COUNT( void )
{
    HRESULT             hr              = S_OK;
	DWORD				dwNumUsers		= 0;

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// call add cached user with junk
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineGetUsers( NULL, &dwNumUsers ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineGetUsersFromHD tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetUsersFromHD_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	CHECKRUN( StartNetwork() );

    //
    // Test XOnlineGetUsersFromHD with a valid drive, a valid user and a valid count
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineGetUsers", "VALID_DRIVE_VALID_USER_VALID_COUNT" );
    EXECUTE( XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_VALID_COUNT() );

    //
    // Test XOnlineGetUsersFromHD with a NULL drive, a valid user and a valid count
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineGetUsers", "NULL_DRIVE_VALID_USER_VALID_COUNT" );
    EXECUTE( XOnlineGetUsersFromHD_NULL_DRIVE_VALID_USER_VALID_COUNT() );

    //
    // Test XOnlineGetUsersFromHD with a NULL drive, a valid user and a valid count
    //
#if 0       // Remove pending fix for RAID #10575
    SETLOG( LogHandle, "danrose", "Online", "XOnlineGetUsers", "VALID_DRIVE_VALID_USER_NULL_COUNT" );
    EXECUTE( XOnlineGetUsersFromHD_VALID_DRIVE_VALID_USER_NULL_COUNT() );

    //
    // Test XOnlineGetUsersFromHD with a NULL drive, a valid user and a valid count
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineGetUsers", "VALID_DRIVE_NULL_USER_VALID_COUNT" );
    EXECUTE( XOnlineGetUsersFromHD_VALID_DRIVE_NULL_USER_VALID_COUNT() );
#endif
	CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlineGetUsersFromHD_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetUsersFromHD_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
    XOnlineGetUsersFromHD_StartTest( NULL );
    XOnlineGetUsersFromHD_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineGetUsersFromHD )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineGetUsersFromHD )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineGetUsersFromHD_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineGetUsersFromHD_EndTest )
END_EXPORT_TABLE( XOnlineGetUsersFromHD )
