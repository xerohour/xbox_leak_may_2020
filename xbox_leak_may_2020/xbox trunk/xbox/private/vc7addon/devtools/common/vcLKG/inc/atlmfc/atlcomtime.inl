// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLCOMTIME_INL__
#define __ATLCOMTIME_INL__

#pragma once

#ifndef __ATLCOMTIME_H__
	#error ATLComTime.inl requires ATLComTime.h to be included first
#endif

#include <math.h>

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// COleDateTimeSpan
/////////////////////////////////////////////////////////////////////////////

ATLCOMTIME_INLINE COleDateTimeSpan::COleDateTimeSpan() : m_span(0), m_status(valid)
{
}

ATLCOMTIME_INLINE COleDateTimeSpan::COleDateTimeSpan(double dblSpanSrc) : m_span(dblSpanSrc), m_status(valid)
{
	CheckRange();
}

ATLCOMTIME_INLINE COleDateTimeSpan::COleDateTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
{
	SetDateTimeSpan(lDays, nHours, nMins, nSecs);
}

ATLCOMTIME_INLINE void COleDateTimeSpan::SetStatus(DateTimeSpanStatus status)
{
	m_status = status;
}

ATLCOMTIME_INLINE COleDateTimeSpan::DateTimeSpanStatus COleDateTimeSpan::GetStatus() const
{
	return m_status;
}

__declspec(selectany) const double
	COleDateTimeSpan::OLE_DATETIME_HALFSECOND =
	1.0 / (2.0 * (60.0 * 60.0 * 24.0));

ATLCOMTIME_INLINE double COleDateTimeSpan::GetTotalDays() const
{
	ATLASSERT(GetStatus() == valid);

	return LONG(m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND));
}

