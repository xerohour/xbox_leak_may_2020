//      Copyright (c) 1996-2001 Microsoft Corporation
//      dls.cpp
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

STDAPI DirectMusicCollectionCreate(REFIID iid, void ** ppDMC )
{
    HRESULT hr = E_OUTOFMEMORY;
    CCollection *pCollection = new CCollection;
    if (pCollection)
    {
        hr = pCollection->QueryInterface(iid,ppDMC);
        // If failed, release the object to free it. If succeeded, bring ref count down to 1.
        pCollection->Release(); 
    }
    return hr;
}

extern CInstManager *g_pInstManager;

CCollection::CCollection() : m_MemTrack(DMTRACK_COLLECTION)
{
	m_cRef = 1;
    if (g_pInstManager)
    {
        g_pInstManager->m_CollectionList.AddHead(this);
    }
//	INITIALIZE_CRITICAL_SECTION(&m_CriticalSection);
}

CCollection::~CCollection()
{
//	DELETE_CRITICAL_SECTION(&m_CriticalSection);
    if (g_pInstManager)
    {
        g_pInstManager->m_CollectionList.Remove(this);
    }
    DWORD dwIndex;
    for (dwIndex = 0; dwIndex < INSTRUMENT_HASH_SIZE; dwIndex++)
    {
        while (!m_InstrumentList[dwIndex].IsEmpty())
        {
            CInstrument *pInstrument = m_InstrumentList[dwIndex].RemoveHead();
            delete pInstrument;
        }
    }
    for (dwIndex = 0; dwIndex < WAVE_HASH_SIZE; dwIndex++)
    {
        while (!m_WavePool[dwIndex].IsEmpty()) 
        {
            CWave *pWave = m_WavePool[dwIndex].RemoveHead();
            pWave->Release();
        }
    }
}

STDMETHODIMP CCollection::QueryInterface(const IID &iid, void **ppv)
{
    V_INAME(IDirectMusicCollection::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);


	if(iid == IID_IUnknown || iid == IID_IDirectMusicCollection)
	{
		*ppv = static_cast<IDirectMusicCollection*>(this);
	} 
	else if(iid == IID_IDirectMusicObject)
	{
		*ppv = static_cast<IDirectMusicObject*>(this);
	}
	else if(iid == IID_IPersistStream)
	{
		*ppv = static_cast<IPersistStream*>(this);
	}
	else if(iid == IID_IPersist)
	{
		*ppv = static_cast<IPersist*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

    reinterpret_cast<IUnknown*>(this)->AddRef();
    
	return S_OK;
}


STDMETHODIMP_(ULONG) CCollection::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCollection::Release()
{
    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

/////////////////////////////////////////////////////////////////////////////
// IPersist

HRESULT CCollection::GetClassID( CLSID* pClassID )
{
	V_INAME(CCollection::GetClassID);
	V_PTR_WRITE(pClassID, CLSID); 
	*pClassID = CLSID_DirectMusicCollection;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream functions

HRESULT CCollection::IsDirty()
{
	return S_FALSE;
}

HRESULT CCollection::Load( IStream* pIStream )
{
	V_INAME(IPersistStream::Load);
	V_INTERFACE(pIStream);

    CRiffParser Parser(pIStream);
    RIFFIO ckMain;
    HRESULT hr = S_OK;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == FOURCC_DLS))
    {
        hr = Load(&Parser);
    }
    else
    {
        Trace(1,"Error: Unknown file format when parsing DLS Collection\n");
        hr = DMUS_E_NOTADLSCOL;
    }
    return hr;
}

void CCollection::AddInstrument(CInstrument *pInstrument)

{
    if (pInstrument != NULL)
    {
        CSourceRegion *pRegion = pInstrument->m_RegionList.GetHead();
        for (;pRegion;pRegion = pRegion->GetNext())
        {
            CWave *pWave = pRegion->m_Sample.m_pWave;
            if (pWave != NULL)
            {
                m_WavePool->AddTail(pWave);
				pWave->AddRef();
            }
        }
        m_InstrumentList[pInstrument->m_dwProgram % INSTRUMENT_HASH_SIZE].AddTail(pInstrument);
    }
}

extern DWORD TimeCents2Samples(long tcTime, DWORD dwSampleRate);
extern DWORD PitchCents2PitchFract(long pcRate,DWORD dwSampleRate);


HRESULT CCollection::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
    RIFFIO ckChild;
    HRESULT hr = S_OK;
    pParser->EnterList(&ckNext);
	while(pParser->NextChunk(&hr))
	{
		switch(ckNext.ckid)
		{
        case FOURCC_DLID:
        case DMUS_FOURCC_VERSION_CHUNK:
        case DMUS_FOURCC_CATEGORY_CHUNK:
        case DMUS_FOURCC_DATE_CHUNK:
            hr = m_Info.ReadChunk(pParser,ckNext.ckid);
			break;
		case FOURCC_LIST:
			switch(ckNext.fccType)
			{
                case DMUS_FOURCC_UNFO_LIST:
                    hr = m_Info.ReadChunk(pParser,ckNext.fccType);
					break;
				case FOURCC_WVPL:
                    hr = LoadWaves(pParser);
                    break;
				case FOURCC_LINS :
					hr = LoadInstruments(pParser);
					break;
			}
			break;
//		case FOURCC_COLH:
//			DLSHEADER dlsHeader;
//			hr = pParser->Read(&dlsHeader, sizeof(DLSHEADER));
//            m_dwNumInstruments = dlsHeader.cInstruments;
//			break;
//		case FOURCC_PTBL:
//			hr = BuildWaveOffsetTable(pParser);
//			break;
        }
	}
    pParser->LeaveList();
    ResolveConnections();
	return hr;
}


