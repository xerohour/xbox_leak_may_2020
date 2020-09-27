/***************************************************************************
 *
 *  Copyright (C) 1/4/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wavbndlr.cpp
 *  Content:    WaveBank/WaveBundler remote functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/4/2002   dereks  Created.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xbdm.h>
#include "debugcmd.h"
#include "wavbndlr.h"

#define DPF(a) OutputDebugString(a "\n")

#define ASSERT(exp) \
    { \
        if(!(exp)) \
        { \
            __asm int 3 \
        } \
    }

#define IS_VALID_HANDLE_VALUE(h) ((h) && (INVALID_HANDLE_VALUE != (h)))

typedef struct _WBPLAYENTRYCONTEXT
{
    struct _WBPLAYENTRYCONTEXT *    pPrev;
    struct _WBPLAYENTRYCONTEXT *    pNext;
    LPDIRECTSOUNDBUFFER             pBuffer;
} WBPLAYENTRYCONTEXT, *LPWBPLAYENTRYCONTEXT;

LPWBPLAYENTRYCONTEXT g_pctxPlayEntry = NULL;
HANDLE g_hMutex = CreateMutex(NULL, FALSE, NULL);

class CAutoLock
{
protected:
    HANDLE                  m_hMutex;

public:
    CAutoLock(HANDLE hMutex)
        : m_hMutex(hMutex)
    {
        ASSERT(IS_VALID_HANDLE_VALUE(m_hMutex));
        WaitForSingleObject(m_hMutex, INFINITE);
    }

    ~CAutoLock(void)
    {
        ASSERT(IS_VALID_HANDLE_VALUE(m_hMutex));
        ReleaseMutex(m_hMutex);
    }
};

#define AutoLock() \
    CAutoLock __AutoLock(g_hMutex)


/****************************************************************************
 *
 *  WbAddEntry
 *
 *  Description:
 *      Adds an entry to the WaveBank!PlayEntry context list.
 *
 *  Arguments:
 *      LPWBPLAYENTRYCONTEXT [in]: context.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbAddEntry"

void
WbAddEntry
(
    LPWBPLAYENTRYCONTEXT    pContext
)
{
    AutoLock();
    
    ASSERT(!pContext->pPrev);
    ASSERT(!pContext->pNext);

    if(g_pctxPlayEntry)
    {
        ASSERT(!g_pctxPlayEntry->pPrev);
        
        pContext->pNext = g_pctxPlayEntry;
        g_pctxPlayEntry->pPrev = pContext;
    }

    g_pctxPlayEntry = pContext;
}
        

/****************************************************************************
 *
 *  WbRemoveEntry
 *
 *  Description:
 *      Removes an entry from the WaveBank!PlayEntry context list.
 *
 *  Arguments:
 *      LPWBPLAYENTRYCONTEXT [in]: context.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbRemoveEntry"

void
WbRemoveEntry
(
    LPWBPLAYENTRYCONTEXT    pContext
)
{
    AutoLock();
    
    if(pContext->pPrev)
    {
        pContext->pPrev->pNext = pContext->pNext;
    }

    if(pContext->pNext)
    {
        pContext->pNext->pPrev = pContext->pPrev;
    }

    if(pContext == g_pctxPlayEntry)
    {
        ASSERT(!pContext->pPrev);
        g_pctxPlayEntry = pContext->pNext;
    }

    pContext->pPrev = pContext->pNext = NULL;
}


/****************************************************************************
 *
 *  WbAllocContext
 *
 *  Description:
 *      Allocates context to track a stream.
 *
 *  Arguments:
 *      LPDIRECTSOUNDBUFFER [in]: buffer.
 *
 *  Returns:  
 *      LPWBPLAYENTRYCONTEXT: context.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbAllocContext"

LPWBPLAYENTRYCONTEXT
WbAllocContext
(
    LPDIRECTSOUNDBUFFER     pBuffer
)
{
    LPWBPLAYENTRYCONTEXT    pContext;

    AutoLock();
    
    if(pContext = (WBPLAYENTRYCONTEXT *)LocalAlloc(LPTR, sizeof(*pContext)))
    {
        pContext->pBuffer = pBuffer;

        WbAddEntry(pContext);
    }

    return pContext;
}
        

/****************************************************************************
 *
 *  WbFreeContext
 *
 *  Description:
 *      Frees context used to track a stream.
 *
 *  Arguments:
 *      LPWBPLAYENTRYCONTEXT [in/out]: context.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbFreeContext"

void
WbFreeContext
(
    LPWBPLAYENTRYCONTEXT    pContext
)
{
    AutoLock();
    
    WbRemoveEntry(pContext);

    if(pContext->pBuffer)
    {
        pContext->pBuffer->Release();
    }

    LocalFree(pContext);
}
        

/****************************************************************************
 *
 *  WbCheckStatus
 *
 *  Description:
 *      Checks the status of a buffer and frees it if it's stopped.
 *
 *  Arguments:
 *      LPWBPLAYENTRYCONTEXT [in/out]: context.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbCheckStatus"

void
WbCheckStatus
(
    LPWBPLAYENTRYCONTEXT    pContext
)
{
    DWORD                   dwStatus    = 0;
    
    AutoLock();
    
    if(pContext->pBuffer)
    {
        pContext->pBuffer->GetStatus(&dwStatus);

        if(!(dwStatus & DSBSTATUS_PLAYING))
        {
            WbFreeContext(pContext);
        }
    }
}
        

/****************************************************************************
 *
 *  WbCheckStatus
 *
 *  Description:
 *      Checks the status of a buffer and frees it if it's stopped.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WbCheckStatusAll"

void
WbCheckStatusAll
(
    void
)
{
    LPWBPLAYENTRYCONTEXT    pContext    = g_pctxPlayEntry;
    LPWBPLAYENTRYCONTEXT    pNext;

    AutoLock();
    
    while(pContext)
    {
        pNext = pContext->pNext;

        WbCheckStatus(pContext);

        pContext = pNext;
    }
}
        

/****************************************************************************
 *
 *  RCmdWaveBankPlayEntry
 *
 *  Description:
 *      Plays an individual sound bank entry.
 *
 *  Arguments:
 *      int [in]: argument count.
 *      char ** [in]: arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "RCmdWaveBankPlayEntry"

void 
RCmdWaveBankPlayEntry
(
    int                     argc, 
    char **                 argv
)
{
    HANDLE                  hFile           = INVALID_HANDLE_VALUE;
    WBPLAYENTRYCONTEXT *    pContext        = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    HRESULT                 hr              = S_OK;
    WAVEBANKENTRY           Entry;
    DSBUFFERDESC            dsbd;
    WAVEBANKUNIWAVEFORMAT   wfx;
    DWORD                   dwRead;
    LPVOID                  pvLock;
    DWORD                   i;

    AutoLock();
    
    ZeroMemory(&dsbd, sizeof(dsbd));

    if(2 != argc)
    {
        DPF("Bad argument count");
        hr = E_INVALIDARG;
    }

    //
    // Open the file
    //
    
    if(SUCCEEDED(hr))
    {
        hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE, NULL);

        if(!IS_VALID_HANDLE_VALUE(hFile))
        {
            DPF("Failed to open file");
            hr = E_FAIL;
        }
    }

    //
    // Read the entry header
    //

    if(SUCCEEDED(hr))
    {
        if(!ReadFile(hFile, &Entry, sizeof(Entry), &dwRead, NULL))
        {
            DPF("Failed to read file header");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr) && (sizeof(Entry) != dwRead))
    {
        DPF("Not enough data read from the file");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        WaveBankExpandFormat(&Entry.Format, &wfx);
    }

    //
    // Create the buffer
    //

    if(SUCCEEDED(hr))
    {
        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = &wfx.WaveFormatEx;
        dsbd.dwBufferBytes = Entry.PlayRegion.dwLength;

        if(FAILED(hr = DirectSoundCreateBuffer(&dsbd, &pBuffer)))
        {
            DPF("Unable to create audio buffer");
        }
    }

    //
    // Allocate context to track the buffer
    //

    if(SUCCEEDED(hr))
    {
        if(!(pContext = WbAllocContext(pBuffer)))
        {
            DPF("Out of memory");
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Read the entire entry into the buffer
    //

    if(SUCCEEDED(hr))
    {
        if(FAILED(hr = pBuffer->Lock(0, 0, &pvLock, &dwRead, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
        {
            DPF("Unable to lock buffer");
        }
    }

    if(SUCCEEDED(hr))
    {
        if(!ReadFile(hFile, pvLock, dwRead, &dwRead, NULL))
        {
            DPF("Unable to read wave data");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        pBuffer->Unlock(pvLock, dwRead, NULL, 0);
    }

    //
    // Set loop points
    //

    if(SUCCEEDED(hr))
    {
        if(FAILED(hr = pBuffer->SetLoopRegion(Entry.LoopRegion.dwStart, Entry.LoopRegion.dwLength)))
        {
            DPF("Unable to set loop region");
        }
    }

    //
    // Start playback
    //

    if(SUCCEEDED(hr))
    {
        hr = pBuffer->Play(0, 0, Entry.LoopRegion.dwLength ? DSBPLAY_LOOPING : 0);
    }

    //
    // Clean up
    //

    if(FAILED(hr))
    {
        if(pContext)
        {
            WbFreeContext(pContext);
        }
        else if(pBuffer)
        {
            pBuffer->Release();
        }
    }

    if(IS_VALID_HANDLE_VALUE(hFile))
    {
        CloseHandle(hFile);
    }
}


/****************************************************************************
 *
 *  RCmdWaveBankStopAll
 *
 *  Description:
 *      Stops all playing entries.
 *
 *  Arguments:
 *      int [in]: argument count.
 *      char ** [in]: arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "RCmdWaveBankStopAll"

void 
RCmdWaveBankStopAll
(
    int                     argc, 
    char **                 argv
)
{
    HRESULT                 hr          = S_OK;
    LPWBPLAYENTRYCONTEXT    pContext;
    
    AutoLock();
    
    if(1 != argc)
    {
        DPF("Bad argument count");
        hr = E_INVALIDARG;
    }

    if(SUCCEEDED(hr))
    {
        for(pContext = g_pctxPlayEntry; pContext; pContext = pContext->pNext)
        {
            pContext->pBuffer->Stop();
        }
    }
}


