// MuteItem.h: interface for the CMuteItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEITEM_H__E41C4423_173C_11D2_850D_00A0C99F7E74__INCLUDED_)
#define AFX_MUTEITEM_H__E41C4423_173C_11D2_850D_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <conductor.h>

// TODO: use the the #define in dmusicf.h when it is added
#define PCHANNEL_MUTE 0xFFFFFFFF

class CMuteItem
{
public:
	// start,end beat,measure
	long		m_lStartMeasure;
    long		m_lStartBeat;
	// end measure and end beat are non-inclusive
	long		m_lEndMeasure;
	long		m_lEndBeat;

	// start, end times
	MUSIC_TIME	m_mtStart;
	MUSIC_TIME	m_mtEnd;

	// pchannel, and pchannel to map to
	DWORD		m_dwPChannel;
	DWORD		m_dwPChannelMap;

	inline void	SetStart(long lMeasure, long lBeat)	{
		m_lStartMeasure = lMeasure;
		m_lStartBeat = lBeat;
	}
	inline void	SetEnd(long lMeasure, long lBeat) {
		m_lEndMeasure = lMeasure;
		m_lEndBeat = lBeat;
	}
	inline void	SetStart(MUSIC_TIME mt) { m_mtStart = mt; }
	inline void	SetEnd(MUSIC_TIME mt) { m_mtEnd = mt; }
	
	inline MUSIC_TIME GetStartTime() { return m_mtStart; }
	inline MUSIC_TIME GetEndTime() { return m_mtEnd; }
	inline void GetStartMeasureBeat(long &lMeasure, long &lBeat) {
		lMeasure = m_lStartMeasure;
		lBeat = m_lStartBeat;
	}
	inline void GetEndMeasureBeat(long &lMeasure, long &lBeat) {
		lMeasure = m_lEndMeasure;
		lBeat = m_lEndBeat;
	}
/*
	inline bool StartEqualsEnd() {
		return (m_lStartMeasure == m_lEndMeasure &&
			m_lStartBeat == m_lEndBeat);
	}
*/
	
	inline bool IsUseless() {
		return (m_mtEnd - m_mtStart <= 0);
	}

	inline void SetPChannel(DWORD dwPChannel) {
		m_dwPChannel = dwPChannel;
	}
	
	inline DWORD GetPChannel() {
		return m_dwPChannel;
	}

	inline void SetPChannelMap(DWORD dwRemap) {
		m_dwPChannelMap = dwRemap;
	}

	inline DWORD GetPChannelMap() {
		return m_dwPChannelMap;
	}

	inline bool	IsMute() {
		return (m_dwPChannelMap == PCHANNEL_MUTE);
	}
	
	inline bool	IsRemap() {
		return !IsMute();
	}
	
	inline void	SetAsMute() {
		m_dwPChannelMap = PCHANNEL_MUTE;
	}

	inline bool Intersects(MUSIC_TIME mtStart, MUSIC_TIME mtEnd) {
		return 
			(m_mtStart >= mtStart && m_mtStart < mtEnd) ||
			(m_mtEnd > mtStart && m_mtEnd <= mtEnd) ||
			(m_mtStart < mtStart && m_mtEnd > mtEnd);
	}


};

#endif // !defined(AFX_MUTEITEM_H__E41C4423_173C_11D2_850D_00A0C99F7E74__INCLUDED_)
