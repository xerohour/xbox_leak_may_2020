#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"
#include "Date.h"
#include "Locale.h"
#include <xconfig.h>

extern CObject** g_rgParam;
extern int g_nParam;
extern CRunner* g_pRunner;
extern CObject* Dereference(CObject* pObject);


IMPLEMENT_NODE("Date", CDateObject, CObject)

START_NODE_FUN(CDateObject, CObject)
	NODE_FUN_IV(getDate)
	NODE_FUN_IV(getDay)
	NODE_FUN_IV(getFullYear)
	NODE_FUN_IV(getHours)
	NODE_FUN_IV(getMilliseconds)
	NODE_FUN_IV(getMinutes)
	NODE_FUN_IV(getMonth)
	NODE_FUN_IV(getSeconds)
	NODE_FUN_IV(getUTCDate)
	NODE_FUN_IV(getUTCDay)
	NODE_FUN_IV(getUTCFullYear)
	NODE_FUN_IV(getUTCHours)
	NODE_FUN_IV(getUTCMilliseconds)
	NODE_FUN_IV(getUTCMinutes)
	NODE_FUN_IV(getUTCMonth)
	NODE_FUN_IV(getUTCSeconds)
	NODE_FUN_SV(toGMTString)
	NODE_FUN_SV(toLocaleString)
	NODE_FUN_SV(toUTCString)

	NODE_FUN_II(isLeapYear)
	NODE_FUN_III(getDaysInMonth)

	NODE_FUN_VV(SetSystemClock)
END_NODE_FUN()

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

	if (g_nParam == 0)
	{
		GetSystemTime(&stime);
	}
	else if (g_nParam >= 3 && g_nParam <= 7)
	{
		ZeroMemory(&stime, sizeof (stime));

		for (int i = 0; i < g_nParam; i += 1)
			g_rgParam[i] = Dereference(g_rgParam[i]);

		CNumObject* pNum = g_rgParam[0]->ToNum();
		stime.wYear = (WORD)pNum->m_nValue;
		if (stime.wYear < 100)
			stime.wYear += 1900;
		pNum->Release();

		pNum = g_rgParam[1]->ToNum();
		stime.wMonth = (WORD)pNum->m_nValue + 1;
		pNum->Release();

		pNum = g_rgParam[2]->ToNum();
		stime.wDay = (WORD)pNum->m_nValue;
		pNum->Release();

		if (g_nParam > 3)
		{
			pNum = g_rgParam[3]->ToNum();
			stime.wHour = (WORD)pNum->m_nValue;
			pNum->Release();

			if (g_nParam > 4)
			{
				pNum = g_rgParam[4]->ToNum();
				stime.wMinute = (WORD)pNum->m_nValue;
				pNum->Release();

				if (g_nParam > 5)
				{
					pNum = g_rgParam[5]->ToNum();
					stime.wSecond = (WORD)pNum->m_nValue;
					pNum->Release();

					if (g_nParam > 6)
					{
						pNum = g_rgParam[6]->ToNum();
						stime.wMilliseconds = (WORD)pNum->m_nValue;
						pNum->Release();
					}
				}
			}
		}
	}
	else
	{
		g_pRunner->Error(_T("Wrong number of parameters to Date constructor\n"));
		return;
	}

	VERIFY(SystemTimeToFileTime(&stime, &m_time));
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
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wDayOfWeek;
}

int CDateObject::getFullYear()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

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
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	return st.wMonth - 1;
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

	if (nYear >= 1900 && nYear < 2000)
		return nYear - 1900;

	return nYear;
}

const TCHAR* rgszMonth3 [] = { _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

CStrObject* CDateObject::toGMTString()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	TCHAR szBuf [32];
	_stprintf(szBuf, _T("%02d %s %d %02d:%02d:%02d GMT"), st.wDay, rgszMonth3[st.wMonth - 1], st.wYear, st.wHour, st.wMinute, st.wSecond);
	return new CStrObject(szBuf);
}

CStrObject* CDateObject::toLocaleString()
{
	FILETIME local;
	FileTimeToLocalFileTime(&m_time, &local);

	SYSTEMTIME st;
	FileTimeToSystemTime(&local, &st);

	TCHAR szBuf [32];
	FormatTime(szBuf, countof(szBuf), &st);
	return new CStrObject(szBuf);
}

CStrObject* CDateObject::toUTCString()
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&m_time, &st);

	TCHAR szBuf [32];
	_stprintf(szBuf, _T("%02d %s %d %02d:%02d:%02d UTC"), st.wDay, rgszMonth3[st.wMonth - 1], st.wYear, st.wHour, st.wMinute, st.wSecond);
	return new CStrObject(szBuf);
}


void CDateObject::SetSystemClock()
{
	SYSTEMTIME stime;
	FileTimeToSystemTime(&m_time, &stime);

	TRACE(_T("Setting system clock to: %d/%d/%d %d:%02d:%02d:%03d\n"),
		stime.wMonth, stime.wDay, stime.wYear,
        stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds);

	VERIFY(XapiSetLocalTime(&stime));

    //
    // NOTE:
    //  If the current time is daylight saving time and
    //  the new time is in standard time, or vice versa,
    //  the result will be off by one hour (+/-).
    //
    //  If that's case, we need to set it again.
    //

    SYSTEMTIME newstime;
    FILETIME newftime;
    LONGLONG offset;

    GetLocalTime(&newstime);
    VERIFY(SystemTimeToFileTime(&newstime, &newftime));

    ASSERT(sizeof(FILETIME) == sizeof(LONGLONG));
    offset = *((LONGLONG*) &newftime) - *((LONGLONG*) &m_time);

    // if we're off by more than 1 minute, set it again
    if ((offset >= 0 ? offset : -offset) >= 600000000) {
        VERIFY(XapiSetLocalTime(&stime));
    }

    //
    // Check to see if this is the first time we're setting date/time
    // If so, remember the date/time information in the refurb sector as well.
    //
    XBOX_REFURB_INFO refurbInfo;
    NTSTATUS status;
    status = ExReadWriteRefurbInfo(&refurbInfo, sizeof(refurbInfo), FALSE);
    if (NT_SUCCESS(status) && refurbInfo.FirstSetTime.QuadPart == 0) {
        refurbInfo.FirstSetTime.LowPart = m_time.dwLowDateTime;
        refurbInfo.FirstSetTime.HighPart = m_time.dwHighDateTime;
        status = ExReadWriteRefurbInfo(&refurbInfo, sizeof(refurbInfo), TRUE);
    }
    ASSERT(NT_SUCCESS(status));
}

