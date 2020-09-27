/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		agcva1.h
 *  Content:	Concrete class that implements CAutoGainControl
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  12/01/99	pnewson Created it
 *  09/01/2000 georgioc started port to xbox
 ***************************************************************************/

#ifndef _AGCVA1_H_
#define _AGCVA1_H_

#include "dvntos.h"

class CVoiceActivation1
{
protected:
	DWORD m_dwFlags;
	DWORD m_dwSensitivity;
	int m_iSampleRate;

	int m_iEnvelopeSampleRate;
	int m_iCurSampleNum;
	int m_iCurEnvelopeValueFast;
	int m_iCurEnvelopeValueSlow;
	int m_iPrevEnvelopeSample;
	int m_iHangoverSamples;
	int m_iCurHangoverSamples;
	int m_iShiftConstantFast;
	int m_iShiftConstantSlow;

	BYTE m_bPeak;

	BOOL m_fVoiceDetectedNow;
	BOOL m_fVoiceHangoverActive;
	BOOL m_fVoiceDetectedThisFrame;

	
   
public:

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'lcvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

	CVoiceActivation1() 
		: m_bPeak(0)
		, m_fVoiceDetectedThisFrame(FALSE)
		, m_fVoiceDetectedNow(FALSE)
		{};

	virtual ~CVoiceActivation1() {};
	
	virtual HRESULT Init(
		DWORD dwFlags, 
		int iSampleRate, 
		DWORD dwSensitivity);
	virtual HRESULT Deinit();
	virtual HRESULT SetSensitivity(DWORD dwFlags, DWORD dwSensitivity);
	virtual HRESULT GetSensitivity(DWORD* pdwFlags, DWORD* pdwSensitivity);
	virtual HRESULT AnalyzeData(BYTE* pbAudioData, DWORD dwAudioDataSize/*, DWORD dwFrameTime*/);
	virtual HRESULT VAResults(BOOL* pfVoiceDetected);
	virtual HRESULT PeakResults(BYTE* pbPeakValue);
};

#endif


