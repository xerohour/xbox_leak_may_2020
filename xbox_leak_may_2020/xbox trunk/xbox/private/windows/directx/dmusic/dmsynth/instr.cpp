//      Copyright (c) 1996-1999 Microsoft Corporation
//      Instrument.cpp
//

#include "simple.h"
#include <mmsystem.h>
#include <dmerror.h>
#include "synth.h"
#include "math.h"
#include "debug.h"
#include "..\shared\validate.h"
#include <dmusicf.h>
void MemDump(char * prompt);

CSourceLFO::CSourceLFO()

{
    Init(0);
}

void CSourceLFO::Init(DWORD dwMode)

{
    m_Registers.dwLFO = dwMode;
    m_Registers.lAmplitudeModulation = 0;
    m_Registers.dwDelay = 0;
    m_Registers.dwDelta = 5 * 65536 / 1500; // 5hz.
    m_Registers.lFilterCutOffRange = 0;
    m_Registers.lPitchModulation = 0;
//    m_pfFrequency = (256 * 4096 * 16 * 5) / dwSampleRate;
//    m_stDelay = 0;
    m_prMWPitchScale = 0;
    m_vrMWVolumeScale = 0;
    m_vrVolumeScale = 0;
    m_prPitchScale = 0;
    m_prCPPitchScale = 0;
    m_vrCPVolumeScale = 0;
    m_prCutoffScale = 0;
    m_prMWCutoffScale = 0;
    m_prCPCutoffScale = 0;    
}

void CSourceLFO::Verify()

{
//    FORCEBOUNDS(m_pfFrequency,64,7600);
//    FORCEBOUNDS(m_stDelay,0,441000);
    FORCEBOUNDS(m_vrVolumeScale,-120,120);
    FORCEBOUNDS(m_vrMWVolumeScale,-120,120);
    FORCEBOUNDS(m_prPitchScale,-1200,1200);
    FORCEBOUNDS(m_prMWPitchScale,-1200,1200);
    FORCEBOUNDS(m_prCPPitchScale,-1200,1200);
    FORCEBOUNDS(m_vrCPVolumeScale,-120,120);
    FORCEBOUNDS(m_prCutoffScale, -12800, 12800);
    FORCEBOUNDS(m_prMWCutoffScale, -12800, 12800);
    FORCEBOUNDS(m_prCPCutoffScale, -12800, 12800);
    FORCEBOUNDS(m_Registers.lAmplitudeModulation,-128,127);
    FORCEBOUNDS(m_Registers.dwDelay,0,0x7FFF);
    FORCEBOUNDS(m_Registers.lFilterCutOffRange,-128,127);
    FORCEBOUNDS(m_Registers.lPitchModulation,-128,127);
    FORCEBOUNDS(m_Registers.dwDelta,0,0x3FF);
}

CSourceEG::CSourceEG()

{
    Init(0);
}

void CSourceEG::Init(DWORD dwEG)

{
    m_trVelAttackScale = 0;
    m_trKeyDecayScale = 0;
    m_trKeyHoldScale = 0;
    m_prCutoffScale = 0;
    m_Registers.dwAttack = 0;
    m_Registers.dwDecay = 0;
    m_Registers.dwDelay = 0;
    m_Registers.dwEG = dwEG;
    m_Registers.lFilterCutOff = 0;
    m_Registers.dwHold = 0;
    m_Registers.dwMode = DSEG_MODE_DELAY;
    m_Registers.lPitchScale = 0;
    m_Registers.dwRelease = 0;
    m_Registers.dwSustain = 255;
}


void CSourceEG::Verify()

{
//    FORCEBOUNDS(m_stAttack,0,1764000);
//    FORCEBOUNDS(m_stDecay,0,1764000);
//    FORCEBOUNDS(m_pcSustain,0,1000);
//    FORCEBOUNDS(m_stRelease,0,1764000);
//    FORCEBOUNDS(m_sScale,-1200,1200);
    FORCEBOUNDS(m_trKeyDecayScale,-12000,12000);
    FORCEBOUNDS(m_trVelAttackScale,-12000,12000);
    FORCEBOUNDS(m_trKeyHoldScale,-12000,12000);
    FORCEBOUNDS(m_prCutoffScale,-12800,12800);
    FORCEBOUNDS(m_Registers.dwAttack,0,3750);
    FORCEBOUNDS(m_Registers.dwDecay,0,3750);
    FORCEBOUNDS(m_Registers.dwDelay,0,3750);
    FORCEBOUNDS(m_Registers.lFilterCutOff,-128,127);
    FORCEBOUNDS(m_Registers.dwHold,0,3750);
    FORCEBOUNDS(m_Registers.lPitchScale,-128,127);
    FORCEBOUNDS(m_Registers.dwRelease,0,3750);
    FORCEBOUNDS(m_Registers.dwSustain,0,255);
}

CSourceFilter::CSourceFilter()
{
    Init();
}

void CSourceFilter::Init()
{
    // First, calculate the playback samplerate in pitch rels. 
    // The reference frequency is a440, which is midi note 69.
    // So, calculate the ratio of the sample rate to 440 and
    // convert into prels (1200 per octave), then add the 
    // offset of 6900.
//    double fSampleRate = (double)48000;

//    fSampleRate /= 440.0;
//    fSampleRate = log(fSampleRate) / log(2.0);
//    fSampleRate *= 1200.0;
//    fSampleRate += 6900.0;
//    m_prSampleRate = (PRELS)fSampleRate;

    m_prCutoff = (PRELS)0x7FFF;
    m_vrQ = (VRELS)0;
    m_prVelScale = (PRELS)0;
    m_prKeyScale = (PRELS)0;
//    m_prCutoffSRAdjust = 0;
//    m_iQIndex = 0;
}


void CSourceFilter::Verify()
{
    if ( m_prCutoff == 0x7FFF )
    {
        m_vrQ = 0;
        m_prVelScale = 0;
        m_prKeyScale = 0;
    }
    else
    {
        FORCEBOUNDS(m_prCutoff, 5535, 11921);
        FORCEBOUNDS(m_vrQ, 0, 225);
        FORCEBOUNDS(m_prVelScale, -12800, 12800);
    }
}

CSourceArticulation::CSourceArticulation()

{
//    m_sVelToVolScale = -9600;
    m_wUsageCount = 0;
    m_sDefaultPan = 0;
//    m_dwSampleRate = 22050;
//    m_PitchEG.m_sScale = 0; // pitch envelope defaults to off
}