HRESULT CSourceArticulation::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 
    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		switch(ckNext.ckid)
		{
        case FOURCC_ART2 :
        case FOURCC_ART1 : 
            CONNECTIONLIST ConnectionList;
            hr = pParser->Read(&ConnectionList, sizeof(CONNECTIONLIST));
            for (DWORD dwIndex = 0; SUCCEEDED(hr) && (dwIndex < ConnectionList.cConnections); dwIndex++)
            {
                CONNECTION Connection;
                hr = pParser->Read(&Connection, sizeof(CONNECTION));
                switch (Connection.usSource)
                {
                case CONN_SRC_NONE :
                    switch (Connection.usDestination)
                    {
                    case CONN_DST_LFO_FREQUENCY :
                        m_LFO.m_Registers.dwDelta = PitchCents2PitchFract(
                            Connection.lScale,6000*64); // 48000*64 / 8)
                        break;
                    case CONN_DST_LFO_STARTDELAY :
                        m_LFO.m_Registers.dwDelay = (DWORD) TimeCents2Samples(
                            (TCENT) Connection.lScale,1500); // 48000 / 32
                        break;
                    case CONN_DST_EG1_ATTACKTIME :
                        m_VolumeEG.m_Registers.dwAttack = TimeCents2Samples(
                            (TCENT) Connection.lScale,94); // 48000 / 512
                        break;
                    case CONN_DST_EG1_DECAYTIME :
                        m_VolumeEG.m_Registers.dwDecay = TimeCents2Samples(
                        (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG1_SUSTAINLEVEL :
                        {
                            // The nvidia chip takes the sustain level in 
                            // absolute volume, not dB, so first convert.
                            long lTemp = (Connection.lScale >> 16) - 1000;
                            lTemp *= 9600;
                            lTemp /= 1000;
                            lTemp = CDigitalAudio::VRELToVFRACT(lTemp);
                            m_VolumeEG.m_Registers.dwSustain = lTemp >> 4;
                        }
                        break;
                    case CONN_DST_EG1_RELEASETIME :
                        m_VolumeEG.m_Registers.dwRelease = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG2_ATTACKTIME :
                        m_PitchEG.m_Registers.dwAttack = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG2_DECAYTIME :
                        m_PitchEG.m_Registers.dwDecay = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG2_SUSTAINLEVEL :
                        m_PitchEG.m_Registers.dwSustain =
                            (SPERCENT) ((long) (Connection.lScale / 257003));
                        break;
                    case CONN_DST_EG2_RELEASETIME :
                        m_PitchEG.m_Registers.dwRelease = TimeCents2Samples(
                            (TCENT) Connection.lScale,94); 
                        break;
                    case CONN_DST_PAN :
                        m_sDefaultPan = (short) 
                            ((long) ((long) Connection.lScale >> 12) / 125);
                        break;

                    /* DLS2 */
                    case CONN_DST_EG1_DELAYTIME:
                        m_VolumeEG.m_Registers.dwDelay = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG1_HOLDTIME:
                        m_VolumeEG.m_Registers.dwHold  = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG2_DELAYTIME:
                        m_PitchEG.m_Registers.dwDelay  = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_EG2_HOLDTIME:
                        m_PitchEG.m_Registers.dwHold = TimeCents2Samples(
                            (TCENT) Connection.lScale,94);
                        break;
                    case CONN_DST_VIB_FREQUENCY :
                        m_LFO2.m_Registers.dwDelta = PitchCents2PitchFract(
                            Connection.lScale,6000*64);
                        break;    
                    case CONN_DST_VIB_STARTDELAY :
                        m_LFO2.m_Registers.dwDelay = TimeCents2Samples(
                            (TCENT) Connection.lScale,1500);
                        break;                                
                    case CONN_DST_FILTER_CUTOFF:
                        // First, get the filter cutoff frequency, which is relative to a440.
                        m_Filter.m_prCutoff = (PRELS)
                            (Connection.lScale >> 16);
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
                            (Connection.lScale >> 16); //>>>>>>>> not really VRELS, but 1/10th's
     //                   m_Filter.m_iQIndex = (DWORD)
     //                       ((m_Filter.m_vrQ / 15.0f) + 0.5f);
                        break;
                    }
                    break;
                case CONN_SRC_LFO :
                    switch (Connection.usControl)
                    {
                    case CONN_SRC_NONE :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_ATTENUATION :
                            m_LFO.m_vrVolumeScale = (VRELS)
                                ((long) (Connection.lScale >> 16));
                            m_LFO.m_Registers.lAmplitudeModulation = Connection.lScale / 38400;
                            break;
                        case CONN_DST_PITCH :
                            m_LFO.m_prPitchScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            m_LFO.m_Registers.lPitchModulation = Connection.lScale / 614400;
                            break;
                        case CONN_DST_FILTER_CUTOFF:
                            m_LFO.m_prCutoffScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            m_LFO.m_Registers.lFilterCutOffRange = Connection.lScale / 4915200; 
                            break;
                        }
                        break;
                    case CONN_SRC_CC1 :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_ATTENUATION :
                            m_LFO.m_vrMWVolumeScale = (VRELS)
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        case CONN_DST_PITCH :
                            m_LFO.m_prMWPitchScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            break;

                        /* DLS2 */
                        case CONN_DST_FILTER_CUTOFF:
                            m_LFO.m_prMWCutoffScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        }
                        break;

                    /* DLS2 */
                    case CONN_SRC_CHANNELPRESSURE :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_ATTENUATION :
                            m_LFO.m_vrCPVolumeScale = (VRELS) 
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        case CONN_DST_PITCH :
                            m_LFO.m_prCPPitchScale  = (PRELS) 
                                ((long) (Connection.lScale >> 16)); 
                            break;

                        /* DLS2 */
                        case CONN_DST_FILTER_CUTOFF:
                            m_LFO.m_prCPCutoffScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        }
                        break;
                    }
                    break;
                case CONN_SRC_KEYONVELOCITY :
                    switch (Connection.usDestination)
                    {
                    case CONN_DST_EG1_ATTACKTIME :
                        m_VolumeEG.m_trVelAttackScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                        break;
                    case CONN_DST_EG2_ATTACKTIME :
                        m_PitchEG.m_trVelAttackScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                        break;

                    /* DLS2 */
                    case CONN_DST_FILTER_CUTOFF:
                        m_Filter.m_prVelScale = (PRELS)
                            ((long) (Connection.lScale >> 16)); 
                        break;
                    }
                    break;
                case CONN_SRC_KEYNUMBER :
                    switch (Connection.usDestination)
                    {
                    case CONN_DST_EG1_DECAYTIME :
                        m_VolumeEG.m_trKeyDecayScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                        break;
                    case CONN_DST_EG2_DECAYTIME :
                        m_PitchEG.m_trKeyDecayScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                        break;

                    /* DLS2 */
                    case CONN_DST_EG1_HOLDTIME :
                        m_PitchEG.m_trKeyDecayScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                        break;
                    case CONN_DST_EG2_HOLDTIME :
                        m_PitchEG.m_trKeyDecayScale = (TRELS)
                            ((long) (Connection.lScale >> 16));
                    case CONN_DST_FILTER_CUTOFF :
                        m_Filter.m_prKeyScale = (PRELS)
                            ((long) (Connection.lScale >> 16));
                        break;
                    }
                    break;
                case CONN_SRC_EG2 :
                    switch (Connection.usDestination)
                    {
                    case CONN_DST_PITCH :
                        m_PitchEG.m_Registers.lPitchScale = Connection.lScale / 614400;
                        break;

                    /* DLS2 */
                    case CONN_DST_FILTER_CUTOFF:
                        m_PitchEG.m_Registers.lFilterCutOff = Connection.lScale / 4915200; 
                        break;
                    }
                    break;      

                /* DLS2 */
                case CONN_SRC_VIBRATO :
                    switch (Connection.usControl)
                    {
                    case CONN_SRC_NONE :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_PITCH :
                            m_LFO2.m_prPitchScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            m_LFO2.m_Registers.lPitchModulation = Connection.lScale / 614400;
                            break;
                        }
                        break;
                    case CONN_SRC_CC1 :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_PITCH :
                            m_LFO2.m_prMWPitchScale = (PRELS)
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        }
                        break;
                    case CONN_SRC_CHANNELPRESSURE :
                        switch (Connection.usDestination)
                        {
                        case CONN_DST_PITCH :
                            m_LFO2.m_prCPPitchScale  = (PRELS) 
                                ((long) (Connection.lScale >> 16)); 
                            break;
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }
    pParser->LeaveList();
    return hr;
}

