/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DSound.cpp

Abstract:

	DSound Playback engine for stress

Author:

	Dan Haffner(danhaff) 16-Jan-2002

Environment:

	Xbox only

Revision History:

	16-Jan-2002     danhaff
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "media.h"
#include "helpers.h"
#include "macros.h"
#include "dsstress.h"


using namespace WFVO;

/*
class SE
{
public: 
    DWORD m_dwStatus;
    DWORD m_dwPacket;
    m_pStream;
    m


};
*/


//------------------------------------------------------------------------------
//	Prototypes
//------------------------------------------------------------------------------
DWORD DSStressThreadProc(void *DSStress);


//------------------------------------------------------------------------------
//	Defines
//------------------------------------------------------------------------------
#define MAX_SOUNDS2D 2000
#define MAX_SOUNDS3D 2000
#define MAX_PLAYINGSOUNDS2D 188
#define MAX_PLAYINGSOUNDS3D 64
#define MAX_ATTACHEDPACKETS 64
//#define WAIT 25000
/********************************************************************************
********************************************************************************/
class DSStress
{
public:
    DSStress(void);
    ~DSStress(void);
    HRESULT Init(DWORD dwBuffers2D, DWORD dwBuffers3D, DWORD dwStreams2D, DWORD dwStreams3D, DWORD dwMixinBuffers2D, DWORD dwMixinBuffers3D, DWORD dwSends2D, DWORD dwSends3D, DWORD dwWait);
    HRESULT Shutdown(void);

private:
    
    HRESULT Help_CreateSoundBuffer(BOOL b3D, WORD wChannels, BOOL bMixin, IDirectSoundBuffer **ppBuffer);
    HRESULT Help_CreateSoundStream(BOOL b3D, WORD wChannels, IDirectSoundStream **ppStream);
    
    HANDLE m_hThread;
    BOOL  m_bInitialized;
    DWORD m_dwStreams2D;
    DWORD m_dwStreams3D;
    DWORD m_dwBuffers2D;
    DWORD m_dwBuffers3D;
    DWORD m_dwMixinBuffers2D;
    DWORD m_dwMixinBuffers3D;
    DWORD m_dwSends2D;
    DWORD m_dwSends3D;
    DWORD m_dwWait;

    DWORD m_dwPlayingSounds2D;
    DWORD m_dwPlayingSounds3D;
    DWORD m_dwStatus2D[MAX_SOUNDS2D];
    DWORD m_dwStatus3D[MAX_SOUNDS3D];

//    XMEDIAPACKET xmp2D;
//    XMEDIAPACKET xmp3D;
    DWORD        m_dwPacket2D[MAX_SOUNDS2D];;
    DWORD        m_dwPacket3D[MAX_SOUNDS3D];;

    DWORD m_bQuitSignal;
    IDirectSoundStream *m_pStream2D[MAX_SOUNDS2D];
    IDirectSoundStream *m_pStream3D[MAX_SOUNDS3D];
    IDirectSoundBuffer *m_pBuffer2D[MAX_SOUNDS2D];
    IDirectSoundBuffer *m_pBuffer3D[MAX_SOUNDS3D];
    IDirectSoundBuffer *m_pMixinBuffer2D[MAX_SOUNDS2D];
    IDirectSoundBuffer *m_pMixinBuffer3D[MAX_SOUNDS3D];
    LPVOID              m_pvBufferData;

    friend DWORD DSStressThreadProc(void *DSStress);
};


