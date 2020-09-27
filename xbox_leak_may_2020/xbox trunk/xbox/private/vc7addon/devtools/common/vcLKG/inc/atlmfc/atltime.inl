// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLTIME_INL__
#define __ATLTIME_INL__

#pragma once

#ifndef __ATLTIME_H__
	#error atltime.inl requires atltime.h to be included first
#endif

namespace ATL
{
/////////////////////////////////////////////////////////////////////////////
// CTimeSpan
/////////////////////////////////////////////////////////////////////////////

ATLTIME_INLINE CTimeSpan::CTimeSpan() :
	m_timeSpan(0)
{
}

ATLTIME_INLINE CTimeSpan::CTimeSpan( __time64_t time ) :
	m_timeSpan( time )
{
}

ATLTIME_INLINE CTimeSpan::CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
{
 	m_timeSpan = nSecs + 60* (nMins + 60* (nHours + __int64(24) * lDays));
}

ATLTIME_INLINE LONGLONG CTimeSpan::GetDays() const
{
	return( m_timeSpan/(24*3600) );
}

ATLTIME_INLINE LONGLONG CTimeSpan::GetTotalHours() const
{
	return( m_timeSpan/3600 );
}

ATLTIME_INLINE LONG CTimeSpan::GetHours() const
{
	return( LONG( GetTotalHours()-(GetDays()*24) ) );
}

ATLTIME_INLINE LONGLONG CTimeSpan::GetTotalMinutes() const
{
	return( m_timeSpan/60 );
}

ATLTIME_INLINE LONG CTimeSpan::GetMinutes() const
{
	return( LONG( GetTotalMinutes()-(GetTotalHours()*60) ) );
}

ATLTIME_INLINE LONGLONG CTimeSpan::GetTotalSeconds() const
{
	return( m_timeSpan );
}

ATLTIME_INLINE LONG CTimeSpan::GetSeconds() const
{
	return( LONG( GetTotalSeconds()-(GetTotalMinutes()*60) ) );
}

ATLTIME_INLINE __time64_t CTimeSpan::GetTimeSpan() const
{
	return( m_timeSpan );
}

ATLTIME_INLINE CTimeSpan CTimeSpan::operator+( CTimeSpan span ) const
{
	return( CTimeSpan( m_timeSpan+span.m_timeSpan ) );
}

ATLTIME_INLINE CTimeSpan CTimeSpan::operator-( CTimeSpan span ) const
{
	return( CTimeSpan( m_timeSpan-span.m_timeSpan ) );
}

ATLTIME_INLINE CTimeSpan& CTimeSpan::operator+=( CTimeSpan span )
{
	m_timeSpan += span.m_timeSpan;
	return( *this );
}

ATLTIME_INLINE CTimeSpan& CTimeSpan::operator-=( CTimeSpan span )
{
	m_timeSpan -= span.m_timeSpan;
	return( *this );
}

ATLTIME_INLINE bool CTimeSpan::operator==( CTimeSpan span ) const
{
	return( m_timeSpan == span.m_timeSpan );
}

ATLTIME_INLINE bool CTimeSpan::operator!=( CTimeSpan span ) const
{
	return( m_timeSpan != span.m_timeSpan );
}

ATLTIME_INLINE bool CTimeSpan::operator<( CTimeSpan span ) const
{
	return( m_timeSpan < span.m_timeSpan );
}

ATLTIME_INLINE bool CTimeSpan::operator>( CTimeSpan span ) const
{
	return( m_timeSpan > span.m_timeSpan );
}

ATLTIME_INLINE bool CTimeSpan::operator<=( CTimeSpan span ) const
{
	return( m_timeSpan <= span.m_timeSpan );
}

ATLTIME_INLINE bool CTimeSpan::operator>=( CTimeSpan span ) const
{
	return( m_timeSpan >= span.m_timeSpan );
}

/////////////////////////////////////////////////////////////////////////////
// CTime
/////////////////////////////////////////////////////////////////////////////

ATLTIME_INLINE CTime WINAPI CTime::GetCurrentTime()
{
	return( CTime( ::_time64( NULL ) ) );
}

ATLTIME_INLINE CTime::CTime() :
	m_time(0)
{
}

ATLTIME_INLINE CTime::CTime( __time64_t time ) :
	m_time( time )
{
}

ATLTIME_INLINE CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
	int nDST)
{
	struct tm atm;
	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	ATLASSERT(nDay >= 1 && nDay <= 31);
	atm.tm_mday = nDay;
	ATLASSERT(nMonth >= 1 && nMonth <= 12);
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
	ATLASSERT(nYear >= 1900);
	atm.tm_year = nYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	m_time = _mktime64(&atm);
	ATLASSERT(m_time != -1);       // indicates an illegal input time
}

