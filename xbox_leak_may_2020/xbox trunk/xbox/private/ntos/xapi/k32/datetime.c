/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    datetime.c

Abstract:

    This module implements Win32 time of day functions

Author:

    Mark Lucovsky (markl) 08-Oct-1990

Revision History:

--*/

#include "basedll.h"
#include <xboxp.h>
#include "xconfig.h"

VOID XapipGetTimeZoneBias(LARGE_INTEGER* bias);

#ifndef XAPILIBP

BOOLEAN
RtlpCutoverTimeToSystemTime(
    PSYSTEMTIME CutoverTime,
    PLARGE_INTEGER SystemTime,
    PLARGE_INTEGER CurrentSystemTime,
    BOOLEAN ThisYear
    )
{
    SYSTEMTIME stCurrentSystemTime;

    //
    // Get the current system time
    //

    FileTimeToSystemTime((PFILETIME) CurrentSystemTime, &stCurrentSystemTime);

    //
    // check for absolute time field. If the year is specified,
    // the the time is an abosulte time
    //

    if ( CutoverTime->wYear ) {

        //
        // Convert this to a time value and make sure it
        // is greater than the current system time
        //

        if ( !SystemTimeToFileTime(CutoverTime,(PFILETIME) SystemTime) ) {
            return FALSE;
            }

        if (SystemTime->QuadPart < CurrentSystemTime->QuadPart) {
            return FALSE;
            }
        return TRUE;
        }
    else {

        TIME_FIELDS WorkingTimeField;
        TIME_FIELDS ScratchTimeField;
        LARGE_INTEGER ScratchTime;
        CSHORT BestWeekdayDate;
        CSHORT WorkingWeekdayNumber;
        CSHORT TargetWeekdayNumber;
        CSHORT TargetYear;
        CSHORT TargetMonth;
        CSHORT TargetWeekday;     // range [0..6] == [Sunday..Saturday]
        BOOLEAN MonthMatches;
        //
        // The time is an day in the month style time
        //
        // the convention is the Day is 1-5 specifying 1st, 2nd... Last
        // day within the month. The day is WeekDay.
        //

        //
        // Compute the target month and year
        //

        TargetWeekdayNumber = CutoverTime->wDay;
        if ( TargetWeekdayNumber > 5 || TargetWeekdayNumber == 0 ) {
            return FALSE;
            }
        TargetWeekday = CutoverTime->wDayOfWeek;
        TargetMonth = CutoverTime->wMonth;
        MonthMatches = FALSE;
        if ( !ThisYear ) {
            if ( TargetMonth < stCurrentSystemTime.wMonth ) {
                TargetYear = stCurrentSystemTime.wYear + 1;
                }
            else if ( TargetMonth > stCurrentSystemTime.wMonth ) {
                TargetYear = stCurrentSystemTime.wYear;
                }
            else {
                TargetYear = stCurrentSystemTime.wYear;
                MonthMatches = TRUE;
                }
            }
        else {
            TargetYear = stCurrentSystemTime.wYear;
            }
try_next_year:
        BestWeekdayDate = 0;

        WorkingTimeField.Year = TargetYear;
        WorkingTimeField.Month = TargetMonth;
        WorkingTimeField.Day = 1;
        WorkingTimeField.Hour = CutoverTime->wHour;
        WorkingTimeField.Minute = CutoverTime->wMinute;
        WorkingTimeField.Second = CutoverTime->wSecond;
        WorkingTimeField.Milliseconds = CutoverTime->wMilliseconds;
        WorkingTimeField.Weekday = 0;

        //
        // Convert to time and then back to time fields so we can determine
        // the weekday of day 1 on the month
        //

        if ( !RtlTimeFieldsToTime(&WorkingTimeField,&ScratchTime) ) {
            return FALSE;
            }
        RtlTimeToTimeFields(&ScratchTime,&ScratchTimeField);

        //
        // Compute bias to target weekday
        //
        if ( ScratchTimeField.Weekday > TargetWeekday ) {
            WorkingTimeField.Day += (7-(ScratchTimeField.Weekday - TargetWeekday));
            }
        else if ( ScratchTimeField.Weekday < TargetWeekday ) {
            WorkingTimeField.Day += (TargetWeekday - ScratchTimeField.Weekday);
            }

        //
        // We are now at the first weekday that matches our target weekday
        //

        BestWeekdayDate = WorkingTimeField.Day;
        WorkingWeekdayNumber = 1;

        //
        // Keep going one week at a time until we either pass the
        // target weekday, or we match exactly
        //

        while ( WorkingWeekdayNumber < TargetWeekdayNumber ) {
            WorkingTimeField.Day += 7;
            if ( !RtlTimeFieldsToTime(&WorkingTimeField,&ScratchTime) ) {
                break;
                }
            RtlTimeToTimeFields(&ScratchTime,&ScratchTimeField);
            WorkingWeekdayNumber++;
            BestWeekdayDate = ScratchTimeField.Day;
            }
        WorkingTimeField.Day = BestWeekdayDate;

        //
        // If the months match, and the date is less than the current
        // date, then be have to go to next year.
        //

        if ( !RtlTimeFieldsToTime(&WorkingTimeField,&ScratchTime) ) {
            return FALSE;
            }
        if ( MonthMatches ) {
            if ( WorkingTimeField.Day < stCurrentSystemTime.wDay ) {
                MonthMatches = FALSE;
                TargetYear++;
                goto try_next_year;
                }
            if ( WorkingTimeField.Day == stCurrentSystemTime.wDay ) {

                if (ScratchTime.QuadPart < CurrentSystemTime->QuadPart) {
                    MonthMatches = FALSE;
                    TargetYear++;
                    goto try_next_year;
                    }
                }
            }
        *SystemTime = ScratchTime;

        return TRUE;
        }
}