void CSourceArticulation::Init(BOOL fWave)

{
    m_VolumeEG.Init(DSEG_AMPLITUDE);
    m_PitchEG.Init(DSEG_MULTI);
    m_LFO.Init(DSLFO_MULTI);       
    m_LFO2.Init(DSLFO_PITCH);      
    m_Filter.Init();
    if (fWave)
    {
        // For Wave, set the volume attack and release to fade in and out very quickly.
        m_VolumeEG.m_Registers.dwRelease = 1;
        m_VolumeEG.m_Registers.dwDecay = 1;
        // And turn off the pitch/filter envelope.
        m_PitchEG.m_Registers.dwMode = DSEG_MODE_DISABLE ;
    }
}

void CSourceArticulation::Verify()

{
    m_LFO.Verify();
    m_PitchEG.Verify();
    m_VolumeEG.Verify();
    m_LFO2.Verify();
    m_Filter.Verify();
}

void CSourceArticulation::AddRef()

{
    m_wUsageCount++;
}

void CSourceArticulation::Release()

{
    m_wUsageCount--;
    if (m_wUsageCount == 0)
    {
        delete this;
    }
}

CSourceSample::CSourceSample()

{
    m_pWave = NULL;
    m_dwLoopStart = 0;
    m_dwLoopEnd = 1;
    m_dwLoopType = WLOOP_TYPE_FORWARD;
    m_dwSampleLength = 0;
    m_dwSampleDataSize = 0;
    m_prFineTune = 0;
    m_bMIDIRootKey = 60;
    m_bOneShot = TRUE;
}

CSourceSample::~CSourceSample()

{ 
    if (m_pWave != NULL)
    {
        m_pWave->Release();
    }
}

void CSourceSample::Verify()

{
    if (m_pWave != NULL)
    {
        FORCEBOUNDS(m_dwSampleLength,0,m_pWave->m_dwSampleLength);
        FORCEBOUNDS(m_dwLoopEnd,1,m_dwSampleLength);
        FORCEBOUNDS(m_dwLoopStart,0,m_dwLoopEnd);
        if ((m_dwLoopEnd - m_dwLoopStart) < 6) 
        {
            m_bOneShot = TRUE;
        }
    }
    FORCEBOUNDS(m_bMIDIRootKey,0,127);
    FORCEBOUNDS(m_prFineTune,-1200,1200);
}

BOOL CSourceSample::CopyFromWave()

{
    if (m_pWave == NULL)
    {
        return FALSE;
    }
    m_dwSampleLength = m_pWave->m_dwSampleLength;
    m_dwSampleDataSize = m_pWave->m_dwSampleDataSize;
    if (!m_bOneShot)
    {
        if (m_dwLoopStart >= m_dwSampleLength)
        {
            m_dwLoopStart = 0;
        }
    }
    Verify();
    return (TRUE);
}


CWave::CWave()
{
    m_hUserData = NULL;
    m_lpFreeHandle = NULL;
    m_pnWave = NULL;
    m_dwSampleLength = 0;
    m_dwSampleDataSize = 0;
    m_wUsageCount = 0;
    m_dwID = 0;
    m_wPlayCount = 0;
}

CWave::~CWave()

{
    if (m_pnWave)
    {
        if (m_lpFreeHandle)
        {
            m_lpFreeHandle((HANDLE) this,m_hUserData);
        } 
        else 
        {
            delete [] m_pnWave;
        }
    }   
}

void CWave::Verify()

{
}

void CWave::AddRef()

{
    m_wUsageCount++;
}

void CWave::Release()

{
    m_wUsageCount--;
    if (m_wUsageCount == 0)
    {
        delete this;
    }
}

CSourceRegion::CSourceRegion()
{
    m_pArticulation = NULL;
    m_vrAttenuation = 0;
    m_prTuning = 0;
    m_bKeyHigh = 127;
    m_bKeyLow = 0;
    m_bGroup = 0;
    m_bAllowOverlap = FALSE;
    m_bVelocityHigh = 127;
    m_bVelocityLow  = 0;
    m_dwChannel = 0;
    m_sWaveLinkOptions = 0;
}

CSourceRegion::~CSourceRegion()
{
    if (m_pArticulation)
    {
        m_pArticulation->Release();
    }
}

void CSourceRegion::Verify()

{
    FORCEBOUNDS(m_bKeyHigh,0,127);
    FORCEBOUNDS(m_bKeyLow,0,127);
    FORCEBOUNDS(m_prTuning,-12000,12000);
    FORCEBOUNDS(m_vrAttenuation,-9600,0);
    m_Sample.Verify();
    if (m_pArticulation != NULL)
    {
        m_pArticulation->Verify();
    }
}

CInstrument::CInstrument() : m_MemTrack(DMTRACK_INSTRUMENT)
{
    m_dwProgram = 0;
}

CInstrument::~CInstrument()
{
    while (!m_RegionList.IsEmpty())
    {
        CSourceRegion *pRegion = m_RegionList.RemoveHead();
        delete pRegion;
    }
}

void CInstrument::Verify()

{
    CSourceRegion *pRegion = m_RegionList.GetHead();
    CSourceArticulation *pArticulation = NULL;
    for (;pRegion != NULL;pRegion = pRegion->GetNext())
    {
        if (pRegion->m_pArticulation != NULL)
        {
            pArticulation = pRegion->m_pArticulation;
        }
        pRegion->Verify();
    }
    pRegion = m_RegionList.GetHead();
    for (;pRegion != NULL;pRegion = pRegion->GetNext())
    {
        if (pRegion->m_pArticulation == NULL  && pArticulation)
        {
            pRegion->m_pArticulation = pArticulation;
            pArticulation->AddRef();
        }
    }
}

CSourceRegion * CInstrument::ScanForRegion(DWORD dwNoteValue, DWORD dwVelocity, CSourceRegion *pRegion)

{
    if ( pRegion == NULL )
        pRegion = m_RegionList.GetHead(); // Starting search 
    else
        pRegion = pRegion->GetNext();     // Continuing search through the rest of the regions

    for (;pRegion;pRegion = pRegion->GetNext())
    {
        if (dwNoteValue >= pRegion->m_bKeyLow  &&
            dwNoteValue <= pRegion->m_bKeyHigh && 
            dwVelocity  >= pRegion->m_bVelocityLow &&
            dwVelocity  <= pRegion->m_bVelocityHigh )
        {
            break ;
        }
    }
    return pRegion;
}

