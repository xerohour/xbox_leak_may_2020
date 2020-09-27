//
//  LockPointerAlias8.cpp
//



#include <d3dx8.h>
#include <tchar.h>
#include <windows.h>
#include <types.h>
#include "LockPointerAlias.h"



UINT LockPointerAlias::TestInitialize8()
{
    if( !m_pD3D8 || !m_pSrcDevice8 )
	{
		WriteToLog(_T("TestInitialize() - Framework devices == NULL\n"));
        return D3DTESTINIT_ABORT;
	}

    ExecuteTest8((UINT)(-1));

    return(D3DTESTINIT_RUN);
}



bool
LockPointerAlias::ExecuteTest8
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
        D3DUSAGE_SOFTWAREPROCESSING,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_RTPATCHES,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_NPATCHES,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DYNAMIC,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_RTPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_NPATCHES | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
        D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC,
    };

    static const D3DPOOL Pools[] =
    {
        D3DPOOL_DEFAULT,
        D3DPOOL_MANAGED,
        D3DPOOL_SYSTEMMEM,
    };

    static const DWORD LockFlags[] =
    {
        D3DLOCK_DISCARD,
        D3DLOCK_NOSYSLOCK,
        D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK,
    };

    typedef union __TESTCASE
    {
        struct
        {
            UINT LockFlag : 4;
            UINT Pool : 3;
            UINT Usage : 6;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(LockFlags),
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
                for( tc.LockFlag = 0; tc.LockFlag < tcCount.LockFlag; tc.LockFlag++ )
                {
                    AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
                }
            }
        }

        return( FALSE );
    }

    if(
        (tc.Usage >= tcCount.Usage) ||
        (tc.Pool >= tcCount.Pool) ||
        (tc.LockFlag >= tcCount.LockFlag)
    )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }


    static const UINT Size = 4096;
    static const DWORD FVF = D3DFVF_VERTEX;

    UINT Usage = Usages[tc.Usage];
    D3DPOOL Pool = Pools[tc.Pool];
    DWORD Flags = LockFlags[tc.LockFlag];

    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x"),
        Usage,
        Pool,
        Flags
    );


    // skip buffer if not valid
    DWORD dwSW;
    if( m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
    {
        m_pSrcDevice8->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW );
    }

    if(
        (  // dynamic is not valid on managed buffers
            (Usage & D3DUSAGE_DYNAMIC) &&
            (Pool == D3DPOOL_MANAGED)
        ) ||
        (  // discard is only valid for dynamic buffers
            !(Usage & D3DUSAGE_DYNAMIC) &&
            (Flags & D3DLOCK_DISCARD)
        ) ||
        (  // software usage is not valid on hardware device
            (Usage & D3DUSAGE_SOFTWAREPROCESSING) &&
            (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
        ) ||
        (  // software usage is required on mixed device with SW renderstate when pool is not sysmem
            !(Usage & D3DUSAGE_SOFTWAREPROCESSING) &&
            (m_pAdapter->Devices[m_pMode->nSrcDevice].dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) &&
            dwSW &&
            (Pool != D3DPOOL_SYSTEMMEM)
        )
    )
    {
        return(FALSE);
    }


    ClearFrame();

    BeginTestCase( sz, nTest );


    BOOL fFail = FALSE;
    HRESULT hr;
    LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
    LPDIRECT3DINDEXBUFFER8 pIB = NULL;
    LPBYTE pbVB = NULL;
    LPBYTE pbIB = NULL;

    LONG lRet;
    DEVMODE dmnew;
    DEVMODE dmcur;


    dmnew.dmSize = sizeof(dmnew);
    dmnew.dmPelsWidth = 640;
    dmnew.dmPelsHeight = 480;
    dmnew.dmBitsPerPel = 8;
    dmnew.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    dmcur.dmSize = sizeof(dmcur);


    do
    {
        lRet = EnumDisplaySettings(
            NULL,
            ENUM_CURRENT_SETTINGS,
            &dmcur
        );
        if( !lRet )
        {
            WriteToLog(
                TEXT("EnumDisplaySettings() returned %d\n"),
                lRet
            );
            fFail = TRUE;
            continue;
        }


        hr = m_pSrcDevice8->CreateVertexBuffer(
            Size,
            Usage,
            FVF,
            Pool,
            &pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
                Size,
                Usage,
                FVF,
                Pool,
                DecodeHResult(hr),
                hr
            );
            fFail = TRUE;
            continue;
        }

        hr = pVB->Lock( 0, 0, &pbVB, Flags );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Lock() failed, returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            fFail = TRUE;
            continue;
        }

        // write to buffer
        try
        {
            for( INT iIndex = 0; iIndex < Size; iIndex++ )
            {
                pbVB[iIndex] = 0x49;
            }
        }
        catch(...)
        {
            WriteToLog(
                TEXT("writing to pointer from Lock() caused unhandled exception\n")
            );
            fFail = TRUE;
            continue;
        }

        // cause lost condition
        lRet = ChangeDisplaySettings( &dmnew, 0 );
        if( lRet != DISP_CHANGE_SUCCESSFUL )
        {
            WriteToLog(
                TEXT("ChangeDisplaySettings() returned %d\n"),
                lRet
            );
            fFail = TRUE;
            continue;
        }

        // check if device lost
        hr = m_pSrcDevice8->TestCooperativeLevel();
        if(
            (hr != D3DERR_DEVICELOST) &&
            (hr != D3DERR_DEVICENOTRESET)
        )
        {
            WriteToLog(
                TEXT("device is not lost; test won't be too useful\n"),
                lRet
            );
            //fFail = TRUE;
            //continue;
        }

        // write to buffer
        try
        {
            for( INT iIndex = 0; iIndex < Size; iIndex++ )
            {
                pbVB[iIndex] = iIndex & 0xff;
            }
        }
        catch(...)
        {
            WriteToLog(
                TEXT("writing to pointer after device lost caused unhandled exception\n")
            );
            fFail = TRUE;
            continue;
        }

        // read and verify
        try
        {
            for( INT iIndex = 0; iIndex < Size; iIndex++ )
            {
                if( pbVB[iIndex] != (iIndex & 0xff) )
                {
                    WriteToLog(
                        TEXT("data mismatch at offset %d: was %x, expected %x\n"),
                        iIndex,
                        pbVB[iIndex],
                        (iIndex & 0xff)
                    );
                    fFail = TRUE;
                }
            }

            if( fFail )
            {
                continue;
            }
        }
        catch(...)
        {
            WriteToLog(
                TEXT("reading from pointer after device lost caused unhandled exception\n")
            );
            fFail = TRUE;
            continue;
        }

        pVB->Unlock();
    }
    while(0);

    RELEASE(pVB);
    RELEASE(pIB);

    // restore prior display settings
    lRet = ChangeDisplaySettings( &dmcur, 0 );
    if( lRet != DISP_CHANGE_SUCCESSFUL )
    {
        WriteToLog(
            TEXT("ChangeDisplaySettings() returned %d\n"),
            lRet
        );
        fFail = TRUE;
    }

    if( fFail )
    {
        Fail();
    }

    return(false);
}