/********************************************************************************
********************************************************************************/
DSStress::DSStress(void)
{
    m_hThread   = NULL;
    m_bInitialized  = NULL;
    m_dwStreams2D = 0;
    m_dwStreams3D = 0;
    m_dwBuffers2D = 0;
    m_dwBuffers3D = 0;
    m_dwMixinBuffers2D = 0;
    m_dwMixinBuffers3D = 0;
    m_dwSends2D= 0;
    m_dwSends3D= 0;
    m_dwWait   = 0;

    memset(m_pStream2D, 0, sizeof(IDirectSoundStream *) * MAX_SOUNDS2D);
    memset(m_pStream3D, 0, sizeof(IDirectSoundStream *) * MAX_SOUNDS3D);
    memset(m_pBuffer2D, 0, sizeof(IDirectSoundBuffer *) * MAX_SOUNDS2D);
    memset(m_pBuffer3D, 0, sizeof(IDirectSoundBuffer *) * MAX_SOUNDS3D);
    memset(m_pMixinBuffer2D, 0, sizeof(IDirectSoundBuffer *) * MAX_SOUNDS2D);
    memset(m_pMixinBuffer3D, 0, sizeof(IDirectSoundBuffer *) * MAX_SOUNDS3D);

//    memset(xmp2D,       0,        sizeof(XMEDIAPACKET) * MAX_SOUNDS2D);
//    memset(xmp3D,       0,        sizeof(XMEDIAPACKET) * MAX_SOUNDS3D);
    memset(m_dwPacket2D,  XMEDIAPACKET_STATUS_SUCCESS,        sizeof(DWORD)        * MAX_SOUNDS2D);
    memset(m_dwPacket3D,  XMEDIAPACKET_STATUS_SUCCESS,        sizeof(DWORD)        * MAX_SOUNDS3D);

    memset(m_dwStatus2D,  FALSE, sizeof(DWORD)* MAX_SOUNDS2D);
    memset(m_dwStatus3D,  FALSE, sizeof(DWORD)* MAX_SOUNDS3D);

    m_dwPlayingSounds2D = 0;
    m_dwPlayingSounds3D = 0;

    m_pvBufferData = NULL;
    m_bQuitSignal = FALSE;

};

/********************************************************************************
********************************************************************************/
DSStress::~DSStress(void)
{
ASSERT(m_bInitialized);
delete[] m_pvBufferData;
m_pvBufferData = NULL;
};


/********************************************************************************
********************************************************************************/
HRESULT DSStress::Shutdown(void)
{
ASSERT(m_bInitialized);
m_bQuitSignal = TRUE;
WaitForThreadEnd(&m_hThread);
return S_OK;
}


/********************************************************************************
********************************************************************************/
HRESULT DSStress::Init(DWORD dwBuffers2D, DWORD dwBuffers3D, DWORD dwStreams2D, DWORD dwStreams3D, DWORD dwMixinBuffers2D, DWORD dwMixinBuffers3D, DWORD dwSends2D, DWORD dwSends3D, DWORD dwWait)
{
    ASSERT(!m_bInitialized);
    ASSERT(dwBuffers2D <= MAX_SOUNDS2D);
    ASSERT(dwBuffers3D <= MAX_SOUNDS3D);
    ASSERT(dwStreams2D <= MAX_SOUNDS2D);
    ASSERT(dwStreams3D <= MAX_SOUNDS3D);
    ASSERT(dwMixinBuffers2D <= MAX_SOUNDS2D);
    ASSERT(dwMixinBuffers3D <= MAX_SOUNDS3D);

    HRESULT hr = S_OK;
    DWORD i = 0;


    //Copy the in-params.
    m_dwBuffers2D = dwBuffers2D;
    m_dwBuffers3D = dwBuffers3D;
    m_dwStreams2D = dwStreams2D;
    m_dwStreams3D = dwStreams3D;
    m_dwMixinBuffers2D = dwMixinBuffers2D;
    m_dwMixinBuffers3D = dwMixinBuffers3D;
    //m_dwWait           = dwWait;
    
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************
    //*********************************************************    
    //***************  REMOVE THIS!!! *************************
    m_dwWait           = 25000;



    m_dwSends2D = dwSends2D;
    m_dwSends3D = dwSends3D;

    //None of the sounds start off playing.
    //  Streams take resources all the time (currently)
    //  MixinBuffers take resources all the time.
    m_dwPlayingSounds2D = dwStreams2D + dwMixinBuffers2D;
    m_dwPlayingSounds3D = dwStreams3D + dwMixinBuffers3D;

    //Create sound data.
    if (SUCCEEDED(hr))
    {
        m_pvBufferData = new BYTE[30000];
        CHECKALLOC(m_pvBufferData);
        for (i=0; i<30000; i++)
            ((char *)(m_pvBufferData))[i] = rand() % 256;
    }

    //Create all the buffers and streams.
    if (SUCCEEDED(hr))
    {
        //Create mixin buffers first so we can send to them.
        for (i=0; i<dwMixinBuffers2D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundBuffer(FALSE, 1, TRUE, &m_pMixinBuffer2D[i]));        
        }
        for (i=0; i<dwMixinBuffers3D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundBuffer(TRUE, 1, TRUE, &m_pMixinBuffer3D[i]));        
        }

        //Create normal buffers.
        for (i=0; i<dwBuffers2D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundBuffer(FALSE, 1, FALSE, &m_pBuffer2D[i]));        
        }
        for (i=0; i<dwBuffers3D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundBuffer(TRUE, 1, FALSE, &m_pBuffer3D[i]));        
        }
        for (i=0; i<dwStreams2D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundStream(FALSE, 1, &m_pStream2D[i]));        
        }
        for (i=0; i<dwStreams3D && SUCCEEDED(hr); i++)
        {
            CHECKRUN(Help_CreateSoundStream(TRUE, 1, &m_pStream3D[i]));        
        }
    }
    

    if (SUCCEEDED(hr))
    {
        m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DSStressThreadProc,(void*)this, 0, NULL);
        if (!m_hThread)
        {
            Log("CMusic::LoadNextSegment: CreateThread(LoadNextSegmentThreadProc) failed!!\n");
            hr = E_FAIL;        
        }
    }

    return S_OK;
}


