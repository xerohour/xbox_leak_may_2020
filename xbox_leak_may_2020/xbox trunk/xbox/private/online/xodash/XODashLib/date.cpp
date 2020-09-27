/*************************************************************************************************\
Date.cpp  			: Implementation of all necessary Date functions
Creation Date		: 2/12/2002 9:47:54 AM
Copyright (c) Microsoft Corporation.  All rights reserved
Author				: Victor Blanco
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

#include "std.h"
#include "date.h"

int CDateObject::isLeapYear(int nYear)
{
    if (nYear % 100 == 0 && nYear % 400 != 0)
		return 0;

	if (nYear % 4 == 0)
		return 1;

	return 0;
}

int CDateObject::getDaysInMonth(int nMonth, int nYear)
{
	static short rgdays [] = { 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (nMonth != 2)
		return rgdays[nMonth - 1];

	return isLeapYear(nYear) ? 29 : 28;
}

CDateObject::CDateObject()
{
    SYSTEMTIME stime;
    GetSystemTime( &stime );
    SystemTimeToFileTime(&stime, &m_time);
}

CDateObject::~CDateObject()
{
}

void CDateObject::setFileTime ( FILETIME ftTime )
{
    if( ftTime.dwHighDateTime == 0 && ftTime.dwLowDateTime == 0 )
    {
	    SYSTEMTIME stime;
        GetSystemTime( &stime );
	    SystemTimeToFileTime(&stime, &m_time);
    }
    else
    {
        m_time.dwHighDateTime = ftTime.dwHighDateTime;
        m_time.dwLowDateTime  = ftTime.dwLowDateTime;
    }
}

int CDateObject::getDate()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wDay;
}

int CDateObject::getDay()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wDay;
}

int CDateObject::getFullYear()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wYear;
}

int CDateObject::getHours()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wHour;
}

int CDateObject::getMilliseconds()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wMilliseconds;
}

int CDateObject::getMinutes()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wMinute;
}

int CDateObject::getMonth()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wMonth;
}

int CDateObject::getSeconds()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wSecond;
}

int CDateObject::getUTCDate()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wDay;
}

int CDateObject::getUTCDay()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wDayOfWeek;
}

int CDateObject::getUTCFullYear()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wYear;
}

int CDateObject::getUTCHours()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wHour;
}

int CDateObject::getUTCMilliseconds()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wMilliseconds;
}

int CDateObject::getUTCMinutes()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wMinute;
}

int CDateObject::getUTCMonth()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wMonth - 1;
}

int CDateObject::getUTCSeconds()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	return st.wSecond;
}

int CDateObject::getYear()
{
	int nYear = getFullYear();

	return nYear;
}

const TCHAR* rgszMonth3 [] = { _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

TCHAR* CDateObject::toGMTString()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	TCHAR *szBuf = new TCHAR[32];
	_stprintf(szBuf, _T("%02d %s %d %02d:%02d:%02d GMT"), st.wDay, rgszMonth3[st.wMonth - 1], st.wYear, st.wHour, st.wMinute, st.wSecond);
	return szBuf;
}

// Initalize all of the date values to 1's and the current year
void CDateObject::clearDate()
{
    SYSTEMTIME st;
    GetSystemTime( &st );
    st.wMonth = st.wSecond = st.wDay = st.wHour = 1;

    SystemTimeToFileTime( &st, &m_time );
}

void CDateObject::setDay( unsigned int nDay )
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

    st.wDay = (WORD)nDay;
    SystemTimeToFileTime( &st, &m_time );
}

void CDateObject::setMonth( unsigned int nMonth )
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

    st.wMonth = (WORD)nMonth;
    SystemTimeToFileTime( &st, &m_time );
}

void CDateObject::setYear( unsigned int nYear )
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

    st.wYear = (WORD)nYear;
    SystemTimeToFileTime( &st, &m_time );
}

// Calc Age looks at the current system time against the time stored as the member variable
int CDateObject::calcAge( void )
{
    // Current time
    SYSTEMTIME stToday;
    GetSystemTime( &stToday );
	
    SYSTEMTIME stBday;
	FileTimeToSystemTime( &m_time, &stBday );

    int nYearsOld = stToday.wYear - stBday.wYear - 1;
    if( ( stToday.wMonth <= stBday.wMonth ) && ( stToday.wDay <= stBday.wDay ) )
        nYearsOld++;
    
    return nYearsOld;
}

void CDateObject::zeroTime( void )
{
    m_time.dwHighDateTime = 0;
    m_time.dwHighDateTime = 0;
}