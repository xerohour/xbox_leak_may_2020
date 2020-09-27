//
//  NormalBuffer.cpp
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



//#define SWPUREDEVICEHACK



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

NormalBuffer::NormalBuffer()
{
    m_szTestName = _T("Normal Buffer");
    m_szCommandKey = _T("NormalBuffer");
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT NormalBuffer::TestInitialize()
{

    m_pD3D =  m_pDisplay->GetDirect3D8();
/*
    if( m_pD3D8 && m_pSrcDevice8 && m_pRefDevice8 )
    {
        m_pD3D =  m_pD3D8;
        m_pDevice = m_pSrcDevice8;
        m_pRefDevice = m_pRefDevice8;
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


    UINT uiRet;

    uiRet = TestInitReal(
        m_pDevice
    );
    if( uiRet != D3DTESTINIT_RUN )
    {
        return( uiRet );
    }

/*
    uiRet = TestInitReal(
        m_pRefDevice
    );
    if( uiRet != D3DTESTINIT_RUN )
    {
        return( uiRet );
    }
*/
    return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool
NormalBuffer::ExecuteTest
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
//        D3DUSAGE_DYNAMIC,
        D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
//        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_SOFTWAREPROCESSING,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DONOTCLIP,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_RTPATCHES,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_NPATCHES,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
 //       D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
//        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
#ifdef D3DUSAGE_LOADONCE
        D3DUSAGE_LOADONCE,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY,
        D3DUSAGE_LOADONCE | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
#endif
    };

    static const D3DPOOL Pools[] =
    {
        POOL_DEFAULT
//        D3DPOOL_DEFAULT,
//        D3DPOOL_MANAGED,
//        D3DPOOL_SYSTEMMEM,
    };

    typedef union __TESTCASE
    {
        struct
        {
            UINT Pool : 3;
            UINT Usage : 6;
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


    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d"),
        Usages[tc.Usage],
        Pools[tc.Pool]
    );

    D3DTEST_NORMALBUFFER curtest;

    curtest.szName = TEXT("");
    curtest.shape = &m_shape;
#ifdef SWPUREDEVICEHACK
    curtest.VBFVF = D3DFVF_TLVERTEX;
#else
    curtest.VBFVF = D3DFVF_VERTEX;
#endif
    curtest.IBFormat = D3DFMT_INDEX16;
    curtest.Usage = Usages[tc.Usage];
    curtest.Pool = Pools[tc.Pool];


    // skip buffer if not valid
    DWORD dwSW = FALSE;
//    if( m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
//    {
//        m_pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
//    }

/*
    if(
        (  // dynamic is valid on managed buffers
            (curtest.Usage & D3DUSAGE_DYNAMIC) &&
            (curtest.Pool == D3DPOOL_MANAGED)
        ) ||
        (  // software usage is not valid on hardware device
            (curtest.Usage & D3DUSAGE_SOFTWAREPROCESSING) &&
            (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
        ) ||
        (  // software usage is required on mixed device with SW renderstate when pool is not sysmem
            !(curtest.Usage & D3DUSAGE_SOFTWAREPROCESSING) &&
            (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) &&
            dwSW &&
            (curtest.Pool != D3DPOOL_SYSTEMMEM)
        )
    )
    {
        return(FALSE);
    }
*/

    ClearFrame();

    BeginTestCase( sz, nTest );

    BOOL bRetTest;
    BOOL bRetRef;

    bRetTest = ExecuteReal(
        m_pDevice,
        &curtest,
        FALSE
    );

/*
    bRetRef = ExecuteReal(
        m_pRefDevice,
        &curtest,
        TRUE
    );
*/

    bool bRet = (bRetTest == TRUE);// && bRetRef;
    if( !bRet )
    {
        Fail();
    }

    return( bRet );
}



UINT
NormalBuffer::TestInitReal
(
    CDevice8* pDevice
)
{

    HRESULT hr;


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
#ifndef UNDER_XBOX
    pDevice->SetRenderState( D3DRS_CLIPPING, FALSE );
#endif

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
NormalBuffer::ExecuteReal
(
    CDevice8* pDevice,
    const D3DTEST_NORMALBUFFER *lpBufInfo,
    BOOL fRef
)
{

    HRESULT hr = D3D_OK;
    CVertexBuffer8* pVB = NULL;
    CIndexBuffer8* pIB = NULL;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;

#if 0
    DWORD Usage = lpBufInfo->Usage;


    // strip sw usage for ref device
    if( fRef )
    {
        Usage = Usage & (!D3DUSAGE_SOFTWAREPROCESSING);
    }
#endif

    do
    {
        hr = pDevice->CreateVertexBuffer(
            lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
            lpBufInfo->Usage,
            lpBufInfo->VBFVF,
            lpBufInfo->Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
                lpBufInfo->Usage,
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

#ifdef SWPUREDEVICEHACK
        {
            D3DTLVERTEX *pdata = (D3DTLVERTEX*)pbObject;

            pdata[0].sx = 0.0f;
            pdata[0].sy = 0.0f;
            pdata[0].sz = 0.0f;
            pdata[0].rhw = 1.0f;
            pdata[0].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
            pdata[0].specular = 0.0f;
            pdata[0].tu = 0.0f;
            pdata[0].tv = 0.0f;

            pdata[1].sx = 500.0f;
            pdata[1].sy = 0.0f;
            pdata[1].sz = 0.0f;
            pdata[1].rhw = 1.0f;
            pdata[1].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
            pdata[1].specular = 0.0f;
            pdata[1].tu = 0.0f;
            pdata[1].tv = 0.0f;

            pdata[2].sx = 0.0f;
            pdata[2].sy = 500.0f;
            pdata[2].sz = 0.0f;
            pdata[2].rhw = 1.0f;
            pdata[2].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
            pdata[2].specular = 0.0f;
            pdata[2].tu = 0.0f;
            pdata[2].tv = 0.0f;
        }
#else
        CopyMemory( pbObject, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX) );
#endif

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
            lpBufInfo->Usage,
            lpBufInfo->IBFormat,
            lpBufInfo->Pool,
            &pIB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nIndices * sizeof(WORD),
                lpBufInfo->Usage,
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

#ifdef SWPUREDEVICEHACK
        {
            PWORD pdata = (PWORD)pbObject;

            pdata[0] = 0;
            pdata[1] = 1;
            pdata[2] = 2;
        }
#else
        CopyMemory( pbObject, lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices * sizeof(WORD) );
#endif

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


#ifdef SWPUREDEVICEHACK
        hr = pDevice->SetVertexShader( D3DFVF_TLVERTEX );
#else
        hr = pDevice->SetVertexShader( D3DFVF_VERTEX );
#endif
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

    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
        hr = pDevice->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
#ifdef SWPUREDEVICEHACK
            0,
            3,
            0,
            1
#else
            0,
            lpBufInfo->shape->m_nVertices,
            0,
            lpBufInfo->shape->m_nIndices / 3
#endif
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("DrawIndexedPrimitive() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
        }

        // End the scene.
        hr = pDevice->EndScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("EndScene() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
        }
    }

    RELEASE(pIB);
    RELEASE(pVB);

    return(fRet);
}

