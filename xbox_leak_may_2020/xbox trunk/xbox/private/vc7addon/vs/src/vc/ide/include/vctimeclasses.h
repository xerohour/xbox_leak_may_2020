// (DianeMe) This was the MFC time manipulation stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.
// NOTE: exceptions are no longer thrown on various errors...

#pragma once

#include <time.h>
#include <vcstring.h>

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class CVCTime;                          // absolute time/date
class CVCTimeSpan;

#ifndef _countof
// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

/////////////////////////////////////////////////////////////////////////////

class CVCTimeSpan
{
public:

// Constructors
	CVCTimeSpan() {}
	CVCTimeSpan(__time64_t time) { m_timeSpan = time; }
	CVCTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
 	{ m_timeSpan = nSecs + 60* (nMins + 60* (nHours + __int64(24) * lDays)); }

	CVCTimeSpan(const CVCTimeSpan& timeSpanSrc)	{ m_timeSpan = timeSpanSrc.m_timeSpan; }
	const CVCTimeSpan& operator=(const CVCTimeSpan& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }

// Attributes
	// extract parts
	LONG GetDays() const	{ _ASSERT(m_timeSpan <= UINT_MAX); return (LONG) (m_timeSpan / (24*3600L)); }
	LONG GetTotalHours() const	{ _ASSERT(m_timeSpan <= UINT_MAX); return (LONG) (m_timeSpan/3600); }
	int GetHours() const	{ return (int)(GetTotalHours() - GetDays()*24); }
	LONG GetTotalMinutes() const	{ _ASSERT(m_timeSpan <= UINT_MAX); return (LONG) (m_timeSpan/60); }
	int GetMinutes() const	{ return (int)(GetTotalMinutes() - GetTotalHours()*60); }
	LONG GetTotalSeconds() const	{ _ASSERT(m_timeSpan <= UINT_MAX); return (LONG) m_timeSpan; }
	int GetSeconds() const	{ return (int)(GetTotalSeconds() - GetTotalMinutes()*60); }

	__int64 GetDays64() const	{ return m_timeSpan / (24*3600L); }
	__int64 GetTotalHours64() const	{ return m_timeSpan/3600; }
	__int64 GetHours64() const	{ return (GetTotalHours64() - GetDays64()*24); }
	__int64 GetTotalMinutes64() const	{ return m_timeSpan/60; }
	__int64 GetMinutes64() const	{ return (GetTotalMinutes64() - GetTotalHours64()*60); }
	__int64 GetTotalSeconds64() const	{ return m_timeSpan; }
	__int64 GetSeconds64() const	{ return (int)(GetTotalSeconds64() - GetTotalMinutes64()*60); }

// Operations
	// time math
	CVCTimeSpan operator-(CVCTimeSpan timeSpan) const	{ return CVCTimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
	CVCTimeSpan operator+(CVCTimeSpan timeSpan) const	{ return CVCTimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
	const CVCTimeSpan& operator+=(CVCTimeSpan timeSpan)	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
	const CVCTimeSpan& operator-=(CVCTimeSpan timeSpan)	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
	BOOL operator==(CVCTimeSpan timeSpan) const	{ return m_timeSpan == timeSpan.m_timeSpan; }
	BOOL operator!=(CVCTimeSpan timeSpan) const	{ return m_timeSpan != timeSpan.m_timeSpan; }
	BOOL operator<(CVCTimeSpan timeSpan) const	{ return m_timeSpan < timeSpan.m_timeSpan; }
	BOOL operator>(CVCTimeSpan timeSpan) const	{ return m_timeSpan > timeSpan.m_timeSpan; }
	BOOL operator<=(CVCTimeSpan timeSpan) const	{ return m_timeSpan <= timeSpan.m_timeSpan; }
	BOOL operator>=(CVCTimeSpan timeSpan) const	{ return m_timeSpan >= timeSpan.m_timeSpan; }

#ifdef _UNICODE
	// for compatibility with MFC 3.x
	CVCString Format(LPCSTR pFormat) const;
#endif
	CVCString Format(LPCTSTR pFormat) const;
	CVCString Format(UINT nID) const;

private:
	__time64_t m_timeSpan;
	friend class CVCTime;
};

class CVCTime
{
public:

// Constructors
	static CVCTime PASCAL GetCurrentTime();

	CVCTime() {}
	CVCTime(__time64_t time64)	{ m_time = time64; }
	CVCTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST = -1);
	CVCTime(WORD wDosDate, WORD wDosTime, int nDST = -1);
	CVCTime(const CVCTime& timeSrc)	{ m_time = timeSrc.m_time; }

	CVCTime(const SYSTEMTIME& sysTime, int nDST = -1);
	CVCTime(const FILETIME& fileTime, int nDST = -1);
	const CVCTime& operator=(const CVCTime& timeSrc) { m_time = timeSrc.m_time; return *this; }

// Attributes
	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;
	BOOL GetAsSystemTime(SYSTEMTIME& timeDest) const;

	time_t GetTime() const 	{ _ASSERT(m_time <= UINT_MAX); return time_t(m_time); }
	__time64_t GetTime64() const 	{ return m_time; }
	int GetYear() const 	{ return (GetLocalTm(NULL)->tm_year) + 1900; }
	int GetMonth() const	{ return GetLocalTm(NULL)->tm_mon + 1; }       // month of year (1 = Jan)
	int GetDay() const	{ return GetLocalTm(NULL)->tm_mday; }         // day of month
	int GetHour() const	{ return GetLocalTm(NULL)->tm_hour; }
	int GetMinute() const 	{ return GetLocalTm(NULL)->tm_min; }
	int GetSecond() const	{ return GetLocalTm(NULL)->tm_sec; }
	int GetDayOfWeek() const	{ return GetLocalTm(NULL)->tm_wday + 1; }   // 1=Sun, 2=Mon, ..., 7=Sat

// Operations
	// time math
	CVCTimeSpan operator-(CVCTime time) const	{ return CVCTimeSpan(m_time - time.m_time); }
	CVCTime operator-(CVCTimeSpan timeSpan) const	{ return CVCTime(m_time - timeSpan.m_timeSpan); }
	CVCTime operator+(CVCTimeSpan timeSpan) const	{ return CVCTime(m_time + timeSpan.m_timeSpan); }
	const CVCTime& operator+=(CVCTimeSpan timeSpan)	{ m_time += timeSpan.m_timeSpan; return *this; }
	const CVCTime& operator-=(CVCTimeSpan timeSpan)	{ m_time -= timeSpan.m_timeSpan; return *this; }
	BOOL operator==(CVCTime time) const	{ return m_time == time.m_time; }
	BOOL operator!=(CVCTime time) const	{ return m_time != time.m_time; }
	BOOL operator<(CVCTime time) const	{ return m_time < time.m_time; }
	BOOL operator>(CVCTime time) const	{ return m_time > time.m_time; }
	BOOL operator<=(CVCTime time) const	{ return m_time <= time.m_time; }
	BOOL operator>=(CVCTime time) const	{ return m_time >= time.m_time; }

	// formatting using "C" strftime
	CVCString Format(LPCTSTR pFormat) const;
	CVCString FormatGmt(LPCTSTR pFormat) const;
	CVCString Format(UINT nFormatID) const;
	CVCString FormatGmt(UINT nFormatID) const;

#ifdef _UNICODE
	// for compatibility with MFC 3.x
	CVCString Format(LPCSTR pFormat) const;
	CVCString FormatGmt(LPCSTR pFormat) const;
#endif

private:
	__time64_t m_time;
};

