/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    httptime.h

Abstract:

    Helper class for parsing HTTP date/time string

Revision History:

    08/17/2000 davidx
        Created it.

--*/

#ifndef _HTTPTIME_H
#define _HTTPTIME_H

//
// Helper class to parse HTTP date/time string
//
class HttpDateTime {

public:

    // Parse a HTTP date/time string
    static BOOL Parse(const CHAR* s, SYSTEMTIME* systime) {
        HttpDateTime dateTime(s);
        if (dateTime.IsInvalid()) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        systime->wYear = dateTime.year;
        systime->wMonth = dateTime.month;
        systime->wDay = dateTime.day;
        systime->wDayOfWeek = dateTime.wkday;
        systime->wHour = dateTime.hour;
        systime->wMinute = dateTime.minute;
        systime->wSecond = dateTime.second;
        systime->wMilliseconds = 0;
        return TRUE;
    }

private:

    // Parse a HTTP date/time string
    HttpDateTime(const CHAR* s) {
        // Mark the object as invalid
        MarkInvalid();

        cp = s;
        while (*cp && *cp != COMMA && *cp != SPACE) cp++;
        if (*cp == 0) return;

        INT wkdaylen = cp - s;
        INT fmt;
        
        // Determine the date/time string format
        // from the content of the day-of-week field

        if (*cp == SPACE) {
            if (wkdaylen != 3) return;
            fmt = DTFMT_ANSIC;
        } else {
            cp++;
            fmt = (wkdaylen == 3) ? DTFMT_RFC822 : DTFMT_RFC850;
        }
        if (*cp++ != SPACE || !ParseWkDay(s, wkdaylen)) return;

        BOOL ok = FALSE;
        switch (fmt) {
        case DTFMT_RFC822:
            ok = ParseInt(&day, 2, SPACE) &&
                 ParseMonth(SPACE) &&
                 ParseInt(&year, 4, SPACE) &&
                 ParseTime() &&
                 strcmp(cp, "GMT") == 0;
            break;

        case DTFMT_RFC850:
            ok = ParseInt(&day, 2, DASH) &&
                 ParseMonth(DASH) &&
                 ParseInt(&year, 2, SPACE) &&
                 ParseTime() &&
                 strcmp(cp, "GMT") == 0;

            // Handle 2-digit year
            year = (WORD) (year + ((year < 70) ? 2000 : 1900));
            break;

        case DTFMT_ANSIC:
            ok = ParseMonth(SPACE) &&
                 ((*cp == SPACE) ?
                    (cp++, ParseInt(&day, 1, SPACE)) :
                    ParseInt(&day, 2, SPACE)) &&
                 ParseTime() &&
                 ParseInt(&year, 4, 0);
            break;
        }

        if (ok && VerifyMonthDay()) {
            // Mark the object as valid if everything was ok
            this->format = (WORD) fmt;
        }
    }

    // Parse an integer field
    BOOL ParseInt(WORD* result, INT width, CHAR sep) {
        WORD val = 0;
        while (width--) {
            if (*cp < '0' || *cp > '9') return FALSE;
            val = (WORD) (val*10 + (*cp++ - '0'));
        }
        if (*cp++ != sep) return FALSE;
        *result = val;
        return TRUE;
    }

    // Parse the time field hh:mm:ss
    BOOL ParseTime() {
        return ParseInt(&hour, 2, COLON) &&
               ParseInt(&minute, 2, COLON) &&
               ParseInt(&second, 2, SPACE) &&
               hour < 24 &&
               minute < 60 &&
               second < 60;
    }

    // Interpret the day-of-week string
    BOOL ParseWkDay(const CHAR* s, INT len) {
        static const PCSTR wkdays[] = {
            "Sunday"
            "Monday",
            "Tuesday",
            "Wednesday",
            "Thursday",
            "Friday",
            "Saturday"
        };

        for (INT d=0; d < 7; d++) {
            if (strncmp(s, wkdays[d], len) == 0) {
                wkday = (WORD) d;
                return TRUE;
            }
        }
        return FALSE;
    }

    // Interpret the month string
    BOOL ParseMonth(CHAR sep) {
        static const CHAR months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
        const CHAR* s = months;

        for (INT m=0; m < 12; m++, s+=3) {
            if (s[0] == cp[0] &&
                s[1] == cp[1] && 
                s[2] == cp[2]) {
                if (cp[3] == sep) {
                    cp += 4;
                    month = (WORD) (m+1);
                    return TRUE;
                } else
                    break;
            }
        }
        return FALSE;
    }

    // Verify the day of the month
    BOOL VerifyMonthDay() {
        // NOTE: This is a simple-minded check here
        // without taking leap-year into account.
        static const BYTE monthDays[12] = {
            31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        };

        return day > 0 && day <= monthDays[month-1];
    }

    VOID MarkInvalid() { format = DTFMT_INVALID; }
    BOOL IsInvalid() { return (format == DTFMT_INVALID); }

    // Date/time string format
    enum {
        DTFMT_INVALID,
        DTFMT_RFC822,
            // RFC 822 format: Sun, 06 Nov 1994 08:49:37 GMT
        DTFMT_RFC850,
            // RFC 850 format: Sunday, 06-Nov-94 08:49:37 GMT
        DTFMT_ANSIC,
            // ANSI C's asctime() format: Sun Nov  6 08:49:37 1994
    };

    WORD format;
    WORD year, month, day, wkday;
    WORD hour, minute, second;
    const CHAR* cp;
};

#endif // !_HTTPTIME_H