__inline VOID
XboxTimeZoneDateToSystemTime(
    const XBOX_TIMEZONE_DATE* tzdate,
    SYSTEMTIME* systime
    )
{
    memset(systime, 0, sizeof(*systime));
    systime->wMonth = tzdate->Month;
    systime->wDay = tzdate->Day;
    systime->wDayOfWeek = tzdate->DayOfWeek;
    systime->wHour = tzdate->Hour;
}

DWORD
WINAPI
XapipQueryTimeZoneInformation(
    OUT PTIME_ZONE_INFORMATION TimeZoneInformation,
    OUT PBOOL pfUseDST
    )
{
    ULONG ulType;
    DWORD dwRet;
    UCHAR buf[EEPROM_TOTAL_MEMORY_SIZE];
    XBOX_USER_SETTINGS* config;

    dwRet = XQueryValue(XC_MAX_OS, &ulType, buf, sizeof(buf), NULL);
    if (dwRet != ERROR_SUCCESS)
        return dwRet;

    config = (XBOX_USER_SETTINGS*) buf;
    TimeZoneInformation->Bias = config->TimeZoneBias;
    
    ASSERT(ARRAYSIZE(TimeZoneInformation->StandardName) >= (XC_TZNAMELEN + 1));
    XboxTimeZoneNameToWstr(config->TimeZoneStdName, TimeZoneInformation->StandardName);
    XboxTimeZoneNameToWstr(config->TimeZoneDltName, TimeZoneInformation->DaylightName);

    XboxTimeZoneDateToSystemTime(&config->TimeZoneStdDate, &TimeZoneInformation->StandardDate);
    XboxTimeZoneDateToSystemTime(&config->TimeZoneDltDate, &TimeZoneInformation->DaylightDate);

    TimeZoneInformation->StandardBias = config->TimeZoneStdBias;
    TimeZoneInformation->DaylightBias = config->TimeZoneDltBias;

    if (pfUseDST)
    {
        *pfUseDST = !(config->MiscFlags & XC_MISC_FLAG_DONT_USE_DST);
    }
    
    return ERROR_SUCCESS;
}

BOOL
WINAPI
XapipUseDaylightSavingTime()
{
    ULONG type, size;
    DWORD flags;

    return XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size) != ERROR_SUCCESS ||
           !(flags & XC_MISC_FLAG_DONT_USE_DST);
}

VOID XapipGetTimeZoneBias(LARGE_INTEGER* bias)
{
    TIME_ZONE_INFORMATION tzinfo;
    LONG tzbias;
    
    switch (GetTimeZoneInformation(&tzinfo)) {
    case TIME_ZONE_ID_UNKNOWN:
        tzbias = tzinfo.Bias;
        break;
    case TIME_ZONE_ID_STANDARD:
        tzbias = tzinfo.Bias + tzinfo.StandardBias;
        break;
    case TIME_ZONE_ID_DAYLIGHT:
        tzbias = tzinfo.Bias + tzinfo.DaylightBias;
        break;
    default:
        tzbias = 0;
        break;
    }

    bias->QuadPart = Int32x32To64(tzbias * 60, 10000000);
}

VOID
WINAPI
GetLocalTime(
    LPSYSTEMTIME lpLocalTime
    )

/*++

Routine Description:

    The current local system date and time can be returned using
    GetLocalTime.

Arguments:

    lpLocalTime - Returns the current system date and time:

        SYSTEMTIME Structure:

        WORD wYear - Returns the current year.

        WORD wMonth - Returns the current month with January equal to 1.

        WORD wDayOfWeek - Returns the current day of the week where
            0=Sunday, 1=Monday...

        WORD wDay - Returns the current day of the month.

        WORD wHour - Returns the current hour.

        WORD wMinute - Returns the current minute within the hour.

        WORD wSecond - Returns the current second within the minute.

        WORD wMilliseconds - Returns the current millisecond within the
            second.

Return Value:

    None.

--*/

{
    LARGE_INTEGER LocalTime;
    LARGE_INTEGER SystemTime;
    LARGE_INTEGER Bias;
    TIME_FIELDS TimeFields;

    KeQuerySystemTime(&SystemTime);

    XapipGetTimeZoneBias(&Bias);
    LocalTime.QuadPart = SystemTime.QuadPart - Bias.QuadPart;

    RtlTimeToTimeFields(&LocalTime,&TimeFields);

    lpLocalTime->wYear         = TimeFields.Year        ;
    lpLocalTime->wMonth        = TimeFields.Month       ;
    lpLocalTime->wDayOfWeek    = TimeFields.Weekday     ;
    lpLocalTime->wDay          = TimeFields.Day         ;
    lpLocalTime->wHour         = TimeFields.Hour        ;
    lpLocalTime->wMinute       = TimeFields.Minute      ;
    lpLocalTime->wSecond       = TimeFields.Second      ;
    lpLocalTime->wMilliseconds = TimeFields.Milliseconds;
}

VOID
WINAPI
GetSystemTime(
    LPSYSTEMTIME lpSystemTime
    )

/*++

Routine Description:

    The current system date and time (UTC based) can be returned using
    GetSystemTime.

Arguments:

    lpSystemTime - Returns the current system date and time:

        SYSTEMTIME Structure:

        WORD wYear - Returns the current year.

        WORD wMonth - Returns the current month with January equal to 1.

        WORD wDayOfWeek - Returns the current day of the week where
            0=Sunday, 1=Monday...

        WORD wDay - Returns the current day of the month.

        WORD wHour - Returns the current hour.

        WORD wMinute - Returns the current minute within the hour.

        WORD wSecond - Returns the current second within the minute.

        WORD wMilliseconds - Returns the current millisecond within the
            second.

Return Value:

    None.

--*/

{
    LARGE_INTEGER SystemTime;
    TIME_FIELDS TimeFields;

    KeQuerySystemTime(&SystemTime);

    RtlTimeToTimeFields(&SystemTime,&TimeFields);

    lpSystemTime->wYear         = TimeFields.Year        ;
    lpSystemTime->wMonth        = TimeFields.Month       ;
    lpSystemTime->wDayOfWeek    = TimeFields.Weekday     ;
    lpSystemTime->wDay          = TimeFields.Day         ;
    lpSystemTime->wHour         = TimeFields.Hour        ;
    lpSystemTime->wMinute       = TimeFields.Minute      ;
    lpSystemTime->wSecond       = TimeFields.Second      ;
    lpSystemTime->wMilliseconds = TimeFields.Milliseconds;
}