HRESULT CSourceRegion::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 

    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		switch(ckNext.ckid)
		{
        case FOURCC_RGNH :
            RGNHEADER RegionHeader;
            hr = pParser->Read(&RegionHeader, sizeof(RGNHEADER));
            m_bKeyHigh = (BYTE) RegionHeader.RangeKey.usHigh;
            m_bKeyLow  = (BYTE) RegionHeader.RangeKey.usLow;
            m_bVelocityHigh = (BYTE) RegionHeader.RangeVelocity.usHigh;
            m_bVelocityLow  = (BYTE) RegionHeader.RangeVelocity.usLow;
            // Old dls collections can have bad velocities...
            if ( m_bVelocityHigh == 0 && m_bVelocityLow == 0 )
                m_bVelocityHigh = 127;
            if (RegionHeader.fusOptions & F_RGN_OPTION_SELFNONEXCLUSIVE)
            {
                m_bAllowOverlap = TRUE;
            }
            else
            {
                m_bAllowOverlap = FALSE;
            }
            m_bGroup = (BYTE) RegionHeader.usKeyGroup;
            break;
        case FOURCC_WSMP :
            WSMPL WaveSample;
            WLOOP WaveLoop;
			hr = pParser->Read(&WaveSample, sizeof(WSMPL));
            m_vrAttenuation = (VRELS) (long) (((WaveSample.lAttenuation) * 10) >> 16);
            m_Sample.m_prFineTune = WaveSample.sFineTune;
            m_Sample.m_bMIDIRootKey = (BYTE) WaveSample.usUnityNote;
			if(WaveSample.cSampleLoops)
			{
				hr = pParser->Read(&WaveLoop, sizeof(WLOOP));
                m_Sample.m_dwLoopStart = WaveLoop.ulStart;
                m_Sample.m_dwLoopEnd = WaveLoop.ulStart + WaveLoop.ulLength;
                m_Sample.m_bOneShot = FALSE;
                m_Sample.m_dwLoopType = WaveLoop.ulType;
			}
            else
            {
                m_Sample.m_bOneShot = TRUE;
            }
            break;
        case FOURCC_WLNK :
            WAVELINK WaveLink;
			hr = pParser->Read(&WaveLink, sizeof(WAVELINK));
            m_sWaveLinkOptions = WaveLink.fusOptions;
            m_dwChannel = WaveLink.ulChannel;
            m_Sample.m_wID = (WORD) WaveLink.ulTableIndex;
            break;
//    m_Sample.m_dwSampleRate = dwSampleRate;
        case FOURCC_LIST:
			switch(ckNext.fccType)  
            {
                case FOURCC_LART :
                case FOURCC_LAR2 :
                m_pArticulation = new CSourceArticulation;
                if (!m_pArticulation)
                {
                    return E_OUTOFMEMORY;
                }
                m_pArticulation->Init(FALSE);
                m_pArticulation->AddRef(); // Will Release when done.
                hr = m_pArticulation->Load(pParser);
                break;
            }
            break;
        }
    }
    pParser->LeaveList();
    return hr;
}


