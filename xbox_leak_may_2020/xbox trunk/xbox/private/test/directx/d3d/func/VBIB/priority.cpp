//
//  Priority.cpp
//
//  priority rendering tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

#include "CVertexIndexBufferTest.h"
#include <d3d8.h>
#include <d3dx8.h>



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

Priority::Priority()
{
    m_szTestName = _T("Priority");
    m_szCommandKey = _T("Priority");
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT Priority::TestInitialize()
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


    // skip devices that don't allow SW VP
    if( (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_PUREDEVICE) ||
        (m_pAdapter->Devices[m_pMode->nRefDevice].dwBehaviorFlags & D3DCREATE_PUREDEVICE) ||
        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
        (m_pAdapter->Devices[m_pMode->nRefDevice].dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) )
    {
        return( D3DTESTINIT_SKIPALL );
    }


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
Priority::ExecuteTest
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
        D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
#ifdef D3DUSAGE_LOADONCE
        D3DUSAGE_LOADONCE,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY,
        D3DUSAGE_LOADONCE | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_LOADONCE | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
#endif
    };

    static const DWORD Priorities[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0xff, 0x100, 0xffff, 0x10000, 0xffffffff };


    typedef union __TESTCASE
    {
        struct
        {
            UINT Usage : 5;
            UINT Priority : 5;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(Usages),
        COUNTOF(Priorities),  // priorities
    };


    TESTCASE tc;
    tc.nTest = (nTest - 1);

    if( nTest == (UINT)(-1) )
    {
        ClearRangeList();

        tc.nTest = 0;
        for( tc.Usage = 0; tc.Usage < tcCount.Usage; tc.Usage++ )
        {
            for( tc.Priority = 0; tc.Priority < tcCount.Priority; tc.Priority++ )
            {
                AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
            }
        }

        return( FALSE );
    }

    if( (tc.Usage >= tcCount.Usage) ||
        (tc.Priority >= tcCount.Priority)  )
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
        TEXT("Usage = 0x%x, Priority = %d"),
        Usages[tc.Usage],
        Priorities[tc.Priority]
    );

    BeginTestCase( sz, nTest );

    D3DTEST_NORMALBUFFER curtest;

    curtest.szName = TEXT("");
    curtest.shape = &m_shape;
    curtest.VBFVF = D3DFVF_VERTEX;
    curtest.IBFormat = D3DFMT_INDEX16;
    curtest.Usage = Usages[tc.Usage];
    curtest.Pool = D3DPOOL_MANAGED;


    BOOL bRetTest;
    BOOL bRetRef;

    bRetTest = ExecuteReal(
        m_pDevice,
        &curtest,
        Priorities[tc.Priority],
        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );

    bRetRef = ExecuteReal(
        m_pRefDevice,
        &curtest,
        Priorities[tc.Priority],
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
Priority::TestInitReal
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
Priority::ExecuteReal
(
    LPDIRECT3DDEVICE8 pDevice,
    const D3DTEST_NORMALBUFFER *lpBufInfo,
    DWORD dwThisPriority,
    BOOL fMixedMode
)
{

    HRESULT hr = D3D_OK;
    LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVBTL = NULL;
    LPDIRECT3DINDEXBUFFER8 pIB = NULL;
    LPBYTE pbObject = NULL;
    BOOL fRet = TRUE;

    BOOL fSoftwareUsage = FALSE;
    DWORD dwSWVertProc = TRUE;


    // determine sw usage
    if( fMixedMode )
    {
        pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSWVertProc );
        fSoftwareUsage = dwSWVertProc;
    }

    do
    {
        // create D3DVERTEX buffer
        hr = pDevice->CreateVertexBuffer(
            lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
            //lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->Usage ^ (fMixedMode ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            D3DFVF_VERTEX,
            lpBufInfo->Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                D3DFVF_VERTEX,
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


        // create index buffer
        hr = pDevice->CreateIndexBuffer(
            lpBufInfo->shape->m_nIndices * sizeof(WORD),
            //lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->Usage ^ (fMixedMode ? D3DUSAGE_SOFTWAREPROCESSING : 0),
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


        // create D3DTLVERTEX buffer and process vertices
        hr = pDevice->CreateVertexBuffer(
            lpBufInfo->shape->m_nVertices * sizeof(D3DTLVERTEX),
            lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            D3DFVF_TLVERTEX,
            lpBufInfo->Pool,
            &pVBTL
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                lpBufInfo->shape->m_nVertices * sizeof(D3DTLVERTEX),
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                D3DFVF_TLVERTEX,
                lpBufInfo->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE ) returned %s (0x%x)\n"),
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

        hr = pDevice->ProcessVertices(
            0,
            0,
            lpBufInfo->shape->m_nVertices,
            pVBTL,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("ProcessVertices() failed, returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

        hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, dwSWVertProc ) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }


        // setup streams and shader for rendering
        hr = pDevice->SetStreamSource(
            0,
            pVBTL,
            sizeof(D3DTLVERTEX)
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetStreamSource(pVBTL) returned %s (0x%x)\n"),
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

        hr = pDevice->SetVertexShader( D3DFVF_TLVERTEX );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetVertexShader(D3DFVF_TLVERTEX) returned %s (0x%x)\n"),
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
        RELEASE(pVBTL);
        return(fRet);
    }

    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
        do
        {
            DWORD dwOldPriority;
            DWORD dwTemp;


            dwOldPriority = pVBTL->SetPriority( dwThisPriority );

            dwTemp = pVBTL->GetPriority();
            if( dwTemp != dwThisPriority )
            {
                WriteToLog(
                    TEXT("GetPriority() returned %d, expected %d\n"),
                    dwTemp,
                    dwThisPriority
                );
                fRet = FALSE;
            }

            hr = pDevice->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                lpBufInfo->shape->m_nVertices,
                0,
                lpBufInfo->shape->m_nIndices / 3
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

            dwTemp = pVBTL->SetPriority( dwOldPriority );
            if( dwTemp != dwThisPriority )
            {
                WriteToLog(
                    TEXT("SetPriority() returned %d, expected %d\n"),
                    dwTemp,
                    dwThisPriority
                );
                fRet = FALSE;
            }

            dwTemp = pVBTL->SetPriority( dwThisPriority );
            if( dwTemp != dwOldPriority )
            {
                WriteToLog(
                    TEXT("SetPriority() returned %d, expected %d\n"),
                    dwTemp,
                    dwOldPriority
                );
                fRet = FALSE;
            }

            dwTemp = pVBTL->GetPriority();
            if( dwTemp != dwThisPriority )
            {
                WriteToLog(
                    TEXT("GetPriority() returned %d, expected %d\n"),
                    dwTemp,
                    dwThisPriority
                );
                fRet = FALSE;
            }

            dwTemp = pVBTL->SetPriority( dwOldPriority );

        } while(0);  // execute once, so continue can bust out

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
    RELEASE(pVBTL);

    return(fRet);
}