CInstManager::CInstManager()

{
    m_fCSInitialized = FALSE;
    InitializeCriticalSection(&m_CriticalSection);
    m_fCSInitialized = TRUE;
    m_dwSynthMemUse = 0;
}

CInstManager::~CInstManager()

{
    if (m_fCSInitialized)
    {
       	while (!m_CollectionList.IsEmpty())
        {
            CCollection *pCollection = m_CollectionList.RemoveHead();
            delete pCollection;
        }

        while (!m_FreeWavePool.IsEmpty()) 
        {
            CWave *pWave = m_FreeWavePool.RemoveHead();
            pWave->Release();
        }
    
/*        for(int nCount = 0; nCount < WAVEART_HASH_SIZE; nCount++)
        {
            while(!m_WaveArtList[nCount].IsEmpty())
            {
                CWaveArt* pWaveArt = m_WaveArtList[nCount].RemoveHead();
                if(pWaveArt)
                {
                    pWaveArt->Release();
                }
            }
        }*/

        DeleteCriticalSection(&m_CriticalSection);
    }
}

void CInstManager::Verify()

{
    DWORD dwIndex;
    EnterCriticalSection(&m_CriticalSection);
    CCollection *pCollection = m_CollectionList.GetHead();
    for(;pCollection;pCollection = pCollection->GetNext())
    {
        for (dwIndex = 0;dwIndex < INSTRUMENT_HASH_SIZE; dwIndex++)
        {
            CInstrument *pInstrument = pCollection->m_InstrumentList[dwIndex].GetHead();
            for (;pInstrument != NULL;pInstrument = pInstrument->GetNext())
            {
                pInstrument->Verify();
            }
        }
    }
    LeaveCriticalSection(&m_CriticalSection);
}

CInstrument * CInstManager::GetInstrument(DWORD dwProgram, DWORD dwKey, DWORD dwVelocity)

{
    CInstrument *pInstrument = NULL;
    EnterCriticalSection(&m_CriticalSection);
    // The instrument must reside in a collection that was assigned to this synth...
    CCollection *pCollection = m_CollectionList.GetHead();
    for (;pCollection != NULL; pCollection = pCollection->GetNext())
    {
        pInstrument = pCollection->m_InstrumentList[dwProgram % INSTRUMENT_HASH_SIZE].GetHead();
        for (;pInstrument != NULL; pInstrument = pInstrument->GetNext())
        {
            if (pInstrument->m_dwProgram == dwProgram) 
            {
                if (pInstrument->ScanForRegion(dwKey, dwVelocity, NULL) != NULL)
                {
                    break;
                }
                else
                {
                    Trace(2,"Warning: No region was found in instrument # %lx that matched note %ld\n",
                        dwProgram,dwKey);
                }
            }
        }
        if (pInstrument) break;
    }
    if (!pInstrument)
    {
        // If not found in collections, this could be a downloaded instrument for XBProducer...
        pInstrument = m_InstrumentList.GetHead();
        for (;pInstrument != NULL; pInstrument = pInstrument->GetNext())
        {
            if (pInstrument->m_dwProgram == dwProgram) 
            {
                if (pInstrument->ScanForRegion(dwKey, dwVelocity, NULL) != NULL)
                {
                    break;
                }
            }
        }
    }
    LeaveCriticalSection(&m_CriticalSection);
    return (pInstrument);
}


DWORD TimeCents2Samples(long tcTime, DWORD dwSampleRate)
{
    if (tcTime ==  0x80000000) return (0);
    double flTemp = tcTime;
    flTemp /= (65536 * 1200);
    flTemp = pow(2.0,flTemp);
    flTemp *= dwSampleRate;
    return (DWORD) flTemp;
}

DWORD PitchCents2PitchFract(long pcRate,DWORD dwSampleRate)

{
    double fTemp = pcRate;
    fTemp /= 65536;
    fTemp -= 6900;
    fTemp /= 1200;
    fTemp = pow(2.0,fTemp);
    fTemp *= 7381975040.0; // (440*256*16*4096);
    fTemp /= dwSampleRate;
    return (DWORD) (fTemp);
}

