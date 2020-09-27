/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       leak.cpp
 *  Content:    leak tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   8/28/01    jgould Created to test Xbox leak
 *
 ****************************************************************************/

#include "leak.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  leak_BasicTest
 *
 *  Description:
 *      Basic Test for leak
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT leak_BasicTest( void )
{
    HRESULT             hr              = S_OK;

	MM_STATISTICS mmStats1 = {sizeof(MM_STATISTICS), 0, 0, 0, 0, 0};
	MM_STATISTICS mmStats2 = {sizeof(MM_STATISTICS), 0, 0, 0, 0, 0};

	int i;
	DWORD LastNumPages = 0;

	//repeat 1026 times.
	//every 128, pause a second, check memory.
	//(the first two times through the loop, don't check, so cache & stuff gets stablized)

	for(i = 0; i <= 0x4002; i++) {
		IDirect3D8 * pD3D;
		D3DDevice  * pDev;
		D3DPRESENT_PARAMETERS d3dpp;

	    pD3D = Direct3DCreate8(D3D_SDK_VERSION);
		if (pD3D == NULL)
			return false;

		ZeroMemory(&d3dpp, sizeof(d3dpp));

		d3dpp.BackBufferWidth           = 640;
		d3dpp.BackBufferHeight          = 480;
		d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount           = 1;
		d3dpp.Windowed                  = false;   // Must be false for Xbox.
		d3dpp.EnableAutoDepthStencil    = true;
		d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
		d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
		d3dpp.FullScreen_RefreshRateInHz= 60;
		d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

		if (pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			NULL,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp,
			&pDev) != S_OK)
		{
			return false;
		}

		pD3D->Release();

		pDev->Release();

		if((i & 127) == 2) {
			Sleep (1000);
			if(i == 2) {
				MmQueryStatistics(&mmStats1);
				LastNumPages = mmStats1.AvailablePages;
			} else {
				MmQueryStatistics(&mmStats2);
				xLog(g_hLog, LastNumPages > mmStats2.AvailablePages ? XLL_FAIL : XLL_PASS, "Pass %d:  %d pages free", (i >> 7), mmStats2.AvailablePages);
			}
		}
	}

	xLog(g_hLog, XLL_INFO, "After %d runs, %d pages leaked == %d bytes/device", i - 3, mmStats1.AvailablePages - mmStats2.AvailablePages, (float)4096*(float)(mmStats1.AvailablePages - mmStats2.AvailablePages) / (float)(i-3));

	if(mmStats1.AvailablePages > mmStats2.AvailablePages) {
		return E_FAIL;
	}

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  leak_StartTest
 *
 *  Description:
 *      The Harness Entry into the leak tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI leak_StartTest( HANDLE LogHandle )
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
    // Test leak in the mannor it was meant to be called
    //

   xSetOwnerAlias( LogHandle, "jgould" );
   xSetComponent( LogHandle, "Direct3D(S)", "Core (s-d3d)");
   xSetFunctionName( LogHandle, "D3DLeak" );
   xStartVariation( LogHandle, "D3DLeak" );

    EXECUTE( leak_BasicTest() );

	xEndVariation( LogHandle );

}

/****************************************************************************
 *
 *  leak_EndTest
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

VOID WINAPI leak_EndTest( VOID )
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

HANDLE g_hLog;

void __cdecl main( void )
{
	WEB_SERVER_INFO wsi = {L"", L"", L"", L"", L"", };
	g_hLog = xCreateLog(L"t:\\d3dleak.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_DEBUG | XLO_CONFIG | XLO_STATE | XLO_REFRESH);
	DebugPrint("D3DLeak's log: %p\n", g_hLog);

    leak_StartTest( g_hLog );
    leak_EndTest();

	xCloseLog(g_hLog);
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( leak )
#pragma data_seg()

BEGIN_EXPORT_TABLE( leak )
    EXPORT_TABLE_ENTRY( "StartTest", leak_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", leak_EndTest )
END_EXPORT_TABLE( leak )
