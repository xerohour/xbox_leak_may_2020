/***************************************************************************
 *
 *	Copyright (C) 1999-2000 Microsoft Corporation.	All Rights Reserved.
 *
 *	File:		 effects.h
 *
 *	Content:	 Declarations for the CEffectChain class and the CEffect
 *				 class hierarchy (CEffect, CDmoEffect and CSendEffect).
 *
 *	Description: These classes implement DX8 audio effects and sends.
 *				 More info in effects.cpp.
 *
 *	History:
 *
 * Date 	 By 	  Reason
 * ========  =======  ======================================================
 * 08/10/99  duganp   Created
 *
 ***************************************************************************/

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "mediaobj.h"

// Various conversions between reftimes, milliseconds, samples and bytes.
// (Defining 'sample' as a sample *block*, with samples for all channels)
// REFERENCE_TIME is in 100-ns units (so 1 reftime tick == 1e-7 seconds).

#define BLOCKALIGN(a, b) \
			(((a) / (b)) * (b))

__inline DWORD MsToBytes(DWORD ms, LPWAVEFORMATEX pwfx)
{
	return BLOCKALIGN(ms * pwfx->nAvgBytesPerSec / 1000, pwfx->nBlockAlign);
}
__inline DWORD BytesToMs(DWORD bytes, LPWAVEFORMATEX pwfx)
{
	return bytes * 1000 / pwfx->nAvgBytesPerSec;
}
__inline DWORD MsToSamples(DWORD ms, LPWAVEFORMATEX pwfx)
{
	return ms * pwfx->nSamplesPerSec / 1000;
}
__inline DWORD SamplesToMs(DWORD samples, LPWAVEFORMATEX pwfx)
{
	return samples * 1000 / pwfx->nSamplesPerSec;
}
__inline DWORD RefTimeToMs(REFERENCE_TIME rt)
{
	return (DWORD)(rt / 10000);
}
__inline REFERENCE_TIME MsToRefTime(DWORD ms)
{
	return (REFERENCE_TIME)ms * 10000;
}
__inline DWORD RefTimeToBytes(REFERENCE_TIME rt, LPWAVEFORMATEX pwfx)
{
	return (DWORD)(BLOCKALIGN(rt * pwfx->nAvgBytesPerSec / 10000000, pwfx->nBlockAlign));
}
__inline REFERENCE_TIME BytesToRefTime(DWORD bytes, LPWAVEFORMATEX pwfx)
{
	return (REFERENCE_TIME)bytes * 10000000 / pwfx->nAvgBytesPerSec;
}

// Figure out if position X is between A and B in a cyclic buffer
#define CONTAINED(A, B, X) ((A) < (B) ? (A) <= (X) && (X) <= (B) \

// Find the distance between positions A and B in a buffer of length L
#define DISTANCE(A, B, L) ((A) <= (B) ? (B) - (A) : (L) + (B) - (A))


#ifdef __cplusplus

//#include "mediaobj.h"   // For DMO_MEDIA_TYPE
//#include "tlist.h"	  // For the CObjectList class template

// Forward declarations
class CBuffer;
class CDirectSoundBufferConfig;
class CEffect;

// Utility functions for the simple mixer used by CSendEffect below
enum MIXMODE {OneToOne=1, MonoToStereo=2};
typedef void MIXFUNCTION(PVOID pSrc, PVOID pDest, DWORD dwSamples, DWORD dwAmpFactor, MIXMODE mixMode);
MIXFUNCTION Mix8bit;
MIXFUNCTION Mix16bit;

// Validator for effect descriptors (can't be in dsvalid.c because it uses C++)
BOOL IsValidEffectDesc(LPCDSEFFECTDESC, CBuffer*);


//
// The DirectSound effects chain class
//

