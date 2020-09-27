//      Copyright (c) 1996-1999 Microsoft Corporation

// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// 4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
//
// We disable this because we use exceptions and do *not* specify -GX (USE_NATIVE_EH in
// sources).
//
// The one place we use exceptions is around construction of objects that call 
// InitializeCriticalSection. We guarantee that it is safe to use in this case with
// the restriction given by not using -GX (automatic objects in the call chain between
// throw and handler are not destructed). Turning on -GX buys us nothing but +10% to code
// size because of the unwind code.
//
// Any other use of exceptions must follow these restrictions or -GX must be turned on.
//
// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
#pragma warning(disable:4530)
//      CSynth.cpp
//      Copyright (c) Microsoft Corporation 1996, 1997, 1998
//

#include "simple.h"
#include <mmsystem.h>
#include "dsoundp.h"   
#include <dmusicc.h>
#include <dmusics.h>
#include "synth.h"
#include "CSynth.h"
#include "debug.h"
#include "..\shared\xsoundp.h" 

CSynth::CSynth() : CMemTrack(DMTRACK_SYNTH)
{
    m_pSynthWorker = NULL;
    m_dssequencer = NULL;
    m_MasterClock.m_pParent = this;
	m_cRef = 0;

    DWORD nIndex;
    CVoice *pVoice;

    m_fCSInitialized = FALSE;
    ::InitializeCriticalSection(&m_CriticalSection);
    m_fCSInitialized = TRUE;

    m_ppControl = NULL;
    m_dwControlCount = 0;
    m_nMaxVoices = 0;
    m_nExtraVoices = 0; 
    m_stLastTime = 0; 
    m_rtLastTime = 0;
    m_dwSampleRate = 48000;
    m_lVolume = 0;
    m_lBoost = 6 * 100;
    m_lGainAdjust = 6 * 100;            // Default 6 dB boost
    SetGainAdjust(m_lGainAdjust);
}

/*HRESULT CSynth::Initialize()
{
    HRESULT hr = DirectSoundCreateSequencer(NULL, 1000, &m_dssequencer);
	return hr;
}*/

CSynth::~CSynth()

{
    CVoice *pVoice;

    if (m_fCSInitialized)
    {
        // If CS never initialized, nothing else will have been set up
        //
        Close();
        while (pVoice = m_VoicesInUse.RemoveHead())
        {
            delete pVoice;
        }
        while (pVoice = m_VoicesFree.RemoveHead())
        {
            delete pVoice;
        }
        while (pVoice = m_VoicesExtra.RemoveHead())
        {
            delete pVoice;
        }

        DeleteCriticalSection(&m_CriticalSection);

        if(m_dssequencer)
        {
            m_dssequencer->Release();
        }
//        if(m_pClock){
//            m_pClock->Release();
//        }
    }
}

