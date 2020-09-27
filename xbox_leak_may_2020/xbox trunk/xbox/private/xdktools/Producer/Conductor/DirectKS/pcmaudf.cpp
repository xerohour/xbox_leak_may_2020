//==============================================================================
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//
// Module Name:
//	pcmaudf.cpp
//
// Abstract:
//  Class implementation of CPCMAudioFilter.
//
//==============================================================================

#include "stdafx.h"
#include "kslibp.h"

// ----------------------------------------------------------------------------------
// CPCMAudioFilter::CPCMAudioFilter
// ----------------------------------------------------------------------------------
CPCMAudioFilter::CPCMAudioFilter
(
    LPCTSTR  pszName,
    LPCTSTR  pszFriendlyName,
    LPCTSTR  pszCLSID,
    LPCTSTR  pszService,
    LPCTSTR  pszBinary
) : CKsFilter(pszName, pszFriendlyName, pszCLSID, pszService, pszBinary)
{
    m_eType = ePCMAudio;
}

#define MASK_MIN_MAX_DATARANGE(dw, dwMask, val, min, max)   if (((min) <= (val)) && ((max) >= (val))) (dw) |= (dwMask)
// ----------------------------------------------------------------------------------
// CPCMAudioFilter::EnumeratePins
//  same as CKsFilter::EnumeratePins, but excludes non PCM pins
// ----------------------------------------------------------------------------------
BOOL    
CPCMAudioFilter::EnumeratePins
(
    void
)
{
    DWORD   fRes            = TRUE;
    DWORD    fViableFilter   = FALSE;
    ULONG    cPins, nPinId;

    // get the number of pins supported by SAD
    fRes = 
        GetPinPropertySimple
        (  
            0,
            KSPROPSETID_Pin,
            KSPROPERTY_PIN_CTYPES,
            &cPins,
            sizeof(cPins)
        );

    if (fRes)
    {
        fRes = FALSE;

        //
        // loop through the pins, looking for audio pins
        //
        for(nPinId = 0; nPinId < cPins; nPinId++)
        {
            ULONG               i;
            BOOL                fViablePin;;
            PKSMULTIPLE_ITEM    pMIInterfaces;
            PKSMULTIPLE_ITEM    pMIMediums;
            PKSMULTIPLE_ITEM    pMIDataRanges;
            PKSDATARANGE        pDataRange;

            //
            // create a new CKsPin and add to the appropriate pin list
            //
            CPCMAudioPin* pNewPin = new CPCMAudioPin(this, nPinId);

            //
            // get COMMUNICATION ---------------
            //
            fRes = 
                GetPinPropertySimple
                ( 
                    nPinId,
                    KSPROPSETID_Pin,
                    KSPROPERTY_PIN_COMMUNICATION,
                    &pNewPin->m_Descriptor.Communication,
                    sizeof(KSPIN_COMMUNICATION)
                );
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_COMMUNICATION");
                goto break_loop;
            }

            // ILL communication
            if ( (pNewPin->m_Descriptor.Communication != KSPIN_COMMUNICATION_SOURCE) &&
                (pNewPin->m_Descriptor.Communication != KSPIN_COMMUNICATION_SINK) &&
                (pNewPin->m_Descriptor.Communication != KSPIN_COMMUNICATION_BOTH) )
                goto break_loop;

            //
            // get PKSPIN_INTERFACEs ---------------
            //
            fRes = GetPinPropertyMulti(nPinId, KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, &pMIInterfaces);
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_INTERFACES");
                goto break_loop;
            }

            pNewPin->m_Descriptor.cInterfaces = pMIInterfaces->Count;
            pNewPin->m_Descriptor.pInterfaces = (PKSPIN_INTERFACE)(pMIInterfaces + 1);

            // make sure that at least one interface is standard streaming
            for(i = 0, fViablePin = FALSE; i < pNewPin->m_Descriptor.cInterfaces && !fViablePin; i++)
            {
                fViablePin = 
                    fViablePin ||
                    IsEqualGUIDAligned(pNewPin->m_Descriptor.pInterfaces[i].Set, KSINTERFACESETID_Standard) && 
                    (pNewPin->m_Descriptor.pInterfaces[i].Id == KSINTERFACE_STANDARD_STREAMING) ;
            }

            LocalFree(pMIInterfaces);

            if (!fViablePin)
                goto break_loop;

            //
            // get PKSPIN_MEDIUMs ---------------
            //
            fRes = GetPinPropertyMulti(nPinId, KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, &pMIMediums);
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_MEDIUMS");
                goto break_loop;
            }

            pNewPin->m_Descriptor.cMediums = pMIMediums->Count;
            pNewPin->m_Descriptor.pMediums = (PKSPIN_MEDIUM)(pMIMediums + 1);

            // make sure that at least one Medium is standard streaming
            for(i = 0, fViablePin = FALSE; i < pNewPin->m_Descriptor.cMediums && !fViablePin; i++)
            {
                fViablePin = 
                    fViablePin ||
                    IsEqualGUIDAligned(pNewPin->m_Descriptor.pMediums[i].Set, KSMEDIUMSETID_Standard) && 
                    (pNewPin->m_Descriptor.pMediums[i].Id == KSMEDIUM_STANDARD_DEVIO);
            }

            LocalFree(pMIMediums);

            if (!fViablePin)
                goto break_loop;

            //
            // get PKSPIN_DATARANGEs ---------------
            //
            fRes = GetPinPropertyMulti(nPinId, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &pMIDataRanges);
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_DATARANGES");
                goto break_loop;
            }

            pNewPin->m_Descriptor.cDataRanges = pMIDataRanges->Count;
            pNewPin->m_Descriptor.pDataRanges = (PKSDATARANGE)(pMIDataRanges + 1);
            pNewPin->m_dwWaveFormatMask = 0x00000000;
			pNewPin->m_dwMinSampleFreq = ULONG_MAX;
			pNewPin->m_dwMaxSampleFreq = 0;

            pDataRange = pNewPin->m_Descriptor.pDataRanges;

            // make sure that at least one data range supports audio
            for(i = 0, fViablePin = FALSE; i < pNewPin->m_Descriptor.cDataRanges; i++)
            {
                // SubType should either be compatible with WAVEFORMATEX or 
                // it should be WILDCARD
                fViablePin = 
                    fViablePin || 
                    IS_VALID_WAVEFORMATEX_GUID(&pDataRange->SubFormat) ||
                    IsEqualGUIDAligned(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_PCM) ||
                    IsEqualGUIDAligned(pDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_WILDCARD);

                if (fViablePin && IsEqualGUIDAligned(pDataRange->MajorFormat, KSDATAFORMAT_TYPE_AUDIO))
                {
                    PKSDATARANGE_AUDIO pDataRangeAudio = (PKSDATARANGE_AUDIO)pDataRange;

                    ULONG maxc = pDataRangeAudio->MaximumChannels;
                    ULONG minf = pDataRangeAudio->MinimumSampleFrequency;
                    ULONG maxf = pDataRangeAudio->MaximumSampleFrequency;
                    ULONG minb = pDataRangeAudio->MinimumBitsPerSample;
                    ULONG maxb = pDataRangeAudio->MaximumBitsPerSample;

					pNewPin->m_dwMinSampleFreq = min( pNewPin->m_dwMinSampleFreq, minf );
					pNewPin->m_dwMaxSampleFreq = max( pNewPin->m_dwMaxSampleFreq, maxf );

                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_MONO,       1, 1, maxc);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_STEREO,     2, 1, maxc);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_QUAD,       4, 1, maxc);    // BUGBUG:  these two are 
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_SURROUND,   4, 1, maxc);    // ambiguous (actually a design bug in ks)
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_5_1,        6, 1, maxc);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_7_1,        8, 1, maxc);

                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_8BIT,   8, minb, maxb);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_16BIT, 16, minb, maxb);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_20BIT, 20, minb, maxb);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_24BIT, 24, minb, maxb);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_32BIT, 32, minb, maxb);

                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_8000HZ,   8000, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_11025HZ, 11025, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_16000HZ, 16000, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_22050HZ, 22050, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_32000HZ, 32000, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_44100HZ, 44100, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_48000HZ, 48000, minf, maxf);
                    MASK_MIN_MAX_DATARANGE(pNewPin->m_dwWaveFormatMask, FMT_96000HZ, 96000, minf, maxf);
                }

                pDataRange = (PKSDATARANGE)( ((PBYTE)pDataRange) + pDataRange->FormatSize);
            }

            LocalFree(pMIDataRanges);

            if (!fViablePin)
                goto break_loop;

            //
            // Get the data flow property
            //
            fRes = 
                GetPinPropertySimple
                (             
                    nPinId,
                    KSPROPSETID_Pin,
                    KSPROPERTY_PIN_DATAFLOW,
                    &pNewPin->m_Descriptor.DataFlow,
                    sizeof(KSPIN_DATAFLOW)
                );
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_DATAFLOW");
                goto break_loop;
            }

            //
            // create a new CKsPin and add to the appropriate pin list
            //
            if (!pNewPin)
            {
                LOG(eWarn2, "Failed to create pin");
                goto break_loop;
            }

            m_listPins.AddTail(pNewPin);

            continue;

