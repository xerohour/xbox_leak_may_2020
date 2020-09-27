/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef MUSICTIME_GRID_CONVERTER_H
#define MUSICTIME_GRID_CONVERTER_H

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



class CMusicTimeGridConverter
// conversions as functions of timesig, measure, beat, grid
{
protected:
	long	m_lTime;
public:
	CMusicTimeGridConverter()
	{
		m_lTime = 0;
	}

	CMusicTimeGridConverter(long time) : m_lTime(time)
	{
	}

	CMusicTimeGridConverter(long lMeasure, long lBeat, long lGrid, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		SetTime( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits, dwIndex );
	}

	/*
	CMusicTimeGridConverter(long lPosition,IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		SetTimeUsingPosition( lPosition, pTimeline, dwGroupBits, dwIndex );
	}
	*/

	CMusicTimeGridConverter(const CMusicTimeGridConverter& cmt)
	{
		m_lTime = cmt.m_lTime;
	}

	CMusicTimeGridConverter& operator=(const CMusicTimeGridConverter& cmt)
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

	/*
	HRESULT GetGrid(long& lGrid, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}
		// TODO:?
		ASSERT(FALSE);
		//HRESULT hr = pTimeline->ClocksToMeasureBeat(dwGroupBits, dwIndex, m_lTime, NULL, &lBeat);
		//ASSERT(hr == S_OK);
		//return hr;
		return E_NOTIMPL;
	}
	*/

	HRESULT GetMeasureBeatGrid(long& lMeasure, long& lBeat, long& lGrid, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}

		// Find out which measure we're in
		if( SUCCEEDED( pTimeline->ClocksToMeasureBeat( dwGroupBits, dwIndex, m_lTime, &lMeasure, NULL ) ) )
		{
			// Find the time of the start of this measure
			long lTime;
			if( SUCCEEDED( pTimeline->MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, 0, &lTime ) ) )
			{
				// Get the TimeSig for this measure
				DMUS_TIMESIGNATURE ts;
				if( SUCCEEDED( pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, lTime, NULL, &ts ) ) )
				{
					// Compute the number of clocks in a beat and a grid
					long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
					long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

					// Convert mtTime into an offset from the start of this measure
					MUSIC_TIME mtTime = m_lTime - lTime;

					lBeat = mtTime / lBeatClocks;

					mtTime %= lBeatClocks;
					lGrid = mtTime / lGridClocks;

					return S_OK;
				}
			}
		}

		return E_FAIL;
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

	HRESULT SetTime(long lMeasure, long lBeat, long lGrid, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}

		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( pTimeline->MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, lTime, NULL, &ts ) ) )
			{
				// Compute the number of clocks per beat and per grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// Calculate the clock value
				m_lTime = lTime + lBeatClocks * lBeat + lGridClocks * lGrid;

				return S_OK;
			}
		}

		return E_FAIL;
	}

	HRESULT SetTimeUsingPosition(long lPosition, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		ASSERT(pTimeline);
		if(pTimeline == NULL)
		{
			return E_POINTER;
		}

		// Convert from pixels to clocks
		HRESULT hr = pTimeline->PositionToClocks( lPosition, &m_lTime );
		ASSERT(hr == S_OK);

		// Convert from clocks to measure, beat, grid
		long lMeasure, lBeat, lGrid;
		hr = GetMeasureBeatGrid( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits, dwIndex );
		ASSERT(hr == S_OK);

		// Convert from measure, beat, brid to clocks
		hr = SetTime( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits, dwIndex );
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
	bool operator < (const CMusicTimeGridConverter& cmt)
	{
		return m_lTime < cmt.m_lTime;
	}
	bool operator > (const CMusicTimeGridConverter& cmt)
	{
		return m_lTime > cmt.m_lTime;
	}
	*/

	long& Time()
	{
		return m_lTime;
	}

	CMusicTimeGridConverter& operator=(long lTime)
	{
		m_lTime = lTime;
		return *this;
	}

	HRESULT AddOneGrid(IDMUSProdTimeline* pTimeline, DWORD dwGroupBits, DWORD dwIndex=0)
	{
		// find time sig at m_lTime
		DMUS_TIMESIGNATURE dmTimeSig;
		HRESULT hr = GetTimeSig(dmTimeSig, pTimeline, dwGroupBits, dwIndex);
		if(SUCCEEDED(hr))
		{
			// Compute the number of clocks per grid
			long lGridClocks = ((DMUS_PPQ * 4) / dmTimeSig.bBeat) / dmTimeSig.wGridsPerBeat;

			// Increment m_lTime;
			m_lTime += lGridClocks;
		}
		return hr;
	}
};



inline CMusicTimeGridConverter operator+(CMusicTimeGridConverter& a, CMusicTimeGridConverter& b)
{
	CMusicTimeGridConverter t(a);
	t.Time() += b.Time();
	return t;
}

inline CMusicTimeGridConverter operator-(CMusicTimeGridConverter& a, CMusicTimeGridConverter& b)
{
	CMusicTimeGridConverter t(a);
	t.Time() -= b.Time();
	return t;
}

#endif //MUSICTIME_GRID_CONVERTER_H