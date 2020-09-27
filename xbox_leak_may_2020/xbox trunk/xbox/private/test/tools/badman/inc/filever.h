//---------------------------------------------------------------------------
//
// MODULE: filever.h
// AUTHOR: Steveko
// DATE:        03-27-92
//        
// Copyright (c) 1992 Microsoft Corporation
//        
// Header file for fileio and versioning badman testing.
//
//---------------------------------------------------------------------------

#define FIO_DEFAULT_FILE_NAME           "z:\\abcdefg.hij"
#define FIO_DEFAULT_FILE_NAME_W        L"z:\\abcdefg.hij"

#define FIO_DEFAULT_FILE_TO_CREATE      "z:\\abcdefg.abc"
#define FIO_DEFAULT_FILE_TO_CREATE_W   L"z:\\abcdefg.abc"

#define FIO_DEFAULT_SRC_FILE_NAME       "z:\\abcdefg.123"
#define FIO_DEFAULT_SRC_FILE_NAME_W    L"z:\\abcdefg.123"

#define FIO_DEFAULT_DEST_FILE_NAME      "z:\\abcdefg.234"
#define FIO_DEFAULT_DEST_FILE_NAME_W   L"z:\\abcdefg.234"

        /* Buffer lengths. */

#define FIO_READWRITE_BUFFER_SIZE              1025
#define FIO_LOGICAL_DRIVE_STRING_BUFFER_LENGTH 1025
#define FIO_VOLUME_NAME_BUFFER_LENGTH          1025
#define FIO_FILE_SYSTEM_NAME_BUFFER_LENGTH     1025

        /* Prototypes. */

HANDLE FIOHandleSetup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);
DWORD  FIODwordSetup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);
LPSTR  FIOLpstrSetup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);
LPWSTR FIOLpstrSetupW(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);

VOID FIOHandleCleanup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);
VOID FIOLpstrCleanup(int CaseNo, LPVOID *SCInfo, HANDLE hLog, HANDLE hConOut);

        /* Useful constants for manipulating DOS dates/times. */

#define DOS_DATE_MASK_DAY    0x001f     // low  5 bits (1-31)
#define DOS_DATE_MASK_MONTH  0x01e0     // mid  4 bits (1-12)
#define DOS_DATE_MASK_YEAR   0xfe00     // high 7 bits (0-119, offset from 1980)

#define DOS_DATE_SHIFT_DAY   0
#define DOS_DATE_SHIFT_MONTH 5
#define DOS_DATE_SHIFT_YEAR  9

#define DOS_TIME_MASK_SECONDS 0x001f    // low  5 bits (0-29, 2 sec granular)
#define DOS_TIME_MASK_MINUTES 0x07e0    // mid  6 bits (0-59)
#define DOS_TIME_MASK_HOURS   0xf800    // high 5 bits (0-23)

#define DOS_TIME_SHIFT_SECONDS  0
#define DOS_TIME_SHIFT_MINUTES  5
#define DOS_TIME_SHIFT_HOURS   11

        /* Specify the year as an offset from 1980 */

#define MAKE_DOS_DATE(month, day, year) \
                     (year  << DOS_DATE_SHIFT_YEAR  | \
                      month << DOS_DATE_SHIFT_MONTH | \
                      day   << DOS_DATE_SHIFT_DAY )

        /* Specify seconds 0-30 (corresponds to DOS 2 second granularity) */

#define MAKE_DOS_TIME(hour, min, sec) \
                     (hour << DOS_TIME_SHIFT_HOURS   | \
                      min  << DOS_TIME_SHIFT_MINUTES | \
                      sec  << DOS_TIME_SHIFT_SECONDS )

#define VER_DEFAULT_SRC_FILE         "z:\\xyzver.src"
#define VER_DEFAULT_SRC_FILE_W      L"z:\\xyzver.src"

#define VER_DEFAULT_DEST_FILE        "z:\\xyzver.dst"
#define VER_DEFAULT_DEST_FILE_W     L"z:\\xyzver.dst"

#define VER_DEFAULT_SRC_DIR          "z:\\"
#define VER_DEFAULT_SRC_DIR_W       L"z:\\"

#define VER_DEFAULT_DEST_DIR         "z:\\xyzver.dst"
#define VER_DEFAULT_DEST_DIR_W      L"z:\\xyzver.dst"

#define VER_DEFAULT_APP_DIR          "z:\\xyzver.app"
#define VER_DEFAULT_APP_DIR_W       L"z:\\xyzver.app"

#define VER_DEFAULT_DIR_NOT_EXISTS       "z:\\notexist"
#define VER_DEFAULT_DIR_NOT_EXISTS_W    L"z:\\notexist"

#define VER_DEFAULT_FILE_WITH_VER_INFO       "verinf.exe"
#define VER_DEFAULT_FILE_WITH_VER_INFO_W    L"verinf.exe"

#define VER_SIZE_LANGUAGE_BUFFER 255