DWORD
GetTickCount(
    VOID
    )

/*++

Routine Description:

    Win32 systems implement a free-running millisecond counter.  The
    value of this counter can be read using GetTickCount.

Arguments:

    None.

Return Value:

    This function returns the number of milliseconds that have elapsed
    since the system was started. If the system has been running for
    a long time, it is possible that the count will repeat. The value of
    the counter is accurate within 55 milliseconds.

--*/

{
    return (DWORD)NtGetTickCount();
}

BOOL
APIENTRY
FileTimeToSystemTime(
    CONST FILETIME *lpFileTime,
    LPSYSTEMTIME lpSystemTime
    )

/*++

Routine Description:

    This functions converts a 64-bit file time value to a time in system
    time format.

Arguments:

    lpFileTime - Supplies the 64-bit file time to convert to the system
        date and time format.

    lpSystemTime - Returns the converted value of the 64-bit file time.

Return Value:

    TRUE - The 64-bit file time was successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    LARGE_INTEGER FileTime;
    TIME_FIELDS TimeFields;

    FileTime.LowPart = lpFileTime->dwLowDateTime;
    FileTime.HighPart = lpFileTime->dwHighDateTime;

#if DBG
    if (FileTime.QuadPart < 0)
    {
        RIP("FileTimeToSystemTime() invalid parameter (lpFileTime)");
    }
#endif // DBG

    RtlTimeToTimeFields(&FileTime, &TimeFields);

    lpSystemTime->wYear         = TimeFields.Year        ;
    lpSystemTime->wMonth        = TimeFields.Month       ;
    lpSystemTime->wDay          = TimeFields.Day         ;
    lpSystemTime->wDayOfWeek    = TimeFields.Weekday     ;
    lpSystemTime->wHour         = TimeFields.Hour        ;
    lpSystemTime->wMinute       = TimeFields.Minute      ;
    lpSystemTime->wSecond       = TimeFields.Second      ;
    lpSystemTime->wMilliseconds = TimeFields.Milliseconds;

    return TRUE;
}


BOOL
APIENTRY
SystemTimeToFileTime(
    CONST SYSTEMTIME *lpSystemTime,
    LPFILETIME lpFileTime
    )

/*++

Routine Description:

    This functions converts a system time value into a 64-bit file time.

Arguments:

    lpSystemTime - Supplies the time that is to be converted into
        the 64-bit file time format.  The wDayOfWeek field is ignored.

    lpFileTime - Returns the 64-bit file time representation of
        lpSystemTime.

Return Value:

    TRUE - The time was successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    TIME_FIELDS TimeFields;
    LARGE_INTEGER FileTime;

    TimeFields.Year         = lpSystemTime->wYear        ;
    TimeFields.Month        = lpSystemTime->wMonth       ;
    TimeFields.Day          = lpSystemTime->wDay         ;
    TimeFields.Hour         = lpSystemTime->wHour        ;
    TimeFields.Minute       = lpSystemTime->wMinute      ;
    TimeFields.Second       = lpSystemTime->wSecond      ;
    TimeFields.Milliseconds = lpSystemTime->wMilliseconds;

    if ( !RtlTimeFieldsToTime(&TimeFields,&FileTime)) {
        XapiSetLastNTError(STATUS_INVALID_PARAMETER);
        return FALSE;
        }
    else {
        lpFileTime->dwLowDateTime = FileTime.LowPart;
        lpFileTime->dwHighDateTime = FileTime.HighPart;
        return TRUE;
        }
}

BOOL
WINAPI
FileTimeToLocalFileTime(
    CONST FILETIME *lpFileTime,
    LPFILETIME lpLocalFileTime
    )

/*++

Routine Description:

    This functions converts a UTC based file time to a local file time.

Arguments:

    lpFileTime - Supplies the UTC based file time that is to be
        converted into a local file time

    lpLocalFileTime - Returns the 64-bit local file time representation of
        lpFileTime.

Return Value:

    TRUE - The time was successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    LARGE_INTEGER FileTime;
    LARGE_INTEGER LocalFileTime;
    LARGE_INTEGER Bias;

    XapipGetTimeZoneBias(&Bias);

    FileTime.LowPart = lpFileTime->dwLowDateTime;
    FileTime.HighPart = lpFileTime->dwHighDateTime;

    LocalFileTime.QuadPart = FileTime.QuadPart - Bias.QuadPart;

    lpLocalFileTime->dwLowDateTime = LocalFileTime.LowPart;
    lpLocalFileTime->dwHighDateTime = LocalFileTime.HighPart;

    return TRUE;
}

BOOL
WINAPI
LocalFileTimeToFileTime(
    CONST FILETIME *lpLocalFileTime,
    LPFILETIME lpFileTime
    )

/*++

Routine Description:

    This functions converts a local file time to a UTC based file time.

Arguments:

    lpLocalFileTime - Supplies the local file time that is to be
        converted into a UTC based file time

    lpFileTime - Returns the 64-bit UTC based file time representation of
        lpLocalFileTime.

Return Value:

    TRUE - The time was successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    LARGE_INTEGER FileTime;
    LARGE_INTEGER LocalFileTime;
    LARGE_INTEGER Bias;

    XapipGetTimeZoneBias(&Bias);

    LocalFileTime.LowPart = lpLocalFileTime->dwLowDateTime;
    LocalFileTime.HighPart = lpLocalFileTime->dwHighDateTime;

    FileTime.QuadPart = LocalFileTime.QuadPart + Bias.QuadPart;

    lpFileTime->dwLowDateTime = FileTime.LowPart;
    lpFileTime->dwHighDateTime = FileTime.HighPart;

    return TRUE;
}

LONG
APIENTRY
CompareFileTime(
    CONST FILETIME *lpFileTime1,
    CONST FILETIME *lpFileTime2
    )

/*++

Routine Description:

    This function compares two 64-bit file times.

Arguments:

    lpFileTime1 - pointer to a 64-bit file time.

    lpFileTime2 - pointer to a 64-bit file time.

Return Value:

    -1 - *lpFileTime1 <  *lpFileTime2

     0 - *lpFileTime1 == *lpFileTime2

    +1 - *lpFileTime1 >  *lpFileTime2

--*/