ATLCOMTIME_INLINE double COleDateTimeSpan::GetTotalHours() const
{
	ATLASSERT(GetStatus() == valid);

	return LONG((m_span + (m_span < 0 ? 
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * 24);
}

ATLCOMTIME_INLINE double COleDateTimeSpan::GetTotalMinutes() const
{
	ATLASSERT(GetStatus() == valid);

	return LONG((m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60));
}

ATLCOMTIME_INLINE double COleDateTimeSpan::GetTotalSeconds() const
{
	ATLASSERT(GetStatus() == valid);

	return LONG((m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60 * 60));
}

ATLCOMTIME_INLINE LONG COleDateTimeSpan::GetDays() const
{
	ATLASSERT(GetStatus() == valid);
	return LONG(m_span);
}

ATLCOMTIME_INLINE LONG COleDateTimeSpan::GetHours() const
{
	return LONG(GetTotalHours()) % 24;
}

ATLCOMTIME_INLINE LONG COleDateTimeSpan::GetMinutes() const
{
	return LONG(GetTotalMinutes()) % 60;
}

ATLCOMTIME_INLINE LONG COleDateTimeSpan::GetSeconds() const
{
	return LONG(GetTotalSeconds()) % 60;
}

ATLCOMTIME_INLINE COleDateTimeSpan& COleDateTimeSpan::operator=(double dblSpanSrc)
{
	m_span = dblSpanSrc;
	m_status = valid;
	CheckRange();
	return *this;
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator==(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return (m_status == dateSpan.m_status &&
		m_span == dateSpan.m_span);
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator!=(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return (m_status != dateSpan.m_status ||
		m_span != dateSpan.m_span);
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator<(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return m_span < dateSpan.m_span;
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator>(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return m_span > dateSpan.m_span;
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator<=(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return m_span <= dateSpan.m_span;
}

ATLCOMTIME_INLINE bool COleDateTimeSpan::operator>=(const COleDateTimeSpan& dateSpan) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return m_span >= dateSpan.m_span;
}

ATLCOMTIME_INLINE COleDateTimeSpan COleDateTimeSpan::operator+(const COleDateTimeSpan& dateSpan) const
{
	COleDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.m_span = m_span + dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

ATLCOMTIME_INLINE COleDateTimeSpan COleDateTimeSpan::operator-(const COleDateTimeSpan& dateSpan) const
{
	COleDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Subtract spans and validate within legal range
	dateSpanTemp.m_span = m_span - dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

ATLCOMTIME_INLINE COleDateTimeSpan& COleDateTimeSpan::operator+=(const COleDateTimeSpan dateSpan)
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	*this = *this + dateSpan;
	CheckRange();
	return *this;
}

ATLCOMTIME_INLINE COleDateTimeSpan& COleDateTimeSpan::operator-=(const COleDateTimeSpan dateSpan)
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	*this = *this - dateSpan;
	CheckRange();
	return *this;
}

ATLCOMTIME_INLINE COleDateTimeSpan COleDateTimeSpan::operator-() const
{
	return -this->m_span;
}

ATLCOMTIME_INLINE COleDateTimeSpan::operator double() const
{
	return m_span;
}

ATLCOMTIME_INLINE void COleDateTimeSpan::SetDateTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
{
	// Set date span by breaking into fractional days (all input ranges valid)
	m_span = lDays + ((double)nHours)/24 + ((double)nMins)/(24*60) +
		((double)nSecs)/(24*60*60);
	m_status = valid;
	CheckRange();
}

ATLCOMTIME_INLINE void COleDateTimeSpan::CheckRange()
{
	if(m_span < -maxDaysInSpan || m_span > maxDaysInSpan)
		m_status = invalid;
}

/////////////////////////////////////////////////////////////////////////////
// COleDateTime
/////////////////////////////////////////////////////////////////////////////

ATLCOMTIME_INLINE COleDateTime WINAPI COleDateTime::GetCurrentTime()
{
	return COleDateTime(::_time64(NULL));
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime() :
	m_dt( 0 ), m_status(valid)
{
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime( const VARIANT& varSrc ) :
	m_dt( 0 ), m_status(valid)
{
	*this = varSrc;
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime( DATE dtSrc ) :
	m_dt( dtSrc ), m_status(valid)
{
}

#ifndef  _WIN64

ATLCOMTIME_INLINE COleDateTime::COleDateTime( time_t timeSrc) :
	m_dt( 0 ), m_status(valid)
{
	*this = timeSrc;
}

#endif

ATLCOMTIME_INLINE COleDateTime::COleDateTime( __time64_t timeSrc ) :
	m_dt( 0 ), m_status(valid)
{
	*this = timeSrc;
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime( const SYSTEMTIME& systimeSrc ) :
	m_dt( 0 ), m_status(valid)
{
	*this = systimeSrc;
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime( const FILETIME& filetimeSrc ) :
	m_dt( 0 ), m_status(valid)
{
	*this = filetimeSrc;
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime(int nYear, int nMonth, int nDay,
	int nHour, int nMin, int nSec)
{
	SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
}

ATLCOMTIME_INLINE COleDateTime::COleDateTime(WORD wDosDate, WORD wDosTime)
{
	m_status = ::DosDateTimeToVariantTime(wDosDate, wDosTime, &m_dt) ?
		valid : invalid;
}

ATLCOMTIME_INLINE void COleDateTime::SetStatus(DateTimeStatus status)
{
	m_status = status;
}

ATLCOMTIME_INLINE COleDateTime::DateTimeStatus COleDateTime::GetStatus() const
{
	return m_status;
}

ATLCOMTIME_INLINE bool COleDateTime::GetAsSystemTime(SYSTEMTIME& sysTime) const
{
	return GetStatus() == valid && ::VariantTimeToSystemTime(m_dt, &sysTime);
}

ATLCOMTIME_INLINE bool COleDateTime::GetAsUDATE(UDATE &udate) const
{
	return SUCCEEDED(::VarUdateFromDate(m_dt, 0, &udate));
}

ATLCOMTIME_INLINE int COleDateTime::GetYear() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wYear : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetMonth() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wMonth : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetDay() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wDay : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetHour() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wHour : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetMinute() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wMinute : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetSecond() const
{ 
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wSecond : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetDayOfWeek() const
{
	SYSTEMTIME st;
	return GetAsSystemTime(st) ? st.wDayOfWeek + 1 : error;
}

ATLCOMTIME_INLINE int COleDateTime::GetDayOfYear() const
{
	UDATE udate;
	return GetAsUDATE(udate) ? udate.wDayOfYear : error;
}

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator=(const VARIANT& varSrc)
{
	if (varSrc.vt != VT_DATE)
	{
		VARIANT varDest;
		varDest.vt = VT_EMPTY;
		if(SUCCEEDED(::VariantChangeType(&varDest, const_cast<VARIANT *>(&varSrc), 0, VT_DATE)))
		{
			m_dt = varDest.date;
			m_status = valid;
		}
		else
			m_status = invalid;
	}
	else
	{
		m_dt = varSrc.date;
		m_status = valid;
	}

	return *this;
}

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator=(DATE dtSrc)
{
	m_dt = dtSrc;
	m_status = valid;
	return *this;
}

#ifndef  _WIN64

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator=(const time_t& timeSrc)
{
	return operator=(static_cast<__time64_t>(timeSrc));
}

#endif // _WIN64

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator=(const __time64_t& timeSrc)
{
	SYSTEMTIME st;
	CTime tmp(timeSrc);

	m_status = tmp.GetAsSystemTime(st) &&
		::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;

	return *this;
}

ATLCOMTIME_INLINE COleDateTime &COleDateTime::operator=(const SYSTEMTIME &systimeSrc)
{
	m_status = ::SystemTimeToVariantTime(const_cast<SYSTEMTIME *>(&systimeSrc), &m_dt) ?
		valid : invalid;
	return *this;
}

ATLCOMTIME_INLINE COleDateTime &COleDateTime::operator=(const FILETIME &filetimeSrc)
{
	SYSTEMTIME st;
	m_status = ::FileTimeToSystemTime(&filetimeSrc, &st) &&
				::SystemTimeToVariantTime(&st, &m_dt) ?
		valid : invalid;

	return *this;
}

ATLCOMTIME_INLINE COleDateTime &COleDateTime::operator=(const UDATE &udate)
{
	m_status = (S_OK == VarDateFromUdate((UDATE*)&udate, 0, &m_dt)) ? valid : invalid;

	return *this;
}

ATLCOMTIME_INLINE bool COleDateTime::operator==( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( m_dt == date.m_dt );
}

ATLCOMTIME_INLINE bool COleDateTime::operator!=( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( m_dt != date.m_dt );
}

ATLCOMTIME_INLINE bool COleDateTime::operator<( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( DoubleFromDate( m_dt ) < DoubleFromDate( date.m_dt ) );
}

ATLCOMTIME_INLINE bool COleDateTime::operator>( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( DoubleFromDate( m_dt ) > DoubleFromDate( date.m_dt ) );
}

ATLCOMTIME_INLINE bool COleDateTime::operator<=( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( DoubleFromDate( m_dt ) <= DoubleFromDate( date.m_dt ) );
}

ATLCOMTIME_INLINE bool COleDateTime::operator>=( const COleDateTime& date ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return( DoubleFromDate( m_dt ) >= DoubleFromDate( date.m_dt ) );
}

ATLCOMTIME_INLINE COleDateTime COleDateTime::operator+( COleDateTimeSpan dateSpan ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan ) ) );
}

ATLCOMTIME_INLINE COleDateTime COleDateTime::operator-( COleDateTimeSpan dateSpan ) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )-(double)dateSpan ) ) );
}

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator+=( COleDateTimeSpan dateSpan )
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	m_dt = DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan );
	return( *this );
}

ATLCOMTIME_INLINE COleDateTime& COleDateTime::operator-=( COleDateTimeSpan dateSpan )
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	m_dt = DateFromDouble( DoubleFromDate( m_dt )-(double)dateSpan );
	return( *this );
}

ATLCOMTIME_INLINE COleDateTimeSpan COleDateTime::operator-(const COleDateTime& date) const
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return DoubleFromDate(m_dt) - DoubleFromDate(date.m_dt);
}

ATLCOMTIME_INLINE COleDateTime::operator DATE() const
{
	ATLASSERT(GetStatus() == valid);
	return( m_dt );
}

ATLCOMTIME_INLINE int COleDateTime::SetDateTime(int nYear, int nMonth, int nDay,
	int nHour, int nMin, int nSec)
{
	SYSTEMTIME st;
	::ZeroMemory(&st, sizeof(SYSTEMTIME));

	st.wYear = WORD(nYear);
	st.wMonth = WORD(nMonth);
	st.wDay = WORD(nDay);
	st.wHour = WORD(nHour);
	st.wMinute = WORD(nMin);
	st.wSecond = WORD(nSec);

	return m_status = ::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;
}

ATLCOMTIME_INLINE int COleDateTime::SetDate(int nYear, int nMonth, int nDay)
{
	return SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
}

ATLCOMTIME_INLINE int COleDateTime::SetTime(int nHour, int nMin, int nSec)
{
	// Set date to zero date - 12/30/1899
	return SetDateTime(1899, 12, 30, nHour, nMin, nSec);
}

ATLCOMTIME_INLINE double WINAPI COleDateTime::DoubleFromDate( DATE date )
{
	double fTemp;

	// No problem if positive
	if( date >= 0 )
	{
		return( date );
	}

	// If negative, must convert since negative dates not continuous
	// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
	fTemp = ceil( date );

	return( fTemp-(date-fTemp) );
}

ATLCOMTIME_INLINE DATE WINAPI COleDateTime::DateFromDouble( double f )
{
	double fTemp;

	// No problem if positive
	if( f >= 0 )
	{
		return( f );
	}

	// If negative, must convert since negative dates not continuous
	// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
	fTemp = floor( f ); // fTemp is now whole part

	return( fTemp+(fTemp-f) );
}

}	// namespace ATL

#endif	// __ATLCOMTIME_INL__