HRESULT CSourceArticulation::Download(DMUS_DOWNLOADINFO * pInfo, 
                                void * pvOffsetTable[], 
                                DWORD dwIndex,
                                BOOL fNewFormat)
{
    if (fNewFormat)
    {
        DMUS_ARTICULATION2 * pdmArtic = 
            (DMUS_ARTICULATION2 *) pvOffsetTable[dwIndex];
        while (pdmArtic)
        {
            if (pdmArtic->ulArtIdx)
            {
                if (pdmArtic->ulArtIdx >= pInfo->dwNumOffsetTableEntries)
                {
                    Trace(1,"Error: Download failed because articulation chunk has an error.\n");
                    return DMUS_E_BADARTICULATION;
                }           
                DWORD dwPosition;
                void *pData = pvOffsetTable[pdmArtic->ulArtIdx];
                CONNECTIONLIST * pConnectionList = 
                    (CONNECTIONLIST *) pData;
                CONNECTION *pConnection;
                dwPosition = sizeof(CONNECTIONLIST);
                for (dwIndex = 0; dwIndex < pConnectionList->cConnections; dwIndex++)
                {
                    pConnection = (CONNECTION *) ((BYTE *)pData + dwPosition);
                    dwPosition += sizeof(CONNECTION);
                    switch (pConnection->usSource)
                    {
                    case CONN_SRC_NONE :
                        switch (pConnection->usDestination)
                        {
                        case CONN_DST_LFO_FREQUENCY :
                            m_LFO.m_Registers.dwDelta = PitchCents2PitchFract(
                                pConnection->lScale,6000*64); // 48000*64 / 8)
                            break;
                        case CONN_DST_LFO_STARTDELAY :
                            m_LFO.m_Registers.dwDelay = (DWORD) TimeCents2Samples(
                                (TCENT) pConnection->lScale,1500); // 48000 / 32
                            break;
                        case CONN_DST_EG1_ATTACKTIME :
                            m_VolumeEG.m_Registers.dwAttack = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94); // 48000 / 512
                            break;
                        case CONN_DST_EG1_DECAYTIME :
                            m_VolumeEG.m_Registers.dwDecay = TimeCents2Samples(
                            (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG1_SUSTAINLEVEL :
                            {
                                // The nvidia chip takes the sustain level in 
                                // absolute volume, not dB, so first convert.
                                long lTemp = (pConnection->lScale >> 16) - 1000;
                                lTemp *= 9600;
                                lTemp /= 1000;
                                lTemp = CDigitalAudio::VRELToVFRACT(lTemp);
                                m_VolumeEG.m_Registers.dwSustain = lTemp >> 4;
                            }
                            break;
                        case CONN_DST_EG1_RELEASETIME :
                            m_VolumeEG.m_Registers.dwRelease = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG2_ATTACKTIME :
                            m_PitchEG.m_Registers.dwAttack = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG2_DECAYTIME :
                            m_PitchEG.m_Registers.dwDecay = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG2_SUSTAINLEVEL :
                            m_PitchEG.m_Registers.dwSustain =
                                (SPERCENT) ((long) (pConnection->lScale / 257003));
                            break;
                        case CONN_DST_EG2_RELEASETIME :
                            m_PitchEG.m_Registers.dwRelease = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94); 
                            break;
                        case CONN_DST_PAN :
                            m_sDefaultPan = (short) 
                                ((long) ((long) pConnection->lScale >> 12) / 125);
                            break;

                        /* DLS2 */
                        case CONN_DST_EG1_DELAYTIME:
                            m_VolumeEG.m_Registers.dwDelay = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG1_HOLDTIME:
                            m_VolumeEG.m_Registers.dwHold  = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG2_DELAYTIME:
                            m_PitchEG.m_Registers.dwDelay  = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_EG2_HOLDTIME:
                            m_PitchEG.m_Registers.dwHold = TimeCents2Samples(
                                (TCENT) pConnection->lScale,94);
                            break;
                        case CONN_DST_VIB_FREQUENCY :
                            m_LFO2.m_Registers.dwDelta = PitchCents2PitchFract(
                                pConnection->lScale,6000*64);
                            break;    
                        case CONN_DST_VIB_STARTDELAY :
                            m_LFO2.m_Registers.dwDelay = TimeCents2Samples(
                                (TCENT) pConnection->lScale,1500);
                            break;                                
                        case CONN_DST_FILTER_CUTOFF:
                            // First, get the filter cutoff frequency, which is relative to a440.
                            m_Filter.m_prCutoff = (PRELS)
                                (pConnection->lScale >> 16);
                            // Then, calculate the resulting prel, taking into consideration
                            // the sample rate and the base of the filter coefficient lookup
                            // table, relative to the sample rate (FILTER_FREQ_RANGE).
                            // This number can then be used directly look up the coefficients in the
                            // filter table. 
         //                   m_Filter.m_prCutoffSRAdjust = (PRELS)
         //                       FILTER_FREQ_RANGE - m_Filter.m_prSampleRate + m_Filter.m_prCutoff; 
                            break;
                        case CONN_DST_FILTER_Q:
                            m_Filter.m_vrQ = (VRELS)
                                (pConnection->lScale >> 16); //>>>>>>>> not really VRELS, but 1/10th's
         //                   m_Filter.m_iQIndex = (DWORD)
         //                       ((m_Filter.m_vrQ / 15.0f) + 0.5f);
                            break;
                        }
                        break;
                    case CONN_SRC_LFO :
                        switch (pConnection->usControl)
                        {
                        case CONN_SRC_NONE :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_ATTENUATION :
                                m_LFO.m_vrVolumeScale = (VRELS)
                                    ((long) (pConnection->lScale >> 16));
                                m_LFO.m_Registers.lAmplitudeModulation = pConnection->lScale / 38400;
                                break;
                            case CONN_DST_PITCH :
                                m_LFO.m_prPitchScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                m_LFO.m_Registers.lPitchModulation = pConnection->lScale / 614400;
                                break;
                            case CONN_DST_FILTER_CUTOFF:
                                m_LFO.m_prCutoffScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                m_LFO.m_Registers.lFilterCutOffRange = pConnection->lScale / 4915200; 
                                break;
                            }
                            break;
                        case CONN_SRC_CC1 :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_ATTENUATION :
                                m_LFO.m_vrMWVolumeScale = (VRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            case CONN_DST_PITCH :
                                m_LFO.m_prMWPitchScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                break;

                            /* DLS2 */
                            case CONN_DST_FILTER_CUTOFF:
                                m_LFO.m_prMWCutoffScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            }
                            break;

                        /* DLS2 */
                        case CONN_SRC_CHANNELPRESSURE :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_ATTENUATION :
                                m_LFO.m_vrCPVolumeScale = (VRELS) 
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            case CONN_DST_PITCH :
                                m_LFO.m_prCPPitchScale  = (PRELS) 
                                    ((long) (pConnection->lScale >> 16)); 
                                break;

                            /* DLS2 */
                            case CONN_DST_FILTER_CUTOFF:
                                m_LFO.m_prCPCutoffScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            }
                            break;
                        }
                        break;
                    case CONN_SRC_KEYONVELOCITY :
                        switch (pConnection->usDestination)
                        {
                        case CONN_DST_EG1_ATTACKTIME :
                            m_VolumeEG.m_trVelAttackScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;
                        case CONN_DST_EG2_ATTACKTIME :
                            m_PitchEG.m_trVelAttackScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;

                        /* DLS2 */
                        case CONN_DST_FILTER_CUTOFF:
                            m_Filter.m_prVelScale = (PRELS)
                                ((long) (pConnection->lScale >> 16)); 
                            break;
                        }
                        break;
                    case CONN_SRC_KEYNUMBER :
                        switch (pConnection->usDestination)
                        {
                        case CONN_DST_EG1_DECAYTIME :
                            m_VolumeEG.m_trKeyDecayScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;
                        case CONN_DST_EG2_DECAYTIME :
                            m_PitchEG.m_trKeyDecayScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;

                        /* DLS2 */
                        case CONN_DST_EG1_HOLDTIME :
                            m_PitchEG.m_trKeyDecayScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;
                        case CONN_DST_EG2_HOLDTIME :
                            m_PitchEG.m_trKeyDecayScale = (TRELS)
                                ((long) (pConnection->lScale >> 16));
                        case CONN_DST_FILTER_CUTOFF :
                            m_Filter.m_prKeyScale = (PRELS)
                                ((long) (pConnection->lScale >> 16));
                            break;
                        }
                        break;
                    case CONN_SRC_EG2 :
                        switch (pConnection->usDestination)
                        {
                        case CONN_DST_PITCH :
                            m_PitchEG.m_Registers.lPitchScale = pConnection->lScale / 614400;
                            break;

                        /* DLS2 */
                        case CONN_DST_FILTER_CUTOFF:
                            m_PitchEG.m_Registers.lFilterCutOff = pConnection->lScale / 4915200; 
                            break;
                        }
                        break;      

                    /* DLS2 */
                    case CONN_SRC_VIBRATO :
                        switch (pConnection->usControl)
                        {
                        case CONN_SRC_NONE :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_PITCH :
                                m_LFO2.m_prPitchScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                m_LFO2.m_Registers.lPitchModulation = pConnection->lScale / 614400;
                                break;
                            }
                            break;
                        case CONN_SRC_CC1 :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_PITCH :
                                m_LFO2.m_prMWPitchScale = (PRELS)
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            }
                            break;
                        case CONN_SRC_CHANNELPRESSURE :
                            switch (pConnection->usDestination)
                            {
                            case CONN_DST_PITCH :
                                m_LFO2.m_prCPPitchScale  = (PRELS) 
                                    ((long) (pConnection->lScale >> 16)); 
                                break;
                            }
                            break;
                        }
                        break;
                    }
                }
            }
            if (pdmArtic->ulNextArtIdx)
            {
                if (pdmArtic->ulNextArtIdx >= pInfo->dwNumOffsetTableEntries)
                {
                    Trace(1,"Error: Download failed because articulation chunk has an error.\n");
                    return DMUS_E_BADARTICULATION;
                }
                pdmArtic = (DMUS_ARTICULATION2 *) pvOffsetTable[pdmArtic->ulNextArtIdx];
            }
            else 
            {
                pdmArtic = NULL;
            }
        }
    }
    Verify();   // Make sure all parameters are legal.

    return S_OK;
}

