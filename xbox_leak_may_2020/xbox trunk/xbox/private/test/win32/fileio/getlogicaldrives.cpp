/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetSetFileAttributes.cpp

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
#include "fileio_win32API.h"

static void ClearNulls(int num, char *buf);

/*

Routine Description:

    Tests the GetLogicalDrives / GetLogicalDriveStrings APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetLogicalDrives_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD GetLogicalDrives(VOID);

    DWORD GetLogicalDriveStrings(
      DWORD nBufferLength,  // size of buffer
      LPTSTR lpBuffer       // drive strings buffer
      );
    */
    xSetFunctionName(hLog, "GetLogicalDrives");

    DWORD drives = GetLogicalDrives();
    unsigned numDrives = 0;

    TESTCASE("GetLogicalDrives")
        {
        if(drives != 0)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        for(DWORD i=1; i<drives; i=i<<1)
            {
            if(drives & i) ++numDrives;
            }

        xLog(hLog, XLL_INFO, "Found %u logical drives", numDrives);
        } ENDTESTCASE;
    
    // TODO actually verify valid drives based on the hardware


    xSetFunctionName(hLog, "GetLogicalDriveStrings");

    char *buffer;
    DWORD size;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(0, NULL)")
        {
        size = GetLogicalDriveStrings(0, NULL);
        if(size == numDrives*4+1)
            TESTPASS(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        } ENDTESTCASE;
    TESTCASE("(0xFFFF, NULL)")
        {
        BUGGEDPASS(138, "ACCESS VIOLATION \"By Design\"");
        size = GetLogicalDriveStrings(0xFFFF, NULL);
        if(size == numDrives*4+1)
            TESTPASS(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("GetLogicalDriveStrings")
        {
        buffer = new char[numDrives*5];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(numDrives*5, buffer);
        ClearNulls(size/4, buffer);
        if(size == numDrives*4)
            TESTPASS(hLog, "returned: %lu, %s (ec: %ld)", size, buffer, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu, %s (ec: %ld)", size, buffer, GetLastError());
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });
    TESTCASE("Verify D: T: Z: U:")
        {
        buffer = new char[numDrives*5];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(numDrives*5, buffer);

        char drives[] = "DTZU";
        for(unsigned i=0; i<sizeof(drives)/sizeof(drives[0]) - 1; i++)
            {
            for(unsigned j=0; j<size/4; j++)
                {
                if(drives[i] == buffer[j*4])
                    break;
                }
            if(j<size/4)
                TESTPASS(hLog, "Found %s", &buffer[j*4]);
            else
                TESTFAIL(hLog, "Could not find %wc", drives[i]);
            }
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });
    TESTCASE("buffer[just enough]")
        {
        buffer = new char[numDrives*4+1];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(numDrives*4+1, buffer);
        ClearNulls(size/4, buffer);
        if(size == numDrives*4)
            TESTPASS(hLog, "returned: %lu, %s (ec: %ld)", size, buffer, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu, %s (ec: %ld)", size, buffer, GetLastError());
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Verify No C:")
        {
        buffer = new char[numDrives*5];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(numDrives*5, buffer);

        char drives[] = "C";
        for(unsigned i=0; i<sizeof(drives)/sizeof(drives[0]) - 1; i++)
            {
            for(unsigned j=0; j<size/4; j++)
                {
                if(drives[i] == buffer[j*4])
                    break;
                }
            if(j==size/4)
                TESTPASS(hLog, "Did not find %wc", drives[i]);
            else
                TESTFAIL(hLog, "Found %wc", drives[i]);
            }
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });
    TESTCASE("buffer[0]")
        {
        buffer = new char[1];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(0, buffer);
        if(size == numDrives*4+1)
            TESTPASS(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });
    TESTCASE("buffer[1]")
        {
        buffer = new char[1];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(1, buffer);
        if(size == numDrives*4+1)
            TESTPASS(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });
    TESTCASE("buffer[just under]")
        {
        buffer = new char[numDrives*4];
        if(!buffer)
            {
            BLOCKED(0, "Memory allocation failed");
            }
        size = GetLogicalDriveStrings(numDrives*4, buffer);
        if(size == numDrives*4+1)
            TESTPASS(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "returned: %lu (ec: %ld)", size, GetLastError());
        } ENDTESTCASE_CLEAN({ if(buffer) delete[] buffer; });

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    return 0;
    }


/*

Routine Description:

    Removes the internal null characters so we can print out a single string
        with all the drive letters.

Arguments:

    int num - the number of drives in the buffer
    char *buf - the buffer filled by GetLogicalDriveStrings()

Return Value:

    None

*/
static void ClearNulls(int num, char *buf)
    {
    for(int i=0; i<num; i++)
        {
        buf[i*4+3] = (short)' ';
        }
    }