class CEffectChain : public IUnknown //: public CDsBasicRuntime
{
	friend class CDSLink;  // FIXME - try to dissolve some of these friendships
	friend class CBuffer;  // So FindSendLoop() can get at m_fxList

// FIXME: sprinkle 'private/protected' liberally in this file
public:
	CEffectChain				 (CBuffer* pBuffer);
	~CEffectChain				 (void);

	HRESULT Initialize			 (DWORD dwFxCount, LPDSEFFECTDESC pFxDesc, LPDWORD pdwResultCodes);
	HRESULT Clone				 (CDirectSoundBufferConfig* pDSBConfigObj);
	HRESULT AcquireFxResources	 (void);
	HRESULT GetFxStatus 		 (LPDWORD pdwResultCodes);
	HRESULT GetEffectInterface	 (REFGUID guidObject, DWORD dwIndex, REFGUID iidInterface, LPVOID* ppObject);
	void	NotifyRelease		 (CBuffer*);
	DWORD	GetFxCount()		 {return m_fxList.GetCount();}

	// Effects processing methods
	void	Render				 (short *pnBuffer,DWORD dwLength,REFERENCE_TIME refTimeStart);


	// IUnknown methods
	STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

private:
	HRESULT FxDiscontinuity 	 (void);

	// Reference Count
	long						 m_cRef;

	// Effects processing state
	CTypedPtrList<CPtrList, CEffect *> m_fxList;		// Effect object chain
	CBuffer*					 m_pDsBuffer;			// Owning DirectSound buffer object
	LPWAVEFORMATEX				 m_pFormat; 			// Pointer to owning buffer's audio format
	BOOL						 m_fHasSend;			// Whether this FX chain contains any sends
														// FIXME: may not be necessary later
	HRESULT 					 m_hrInit;				// Return code from initialization
};


//
// Base class for all DirectSound audio effects
//

class CEffect : public IUnknown//: public CDsBasicRuntime  // FIXME: to save some memory we could derive CEffect from CRefCount 
										// and implement the ": CRefCount(1)", "delete this" stuff etc here.
{
public:
	CEffect 						(DSEFFECTDESC& fxDescriptor);
	virtual ~CEffect				(void) {}
	virtual HRESULT Initialize		(DMO_MEDIA_TYPE*) =0;
	virtual HRESULT Clone			(IMediaObject*, DMO_MEDIA_TYPE*) =0;
	virtual HRESULT Process 		(DWORD dwBytes, BYTE* pAudio, REFERENCE_TIME refTimeStart, DWORD dwSendOffset, LPWAVEFORMATEX pFormat) =0;
	virtual HRESULT Discontinuity	(void) = 0;
	virtual HRESULT GetInterface	(REFIID, LPVOID*) =0;

	// These two methods are only required by CSendEffect:
	virtual void NotifyRelease(CBuffer*) {}
	virtual CBuffer* GetDestBuffer(void) {return NULL;}

	// IUnknown methods
	STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	long							m_cRef;

	HRESULT AcquireFxResources		(void);

	DSEFFECTDESC					m_fxDescriptor; 	// Creation parameters
	DWORD							m_fxStatus; 		// Current effect status
};


//
// Class representing DirectX Media Object effects
//

class CDmoEffect : public CEffect
{
public:
	CDmoEffect				(DSEFFECTDESC& fxDescriptor);
	~CDmoEffect 			(void);
	HRESULT Initialize		(DMO_MEDIA_TYPE*);
	HRESULT Clone			(IMediaObject*, DMO_MEDIA_TYPE*);
	HRESULT Process 		(DWORD dwBytes, BYTE* pAudio, REFERENCE_TIME refTimeStart, DWORD dwSendOffset =0, LPWAVEFORMATEX pFormat =NULL);
	HRESULT Discontinuity	(void)							{return m_pMediaObject->Discontinuity(0);}
	HRESULT GetInterface	(REFIID riid, LPVOID* ppvObj)	{return m_pMediaObject->QueryInterface(riid, ppvObj);}