/********************************************************************************
********************************************************************************/
DWORD DSStressThreadProc(void *pvDSStress)
{
HRESULT hr = S_OK;
DSStress *pDSStress = (DSStress*)pvDSStress;
XMEDIAPACKET xmp = {0};
DWORD i= 0;
DWORD dwStatus = 0;


ASSERT(pvDSStress);
ASSERT(!pDSStress->m_bInitialized);

    while (!pDSStress->m_bQuitSignal)
    {


        //Process the mixin buffers first.
        for (i=0; i<pDSStress->m_dwMixinBuffers2D && SUCCEEDED(hr); i++)
        {
            if (!(rand() % pDSStress->m_dwWait))
            {

                CHECKRUN(pDSStress->m_pMixinBuffer2D[i]->SetFrequency(DSBFREQUENCY_MIN + rand() % (DSBFREQUENCY_MAX - DSBFREQUENCY_MIN)));

//                CHECKRUN(pDSStress->m_pMixinBuffer2D[i]->Stop());
//                CHECKRUN(pDSStress->m_pMixinBuffer2D[i]->SetCurrentPosition(0));
//                CHECKRUN(pDSStress->m_pMixinBuffer2D[i]->Play(0, 0, rand() % 2 ? DSBPLAY_LOOPING : 0));                    

            }
        }
        for (i=0; i<pDSStress->m_dwMixinBuffers3D && SUCCEEDED(hr); i++)
        {
            if (!(rand() % pDSStress->m_dwWait))
            {
                CHECKRUN(pDSStress->m_pMixinBuffer3D[i]->SetFrequency(DSBFREQUENCY_MIN + rand() % (DSBFREQUENCY_MAX - DSBFREQUENCY_MIN)));
//                CHECKRUN(pDSStress->m_pMixinBuffer3D[i]->Stop());
//                CHECKRUN(pDSStress->m_pMixinBuffer3D[i]->SetCurrentPosition(0));
//                CHECKRUN(pDSStress->m_pMixinBuffer3D[i]->Play(0, 0, rand() % 2 ? DSBPLAY_LOOPING : 0));                    

            }
        }





        //Start and/or stop some buffers.
        for (i=0; i<pDSStress->m_dwBuffers2D && SUCCEEDED(hr); i++)
        {

            //Get the status.
            CHECKRUN(pDSStress->m_pBuffer2D[i]->GetStatus(&dwStatus));
            
            //Check to see if this sound has ended.
            if (pDSStress->m_dwStatus2D[i])
            {
                if (!(dwStatus & DSBSTATUS_PLAYING))
                {
                    pDSStress->m_dwPlayingSounds2D--;
                    pDSStress->m_dwStatus2D[i] = FALSE;
                    DirectSoundDoWork();
                }
            }
            
            if (pDSStress->m_dwPlayingSounds2D < MAX_PLAYINGSOUNDS2D)
            {
                if (!(rand() % pDSStress->m_dwWait))
                {
                    CHECKRUN(pDSStress->m_pBuffer2D[i]->Stop());
                    CHECKRUN(pDSStress->m_pBuffer2D[i]->SetCurrentPosition(0));
                    CHECKRUN(pDSStress->m_pBuffer2D[i]->Play(0, 0, 0));                    

                    //If it wasn't playing but it is now, then note that.
                    if (!(dwStatus & DSBSTATUS_PLAYING))
                    {
                        pDSStress->m_dwStatus2D[i] = TRUE;
                        pDSStress->m_dwPlayingSounds2D++;
                    }
                }
            }
        }
        //Start and/or stop some buffers.
        for (i=0; i<pDSStress->m_dwBuffers3D && SUCCEEDED(hr); i++)
        {

            //Get status.
            CHECKRUN(pDSStress->m_pBuffer3D[i]->GetStatus(&dwStatus));

            //If this sound has ended, then note that.
            if (pDSStress->m_dwStatus3D[i])
            {
                if (!(dwStatus & DSBSTATUS_PLAYING))
                {
                    pDSStress->m_dwPlayingSounds3D--;
                    pDSStress->m_dwStatus3D[i] = FALSE;
                    DirectSoundDoWork();
                }
            }


            if (pDSStress->m_dwPlayingSounds3D < MAX_PLAYINGSOUNDS3D)
            {
                if (!(rand() % pDSStress->m_dwWait))
                {
                    CHECKRUN(pDSStress->m_pBuffer3D[i]->Stop());
                    CHECKRUN(pDSStress->m_pBuffer3D[i]->SetCurrentPosition(0));
                    CHECKRUN(pDSStress->m_pBuffer3D[i]->Play(0, 0, 0));
                
                    //If it wasn't playing but it is now, then note that.
                    if (!(dwStatus & DSBSTATUS_PLAYING))
                    {
                        pDSStress->m_dwStatus3D[i] = TRUE;
                        pDSStress->m_dwPlayingSounds3D++;
                    }
                }
            }
        }


        if (pDSStress->m_dwPlayingSounds2D < MAX_PLAYINGSOUNDS2D)
        {
            for (i=0; i<pDSStress->m_dwStreams2D && SUCCEEDED(hr); i++)
            {            

                if (pDSStress->m_dwPacket2D[i] == XMEDIAPACKET_STATUS_SUCCESS)
                {
                    if (!(rand() % pDSStress->m_dwWait))
                    {
                        xmp.pvBuffer = pDSStress->m_pvBufferData;
                        xmp.dwMaxSize = 30000;
                        xmp.pdwStatus = &pDSStress->m_dwPacket2D[i];
                        CHECKRUN(pDSStress->m_pStream2D[i]->Process(&xmp, NULL));
                    }
                }            
            }
        }

        if (pDSStress->m_dwPlayingSounds3D < MAX_PLAYINGSOUNDS3D)
        {
            for (i=0; i<pDSStress->m_dwStreams3D && SUCCEEDED(hr); i++)
            {
                if (pDSStress->m_dwPacket3D[i] == XMEDIAPACKET_STATUS_SUCCESS)
                {
                    if (!(rand() % pDSStress->m_dwWait))
                    {
                        xmp.pvBuffer = pDSStress->m_pvBufferData;
                        xmp.dwMaxSize = 30000;
                        xmp.pdwStatus = &pDSStress->m_dwPacket3D[i];
                        CHECKRUN(pDSStress->m_pStream3D[i]->Process(&xmp, NULL));
                    }
                }            
            }
        }

        //Take care of stream-related items in case there are no buffers.
        DirectSoundDoWork();
    }

    return 0;
};





