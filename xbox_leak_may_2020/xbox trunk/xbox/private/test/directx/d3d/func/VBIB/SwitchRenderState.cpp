//
//  SwitchRenderState.cpp
//
//  SwitchRenderState rendering tests.
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

SwitchRenderState::SwitchRenderState()
{
    m_szTestName = _T("SwitchRenderState");
    m_szCommandKey = _T("SwitchRenderState");
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT SwitchRenderState::TestInitialize()
{

    m_pD3D =  m_pDisplay->GetDirect3D();
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


    // skip devices that aren't mixed mode
    if( !(m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
    {
        return( D3DTESTINIT_SKIPALL );
    }


    // create the object
    if (!m_shape.NewShape( CS_SPHERE, 10 )) {
        return D3DTESTINIT_ABORT;
    }


    // init the devices
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
SwitchRenderState::ExecuteTest
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


    static const D3DTEST_NORMALBUFFER TestStages[] =
    {
    //  { szName, shape, VBFVF, IBFormat, Usage, Pool },

        { TEXT("FVF = VERTEX, FMT = INDEX16"), NULL, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_DEFAULT },
    };


    typedef union __TESTCASE
    {
        struct
        {
            UINT TestStage : 3;
            UINT Pool : 3;
            UINT Usage : 5;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(TestStages),
        COUNTOF(Pools),
        COUNTOF(Usages),
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
                for( tc.TestStage = 0; tc.TestStage < tcCount.TestStage; tc.TestStage++ )
                {
                    AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
                }
            }
        }

        return( FALSE );
    }

    if( (tc.Usage >= tcCount.Usage) ||
        (tc.Pool >= tcCount.Pool) ||
        (tc.TestStage >= tcCount.TestStage)
    )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }


    D3DTEST_NORMALBUFFER curtest;

    curtest.szName = TestStages[tc.TestStage].szName;
    curtest.VBFVF = TestStages[tc.TestStage].VBFVF;
    curtest.IBFormat = TestStages[tc.TestStage].IBFormat;
    curtest.Usage = Usages[tc.Usage];
    curtest.Pool = Pools[tc.Pool];


    if( (Usages[tc.Usage] & D3DUSAGE_DYNAMIC) && (Pools[tc.Pool] == D3DPOOL_MANAGED) )
    {
        return(FALSE);
    }

    ClearFrame();


    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d, %s"),
        Usages[tc.Usage],
        Pools[tc.Pool],
        TestStages[tc.TestStage].szName
    );

    BeginTestCase( sz, nTest );


    BOOL bRetTest;
    BOOL bRetRef;

    bRetTest = ExecuteReal(
        m_pDevice,
        &curtest,
        FALSE
    );

    bRetRef = ExecuteReal(
        m_pRefDevice,
        &curtest,
        TRUE
    );


    bool bRet = bRetTest && bRetRef;
    if( !bRet )
    {
        Fail();
    }

    return( bRet );
}



UINT
SwitchRenderState::TestInitReal
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
    pDevice->SetRenderState( D3DRS_CLIPPING, FALSE );  // so donotclip buffers won't die in process vertices

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
SwitchRenderState::ExecuteReal
(
    LPDIRECT3DDEVICE8 pDevice,
    const D3DTEST_NORMALBUFFER *curtest,
    BOOL fReference
)
{

    HRESULT hr = D3D_OK;
    LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
    LPDIRECT3DINDEXBUFFER8 pIB = NULL;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;
    DWORD dwSWVertProc;
    UINT iMemFreeStart;
    UINT iMemFreeEnd;


    if( !fReference )
    {
        pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSWVertProc );

        iMemFreeStart = GetAvailMemory( curtest->Pool );
    }

    do
    {
        hr = pDevice->CreateVertexBuffer(
            m_shape.m_nVertices * sizeof(D3DVERTEX),
            curtest->Usage | (fReference ? 0 : D3DUSAGE_SOFTWAREPROCESSING),
            D3DFVF_VERTEX,
            curtest->Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                m_shape.m_nVertices * sizeof(D3DVERTEX),
                curtest->Usage | (fReference ? 0 : D3DUSAGE_SOFTWAREPROCESSING),
                D3DFVF_VERTEX,
                curtest->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pVB->Lock(
            0,
            m_shape.m_nVertices * sizeof(D3DVERTEX),
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

        CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

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


        hr = pDevice->CreateIndexBuffer(
            m_shape.m_nIndices * sizeof(WORD),
            curtest->Usage | (fReference ? 0 : D3DUSAGE_SOFTWAREPROCESSING),
            curtest->IBFormat,
            curtest->Pool,
            &pIB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
                m_shape.m_nIndices * sizeof(WORD),
                curtest->Usage | (fReference ? 0 : D3DUSAGE_SOFTWAREPROCESSING),
                curtest->IBFormat,
                curtest->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pIB->Lock(
            0,
            m_shape.m_nIndices * sizeof(WORD),
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

        CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

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


        hr = pDevice->SetStreamSource(
            0,
            pVB,
            sizeof(D3DVERTEX)
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetStreamSource(pVB) returned %s (0x%x)\n"),
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

        if( !fReference )
        {
            // switch render state
            hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, !dwSWVertProc );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, %d ) returned %s (0x%x)\n"),
                    !dwSWVertProc,
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }


            // set stream source
            LPDIRECT3DVERTEXBUFFER8 pVBRet = NULL;
            UINT iStride;

            // get and compare
            hr = pDevice->GetStreamSource(
                0,
                &pVBRet,
                &iStride
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("GetStreamSource() returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
            }
            else if( pVBRet != NULL )
            {
                RELEASE(pVBRet);
                WriteToLog(TEXT("GetStreamSource() didn't return NULL pointer after renderstate change\n"));
                fRet = FALSE;
            }

            hr = pDevice->SetStreamSource(
                0,
                pVB,
                sizeof(D3DVERTEX)
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("SetStreamSource(pVB) returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
                continue;
            }


            // set indices
            LPDIRECT3DINDEXBUFFER8 pIBRet = NULL;
            UINT iBaseIndex = 0;

            // get and compare
            hr = pDevice->GetIndices(
                &pIBRet,
                &iBaseIndex
            );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("GetIndices() returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
            }
            else if( pIBRet != NULL )
            {
                RELEASE(pIBRet);
                WriteToLog(TEXT("GetIndices() didn't return NULL pointer after renderstate change\n"));
                fRet = FALSE;
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

            // set vertex shader
            DWORD dwShader;

            // get and compare
            hr = pDevice->GetVertexShader( &dwShader );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("GetVertexShader() returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
                fRet = FALSE;
            }
            else if( dwShader != NULL )
            {
                WriteToLog(TEXT("GetVertexShader() didn't return empty handle after renderstate change\n"));
                fRet = FALSE;
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
        }

    } while(0);


    if( !fRet )
    {
        RELEASE(pIB);
        RELEASE(pVB);

        if( !fReference )
        {
            hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc ) returned %s (0x%x)\n"),
                    DecodeHResult(hr),
                    hr
                );
            }
        }

        return(fRet);
    }

    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
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

    if( !fReference )
    {
        hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc ) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
        }


        // get avail memory, compare to start
        iMemFreeEnd = GetAvailMemory( curtest->Pool );

        if( iMemFreeEnd < iMemFreeStart )
        {
            WriteToLog(
                TEXT("memory leaked: %d\n"),
                iMemFreeStart - iMemFreeEnd
            );
            fRet = FALSE;
        }
    }


    return(fRet);
}