	IMediaObject*			m_pMediaObject; 		// The DMO's standard interface (required)
	IMediaObjectInPlace*	m_pMediaObjectInPlace;	// The DMO's special interface (optional)
};


//
// Class representing DirectSound audio sends
//

class CSendEffect : public CEffect
{
public:
	CSendEffect(DSEFFECTDESC& fxDescriptor, CBuffer* pSrcBuffer);
	~CSendEffect(void);

	HRESULT Initialize		(DMO_MEDIA_TYPE*);
	HRESULT Clone			(IMediaObject*, DMO_MEDIA_TYPE*);
	HRESULT Process 		(DWORD dwBytes, BYTE* pAudio, REFERENCE_TIME refTimeStart, DWORD dwSendOffset =0, LPWAVEFORMATEX pFormat =NULL);
	void	NotifyRelease	(CBuffer*);
#ifdef ENABLE_I3DL2SOURCE
	HRESULT Discontinuity	(void)							{return m_pI3DL2SrcDMO ? m_pI3DL2SrcDMO->Discontinuity(0) : DS_OK;}
#else
	HRESULT Discontinuity	(void)							{return DS_OK;}
#endif
	HRESULT GetInterface	(REFIID riid, LPVOID* ppvObj)	{return m_impDSFXSend.QueryInterface(riid, ppvObj);}
	CBuffer* GetDestBuffer(void)		{return m_pDestBuffer;}

	// IDirectSoundFXSend methods
	HRESULT SetAllParameters(LPCDSFXSend);
	HRESULT GetAllParameters(LPDSFXSend);

private:
	// COM interface helper object
	struct CImpDirectSoundFXSend : public IDirectSoundFXSend
	{
		// INTERFACE_SIGNATURE m_signature;
		CSendEffect* m_pObject;

		// IUnknown methods (FIXME - missing the param validation layer)
		ULONG	STDMETHODCALLTYPE AddRef()	{return m_pObject->AddRef();}
		ULONG	STDMETHODCALLTYPE Release() {return m_pObject->Release();}
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObj);

		// IDirectSoundFXSend methods (FIXME - missing the param validation layer)
		HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDSFXSend pcDsFxSend) {return m_pObject->SetAllParameters(pcDsFxSend);}
		HRESULT STDMETHODCALLTYPE GetAllParameters(LPDSFXSend pDsFxSend)  {return m_pObject->GetAllParameters(pDsFxSend);}
	};
	friend struct CImpDirectSoundFXSend;

	// Data members
	CImpDirectSoundFXSend		 m_impDSFXSend; 		// COM interface helper object
	MIXFUNCTION*				 m_pMixFunction;		// Current mixing routine
	MIXMODE 					 m_mixMode; 			// Current mixing mode
	CBuffer*					 m_pSrcBuffer;			// Source buffer for the send - FIXME: may be able to lose this
	CBuffer*					 m_pDestBuffer; 		// Destination buffer for the send
	LONG						 m_lSendLevel;			// DSBVOLUME attenuation (millibels)
	DWORD						 m_dwAmpFactor; 		// Corresponding amplification factor
#ifdef ENABLE_I3DL2SOURCE
	IMediaObject*				 m_pI3DL2SrcDMO;		// Interfaces on our contained I3DL2 source DMO
	IMediaObjectInPlace*		 m_pI3DL2SrcDMOInPlace; // (if this happends to be an I3DL2 send effect).
#endif
};


// FIXME: Support for IMediaObject-only DMOs goes here
#if 0

//
// Utility class used to wrap our audio buffers in an IMediaBuffer interface,
// so we can use a DMO's IMediaObject interface if it lacks IMediaObjectInPlace.
//

class CMediaBuffer //: public CUnknown // (but this has dependencies on CImpUnknown...)
{
	// Blah.
};

#endif // 0
#endif // __cplusplus
#endif // __EFFECTS_H__