{
    ULARGE_INTEGER FileTime1;
    ULARGE_INTEGER FileTime2;

    FileTime1.LowPart = lpFileTime1->dwLowDateTime;
    FileTime1.HighPart = lpFileTime1->dwHighDateTime;
    FileTime2.LowPart = lpFileTime2->dwLowDateTime;
    FileTime2.HighPart = lpFileTime2->dwHighDateTime;
    if (FileTime1.QuadPart < FileTime2.QuadPart) {
        return( -1 );
        }
    else
    if (FileTime1.QuadPart > FileTime2.QuadPart) {
        return( 1 );
        }
    else {
        return( 0 );
        }
}

DWORD
WINAPI
GetTimeZoneInformation(
    LPTIME_ZONE_INFORMATION lpTimeZoneInformation
    )

/*++

Routine Description:

    This function allows an application to get the current timezone
    parameters These parameters control the Universal time to Local time
    translations.

    All UTC time to Local time translations are based on the following
    formula:

        UTC = LocalTime + Bias

    The return value of this function is the systems best guess of
    the current time zone parameters. This is one of:

        - Unknown

        - Standard Time

        - Daylight Savings Time

    If SetTimeZoneInformation was called without the transition date
    information, Unknown is returned, but the currect bias is used for
    local time translation.  Otherwise, the system will correctly pick
    either daylight savings time or standard time.

    The information returned by this API is identical to the information
    stored in the last successful call to SetTimeZoneInformation.  The
    exception is the Bias field returns the current Bias value in

Arguments:

    lpTimeZoneInformation - Supplies the address of the time zone
        information structure.

Return Value:

    TIME_ZONE_ID_UNKNOWN - The system can not determine the current
        timezone.  This is usually due to a previous call to
        SetTimeZoneInformation where only the Bias was supplied and no
        transition dates were supplied.

    TIME_ZONE_ID_STANDARD - The system is operating in the range covered
        by StandardDate.

    TIME_ZONE_ID_DAYLIGHT - The system is operating in the range covered
        by DaylightDate.

    0xffffffff - The operation failed.  Extended error status is
        available using GetLastError.

--*/
{
    DWORD dwStatus;
    DWORD dwRet = TIME_ZONE_ID_UNKNOWN;
    BOOL fUseDST;
    LARGE_INTEGER StandardTime, DaylightTime, CurrentUniversalTime;
    LARGE_INTEGER TimeZoneBias;
    LARGE_INTEGER NewTimeZoneBias;
    LARGE_INTEGER LocalCustomBias;
    LARGE_INTEGER UtcStandardTime;
    LARGE_INTEGER UtcDaylightTime;
    //
    // get the timezone data from the system
    //

    dwStatus = XapipQueryTimeZoneInformation(lpTimeZoneInformation, &fUseDST);
    if (ERROR_SUCCESS != dwStatus)
    {
        SetLastError(dwStatus);
        return 0xffffffff;
    }

    if (!fUseDST)
    {
        //
        // If we're not using daylight saving time, clear all fields related to
        // daylight savings time
        //
        
        lpTimeZoneInformation->StandardBias = 0;
        RtlZeroMemory(&(lpTimeZoneInformation->StandardDate),
                      sizeof(lpTimeZoneInformation->StandardDate));
        
        lpTimeZoneInformation->DaylightBias = 0;
        RtlZeroMemory(&(lpTimeZoneInformation->DaylightDate),
                      sizeof(lpTimeZoneInformation->DaylightDate));
        RtlZeroMemory(lpTimeZoneInformation->DaylightName,
                      sizeof(lpTimeZoneInformation->DaylightName));
    }

    KeQuerySystemTime(&CurrentUniversalTime);

    //
    // Check to see if we have timezone cutover information and if so,
    // compute the cutover dates and compute what our current bias is
    //

    if (lpTimeZoneInformation->StandardDate.wMonth &&
        lpTimeZoneInformation->DaylightDate.wMonth &&
        RtlpCutoverTimeToSystemTime(
                &(lpTimeZoneInformation->StandardDate),
                &StandardTime,
                &CurrentUniversalTime,
                TRUE) &&
        RtlpCutoverTimeToSystemTime(
                &(lpTimeZoneInformation->DaylightDate),
                &DaylightTime,
                &CurrentUniversalTime,
                TRUE))
    {
        //
        // Get the new timezone bias
        //

        NewTimeZoneBias.QuadPart = Int32x32To64(lpTimeZoneInformation->Bias*60, 10000000);
    
        //
        // Convert standard time and daylight time to utc
        //

        LocalCustomBias.QuadPart = Int32x32To64(lpTimeZoneInformation->StandardBias*60, 10000000);
        TimeZoneBias.QuadPart = NewTimeZoneBias.QuadPart + LocalCustomBias.QuadPart;
        UtcDaylightTime.QuadPart = DaylightTime.QuadPart + TimeZoneBias.QuadPart;

        LocalCustomBias.QuadPart = Int32x32To64(lpTimeZoneInformation->DaylightBias*60, 10000000);
        TimeZoneBias.QuadPart = NewTimeZoneBias.QuadPart + LocalCustomBias.QuadPart;
        UtcStandardTime.QuadPart = StandardTime.QuadPart + TimeZoneBias.QuadPart;
        
        //
        // If daylight < standard, then time >= daylight and
        // less than standard is daylight
        //

        if (UtcDaylightTime.QuadPart < UtcStandardTime.QuadPart)
        {
            //
            // If today is >= DaylightTime and < StandardTime, then
            // We are in daylight savings time
            //

            if ( (CurrentUniversalTime.QuadPart >= UtcDaylightTime.QuadPart) &&
                 (CurrentUniversalTime.QuadPart < UtcStandardTime.QuadPart) )
            {
                dwRet = TIME_ZONE_ID_DAYLIGHT;
            }
            else
            {
                dwRet = TIME_ZONE_ID_STANDARD;
            }
        }
        else
        {
            //
            // If today is >= StandardTime and < DaylightTime, then
            // We are in standard time
            //

            if ( (CurrentUniversalTime.QuadPart >= UtcStandardTime.QuadPart) &&
                 (CurrentUniversalTime.QuadPart < UtcDaylightTime.QuadPart) )
            {
                dwRet = TIME_ZONE_ID_STANDARD;
            }
            else
            {
                dwRet = TIME_ZONE_ID_DAYLIGHT;
            }
        }
    }

    return dwRet;
}

