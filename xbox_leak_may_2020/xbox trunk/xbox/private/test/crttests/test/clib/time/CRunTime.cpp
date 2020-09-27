
#include <xtl.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
//#include <afx.h>
#include "CRunTime.h"
#include "testcrt.h"
#ifndef _WIN32
#include <dos.h>
#endif
/*
BOOL isDST(CTime cTime) {
	BOOL bRetVal = FALSE;
	if ((cTime.GetMonth() == 4) && (cTime.GetDay() > cTime.GetDayOfWeek())) {
		bRetVal = TRUE;
	}
	else if ((cTime.GetMonth() > 4) && (cTime.GetMonth() < 10)) {
		bRetVal = TRUE;
	}
	else if (cTime.GetMonth() == 10 ) {
		if ((31-cTime.GetDay()) < 7) {
			if ((7 - (31-cTime.GetDay())) >= cTime.GetDayOfWeek()) {
				bRetVal = FALSE;
			}
			else bRetVal = TRUE;
			}
		else bRetVal = TRUE;
	}
	return bRetVal;
}
*/

#ifndef _WIN32
void Sleep (clock_t wait) {
	clock_t goal;
	goal = wait + clock();
	while (goal > clock());
}
#endif


BOOL CheckDatesDOSTm(struct tm *ptmTime, DTMINFO tDates) {
	BOOL bRetVal;
	bRetVal =           compare(_T("Year     "), ptmTime->tm_year + 1900, tDates.nYear);
	bRetVal = bRetVal | compare(_T("Month    "), ptmTime->tm_mon + 1    , tDates.nMonth);
	bRetVal = bRetVal | compare(_T("Day      "), ptmTime->tm_mday       , tDates.nDay);
	bRetVal = bRetVal | compare(_T("DayOfWeek"), ptmTime->tm_wday + 1   , tDates.nDayOfWeek);
	bRetVal = bRetVal | compare(_T("Hour     "), ptmTime->tm_hour       , tDates.nHour);
	bRetVal = bRetVal | compare(_T("Minute   "), ptmTime->tm_min        , tDates.nMinute);
	bRetVal = bRetVal | compare(_T("Second   "), ptmTime->tm_sec        , tDates.nSecond);

	return bRetVal;
}

#ifndef _WIN32 
BOOL CheckDatesDOSSYS(_dosdate_t sysDate, DTMINFO tDates) {
	BOOL bRetVal;
	bRetVal =           compare(_T("Year     "), sysDate.year         , tDates.nYear);
	bRetVal = bRetVal | compare(_T("Month    "), sysDate.month        , tDates.nMonth);
	bRetVal = bRetVal | compare(_T("Day      "), sysDate.day          , tDates.nDay);
	bRetVal = bRetVal | compare(_T("DayOfWeek"), sysDate.dayofweek + 1, tDates.nDayOfWeek);
	return bRetVal;
}

#else
BOOL CheckDatesSYS(SYSTEMTIME sysDate,  DTMINFO tDates) {
	BOOL bRetVal;
	bRetVal =           compare(_T("Year     "), sysDate.wYear         , tDates.nYear);
	bRetVal = bRetVal | compare(_T("Month    "), sysDate.wMonth        , tDates.nMonth);
	bRetVal = bRetVal | compare(_T("Day      "), sysDate.wDay          , tDates.nDay);
	bRetVal = bRetVal | compare(_T("DayOfWeek"), sysDate.wDayOfWeek + 1, tDates.nDayOfWeek);
	return bRetVal;
}

BOOL CheckDatesSYS2(SYSTEMTIME sysDate,  DTMINFO tDates) {
	BOOL bRetVal;
	bRetVal =           compare(_T("Year     "), sysDate.wYear         , tDates.nYear);
	bRetVal = bRetVal | compare(_T("Month    "), sysDate.wMonth        , tDates.nMonth);
	bRetVal = bRetVal | compare(_T("Day      "), sysDate.wDay          , tDates.nDay);
	bRetVal = bRetVal | compare(_T("DayOfWeek"), sysDate.wDayOfWeek + 1, tDates.nDayOfWeek);
	bRetVal = bRetVal | compare(_T("Hour     "), sysDate.wHour         , tDates.nHour);
	bRetVal = bRetVal | compare(_T("Minute   "), sysDate.wMinute       , tDates.nMinute);
	bRetVal = bRetVal | compare(_T("Second   "), sysDate.wSecond       , tDates.nSecond);
	return bRetVal;
}

#endif