// CSynth::QueryInterface
//
STDMETHODIMP
CSynth::QueryInterface(const IID &iid,
                                   void **ppv)
{

    if (iid == IID_IUnknown || iid == IID_IDirectMusicSynthX) {
        *ppv = static_cast<IDirectMusicSynthX*>(this);
    }
    else
	{
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    AddRef();
    return S_OK;
}


// CSynth::AddRef
//
STDMETHODIMP_(ULONG)
CSynth::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CSynth::Release
//
STDMETHODIMP_(ULONG)
CSynth::Release()
{
    if (!InterlockedDecrement(&m_cRef)) {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CSynth::SetMasterVolume(LONG lVolume)

{
	::EnterCriticalSection(&m_CriticalSection);
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    m_lVolume =  lVolume;
    m_lGainAdjust = m_lVolume + m_lBoost;

    SetGainAdjust(m_lGainAdjust);
    ::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}

STDMETHODIMP CSynth::GetMasterClock(
	IReferenceClock **ppClock)	// Pointer to master <i IReferenceClock>, 
								// used by all devices in current instance of DirectMusic.

{
    return m_MasterClock.QueryInterface(IID_IReferenceClock,(void **) ppClock);
}


short CSynth::ChangeVoiceCount(CVoiceList *pList,short nOld,short nCount)

{
    if (nCount > nOld)
    {
        short nNew = nCount - nOld;
        for (;nNew != 0; nNew--)
        {
            CVoice *pVoice = new CVoice;
            if (pVoice != NULL)
            {
                pList->AddHead(pVoice);
            }
        }
    }
    else
    {
        short nNew = nOld - nCount;
        for (;nNew > 0; nNew--)
        {
            CVoice *pVoice = pList->RemoveHead();
            if (pVoice != NULL)
            {
                delete pVoice;
            }
            else 
            {
                nCount += nNew;
                break;
            }
        }
    }
    return nCount;
}

HRESULT CSynth::SetMaxVoices(short nVoices,short nTempVoices)

{
    ::EnterCriticalSection(&m_CriticalSection);

    m_nMaxVoices = ChangeVoiceCount(&m_VoicesFree,m_nMaxVoices,nVoices);
    m_nExtraVoices = ChangeVoiceCount(&m_VoicesExtra,m_nExtraVoices,nTempVoices);

    ::LeaveCriticalSection(&m_CriticalSection);
    return S_OK;
}

STDMETHODIMP CSynth::SetNumChannelGroups(DWORD dwCableCount)

{
    HRESULT hr = S_OK;
    CControlLogic **ppControl;
    ::EnterCriticalSection(&m_CriticalSection);
    if (m_dwControlCount != dwCableCount)
    {
        try
        {
            ppControl = new CControlLogic *[dwCableCount];
        }
        catch( ... )
        {
            ppControl = NULL;
        }

        if (ppControl)
        {
            DWORD dwX;
            for (dwX = 0; dwX < dwCableCount; dwX++)
            {
                ppControl[dwX] = NULL;
            }
            if (m_dwControlCount < dwCableCount)
            {
                for (dwX = 0; dwX < m_dwControlCount; dwX++)
                {
                    ppControl[dwX] = m_ppControl[dwX];
                }
                for (;dwX < dwCableCount; dwX++)
                {
                    try
                    {
                        ppControl[dwX] = new CControlLogic;
                    }
                    catch( ... )
                    {
                        ppControl[dwX] = NULL;
                    }

                    if (ppControl[dwX])
                    {
                        hr = ppControl[dwX]->Init(&m_Instruments, this);
                        if (FAILED(hr))
                        {
                            delete ppControl[dwX];
                            ppControl[dwX] = NULL;
                            dwCableCount = dwX;
                            break;
                        }

                        ppControl[dwX]->SetGainAdjust(m_vrGainAdjust);
                    }
                    else
                    {
                        dwCableCount = dwX;
                        break;
                    }
                }
            }
            else
            {
                AllNotesOff();
                for (dwX = 0; dwX < dwCableCount; dwX++)
                {
                    ppControl[dwX] = m_ppControl[dwX];
                }
                for (; dwX < m_dwControlCount; dwX++)
                {
                    if (m_ppControl[dwX])
                    {
                        delete m_ppControl[dwX];
                    }
                }
            }
            if (m_ppControl)
            {
                delete[] m_ppControl;
            }
            m_ppControl = ppControl;
            m_dwControlCount = dwCableCount;
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return hr;
}

void CSynth::SetGainAdjust(VREL vrGainAdjust)
{
    DWORD idx;

    m_vrGainAdjust = vrGainAdjust;
    ::EnterCriticalSection(&m_CriticalSection);
    
    for (idx = 0; idx < m_dwControlCount; idx++)
    {
        m_ppControl[idx]->SetGainAdjust(m_vrGainAdjust);
    }

    ::LeaveCriticalSection(&m_CriticalSection);
}

STDMETHODIMP CSynth::Init(DWORD dwVoicePoolSize)

{
    HRESULT hr;
    ::EnterCriticalSection(&m_CriticalSection);
    hr = DirectSoundCreateSequencer(NULL, 1000, &m_dssequencer);
    if (SUCCEEDED(hr))
    {
        hr = Activate();
    }
    if (SUCCEEDED(hr))
    {   
        hr = SetNumChannelGroups(0);
    }
    if (SUCCEEDED(hr))
    {
        SetMaxVoices((short) dwVoicePoolSize, 8);
    }
//    m_vrGainAdjust = 6 * 100;            // Default 6 dB boost
    ::LeaveCriticalSection(&m_CriticalSection);
    return hr;
}

HRESULT CSynth::Close()

{
    Deactivate();
    ::EnterCriticalSection(&m_CriticalSection);
    AllNotesOff();
    DWORD dwX;
    for (dwX = 0; dwX < m_dwControlCount; dwX++)
    {
        if (m_ppControl[dwX])
        {
            delete m_ppControl[dwX];
        }
    }
    m_dwControlCount = 0;
    if (m_ppControl)
    {
        delete[] m_ppControl;
        m_ppControl = NULL;
    }
    m_stLastTime = 0;
    m_rtLastTime = 0;
    ::LeaveCriticalSection(&m_CriticalSection);
    return S_OK;
}
   
static void WINAPI _SynthWorker(LPVOID lpParam,REFERENCE_TIME *prtWakeUp)
{
    ((CSynth *)lpParam)->DoWork(prtWakeUp);
}

extern CBossMan g_BossMan;

HRESULT CSynth::Activate()

{
    if (!m_pSynthWorker)
    {
        m_pSynthWorker = g_BossMan.InstallWorker(_SynthWorker,this,5,"DM Worker: Synth (MilsPerSecond)");
    }
    m_stLastTime = 0;
    m_rtLastTime = 0;
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    m_dwDLSVoices = 0;
    m_dwWaveVoices = 0;
    m_dwStreamVoices = 0;
    m_dwFreeVoices = 0;
    DMusicRegisterPerformanceCounter( "DM Synth Active DLS Voices",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_dwDLSVoices );
    DMusicRegisterPerformanceCounter( "DM Synth Active Wave Voices",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_dwWaveVoices );
    DMusicRegisterPerformanceCounter( "DM Synth Active Stream Voices",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_dwStreamVoices );
    DMusicRegisterPerformanceCounter( "DM Synth Free Voices",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_dwFreeVoices );
#endif
    return S_OK;
}

HRESULT CSynth::Deactivate()

{
    if (m_pSynthWorker)
    {
        m_pSynthWorker->PinkSlip();
        m_pSynthWorker = NULL;
    }
    AllNotesOff();
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicUnregisterPerformanceCounter( "DM Synth Active DLS Voices");
    DMusicUnregisterPerformanceCounter( "DM Synth Active Wave Voices");
    DMusicUnregisterPerformanceCounter( "DM Synth Active Stream Voices");
    DMusicUnregisterPerformanceCounter( "DM Synth Free Voices");
#endif
    return S_OK;
}

void CSynth::DoWork(REFERENCE_TIME *prtWakeUp)

{
    DirectSoundDoWork();

    CVoice *pVoice;
    CVoice *pNextVoice;

    ::EnterCriticalSection(&m_CriticalSection);

    // There seems to be a 1% drift between the audio time and the hardware time on Xbox.
    // This synchronizes the two clocks

    m_MasterClock.GetTime(&m_rtLastTime);
    m_dssequencer->SetTime(m_rtLastTime);

    pVoice = m_VoicesInUse.GetHead();

    for (;pVoice != NULL;pVoice = pNextVoice)
    {
        pNextVoice = pVoice->GetNext();
        pVoice->DoWork(this);
        if (pVoice->m_fInUse == FALSE) 
        {
            m_VoicesInUse.Remove(pVoice);
            m_VoicesFree.AddHead(pVoice);
        }
    }
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DWORD dwDLSVoices = 0;
    DWORD dwWaveVoices = 0;
    DWORD dwStreamVoices = 0;
    DWORD dwFreeVoices = m_VoicesFree.GetCount() + m_VoicesExtra.GetCount();
    pVoice = m_VoicesInUse.GetHead();
    for (;pVoice != NULL;pVoice = pVoice->GetNext())
    {
        if (pVoice->m_fWave)
        {
            if (pVoice->m_fStreamed)
            {
                dwStreamVoices++;
            }
            else
            {
                dwWaveVoices++;
            }
        }
        else
        {
            dwDLSVoices++;
        }
    }

    m_dwDLSVoices = dwDLSVoices;
    m_dwWaveVoices = dwWaveVoices;
    m_dwStreamVoices = dwStreamVoices;
    m_dwFreeVoices = dwFreeVoices;
#endif
    ::LeaveCriticalSection(&m_CriticalSection);
}

CVoice *CSynth::OldestVoice()

{
    CVoice *pVoice;
    CVoice *pBest = NULL;
    pVoice = m_VoicesInUse.GetHead();
    pBest = pVoice;
    if (pBest)
    {
        pVoice = pVoice->GetNext();
        for (;pVoice;pVoice = pVoice->GetNext())
        {
            if (!pVoice->m_fTag)
            {
                if (pBest->m_fTag)
                {
                    pBest = pVoice;
                }
                else
                {
                    if (pVoice->m_dwPriority <= pBest->m_dwPriority)
                    {
                        if (pVoice->m_fNoteOn) 
                        {
                            if (pBest->m_fNoteOn)
                            {
                                if (pBest->m_rtStartTime > pVoice->m_rtStartTime)
                                {
                                    pBest = pVoice;
                                }
                            }
                        }
                        else
                        {
                            if (pBest->m_fNoteOn ||
                                (pBest->m_rtStopTime < pVoice->m_rtStopTime))
                            {
                                pBest = pVoice;
                            }
                        }
                    }
                }
            }
        }
        if (pBest->m_fTag)
        {
            pBest = NULL;
        }
    }
    return pBest;
}

CVoice *CSynth::StealVoice(DWORD dwPriority)

{
    CVoice *pVoice;
    CVoice *pBest = NULL;
    pVoice = m_VoicesInUse.GetHead();
    for (;pVoice != NULL;pVoice = pVoice->GetNext())
    {
        if (pVoice->m_dwPriority <= dwPriority)
        {
            if (!pBest)
            {
                pBest = pVoice;
            }
            else
            {
                if (pVoice->m_fNoteOn == FALSE) 
                {
                    if ((pBest->m_fNoteOn == TRUE) ||
                        (pBest->m_rtStopTime > pVoice->m_rtStopTime))
                    {
                        pBest = pVoice;
                    }
                }
                else
                {
                    if (pBest->m_rtStartTime > pVoice->m_rtStartTime)
                    {
                        pBest = pVoice;
                    }
                }
            }
        }
    }
    if (pBest != NULL)
    {
        pBest->ClearVoice();
        pBest->m_fInUse = FALSE; 
        m_VoicesInUse.Remove(pBest);

        pBest->SetNext(NULL);
    }
    return pBest;
}

void CSynth::QueueVoice(CVoice *pVoice)

/*  This function queues a voice in the list of currently 
    synthesizing voices. It places them in the queue so that
    the higher priority voices are later in the queue. This
    allows the note stealing algorithm to take off the top of
    the queue.
    And, we want older playing notes to be later in the queue
    so the note ons and offs overlap properly. So, the queue is
    sorted in priority order with older notes later within one
    priority level.
*/

{
    CVoice *pScan = m_VoicesInUse.GetHead();
    CVoice *pNext = NULL;
    if (!pScan) // Empty list?
    {
        m_VoicesInUse.AddHead(pVoice);
        return;
    }
    if (pScan->m_dwPriority > pVoice->m_dwPriority)
    {   // Are we lower priority than the head of the list?
        m_VoicesInUse.AddHead(pVoice);
        return;
    }

    pNext = pScan->GetNext();
    for (;pNext;)
    {
        if (pNext->m_dwPriority > pVoice->m_dwPriority)
        {
            // Lower priority than next in the list.
            pScan->SetNext(pVoice);
            pVoice->SetNext(pNext);
            return;
        }
        pScan = pNext;
        pNext = pNext->GetNext();
    }
    // Reached the end of the list.
    pScan->SetNext(pVoice);
    pVoice->SetNext(NULL);
}

void CSynth::StealNotes(REFERENCE_TIME rtTime)

{
    CVoice *pVoice;
    long lToMove = m_nExtraVoices - m_VoicesExtra.GetCount();
    if (lToMove > 0)
    {
        for (;lToMove > 0;)
        {
            pVoice = m_VoicesFree.RemoveHead();
            if (pVoice != NULL)
            {
                m_VoicesExtra.AddHead(pVoice);
                lToMove--;
            }
            else break;
        }
        if (lToMove > 0)
        {
            pVoice = m_VoicesInUse.GetHead();
            for (;pVoice;pVoice = pVoice->GetNext())
            {
                if (pVoice->m_fTag) // Voice is already slated to be returned.
                {
                    lToMove--;
                }
            }
            for (;lToMove > 0;lToMove--)
            {
                pVoice = OldestVoice();
                if (pVoice != NULL)
                {
                    pVoice->QuickStopVoice(rtTime);
                }
                else break;
            }
        }
    }
}

STDMETHODIMP CSynth::Unload(HANDLE hDownload,
                       HRESULT ( CALLBACK *lpFreeMemory)(HANDLE,HANDLE),
                       HANDLE hUserData)
{
    return m_Instruments.Unload( hDownload, lpFreeMemory, hUserData);
}

STDMETHODIMP CSynth::Download(LPHANDLE phDownload, void * pdwData, LPBOOL bpFree) 

{
    return m_Instruments.Download( phDownload, (DWORD *) pdwData,  bpFree);
}

STDMETHODIMP CSynth::SendShortMsg(REFERENCE_TIME rt, DWORD dwGroup, DWORD dwMsg)
{
	::EnterCriticalSection(&m_CriticalSection);
    if ( rt == 0 ) // Special case of time == 0.
    {
        rt = m_rtLastTime;
    }

    REFERENCE_TIME rtNow;

    m_MasterClock.GetTime(&rtNow);
    m_dssequencer->SetTime(rtNow);

    if (dwGroup <= m_dwControlCount)
    {
        if (dwGroup == 0) // Play all groups if 0.
        {
            for (; dwGroup < m_dwControlCount; dwGroup++)
            {
                m_ppControl[dwGroup]->RecordMIDI(rt,(BYTE) (dwMsg & 0xFF), 
                    (BYTE)((dwMsg >> 8) & 0x7F),(BYTE)((dwMsg >> 16) & 0x7F));
            }
        }
        else
        {
            m_ppControl[dwGroup - 1]->RecordMIDI(rt,(BYTE) (dwMsg & 0xFF), 
                    (BYTE)((dwMsg >> 8) & 0x7F),(BYTE)((dwMsg >> 16) & 0x7F));
        }
    }
	::LeaveCriticalSection(&m_CriticalSection);
	return S_OK;
}



STDMETHODIMP CSynth::SendLongMsg(REFERENCE_TIME rt, DWORD dwGroup, BYTE *pbMsg, DWORD dwLength)
{
	::EnterCriticalSection(&m_CriticalSection);
    if ( rt == 0 ) // Special case of time == 0.
    {
        rt = m_rtLastTime;
    }


    REFERENCE_TIME rtNow;

    m_MasterClock.GetTime(&rtNow);
    m_dssequencer->SetTime(rtNow);

    if (dwGroup <= m_dwControlCount)
    {
        if (dwGroup == 0) // Play all groups if 0.
        {
            for (; dwGroup < m_dwControlCount; dwGroup++)
            {
                m_ppControl[dwGroup]->RecordSysEx(dwLength,pbMsg, rt); 
            }
        }
        else
        {
            m_ppControl[dwGroup - 1]->RecordSysEx(dwLength,pbMsg, rt); 
        }
    }
	::LeaveCriticalSection(&m_CriticalSection);
	return S_OK;
}

HRESULT CSynth::AllNotesOff()

{
    CVoice *pVoice;
    ::EnterCriticalSection(&m_CriticalSection);
    while (pVoice = m_VoicesInUse.RemoveHead())
    {
        pVoice->ClearVoice();
        pVoice->m_fInUse = FALSE; 
        m_VoicesFree.AddHead(pVoice);
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return (S_OK);
}

STDMETHODIMP CSynth::SetChannelPriority(
    DWORD dwChannelGroup, 
    DWORD dwChannel, 
    DWORD dwPriority)
{
    HRESULT hr = S_OK;

    ::EnterCriticalSection(&m_CriticalSection);
    
    dwChannelGroup--;
    if ((dwChannelGroup >= m_dwControlCount) || (dwChannel > 15))
    {
        Trace(1,"Error: Request to set channel priority on channel group %ld, channel %ld, is out of range.\n",
            dwChannelGroup,dwChannel);
        hr = E_INVALIDARG;
    }
    else
    {
        if (m_ppControl)
        {
            hr = m_ppControl[dwChannelGroup]->SetChannelPriority(dwChannel,dwPriority);
        }
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CSynth::GetChannelPriority(
    DWORD dwChannelGroup, 
    DWORD dwChannel, 
    LPDWORD pdwPriority)
{
    HRESULT hr = S_OK;

    ::EnterCriticalSection(&m_CriticalSection);

    dwChannelGroup--;
    if ((dwChannelGroup >= m_dwControlCount) || (dwChannel > 15))
    {
        Trace(1,"Error: Request to get channel priority on channel group %ld, channel %ld, is out of range.\n",
            dwChannelGroup,dwChannel);
        hr = E_INVALIDARG;
    }
    else
    {
        if (m_ppControl)
        {
            hr = m_ppControl[dwChannelGroup]->GetChannelPriority(dwChannel,pdwPriority);
        }
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}


//////////////////////////////////////////////////////////
// Directx8 Methods 

STDMETHODIMP CSynth::PlayWave(
    REFERENCE_TIME rt, 
    DWORD dwChannelGroup, 
    DWORD dwChannel, 
    PREL prPitch,
    VREL vrVolume,
    SAMPLE_TIME stVoiceStart,
    SAMPLE_TIME stLoopStart,
    SAMPLE_TIME stLoopEnd,
    IDirectSoundWave *pIWave, 
    DWORD *pdwVoiceID 
    )
{
    HRESULT hr = S_OK;
    ::EnterCriticalSection(&m_CriticalSection);

    dwChannelGroup--;
    if ((dwChannelGroup >= m_dwControlCount) || (dwChannel > 15))
    {
        Trace(1,"Error: Request to set play voice on channel group %ld, channel %ld, is out of range.\n",
            dwChannelGroup,dwChannel);
        hr = E_INVALIDARG;
    }

    if ( rt == 0 ) // Special case of time == 0.
    {
        rt = m_rtLastTime;
    }

    m_ppControl[dwChannelGroup]->RecordWaveEvent(
        rt, 
        (BYTE)(dwChannel & 0xF), 
        vrVolume, 
        prPitch, 
        stVoiceStart,
        stLoopStart,
        stLoopEnd,
        pIWave,
        pdwVoiceID);

    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CSynth::StopWave(
    REFERENCE_TIME rt, 
    DWORD dwVoiceId )
{
    HRESULT hr = S_OK;
    STIME stTime;

    ::EnterCriticalSection(&m_CriticalSection);

    if ( rt == 0 ) // Special case of time == 0.
    {
        rt = m_rtLastTime;
    }

    CVoice * pVoice = m_VoicesInUse.GetHead();
    for (;pVoice != NULL;pVoice = pVoice->GetNext())
    {
        if ( pVoice->m_dwVoiceId == dwVoiceId )
        {
            pVoice->StopVoice(rt);
            if (pVoice->m_fInUse == FALSE) 
            {
                m_VoicesInUse.Remove(pVoice);
                m_VoicesFree.AddHead(pVoice);
            }
            break;
        }
    }

    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CSynth::AssignChannelToOutput(DWORD dwChannelGroup, DWORD dwChannel, IDirectSoundBuffer *pBuffer,DWORD dwMixBins, BYTE *pbControllers  )
{
    HRESULT hr = S_OK;

    ::EnterCriticalSection(&m_CriticalSection);

    dwChannelGroup--;
    if ((dwChannelGroup >= m_dwControlCount) || (dwChannel > 15))
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        hr = m_ppControl[dwChannelGroup]->AssignChannelToOutput(dwChannel, pBuffer, dwMixBins, pbControllers);
    }

    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
} 

CClock::CClock()

{
    m_pParent = NULL;
    m_dwLastPosition = 0;
    m_llSampleTime = 0;
}


STDMETHODIMP CClock::QueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUnknown || iid == IID_IReferenceClock)
    {
        *ppv = static_cast<IReferenceClock*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    
    return S_OK;
}

STDMETHODIMP_(ULONG) CClock::AddRef()
{
    return m_pParent->AddRef();
}

STDMETHODIMP_(ULONG) CClock::Release()
{
    return m_pParent->Release();
}

STDMETHODIMP CClock::AdviseTime(REFERENCE_TIME ,REFERENCE_TIME,HANDLE,DWORD *)
{
    return E_NOTIMPL;
}

STDMETHODIMP CClock::AdvisePeriodic(REFERENCE_TIME,REFERENCE_TIME,HANDLE,DWORD * )
{
    return E_NOTIMPL;
}

STDMETHODIMP CClock::Unadvise( DWORD )
{
    return E_NOTIMPL;
}

EXTERN_C DWORD WINAPI DirectSoundGetSampleTime(void);

STDMETHODIMP CClock::GetTime(LPREFERENCE_TIME pTime)
{
    if( pTime == NULL )
    {
        return E_INVALIDARG;
    }

	EnterCriticalSection( &m_pParent->m_CriticalSection );

    DWORD dwPosition = DirectSoundGetSampleTime();

    // Check to see if we looped around, which happens every 24 hours

    LONGLONG delta;
    if(dwPosition < m_dwLastPosition){
        // This is either a loop, or a dsound bug

        if(dwPosition < 100000 && m_dwLastPosition > (1 << 30) ){
            Trace(-1, "CClock::GetTime sample time wrapped (as it should every 24 hours. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = ((((LONGLONG) 1) << 32) + dwPosition) - m_dwLastPosition;
        }
        else {
            Trace(-1, "CClock::GetTime stream position decreased unexpectedly. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = 0; // Ignore.
            m_dwLastPosition = dwPosition; // Catch up. (Sometimes DSound resets the stream on us.)
        }
    }
    else {
        delta = dwPosition - m_dwLastPosition;
    }

    m_dwLastPosition = dwPosition;
    m_llSampleTime += delta;
    LONGLONG llTemp = m_llSampleTime * 10000;
	llTemp /= 48000;
	llTemp *= 1000;
    *pTime = llTemp;
	LeaveCriticalSection( &m_pParent->m_CriticalSection );
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// DirectMusicSynthCreate

STDAPI DirectMusicSynthCreate(REFIID iid, void ** ppDMSynth )
{
    CSynth *pSynth = new CSynth;
    if (pSynth)
    {
        return pSynth->QueryInterface(iid,ppDMSynth);
    }
    return E_OUTOFMEMORY;
}


