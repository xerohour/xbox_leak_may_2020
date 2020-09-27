//
//  LockDepth.cpp
//
//  D3DFMT_D16_LOCKABLE Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "LockDepth.h"

// ISSUE:  Why does FUDGE_FACTOR have to be 0.125 for the test to execute properly?
//         What FUDGE_FACTOR represents is a downward shift of 1/8 of one pixel in
//         our sampling location, as we "predict" what the z-buffer value *should* be.
#define FUDGE_FACTOR 0.125f

struct TEX2DVERTEX { D3DXVECTOR4 p;   DWORD color; };

#define D3DFVF_TEX2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

#define MAX_VERTICES 32
#define PALETTE_IDENTIFIER 3

inline TEX2DVERTEX InitTex2DVertex( const D3DXVECTOR4& p, D3DCOLOR color )
{
    TEX2DVERTEX v;   v.p = p;   v.color = color;
    return v;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    LockDepth*  pLockDepth;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pLockDepth = new LockDepth();
    if (!pLockDepth) {
        return FALSE;
    }

    // Initialize the scene
    if (!pLockDepth->Create(pDisplay)) {
        pLockDepth->Release();
        return FALSE;
    }

    bRet = pLockDepth->Exhibit(pnExitCode);

    // Clean up the scene
    pLockDepth->Release();

    return bRet;
}

//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

LockDepth::LockDepth()
{
    m_szTestName = _T("LockDepth");
    m_szCommandKey = _T("LockDepth");

    m_ModeOptions.uMinDXVersion = 0x800;
    m_ModeOptions.uMaxDXVersion = 0x800;

    m_ModeOptions.fReference = false;
    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;

    m_ModeOptions.bSWDevices = false;
    m_ModeOptions.bHWDevices = true;
	m_ModeOptions.dwDeviceTypes = D3DDEVTYPE_HAL;//DEVICETYPE_HAL;

    m_pDevice = NULL;
    m_pD3D = NULL;

	m_pSrcVB = NULL;

	INT i;
	ReadInteger( _T("TOL"), 1, &i );

	if( i>= 0 )
		m_wTolerance = i<32768 ? LOWORD(i) : 32767;
	else
		m_wTolerance = (WORD)0;

}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT LockDepth::TestInitialize()
{
    UINT uResult = CD3DTest::TestInitialize();

    if(uResult == D3DTESTINIT_RUN)
    {
//		m_pDevice = m_pSrcDevice8;
		m_pD3D =  m_pD3D8;

        // set the test range
        SetTestRange(1, 64);


        // log everything
//        g_pD3DFramework->m_pLog->SetOption( MAXFAILS, 0 );


        // set window title
        TCHAR szText[MAX_PATH];

//        ConstructTestCaption(szText);
//        SetWindowText( g_pD3DFramework->m_hWindow, szText );

		m_pSrcVB = CreateVertexBuffer( m_pDevice, NULL, MAX_VERTICES * sizeof(TEX2DVERTEX), 0, D3DFVF_TEX2DVERTEX );
        if (!m_pSrcVB) {
            return D3DTESTINIT_ABORT;
        }

		OneTimeInit( );

    }

    return uResult;
}

bool LockDepth::TestTerminate()
{

//	RELEASE( m_pSrcVB );
    ReleaseVertexBuffer(m_pSrcVB);

	return CD3DTest::TestTerminate( );
}

//-----------------------------------------------------------------------------

