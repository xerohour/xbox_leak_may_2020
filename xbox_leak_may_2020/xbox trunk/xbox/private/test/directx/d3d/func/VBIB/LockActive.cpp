//
//  LockActive.cpp
//
//  lock selected vertex/index buffer tests.
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



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

LockActive::LockActive()
{
    m_szTestName = _T("Lock Active");
    m_szCommandKey = _T("LockActive");

    m_pSrcVB = NULL;
    m_pSrcIB = NULL;
    m_pRefVB = NULL;
    m_pRefIB = NULL;
}

LockActive::~LockActive()
{
    RELEASE( m_pSrcVB );
    RELEASE( m_pSrcIB );
    RELEASE( m_pRefVB );
    RELEASE( m_pRefIB );
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT LockActive::TestInitialize()
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
    if (!m_shape2.NewShape( CS_PYRAMID )) {
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
LockActive::ExecuteTest
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
    };

    static const D3DPOOL Pools[] =
    {
        POOL_DEFAULT
//        D3DPOOL_DEFAULT,
//        D3DPOOL_MANAGED,
//        D3DPOOL_SYSTEMMEM,
    };

    static const UINT LockFlags[] =
    {
        NULL,
#ifndef UNDER_XBOX
        D3DLOCK_DISCARD,
#endif
        D3DLOCK_NOOVERWRITE,
//        D3DLOCK_NOSYSLOCK,
#ifndef UNDER_XBOX
        D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE,
#endif
//        D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK,
//        D3DLOCK_NOOVERWRITE|D3DLOCK_NOSYSLOCK,
//        D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE|D3DLOCK_NOSYSLOCK,
    };

    static const D3DTEST_LOCKACTIVE LockActiveTests[] =
    {
    //  { szName, fCreate, fAppendVB, fAppendIB, fDraw1, fDraw2 },

        { TEXT("Creating buffers, rendering base objects"), TRUE, FALSE, FALSE, TRUE, FALSE },
        { TEXT("Locking Vertex Buffer and adding second object"), FALSE, TRUE, FALSE, TRUE, FALSE },
        { TEXT("Locking Index Buffer and adding second object"), FALSE, FALSE, TRUE, TRUE, FALSE },
        { TEXT("Rendering second object"), FALSE, FALSE, FALSE, FALSE, TRUE },
        { TEXT("Rendering both objects"), FALSE, FALSE, FALSE, TRUE, TRUE },
        { TEXT("Multiple locks and drawprim in one scene"), TRUE, TRUE, TRUE, TRUE, TRUE },

    };


    typedef union __TESTCASE
    {
        struct
        {
            UINT TestStage : 3;
            UINT LockFlag : 6;
            UINT Pool : 3;
            UINT Usage : 5;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(LockActiveTests),
        COUNTOF(LockFlags),
        COUNTOF(Pools),
        COUNTOF(Usages)
    };


    static TESTCASE prevtest = {0};
    static BOOL bSkip = FALSE;

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
                for( tc.LockFlag = 0; tc.LockFlag < tcCount.LockFlag; tc.LockFlag++ )
                {
                    for( tc.TestStage = 0; tc.TestStage < tcCount.TestStage; tc.TestStage++ )
                    {
                        AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
                    }
                }
            }
        }

        return( FALSE );
    }

    if( (tc.Usage >= tcCount.Usage) ||
        (tc.Pool >= tcCount.Pool) ||
        (tc.LockFlag >= tcCount.LockFlag ) ||
        (tc.TestStage >= tcCount.TestStage) )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }

    if( tc.TestStage == 0 || tc.TestStage == 5 )
    {
        bSkip = FALSE;
    }
    else if( tc.TestStage != (prevtest.TestStage + 1) % tcCount.TestStage )
    {
        bSkip = TRUE;
    }

    if( bSkip )
    {
        return(FALSE);
    }

    prevtest.nTest = tc.nTest;

/*
    if(
        ((Usages[tc.Usage] & D3DUSAGE_DYNAMIC) && (Pools[tc.Pool] == D3DPOOL_MANAGED)) ||
        (!(Usages[tc.Usage] & D3DUSAGE_DYNAMIC) && (LockFlags[tc.LockFlag] & D3DLOCK_DISCARD)) ||
        (!(Usages[tc.Usage] & D3DUSAGE_DYNAMIC) && (LockFlags[tc.LockFlag] & D3DLOCK_NOOVERWRITE))
    )
    {
        return(FALSE);
    }
*/

    ClearFrame();


    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x; %s"),
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag],
        LockActiveTests[tc.TestStage].szName
    );

    BeginTestCase( sz, nTest );

 
    BOOL bRetTest;
    BOOL bRetRef;