/********************************************************************************
********************************************************************************/
HRESULT DSStress::Help_CreateSoundBuffer(BOOL b3D, WORD wChannels, BOOL bMixin, IDirectSoundBuffer **ppBuffer)
{
    HRESULT hr = S_OK;    

    ASSERT(wChannels < 8);
    
    DSBUFFERDESC dsbd = {0};
    dsbd.dwSize = sizeof(dsbd);
    
    //Can't have locdefer mixin buffahs.
    if (!bMixin)
        dsbd.dwFlags = DSBCAPS_LOCDEFER;

    if (b3D)
        dsbd.dwFlags |= DSBCAPS_CTRL3D;

    if (bMixin)
        dsbd.dwFlags |= DSBCAPS_MIXIN;
    
    //Set up the wave format.
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = wChannels;
    wfx.nSamplesPerSec = 8000;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;
    
    if (!bMixin)
        dsbd.lpwfxFormat = &wfx;
    else
        dsbd.lpwfxFormat = NULL;

    if (!bMixin)
        dsbd.dwBufferBytes = 30000;
    else
        dsbd.dwBufferBytes = 0;


    CHECKRUN(DirectSoundCreateBuffer(&dsbd, ppBuffer));

    if (!bMixin)
    {
        CHECKRUN((*ppBuffer)->SetBufferData(m_pvBufferData, dsbd.dwBufferBytes));
        
        //assume mixin buffers have already been created.
        if (!b3D)
        {            
            if (m_dwSends2D)
            {
                CHECKRUN((*ppBuffer)->SetOutputBuffer(m_pMixinBuffer2D[rand() % m_dwMixinBuffers2D]));
                m_dwSends2D--;
            }
            else if (m_dwSends3D)
            {
                CHECKRUN((*ppBuffer)->SetOutputBuffer(m_pMixinBuffer3D[rand() % m_dwMixinBuffers3D]));
                m_dwSends3D--;
            }
        }

    }

    return hr;
}



