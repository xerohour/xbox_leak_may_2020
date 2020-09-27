/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineRemoveCachedUser.cpp
 *  Content:    XOnlineRemoveCachedUser tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineRemoveCachedUser
 *
 ****************************************************************************/

#include "XOnlineRemoveCachedUser.h"
#include <XOnlineAuthUtil.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;

/****************************************************************************
 *
 *  XOnlineRemoveCachedUser_VALID_DRIVE_VALID_USER_EXISTS
 *
 *	Description:
 *		Go through each mountable drive (MU and hard disk), 
 *		and try to remove the 1 user that exists on that drive
 *
 *	Input: 
 *		chDrive: 'a' thru 'z' 
 *		pUser: valid user
 *
 *	System State:
 *		User exists on drive: TRUE
 *		User on drive valid: TRUE
 *		Number of users on drive: 1
 *		MU at drive exists: TRUE
 *
 *	Verify: 
 *		Return value is S_OK
 *		User on drive does not exist
 *		0 user on drive
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineRemoveCachedUser_BASIC_TEST( void )
{
    HRESULT             hr              = S_OK;
    DWORD				dwNumUsers		= 0;
    
    XONLINEP_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER aVerifyUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    
    ZeroMemory( aIniUsers, sizeof( aIniUsers ) );
    ZeroMemory( aVerifyUsers, sizeof( aVerifyUsers ) );
    
    // Generate a list of users in memory.
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( aIniUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    CHECKRUN( XOnlineGetUsers( (PXONLINE_USER)aIniUsers, &dwNumUsers ) );
    if (SUCCEEDED(hr))
    {
        hr = (dwNumUsers == XONLINE_MAX_HD_ONLINE_USERS + CountMUAccounts((PXONLINE_USER) aIniUsers)) ? S_OK : E_FAIL;
    }
    
    // Delete 1 user until there are none.
    for ( DWORD i = 0; i < XONLINE_MAX_HD_ONLINE_USERS + CountMUAccounts((PXONLINE_USER) aIniUsers) && SUCCEEDED( hr ); i++ )
    {
        // Only delete HD accounts.
        if (IsHDAccount((XONLINE_USER const *) aIniUsers + i))
        {
            // Delete 1 user from the HD.
            CHECKRUN( _XOnlineRemoveUserFromHD( (PXONLINE_USER)( aIniUsers + i ) ) );
            ZeroMemory(aIniUsers + i, sizeof(XONLINEP_USER));
        }
    }
    
    //
    // check to make sure there are the correct number of user accounts
    //    
    // Get all user accounts.
    CHECKRUN( XOnlineGetUsers( aVerifyUsers, &dwNumUsers ) );
    if ( SUCCEEDED( hr ) )
    {
        // Verify that only MU accounts remain.
        hr = (dwNumUsers == CountMUAccounts(aVerifyUsers)) ? S_OK : E_FAIL;
    }

    return hr;
}

/****************************************************************************
 *
 *  XOnlineRemoveCachedUser_UNDERFLOW_TEST 
 *
 *	Description:
 *		Go through each mountable drive (MU and hard disk), 
 *		and try to remove a user when there are no users on the drive
 *
 *	Input: 
 *		chDrive: 'a' thru 'z' 
 *		pUser: valid user
 *
 *	System State:
 *		User exists on drive: FALSE
 *		User on drive valid: N/A
 *		Number of users on drive: 0
 *		MU at drive exists: TRUE
 *
 *	Verify: 
 *		Return value is not S_OK
 *		User on drive does not exist
 *		0 users on drive
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineRemoveCachedUser_UNDERFLOW_TEST ( void )
{
    HRESULT             hr              = S_OK;
	DWORD				dwNumUsers		= 0;

	XONLINE_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINE_USER aVerifyUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	ZeroMemory( aIniUsers, sizeof( aIniUsers ) );

	// populate the list of users
	CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aIniUsers, XONLINE_MAX_HD_ONLINE_USERS ) );

    // Change the userID to non-existant one.
    aIniUsers[0].xuid.qwUserID ^= (ULONGLONG) 0xba5eba11baadf00d;

    if (SUCCEEDED(hr))
    {
        hr = _XOnlineRemoveUserFromHD( aIniUsers );
        hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;
    }

    return hr;
}

/****************************************************************************
 *
 *  XOnlineRemoveCachedUser_NULL_USER
 *
 *	Description:
 *		Make sure API fails with NULL user pointer
 *
 *	Input: 
 *		chDrive: XONLINE_LOCAL_HARD_DRIVE
 *		pUser: NULL
 *
 *	System State:
 *		User exists on drive: N/A
 *		User on drive valid: N/A
 *		Number of users on drive: N/A
 *		MU at drive exists: N/A
 *
 *	Verify: 
 *		Return value is not S_OK
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineRemoveCachedUser_NULL_USER( void )
{
    HRESULT             hr              = S_OK;

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	CHECKRUN( SUCCEED_ON_FAIL( _XOnlineRemoveUserFromHD( NULL ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    return hr;
}

/****************************************************************************
 *
 *  XOnlineRemoveCachedUser_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineRemoveCachedUser tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineRemoveCachedUser_StartTest( HANDLE LogHandle )
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
    // Test XOnlineRemoveCachedUser with valid drives and valid users exist
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineRemoveCachedUser", "BASIC_TEST" );
    EXECUTE( XOnlineRemoveCachedUser_BASIC_TEST() );

    if (!BVTMode())
    {
        //
        // Test XOnlineRemoveCachedUser with no users on HD
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineRemoveCachedUser", "UNDERFLOW_TEST" );
        EXECUTE( XOnlineRemoveCachedUser_UNDERFLOW_TEST() );
        
        //
        // Test XOnlineRemoveCachedUser with NULL user
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineRemoveCachedUser", "NULL_USER" );
        EXECUTE( XOnlineRemoveCachedUser_NULL_USER() );
    }

	CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlineRemoveCachedUser_EndTest
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

VOID WINAPI XOnlineRemoveCachedUser_EndTest( VOID )
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
    XOnlineRemoveCachedUser_StartTest( NULL );
    XOnlineRemoveCachedUser_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineRemoveCachedUser )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineRemoveCachedUser )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineRemoveCachedUser_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineRemoveCachedUser_EndTest )
END_EXPORT_TABLE( XOnlineRemoveCachedUser )
