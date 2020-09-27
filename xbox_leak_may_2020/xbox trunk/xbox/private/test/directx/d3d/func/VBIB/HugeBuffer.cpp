//
//  HugeBuffer.cpp
//
//  tests for optimized buffers.
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

HugeBuffer::HugeBuffer()
{
    m_szTestName = _T("Huge Buffer");
    m_szCommandKey = _T("HugeBuffer");
    m_MemSize = 0;
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT HugeBuffer::TestInitialize()
{

    m_pD3D = m_pDisplay->GetDirect3D8();
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


    if (!m_shape15bit.NewShape( CS_SPHERE, 127 )) {
        return D3DTESTINIT_ABORT;
    }

    if (!m_shape16bit.NewShape( CS_SPHERE, 128 )) {
        return D3DTESTINIT_ABORT;
    }

    if (!m_shape.NewShape( CS_SPHERE, 181 )) {
        return D3DTESTINIT_ABORT;
    }


    // get amount of system memory
    MEMORYSTATUS memstat;
    GlobalMemoryStatus(&memstat);
    m_MemSize = memstat.dwTotalPhys;


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
HugeBuffer::ExecuteTest
(
    UINT nTest
)
{
    D3DTEST_HUGEBUFFER HugeBufferTests[] =
    {
    //  { szName, shape, VBFVF, IBFormat, Usage, Pool },

        { TEXT("Pool = DEFAULT"), &m_shape15bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, POOL_DEFAULT },
//        { TEXT("Pool = DEFAULT"), &m_shape15bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_DEFAULT },
//        { TEXT("Pool = MANAGED"), &m_shape15bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_MANAGED },
//        { TEXT("Pool = SYSTEMMEM"), &m_shape15bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape16bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, POOL_DEFAULT },
//        { TEXT("Pool = DEFAULT"), &m_shape16bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_DEFAULT },
//        { TEXT("Pool = MANAGED"), &m_shape16bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_MANAGED },
//        { TEXT("Pool = SYSTEMMEM"), &m_shape16bit, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, POOL_DEFAULT },
//        { TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_DEFAULT },
//        { TEXT("Pool = MANAGED"), &m_shape, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_MANAGED },
//        { TEXT("Pool = SYSTEMMEM"), &m_shape, 1, D3DFVF_VERTEX, D3DFMT_INDEX16, NULL, D3DPOOL_SYSTEMMEM },

//        { TEXT("Pool = DEFAULT"), &m_shape, 10, D3DFVF_VERTEX, D3DFMT_INDEX32, NULL, D3DPOOL_DEFAULT },
//        { TEXT("Pool = MANAGED"), &m_shape, 10, D3DFVF_VERTEX, D3DFMT_INDEX32, NULL, D3DPOOL_MANAGED },
//        { TEXT("Pool = SYSTEMMEM"), &m_shape, 10, D3DFVF_VERTEX, D3DFMT_INDEX32, NULL, D3DPOOL_SYSTEMMEM },
    };
    static const UINT iHugeBufferTests = COUNTOF(HugeBufferTests);


    static const UINT iNumTests = iHugeBufferTests;

    if( nTest == (UINT)(-1) )
    {
        SetTestRange( 1, iNumTests );
        return( FALSE );
    }

    if( nTest > iNumTests )
    {
        return( FALSE );
    }

    ClearFrame();

    const D3DTEST_HUGEBUFFER *curtest = &HugeBufferTests[nTest-1];


    UINT NumVertices = curtest->shape->m_nVertices * curtest->iReps;
    UINT NumPrims = (curtest->shape->m_nIndices * curtest->iReps) / 3;

    // skip test if number of vertices > maxindex
    if(
        (NumVertices > m_d3dcaps.MaxVertexIndex) ||
        (NumVertices > m_d3dcaps.MaxVertexIndex)
    )
    {
        WriteToLog(
            TEXT("skipping test %d: NumVertices (%x) greater than supported by device (%x,%x)\n"),
            nTest,
            NumVertices,
            m_d3dcaps.MaxVertexIndex,
            m_d3dcaps.MaxVertexIndex
        );
        return(FALSE);
    }

    // skip test if number of prim > maxprim
    if(
        (NumPrims > m_d3dcaps.MaxPrimitiveCount) ||
        (NumPrims > m_d3dcaps.MaxPrimitiveCount)
    )
    {
        WriteToLog(
            TEXT("skipping test %d: NumPrimitives (%x) greater than supported by device (%x,%x)\n"),
            nTest,
            NumPrims,
            m_d3dcaps.MaxPrimitiveCount,
            m_d3dcaps.MaxPrimitiveCount
        );
        return(FALSE);
    }

    // skip larger shapes if not enough memory
    if( (NumVertices > 500000) && (m_MemSize < 0x10000000 /*256M*/) )
    {
        return(FALSE);
    }


    TCHAR sz[MAX_PATH];
    wsprintf( sz,
        TEXT("%d vertices, %d indices: %s"),
        curtest->shape->m_nVertices * curtest->iReps,
        curtest->shape->m_nIndices * curtest->iReps,
        curtest->szName
    );

    BeginTestCase( sz, nTest );


    BOOL bSrcRet;
    BOOL bRefRet;


    bSrcRet = ExecuteReal(
        m_pDevice,
        curtest,
        false
//        (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    );

//    bRefRet = ExecuteReal(
//        m_pRefDevice,
//        curtest,
//        (m_pAdapter->Devices[m_pMode->nRefDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
//    );


    bool bRet = (bSrcRet == TRUE);// && bRefRet;

    if( !bRet )
    {
        Fail();
    }

    return( bRet );
}



UINT
HugeBuffer::TestInitReal
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
HugeBuffer::ExecuteReal
(
    CDevice8* pDevice,
    const D3DTEST_HUGEBUFFER *lpBufInfo,
    BOOL fMixedMode
)
{

    HRESULT hr = D3D_OK;
    CVertexBuffer8* pVB = NULL;
    CIndexBuffer8* pIB = NULL;
    LPBYTE pVertices = NULL;
    LPBYTE pIndices = NULL;
    BOOL fRet = TRUE;

    BOOL fSoftwareUsage = FALSE;


    // determine sw usage
    if( fMixedMode )
    {
//        DWORD dwSW;
//        pDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
//        fSoftwareUsage = dwSW;
    }

    UINT nVertices = lpBufInfo->iReps * lpBufInfo->shape->m_nVertices;
    UINT nIndices = lpBufInfo->iReps * lpBufInfo->shape->m_nIndices;

    if( ((nVertices > (UINT)0xffff) && (lpBufInfo->IBFormat == D3DFMT_INDEX16)) )
    {
        WriteToLog(
            TEXT("nVertices (%d) is greater than WORD index can hold, image may look strange\n"),
            nVertices
        );
    }

    do
    {
        hr = pDevice->CreateVertexBuffer(
            nVertices * sizeof(D3DVERTEX),
            lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->VBFVF,
            lpBufInfo->Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                nVertices * sizeof(D3DVERTEX),
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                lpBufInfo->VBFVF,
                lpBufInfo->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }


        hr = pDevice->CreateIndexBuffer(
#ifndef UNDER_XBOX
            nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
#else
            nIndices * sizeof(WORD),
#endif
            lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
            lpBufInfo->IBFormat,
            lpBufInfo->Pool,
            &pIB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
#ifndef UNDER_XBOX
                nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
#else
                nIndices * sizeof(WORD),
#endif
                lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
                lpBufInfo->IBFormat,
                lpBufInfo->Pool,
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }


        hr = pVB->Lock(
            0,
            nVertices * sizeof(D3DVERTEX),
            &pVertices,
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
            break;
        }

        hr = pIB->Lock(
            0,
#ifndef UNDER_XBOX
            nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
#else
            nIndices * sizeof(WORD),
#endif
            &pIndices,
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
            break;
        }

        for(
            UINT iVertStart = 0, iIndexStart = 0;
            ((iVertStart + lpBufInfo->shape->m_nVertices) <= nVertices) && ((iIndexStart + lpBufInfo->shape->m_nIndices) <= nIndices);
            iVertStart += lpBufInfo->shape->m_nVertices, iIndexStart += lpBufInfo->shape->m_nIndices
        )
        {
            UINT iTemp;

            for( iTemp = iVertStart; iTemp < (UINT)lpBufInfo->shape->m_nVertices; iTemp++ )
            {
                ((D3DVERTEX*)pVertices)[iTemp] = lpBufInfo->shape->m_pVertices[iTemp];
            }

            switch( lpBufInfo->IBFormat)
            {
                case D3DFMT_INDEX16:
                    for( iTemp = iIndexStart; iTemp < (UINT)lpBufInfo->shape->m_nIndices; iTemp++ )
                    {
                        ((WORD*)pIndices)[iTemp] = (WORD)lpBufInfo->shape->m_pIndices[iTemp] + (WORD)iVertStart;
                    }
                    break;

//                case D3DFMT_INDEX32:
//                    for( UINT iTemp = iIndexStart; iTemp < lpBufInfo->shape->m_nIndices; iTemp++ )
//                    {
//                        ((DWORD*)pIndices)[iTemp] = (DWORD)lpBufInfo->shape->m_pIndices[iTemp] + (DWORD)iVertStart;
//                    }
//                    break;

                default:
                    break;
            }
        }

        hr = pVB->Unlock();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Unlock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            break;
        }

        hr = pIB->Unlock();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Unlock() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            break;
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
        Fail();
        return(fRet);
    }

    hr = pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );
    if( FAILED(hr) )
    {
        WriteToLog(
            TEXT("Clear() returned %s (0x%x)\n"),
            DecodeHResult(hr),
            hr
        );
        fRet = FALSE;
    }

    do
    {
        hr = pDevice->BeginScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("BeginScene() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fRet = FALSE;
            continue;
        }

#ifdef UNDER_XBOX
        if (!(GetStartupContext() & TSTART_STRESS)) {
#endif
            hr = pDevice->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                nVertices,
                0,
                nIndices / 3
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
#ifdef UNDER_XBOX
        }
        else {
            pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nVertices, 0, nIndices / 12);
            pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nVertices, nIndices / 4, nIndices / 12);
            pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nVertices, nIndices / 2, nIndices / 12);
            pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nVertices, 3 * nIndices / 4, nIndices / 12);
        }
#endif
    }
    while(0);

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

    RELEASE(pIB);
    RELEASE(pVB);

    return(fRet);
}