HRESULT CInstrument::LoadRegions(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 
    CSourceArticulation *pArticulation = NULL;
    BOOL fIsDrum = FALSE;

    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		if ((ckNext.ckid == FOURCC_LIST) && 
            ((ckNext.fccType == FOURCC_RGN) || (ckNext.fccType == FOURCC_RGN2)))
        {
            CSourceRegion *pRegion = NULL;

            pRegion = new CSourceRegion;
            if (!pRegion)
            {
                return E_OUTOFMEMORY;
            }
            hr = pRegion->Load(pParser);
            // If it failed, it's still okay to insert in list, will be cleared later.
            m_RegionList.AddHead(pRegion);
        }
	}
    pParser->LeaveList();

	return hr;
}

HRESULT CInstrument::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 
    CSourceArticulation *pArticulation = NULL;
    BOOL fIsDrum = FALSE;

    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		switch(ckNext.ckid)
		{
        case FOURCC_INSH :
			INSTHEADER instHeader;
			hr = pParser->Read(&instHeader, sizeof(INSTHEADER));
			m_dwProgram = instHeader.Locale.ulInstrument;
			m_dwProgram |= (instHeader.Locale.ulBank) << 8;
			m_dwProgram |= (instHeader.Locale.ulBank & 0x80000000);
            break;
		case FOURCC_LIST:
			switch(ckNext.fccType)
			{
            case FOURCC_LRGN :
                hr = LoadRegions(pParser);
                break;
            case FOURCC_LART :
            case FOURCC_LAR2 :
                if (pArticulation)
                {
                    pArticulation->Release();
                }
                
                pArticulation = new CSourceArticulation;

                if (!pArticulation)
                {
                    return E_OUTOFMEMORY;
                }
                pArticulation->Init(FALSE);
                pArticulation->AddRef(); // Will Release when done.
                hr = pArticulation->Load(pParser);
                break;
            }
        }
    }

    pParser->LeaveList();
    if (pArticulation)
    {
        for (CSourceRegion *pr = m_RegionList.GetHead();
             pr != NULL;
             pr = pr->GetNext())
        {
            if (pr->m_pArticulation == NULL)
            {
				pr->m_pArticulation = pArticulation;
				pArticulation->AddRef();    
			}
        }
        pArticulation->Release();   // Release initial AddRef();
    }
    else
    {
        for (CSourceRegion *pr = m_RegionList.GetHead();
             pr != NULL;
             pr = pr->GetNext())
        {
            if (pr->m_pArticulation == NULL)
            {
                return DMUS_E_BADARTICULATION;
            }
        }
    }
	return hr;
}