#else  // ! XAPILIBP

__inline VOID
SystemTimeToXboxTimeZoneDate(
    const SYSTEMTIME* systime,
    XBOX_TIMEZONE_DATE* tzdate
    )
{
    ASSERT(systime->wYear == 0);
    tzdate->Month = (BYTE) systime->wMonth;
    tzdate->Day = (BYTE) systime->wDay;
    tzdate->DayOfWeek = (BYTE) systime->wDayOfWeek;
    tzdate->Hour = (BYTE) systime->wHour;
}

DWORD
WINAPI
XapipSetTimeZoneInformation(
    IN PTIME_ZONE_INFORMATION TimeZoneInformation
    )
{
    ULONG ulType, ulSize;
    DWORD dwRet;
    CHAR buf[EEPROM_TOTAL_MEMORY_SIZE];
    XBOX_USER_SETTINGS* config;

    dwRet = XQueryValue(XC_MAX_OS, &ulType, buf, sizeof(buf), &ulSize);
    if (dwRet != ERROR_SUCCESS)
        return dwRet;

    config = (XBOX_USER_SETTINGS*) buf;
    config->TimeZoneBias = TimeZoneInformation->Bias;

    WstrToXboxTimeZoneName(TimeZoneInformation->StandardName, config->TimeZoneStdName);
    WstrToXboxTimeZoneName(TimeZoneInformation->DaylightName, config->TimeZoneDltName);

    SystemTimeToXboxTimeZoneDate(&TimeZoneInformation->StandardDate, &config->TimeZoneStdDate);
    SystemTimeToXboxTimeZoneDate(&TimeZoneInformation->DaylightDate, &config->TimeZoneDltDate);

    config->TimeZoneStdBias = TimeZoneInformation->StandardBias;
    config->TimeZoneDltBias = TimeZoneInformation->DaylightBias;

    return XSetValue(XC_MAX_OS, REG_BINARY, buf, ulSize);
}


BOOL
WINAPI
XapiSetSystemTime(
    CONST SYSTEMTIME *lpSystemTime
    )