bool
LockDepth::ExecuteTest
(
    UINT nTest
)
{
	D3DDEVICE_CREATION_PARAMETERS Parameters;
	D3DSURFACE_DESC Desc;
    D3DFORMAT fmtd;

#ifndef UNDER_XBOX
    fmtd = D3DFMT_D16_LOCKABLE;
#else
    fmtd = D3DFMT_LIN_D16;
#endif

	BOOL fFail;

    HRESULT hr=D3D_OK;

    ClearFrame();

#ifndef UNDER_XBOX
	if( FAILED (m_pSrcDevice8->GetCreationParameters( &Parameters ) ) )
		return (FALSE);
#else
    Parameters.AdapterOrdinal = 0;
    Parameters.DeviceType = D3DDEVTYPE_HAL;
#endif

	CSurface8*	pSrcSurface=NULL;
	if( FAILED( m_pSrcDevice8->GetRenderTarget( &pSrcSurface ) ) )
		return (FALSE);
	
	if( pSrcSurface )
	{
		pSrcSurface->GetDesc( &Desc	);
		RELEASE( pSrcSurface );
	}
	

    BeginTestCase( TEXT("D3DFMT_D16_LOCKABLE"), nTest );

	fFail = FALSE;

	if( FAILED( m_pD3D->CheckDeviceFormat(	Parameters.AdapterOrdinal,
											Parameters.DeviceType,
											Desc.Format,
											D3DUSAGE_DEPTHSTENCIL,
											D3DRTYPE_SURFACE,
											fmtd ) ) )
	{
		WriteToLog(LOG_ABORT, 
					TEXT("CheckDeviceFormat() disallowed D16_LOCKABLE.\n")
					);
//		g_pD3DFramework->m_pLog->SetResult(ABORT);
		EndTestCase();
		return(FALSE);
	}

	if( FAILED( m_pD3D->CheckDepthStencilMatch(	Parameters.AdapterOrdinal,
												Parameters.DeviceType,
												Desc.Format,
												Desc.Format,
												fmtd ) ) )
	{
		WriteToLog(LOG_ABORT, 
					TEXT("CheckDepthStencilMatch() disallowed D16_LOCKABLE.\n")
					);
//		g_pD3DFramework->m_pLog->SetResult(ABORT);
		EndTestCase();
		return(FALSE);
	}

	CSurface8* pSrcDepthSurface=NULL;
	hr = m_pSrcDevice8->CreateDepthStencilSurface(
												  Desc.Width,
												  Desc.Height,
												  fmtd,
												  Desc.MultiSampleType,
												  &pSrcDepthSurface
												);

	
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("CreateDepthStencilSurface Failed.\n")
					);
		Fail();
		EndTestCase();
		return(FALSE);
	}
	
	hr = m_pSrcDevice8->SetRenderTarget( NULL, pSrcDepthSurface );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("SetRenderTarget Failed.\n")
					);
		Fail();
		EndTestCase();
		return(FALSE);
	}

	// golden
	D3DLOCKED_RECT lrc;
	
	hr = pSrcDepthSurface->LockRect( &lrc, NULL, D3DLOCK_TILED );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("LockRect Failed.\n")
					);
		m_pSrcDevice8->SetRenderTarget( NULL, NULL );
		RELEASE( pSrcDepthSurface );
		Fail();
		EndTestCase();
		return(FALSE);
	}

	if( !InitializeQuickTest( nTest, &lrc, &Desc ) )
	{
		WriteToLog(
					TEXT("Test Initialization Failed.\n")
					);
		fFail = TRUE;
	}

	hr = pSrcDepthSurface->UnlockRect( );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("UnlockRect Failed.\n")
					);
		fFail = TRUE;
	}

	
	
	if( !ExecuteQuickTest( nTest ) )
	{
		WriteToLog(
					TEXT("Test Execution Failed.\n")
					);
		fFail = TRUE;
	}

	LPTSTR szLogBuffer = (LPTSTR)malloc( 4096 );
	if( !szLogBuffer )
	{
		WriteToLog(
					TEXT("malloc failed.\n")
					);
		m_pSrcDevice8->SetRenderTarget( NULL, NULL );
		RELEASE( pSrcDepthSurface );
		Fail();
		EndTestCase();
		return(FALSE);
	}
	lstrcpy( szLogBuffer, _T("") );
	
	hr = m_pSrcDevice8->GetBackBuffer( 0, (D3DBACKBUFFER_TYPE)0, &pSrcSurface );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("GetBackBuffer Failed.\n")
					);
		m_pSrcDevice8->SetRenderTarget( NULL, NULL );
		RELEASE( pSrcDepthSurface );
		Fail();
		EndTestCase();
		return(FALSE);
	}

	D3DLOCKED_RECT lrcColor;
	hr = pSrcSurface->LockRect( &lrcColor, NULL, D3DLOCK_TILED );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("LockRect Failed.\n")
					);
		m_pSrcDevice8->SetRenderTarget( NULL, NULL );
		RELEASE( pSrcDepthSurface );
		Fail();
		EndTestCase();
		return(FALSE);
	}

	hr = pSrcDepthSurface->LockRect( &lrc, NULL, D3DLOCK_TILED );
	if( FAILED( hr ) )
	{
		pSrcSurface->UnlockRect( );
		WriteToLog(
					TEXT("LockRect Failed.\n")
					);
		m_pSrcDevice8->SetRenderTarget( NULL, NULL );
		RELEASE( pSrcDepthSurface );
		Fail();
		EndTestCase();
		return(FALSE);
	}

	if( !ValidateQuickTest( nTest, &lrcColor, &Desc, &lrc, &Desc, szLogBuffer )  && !fFail )
	{
		fFail = TRUE;
	}

	hr = pSrcDepthSurface->UnlockRect( );
	if( FAILED( hr ) )
	{
		pSrcSurface->UnlockRect( );
		WriteToLog(
					TEXT("UnlockRect Failed.\n")
					);
		fFail = TRUE;
	}

	hr = pSrcSurface->UnlockRect( );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("UnlockRect Failed.\n")
					);
		fFail = TRUE;
	}

	if( lstrlen( szLogBuffer ) > 0 ) 
		WriteToLog( szLogBuffer );
	free( szLogBuffer );

	RELEASE( pSrcSurface );