HRESULT CSourceRegion::Download(DMUS_DOWNLOADINFO * pInfo, 
                                void * pvOffsetTable[], 
                                DWORD *pdwRegionIX,
                                BOOL fNewFormat)
{
    DMUS_REGION * pdmRegion = (DMUS_REGION *) pvOffsetTable[*pdwRegionIX];
    *pdwRegionIX = pdmRegion->ulNextRegionIdx;  // Clear to avoid loops.
    pdmRegion->ulNextRegionIdx = 0;
    // Read the Region chunk...
    m_bKeyHigh = (BYTE) pdmRegion->RangeKey.usHigh;
    m_bKeyLow  = (BYTE) pdmRegion->RangeKey.usLow;
    m_bVelocityHigh = (BYTE) pdmRegion->RangeVelocity.usHigh;
    m_bVelocityLow  = (BYTE) pdmRegion->RangeVelocity.usLow;

    //
    // Fix DLS Designer bug
    // Designer was putting velocity ranges that fail
    // on DLS2 synths
    //
    if ( m_bVelocityHigh == 0 && m_bVelocityLow == 0 )
        m_bVelocityHigh = 127;

    if (pdmRegion->fusOptions & F_RGN_OPTION_SELFNONEXCLUSIVE)
    {
        m_bAllowOverlap = TRUE;
    }
    else
    {
        m_bAllowOverlap = FALSE;
    }
    m_bGroup = (BYTE) pdmRegion->usKeyGroup;
    // Now, the WSMP and WLOOP chunks...
    m_vrAttenuation = (short) ((long) ((pdmRegion->WSMP.lAttenuation) * 10) >> 16);
    m_Sample.m_prFineTune = pdmRegion->WSMP.sFineTune;
    m_Sample.m_bMIDIRootKey = (BYTE) pdmRegion->WSMP.usUnityNote;
    if (pdmRegion->WSMP.cSampleLoops == 0)
    {
        m_Sample.m_bOneShot = TRUE;
    }
    else
    {
        m_Sample.m_dwLoopStart = pdmRegion->WLOOP[0].ulStart;
        m_Sample.m_dwLoopEnd = m_Sample.m_dwLoopStart + pdmRegion->WLOOP[0].ulLength;
        m_Sample.m_bOneShot = FALSE;
        m_Sample.m_dwLoopType = pdmRegion->WLOOP[0].ulType;
    }

    m_sWaveLinkOptions = pdmRegion->WaveLink.fusOptions;
    m_dwChannel = pdmRegion->WaveLink.ulChannel;
    
    if ( (m_dwChannel != WAVELINK_CHANNEL_LEFT) && !IsMultiChannel() )
    {
        Trace(1, "Download failed: Attempt to use a non-mono channel without setting the multichannel flag.\n");
        return DMUS_E_NOTMONO;
    }
    
    m_Sample.m_wID = (WORD) pdmRegion->WaveLink.ulTableIndex;
    
    // Does it have its own articulation?
    //
    if (pdmRegion->ulRegionArtIdx )
    {
        if (pdmRegion->ulRegionArtIdx >= pInfo->dwNumOffsetTableEntries)
        {
            Trace(1,"Error: Download failed because articulation chunk has an error.\n");
            return DMUS_E_BADARTICULATION;
        }

        CSourceArticulation *pArticulation = new CSourceArticulation;
        if (pArticulation)
        {
            pArticulation->Init(FALSE);
            HRESULT hr = pArticulation->Download(pInfo, pvOffsetTable, 
                    pdmRegion->ulRegionArtIdx, fNewFormat);

            if (FAILED(hr))
            {
                delete pArticulation;
                return hr;
            }
            m_pArticulation = pArticulation;
            m_pArticulation->AddRef();
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }
    return S_OK;
}



HRESULT CInstManager::DownloadInstrument(LPHANDLE phDownload, 
                                         DMUS_DOWNLOADINFO *pInfo, 
                                         void *pvOffsetTable[], 
                                         void *pvData,
                                         BOOL fNewFormat)
                                         
{
    DMUS_INSTRUMENT *pdmInstrument = (DMUS_INSTRUMENT *) pvData;
    CInstrument *pInstrument = new CInstrument;
    if (pInstrument)
    {
        Trace(3,"Downloading instrument %lx\n",pdmInstrument->ulPatch);
        pInstrument->m_dwProgram = pdmInstrument->ulPatch;

        DWORD dwRegionIX = pdmInstrument->ulFirstRegionIdx;
        pdmInstrument->ulFirstRegionIdx = 0; // Clear to avoid loops.
        while (dwRegionIX)
        {
            if (dwRegionIX >= pInfo->dwNumOffsetTableEntries)
            {
                Trace(1,"Error: Download failed because instrument has error in region list.\n");
                delete pInstrument;
                return DMUS_E_BADINSTRUMENT;
            }
            CSourceRegion *pRegion = new CSourceRegion;
            if (!pRegion)
            {
                delete pInstrument;
                return E_OUTOFMEMORY;
            }
            pInstrument->m_RegionList.AddHead(pRegion);
            HRESULT hr = pRegion->Download(pInfo, pvOffsetTable, &dwRegionIX, fNewFormat);
            if (FAILED(hr))
            {
                delete pInstrument;
                return hr;
            }
            EnterCriticalSection(&m_CriticalSection);
            CWave *pWave = m_WavePool[pRegion->m_Sample.m_wID % WAVE_HASH_SIZE].GetHead();
            for (;pWave;pWave = pWave->GetNext())
            {
                if (pRegion->m_Sample.m_wID == pWave->m_dwID)
                {
                    pRegion->m_Sample.m_pWave = pWave;
                    pWave->AddRef();
                    pRegion->m_Sample.CopyFromWave();
                    break;
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
        }
        if (pdmInstrument->ulGlobalArtIdx)
        {
            if (pdmInstrument->ulGlobalArtIdx >= pInfo->dwNumOffsetTableEntries)
            {
                Trace(1,"Error: Download failed because of out of range articulation chunk.\n");
                delete pInstrument;
                return DMUS_E_BADARTICULATION;
            }

            CSourceArticulation *pArticulation = new CSourceArticulation;
            if (pArticulation)
            {
                pArticulation->Init(FALSE);
                HRESULT hr = pArticulation->Download(pInfo, pvOffsetTable, 
                        pdmInstrument->ulGlobalArtIdx, fNewFormat);
                if (FAILED(hr))
                {
                    delete pArticulation;
                    return hr;
                }
                for (CSourceRegion *pr = pInstrument->m_RegionList.GetHead();
                     pr != NULL;
                     pr = pr->GetNext())
                {
                    if (pr->m_pArticulation == NULL)
                    {
                        pr->m_pArticulation = pArticulation;
                        pArticulation->AddRef();    
                    }
                }
                if (!pArticulation->m_wUsageCount)
                {
                    delete pArticulation;
                }
            }
            else
            {
                return E_OUTOFMEMORY;
            }
        }
        else
        {
            for (CSourceRegion *pr = pInstrument->m_RegionList.GetHead();
                 pr != NULL;
                 pr = pr->GetNext())
            {
                if (pr->m_pArticulation == NULL)
                {
                    Trace(1,"Error: Download failed because region has no articulation.\n");
                    delete pInstrument;
                    return DMUS_E_NOARTICULATION;
                }
            }
        }
        EnterCriticalSection(&m_CriticalSection);
        m_InstrumentList.AddHead(pInstrument);
        LeaveCriticalSection(&m_CriticalSection);
        *phDownload = (HANDLE) pInstrument;
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT CInstManager::DownloadWave(LPHANDLE phDownload, 
                                   DMUS_DOWNLOADINFO *pInfo, 
                                   void *pvOffsetTable[], 
                                   void *pvData)
{
    DMUS_WAVE *pdmWave = (DMUS_WAVE *) pvData; 
    if ((pdmWave->WaveformatEx.wFormatTag != WAVE_FORMAT_PCM) &&
        (pdmWave->WaveformatEx.wFormatTag != WAVE_FORMAT_XBOX_ADPCM))
    {
        Trace(1,"Error: Download failed because wave data is not PCM format.\n");
        return DMUS_E_NOTPCM;
    }
    
    if (pdmWave->WaveformatEx.nChannels != 1)
    {
        Trace(1,"Error: Download failed because wave data is not mono.\n");
        return DMUS_E_NOTMONO;
    }

    if (pdmWave->ulWaveDataIdx >= pInfo->dwNumOffsetTableEntries)
    {
        Trace(1,"Error: Download failed because wave data is at invalid location.\n");
        return DMUS_E_BADWAVE;
    }

    CWave *pWave = new CWave;
    if (pWave)
    {   
        DMUS_WAVEDATA *pdmWaveData= (DMUS_WAVEDATA *) 
            pvOffsetTable[pdmWave->ulWaveDataIdx];
        pWave->m_dwID = pInfo->dwDLId;
        pWave->m_hUserData = NULL;
        pWave->m_lpFreeHandle = NULL;
        pWave->m_dwSampleLength = pdmWaveData->cbSize;
        pWave->m_dwSampleDataSize = pdmWaveData->cbSize;
        pWave->m_pnWave = (short *) &pdmWaveData->byData[0];
        pWave->m_WaveFormat.wfx = pdmWave->WaveformatEx;

        if (pdmWave->WaveformatEx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            pWave->m_WaveFormat.wSamplesPerBlock = 64;
        }
        EnterCriticalSection(&m_CriticalSection);
        m_WavePool[pWave->m_dwID % WAVE_HASH_SIZE].AddHead(pWave);
        LeaveCriticalSection(&m_CriticalSection);
        *phDownload = (HANDLE) pWave;
        pWave->AddRef();

        // Track memory usage 
        m_dwSynthMemUse += pWave->m_dwSampleDataSize;  
        Trace(3,"Downloading wave %ld memory usage %ld\n",pInfo->dwDLId,m_dwSynthMemUse);

        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT CInstManager::Download(LPHANDLE phDownload, 
                               void * pvData,
                               LPBOOL pbFree)
                               

{
    V_INAME(IDirectMusicSynthX::Download);
    V_BUFPTR_READ(pvData,sizeof(DMUS_DOWNLOADINFO));

    HRESULT hr = DMUS_E_UNKNOWNDOWNLOAD;
    void ** ppvOffsetTable;     // Array of pointers to chunks in data.
    DMUS_DOWNLOADINFO * pInfo = (DMUS_DOWNLOADINFO *) pvData;
    DMUS_OFFSETTABLE* pOffsetTable = (DMUS_OFFSETTABLE *)(((BYTE*)pvData) + sizeof(DMUS_DOWNLOADINFO));
    char *pcData = (char *) pvData;

    V_BUFPTR_READ(pvData,pInfo->cbSize);

    //Code fails if pInfo->dwNumOffsetTableEntries == 0
    //Saninty check here for debug
    assert(pInfo->dwNumOffsetTableEntries);
    
    ppvOffsetTable = new void *[pInfo->dwNumOffsetTableEntries];
    if (ppvOffsetTable) // Create the pointer array and validate.
    {
        DWORD dwIndex;
        for (dwIndex = 0; dwIndex < pInfo->dwNumOffsetTableEntries; dwIndex++)
        {
            if (pOffsetTable->ulOffsetTable[dwIndex] >= pInfo->cbSize)
            {
                delete[] ppvOffsetTable;
                Trace(1,"Error: Download failed because of corrupt download tables.\n");
                return DMUS_E_BADOFFSETTABLE;   // Bad!
            }
            ppvOffsetTable[dwIndex] = (void *) &pcData[pOffsetTable->ulOffsetTable[dwIndex]];
        }
        if (pInfo->dwDLType == DMUS_DOWNLOADINFO_INSTRUMENT) // Instrument.
        {
            *pbFree = TRUE;
            hr = DownloadInstrument(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0],FALSE); 
        }
        else if (pInfo->dwDLType == DMUS_DOWNLOADINFO_INSTRUMENT2) // New instrument format.
        {
            *pbFree = TRUE;
            hr = DownloadInstrument(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0],TRUE); 
        }
        else if (pInfo->dwDLType == DMUS_DOWNLOADINFO_WAVE) // Wave.
        {
            *pbFree = FALSE;
            hr = DownloadWave(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0]); 
        }
//        else if (pInfo->dwDLType == DMUS_DOWNLOADINFO_WAVEARTICULATION) // Wave onshot & streaming 
//        {
//            *pbFree = TRUE;
//            hr = DownloadWaveArticulation(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0]); 
//        }
//        else if (pInfo->dwDLType == DMUS_DOWNLOADINFO_STREAMINGWAVE) // Streaming 
//        {
//            *pbFree = FALSE;
//            hr = DownloadWaveRaw(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0]); 
//        }
//        else if (pInfo->dwDLType == DMUS_DOWNLOADINFO_ONESHOTWAVE) // Wave onshot
//        {
//            *pbFree = FALSE;
//            hr = DownloadWaveRaw(phDownload, pInfo, ppvOffsetTable, ppvOffsetTable[0]); 
//        }

        delete[] ppvOffsetTable;
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    return hr;
}

HRESULT CInstManager::Unload(HANDLE hDownload,
                             HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE),
                             HANDLE hUserData)

{
    DWORD dwIndex;
    EnterCriticalSection(&m_CriticalSection);
    CInstrument *pInstrument = m_InstrumentList.GetHead();
    for (;pInstrument != NULL; pInstrument = pInstrument->GetNext())
    {
        if (pInstrument == (CInstrument *) hDownload) 
        {
            Trace(3,"Unloading instrument %lx\n",pInstrument->m_dwProgram);
            m_InstrumentList.Remove(pInstrument);
            delete pInstrument;
            LeaveCriticalSection(&m_CriticalSection);
            return S_OK;
        }
    }
    for (dwIndex = 0; dwIndex < WAVE_HASH_SIZE; dwIndex++)
    {
        CWave *pWave = m_WavePool[dwIndex].GetHead();
        for (;pWave != NULL;pWave = pWave->GetNext())
        {
            if (pWave == (CWave *) hDownload)
            {
                // Track memory usage 
                m_dwSynthMemUse -= pWave->m_dwSampleDataSize;  

                Trace(3,"Unloading wave %ld memory usage %ld\n",pWave->m_dwID,m_dwSynthMemUse);
                m_WavePool[dwIndex].Remove(pWave);

                pWave->m_hUserData = hUserData;
                pWave->m_lpFreeHandle = lpFreeHandle;
                pWave->Release();
                LeaveCriticalSection(&m_CriticalSection);
                return S_OK;
            }
        }
    }
/*    for (dwIndex = 0; dwIndex < WAVE_HASH_SIZE; dwIndex++)
    {
        CWaveArt* pWaveArt = m_WaveArtList[dwIndex].GetHead();
        for (;pWaveArt != NULL;pWaveArt = pWaveArt->GetNext())
        {
            if (pWaveArt == (CWaveArt *) hDownload)
            {
                Trace(3,"Unloading wave articulation %ld\n",pWaveArt->m_dwID,m_dwSynthMemUse);
                m_WaveArtList[dwIndex].Remove(pWaveArt);

                pWaveArt->Release();
                LeaveCriticalSection(&m_CriticalSection);
                return S_OK;
            }
        }
    }*/
    LeaveCriticalSection(&m_CriticalSection);
    Trace(1,"Error: Unload failed - downloaded object not found.\n");
    return E_FAIL;
}

//////////////////////////////////////////////////////////
// Directx8 Methods 

/*CWave * CInstManager::GetWave(DWORD dwDLId)
{
    EnterCriticalSection(&m_CriticalSection);
    CWave *pWave = m_WavePool[dwDLId % WAVE_HASH_SIZE].GetHead();
    for (;pWave;pWave = pWave->GetNext())
    {
        if (dwDLId == pWave->m_dwID)
        {
            break;
        }
    }
    LeaveCriticalSection(&m_CriticalSection);

    return pWave;
}*/

/*CWaveArt *  CInstManager::GetWaveArt(DWORD dwDLId)
{
    EnterCriticalSection(&m_CriticalSection);
    CWaveArt *pWaveArt = m_WaveArtList[dwDLId % WAVEART_HASH_SIZE].GetHead();
    for (;pWaveArt;pWaveArt = pWaveArt->GetNext())
    {
        if (dwDLId == pWaveArt->m_dwID)
        {
            break;
        }
    }
    LeaveCriticalSection(&m_CriticalSection);

    return pWaveArt;
}

HRESULT CInstManager::DownloadWaveArticulation(LPHANDLE phDownload, 
                                   DMUS_DOWNLOADINFO *pInfo, 
                                   void *pvOffsetTable[], 
                                   void *pvData)
{
    DMUS_WAVEARTDL* pWaveArtDl  = (DMUS_WAVEARTDL*)pvData; 
    WAVEFORMATEX *pWaveformatEx = (WAVEFORMATEX *) pvOffsetTable[1]; 
    DWORD *dwDlId = (DWORD*)pvOffsetTable[2]; 
    DWORD i;

    CWaveArt* pWaveArt = new CWaveArt();
    if ( pWaveArt )
    {
        pWaveArt->m_dwID = pInfo->dwDLId;
        pWaveArt->m_WaveArtDl = *pWaveArtDl;;
        pWaveArt->m_WaveformatEx = *pWaveformatEx;
        if (pWaveArt->m_WaveformatEx.nChannels > 1)
        {
            pWaveArt->m_WaveformatEx.nAvgBytesPerSec /= pWaveArt->m_WaveformatEx.nChannels;
            pWaveArt->m_WaveformatEx.nBlockAlign /= pWaveArt->m_WaveformatEx.nChannels;
            pWaveArt->m_WaveformatEx.nChannels = 1;
        }
        if (pWaveformatEx->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            pWaveArt->m_bSampleType = SFORMAT_ADPCM;
        }
        else if (pWaveformatEx->wBitsPerSample == 8)
        {
            pWaveArt->m_bSampleType = SFORMAT_8;
        }
        else if (pWaveformatEx->wBitsPerSample == 16)
        {
            pWaveArt->m_bSampleType = SFORMAT_16;
        }
        else
        {
            Trace(1,"Error: Download failed because wave data is %ld bits instead of 8 or 16.\n",(long) pWaveformatEx->wBitsPerSample);
            delete pWaveArt;
            return DMUS_E_BADWAVE;
        }

        for ( i = 0; i < pWaveArtDl->ulBuffers; i++ )
        {
            // Get wave buffer and fill header with waveformat data
            CWave *pWave = GetWave(dwDlId[i]);
            assert(pWave);
            if (!pWave)
            {
                delete pWaveArt;
                return E_POINTER;
            }
            pWave->m_dwSampleRate = pWaveformatEx->nSamplesPerSec;
            // Reset the sample length so it can be set correctly in the ensuing sample size cases.
            pWave->m_dwSampleLength = pWave->m_dwSampleDataSize;

            if (pWaveformatEx->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
            {
                // Every 36 bytes is 64 samples. Buffer must be a multiple of 36 bytes or the
                // wave will be garbled on playback when we cross a buffer boundary.
                assert(pWave->m_dwSampleLength / 36 * 36 == pWave->m_dwSampleLength);

                pWave->m_dwSampleLength /= 36;
                pWave->m_dwSampleLength *= 64;  // This forces it to be block aligned.
                pWave->m_bSampleType = SFORMAT_ADPCM;
            }
            else if (pWaveformatEx->wBitsPerSample == 8)
            {
                pWave->m_bSampleType = SFORMAT_8;
            }
            else if (pWaveformatEx->wBitsPerSample == 16)
            {
                pWave->m_dwSampleLength >>= 1;
                pWave->m_bSampleType = SFORMAT_16;
            }
            else
            {
                Trace(1,"Error: Download failed because wave data is %ld bits instead of 8 or 16.\n",(long) pWaveformatEx->wBitsPerSample);
                delete pWaveArt;
                return DMUS_E_BADWAVE;
            }
#ifdef XBOX
    // We don't add a sample on the end for interpolation on Xbox. The hardware knows what to do.
#else
            pWave->m_dwSampleLength++;  // We always add one sample to the end for interpolation.

            // Default is to duplicate last sample. This will be overrwritten for
            // streaming waves.
            //
            if (pWave->m_dwSampleLength > 1)
            {
                if (pWave->m_bSampleType == SFORMAT_8)
                {
                    char* pb = (char*)pWave->m_pnWave;
                    pb[pWave->m_dwSampleLength - 1] = pb[pWave->m_dwSampleLength - 2];
                }
                else if (pWave->m_bSampleType == SFORMAT_16)
                {
                    short *pn = pWave->m_pnWave;
                    pn[pWave->m_dwSampleLength - 1] = pn[pWave->m_dwSampleLength - 2];
                }
            }
#endif

            // Create a WaveBuffer listitem and save the wave in and add it to the circular buffer list
            CWaveBuffer* pWavBuf = new CWaveBuffer();
            if ( pWavBuf == NULL )
            {
                delete pWaveArt;
                return E_OUTOFMEMORY;
            }
            pWavBuf->m_pWave = pWave;

            // This Articulation will be handling streaming data 
            if ( pWave->m_bStream )
                pWaveArt->m_bStream = TRUE;

            pWaveArt->m_pWaves.AddTail(pWavBuf);
        }

        EnterCriticalSection(&m_CriticalSection);
        if (pWaveArt)
        {
            CWaveBuffer* pCurrentBuffer = pWaveArt->m_pWaves.GetHead();
            for (; pCurrentBuffer; pCurrentBuffer = pCurrentBuffer->GetNext() )
            {
                if (pCurrentBuffer->m_pWave)
                {
                    pCurrentBuffer->m_pWave->AddRef();
                }
            }
        }
        m_WaveArtList[pWaveArt->m_dwID % WAVEART_HASH_SIZE].AddHead(pWaveArt);
        LeaveCriticalSection(&m_CriticalSection);

        *phDownload = (HANDLE) pWaveArt;

        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT CInstManager::DownloadWaveRaw(LPHANDLE phDownload, 
                                   DMUS_DOWNLOADINFO *pInfo, 
                                   void *pvOffsetTable[], 
                                   void *pvData)
{
    CWave *pWave = new CWave;
    if (pWave)
    {   
        DMUS_WAVEDATA *pdmWaveData= (DMUS_WAVEDATA *)pvData;
        Trace(3,"Downloading raw wave data%ld\n",pInfo->dwDLId);

        pWave->m_dwID = pInfo->dwDLId;
        pWave->m_hUserData = NULL;
        pWave->m_lpFreeHandle = NULL;
        pWave->m_dwSampleLength = pdmWaveData->cbSize;
        pWave->m_dwSampleDataSize = pdmWaveData->cbSize;
        pWave->m_pnWave = (short *) &pdmWaveData->byData[0];

        if ( pInfo->dwDLType == DMUS_DOWNLOADINFO_STREAMINGWAVE )
        {
            pWave->m_bStream = TRUE;
            pWave->m_bValid = TRUE;
        }

        EnterCriticalSection(&m_CriticalSection);
        m_WavePool[pWave->m_dwID % WAVE_HASH_SIZE].AddHead(pWave);
        LeaveCriticalSection(&m_CriticalSection);

        *phDownload = (HANDLE) pWave;
        pWave->AddRef();

        m_dwSynthMemUse += pWave->m_dwSampleDataSize; 

        return S_OK;
    }
    return E_OUTOFMEMORY;
}

*/