#if 0
    wsprintf( sz,
        TEXT("%d: Usage = 0x%x, Pool = %d, LockFlags = 0x%x; %s\n"),
        nTest,
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag],
        LockActiveTests[tc.TestStage].szName
    );
    OutputDebugString(sz);
#endif

    bRetTest = ExecuteReal(
        m_pDevice,
        &m_pSrcVB,
        &m_pSrcIB,
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag],
        &LockActiveTests[tc.TestStage],
        false
//        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );

#if 0
    OutputDebugString(TEXT("...\n"));
#endif

/*
    bRetRef = ExecuteReal(
        m_pRefDevice,
        &m_pRefVB,
        &m_pRefIB,
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag],
        &LockActiveTests[tc.TestStage],
        (m_pAdapter->Devices[m_pMode->nRefDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );
*/
#if 0
    OutputDebugString(TEXT("done.\n"));
#endif

    bool bRet = (bRetTest == TRUE);// && bRetRef;
    if( !bRet )
    {
        bSkip = TRUE;
        Fail();
    }

    return( bRet );
}



UINT
LockActive::TestInitReal
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
        D3DXMatrixRotationX( &d3dMatOut, -0.46f )
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
        { 0.8f, 0.0f, 0.0f, 0.0f },  // diffuse
        { 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
        { 0.0f, 0.0f, 1.0f, 0.0f },	 // specular
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
LockActive::ExecuteReal
(
    CDevice8* pDevice,
    CVertexBuffer8 **ppVB,
    CIndexBuffer8 **ppIB,
    DWORD Usage,
    D3DPOOL Pool,
    DWORD LockFlags,
    const D3DTEST_LOCKACTIVE *Test,
    BOOL fMixedMode
)
{

    HRESULT hr = D3D_OK;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;

    LPTSTR szLastCall = TEXT("");

    do
    {
//        try
        {
            if( Test->fCreate )  // create buffers, copy first object
            {
                BOOL fSoftwareUsage = FALSE;

                // determine sw usage
                if( fMixedMode )
                {
//                    DWORD dwSW;
//                    pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
//                    fSoftwareUsage = dwSW;
                }

                szLastCall = TEXT("Release(VB)");
                RELEASE(*ppVB);

                szLastCall = TEXT("CreateVertexBuffer()");
                hr = pDevice->CreateVertexBuffer(
                    (m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
                    Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                    D3DFVF_VERTEX,
                    Pool,
                    ppVB
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                        (m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
                        Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                        D3DFVF_VERTEX,
                        Pool,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("VB->Lock()");
                hr = (*ppVB)->Lock(
                    0,
                    m_shape.m_nVertices * sizeof(D3DVERTEX),
                    &pbObject,
                    LockFlags
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Lock(0,%d,,0x%x) returned %s (0x%x)\n"),
                        m_shape.m_nVertices * sizeof(D3DVERTEX),
                        LockFlags,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("CopyMemory(,VB,)");
                CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

                szLastCall = TEXT("VB->Unlock()");
                hr = (*ppVB)->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Unlock() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("SetStreamSource()");
                hr = pDevice->SetStreamSource(
                    0,
                    *ppVB,
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


                szLastCall = TEXT("Release(IB)");
                RELEASE(*ppIB);

                szLastCall = TEXT("CreateIndexBuffer()");
                hr = pDevice->CreateIndexBuffer(
                    (m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
                    Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                    D3DFMT_INDEX16,
                    Pool,
                    ppIB
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
                        (m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
                        Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                        D3DFMT_INDEX16,
                        Pool,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("IB->Lock()");
                hr = (*ppIB)->Lock(
                    0,
                    m_shape.m_nIndices * sizeof(WORD),
                    &pbObject,
                    LockFlags
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Lock(0,%d,,0x%x) returned %s (0x%x)\n"),
                        m_shape.m_nIndices * sizeof(WORD),
                        LockFlags,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("CopyMemory(,IB,)");
                CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

                szLastCall = TEXT("IB->Unlock()");
                hr = (*ppIB)->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Unlock() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("SetIndices()");
                hr = pDevice->SetIndices(
                    *ppIB,
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
            }


            if( Test->fAppendVB )  // add second object to vb
            {
                szLastCall = TEXT("VB->Lock()");
                hr = (*ppVB)->Lock(
                    m_shape.m_nVertices * sizeof(D3DVERTEX),
                    m_shape2.m_nVertices * sizeof(D3DVERTEX),
                    &pbObject,
                    LockFlags
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Lock(%d,%d,,0x%x) returned %s (0x%x)\n"),
                        m_shape.m_nVertices * sizeof(D3DVERTEX),
                        m_shape2.m_nVertices * sizeof(D3DVERTEX),
                        LockFlags,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("CopyMemory(,VB,)");
                CopyMemory( pbObject, m_shape2.m_pVertices, m_shape2.m_nVertices * sizeof(D3DVERTEX) );

                szLastCall = TEXT("VB->Unlock()");
                hr = (*ppVB)->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("VB::Unlock() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }
            }


            if( Test->fAppendIB )  // add second object to ib
            {
                szLastCall = TEXT("IB->Lock()");
                hr = (*ppIB)->Lock(
                    m_shape.m_nIndices * sizeof(WORD),
                    m_shape2.m_nIndices * sizeof(WORD),
                    &pbObject,
                    LockFlags
                );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Lock(%d,%d,,0x%x) returned %s (0x%x)\n"),
                        m_shape.m_nIndices * sizeof(WORD),
                        m_shape2.m_nIndices * sizeof(WORD),
                        LockFlags,
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                szLastCall = TEXT("IB mem copy");
                for( UINT iIndex = 0; iIndex < (UINT)m_shape2.m_nIndices; iIndex++ )
                {
                    ((WORD*)pbObject)[iIndex] = m_shape2.m_pIndices[iIndex] + m_shape.m_nVertices;
                }

                szLastCall = TEXT("IB->Unlock()");
                hr = (*ppIB)->Unlock();
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("IB::Unlock() returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }
            }
        }
/*
        catch(...)
        {
            WriteToLog(
                TEXT("%s caused unhandled exception\n"),
                szLastCall
            );
            fRet = FALSE;
        }
*/
    }
    while(0);

    if( !fRet )
    {
        return(fRet);
    }
 

    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
        do
        {
//            try
            {
                szLastCall = TEXT("SetVertexShader()");
                hr = pDevice->SetVertexShader( D3DFVF_VERTEX );
                if( FAILED(hr) )
                {
                    WriteToLog(
                        TEXT("SetVertexShader(D3DFVF_VERTEX) returned %s (0x%x)\n"),
                        DecodeHResult(hr),
                        hr
                    );
                    fRet = FALSE;
                    continue;
                }

                if( Test->fDraw1 )  // draw first opject
                {
                    szLastCall = TEXT("DrawIndexedPrimitive(1)");
                    hr = pDevice->DrawIndexedPrimitive(
                        D3DPT_TRIANGLELIST,
                        0,
                        m_shape.m_nVertices,
                        0,
                        m_shape.m_nIndices / 3
                    );
                    if( FAILED(hr) )
                    {
                        WriteToLog(
                            TEXT("DrawIndexedPrimitive() returned %s (0x%x)\n"),
                            DecodeHResult(hr),
                            hr
                        );
                        fRet = FALSE;
                        continue;
                    }
                }

                if( Test->fDraw2 )  // draw second object
                {
                    szLastCall = TEXT("DrawIndexedPrimitive(2)");
                    hr = pDevice->DrawIndexedPrimitive(
                        D3DPT_TRIANGLELIST,
                        m_shape.m_nVertices,
                        m_shape2.m_nVertices,
                        m_shape.m_nIndices,
                        m_shape2.m_nIndices / 3
                    );
                    if( FAILED(hr) )
                    {
                        WriteToLog(
                            TEXT("DrawIndexedPrimitive() returned %s (0x%x)\n"),
                            DecodeHResult(hr),
                            hr
                        );
                        fRet = FALSE;
                        continue;
                    }
                }
            }
/*
            catch(...)
            {
                WriteToLog(
                    TEXT("%s caused unhandled exception\n"),
                    szLastCall
                );
                fRet = FALSE;
            }
*/
        }
        while(0);  // execute once, so continue can bust out

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


    return(fRet);
}

