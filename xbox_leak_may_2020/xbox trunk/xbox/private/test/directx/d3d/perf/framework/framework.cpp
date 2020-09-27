/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       framework.cpp
 *  Content:    framework tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/22/01    jeffsul Created to test Xbox performance with framework
 *
 ****************************************************************************/

#include "framework.h"

#include "Benmark5.h"
#include "Fillrate.h"
#include "FSAA.h"
//#include "Perf.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  framework_BasicTest
 *
 *  Description:
 *      Basic Test for framework
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT framework_BasicTest( void )
{
    HRESULT             hr              =	S_OK;

	BOOL				bRunBenMark, 
						bRunFillRate, 
						bRunFsaa;

	bRunBenMark			=	GetPrivateProfileIntA( FRAMEWORK_APP_NAME, "RunBenMark",	FALSE, FRAMEWORK_INI_PATH );
	bRunFillRate		=	GetPrivateProfileIntA( FRAMEWORK_APP_NAME, "RunFillRate",	FALSE, FRAMEWORK_INI_PATH );
	bRunFsaa			=	GetPrivateProfileIntA( FRAMEWORK_APP_NAME, "RunFsaa",		FALSE, FRAMEWORK_INI_PATH );
	//bRunPerf			=	GetPrivateProfileIntA( FRAMEWORK_APP_NAME, "RunPerf",		FALSE, FRAMEWORK_INI_PATH );

	if ( bRunBenMark )
	{
		CBenMark BenMark;
		BenMark.Run();
	}

	if ( bRunFsaa )
	{
		CFsaa Fsaa;
		Fsaa.Run();
	}

	/*if ( bRunPerf )
	{
		CPerf Perf;
		Perf.Run();
	}*/

	if ( bRunFillRate )
	{
		CFillRate FillRate;
		FillRate.Run();
	}

	XLaunchNewImage( NULL, NULL );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  framework_StartTest
 *
 *  Description:
 *      The Harness Entry into the framework tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI framework_StartTest( HANDLE LogHandle )
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
    // Test framework in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "framework", "Basic" );
    EXECUTE( framework_BasicTest() );

}

/****************************************************************************
 *
 *  framework_EndTest
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

VOID WINAPI framework_EndTest( VOID )
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
    framework_StartTest( NULL );
    framework_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( framework )
#pragma data_seg()

BEGIN_EXPORT_TABLE( framework )
    EXPORT_TABLE_ENTRY( "StartTest", framework_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", framework_EndTest )
END_EXPORT_TABLE( framework )