ATLTIME_INLINE CTime::CTime(WORD wDosDate, WORD wDosTime, int nDST)
{
	struct tm atm;
	atm.tm_sec = (wDosTime & ~0xFFE0) << 1;
	atm.tm_min = (wDosTime & ~0xF800) >> 5;
	atm.tm_hour = wDosTime >> 11;

	atm.tm_mday = wDosDate & ~0xFFE0;
	atm.tm_mon = ((wDosDate & ~0xFE00) >> 5) - 1;
	atm.tm_year = (wDosDate >> 9) + 80;
	atm.tm_isdst = nDST;
	m_time = _mktime64(&atm);
	ATLASSERT(m_time != -1);       // indicates an illegal input time
}

ATLTIME_INLINE CTime::CTime(const SYSTEMTIME& sysTime, int nDST)
{
	if (sysTime.wYear < 1900)
	{
		__time64_t time0 = 0L;
		CTime timeT(time0);
		*this = timeT;
	}
	else
	{
		CTime timeT(
			(int)sysTime.wYear, (int)sysTime.wMonth, (int)sysTime.wDay,
			(int)sysTime.wHour, (int)sysTime.wMinute, (int)sysTime.wSecond,
			nDST);
		*this = timeT;
	}
}

ATLTIME_INLINE CTime::CTime(const FILETIME& fileTime, int nDST)
{
	// first convert file time (UTC time) to local time
	FILETIME localTime;
	if (!FileTimeToLocalFileTime(&fileTime, &localTime))
	{
		m_time = 0;
		return;
	}

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&localTime, &sysTime))
	{
		m_time = 0;
		return;
	}

	// then convert the system time to a time_t (C-runtime local time)
	CTime timeT(sysTime, nDST);
	*this = timeT;
}

ATLTIME_INLINE CTime& CTime::operator=( __time64_t time )
{
	m_time = time;

	return( *this );
}

ATLTIME_INLINE CTime& CTime::operator+=( CTimeSpan span )
{
	m_time += span.GetTimeSpan();

	return( *this );
}

ATLTIME_INLINE CTime& CTime::operator-=( CTimeSpan span )
{
	m_time -= span.GetTimeSpan();

	return( *this );
}

ATLTIME_INLINE CTimeSpan CTime::operator-( CTime time ) const
{
	return( CTimeSpan( m_time-time.m_time ) );
}

ATLTIME_INLINE CTime CTime::operator-( CTimeSpan span ) const
{
	return( CTime( m_time-span.GetTimeSpan() ) );
}

ATLTIME_INLINE CTime CTime::operator+( CTimeSpan span ) const
{
	return( CTime( m_time+span.GetTimeSpan() ) );
}

ATLTIME_INLINE bool CTime::operator==( CTime time ) const
{
	return( m_time == time.m_time );
}

ATLTIME_INLINE bool CTime::operator!=( CTime time ) const
{
	return( m_time != time.m_time );
}

ATLTIME_INLINE bool CTime::operator<( CTime time ) const
{
	return( m_time < time.m_time );
}

ATLTIME_INLINE bool CTime::operator>( CTime time ) const
{
	return( m_time > time.m_time );
}

ATLTIME_INLINE bool CTime::operator<=( CTime time ) const
{
	return( m_time <= time.m_time );
}

ATLTIME_INLINE bool CTime::operator>=( CTime time ) const
{
	return( m_time >= time.m_time );
}

ATLTIME_INLINE struct tm* CTime::GetGmtTm(struct tm* ptm) const
{
	if (ptm != NULL)
	{
		*ptm = *_gmtime64(&m_time);
		return ptm;
	}
	else
		return _gmtime64(&m_time);
}

ATLTIME_INLINE struct tm* CTime::GetLocalTm(struct tm* ptm) const
{
	if (ptm != NULL)
	{
		struct tm* ptmTemp = _localtime64(&m_time);
		if (ptmTemp == NULL)
			return NULL;    // indicates the m_time was not initialized!

		*ptm = *ptmTemp;
		return ptm;
	}
	else
		return _localtime64(&m_time);
}

