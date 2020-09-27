/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    RSTest.cpp

Abstract:

    RSTest tests

--*/

#include "RSTest.h"
#include <regsuite.h>
#include <stdio.h>

#define TEST_PTR	0x40123068

VOID _stdcall /*(*RSINIT_FUNC)*/ParseFunc( LPCSTR szKey, LPCSTR szValue, LPVOID lpUserParam )
{
	if ( TEST_PTR != (INT)lpUserParam )
	{
		DbgPrint( "User param was %x, should be %x\n", lpUserParam, TEST_PTR );
	}
	DbgPrint( "Key = '%s' Value = '%s'\n", szKey, szValue );
	return;
}

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
RSTest_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for RSTest

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;
	DWORD				dwSuccess		= XLL_FAIL;
	UINT64				uiPass, uiFail;
	CHAR				szNotes[1024];

	uiPass = uiFail = 0;

	//MEDIAFILEDESC MediaFiles[6];

	//LoadMedia( MediaFiles );

    //
    // Return the Hresult
    //
	RSTESTPARAMS	TestParams;
	dwSuccess = RSBeginTest( 0, &TestParams, ParseFunc, (LPVOID)TEST_PTR );
	if ( XLL_PASS == dwSuccess )
	{
		uiPass++;
		sprintf( szNotes,
			"D3D_uiBackBufferWidth = %d\\n\
D3D_uiBackBufferHeight = %d\\n\
D3D_BackBufferFormat = %x\\n\
D3D_uiBackBufferCount = %d\\n\
D3D_MultiSampleType = %x\\n\
D3D_SwapEffect = %x\\n\
D3D_bEnableAutoDepthStencil = %d\\n\
D3D_AutoDepthStencilFormat = %x\\n\
D3D_dwFlags = %x\\n\
D3D_uiFullScreen_RefreshRateInHz = %d\\n\
D3D_uiFullScreen_PresentationInterval = %x\\n\
D3D_bSingleStepPusher = %d\\n\
D3D_bD3dParser = %d\\n\
D3D_bNullHardware = %d\\n\
DM_bDoWorkLocally = %d\\n\
DM_dwDoWorkFrequency = %d\\n\
DM_dwDebugLevel = %d\\n\
DM_dwRIPLevel = %d\\n\
DM_dwTestDebugLevel = %d\\n\
DM_dwVoiceCount = %d\\n\
DM_szDMHeap[MAX_PATH] = %s\\n\
DM_dwHeapSizeNorm = %d\\n\
DM_dwHeapSizePhys = %d\\n\
DS_bEnableHeadphones = %d\\n\
DS_szFXImage[MAX_PATH] = %s\\n\
DS_szHRTF[MAX_PATH] = %s\\n\
RS_szTestRun[MAX_PATH] = %s\\n\
RS_szHttpServer[MAX_PATH] = %s\\n\
RS_szConfigPage[MAX_PATH] = %s\\n\
RS_szStatePage[MAX_PATH] = %s\\n\
RS_szLogPage[MAX_PATH] = %s\\n\
RS_szPostFileName[MAX_PATH] = %s\\n\
RS_bLogResults = %d\\n\
RS_dwDecreaseMemory = %d\\n\
RS_dwDSUsedVoices2D = %d\\n\
RS_dwDSUsedVoices3D = %d\\n\
RS_dwMLErrorLevel = %d",
			TestParams.D3D_uiBackBufferWidth,
			TestParams.D3D_uiBackBufferHeight,
			TestParams.D3D_BackBufferFormat,
			TestParams.D3D_uiBackBufferCount,
			TestParams.D3D_MultiSampleType,
			TestParams.D3D_SwapEffect,
			TestParams.D3D_bEnableAutoDepthStencil,
			TestParams.D3D_AutoDepthStencilFormat,
			TestParams.D3D_dwFlags,						
			TestParams.D3D_uiFullScreen_RefreshRateInHz, 
			TestParams.D3D_uiFullScreen_PresentationInterval,
			TestParams.D3D_bSingleStepPusher,
			TestParams.D3D_bD3dParser,
			TestParams.D3D_bNullHardware,
			TestParams.DM_bDoWorkLocally,					
			TestParams.DM_dwDoWorkFrequency,				
			TestParams.DM_dwDebugLevel,					
			TestParams.DM_dwRIPLevel,						
			TestParams.DM_dwTestDebugLevel,				
			TestParams.DM_dwVoiceCount,					
			TestParams.DM_szDMHeap,				
			TestParams.DM_dwHeapSizeNorm,					
			TestParams.DM_dwHeapSizePhys,					
			TestParams.DS_bEnableHeadphones,				
			TestParams.DS_szFXImage,				
			TestParams.DS_szHRTF,
			TestParams.RS_szTestRun,				
			TestParams.RS_szHttpServer,			
			TestParams.RS_szConfigPage,			
			TestParams.RS_szStatePage,			
			TestParams.RS_szLogPage,				
			TestParams.RS_szPostFileName,		
			TestParams.RS_bLogResults,						
			TestParams.RS_dwDecreaseMemory,				
			TestParams.RS_dwDSUsedVoices2D,				
			TestParams.RS_dwDSUsedVoices3D,				
			TestParams.RS_dwMLErrorLevel 
			);
	}
	else
	{
		uiFail++;
		sprintf( szNotes, "Initialization failure" );
	}

	DbgPrint( szNotes );
	RSEndTest( uiPass, /*uiFail*/1, szNotes );
				
    return hr;
}

VOID 
WINAPI 
RSTest_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the RSTest tests

Arguments:

    LogHandle - a handle to a logging object

Return Value:

    None

--*/
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
    // Test RSTest in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "RSTest", "Basic" ) )
    {
        SETLOG( LogHandle, "jeffsul", "Online", "RSTest", "Basic" );
        EXECUTE( RSTest_BasicTest() );
    }

}

VOID 
WINAPI 
RSTest_EndTest( 
    VOID 
    )
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
}

#ifdef NOLOGGING
void 
__cdecl 
main( 
    void 
    )
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    RSTest_StartTest( NULL );
    RSTest_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( RSTest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( RSTest )
    EXPORT_TABLE_ENTRY( "StartTest", RSTest_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", RSTest_EndTest )
END_EXPORT_TABLE( RSTest )
