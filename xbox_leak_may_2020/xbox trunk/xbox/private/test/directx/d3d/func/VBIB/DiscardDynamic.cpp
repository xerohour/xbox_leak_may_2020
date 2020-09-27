//
//  DiscardDynamic.cpp
//

#include "CVertexIndexBufferTest.h"
#include <d3d8.h>
#include <d3dx8.h>



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

DiscardDynamic::DiscardDynamic()
{
    m_szTestName = _T("Discard Dynamic");
    m_szCommandKey = _T("DiscardDynamic");
}

DiscardDynamic::~DiscardDynamic()
{
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT DiscardDynamic::TestInitialize()
{

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

    uiRet = TestInitReal(
        m_pRefDevice
    );
    if( uiRet != D3DTESTINIT_RUN )
    {
        return( uiRet );
    }

    return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool
DiscardDynamic::ExecuteTest
(
    UINT nTest
)
{

    static const UINT Usages[] =
    {
        D3DUSAGE_DYNAMIC,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        D3DUSAGE_DYNAMIC | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS,
        D3DUSAGE_DYNAMIC | D3DUSAGE_RTPATCHES,
        D3DUSAGE_DYNAMIC | D3DUSAGE_NPATCHES,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_DYNAMIC | D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_DYNAMIC | D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
    };

    static const D3DPOOL Pools[] =
    {
        D3DPOOL_DEFAULT,
        D3DPOOL_SYSTEMMEM,
    };

    static const UINT LockFlags[] =
    {
        NULL,
        D3DLOCK_DISCARD,
        D3DLOCK_NOSYSLOCK,
        D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK,
    };


    typedef union __TESTCASE
    {
        struct
        {
            UINT LockFlag2 : 4;
            UINT LockFlag1 : 4;
            UINT Pool : 3;
            UINT Usage : 5;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(LockFlags),
        COUNTOF(LockFlags),
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
                for( tc.LockFlag1 = 0; tc.LockFlag1 < tcCount.LockFlag1; tc.LockFlag1++ )
                {
                    for( tc.LockFlag2 = 0; tc.LockFlag2 < tcCount.LockFlag2; tc.LockFlag2++ )
                    {
                        AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
                    }
                }
            }
        }

        return( FALSE );
    }

    if(
        (tc.Usage >= tcCount.Usage) ||
        (tc.Pool >= tcCount.Pool) ||
        (tc.LockFlag1 >= tcCount.LockFlag1) ||
        (tc.LockFlag2 >= tcCount.LockFlag2)
    )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }


    ClearFrame();


    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x, 0x%x"),
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag1],
        LockFlags[tc.LockFlag2]
    );

    BeginTestCase( sz, nTest );


    BOOL bRetTest;
    BOOL bRetRef;

    bRetTest = ExecuteReal(
        m_pDevice,
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag1],
        LockFlags[tc.LockFlag2],
        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );

    bRetRef = ExecuteReal(
        m_pRefDevice,
        Usages[tc.Usage],
        Pools[tc.Pool],
        LockFlags[tc.LockFlag1],
        LockFlags[tc.LockFlag2],
        (m_pAdapter->Devices[m_pMode->nRefDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );


    bool bRet = bRetTest && bRetRef;
    if( !bRet )
    {
        Fail();
    }

    return( bRet );
}



UINT
DiscardDynamic::TestInitReal
(
    LPDIRECT3DDEVICE8 pDevice
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
DiscardDynamic::ExecuteReal
(
    LPDIRECT3DDEVICE8 pDevice,
    DWORD Usage,
    D3DPOOL Pool,
    DWORD LockFlags1,
    DWORD LockFlags2,
    BOOL fMixedMode
)
{

    HRESULT hr = D3D_OK;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;
    LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
    LPDIRECT3DINDEXBUFFER8 pIB = NULL;


    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );

    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
        do
        {
            BOOL fSoftwareUsage = FALSE;

            // determine sw usage
            if( fMixedMode )
            {
                DWORD dwSW;
                pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
                fSoftwareUsage = dwSW;
            }

            // create buffers
            hr = pDevice->CreateVertexBuffer(
                (m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
                Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                D3DFVF_VERTEX,
                Pool,
                &pVB
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

            hr = pDevice->CreateIndexBuffer(
                (m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
                Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
                D3DFMT_INDEX16,
                Pool,
                &pIB
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


            // setup streams
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
                    TEXT("SetVertexShader(D3DFVF_VERTEX) returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }


            // first vb lock
            hr = pVB->Lock(
                0,
                m_shape.m_nVertices * sizeof(D3DVERTEX),
                &pbObject,
                LockFlags1
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("VB::Lock(0,%d,,0x%x) returned %s (0x%x)\n"),
                    m_shape.m_nVertices * sizeof(D3DVERTEX),
                    LockFlags1,
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

            hr = pVB->Unlock();
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


            // first ib lock
            hr = pIB->Lock(
                0,
                m_shape.m_nIndices * sizeof(WORD),
                &pbObject,
                LockFlags1
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("IB::Lock(0,%d,,0x%x) returned %s (0x%x)\n"),
                    m_shape.m_nIndices * sizeof(WORD),
                    LockFlags1,
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

            hr = pIB->Unlock();
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


            // draw first object
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


            // second vb lock
            hr = pVB->Lock(
                0,
                m_shape2.m_nVertices * sizeof(D3DVERTEX),
                &pbObject,
                LockFlags2
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("VB::Lock(%d,%d,,0x%x) returned %s (0x%x)\n"),
                    0,
                    m_shape2.m_nVertices * sizeof(D3DVERTEX),
                    LockFlags2,
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            CopyMemory( pbObject, m_shape2.m_pVertices, m_shape2.m_nVertices * sizeof(D3DVERTEX) );

            hr = pVB->Unlock();
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


            // second ib lock
            hr = pIB->Lock(
                0,
                m_shape2.m_nIndices * sizeof(WORD),
                &pbObject,
                LockFlags2
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("IB::Lock(%d,%d,,0x%x) returned %s (0x%x)\n"),
                    0,
                    m_shape2.m_nIndices * sizeof(WORD),
                    LockFlags2,
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }

            CopyMemory( pbObject, m_shape2.m_pIndices, m_shape2.m_nIndices * sizeof(WORD) );

            hr = pIB->Unlock();
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


            // draw second object
            hr = pDevice->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                m_shape2.m_nVertices,
                0,
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
        while(0);  // execute once, so continue can bust out


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

        RELEASE(pVB);
        RELEASE(pIB);
    }


    return(fRet);
}

