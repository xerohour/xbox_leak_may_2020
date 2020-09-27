//
//  LostOnReset.cpp
//
//  normal vb/ib rendering tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "CVertexIndexBufferTest.h"



extern CD3DWindowFramework   *g_pD3DFramework;



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

LostOnReset::LostOnReset()
{
    m_szTestName = _T("Lost on Reset");
    m_szCommandKey = _T("LostOnReset");

    m_ModeOptions.fReference = false;

    m_pDevice = NULL;
    m_pRefDevice = NULL;
}

LostOnReset::~LostOnReset()
{
    RELEASE(m_pDevice);
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT LostOnReset::TestInitialize()
{

    m_pD3D = m_pDisplay->GetDirect3D8();
/*
    if( m_pD3D8 && m_pSrcDevice8 )
    {
        m_pD3D =  m_pD3D8;
    }
    else
	{
		WriteToLog(_T("TestInitialize() - Framework devices == NULL\n"));
        return D3DTESTINIT_ABORT;
	}
*/

    // set the test range
    ExecuteTest( (UINT)(-1) );


    if (!m_shape.NewShape( CS_SPHERE, 10 )) {
        return D3DTESTINIT_ABORT;
    }


    return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool
LostOnReset::ExecuteTest
(
    UINT nTest
)
{

    static const UINT Usages[] =
    {
        NULL,
        D3DUSAGE_WRITEONLY,
        D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS,
        D3DUSAGE_RTPATCHES,
        D3DUSAGE_NPATCHES,
        D3DUSAGE_DYNAMIC,
        D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
        D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
        D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
#ifdef D3DUSAGE_LOADONCE
        D3DUSAGE_LOADONCE,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY,
        D3DUSAGE_LOADONCE | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
#endif
    };

    static const D3DPOOL Pools[] =
    {
        D3DPOOL_DEFAULT,
        D3DPOOL_MANAGED,
        D3DPOOL_SYSTEMMEM,
    };

    typedef union __TESTCASE
    {
        struct
        {
            UINT Pool : 3;
            UINT Usage : 5;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(Pools),
        COUNTOF(Usages)
    };


    TESTCASE tc;
    tc.nTest = (nTest - 1);

    if( nTest == (UINT)(-1) )
    {
        ClearRangeList();

        tc.nTest = 0;
        for( tc.Usage = 0; tc.Usage < tcCount.Usage; tc.Usage++ )
        {
            for( tc.Pool = 0; tc.Pool < tcCount.Pool; tc.Pool++ )
            {
                AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
            }
        }

        return( FALSE );
    }

    if( (tc.Usage >= tcCount.Usage) ||
        (tc.Pool >= tcCount.Pool)  )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }

    if( (Usages[tc.Usage] & D3DUSAGE_DYNAMIC) && (Pools[tc.Pool] == D3DPOOL_MANAGED) )
    {
        return(FALSE);
    }

    ClearFrame();


    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d"),
        Usages[tc.Usage],
        Pools[tc.Pool]
    );

    BeginTestCase( sz, nTest );

    D3DTEST_NORMALBUFFER curtest;

    curtest.szName = TEXT("");
    curtest.shape = &m_shape;
    curtest.VBFVF = D3DFVF_VERTEX;
    curtest.IBFormat = D3DFMT_INDEX16;
    curtest.Usage = Usages[tc.Usage];
    curtest.Pool = Pools[tc.Pool];


    // initialize every test case because Reset() from previous test may fail
    UINT uiRet;

    uiRet = TestInitReal( &m_pDevice );
    if( uiRet != D3DTESTINIT_RUN )
    {
        Fail();
        return(FALSE);
    }


    BOOL bRetTest;

    bRetTest = ExecuteReal(
        m_pDevice,
        &curtest,
        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );

    if( !bRetTest )
    {
        Fail();
    }

    RELEASE(m_pDevice);

    return(FALSE);
}



UINT
LostOnReset::TestInitReal
(
    LPDIRECT3DDEVICE8 *ppDevice
)
{

    HRESULT hr;
    LPDIRECT3DDEVICE8 pDevice = NULL;
    D3DDISPLAYMODE Mode;


    m_pD3D->GetAdapterDisplayMode(m_pMode->nAdapter, &Mode);

    // Fill presentation m_SrcParameters
    m_SrcParam.BackBufferWidth = m_pMode->dwWidth;
    m_SrcParam.BackBufferHeight = m_pMode->dwHeight;
    m_SrcParam.BackBufferFormat = Mode.Format;
    m_SrcParam.BackBufferCount = 1;
    m_SrcParam.MultiSampleType = m_MultiSampleTypeList[m_pMode->nSrcMultiSampleType];
    m_SrcParam.SwapEffect = (m_SrcParam.MultiSampleType == D3DMULTISAMPLE_NONE) ? D3DSWAPEFFECT_COPY_VSYNC : D3DSWAPEFFECT_DISCARD;
    m_SrcParam.hDeviceWindow = g_pD3DFramework->m_DisplayWindow.m_hWindow;
    m_SrcParam.Windowed = m_pMode->fFullScreen ? FALSE : TRUE;
    m_SrcParam.EnableAutoDepthStencil = (m_pMode->nZBufferFormat == DISABLE_ZBUFFER) ? FALSE : TRUE;
    m_SrcParam.AutoDepthStencilFormat = (m_pMode->nZBufferFormat == DISABLE_ZBUFFER) ? D3DFMT_UNKNOWN : m_pCommonZBufferFormats[m_pMode->nZBufferFormat].d3dfFormat;
    m_SrcParam.Flags = 0;
    m_SrcParam.FullScreen_RefreshRateInHz = m_pMode->fFullScreen ? m_pMode->dwRefreshRate : 0;
    m_SrcParam.FullScreen_PresentationInterval = 0;


    // createdevice
    hr = m_pD3D->CreateDevice(
        m_pMode->nAdapter,
        m_pAdapter->Devices[m_pMode->nSrcDevice].DeviceType,
        g_pD3DFramework->m_DisplayWindow.m_hWindow,
        m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags, 
        &m_SrcParam,
        ppDevice
    );
    if( FAILED(hr) )
    {
        WriteToLog(
            TEXT("CreateDevice() returned %s (0x%x)\n"),
            DecodeHResult(hr),
            hr
        );
        return(D3DTESTINIT_ABORT);
    }

    pDevice = *ppDevice;


    // Set the magic matricies
    D3DMATRIX matView =
    {
        1.0f,    0,       0,       0,
        0,       1.0f,    0,       0,
        0,       0,       1.0f,    0,
        0,       0,       2.0f,/*10.0f,*/   1.0f
    };
    pDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DMATRIX matProj =
    {
        2.0f,    0,       0,       0,
        0,       2.0f,    0,       0,
        0,       0,       1.0f,    1.0f,
        0,       0,       -1.0f,   0.0f
    };
    pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set appropiate states
    pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 0xff, 0xff, 0xff ) );
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    pDevice->SetRenderState( D3DRS_CLIPPING, FALSE );

    D3DXMATRIX d3dMat;
    D3DXMATRIX d3dMatOut;

    D3DMATRIX matWorld =
    {
        1.0f,    0,       0,       0,
        0,       1.0f,    0,       0,
        0,       0,       1.0f,    0,
        0,       0,       0,       1.0f
    };

    d3dMat = matWorld;

    D3DXMatrixMultiply(
        &d3dMatOut,
        &d3dMat,
        D3DXMatrixRotationY( &d3dMatOut, 2.0f )
    );

    d3dMat = d3dMatOut;

    D3DXMatrixMultiply(
        &d3dMatOut,
        &d3dMat,
        D3DXMatrixRotationZ( &d3dMatOut, 0.40f )
    );

    d3dMat = d3dMatOut;

    D3DXMatrixMultiply(
        &d3dMatOut,
        &d3dMat,
        D3DXMatrixRotationX( &d3dMatOut, 2.0f )
    );

    d3dMat = d3dMatOut;

    hr = pDevice->SetTransform(
        D3DTS_WORLD,
        &d3dMat
    );
    if( FAILED(hr) )
    {
        WriteToLog(
            TEXT("SetTransform() returned %s (0x%x)\n"),
            DecodeHResult(hr),
            hr
        );
        return(D3DTESTINIT_ABORT);
    }


    D3DMATERIAL8 mtrl =
    {
        { 1.0f, 0.0f, 0.0f, 0.0f },  // diffuse
        { 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
        { 1.0f, 0.0f, 0.0f, 0.0f },	 // specular
        { 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
        0.0f  // sharpness (for specular)
    };

    hr = pDevice->SetMaterial(
        &mtrl
    );
    if( FAILED(hr) )
    {
        WriteToLog(
            TEXT("SetMaterial() returned %s (0x%x)\n"),
            DecodeHResult(hr),
            hr
        );
        return(D3DTESTINIT_ABORT);
    }


    return(D3DTESTINIT_RUN);
}



BOOL
LostOnReset::ExecuteReal
(
    LPDIRECT3DDEVICE8 pDevice,
    const D3DTEST_NORMALBUFFER *lpBufInfo,
    BOOL fMixedMode
)
{

    HRESULT hr = D3D_OK;
    LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
    LPDIRECT3DINDEXBUFFER8 pIB = NULL;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;

    BOOL fSoftwareUsage = FALSE;


    // determine sw usage
    if( fMixedMode )
    {
        DWORD dwSW;
        pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
        fSoftwareUsage = dwSW;
    }

    do
    {
        hr = pDevice->CreateVertexBuffer(
            lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
            lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->VBFVF,
            lpBufInfo->Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                lpBufInfo->VBFVF,
                lpBufInfo->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pVB->Lock(
            0,
            lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
            &pbObject,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Lock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        CopyMemory( pbObject, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX) );

        hr = pVB->Unlock();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Unlock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pDevice->SetStreamSource(
            0,
            pVB,
            sizeof(D3DVERTEX)
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetStreamSource() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }


        hr = pDevice->CreateIndexBuffer(
            lpBufInfo->shape->m_nIndices * sizeof(WORD),
            lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->IBFormat,
            lpBufInfo->Pool,
            &pIB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nIndices * sizeof(WORD),
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                lpBufInfo->IBFormat,
                lpBufInfo->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pIB->Lock(
            0,
            lpBufInfo->shape->m_nIndices * sizeof(WORD),
            &pbObject,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Lock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        CopyMemory( pbObject, lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices * sizeof(WORD) );

        hr = pIB->Unlock();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Unlock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pDevice->SetIndices(
            pIB,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetIndices() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }


        hr = pDevice->SetVertexShader( D3DFVF_VERTEX );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetVertexShader() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

    } while(0);


    if( !fRet )
    {
        RELEASE(pIB);
        RELEASE(pVB);
        return(fRet);
    }


    do
    {
        if( lpBufInfo->Pool == D3DPOOL_DEFAULT )
        {
            // make sure reset fails until buffers freed

            hr = pDevice->Reset( &m_SrcParam );
            if( hr != D3DERR_DEVICELOST )
            {
                WriteToLog(
                    TEXT("Reset() (with VB and IB) returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            try
            {
                RELEASE(pVB);
            }
            catch(...)
            {
                WriteToLog(
                    TEXT("VB->Release() caused unhandled exception\n")
                );
                Fail();
            }

            hr = pDevice->Reset( &m_SrcParam );
            if( hr != D3DERR_DEVICELOST )
            {
                WriteToLog(
                    TEXT("Reset() (with IB) returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            try
            {
                RELEASE(pIB);
            }
            catch(...)
            {
                WriteToLog(
                    TEXT("IB->Release() caused unhandled exception\n")
                );
                Fail();
            }

            hr = pDevice->Reset( &m_SrcParam );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("Reset() returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

        }
        else
        {
            // buffers should not be lost

            hr = pDevice->Reset( &m_SrcParam );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("Reset() returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

#ifdef D3DUSAGE_LOADONCE
            if( !(lpBufInfo->Usage & D3DUSAGE_LOADONCE) )
            {
#endif
                hr = pVB->Lock(
                    0,
                    0,
                    &pbObject,
                    0
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Lock() after Reset() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                hr = pVB->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Unlock() after Reset() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                hr = pIB->Lock(
                    0,
                    0,
                    &pbObject,
                    0
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Lock() after Reset() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                hr = pIB->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Unlock() after Reset() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }
#ifdef D3DUSAGE_LOADONCE
            }
#endif
        }
    }
    while(0);


    RELEASE(pIB);
    RELEASE(pVB);

    return(fRet);
}