//	POINT p={0,0};
//	Present( p,CD3D_SRC);
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	hr = m_pSrcDevice8->SetRenderTarget( NULL, NULL );
	if( FAILED( hr ) )
	{
		WriteToLog(
					TEXT("SetRenderTarget Failed.\n")
					);
		fFail = TRUE;
	}

	RELEASE( pSrcDepthSurface );

	if( fFail )
	{
		Fail();
	}


    EndTestCase();

    return(FALSE);
}

//-----------------------------------------------------------------------------
//
// The z buffer is locked, take any actions we want to do.  Populate with data if desired.
//
//-----------------------------------------------------------------------------

bool
LockDepth::InitializeQuickTest
(
    UINT nTest,
	D3DLOCKED_RECT*  plrcDepth,
	D3DSURFACE_DESC* DescDepth
)
{
	if( nTest == 1)
		return TRUE;

	if( nTest == 2)
		return TRUE;

	if( nTest == 3)
		return TRUE;

	BYTE* lpByte;
	BYTE* lpScanLine;
	WORD* lpThisWord;

	UINT x,y;
	lpByte = (BYTE*) plrcDepth->pBits;
	lpScanLine = lpByte;
	lpThisWord = NULL;

	for(y = 0; y < DescDepth->Height; y++)
	{
		lpThisWord = (WORD*) lpScanLine;
		for( x=0; x< DescDepth->Width; x++ )
			*lpThisWord++ = GetZDepthWord( nTest, (FLOAT)x-0.5f,(FLOAT)y-0.5f );
		lpScanLine += plrcDepth->Pitch;
	}

	return TRUE;

}

//-----------------------------------------------------------------------------
//
// Take and actions we want to do, while the buffers are NOT locked.
//
//-----------------------------------------------------------------------------

bool
LockDepth::ExecuteQuickTest
(
	UINT nTest
)
{
	HRESULT hr=D3D_OK;

	if( nTest == 1 )
	{
		hr = m_pSrcDevice8->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
		return SUCCEEDED(hr);
	}

	if( nTest == 2 )
	{
		hr = m_pSrcDevice8->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 0.0f, 0 );
		return SUCCEEDED(hr);
	}

	if( nTest == 3 )
	{
		hr = m_pSrcDevice8->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 0.5f, 0 );
		return SUCCEEDED(hr);
	}

	hr = m_pSrcDevice8->Clear( 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0 );

	if( SUCCEEDED( hr) )
		hr = Render( nTest );

	return SUCCEEDED(hr);
}

//-----------------------------------------------------------------------------
//
// determine if the color and z buffers contain results that we expected
//
//-----------------------------------------------------------------------------