break_loop:
            if (pNewPin)
            {
                delete pNewPin;
                pNewPin = NULL;
            }
        }
    }

    ClassifyPins(&m_listPins);
    
    fViableFilter =
        !   (
                m_listRenderSinkPins.IsEmpty() && 
                m_listRenderSourcePins.IsEmpty() && 
                m_listCaptureSinkPins.IsEmpty() &&
                m_listCaptureSourcePins.IsEmpty()
            );

    // the end
    return fViableFilter;
}

// ----------------------------------------------------------------------------------
// CPCMAudioFilter::FindViablePin
//  look through m_listRenderPins or m_listCapturePins and find one that can do pwfx
// ----------------------------------------------------------------------------------
CPCMAudioPin*
CPCMAudioFilter::FindViablePin
(
    CList<CKsPin>*  pList,
    WAVEFORMATEX*   pwfx
)
{
    CNode<CKsPin>*  pNodeNext;
    CNode<CKsPin>*  pNode;
    CPCMAudioPin*   pPin;
    
    pNode = pList->GetHead();
    while(pNode)
    {
        pNodeNext = pList->GetNext(pNode);
        pPin = (CPCMAudioPin*)pNode->pData;

        BOOL    fViablePin = FALSE;

        //
        // make sure that pwfx parameters are in at least one DataRange supported by this pin
        //
        DWORD   dw = 0x00000000;

        if (pwfx->wBitsPerSample == 8)       dw |= FMT_8BIT ;
        if (pwfx->wBitsPerSample == 16)      dw |= FMT_16BIT;
        if (pwfx->wBitsPerSample == 20)      dw |= FMT_20BIT;
        if (pwfx->wBitsPerSample == 24)      dw |= FMT_24BIT;
        if (pwfx->wBitsPerSample == 32)      dw |= FMT_32BIT;

        if (pwfx->nSamplesPerSec ==  8000)   dw |= FMT_8000HZ ;
        if (pwfx->nSamplesPerSec == 11025)   dw |= FMT_11025HZ;
        if (pwfx->nSamplesPerSec == 16000)   dw |= FMT_16000HZ;
        if (pwfx->nSamplesPerSec == 22050)   dw |= FMT_22050HZ;
        if (pwfx->nSamplesPerSec == 32000)   dw |= FMT_32000HZ;
        if (pwfx->nSamplesPerSec == 44100)   dw |= FMT_44100HZ;
        if (pwfx->nSamplesPerSec == 48000)   dw |= FMT_48000HZ;
        if (pwfx->nSamplesPerSec == 96000)   dw |= FMT_96000HZ;

        if (pwfx->nChannels == 1)            dw |= FMT_MONO  ;
        if (pwfx->nChannels == 2)            dw |= FMT_STEREO;
        if (pwfx->nChannels == 4)            dw |= FMT_QUAD | FMT_SURROUND;
        if (pwfx->nChannels == 6)            dw |= FMT_5_1;
        if (pwfx->nChannels == 7)            dw |= FMT_7_1;

        fViablePin = (dw == (dw & pPin->m_dwWaveFormatMask));
        if (fViablePin)
            return pPin;

        pNode = pNodeNext;
    }

    return NULL;
}

