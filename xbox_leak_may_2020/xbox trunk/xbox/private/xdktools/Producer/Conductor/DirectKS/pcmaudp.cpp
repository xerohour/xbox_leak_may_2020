//==============================================================================
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//
// Module Name:
//	pcmaudp.cpp
//
// Abstract:
//  Class implementation of CPCMAudioPin.
//
//==============================================================================

#include "stdafx.h"
#include "kslibp.h"

CPCMAudioPin::CPCMAudioPin
(
    CKsFilter*  pFilter,
    ULONG       nId
) : CKsPin(pFilter, nId),
    m_dwWaveFormatMask(0),
	m_dwMinSampleFreq(ULONG_MAX),
	m_dwMaxSampleFreq(0)
{
    // create a KSPIN_CONNECT structure to describe a waveformatex pin
    m_pksPinCreate = 
        (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX));
    if (!m_pksPinCreate)
        return;

    ZeroMemory(&m_nPosition, sizeof(KSAUDIO_POSITION));

    m_pksPinCreate->Interface.Set              = KSINTERFACESETID_Standard;
    m_pksPinCreate->Interface.Id               = KSINTERFACE_STANDARD_STREAMING;
    m_pksPinCreate->Interface.Flags            = 0;
    m_pksPinCreate->Medium.Set                 = KSMEDIUMSETID_Standard;
    m_pksPinCreate->Medium.Id                  = KSMEDIUM_TYPE_ANYINSTANCE;
    m_pksPinCreate->Medium.Flags               = 0;
    m_pksPinCreate->PinId                      = nId;
    m_pksPinCreate->PinToHandle                = NULL;
    m_pksPinCreate->Priority.PriorityClass     = KSPRIORITY_NORMAL;
    m_pksPinCreate->Priority.PrioritySubClass  = 1;

    // point m_pksDataFormat to just after the pConnect struct
    PKSDATAFORMAT_WAVEFORMATEX pksDataFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)(m_pksPinCreate + 1);

    // set up format for KSDATAFORMAT_WAVEFORMATEX
    pksDataFormatWfx->DataFormat.FormatSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
    pksDataFormatWfx->DataFormat.Flags = 0;
    pksDataFormatWfx->DataFormat.Reserved = 0;
    pksDataFormatWfx->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pksDataFormatWfx->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pksDataFormatWfx->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;

    m_pksDataFormat = (PKSDATAFORMAT)pksDataFormatWfx;
}

// ----------------------------------------------------------------------------------
// Copy Constructor
// ----------------------------------------------------------------------------------
CPCMAudioPin::CPCMAudioPin
(
    CKsPin*  pkspCopy
) : CKsPin(pkspCopy->m_pFilter, pkspCopy),
    m_dwWaveFormatMask(0),
	m_dwMinSampleFreq(ULONG_MAX),
	m_dwMaxSampleFreq(0)
{
    // create a KSPIN_CONNECT structure to describe a waveformatex pin
    SafeLocalFree(m_pksPinCreate);

    m_pksPinCreate = 
        (PKSPIN_CONNECT)LocalAlloc(LPTR, sizeof(KSPIN_CONNECT) + sizeof(KSDATAFORMAT_WAVEFORMATEX));
    if (!m_pksPinCreate)
        return;

    ZeroMemory(&m_nPosition, sizeof(KSAUDIO_POSITION));

    m_pksPinCreate->Interface.Set              = KSINTERFACESETID_Standard;
    m_pksPinCreate->Interface.Id               = KSINTERFACE_STANDARD_STREAMING;
    m_pksPinCreate->Interface.Flags            = 0;
    m_pksPinCreate->Medium.Set                 = KSMEDIUMSETID_Standard;
    m_pksPinCreate->Medium.Id                  = KSMEDIUM_TYPE_ANYINSTANCE;
    m_pksPinCreate->Medium.Flags               = 0;
    m_pksPinCreate->PinId                      = m_nId;
    m_pksPinCreate->PinToHandle                = NULL;
    m_pksPinCreate->Priority.PriorityClass     = KSPRIORITY_NORMAL;
    m_pksPinCreate->Priority.PrioritySubClass  = 1;

    // point m_pksDataFormat to just after the pConnect struct
    PKSDATAFORMAT_WAVEFORMATEX pksDataFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)(m_pksPinCreate + 1);

    // set up format for KSDATAFORMAT_WAVEFORMATEX
    pksDataFormatWfx->DataFormat.FormatSize = sizeof(KSDATAFORMAT_WAVEFORMATEX);
    pksDataFormatWfx->DataFormat.Flags = 0;
    pksDataFormatWfx->DataFormat.Reserved = 0;
    pksDataFormatWfx->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pksDataFormatWfx->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pksDataFormatWfx->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;

    m_pksDataFormat = (PKSDATAFORMAT)pksDataFormatWfx;