void CCollection::ResolveConnections()

{
    DWORD dwIndex;
    for (dwIndex = 0; dwIndex < INSTRUMENT_HASH_SIZE; dwIndex++)
    {
		for (CInstrument *pInstrument = m_InstrumentList[dwIndex].GetHead();
			 pInstrument != NULL;
			 pInstrument = pInstrument->GetNext())
		{
			CSourceRegion *pRegion = pInstrument->m_RegionList.GetHead();
			for (;pRegion != NULL;pRegion = pRegion->GetNext())
			{
                CWave *pWave = m_WavePool[pRegion->m_Sample.m_wID % WAVE_HASH_SIZE].GetHead();
                for (;pWave;pWave = pWave->GetNext())
                {
				    if (pRegion->m_Sample.m_wID == pWave->m_dwID)
				    {
					    pRegion->m_Sample.m_pWave = pWave;
					    pWave->AddRef(); 
                        break;
				    }
                }
            }
        }
    }
}

#define FOURCC_WAVE mmioFOURCC('W','A','V','E')
#define FOURCC_wave mmioFOURCC('w','a','v','e')

HRESULT CCollection::LoadWaves(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 
    DWORD dwWaveID = 0;

    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		if ((ckNext.ckid == FOURCC_LIST) && 
            ((ckNext.fccType == FOURCC_WAVE) || (ckNext.fccType == FOURCC_wave)))
        {
            CWave *pWave = new CWave;
            if (!pWave)
            {
                return E_OUTOFMEMORY;
            }
            hr = pWave->Load(pParser);
            pWave->AddRef();
            pWave->m_dwID = dwWaveID++;
            // Failure will cause everything to be deleted, so insert in list in interest of efficiency.
            m_WavePool[pWave->m_dwID % WAVE_HASH_SIZE].AddHead(pWave);
        }
    }
    pParser->LeaveList();
	return hr;
}

