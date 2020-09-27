/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	capi.c

Abstract:

	capi tests - Created to test Xbox capi

Author:

	Jeff Sullivan (jeffsul) 26-Jun-2001

[Notes:]

	The app just draws a triangle to screen to make sure everything runs properly

Revision History:

	26-Jun-2001 jeffsul
		Initial Version

--*/

#include "capi.h"
#include "capiTests.h"
#include <stdio.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;


//------------------------------------------------------------------------------
//	Global Variables
//------------------------------------------------------------------------------

CAPI capiTest;


/*++

	InitD3D

Routine Description:

	initialize direct3d settings

Arguments:

	hWnd - a handle to the window being created (NULL for Xbox)

Return Value:

	S_OK on success
    E_XX on failure

--*/

HRESULT InitD3D( HWND hWnd )
{
	HRESULT hr = S_OK;

    D3DPRESENT_PARAMETERS d3dpp;
    DWORD startTime;
    DWORD initializationTime;
    IDirect3D8 *pD3D;
    CHAR stringBuffer[200];
     
    startTime = timeGetTime();

    // Create D3D 8.
    pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
	{
        return E_FAIL;
	}

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = hWnd;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the device.

	EXECUTECHECK( IDirect3D8_CreateDevice(	pD3D, 
											D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_HAL, 
											hWnd, 
											D3DCREATE_HARDWARE_VERTEXPROCESSING, 
											&d3dpp, 
											&capiTest.pDev ) );
 

    // Now we no longer need the D3D interface so lets free it.
    IDirect3D8_Release(pD3D);

    IDirect3DDevice8_BlockUntilIdle(capiTest.pDev);

    initializationTime = timeGetTime() - startTime;

    sprintf(stringBuffer, "capi: Total D3D initialization time: %lims\n", initializationTime);

    OutputDebugStringA(stringBuffer);

    return S_OK;
}


/*++

	Paint

Routine Description:

	Draws a triangle to screen

Arguments:

	None

Return Value:

	None

--*/

void Paint()
{
	HRESULT hr = S_OK;

    FLOAT data0[4] = { 0, 1, 2, 3 };
    FLOAT data95[4] = { 95, 95, 95, 95 };
    FLOAT garbage[4] = { 34, 1265, 12, 1235 };

	// Define our screen space triangle.
	static struct { float x,y,z,w; DWORD color; } Verts[] =
	{
		{320.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, },
		{620.0f, 430.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 20.0f, 430.0f, 0.5f, 1.0f, 0xff0000ff, },
	};

    // Clear the frame buffer, Zbuffer.
    EXECUTE( IDirect3DDevice8_Clear(capiTest.pDev, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 58, 110, 165 ), 0.0f, 0) );

    EXECUTE( IDirect3DDevice8_SetVertexShader(capiTest.pDev, CUSTOMVERTEX_FVF) );

    EXECUTE( IDirect3DDevice8_SetRenderState(capiTest.pDev, D3DRS_CULLMODE, D3DCULL_NONE) );

    EXECUTE( IDirect3DDevice8_DrawVerticesUP(capiTest.pDev, 
                                             D3DPT_TRIANGLELIST,
                                             3,                    // VertexCount
                                             Verts,                // pVertexStreamZeroData
                                             sizeof(Verts[0])) );  // VertexZeroStreamStride
    // Flip the buffers.

    EXECUTE( IDirect3DDevice8_Present(capiTest.pDev, NULL, NULL, NULL, NULL) );
}


/****************************************************************************
 *
 *  capi_BasicTest
 *
 *  Description:
 *      Basic Test for capi
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT capi_BasicTest( void )
{
    HRESULT hr = S_OK;

	BOOL Toggle = FALSE;
    DWORD Time = GetTickCount();

	capiTest.bQuit = FALSE;
	capiTest.pDev = NULL;

	// make sure that all C API function calls will at least compile, then maybe we'll test running them
	EXECUTE( RunTests() );

	EXECUTECHECK( InitD3D( NULL ) );

    while ( !capiTest.bQuit )
    {
        Paint();
    
        if (GetTickCount() - Time > 2000)
        {
            Toggle = !Toggle;
            Time = GetTickCount();

            D3DDevice_PersistDisplay();
        }
    }

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  capi_StartTest
 *
 *  Description:
 *      The Harness Entry into the capi tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI capi_StartTest( HANDLE LogHandle )
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
    // Test capi in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "capi", "Basic" );
    EXECUTE( capi_BasicTest() );

}

/****************************************************************************
 *
 *  capi_EndTest
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

VOID WINAPI capi_EndTest( VOID )
{
	INT ref_count = 0;

	if ( capiTest.pDev != NULL )
	{
		ref_count = IDirect3DDevice8_Release( capiTest.pDev );
		if( ref_count != 0 )
		{
			DbgPrint( "Wrong ref count for capiTest.pDev: %d\n", ref_count );
		}
	}
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
    capi_StartTest( NULL );
    capi_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( capi )
#pragma data_seg()

BEGIN_EXPORT_TABLE( capi )
    EXPORT_TABLE_ENTRY( "StartTest", capi_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", capi_EndTest )
END_EXPORT_TABLE( capi )
