// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//
// Implementation of CAutDirectMusicSegmentState.
//

/*#include "stdinc.h"
#include "autsegmentstate.h"
#include "activescript.h"
#include "autconstants.h"
#include <limits>*/

#include "pchscript.h"

const WCHAR CAutDirectMusicSegmentState::ms_wszClassName[] = L"SegmentState";

//////////////////////////////////////////////////////////////////////
// Method Names/DispIDs

const DISPID DMPDISP_IsPlaying = 1;
const DISPID DMPDISP_Stop = 2;

const AutDispatchMethod CAutDirectMusicSegmentState::ms_Methods[] =
	{
		// dispid,				name,
			// return:	type,	(opt),	(iid),
			// parm 1:	type,	opt,	iid,
			// parm 2:	type,	opt,	iid,
			// ...
			// ADT_None
		{ DMPDISP_IsPlaying,					L"IsPlaying",
						ADT_Long,		true,	&IID_NULL,						// returns true if playing
						ADT_None },
		{ DMPDISP_Stop,							L"Stop",
						ADPARAM_NORETURN,
						ADT_Long,		true,	&IID_NULL,						// flags
						ADT_None },
		{ DISPID_UNKNOWN }
	};

const DispatchHandlerEntry<CAutDirectMusicSegmentState> CAutDirectMusicSegmentState::ms_Handlers[] =
	{
		{ DMPDISP_IsPlaying, IsPlaying },
		{ DMPDISP_Stop, Stop },
		{ DISPID_UNKNOWN }
	};

//////////////////////////////////////////////////////////////////////
// Creation

CAutDirectMusicSegmentState::CAutDirectMusicSegmentState(
		IUnknown* pUnknownOuter,
		const IID& iid,
		void** ppv,
		HRESULT *phr)
  : BaseImpSegSt(pUnknownOuter, iid, ppv, phr)
{
}

HRESULT
CAutDirectMusicSegmentState::CreateInstance(
		IUnknown* pUnknownOuter,
		const IID& iid,
		void** ppv)
{
	HRESULT hr = S_OK;
	CAutDirectMusicSegmentState *pInst = new CAutDirectMusicSegmentState(pUnknownOuter, iid, ppv, &hr);
	if (FAILED(hr))
	{
		delete pInst;
		return hr;
	}
	if (pInst == NULL)
		return E_OUTOFMEMORY;
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Automation

HRESULT
CAutDirectMusicSegmentState::IsPlaying(AutDispatchDecodedParams *paddp)
{
	LONG *plRet = reinterpret_cast<LONG*>(paddp->pvReturn);
	if (!plRet)
		return S_OK;

	IDirectMusicPerformance *pPerf = CActiveScriptManager::GetCurrentPerformanceWEAK();
	HRESULT hr = pPerf->IsPlaying(NULL, m_pITarget);
	*plRet = ::BoolForVB(hr == S_OK);
	return hr;
}

const FlagMapEntry gc_flagmapStop[] =
	{
		{ ScriptConstants::AtGrid,			DMUS_SEGF_GRID },
		{ ScriptConstants::AtBeat,			DMUS_SEGF_BEAT },
		{ ScriptConstants::AtMeasure,		DMUS_SEGF_MEASURE },
		{ ScriptConstants::AtImmediate,		DMUS_SEGF_DEFAULT }, // this flag gets flipped later
		{ 0 }
	};

HRESULT
CAutDirectMusicSegmentState::Stop(AutDispatchDecodedParams *paddp)
{
	LONG lFlags = paddp->params[0].lVal;
	DWORD dwFlags = MapFlags(lFlags, gc_flagmapStop);
		// Reverse the default flag because our flag means the opposite.  Default is the default and immediate is the flag.
		//dwFlags ^= DMUS_SEGF_DEFAULT;

        //If we're "borrowing" the default flag to signify "immediate" then we'll assume other SEGF transition flags aren't specified and simply
        //  turn off this bug.
        //  If other transition flags ARE specified then they'll override "immediate".
        if (dwFlags & DMUS_SEGF_DEFAULT)
        {
    		dwFlags ^= DMUS_SEGF_DEFAULT;
        }

        //If we're NOT borrowing the default flag to signify "immediate", it may or may not be necessary to add.  It will only be necessary if
        //  no other transition flags were specified.
        else
        {
            if (!(dwFlags & (DMUS_SEGF_SEGMENTEND | DMUS_SEGF_GRID | DMUS_SEGF_BEAT | DMUS_SEGF_MEASURE | DMUS_SEGF_MARKER | DMUS_SEGF_ALIGN)))
            {
	            dwFlags |= DMUS_SEGF_DEFAULT;
            }
        }


	IDirectMusicPerformance8 *pPerformance = CActiveScriptManager::GetCurrentPerformanceWEAK();

	__int64 i64IntendedStartTime;
	DWORD dwIntendedStartTimeFlags;
	CActiveScriptManager::GetCurrentTimingContext(&i64IntendedStartTime, &dwIntendedStartTimeFlags);
#ifdef DXAPI
	HRESULT hr = pPerformance->Stop(NULL, m_pITarget, i64IntendedStartTime, dwFlags | dwIntendedStartTimeFlags);
#else
    HRESULT hr = pPerformance->StopEx( m_pITarget, i64IntendedStartTime, dwFlags | dwIntendedStartTimeFlags);
#endif
	return hr;
}
