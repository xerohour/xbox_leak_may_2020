/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineCreateMachineAccount.cpp
 *  Content:    XOnlineCreateMachineAccount tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/22/01    danrose Created to test Xbox XOnlineCreateMachineAccount
 *
 ****************************************************************************/

#include "XOnlineCreateMachineAccount.h"

//@@@ drm: XOnlineCreateMachineAccount no longer exists
#define XOnlineCreateMachineAccount() (E_FAIL)

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_ValidAccountNoInternet
 *
 *	Description:
 *		Try to create a machine account on an xbox that has a 
 *		valid machine account and is not connected to the internet
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: TRUE
 *		Account valid: TRUE
 *		Connected to internet: FALSE
 *
 *	Verify: 
 *		Return code is NOT S_OK
 *		Old account exists
 *		Old account is not modified
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineCreateMachineAccount_ValidAccountNoInternet( void )
{
    HRESULT             hr              = S_OK;

	//
	// create a temp devkit machine acount
	//

	CHECKRUN( CreateMachineAccount() );

	//
	// Create the machine Account
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineCreateMachineAccount(), 0xc000005e ) );

	//
	// verify a machine account exists
	//

	CHECKRUNBOOL( _XOnlineHasMachineAccount() );

	//
	// verify the account that exists is the old account
	//

	CHECKRUNBOOL( VerifyMachineAccount( 1, "SN999999999999@machines.xbox.com", "PASSWORD" ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_ValidAccountHasInternet
 *
 *	Description:
 *		Try to create a machine account on an xbox that has a 
 *		valid machine account and is connected to the internet
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: TRUE
 *		Account valid: TRUE
 *		Connected to internet: TRUE
 *
 *	Verify: 
 *		Return code is NOT S_OK
 *		Old account exists
 *		Old account is not modified
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineCreateMachineAccount_ValidAccountHasInternet( void )
{
    HRESULT             hr              = S_OK;

	//
	// start the network
	//

	CHECKRUN( StartNetwork() );

	//
	// create a temp devkit machine acount
	//

	CHECKRUN( CreateMachineAccount() );

	//
	// Create the machine Account
	//

	CHECKRUN( XOnlineCreateMachineAccount() );

	//
	// verify a machine account exists
	//

	CHECKRUNBOOL( XOnlineHasMachineAccount() );

	//
	// verify the account that exists is the old account
	//

	CHECKRUNBOOL( !VerifyMachineAccount( 1, "SN999999999999@machines.xbox.com", "PASSWORD" ) );

	//
	// close the network
	//

	CloseNetwork();

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_NoAccountNoInternet
 *
 *	Description: 
 *		Create a machine account on an xbox that has no 
 *		machine account and is not connected to the internet
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: FALSE
 *		Account valid: N/A
 *		Connected to internet: FALSE
 *
 *	Verify: 
 *		Return code is NOT S_OK
 *		No account exists
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineCreateMachineAccount_NoAccountNoInternet( void )
{
    HRESULT             hr              = S_OK;

	//
	// Erase the machine acount
	//

	CHECKRUN( RemoveMachineAccount() );

	//
	// Create the machine Account
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineCreateMachineAccount(), 0xc000005e ) );

	//
	// verify a machine account exists
	//

	CHECKRUNBOOL( ! XOnlineHasMachineAccount() );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_NoAccountHasInternet
 *
 *  Description:
 *		Create a machine account on an xbox that has no machine account 
 *		and is connected to the internet
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: FALSE
 *		Account valid: N/A
 *		Connected to internet: TRUE
 *
 *	Verify: 
 *		Return code is S_OK
 *		Valid account exists
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineCreateMachineAccount_NoAccountHasInternet( void )
{
    HRESULT             hr              = S_OK;

	//
	// start the network
	//

	CHECKRUN( StartNetwork() );

	//
	// Erase the machine acount
	//

	CHECKRUN( RemoveMachineAccount() );

	//
	// Create the machine Account
	//

	CHECKRUN( XOnlineCreateMachineAccount() );

	//
	// verify a machine account exists
	//

	CHECKRUNBOOL( XOnlineHasMachineAccount() );

	//
	// close the network
	//

	CloseNetwork();

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineCreateMachineAccount tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineCreateMachineAccount_StartTest( HANDLE LogHandle )
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
    // Test XOnlineCreateMachineAccount without an account, with net
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineCreateMachineAccount", "NoAccountHasInternet" );
    EXECUTE( XOnlineCreateMachineAccount_NoAccountHasInternet() );

	//
	// Test XOnlineCreateMachineAccount without an account, no net
	//

    SETLOG( LogHandle, "danrose", "Online", "XOnlineCreateMachineAccount", "NoAccountNoInternet" );
    EXECUTE( XOnlineCreateMachineAccount_NoAccountNoInternet() );

	//
	// Test XOnlineCreateMachineAccount with an account, no net
	//

    SETLOG( LogHandle, "danrose", "Online", "XOnlineCreateMachineAccount", "ValidAccountHasInternet" );
    EXECUTE( XOnlineCreateMachineAccount_ValidAccountHasInternet() );

	//
	// Test XOnlineCreateMachineAccount with a valid account, but no internet
	//

    SETLOG( LogHandle, "danrose", "Online", "XOnlineCreateMachineAccount", "ValidAccountNoInternet" );
    EXECUTE( XOnlineCreateMachineAccount_ValidAccountNoInternet() );

}

/****************************************************************************
 *
 *  XOnlineCreateMachineAccount_EndTest
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

VOID WINAPI XOnlineCreateMachineAccount_EndTest( VOID )
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
    XOnlineCreateMachineAccount_StartTest( NULL );
    XOnlineCreateMachineAccount_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineCreateMachineAccount )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineCreateMachineAccount )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineCreateMachineAccount_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineCreateMachineAccount_EndTest )
END_EXPORT_TABLE( XOnlineCreateMachineAccount )