bool
LockDepth::ValidateQuickTest
(
    UINT nTest,
	D3DLOCKED_RECT*  plrcColor,
	D3DSURFACE_DESC* DescColor,
	D3DLOCKED_RECT*  plrcDepth,
	D3DSURFACE_DESC* DescDepth,
	LPTSTR			 szLogBuffer
)
{
	BYTE* lpByte;
	BYTE* lpScanLine;
	WORD* lpThisWord;
	BYTE* lpThisByte;
	BYTE* lpDepthScanLine;

	TCHAR sz[MAX_PATH];

	UINT x,y;
	WORD  wKey;

	// the ClearBuffer tests - easy
	if( nTest == 1 || nTest == 2 || nTest == 3)
	{
		// determine which value we expect to find in the z-buffer from the previous clear
		switch( nTest )
		{
		case 1:
			wKey = (WORD)-1;
			break;
		case 2:
			wKey = (WORD) 0;
			break;
		case 3:
			wKey = (WORD) 0x8000;
			break;
		default:
			wKey = (WORD) 0;
			break;
		}
	
		lpByte = (BYTE*) plrcDepth->pBits;
		lpScanLine = lpByte;
		lpThisWord = NULL;

		for(y = 0; y < DescDepth->Height; y++)
		{
			lpThisWord = (WORD*) lpScanLine;
			for( x=0; x< DescDepth->Width; x++ )
			{
				// if we are within 1 of our target, we pass.  Otherwise, fail.
				if( (WORD)(*lpThisWord - wKey + m_wTolerance) > (WORD)(m_wTolerance<<1) )
				{
					_stprintf( sz, _T("ZBuffer cleared to %u, expected %u.\n"), *lpThisWord, wKey );
					lstrcat( szLogBuffer, sz );
					return FALSE;
				}
				lpThisWord++;
			}
			lpScanLine += plrcDepth->Pitch;
		}
	
		return TRUE;
	}

	
	//  This section handles the render cases. 

	lpScanLine = (BYTE*) plrcColor->pBits;
	lpDepthScanLine = (BYTE*) plrcDepth->pBits;
	
	BYTE bKey;

	DWORD dwStrictErrors=(DWORD)0;
	DWORD dwErrors=(DWORD)0;
	DWORD dwCloseCalls=(DWORD)0;

	DWORD dwDepthErrors=0;

	WORD wImageDepth;
	WORD wDepthCalculated;
	WORD wDepthMin;
	WORD wDepthActual;

	BOOL  bCloseCall;
	BOOL  bThisTestFailed;
	BOOL  bThisDepthTestFailed;

	static UINT uMaxFailsLog = 20;
	UINT uFailsLog=0;

	for(y = 0; y < DescColor->Height; y++)
	{
		lpThisByte = lpScanLine;
		lpThisWord = (WORD*) lpDepthScanLine;
		for( x=0; x< DescColor->Width; x++ )
		{
			bThisTestFailed = FALSE;
			
			// Predict what the z-buffer depth would be if it came from one of the triangles we rendered.
			wImageDepth = GetZImageDepthWord(nTest,(FLOAT)x-0.5f,(FLOAT)y -0.5f + FUDGE_FACTOR);
			
			// predict what the z-buffer depth would be if the pixel failed the z-test.
			// (this is the z-buffer depth that we stuffed into the buffer the 1st time we locked it).
			wDepthCalculated = GetZDepthWord(nTest,(FLOAT)x-0.5f,(FLOAT)y -0.5f);

			// conduct our own z-test
			bKey = wImageDepth > wDepthCalculated ? (BYTE)0 : (BYTE)-1;
			
			// which z was the winner?
			wDepthMin = wImageDepth < wDepthCalculated ? wImageDepth : wDepthCalculated;
			
			// determine if the z-values are really too close to call
			// the reason we do this is, we are not concerned about "slop" in the color buffer.  The
			// pixel will either be black, or it will be white.  The "slop" we are concerned with is
			// in the depth buffer.  It could make a white pixel black, and vice versa.
			bCloseCall = (WORD)(wImageDepth - wDepthCalculated + m_wTolerance) <= (WORD)(m_wTolerance)<<1 ? TRUE : FALSE;

			// read the z-buffer.  If the value
			wDepthActual = *lpThisWord;
			
			// does the depth buffer match our prediction to within 1?
			bThisDepthTestFailed = (WORD)(wDepthMin - wDepthActual + m_wTolerance) <= (WORD)(m_wTolerance)<<1 ? FALSE : TRUE;

			// next z-buffer value
			lpThisWord++;

			switch( DescColor->Format )
			{
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
#ifdef UNDER_XBOX
			case D3DFMT_LIN_X8R8G8B8:
			case D3DFMT_LIN_A8R8G8B8:
#endif
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				// hack, alter the color buffer value if the depth test was bad, to make it visibly different.
				if( bThisDepthTestFailed )
					*lpThisByte = (BYTE)0x80;
				lpThisByte++;
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				// hack, alter the color buffer value if the color test was bad, to indicate which pixels fail
				if( bThisTestFailed )
					*lpThisByte = (BYTE)0x80;
				lpThisByte++;
				lpThisByte++;
				break;
			case D3DFMT_R8G8B8:
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				break;
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
#ifdef UNDER_XBOX
			case D3DFMT_LIN_X1R5G5B5:
			case D3DFMT_LIN_A1R5G5B5:
#endif
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				if( ((*lpThisByte)&0x7f) != (bKey&0x7f) ) bThisTestFailed = TRUE;
				lpThisByte++;
				break;
			case D3DFMT_R5G6B5:
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				if( *lpThisByte != bKey ) bThisTestFailed = TRUE;
				lpThisByte++;
				break;
			default:
				bThisTestFailed = TRUE;
				break;
			}
			if( bThisTestFailed )
				dwStrictErrors++;
			if( bThisTestFailed && !bCloseCall)
				dwErrors++;
			if( bCloseCall )
				dwCloseCalls++;
			if( bThisDepthTestFailed )
				dwDepthErrors++;
			// next lines temporary
			if( bThisDepthTestFailed && uFailsLog < uMaxFailsLog)
			{
				uFailsLog++;
				_stprintf( sz, _T("Pixel x=%u,y=%u Actual z=%u, Expected z=%u.\n"), x, y, (UINT)wDepthActual,(UINT)wDepthMin );
				lstrcat( szLogBuffer, sz );
			}
		}
		lpScanLine += plrcColor->Pitch;
		lpDepthScanLine += plrcDepth->Pitch;
	}


	if( dwErrors > 0 )
	{
		_stprintf( sz, _T("%u errors in Color Buffer.\n"), dwErrors );
		lstrcat( szLogBuffer, sz );
	}

	if( dwDepthErrors > 0 )
	{
		_stprintf( sz, _T("%u errors in Depth Buffer.\n"), dwDepthErrors );
		lstrcat( szLogBuffer, sz );
	}

	if( dwErrors > 0 )
		return FALSE;

	if( dwDepthErrors > 0 )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
//
// Render two trianges:
//
//  z=0         z=1
//     +---------+
//     |\        |
//     |  \      |
//     |    \    |
//     |      \  |
//     |        \|
//     +---------+
//  z=1          z=0
//
//-----------------------------------------------------------------------------

HRESULT LockDepth::Render( UINT nTest )
{

	BeginScene();

	SetVertexShader( D3DFVF_TEX2DVERTEX );
	SetStreamSource( 0, m_pSrcVB, NULL, sizeof( TEX2DVERTEX), CD3D_SRC );
#ifndef UNDER_XBOX
    SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE, CD3D_SRC );
#endif


    // Fill vertex buffer
    TEX2DVERTEX* pSrcVertices,*pVertices;
    DWORD         dwNumTriangles = 0;

    FLOAT sx = 0.0f;
    FLOAT sy = 0.0f;
    FLOAT w = (FLOAT)m_pDisplay->GetWidth();//m_pSrcTarget->m_dwWidth;
    FLOAT h = (FLOAT)m_pDisplay->GetHeight();//m_pSrcTarget->m_dwHeight;

	if( FAILED( m_pSrcVB->Lock( 0, MAX_VERTICES * sizeof(TEX2DVERTEX), (BYTE**)&pSrcVertices, 0 )) ) return E_FAIL;

	pVertices = pSrcVertices;

	*pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,GetZImageDepthFloat(nTest,sx+0,sy+h),1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,GetZImageDepthFloat(nTest,sx+0,sy+0),1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,GetZImageDepthFloat(nTest,sx+w,sy+h),1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,GetZImageDepthFloat(nTest,sx+w,sy+0),1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,GetZImageDepthFloat(nTest,sx+w,sy+h),1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,GetZImageDepthFloat(nTest,sx+0,sy+0),1.0f), 0xffffffff );