/********************************************************************************
********************************************************************************/
HRESULT DSStress::Help_CreateSoundStream(BOOL b3D, WORD wChannels, IDirectSoundStream **ppStream)
{
    HRESULT hr = S_OK;    
    
    DSSTREAMDESC dssd = {0};
    dssd.dwFlags = DSBCAPS_LOCDEFER;
    if (b3D)
        dssd.dwFlags |= DSBCAPS_CTRL3D;

    dssd.dwMaxAttachedPackets = MAX_ATTACHEDPACKETS;
    
    //Set up the wave format.
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = wChannels;
    wfx.nSamplesPerSec = 8000;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;
    dssd.lpwfxFormat = &wfx;

    CHECKRUN(DirectSoundCreateStream(&dssd, ppStream));
    return hr;
}


DSStress *g_pDSStress = NULL;
DWORD     g_dwThreads = NULL;



/********************************************************************************
********************************************************************************/
HRESULT StartDSStress(DWORD dwThreads, DWORD dwBuffers2D, DWORD dwBuffers3D, DWORD dwStreams2D, DWORD dwStreams3D, DWORD dwMixinBuffers2D, DWORD dwMixinBuffers3D, DWORD dwSends2D, DWORD dwSends3D, DWORD dwWait)
{
    HRESULT hr = S_OK;
    DWORD i = 0;

    g_dwThreads = dwThreads;

    g_pDSStress = new DSStress[g_dwThreads];    
    CHECKALLOC(g_pDSStress);

    for (i=0; i<g_dwThreads && SUCCEEDED(hr); i++)
    {
        CHECKRUN(g_pDSStress[i].Init(dwBuffers2D, dwBuffers3D, dwStreams2D, dwStreams3D, dwMixinBuffers2D, dwMixinBuffers3D, dwSends2D, dwSends3D, dwWait));
    }    

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT StopDSStress(void)
{

    delete []g_pDSStress;
    g_pDSStress = NULL;
    return S_OK;
};


