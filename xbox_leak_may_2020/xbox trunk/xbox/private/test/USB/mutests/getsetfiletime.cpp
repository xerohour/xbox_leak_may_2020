/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    GetSetFileTime.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "mutests.h"

/*

Routine Description:

    Tests the GetFileTime / SetFileTime APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::GetSetFileTime_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL GetFileTime(
      HANDLE hFile,                 // handle to file
      LPFILETIME lpCreationTime,    // creation time
      LPFILETIME lpLastAccessTime,  // last access time
      LPFILETIME lpLastWriteTime    // last write time
    );

    BOOL SetFileTime(
      HANDLE hFile,                     // handle to file
      CONST FILETIME *lpCreationTime,   // creation time
      CONST FILETIME *lpLastAccessTime, // last-access time
      CONST FILETIME *lpLastWriteTime   // last-write time
    );
    */
    xSetFunctionName(hLog, "GetFileTime / SetFileTime");

    unsigned date;
    HANDLE hFile;
    HANDLE hFile2;

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FFT01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%ws:\\FFT02_%d.txt", BASE_DRIVE, ThreadID);

    FILETIME fileTime[3];
    FILETIME gfileTime[3];

    // initialize system state
    hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD time = GetTickCount();



    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    for(date=0; date<20; date++)
        {
        TESTCASE("Valid Dates")
            {
            for(unsigned i=0; i<3; i++)
                {
                // just create some different dates
                if(CreateFileTime(&fileTime[i], (25+i) + 1974, date % (9+i) + 1, date % (26+i) + 1, i==1? 0 :(date % (21+i)), i==1? 0 : (date % (55+i)), 0, 0) == FALSE)
                    xLog(hLog, XLL_WARN, "SystemTimeToFileTime Error: %ld", GetLastError());
                }

            if(SetFileTime(hFile, &fileTime[0], &fileTime[1], &fileTime[2]) == TRUE)
                TESTPASS(hLog, "SetFileTime (ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "SetFileTime (ec: %ld)", GetLastError());

            if(GetFileTime(hFile, &gfileTime[0], &gfileTime[1], &gfileTime[2]) == TRUE)
                TESTPASS(hLog, "GetFileTime (ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "GetFileTime (ec: %ld)", GetLastError());

            if(memcmp(fileTime, gfileTime, sizeof(FILETIME)*3) == 0)
                {
                SYSTEMTIME sysTime;
                char buffer[256];
                char *t = buffer;

                t += sprintf(t, "Set");
                FileTimeToSystemTime(&fileTime[0], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&fileTime[1], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&fileTime[2], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

                t += sprintf(t, " == Get");
                FileTimeToSystemTime(&gfileTime[0], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&gfileTime[1], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&gfileTime[2], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                TESTPASS(hLog, buffer);
                }
            else
                {
                SYSTEMTIME sysTime;
                char buffer[256];
                char *t = buffer;

                t += sprintf(t, "Set");
                FileTimeToSystemTime(&fileTime[0], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&fileTime[1], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&fileTime[2], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

                t += sprintf(t, " != Get");
                FileTimeToSystemTime(&gfileTime[0], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&gfileTime[1], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                FileTimeToSystemTime(&gfileTime[2], &sysTime);
                t += sprintf(t, "(%d/%d/%d %d:%d:%d.%d)", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                TESTFAIL(hLog, buffer);
                }
            } ENDTESTCASE;
        }



    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    CloseHandle(hFile);
    DeleteFile(lpFile1);

    return 0;
    }