BOOL TestCRunTime() {
	BOOL bRetVal;
	BOOL bResult;
//	CString sOut(' ', 128);

	struct tm tmTime;
	struct tm *ptmTime2;
	time_t tTime;
#ifndef _WIN32
	struct _dosdate_t sysDate;
	struct _dostime_t sysTime;
	struct _dosdate_t sysDate2;
	struct _dostime_t sysTime2;
#else	
	SYSTEMTIME sysDate;
	SYSTEMTIME sysDate2;
	FILETIME fileTime;
	FILETIME fileTime2;
#endif
	int x;

	bRetVal = FALSE;
#ifndef _WIN32
//	sOut = "Checking CRunTime: _dos_setdate(_dosdate_t &sysDate);\n";
	fputs ("Checking CRunTime: _dos_setdate(_dosdate_t &sysDate);\n", f1);
//	f1.WriteString(sOut);
//	sOut = "                   _dos_settime(_dostime_t &sysTime);\n";
	fputs ("                   _dos_settime(_dostime_t &sysTime);\n", f1);
#else
//	sOut = "Checking CRunTime: SetSystemTime(SYSTEMTIME &sysTime);\n";
	fputs ("Checking CRunTime: SetSystemTime(SYSTEMTIME &sysTime);\n", f1);
#endif
//	f1.WriteString(sOut);
	for (x = 0; tDates[x].nYear != 0;x++) {
		if (((tDates[x].nMonth == 2) && (tDates[x].nDay == 28) ) || ((tDates[x].nMonth == 12) && (tDates[x].nDay == 31))) {

#ifndef _WIN32
			sysDate2.year = tDates[x].nYear;
			sysDate2.month = tDates[x].nMonth;
			sysDate2.day = tDates[x].nDay;
			sysDate2.dayofweek = tDates[x].nDayOfWeek - 1;
			sysTime2.hour = 23;
			sysTime2.minute = 59;
			sysTime2.second = 59;
			sysTime2.hsecond = 0;
			_dos_getdate(&sysDate);
			_dos_gettime(&sysTime);
			bResult = _dos_setdate(&sysDate2);
			bResult = _dos_settime(&sysTime2);
			Sleep(2000);
			_dos_getdate(&sysDate2);
			_dos_gettime(&sysTime2);
			bResult = _dos_setdate(&sysDate);
			bResult = _dos_settime(&sysTime);
			bRetVal = bRetVal | CheckDatesDOSSYS(sysDate2,tDates[x + 1]);
#else
			sysDate2.wYear = tDates[x].nYear;
			sysDate2.wMonth = tDates[x].nMonth;
			sysDate2.wDay = tDates[x].nDay;
			sysDate2.wDayOfWeek = tDates[x].nDayOfWeek - 1;
			sysDate2.wHour = 23;
			sysDate2.wMinute = 59;
			sysDate2.wSecond = 59;
			sysDate2.wMilliseconds = 0;
			GetSystemTime(&sysDate);
            // xbox does not have SetSystemTime()
			// bResult = SetSystemTime(&sysDate2);
			Sleep(2000);
			GetSystemTime(&sysDate2);
            // xbox does not have SetSystemTime()
			// bResult = SetSystemTime(&sysDate);
			bRetVal = bRetVal | CheckDatesSYS(sysDate2,tDates[x + 1]);
#endif		
		}
	}
	PrintPassFail(bRetVal);

	bRetVal = FALSE;
////#ifndef _WIN32
//	sOut = "Checking CRunTime: mktime(struct tm *tmTime);\n";
	fputs ("Checking CRunTime: mktime(struct tm *tmTime);\n", f1);
//	f1.WriteString(sOut);
//	sOut = "                   localtime(time_t *timer);\n";
	fputs ("                   localtime(time_t *timer);\n", f1);
////#else	
//	sOut = "Checking CRunTime: SystemTimeToFileTime(SYSTEMTIME *sysTime, FILETIME *fileTime);\n";
	fputs ("Checking CRunTime: SystemTimeToFileTime(SYSTEMTIME *sysTime, FILETIME *fileTime);\n", f1);
//	f1.WriteString(sOut);
//	sOut = "                   SystemTimeToFileTime(FILETIME *fileTime, SYSTEMTIME &sysTime);\n";
	fputs ("                   SystemTimeToFileTime(FILETIME *fileTime, SYSTEMTIME &sysTime);\n", f1);
////#endif
//	f1.WriteString(sOut);
	for (x = 0; tDates[x].nYear != 0;x++) {

////#ifndef _WIN32
		tmTime.tm_year = tDates[x].nYear - 1900;
		tmTime.tm_mon = tDates[x].nMonth - 1;
		tmTime.tm_mday = tDates[x].nDay;
		tmTime.tm_wday = tDates[x].nDayOfWeek - 1;
		tmTime.tm_hour = tDates[x].nHour;
		tmTime.tm_min = tDates[x].nMinute;
		tmTime.tm_sec = tDates[x].nSecond;
		tmTime.tm_isdst = -1;
		tTime = mktime(&tmTime);
		ptmTime2 = localtime(&tTime);		
		bRetVal = bRetVal | CheckDatesDOSTm(ptmTime2,tDates[x]);
////#else
		sysDate.wYear = tDates[x].nYear;
		sysDate.wMonth = tDates[x].nMonth;
		sysDate.wDay = tDates[x].nDay;
		sysDate.wDayOfWeek = tDates[x].nDayOfWeek - 1;
		sysDate.wHour = tDates[x].nHour;
		sysDate.wMinute = tDates[x].nMinute;
		sysDate.wSecond = tDates[x].nSecond;
		sysDate.wMilliseconds = 0;
		bResult = SystemTimeToFileTime(&sysDate, &fileTime);
		bResult = LocalFileTimeToFileTime(&fileTime, &fileTime2);
		bResult = FileTimeToLocalFileTime(&fileTime2, &fileTime);
		bResult = FileTimeToSystemTime(&fileTime, &sysDate2);
		bRetVal = bRetVal | CheckDatesSYS2(sysDate2,tDates[x]);
////#endif		
	}
	PrintPassFail(bRetVal);

	return TRUE;
}



  