/*++

Routine Description:

    The current UTC based system date and time can be set using
    SetSystemTime.

Arguments:

    lpSystemTime - Supplies the date and time to set. The wDayOfWeek field
        is ignored.

Return Value:

    TRUE - The current system date and time was set.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    LARGE_INTEGER SystemTime;
    TIME_FIELDS TimeFields;
    BOOLEAN ReturnValue;
    PVOID State;
    NTSTATUS Status;

    ReturnValue = TRUE;

    TimeFields.Year         = lpSystemTime->wYear        ;
    TimeFields.Month        = lpSystemTime->wMonth       ;
    TimeFields.Day          = lpSystemTime->wDay         ;
    TimeFields.Hour         = lpSystemTime->wHour        ;
    TimeFields.Minute       = lpSystemTime->wMinute      ;
    TimeFields.Second       = lpSystemTime->wSecond      ;
    TimeFields.Milliseconds = lpSystemTime->wMilliseconds;

    if ( !RtlTimeFieldsToTime(&TimeFields,&SystemTime) ) {
        Status = STATUS_INVALID_PARAMETER;
        ReturnValue = FALSE;
        }
    else {
        NtSetSystemTime(&SystemTime,NULL);
        }

    if ( !ReturnValue ) {
        XapiSetLastNTError(Status);
        }

    return ReturnValue;
}

BOOL
WINAPI
XapiSetLocalTime(
    CONST SYSTEMTIME *lpLocalTime
    )

/*++

Routine Description:

    The current local system date and time can be set using
    SetLocalTime.

Arguments:

    lpSystemTime - Supplies the date and time to set. The wDayOfWeek field
        is ignored.

Return Value:

    TRUE - The current system date and time was set.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    LARGE_INTEGER SystemTime;
    LARGE_INTEGER LocalTime;
    TIME_FIELDS TimeFields;
    BOOLEAN ReturnValue;
    PVOID State;
    NTSTATUS Status;
    LARGE_INTEGER Bias;

    XapipGetTimeZoneBias(&Bias);
    ReturnValue = TRUE;

    TimeFields.Year         = lpLocalTime->wYear        ;
    TimeFields.Month        = lpLocalTime->wMonth       ;
    TimeFields.Day          = lpLocalTime->wDay         ;
    TimeFields.Hour         = lpLocalTime->wHour        ;
    TimeFields.Minute       = lpLocalTime->wMinute      ;
    TimeFields.Second       = lpLocalTime->wSecond      ;
    TimeFields.Milliseconds = lpLocalTime->wMilliseconds;

    if ( !RtlTimeFieldsToTime(&TimeFields,&LocalTime) ) {
        Status = STATUS_INVALID_PARAMETER;
        ReturnValue = FALSE;
        }
    else {

        SystemTime.QuadPart = LocalTime.QuadPart + Bias.QuadPart;
        NtSetSystemTime(&SystemTime,NULL);
        }

    if ( !ReturnValue ) {
        XapiSetLastNTError(Status);
        }

    return ReturnValue;
}

#endif // ! XAPILIBP


// xbox doesn't allow you to set the time (at least, for now)
#if 0
BOOL
WINAPI
SetTimeZoneInformation(
    CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation
    )

/*++

Routine Description:

    This function allows an application to set timezone parameters into
    their system.  These parameters control the Universal time to Local
    time translations.

    All UTC time to Local time translations are based on the following
    formula:

        UTC = LocalTime + Bias

    This API allows the caller to program the current time zone bias,
    and optionally set up the system to automatically sense daylight
    savings time and standard time transitions.

    The timezone bias information is controlled by the
    TIME_ZONE_INFORMATION structure.

    Bias - Supplies the current bias in minutes for local time
        translation on this machine where LocalTime + Bias = UTC.  This
        is a required filed of this structure.

    StandardName - Supplies an optional abbreviation string associated
        with standard time on this system.  This string is uniterpreted
        and is supplied and used only by callers of this API and of
        GetTimeZoneInformation.

    StandardDate - Supplies an optional date and time (UTC) that
        describes the transition into standard time.  A value of 0 in
        the wMonth field tells the system that StandardDate is not
        specified.  If this field is specified, then DaylightDate must
        also be specified.  Additionally, local time translations done
        during the StandardTime range will be done relative to the
        supplied StandardBias value (added to Bias).

        This field supports two date formats. Absolute form specifies and
        exact date and time when standard time begins. In this form, the
        wYear, wMonth, wDay, wHour, wMinute, wSecond, and wMilliseconds
        of the SYSTEMTIME structure are used to specify an exact date.

        Day-in-month time is specified by setting wYear to 0, setting
        wDayOfWeek to an appropriate weekday, and using wDay in the
        range of 1-5 to select the correct day in the month.  Using this
        notation, the first sunday in april may be specified as can be
        the last thursday in october (5 is equal to "the last").

    StandardBias - Supplies an optional bias value to be used during
        local time translations that occur during Standard Time. This
        field is ignored if StandardDate is not supplied.
         This bias value
        is added to the Bias field to form the Bias used during standard
        time. In most time zones, the value of this field is zero.

    DaylightName - Supplies an optional abbreviation string associated
        with daylight savings time on this system.  This string is
        uniterpreted and is supplied and used only by callers of this
        API and of GetTimeZoneInformation.

    DaylightDate - Supplies an optional date and time (UTC) that
        describes the transition into daylight savings time.  A value of
        0 in the wMonth field tells the system that DaylightDate is not
        specified.  If this field is specified, then StandardDate must
        also be specified.  Additionally, local time translations done
        during the DaylightTime range will be done relative to the
        supplied DaylightBias value (added to Bias). The same dat formats
        supported by StandardDate are supported ib DaylightDate.

    DaylightBias - Supplies an optional bias value to be used during
        local time translations that occur during Daylight Savings Time.
        This field is ignored if DaylightDate is not supplied.  This
        bias value is added to the Bias field to form the Bias used
        during daylight time.  In most time zones, the value of this
        field is -60.

Arguments:

    lpTimeZoneInformation - Supplies the address of the time zone
        information structure.

Return Value:

    TRUE - The operation was successful.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    RTL_TIME_ZONE_INFORMATION tzi;
    NTSTATUS Status;

    tzi.Bias            = lpTimeZoneInformation->Bias;
    tzi.StandardBias    = lpTimeZoneInformation->StandardBias;
    tzi.DaylightBias    = lpTimeZoneInformation->DaylightBias;

    RtlMoveMemory(&tzi.StandardName,&lpTimeZoneInformation->StandardName,sizeof(tzi.StandardName));
    RtlMoveMemory(&tzi.DaylightName,&lpTimeZoneInformation->DaylightName,sizeof(tzi.DaylightName));

    tzi.StandardStart.Year         = lpTimeZoneInformation->StandardDate.wYear        ;
    tzi.StandardStart.Month        = lpTimeZoneInformation->StandardDate.wMonth       ;
    tzi.StandardStart.Weekday      = lpTimeZoneInformation->StandardDate.wDayOfWeek   ;
    tzi.StandardStart.Day          = lpTimeZoneInformation->StandardDate.wDay         ;
    tzi.StandardStart.Hour         = lpTimeZoneInformation->StandardDate.wHour        ;
    tzi.StandardStart.Minute       = lpTimeZoneInformation->StandardDate.wMinute      ;
    tzi.StandardStart.Second       = lpTimeZoneInformation->StandardDate.wSecond      ;
    tzi.StandardStart.Milliseconds = lpTimeZoneInformation->StandardDate.wMilliseconds;

    tzi.DaylightStart.Year         = lpTimeZoneInformation->DaylightDate.wYear        ;
    tzi.DaylightStart.Month        = lpTimeZoneInformation->DaylightDate.wMonth       ;
    tzi.DaylightStart.Weekday      = lpTimeZoneInformation->DaylightDate.wDayOfWeek   ;
    tzi.DaylightStart.Day          = lpTimeZoneInformation->DaylightDate.wDay         ;
    tzi.DaylightStart.Hour         = lpTimeZoneInformation->DaylightDate.wHour        ;
    tzi.DaylightStart.Minute       = lpTimeZoneInformation->DaylightDate.wMinute      ;
    tzi.DaylightStart.Second       = lpTimeZoneInformation->DaylightDate.wSecond      ;
    tzi.DaylightStart.Milliseconds = lpTimeZoneInformation->DaylightDate.wMilliseconds;

    Status = RtlSetTimeZoneInformation( &tzi );
    if (!NT_SUCCESS( Status )) {
        XapiSetLastNTError(Status);
        return FALSE;
        }
    //
    // Refresh the system's concept of time
    //

    NtSetSystemTime(NULL,NULL);

    return TRUE;
}
#endif // 0

// xbox does not support these:
#if 0

BOOL
WINAPI
GetSystemTimeAdjustment(
    PDWORD lpTimeAdjustment,
    PDWORD lpTimeIncrement,
    PBOOL  lpTimeAdjustmentDisabled
    )

/*++

Routine Description:

    This function is used to support algorithms that want to synchronize
    the time of day (reported via GetSystemTime and GetLocalTime) with
    another time source using a programmed clock adjustment over a
    period of time.

    To facilitate this, the system computes the time of day by adding a
    value to a time of day counter at a periodic interval.  This API
    allows the caller to obtain the periodic interval (clock interrupt
    rate), and the amount added to the time of day with each interrupt.

    A boolean value is also returned which indicates whether or not this
    time adjustment algorithm is even being used.  A value of TRUE
    indicates that adjustment is not being used.  If this is the case,
    the system may attempt to keep the time of day clock in sync using
    its own internal mechanisms.  This may cause time of day to
    periodicly "jump" to the "correct time".


Arguments:

    lpTimeAdjustment - Returns the number of 100ns units added to the
        time of day counter at each clock interrupt.

    lpTimeIncrement - Returns the clock interrupt rate in 100ns units.

    lpTimeAdjustmentDisabled - Returns an indicator which specifies
        whether or not time adjustment is inabled.  A value of TRUE
        indicates that periodic adjustment is disabled
        (*lpTimeAdjustment == *lpTimeIncrement), AND that the system is
        free to serialize time of day using any mechanism it wants.
        This may cause periodic time jumps as the system serializes time
        of day to the "correct time".  A value of false indicates that
        programmed time adjustment is being used to serialize the time
        of day, and that the system will not interfere with this scheme
        and will not attempt to synchronize time of day on its own.

Return Value:

    TRUE - The operation was successful.

    FALSE - The operation failed.  Use GetLastError to obtain detailed
        error information.

--*/
{
    NTSTATUS Status;
    SYSTEM_QUERY_TIME_ADJUST_INFORMATION TimeAdjust;
    BOOL b;
    Status = NtQuerySystemInformation(
                SystemTimeAdjustmentInformation,
                &TimeAdjust,
                sizeof(TimeAdjust),
                NULL
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        b = FALSE;
        }
    else {
        *lpTimeAdjustment = TimeAdjust.TimeAdjustment;
        *lpTimeIncrement = TimeAdjust.TimeIncrement;
        *lpTimeAdjustmentDisabled = TimeAdjust.Enable;
        b = TRUE;
        }

    return b;
}

