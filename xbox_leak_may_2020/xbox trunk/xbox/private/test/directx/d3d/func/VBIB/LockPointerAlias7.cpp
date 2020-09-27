//
//  LockPointerAlias7.cpp
//



#include <d3d.h>
#include <d3dx.h>
#include <tchar.h>
#include <windows.h>
#include "LockPointerAlias.h"



UINT LockPointerAlias::TestInitialize7()
{
    if( !m_pD3D7 || !m_pSrcDevice7 )
	{
		WriteToLog(_T("TestInitialize() - Framework devices == NULL\n"));
        return D3DTESTINIT_ABORT;
	}

    ExecuteTest7((UINT)(-1));

    return(D3DTESTINIT_RUN);
}



bool
LockPointerAlias::ExecuteTest7
(
    UINT nTest
)
{
    static const DWORD CapsList[] =
    {
        NULL,
        D3DVBCAPS_DONOTCLIP,
        D3DVBCAPS_WRITEONLY,
        D3DVBCAPS_SYSTEMMEMORY,
        D3DVBCAPS_DONOTCLIP | D3DVBCAPS_WRITEONLY,
        D3DVBCAPS_DONOTCLIP | D3DVBCAPS_SYSTEMMEMORY,
        D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY,
        D3DVBCAPS_DONOTCLIP | D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY,
    };

    static const DWORD LockFlags[] =
    {
        DDLOCK_NOSYSLOCK,
        DDLOCK_DISCARDCONTENTS,
        DDLOCK_NOSYSLOCK | DDLOCK_DISCARDCONTENTS,
    };

    typedef union __TESTCASE
    {
        struct
        {
            UINT LockFlag : 4;
            UINT CapsList : 6;
        };
        UINT nTest;

    } TESTCASE, *LPTESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(LockFlags),
        COUNTOF(CapsList),
    };


    TESTCASE tc;
    tc.nTest = (nTest - 1);

    if( nTest == (UINT)(-1) )
    {
        ClearRangeList();

        tc.nTest = 0;
        for( tc.CapsList = 0; tc.CapsList < tcCount.CapsList; tc.CapsList++ )
        {
            for( tc.LockFlag = 0; tc.LockFlag < tcCount.LockFlag; tc.LockFlag++ )
            {
                AddTestRange( (tc.nTest + 1), (tc.nTest + 1) );
            }
        }

        return( FALSE );
    }

    if(
        (tc.CapsList >= tcCount.CapsList) ||
        (tc.LockFlag >= tcCount.LockFlag)
    )
    {
        WriteToLog(
            TEXT("No such test (%d)\n"),
            nTest
        );

        return(FALSE);
    }


    D3DVERTEXBUFFERDESC vbdesc =
    {
        sizeof(vbdesc),
        CapsList[tc.CapsList],
        D3DFVF_VERTEX,
        4096  // size
    };
    DWORD Flags = LockFlags[tc.LockFlag];

    TCHAR sz[256];

    wsprintf( sz,
        TEXT("Caps = 0x%x, LockFlags = 0x%x"),
        vbdesc.dwCaps,
        Flags
    );


    // skip buffer if not valid
    // no cases to consider?


    ClearFrame();

    BeginTestCase( sz, nTest );


    BOOL fFail = FALSE;
    HRESULT hr;
    LPDIRECT3DVERTEXBUFFER7 pVB = NULL;
    LPBYTE pbVB = NULL;

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


        hr = m_pD3D7->CreateVertexBuffer(
            &vbdesc,
            &pVB,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer() failed\n")
            );
            continue;
        }

        hr = pVB->Lock(
            Flags,
            (LPVOID*)&pbVB,
            NULL
        );
        if( FAILED(hr) )
        {
            WriteToLog( TEXT("Lock() failed\n") );
            continue;
        }

        // write to buffer
        try
        {
            for( INT iIndex = 0; iIndex < vbdesc.dwSize; iIndex++ )
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
        hr = (m_pSrcTarget->GetSurface7())->IsLost();
        if( hr != DDERR_SURFACELOST )
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
            for( INT iIndex = 0; iIndex < vbdesc.dwSize; iIndex++ )
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
            for( INT iIndex = 0; iIndex < vbdesc.dwSize; iIndex++ )
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

    hr = m_pDDraw7->RestoreAllSurfaces();
    WriteToLog(TEXT("%s 0x%x\n"),DecodeHResult(hr),hr);

    if( fFail )
    {
        Fail();
    }

    return(false);
}

