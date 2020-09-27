/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineHasMachineAccount.cpp
 *  Content:    XOnlineHasMachineAccount tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/23/01    danrose Created to test Xbox XOnlineHasMachineAccount
 *
 ****************************************************************************/

#include "XOnlineHasMachineAccount.h"

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
 *  XOnlineHasMachineAccount_NoAccount
 *
 *	Description:
 *		Check to see if this machine has a machine account when it does not
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: FALSE
 *		Account valid: N/A
 *
 *	Verify: 
 *		Return value is FALSE
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineHasMachineAccount_NoAccount( void )
{
    HRESULT             hr              = S_OK;

	//
	// Erase the machine acount
	//

	CHECKRUN( RemoveMachineAccount() );

	//
	// check the machine account
	//

	CHECKRUNBOOL( ! XOnlineHasMachineAccount() );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineHasMachineAccount_ValidAccount
 *
 *	Description:
 *		Check to see if this machine has a machine account when it has a valid account
 *
 *	Input: 
 *		None
 *
 *	System State:
 *		Account exists: TRUE
 *		Account valid: TRUE
 *
 *	Verify: 
 *		Return value is TRUE
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineHasMachineAccount_ValidAccount( void )
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
	// Create a new machine account
	//

	CHECKRUN( XOnlineCreateMachineAccount() );

	//
	// check the machine account
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
 *  XOnlineHasMachineAccount_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineHasMachineAccount tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineHasMachineAccount_StartTest( HANDLE LogHandle )
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
    // Test XOnlineHasMachineAccount when no account exists
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineHasMachineAccount", "NoAccount" );
    EXECUTE( XOnlineHasMachineAccount_NoAccount() );

    //
    // Test XOnlineHasMachineAccount when an account exists
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineHasMachineAccount", "ValidAccount" );
    EXECUTE( XOnlineHasMachineAccount_ValidAccount() );

}

/****************************************************************************
 *
 *  XOnlineHasMachineAccount_EndTest
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

VOID WINAPI XOnlineHasMachineAccount_EndTest( VOID )
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
    XOnlineHasMachineAccount_StartTest( NULL );
    XOnlineHasMachineAccount_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineHasMachineAccount )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineHasMachineAccount )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineHasMachineAccount_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineHasMachineAccount_EndTest )
END_EXPORT_TABLE( XOnlineHasMachineAccount )