BOOL
WINAPI
SetSystemTimeAdjustment(
    DWORD dwTimeAdjustment,
    BOOL  bTimeAdjustmentDisabled
    )

/*++

Routine Description:

    This function is used to tell the system the parameters it should
    use to periodicaly synchronize time of day with some other source.

    This API supports two modes of operation.

    In the first mode, bTimeAdjustmentDisabled is set to FALSE.  At each
    clock interrupt, the value of dwTimeAdjustment is added to the time
    of day.  The clock interrupt rate may be obtained using
    GetSystemTimeAdjustment, and looking at the returned value of
    lpTimeIncrement.

    In the second mode, bTimeAdjustmentDisabled is set to TRUE.  At each
    clock interrupt, the clock interrupt rate is added to the time of
    day.  The system may also periodically refresh the time of day using
    other internal algorithms.  These may produce "jumps" in time.

    The application must have system-time privilege (the
    SE_SYSTEMTIME_NAME privilege) for this function to succeed.  This
    privilege is disabled by default.  Use the AdjustTokenPrivileges
    function to enable the privilege and again to disable it after the
    time adjustment has been set.

Arguments:

    dwTimeAdjustment - Supplies the value (in 100ns units) that is to be
        added to the time of day at each clock interrupt.

    bTimeAdjustmentDisabled - Supplies a flag which specifies the time
        adjustment mode that the system is to use.  A value of TRUE
        indicates the the system should synchronize time of day using
        its own internal mechanisms.  When this is the case, the value
        of dwTimeAdjustment is ignored.  A value of FALSE indicates that
        the application is in control, and that the value specified by
        dwTimeAdjustment is to be added to the time of day at each clock
        interrupt.

Return Value:

    TRUE - The operation was successful.

    FALSE - The operation failed.  Use GetLastError to obtain detailed
        error information.

--*/

{
    NTSTATUS Status;
    SYSTEM_SET_TIME_ADJUST_INFORMATION TimeAdjust;
    BOOL b;

    b = TRUE;
    TimeAdjust.TimeAdjustment = dwTimeAdjustment;
    TimeAdjust.Enable = (BOOLEAN)bTimeAdjustmentDisabled;
    Status = NtSetSystemInformation(
                SystemTimeAdjustmentInformation,
                &TimeAdjust,
                sizeof(TimeAdjust)
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        b = FALSE;
        }

    return b;
}