ATLTIME_INLINE bool CTime::GetAsSystemTime(SYSTEMTIME& timeDest) const
{
	struct tm* ptm = GetLocalTm(NULL);
	if (!ptm)
		return false;

	timeDest.wYear = (WORD) (1900 + ptm->tm_year);
	timeDest.wMonth = (WORD) (1 + ptm->tm_mon);
	timeDest.wDayOfWeek = (WORD) ptm->tm_wday;
	timeDest.wDay = (WORD) ptm->tm_mday;
	timeDest.wHour = (WORD) ptm->tm_hour;
	timeDest.wMinute = (WORD) ptm->tm_min;
	timeDest.wSecond = (WORD) ptm->tm_sec;
	timeDest.wMilliseconds = 0;

	return true;
}

ATLTIME_INLINE __time64_t CTime::GetTime() const
{
	return( m_time );
}

ATLTIME_INLINE int CTime::GetYear() const
{ 
	return( GetLocalTm()->tm_year+1900 ); 
}

ATLTIME_INLINE int CTime::GetMonth() const
{ 
	return( GetLocalTm()->tm_mon+1 ); 
}

ATLTIME_INLINE int CTime::GetDay() const
{ 
	return( GetLocalTm()->tm_mday ); 
}

ATLTIME_INLINE int CTime::GetHour() const
{ 
	return( GetLocalTm()->tm_hour ); 
}

ATLTIME_INLINE int CTime::GetMinute() const
{ 
	return( GetLocalTm()->tm_min ); 
}

ATLTIME_INLINE int CTime::GetSecond() const
{ 
	return( GetLocalTm()->tm_sec ); 
}

ATLTIME_INLINE int CTime::GetDayOfWeek() const
{ 
	return( GetLocalTm()->tm_wday+1 ); 
}

/////////////////////////////////////////////////////////////////////////////
// CFileTimeSpan
/////////////////////////////////////////////////////////////////////////////

ATLTIME_INLINE CFileTimeSpan::CFileTimeSpan() :
	m_nSpan( 0 )
{
}

ATLTIME_INLINE CFileTimeSpan::CFileTimeSpan( const CFileTimeSpan& span ) :
	m_nSpan( span.m_nSpan )
{
}

ATLTIME_INLINE CFileTimeSpan::CFileTimeSpan( LONGLONG nSpan ) :
	m_nSpan( nSpan )
{
}

ATLTIME_INLINE CFileTimeSpan& CFileTimeSpan::operator=( const CFileTimeSpan& span )
{
	m_nSpan = span.m_nSpan;

	return( *this );
}

ATLTIME_INLINE CFileTimeSpan& CFileTimeSpan::operator+=( CFileTimeSpan span )
{
	m_nSpan += span.m_nSpan;

	return( *this );
}

ATLTIME_INLINE CFileTimeSpan& CFileTimeSpan::operator-=( CFileTimeSpan span )
{
	m_nSpan -= span.m_nSpan;

	return( *this );
}

ATLTIME_INLINE CFileTimeSpan CFileTimeSpan::operator+( CFileTimeSpan span ) const
{
	return( CFileTimeSpan( m_nSpan+span.m_nSpan ) );
}

ATLTIME_INLINE CFileTimeSpan CFileTimeSpan::operator-( CFileTimeSpan span ) const
{
	return( CFileTimeSpan( m_nSpan-span.m_nSpan ) );
}

ATLTIME_INLINE bool CFileTimeSpan::operator==( CFileTimeSpan span ) const
{
	return( m_nSpan == span.m_nSpan );
}

ATLTIME_INLINE bool CFileTimeSpan::operator!=( CFileTimeSpan span ) const
{
	return( m_nSpan != span.m_nSpan );
}

ATLTIME_INLINE bool CFileTimeSpan::operator<( CFileTimeSpan span ) const
{
	return( m_nSpan < span.m_nSpan );
}

ATLTIME_INLINE bool CFileTimeSpan::operator>( CFileTimeSpan span ) const
{
	return( m_nSpan > span.m_nSpan );
}

ATLTIME_INLINE bool CFileTimeSpan::operator<=( CFileTimeSpan span ) const
{
	return( m_nSpan <= span.m_nSpan );
}

