/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineAddCachedUser.cpp
 *  Content:    XOnlineAddCachedUser tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineAddCachedUser
 *  01/05/02    davidhef Fixed bug in comparison of user data
 *  01/10/02    davidhef Converted to new accounts tool.  Rewrote tests
 *                          to account for API changes.
 *
 ****************************************************************************/

#include "XOnlineAddCachedUser.h"
#include <XOnlineAuthUtil.h>
#include <CXLiveConnectionManager.h>

using XLive::CXLiveConnectionManager;


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
 *  XOnlineAddCachedUser_BASIC_TEST
 *
 *	Description:
 *		Add valid users to HD until it can take no more.
 *
 *	System State:
 *		User exists on drive: FALSE
 *		User on drive valid: N/A
 *		Number of users on drive: 0
 *
 *	Verify: 
 *		Return value is S_OK
 *		Correct number of users exist on HD
 *		User on drive valid
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineAddCachedUser_BASIC_TEST( void )
{
    HRESULT             hr              = S_OK;
    DWORD				dwNumUsers		= 0;
    
    XONLINEP_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER aVerifyUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);

    // zero out the users
    ZeroMemory( aIniUsers, sizeof( aIniUsers ) );
    
    // zero out the verify users
    ZeroMemory( aVerifyUsers, sizeof( aVerifyUsers ) );
    
    // Generate a list of users in memory.
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( aIniUsers, XONLINE_MAX_HD_ONLINE_USERS, true ) );
    
    // Logon with 0 users just to create a connection.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(&dwServices[0], cServices));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection((PXONLINE_USER) &aIniUsers[0]));

    CHECKRUN( DeleteAllUsersFromHD() );

    // Add users until we reach the max allowed.
    for ( DWORD i = 0; i < XONLINE_MAX_HD_ONLINE_USERS && SUCCEEDED( hr ); i++ )
    {
        // add 1 user to this drive
        CHECKRUN( _XOnlineAddUserToHD( (PXONLINE_USER)( aIniUsers + i ) ) );
        
        // Get the users on the drive
        CHECKRUN( XOnlineGetUsers( aVerifyUsers, &dwNumUsers ) );
        
        // check to make sure there are the correct number of users
        if ( SUCCEEDED( hr ) )
        {
            hr = ((i + 1) == dwNumUsers - CountMUAccounts(aVerifyUsers)) ? S_OK : E_FAIL;
        }
    }

    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());

    return hr;
}


/****************************************************************************
 *
 *  XOnlineAddCachedUser_OVERFLOW_TEST
 *
 *	Description:
 *		Go through each mountable drive (mu and hard disk), 
 *		and add a cached user to that drive when it is full
 *
 *	Input: 
 *		chDrive: 'a' thru 'z' 
 *		pUser: valid user
 *
 *	System State:
 *		User exists on drive: FALSE
 *		User on drive valid: N/A
 *		Number of users on drive: MAX_STORED_ONLINE_USERS	
 *		MU at drive exists: TRUE
 *
 *	Verify: 
 *		Return value is not S_OK
 *		Only XONLINE_MAX_STORED_ONLINE_USERS on drive
 *		Users on drive are valid
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineAddCachedUser_OVERFLOW_TEST( void )
{
    HRESULT             hr              = S_OK;
    
    DWORD				dwNumUsers		= 0;
    
    XONLINE_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER aVerifyUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    
    XONLINEP_USER user;
    
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);

    ZeroMemory( &user, sizeof( XONLINE_USER ) );
    
    // fill in the user with some information
    user.xuid.qwUserID = 0x0123456789ABCDEF;
    strcpy( user.name, "dan" );
    strcpy( user.kingdom, "xbox" );
    strcpy( user.domain, "xboxtest.com" );
    strcpy( user.realm, "passport.net" );
    
    for ( BYTE i = 0; i < XONLINE_KEY_LENGTH; i++ )
    {
        user.key[i] = i;
    }
    
    ZeroMemory( aIniUsers, sizeof( aIniUsers ) );
    ZeroMemory( aVerifyUsers, sizeof( aVerifyUsers ) );
    
    // remove all users on the drive, put 8 users on it
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aIniUsers, XONLINE_MAX_HD_ONLINE_USERS, true ) );
    
    // Logon with 0 users just to create a connection.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(&dwServices[0], cServices));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection((PXONLINE_USER) &aIniUsers[0]));

    // add 1 user to this drive
    CHECKRUN( SUCCEED_ON_FAIL( _XOnlineAddUserToHD( (PXONLINE_USER) &user ), E_OUTOFMEMORY ) );
    
    // Get the users on the drive
    CHECKRUN( XOnlineGetUsers( aVerifyUsers, &dwNumUsers ) );

    if ( SUCCEEDED( hr ) )
    {
        hr = (XONLINE_MAX_HD_ONLINE_USERS == dwNumUsers - CountMUAccounts(aVerifyUsers)) ? S_OK : E_FAIL;
    }
    
    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());

    return hr;
}

/****************************************************************************
 *
 *  XOnlineAddCachedUser_NULL_USER
 *
 *	Description:
 *		Make sure API fails with NULL pUser
 *
 *	Input: 
 *		chDrive: XONLINE_LOCAL_HARD_DRIVE, 
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
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineAddCachedUser_NULL_USER( void )
{
    HRESULT             hr              = S_OK;
    XONLINE_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);

    ZeroMemory(aIniUsers, sizeof(aIniUsers));
	
	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

    // Logon with 0 users just to create a connection.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(&dwServices[0], cServices));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection((PXONLINE_USER) &aIniUsers[0]));

	//
	// call AddCachedUser
	//

	CHECKRUN( SUCCEED_ON_FAIL( _XOnlineAddUserToHD( NULL ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());
    
    return hr;
}


/****************************************************************************
 *
 *  XOnlineAddCachedUser_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineAddCachedUser tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineAddCachedUser_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    //
    // Test XOnlineAddCachedUser with valid drives and valid users
    //

    CHECKRUN( StartNetwork() );
    
    SETLOG( LogHandle, "danrose", "Online", "XOnlineAddCachedUser", "BASIC_TEST" );
    EXECUTE( XOnlineAddCachedUser_BASIC_TEST() );
    
    if (!BVTMode())
    {
        //
        // Test XOnlineAddCachedUser when the drive is full
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineAddCachedUser", "OVERFLOW_TEST" );
        EXECUTE( XOnlineAddCachedUser_OVERFLOW_TEST() );
        
        //
        // Test XOnlineAddCachedUser with a NULL user
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineAddCachedUser", "NULL_USER" );
        EXECUTE( XOnlineAddCachedUser_NULL_USER() );
    }
    
    CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlineAddCachedUser_EndTest
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

VOID WINAPI XOnlineAddCachedUser_EndTest( VOID )
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
    XOnlineAddCachedUser_StartTest( NULL );
    XOnlineAddCachedUser_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineAddCachedUser )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineAddCachedUser )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineAddCachedUser_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineAddCachedUser_EndTest )
END_EXPORT_TABLE( XOnlineAddCachedUser )