BOOL
WINAPI
SystemTimeToTzSpecificLocalTime(
    LPTIME_ZONE_INFORMATION lpTimeZoneInformation,
    LPSYSTEMTIME lpUniversalTime,
    LPSYSTEMTIME lpLocalTime
    )
{

    TIME_ZONE_INFORMATION TziData;
    LPTIME_ZONE_INFORMATION Tzi;
    RTL_TIME_ZONE_INFORMATION tzi;
    LARGE_INTEGER TimeZoneBias;
    LARGE_INTEGER NewTimeZoneBias;
    LARGE_INTEGER LocalCustomBias;
    LARGE_INTEGER StandardTime;
    LARGE_INTEGER DaylightTime;
    LARGE_INTEGER UtcStandardTime;
    LARGE_INTEGER UtcDaylightTime;
    LARGE_INTEGER CurrentUniversalTime;
    LARGE_INTEGER ComputedLocalTime;
    ULONG CurrentTimeZoneId = 0xffffffff;

    //
    // Get the timezone information into a useful format
    //
    if ( !ARGUMENT_PRESENT(lpTimeZoneInformation) ) {

        //
        // Convert universal time to local time using current timezone info
        //
        if (GetTimeZoneInformation(&TziData) == TIME_ZONE_ID_INVALID) {
            return FALSE;
            }
        Tzi = &TziData;
        }
    else {
        Tzi = lpTimeZoneInformation;
        }

    tzi.Bias            = Tzi->Bias;
    tzi.StandardBias    = Tzi->StandardBias;
    tzi.DaylightBias    = Tzi->DaylightBias;

    RtlMoveMemory(&tzi.StandardName,&Tzi->StandardName,sizeof(tzi.StandardName));
    RtlMoveMemory(&tzi.DaylightName,&Tzi->DaylightName,sizeof(tzi.DaylightName));

    tzi.StandardStart.Year         = Tzi->StandardDate.wYear        ;
    tzi.StandardStart.Month        = Tzi->StandardDate.wMonth       ;
    tzi.StandardStart.Weekday      = Tzi->StandardDate.wDayOfWeek   ;
    tzi.StandardStart.Day          = Tzi->StandardDate.wDay         ;
    tzi.StandardStart.Hour         = Tzi->StandardDate.wHour        ;
    tzi.StandardStart.Minute       = Tzi->StandardDate.wMinute      ;
    tzi.StandardStart.Second       = Tzi->StandardDate.wSecond      ;
    tzi.StandardStart.Milliseconds = Tzi->StandardDate.wMilliseconds;

    tzi.DaylightStart.Year         = Tzi->DaylightDate.wYear        ;
    tzi.DaylightStart.Month        = Tzi->DaylightDate.wMonth       ;
    tzi.DaylightStart.Weekday      = Tzi->DaylightDate.wDayOfWeek   ;
    tzi.DaylightStart.Day          = Tzi->DaylightDate.wDay         ;
    tzi.DaylightStart.Hour         = Tzi->DaylightDate.wHour        ;
    tzi.DaylightStart.Minute       = Tzi->DaylightDate.wMinute      ;
    tzi.DaylightStart.Second       = Tzi->DaylightDate.wSecond      ;
    tzi.DaylightStart.Milliseconds = Tzi->DaylightDate.wMilliseconds;

    //
    // convert the input universal time to NT style time
    //
    if ( !SystemTimeToFileTime(lpUniversalTime,(LPFILETIME)&CurrentUniversalTime) ) {
        return FALSE;
        }

    //
    // Get the new timezone bias
    //

    NewTimeZoneBias.QuadPart = Int32x32To64(tzi.Bias*60, 10000000);

    //
    // Now see if we have stored cutover times
    //

    if ( tzi.StandardStart.Month && tzi.DaylightStart.Month ) {

        //
        // We have timezone cutover information. Compute the
        // cutover dates and compute what our current bias
        // is
        //

        if ( !RtlpCutoverTimeToSystemTime(
                &tzi.StandardStart,
                &StandardTime,
                &CurrentUniversalTime,
                TRUE
                ) ) {
            XapiSetLastNTError(STATUS_INVALID_PARAMETER);
            return FALSE;
            }

        if ( !RtlpCutoverTimeToSystemTime(
                &tzi.DaylightStart,
                &DaylightTime,
                &CurrentUniversalTime,
                TRUE
                ) ) {
            XapiSetLastNTError(STATUS_INVALID_PARAMETER);
            return FALSE;
            }

        //
        // Convert standard time and daylight time to utc
        //

        LocalCustomBias.QuadPart = Int32x32To64(tzi.StandardBias*60, 10000000);
        TimeZoneBias.QuadPart = NewTimeZoneBias.QuadPart + LocalCustomBias.QuadPart;
        UtcDaylightTime.QuadPart = DaylightTime.QuadPart + TimeZoneBias.QuadPart;

        LocalCustomBias.QuadPart = Int32x32To64(tzi.DaylightBias*60, 10000000);
        TimeZoneBias.QuadPart = NewTimeZoneBias.QuadPart + LocalCustomBias.QuadPart;
        UtcStandardTime.QuadPart = StandardTime.QuadPart + TimeZoneBias.QuadPart;

        //
        // If daylight < standard, then time >= daylight and
        // less than standard is daylight
        //

        if ( UtcDaylightTime.QuadPart < UtcStandardTime.QuadPart ) {

            //
            // If today is >= DaylightTime and < StandardTime, then
            // We are in daylight savings time
            //

            if ( (CurrentUniversalTime.QuadPart >= UtcDaylightTime.QuadPart) &&
                 (CurrentUniversalTime.QuadPart < UtcStandardTime.QuadPart) ) {

                CurrentTimeZoneId = TIME_ZONE_ID_DAYLIGHT;
                }
            else {
                CurrentTimeZoneId = TIME_ZONE_ID_STANDARD;
                }
            }
        else {

            //
            // If today is >= StandardTime and < DaylightTime, then
            // We are in standard time
            //

            if ( (CurrentUniversalTime.QuadPart >= UtcStandardTime.QuadPart ) &&
                 (CurrentUniversalTime.QuadPart < UtcDaylightTime.QuadPart ) ) {

                CurrentTimeZoneId = TIME_ZONE_ID_STANDARD;
                }
            else {
                CurrentTimeZoneId = TIME_ZONE_ID_DAYLIGHT;
                }
            }

        //
        // At this point, we know our current timezone and the
        // Universal time of the next cutover.
        //

        LocalCustomBias.QuadPart = Int32x32To64(
                            CurrentTimeZoneId == TIME_ZONE_ID_DAYLIGHT ?
                                tzi.DaylightBias*60 :
                                tzi.StandardBias*60,                // Bias in seconds
                            10000000
                            );

        TimeZoneBias.QuadPart = NewTimeZoneBias.QuadPart + LocalCustomBias.QuadPart;

        }
    else {
        TimeZoneBias = NewTimeZoneBias;
        }

    ComputedLocalTime.QuadPart = CurrentUniversalTime.QuadPart - TimeZoneBias.QuadPart;

    if ( !FileTimeToSystemTime((LPFILETIME)&ComputedLocalTime,lpLocalTime) ) {
        return FALSE;
        }

    return TRUE;
}

#endif // 0

