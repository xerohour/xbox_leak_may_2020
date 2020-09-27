/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       testac97.cpp
 *  Content:    AC97 XMO tests.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/07/01    dereks  Created.
 *
 ****************************************************************************/

#include "testds.h"


/****************************************************************************
 *
 *  TestAc97XmoCallback
 *
 *  Description:
 *      Callback function for TestAc97Xmo.
 *
 *  Arguments:
 *      LPVOID [in]: stream context.
 *      LPVOID [in]: packet context.
 *      DWORD [in]: packet status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestAc97XmoCallback"

void CALLBACK
TestAc97XmoCallback
(
    LPVOID                  pvStreamContext,
    LPVOID                  pvPacketContext,
    DWORD                   dwStatus
)
{
    LPDWORD                 pdwSubmittedMask    = (LPDWORD)pvStreamContext;
    DWORD                   dwPacketIndex       = (DWORD)pvPacketContext;

    *pdwSubmittedMask &= ~(1UL << dwPacketIndex);
}    


/****************************************************************************
 *
 *  TestAc97Xmo
 *
 *  Description:
 *      Tests the AC97 XMO.
 *
 *  Arguments:
 *      DWORD [in]: channel index.
 *      DWORD [in]: channel mode.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestAc97XmoCallback"

HRESULT
TestAc97Xmo
(
    DWORD                   dwChannel,
    DWORD                   dwMode
)
{
    static const DWORD      dwPacketSize        = 0x1000;
    XAc97MediaObject *      pDevice             = NULL;
    XFileMediaObject *      pWaveFile           = NULL;
    LPVOID                  pvBufferData        = NULL;
    volatile DWORD          dwSubmittedMask     = 0;
    DWORD                   dwSubmittedSize     = 0;
    XMEDIAPACKET            xmp;
    DWORD                   dwRead;
    DWORD                   dwPosition;
    DWORD                   i;
    HRESULT                 hr;

    hr = ValidateResult(Ac97CreateMediaObject(dwChannel, TestAc97XmoCallback, (LPVOID)&dwSubmittedMask, &pDevice));

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pDevice->SetMode(dwMode));
    }

    if(SUCCEEDED(hr))
    {
        if(DSAC97_MODE_ENCODED == dwMode)
        {
            hr = ValidateResult(OpenWaveFile("t:\\media\\audio\\ac3\\dd-broadway.wav", NULL, &pWaveFile));
        }
        else
        {
            hr = ValidateResult(OpenStdWaveFile(CreateFormat(WAVE_FORMAT_PCM, 48000, 16, 2), &pWaveFile));
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidatePtr(pvBufferData = XPhysicalAlloc(dwPacketSize * DSAC97_MAX_ATTACHED_PACKETS, ~0UL, PAGE_SIZE, PAGE_READWRITE));
    }

    while(SUCCEEDED(hr))
    {
        while(((1UL << DSAC97_MAX_ATTACHED_PACKETS) - 1) == dwSubmittedMask);

        for(i = 0; i < DSAC97_MAX_ATTACHED_PACKETS; i++)
        {
            if(!(dwSubmittedMask & (1UL << i)))
            {
                break;
            }
        }

        ASSERT(i < DSAC97_MAX_ATTACHED_PACKETS);

        xmp.pvBuffer = (LPBYTE)pvBufferData + (dwPacketSize * i);
        xmp.dwMaxSize = dwPacketSize;
        xmp.pdwCompletedSize = &dwRead;
        xmp.pdwStatus = NULL;
        xmp.pContext = NULL;
        xmp.prtTimestamp = NULL;

        hr = ValidateResult(pWaveFile->Process(NULL, &xmp));

        if(SUCCEEDED(hr))
        {
            if(dwRead)
            {
                dwSubmittedMask |= 1UL << i;

                xmp.dwMaxSize = dwRead;
                xmp.pdwCompletedSize = NULL;
                xmp.pContext = (LPVOID)i;

                dwSubmittedSize += dwRead;

                hr = ValidateResult(pDevice->Process(&xmp, NULL));

                if(SUCCEEDED(hr))
                {
                    hr = ValidateResult(pDevice->Discontinuity());
                }

                if(SUCCEEDED(hr))
                {
                    hr = pDevice->GetCurrentPosition(&dwPosition);
                }

                if(SUCCEEDED(hr))
                {
                    DPF_TEST("submitted %lu, position %lu", dwSubmittedSize, dwPosition);
                }
            }
            else
            {
                while(dwSubmittedMask);

                hr = pDevice->GetCurrentPosition(&dwPosition);

                if(SUCCEEDED(hr))
                {
                    DPF_TEST("FINAL: submitted %lu, position %lu", dwSubmittedSize, dwPosition);
                }

                break;
            }
        }
    }

    if(pvBufferData)
    {
        XPhysicalFree(pvBufferData);
    }

    RELEASE(pWaveFile);
    RELEASE(pDevice);

    return hr;
}