HRESULT CWave::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 
    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		switch(ckNext.ckid)
		{
        case mmioFOURCC('f','m','t',' ') :
            hr = pParser->Read(&m_WaveFormat, sizeof(XBOXADPCMWAVEFORMAT));
            if ((m_WaveFormat.wfx.wFormatTag != WAVE_FORMAT_PCM)
                && (m_WaveFormat.wfx.wFormatTag != WAVE_FORMAT_XBOX_ADPCM)) 
            {
                return E_FAIL;
            } 
            if (m_WaveFormat.wfx.wBitsPerSample == 8)
            {
                m_bSampleType = SFORMAT_8;
            }
            else if (m_WaveFormat.wfx.wBitsPerSample == 16)
            {
                m_bSampleType = SFORMAT_16;
            }
            m_dwSampleRate = m_WaveFormat.wfx.nSamplesPerSec;
            break;
        case FOURCC_WSMP :
            WSMPL WaveSample;
            WLOOP WaveLoop;
			hr = pParser->Read(&WaveSample, sizeof(WSMPL));
/*            m_vrAttenuation = (VRELS) (long) (((WaveSample.lAttenuation) * 10) >> 16);
            m_Sample.m_prFineTune = WaveSample.sFineTune;
            m_Sample.m_bMIDIRootKey = (BYTE) WaveSample.usUnityNote;*/
			if(WaveSample.cSampleLoops)
			{
				hr = pParser->Read(&WaveLoop, sizeof(WLOOP));
/*                m_Sample.m_dwLoopStart = WaveLoop.ulStart;
                m_Sample.m_dwLoopEnd = WaveLoop.ulStart + WaveLoop.ulLength;
                m_Sample.m_bOneShot = FALSE;
                m_Sample.m_dwLoopType = WaveLoop.ulType;*/
			}
            else
            {
//                m_Sample.m_bOneShot = TRUE;
            }
            break;
        case mmioFOURCC('d','a','t','a') :
            m_dwSampleDataSize = ckNext.cksize;
            m_pnWave = (short *) new char[m_dwSampleDataSize];
            if (!m_pnWave)
            {
                return E_OUTOFMEMORY;
            }
            hr = pParser->Read(m_pnWave, m_dwSampleDataSize);
            break;
        }
    }
    pParser->LeaveList();

	return hr;
}

HRESULT CCollection::LoadInstruments(CRiffParser *pParser)
{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 

    pParser->EnterList(&ckNext);

    while (pParser->NextChunk(&hr))
    {
		if ((ckNext.ckid == FOURCC_LIST) && 
            (ckNext.fccType == FOURCC_INS))
        {
            CInstrument *pInstrument = new CInstrument;
            if (!pInstrument)
            {
                return E_OUTOFMEMORY;
            }
            hr = pInstrument->Load(pParser);
            // Failure will cause everything to be deleted, so insert in list in interest of efficiency.
            m_InstrumentList[pInstrument->m_dwProgram % INSTRUMENT_HASH_SIZE].AddTail(pInstrument);
        }
    }
    pParser->LeaveList();
	return hr;
}

HRESULT CCollection::Save( IStream* pIStream, BOOL fClearDirty )
{
	return E_NOTIMPL;
}

HRESULT CCollection::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDirectMusicObject

STDMETHODIMP CCollection::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CCollection::GetDescriptor);
	V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.GetDescriptor(pDesc,CLSID_DirectMusicCollection);
}

STDMETHODIMP CCollection::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CCollection::SetDescriptor);
	V_STRUCTPTR_READ(pDesc, DMUS_OBJECTDESC);
	return m_Info.SetDescriptor(pDesc);
}

STDMETHODIMP CCollection::ParseDescriptor(LPSTREAM pIStream, LPDMUS_OBJECTDESC pDesc) 
{
    V_INAME(CCollection::ParseDescriptor);
    V_INTERFACE(pIStream);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.ParseDescriptor(pIStream,pDesc,FOURCC_DLS,CLSID_DirectMusicCollection);
}

STDMETHODIMP CCollection::GetInstrument(DWORD dwPatch, IDirectMusicInstrument** pInstrument)

{
    return E_NOTIMPL;
}

STDMETHODIMP CCollection::EnumInstrument(DWORD dwIndex, DWORD* pdwPatch, LPWSTR pName, DWORD cwchName)

{
    return E_NOTIMPL;
}