//    m_pksPinCreate      = pkspCopy->m_pksPinCreate;     // creation parameters of pin
//    m_pksDataFormat     = pkspCopy->m_pksDataFormat;    // description of data
    // must NULL any pointers in pkspCopy that we inherited so that we don't free them on delete
    pkspCopy->m_pbStreamData = NULL;

    if (pkspCopy->m_eType == ePCMAudio)
	{
        m_dwWaveFormatMask = ((CPCMAudioPin*)pkspCopy)->m_dwWaveFormatMask;
		m_dwMinSampleFreq = ((CPCMAudioPin*)pkspCopy)->m_dwMinSampleFreq;
		m_dwMaxSampleFreq = ((CPCMAudioPin*)pkspCopy)->m_dwMaxSampleFreq;
	}

    // BUGBUG:  ?? is this right??
    delete pkspCopy;
}

// ----------------------------------------------------------------------------------
// CPCMAudioPin::SetFormat
//  sets the format
// ----------------------------------------------------------------------------------
BOOL    
CPCMAudioPin::SetFormat
(
    WAVEFORMATEX* pwfx
)
{
    if (!(pwfx && m_pksDataFormat))
        return FALSE;

    PKSDATAFORMAT_WAVEFORMATEX pksDataFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)m_pksDataFormat;

    CopyMemory(&pksDataFormatWfx->WaveFormatEx, pwfx, sizeof(WAVEFORMATEX));
    pksDataFormatWfx->DataFormat.SampleSize = (USHORT)(pwfx->nChannels * pwfx->wBitsPerSample / 8);

    return TRUE;
}


// ----------------------------------------------------------------------------------
// CPCMAudioPin::CreateDataBuffer
//  Allocates a buffer of appropriately sized to hold dwBufferSeconds worth o' data
// ----------------------------------------------------------------------------------
BOOL    
CPCMAudioPin::CreateDataBuffer
(
    DWORD   dwBufferSeconds,
    DWORD   dwAlignment /* = 0 */
)
{
    BOOL    fRes;
    PKSDATAFORMAT_WAVEFORMATEX pksDataFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)m_pksDataFormat;

    fRes = (pksDataFormatWfx != NULL);

    m_dwAlignment = dwAlignment;

    if (fRes)
    {
        m_cbStreamData = 
              dwBufferSeconds
            * pksDataFormatWfx->WaveFormatEx.nChannels 
            * pksDataFormatWfx->WaveFormatEx.nSamplesPerSec
            * pksDataFormatWfx->WaveFormatEx.wBitsPerSample 
            / 8;

        ASSERT(!m_pbStreamData);
        
        m_pbStreamData = (PBYTE)LocalAlloc(LPTR, m_cbStreamData + m_dwAlignment);

        fRes = (m_pbStreamData != NULL);
        if (!fRes)
            LOG(eWarn1, "Error!  Out of memory!");

        m_pbStreamData += m_dwAlignment;
    }

    return fRes;
}

BOOL    
CPCMAudioPin::CreateDataBufferMS
(
    DWORD   dwBufferMiliSeconds,
    DWORD   dwAlignment /* = 0 */
)
{
    BOOL    fRes;
    PKSDATAFORMAT_WAVEFORMATEX pksDataFormatWfx = (PKSDATAFORMAT_WAVEFORMATEX)m_pksDataFormat;

    fRes = (pksDataFormatWfx != NULL);

    m_dwAlignment = dwAlignment;

    if (fRes)
    {
        m_cbStreamData = 
              dwBufferMiliSeconds
            * pksDataFormatWfx->WaveFormatEx.nChannels 
            * pksDataFormatWfx->WaveFormatEx.nSamplesPerSec
            * pksDataFormatWfx->WaveFormatEx.wBitsPerSample 
            / 8
            / 1000;

        ASSERT(!m_pbStreamData);
        
        m_pbStreamData = (PBYTE)LocalAlloc(LPTR, m_cbStreamData + m_dwAlignment);
        
        fRes = (m_pbStreamData != NULL);
        if (!fRes)
            LOG(eWarn1, "Error!  Out of memory!");

        m_pbStreamData += m_dwAlignment;
    }

    return fRes;
}

BOOL
CPCMAudioPin::GetPosition
(
    KSAUDIO_POSITION* pPos
)
{
    BOOL                fRes = (pPos != NULL);

    if (fRes)    
        fRes = 
            GetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, pPos, sizeof(KSAUDIO_POSITION));

    if (fRes)
        CopyMemory(&m_nPosition, pPos, sizeof(KSAUDIO_POSITION));

    return fRes;
}

BOOL
CPCMAudioPin::SetPosition
(
    KSAUDIO_POSITION* pPos
)
{
    BOOL                fRes = (pPos != NULL);

    if (fRes)    
        fRes = 
            SetPropertySimple(KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION, pPos, sizeof(KSAUDIO_POSITION));

    if (fRes)
        CopyMemory(&m_nPosition, pPos, sizeof(KSAUDIO_POSITION));

    return fRes;
}