ATLTIME_INLINE bool CFileTimeSpan::operator>=( CFileTimeSpan span ) const
{
	return( m_nSpan >= span.m_nSpan );
}

ATLTIME_INLINE LONGLONG CFileTimeSpan::GetTimeSpan() const
{
	return( m_nSpan );
}

ATLTIME_INLINE void CFileTimeSpan::SetTimeSpan( LONGLONG nSpan )
{
	m_nSpan = nSpan;
}


/////////////////////////////////////////////////////////////////////////////
// CFileTime
/////////////////////////////////////////////////////////////////////////////

ATLTIME_INLINE CFileTime::CFileTime()
{
	dwLowDateTime = 0;
	dwHighDateTime = 0;
}

ATLTIME_INLINE CFileTime::CFileTime( const FILETIME& ft )
{
	dwLowDateTime = ft.dwLowDateTime;
	dwHighDateTime = ft.dwHighDateTime;
}

ATLTIME_INLINE CFileTime::CFileTime( ULONGLONG nTime )
{
	dwLowDateTime = DWORD( nTime );
	dwHighDateTime = DWORD( nTime>>32 );
}

ATLTIME_INLINE CFileTime& CFileTime::operator=( const FILETIME& ft )
{
	dwLowDateTime = ft.dwLowDateTime;
	dwHighDateTime = ft.dwHighDateTime;

	return( *this );
}

ATLTIME_INLINE CFileTime WINAPI CFileTime::GetCurrentTime()
{
	CFileTime ft;
	GetSystemTimeAsFileTime(&ft);
	return ft;
}

ATLTIME_INLINE CFileTime& CFileTime::operator+=( CFileTimeSpan span )
{
	SetTime( GetTime()+span.GetTimeSpan() );

	return( *this );
}

ATLTIME_INLINE CFileTime& CFileTime::operator-=( CFileTimeSpan span )
{
	SetTime( GetTime()-span.GetTimeSpan() );

	return( *this );
}

ATLTIME_INLINE CFileTime CFileTime::operator+( CFileTimeSpan span ) const
{
	return( CFileTime( GetTime()+span.GetTimeSpan() ) );
}

ATLTIME_INLINE CFileTime CFileTime::operator-( CFileTimeSpan span ) const
{
	return( CFileTime( GetTime()-span.GetTimeSpan() ) );
}

ATLTIME_INLINE CFileTimeSpan CFileTime::operator-( CFileTime ft ) const
{
	return( CFileTimeSpan( GetTime()-ft.GetTime() ) );
}

ATLTIME_INLINE bool CFileTime::operator==( CFileTime ft ) const
{
	return( GetTime() == ft.GetTime() );
}

ATLTIME_INLINE bool CFileTime::operator!=( CFileTime ft ) const
{
	return( GetTime() != ft.GetTime() );
}

ATLTIME_INLINE bool CFileTime::operator<( CFileTime ft ) const
{
	return( GetTime() < ft.GetTime() );
}

ATLTIME_INLINE bool CFileTime::operator>( CFileTime ft ) const
{
	return( GetTime() > ft.GetTime() );
}

ATLTIME_INLINE bool CFileTime::operator<=( CFileTime ft ) const
{
	return( GetTime() <= ft.GetTime() );
}

ATLTIME_INLINE bool CFileTime::operator>=( CFileTime ft ) const
{
	return( GetTime() >= ft.GetTime() );
}

ATLTIME_INLINE ULONGLONG CFileTime::GetTime() const
{
	return( (ULONGLONG( dwHighDateTime )<<32)|dwLowDateTime );
}

ATLTIME_INLINE void CFileTime::SetTime( ULONGLONG nTime )
{
	dwLowDateTime = DWORD( nTime );
	dwHighDateTime = DWORD( nTime>>32 );
}

ATLTIME_INLINE CFileTime CFileTime::UTCToLocal() const
{
	CFileTime ftLocal;

	::FileTimeToLocalFileTime( this, &ftLocal );

	return( ftLocal );
}

ATLTIME_INLINE CFileTime CFileTime::LocalToUTC() const
{
	CFileTime ftUTC;

	::LocalFileTimeToFileTime( this, &ftUTC );

	return( ftUTC );
}

}  // namespace ATL
#endif //__ATLTIME_INL__
