/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef MusicTimeConverter_H
#define MusicTimeConverter_H

#include "timeline.h"
#include "dmusici.h"

#define PPQ DMUS_PPQ

#define NOTE_CLOCKS(note, ppq) ( (ppq)*4 /(note) )



// draw helper function
inline long LeftMargin(IDMUSProdTimeline* pTimeline)
{
	long lClocks, lPosition;
	pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
	pTimeline->ClocksToPosition(lClocks, &lPosition);
	return --lPosition;
}



class CMusicTimeConverter
// conversions as functions of timesig, measure, beat, 
{
	long	m_lTime;
public:
	CMusicTimeConverter()
	{
		m_lTime = 0;
	}
	CMusicTimeConverter(long time) : m_lTime(time)
	{
	}
	CMusicTimeConverter(long lMeasure, long lBeat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		SetTime( lMeasure, lBeat, pTimeline, dwGroupBits, dwIndex );
	}
	CMusicTimeConverter(long lPosition,IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		SetTimeUsingPosition( lPosition, pTimeline, dwGroupBits, dwIndex );
	}
	CMusicTimeConverter(const CMusicTimeConverter& cmt)
	{
		m_lTime = cmt.m_lTime;
	}
	CMusicTimeConverter& operator=(const CMusicTimeConverter& cmt)
	{
		m_lTime = cmt.m_lTime;
		return *this;
	}
	HRESULT GetMeasure(long& lMeasure, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		HRESULT hr = pTimeline->ClocksToMeasureBeat(dwGroupBits, dwIndex, m_lTime, &lMeasure, NULL);
		ASSERT(hr == S_OK);
		return hr;
	}
	HRESULT GetBeat(long& lBeat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		HRESULT hr = pTimeline->ClocksToMeasureBeat(dwGroupBits, dwIndex, m_lTime, NULL, &lBeat);
		ASSERT(hr == S_OK);
		return hr;
	}
	HRESULT GetMeasureBeat(long& lMeasure, long& lBeat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		HRESULT hr = pTimeline->ClocksToMeasureBeat(dwGroupBits, dwIndex, m_lTime, &lMeasure, &lBeat);
		ASSERT(hr == S_OK);
		return hr;
	}
	HRESULT GetMeasureBeat(long& lMeasureReturn, long& lBeatReturn, long lBeatsPerMeasure, long lBeat)
	{
		lBeatReturn = (m_lTime * lBeat) / (4 * PPQ);
		lMeasureReturn = lBeatReturn / lBeatsPerMeasure;
		lBeatReturn = lBeatReturn % lBeatsPerMeasure;
		return S_OK;
	}
	HRESULT GetTimeSig(DMUS_TIMESIGNATURE& dmTimeSig, IDMUSProdTimeline* pTimeline, 
						DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		return pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, m_lTime, NULL, &dmTimeSig);
	}
	HRESULT SetTime(long lMeasure, long lBeat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		HRESULT hr = pTimeline->MeasureBeatToClocks(dwGroupBits, dwIndex, lMeasure, lBeat, &m_lTime);
		ASSERT(hr == S_OK);
		return hr;
	}
	HRESULT SetTimeUsingPosition(long lPosition, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT (pTimeline);
		long lMeasure, lBeat;
		HRESULT hr = pTimeline->PositionToMeasureBeat(dwGroupBits, dwIndex, lPosition, &lMeasure, &lBeat);
		ASSERT(hr == S_OK);
		hr = pTimeline->MeasureBeatToClocks(dwGroupBits, dwIndex, lMeasure, lBeat, &m_lTime);
		ASSERT(hr == S_OK);
		return hr;
	}
	// dangerous because allows adding cmusictimeconverters by adding times which could be wrong in multimeters
	// useful for relational ops
	operator long()
	{
		return m_lTime;
	}
	/*
	bool operator < (const CMusicTimeConverter& cmt)
	{
		return m_lTime < cmt.m_lTime;
	}
	bool operator > (const CMusicTimeConverter& cmt)
	{
		return m_lTime > cmt.m_lTime;
	}
	*/
	long& Time()
	{
		return m_lTime;
	}
	CMusicTimeConverter& operator=(long lTime)
	{
		m_lTime = lTime;
		return *this;
	}
	HRESULT AddOffset(long lMeasure, long lBeat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		// find time sig at m_lTime
		DMUS_TIMESIGNATURE dmTimeSig;
		HRESULT hr;
		hr = GetTimeSig(dmTimeSig, pTimeline, dwGroupBits, dwIndex);
		if(SUCCEEDED(hr))
		{
			// find our measure and beat
			long lTmpMeasure, lTmpBeat;
			hr = GetMeasureBeat(lTmpMeasure,lTmpBeat, pTimeline, dwGroupBits, dwIndex);
			if(SUCCEEDED(hr))
			{
				long lTmpBeat2 = (lTmpBeat + lBeat) % dmTimeSig.bBeatsPerMeasure;
				lTmpMeasure = lTmpMeasure + lMeasure + (lTmpBeat + lBeat) / dmTimeSig.bBeatsPerMeasure;
				hr = SetTime(lTmpMeasure, lTmpBeat2, pTimeline, dwGroupBits, dwIndex);
			}
		}
		return hr;
	}
};



inline CMusicTimeConverter operator+(CMusicTimeConverter& a, CMusicTimeConverter& b)
{
	CMusicTimeConverter t(a);
	t.Time() += b.Time();
	return t;
}

inline CMusicTimeConverter operator-(CMusicTimeConverter& a, CMusicTimeConverter& b)
{
	CMusicTimeConverter t(a);
	t.Time() -= b.Time();
	return t;
}

#endif