/*  - Compress image down to 5 pixels vertically, check color buffer, vary color from 0-255 vertically
	Note: Findings were that the color buffer did NOT show a 1/8 pixel shift.
	pixel 0 19/1A 25   
	pixel 1 4C/4D 76   
	pixel 2 7F/80 127  
	pixel 3 B2/B3 178  
	pixel 4 E5/E6 229  
	*pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+5.0f-0.5f,0.0f,1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0.0f-0.5f,0.0f,1.0f), 0x00000000 );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+5.0f-0.5f,0.0f,1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0.0f-0.5f,0.0f,1.0f), 0x00000000 );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+5.0f-0.5f,0.0f,1.0f), 0xffffffff );
    *pVertices++ = InitTex2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0.0f-0.5f,0.0f,1.0f), 0x00000000 );
*/    
	dwNumTriangles += 2;

    // Unlock and render the vertex buffer
 	m_pSrcVB->Unlock( );

//	DrawPrimitive8( D3DPT_TRIANGLELIST, 0, dwNumTriangles, CD3D_SRC );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

	EndScene();

    return S_OK;
}

//-----------------------------------------------------------------------------
//
//  State changes, etc, that only need to be done once. 
//
//-----------------------------------------------------------------------------

HRESULT LockDepth::OneTimeInit( VOID )
{
    SetRenderState( D3DRS_ZENABLE, (DWORD) TRUE );
    SetRenderState( D3DRS_DITHERENABLE, (DWORD) TRUE );
    SetRenderState( D3DRS_SPECULARENABLE, (DWORD) FALSE );
    SetRenderState( D3DRS_AMBIENT, (DWORD) 0x00ffffff );
    SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) FALSE );

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f,-5.0f,-10.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMATRIX  matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    FLOAT fAspect = (FLOAT)m_pDisplay->GetWidth() / (FLOAT)m_pDisplay->GetHeight();//m_pSrcTarget->m_dwWidth / (FLOAT)m_pSrcTarget->m_dwHeight;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    SetTransform( D3DTS_WORLD,      &matWorld );
    SetTransform( D3DTS_VIEW,       &matView );
    SetTransform( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

//-----------------------------------------------------------------------------
//
//  Determines what value to populate our zbuffer with prior to rendering
//
//-----------------------------------------------------------------------------

WORD
LockDepth::GetZDepthWord
(
	UINT nTest,
	FLOAT x,
	FLOAT y
)
{

	WORD w;

	w = (WORD) ((x * 16.0f* (FLOAT)nTest) + y * 500.0f);
	w += (WORD) ((x * 201.0f) - (y * 20.0f * (FLOAT)nTest));

	if( nTest > 48)
		w = (WORD) (nTest-49) * 5001;

	return w;
}

//-----------------------------------------------------------------------------
//
//  Determines what value to populate our zbuffer with prior to rendering - returns a FLOAT.
//
//-----------------------------------------------------------------------------

FLOAT
LockDepth::GetZDepthFloat
(
	UINT nTest,
	FLOAT x,
	FLOAT y
)
{
	WORD w = GetZDepthWord( nTest, x, y);

	return ((float)w )/(float)65535.0f;

}

//-----------------------------------------------------------------------------
//
//  Predicts what the rendered triangles z-depth should be, based on a x,y position
//
//-----------------------------------------------------------------------------

FLOAT
LockDepth::GetZImageDepthFloat
(
	UINT nTest,
	FLOAT x,
	FLOAT y
)
{

	float f;

    FLOAT w = (FLOAT)m_pDisplay->GetWidth();//m_pSrcTarget->m_dwWidth;
    FLOAT h = (FLOAT)m_pDisplay->GetHeight();//m_pSrcTarget->m_dwHeight;

	if( w == 0.0f || h == 0.0f ) return (float)0.0f;

	f= ((x / w) - (y / h))*65535.0f/65536.0f;

	if( f<0.0f ) return -f;

	return f;

}

//-----------------------------------------------------------------------------
//
//  Predicts what the rendered triangles z-depth should be, based on a x,y position - returns WORD
//
//-----------------------------------------------------------------------------

WORD
LockDepth::GetZImageDepthWord
(
	UINT nTest,
	FLOAT x,
	FLOAT y
)
{
	float f = GetZImageDepthFloat( nTest, x, y );

	WORD w = (WORD) (f*65535.0f);

	return w;
}