// ----------------------------------------------------------------------------------
// CPCMAudioFilter::CreateRenderPin
//  look through m_listRenderPins and find one that can do pwfx
// ----------------------------------------------------------------------------------
BOOL
CPCMAudioFilter::CreateRenderPin
(
    WAVEFORMATEX*   pwfx,
    DWORD           dwBufferSeconds,
    DWORD           dwAlignment /* = 0 */
)
{
    CPCMAudioPin* pPin = FindViablePin(&m_listRenderSinkPins, pwfx);

    if (!pPin)
    {
        LOG(eWarn2, "Could not find a Render pin that supports the given wave format");
        SetLastError(ERROR_BAD_FORMAT);
        return FALSE;
    }

    SetLastError(0);
    m_pRenderPin = (CKsPin*)pPin;

    if (!pPin->SetFormat(pwfx))
    {
        LOG(eWarn2, "Failed to set Render Pin format");
        return FALSE;
    }

    if (pPin->Instantiate())
    {
        LOG(eInfo2, "Successfully instantiated Render Pin.  Handle = 0x%08x", pPin->m_handle);
    }
    else
    {
        LOG(eWarn2, "Failed to instantiate Render Pin");
        return FALSE;
    }

    return pPin->CreateDataBuffer(dwBufferSeconds, dwAlignment);
}

// ----------------------------------------------------------------------------------
// CPCMAudioFilter::CreateCapturePin
//  look through m_listCapturePins and find one that can do pwfx
// ----------------------------------------------------------------------------------
BOOL
CPCMAudioFilter::CreateCapturePin
(
    WAVEFORMATEX*   pwfx,
    DWORD           dwBufferSeconds,
    DWORD           dwAlignment /* = 0 */
)
{
    CPCMAudioPin*   pPin = FindViablePin(&m_listCaptureSinkPins, pwfx);

    if (!pPin)
    {
        LOG(eWarn2, "Could not find a Capture pin that supports the given wave format");
        SetLastError(ERROR_BAD_FORMAT);
        return FALSE;
    }

    SetLastError(0);
    m_pCapturePin = (CKsPin*)pPin;

    if (!pPin->SetFormat(pwfx))
    {
        LOG(eWarn2, "Failed to set Capture Pin format");
        return FALSE;
    }

    if (pPin->Instantiate())
    {
        LOG(eInfo2, "Successfully instantiated Capture Pin.  Handle = 0x%08x", pPin->m_handle);
    }
    else
    {
        LOG(eWarn2, "Failed to instantiate Capture Pin");
        return FALSE;
    }

    return pPin->CreateDataBuffer(dwBufferSeconds, dwAlignment);
}

