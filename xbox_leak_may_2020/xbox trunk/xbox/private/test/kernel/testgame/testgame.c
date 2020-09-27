/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    testgame.c

Abstract:

    test title, meant to be marked with a new number and run to simulate a game booting

Author:

    John Daly (johndaly) 9-Oct-2000

Environment:

    X-Box

Notes:

    totally ANSI

Revision History:

    9-Oct-2000  johndaly
    created

Current Status:

    need to globalize getting / setting config data to centralize the calls
    

--*/

#include "testgame.h"

void 
__cdecl 
main(
    void
    )
/*++

Routine Description:

    calls the main sets of functionality

Arguments:

    None

Return Value:

    None

--*/ 

{   
    ULONG TitleID;
    HANDLE hLogFile;
    TCHAR FileName[MAX_PATH] = {0};
    DWORD Error = 0;
    char *tmpPtr;
    int x;
    LARGE_INTEGER liDistanceToMove = {0};
    
    OutputDebugString(TEXT("********************************************************************************\r\nTestGame starting\r\n"));

    //
    // get our TitleID so we know which instance of this test we are
    //
    
    TitleID = GetTitleID();

    //
    // create a log file in a permanent place...
    // which will be the tdata\TitleID / title persistant data
    //

    _stprintf(FileName, TEXT("T:\\%8.8X.log"), TitleID);
    hLogFile = CreateFile((LPCSTR)FileName,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    
    OutputDebugString(FileName);
    OutputDebugString(TEXT("\r\n"));

    if (INVALID_HANDLE_VALUE == hLogFile) {
        Error = GetLastError();
        OutputDebugString(TEXT("CreatFile Failed\n"));
        DebugBreak();
    }
    
    //
    // seek to EOF
    //
     
    SetFilePointerEx(hLogFile, liDistanceToMove, NULL, FILE_END);

    //
    // initialize data buffer for saved games / cache
    //
    
    FileDataBuffer = calloc(1, FILE_DATA_SIZE);
    
    if (NULL == FileDataBuffer) {
        OutputDebugString(TEXT("Out of memory!"));
        DebugBreak();
    }

    tmpPtr = (char *)FileDataBuffer;
    for (x = 1; x < FILE_DATA_SIZE; x++) {
        *tmpPtr = (char)(x % 16);
        ++tmpPtr;
    }

    //
    // check machine state and record it
    //
    
    CheckMachineStatePre(TitleID, hLogFile);

    //
    // check saved config data from hard disk (XSetValue / XGetValue)
    //
    
    CheckConfigData(TitleID, hLogFile);

    //
    // save some games
    //

    SaveSomeGames(TitleID, hLogFile);

    //
    // fill the cache with files marked with this TestID
    //
    
    FillCachePartition(TitleID, hLogFile);

    //
    // use some title persistant data
    //
    
    UseTitlePersistentData(TitleID, hLogFile);

    //
    // name / check MU name
    //
    
    NameMUs(TitleID, hLogFile);

    //
    // test out NickNames
    //
    
    UseNickNames(TitleID, hLogFile);

    //
    // test out Title Info
    //
    
    CheckTitleInfo(TitleID, hLogFile);

    //
    // check machine state and record it
    //
    
    CheckMachineStatePost(TitleID, hLogFile);
    
    //
    // run any tests that need to be run outside of the timed portion
    //
    
    AltTitleTest(TitleID, hLogFile);
    
    //
    // modify system to boot the next test variation
    //
    
    SetupNextRun(TitleID, hLogFile);
    
}

ULONG
GetTitleID(
    void
    )
/*++

Routine Description:

    get our TitleID so we know which instance of this test we are

Arguments:

    None

Return Value:

    ULONG - title ID number XUID
    0 if information not found

--*/ 
{
    PXBEIMAGE_HEADER ImageHeader;
    PXBEIMAGE_CERTIFICATE Certificate;

    ImageHeader = XeImageHeader();
    if (NULL == ImageHeader) {
        OutputDebugString(TEXT("NULL ImageHeader!\n"));
        DebugBreak();
    }

    if (ImageHeader) {
        Certificate = (PVOID)(ImageHeader->Certificate);
        if (NULL == Certificate) {
            OutputDebugString(TEXT("NULL Certificate!\n"));
            DebugBreak();
        }
    }
    if (Certificate && ImageHeader) {
        return ((Certificate)->TitleID);
    } else {
        return(0);
    }
}

void
CheckMachineStatePre(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    check and log the state of the machine before we do any work

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

--*/ 
{
    #define INIBUFFERSIZE 100
    TCHAR INIBuffer[INIBUFFERSIZE] = {0};
    DWORD RetVal;
    SYSTEMTIME SystemTime;
    DWORD LaunchDataType;
    LAUNCH_DATA LaunchData;
    DWORD dwRetval;
    int x = 0;
    
    //
    // look at the passed in data from the preceding title
    //
    
    dwRetval = XGetLaunchInfo(&LaunchDataType, &LaunchData);
    
    //
    // validate the data
    //

    if (ERROR_SUCCESS == dwRetval) {
        for (x = 0; x < MAX_LAUNCH_DATA_SIZE / sizeof(sizeof(TitleID)); x++) {
            if (*(ULONG *)&LaunchData.Data[x*(sizeof(TitleID))] != TitleID) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("launch data mismatch!\r\n"));
                break;
            }
        }
        if (x >= MAX_LAUNCH_DATA_SIZE / sizeof(TitleID)) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("launch data OK!\r\n"));
        }
    } else {
        LogText(TitleID,
                hLogFile,
                TEXT("No launch data returned\r\n"));
    }
    
    //
    // set up the timer,
    // start timer so we can see how long this test takes
    //
    
    if ( FALSE == QueryPerformanceFrequency(&Frequency) ) {
        ;// log and exit
    }

    StartTimer();
    
    //
    // log the test ID
    //
    
    LogText(TitleID,hLogFile,TEXT("Test ID:%8.8X\r\n"),TitleID);

    //
    // what is the current time?
    // log it
    //

    GetSystemTime(&SystemTime);
    LogText(TitleID,
            hLogFile,
            TEXT("Time: Year:%d Month:%d DayOfWeek:%d Day:%d Hour:%d Minute:%d\r\n"),
            SystemTime.wYear, 
            SystemTime.wMonth, 
            SystemTime.wDayOfWeek, 
            SystemTime.wDay, 
            SystemTime.wHour, 
            SystemTime.wMinute);
    
    //
    // mount all plugged in MUs
    //

    MountMUs(TitleID, hLogFile);

    //
    // log the number of partitions, and the type / amount of space left on each
    //
    
    LogDiskStuff(TitleID, hLogFile);

    //
    // what else...
    //
}

void
SaveSomeGames(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    save the number and size of games specified in the 
    INI file for this instance of the game

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None
    
Note: Need to handle saving to MUs as well

--*/ 
{
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    BOOL bRetVal;
    int FileCount, FileSize, x;
    WCHAR wgamename[13] = {0};
    char agamename[13] = {0};
    CHAR PathBuffer[MAX_PATH] = {0};
    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;
    TCHAR SaveRoot[] = {TEXT("U:\\")};
    HANDLE hFile;
    int y = 0;
    int IterationC;

    //
    // need to save games based on iteration...
    //

    IterationC = GetPrivateProfileIntA("Iterations", 
                                       "current", 
                                       1, 
                                       "testgame.ini");
    
    //
    // get the number and size of the games to save from the INI file
    //

    FileCount = GetPrivateProfileIntA("SavedGame","count", 10, "testgame.ini");
    FileSize = GetPrivateProfileIntA("SavedGame","size", 1024, "testgame.ini");

    //
    // for each available savegame storage location (disk plus MUs)
    //

    while ( UDataDrives[y] ) {

        //
        // grab the next UData drive from the list
        //

        SaveRoot[0] = UDataDrives[y];
        
        //
        // is there savegame space available?
        // look in U:\
        //

        if ( FALSE == GetDiskFreeSpaceEx((LPCSTR)SaveRoot,
                                         &FreeBytesAvailable,
                                         &TotalNumberOfBytes,
                                         &TotalNumberOfFreeBytes) ) {
            OutputDebugString(TEXT("GetDiskFreeSpaceEx has failed! (U:\\)\n"));
            DebugBreak();
        }

        if ( FreeBytesAvailable.QuadPart < (FileCount * FileSize) ) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: out of savegame space\r\n"));
            ++y;
            continue;
        }

        //
        // save them
        //

        for ( x = 0; x < FileCount; x++ ) {

            swprintf(wgamename, L"game%4.4x%4.4x", IterationC, x);

            bRetVal = XCreateSaveGame((LPCSTR)SaveRoot,
                                      wgamename,
                                      OPEN_ALWAYS,
                                      0,
                                      PathBuffer,
                                      sizeof(PathBuffer));
            if (ERROR_SUCCESS != bRetVal) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: XCreateSaveGame failed : winerror:%d\r\n"), bRetVal);
                continue;
            }

            sprintf(agamename, "game%4.4x%4.4x", IterationC, x);
            strcat(PathBuffer, agamename);
            strcat(PathBuffer, ".sav");

            hFile = CreateFile(PathBuffer,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

            if ( INVALID_HANDLE_VALUE == hFile ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: CreateFile failed while saving game\r\n"));
                continue;
            }

            //
            // write some easily understood pattern of data to the file
            //

            dwBytesToWrite = FileSize;
            while ( dwBytesToWrite ) {

                if ( 0 == WriteFile(hFile,
                                    FileDataBuffer,
                                    min(dwBytesToWrite, FILE_DATA_SIZE),
                                    &dwBytesWritten,
                                    NULL) ) {
                    LogText(TitleID,
                            hLogFile,
                            TEXT("ERROR: CreateFile failed while saving game\r\n"));
                    CloseHandle(hFile);
                    break;
                }

                if ( dwBytesToWrite >= dwBytesWritten ) {
                    dwBytesToWrite -= dwBytesWritten;
                } else {
                    dwBytesToWrite = 0;
                }
            }
            CloseHandle(hFile);
        }

        //
        // log the number that were successfully saved
        //

        LogText(TitleID,
                hLogFile,
                TEXT("Saved: %d game files to %c:\\ \r\n"),
                x,
                UDataDrives[y]);
        ++y;
    }
    
    return;
}

//
// stuff for following function
//

#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }
static const OBJECT_STRING g_DDosDevicePrefix =  CONSTANT_OBJECT_STRING(OTEXT("\\??\\Z:"));

void
FillCachePartition(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    save the number and size of cache files specified in the 
    INI file for this instance of the game

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None
    
Notes:

    Yes, this function is full of duplicated cut and pasted code...so shoot me!

--*/ 
{
    #define TESTFILES 10
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    BOOL bRetVal;
    int FileCount, FileSize, x;
    char dataname[MAX_PATH] = {0};
    HANDLE hFile;
    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;
    NTSTATUS Status;
    HANDLE LinkHandle = NULL;
    OBJECT_ATTRIBUTES Obja;
    OCHAR szLinkTarget[MAX_PATH];
    OBJECT_STRING LinkTarget;
    int IterationC;
    ULONG flags;
    HANDLE hFileArray[TESTFILES];
    DWORD dwLastError;

    //
    // need to save games based on iteration...
    //

    IterationC = GetPrivateProfileIntA("Iterations", 
                                       "current", 
                                       1, 
                                       "testgame.ini");

    //
    // look in Z:\
    // log cache partition information
    // first clean it up...
    //

    //
    // this code is needed if we build with /INITFLAGS:0
    // It will cause problems (always using the same cache partition) if used with other flags
    //

    flags = XeImageHeader()->InitFlags;

    LogText(TitleID,
            hLogFile,
            TEXT("detected: //INITFLAGS: %8.8d \r\n"),
            flags);

    if (0 == flags) {
        if (FALSE == XMountUtilityDrive(TRUE)) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("XMountUtilityDrive(TRUE) returned failure (only relevant if //INITFLAGS:0). Code: %8.8d \r\n"),
                    GetLastError());
        }
    }

    InitializeObjectAttributes(&Obja,
                               (POBJECT_STRING) &g_DDosDevicePrefix,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenSymbolicLinkObject(&LinkHandle, &Obja);

    if ( NT_SUCCESS(Status) ) {
        LinkTarget.Buffer = szLinkTarget;
        LinkTarget.Length = 0;
        LinkTarget.MaximumLength = sizeof(szLinkTarget);

        Status = NtQuerySymbolicLinkObject(LinkHandle, &LinkTarget, NULL);
    
        if ( NT_SUCCESS(Status) ) {
            if ( LinkHandle )
                NtClose(LinkHandle);

            LogText(TitleID,
                    hLogFile,
                    TEXT("Cache Partition: %.*s\r\n"),
                    LinkTarget.Length,
                    LinkTarget.Buffer);

            //
            // any free space...
            //

            if ( FALSE == GetDiskFreeSpaceEx("Z:\\",
                                             &FreeBytesAvailable,
                                             &TotalNumberOfBytes,
                                             &TotalNumberOfFreeBytes) ) {
                OutputDebugString(TEXT("GetDiskFreeSpaceEx has failed! (Z:\\)\n"));
                DebugBreak();
            }

            //
            // get the number and size of the files to cache from the INI file
            //

            FileCount = GetPrivateProfileIntA("CacheFill","count", 10, "testgame.ini");
            FileSize = GetPrivateProfileIntA("CacheFill","size", 1024, "testgame.ini");

            if ( FreeBytesAvailable.QuadPart < (FileCount * FileSize) ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: out of Cache space\r\n"));
                LogText(TitleID,
                        hLogFile,
                        TEXT("space found : 0x%I64x\r\n"),
                        FreeBytesAvailable.QuadPart);
                return;
            }

            //
            // cache them
            //

            for ( x = 0; x < FileCount; x++ ) {

                sprintf(dataname, "Z:\\data%4.4x%4.4x.dat", IterationC, x);

                hFile = CreateFile(dataname,
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);

                if ( INVALID_HANDLE_VALUE == hFile ) {
                    LogText(TitleID,
                            hLogFile,
                            TEXT("ERROR: CreateFile failed while saving data (cache)\r\n"));
                    continue;
                }

                //
                // write some easily understood pattern of data to the file
                //

                dwBytesToWrite = FileSize;
                while ( dwBytesToWrite ) {

                    if ( 0 == WriteFile(hFile,
                                        FileDataBuffer,
                                        min(dwBytesToWrite, FILE_DATA_SIZE),
                                        &dwBytesWritten,
                                        NULL) ) {
                        LogText(TitleID,
                                hLogFile,
                                TEXT("ERROR: WriteFile failed while saving data (cache)\r\n"));
                        CloseHandle(hFile);
                        break;
                    }

                    if ( dwBytesToWrite >= dwBytesWritten ) {
                        dwBytesToWrite -= dwBytesWritten;
                    } else {
                        dwBytesToWrite = 0;
                    }
                }
                CloseHandle(hFile);
            }

            //
            // log the number that were successfully cached
            //

            LogText(TitleID,
                    hLogFile,
                    TEXT("Saved: %d data files\r\n"),
                    x);

        } else {
            OutputDebugString(TEXT("NtQuerySymbolicLinkObject has failed!\n"));
        //    DebugBreak();
        }

    } else {
        OutputDebugString(TEXT("NtOpenSymbolicLinkObject has failed! \n"));
        //DebugBreak();
    }

    //
    // new test for XFormatUtilityDrive()
    //
    
    //
    // case - drive mounted, no open files
    // assumes no files are open on z:\
    // double-check in debugger
    //

    if(FALSE == XFormatUtilityDrive()) {
        LogText(TitleID,
                hLogFile,
                TEXT("ERROR: XFormatUtilityDrive failed - no files open scenario\r\n"));
    } else {
        LogText(TitleID,
                hLogFile,
                TEXT("XFormatUtilityDrive succeeded - no files open scenario\r\n"));
    }

    if ( FALSE == GetDiskFreeSpaceEx("Z:\\",
                                     &FreeBytesAvailable,
                                     &TotalNumberOfBytes,
                                     &TotalNumberOfFreeBytes) ) {
        OutputDebugString(TEXT("GetDiskFreeSpaceEx has failed! (Z:\\)\n"));
        DebugBreak();
    }
    
    //
    // case - drive mounted, some open files
    //

    //
    // open some files
    //

    for ( x = 0; x < TESTFILES; x++ ) {

        sprintf(dataname, "Z:\\data%4.4x%4.4x.dat", IterationC, x);

        hFileArray[x] = CreateFile(dataname,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if ( INVALID_HANDLE_VALUE == hFileArray[x] ) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: CreateFile failed while saving data (cache)\r\n"));
            continue;
        }

        //
        // write some easily understood pattern of data to the file
        //

        dwBytesToWrite = FileSize;
        while ( dwBytesToWrite ) {

            if ( 0 == WriteFile(hFileArray[x],
                                FileDataBuffer,
                                min(dwBytesToWrite, FILE_DATA_SIZE),
                                &dwBytesWritten,
                                NULL) ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: CreateFile failed while saving data (cache)\r\n"));
                break;
            }

            if ( dwBytesToWrite >= dwBytesWritten ) {
                dwBytesToWrite -= dwBytesWritten;
            } else {
                dwBytesToWrite = 0;
            }
        }
    }

    if(FALSE == XFormatUtilityDrive()) {
        LogText(TitleID,
                hLogFile,
                TEXT("ERROR: XFormatUtilityDrive failed - files open scenario\r\n"));
    } else {
        LogText(TitleID,
                hLogFile,
                TEXT("XFormatUtilityDrive succeeded - files open scenario\r\n"));
    }

    //
    // Probe then close files
    // what should happen?
    //

    for ( x = 0; x < TESTFILES; x++ ) {
        
        //
        // try to do something with the file - how should it fail?
        // read and write...
        //
        
        dwBytesToWrite = FileSize;
        if ( 0 == WriteFile(hFileArray[x],
                            FileDataBuffer,
                            min(dwBytesToWrite, FILE_DATA_SIZE),
                            &dwBytesWritten,
                            NULL) ) {
            dwLastError = GetLastError();
            LogText(TitleID,
                    hLogFile,
                    TEXT("WriteFile failed as expected saving data\r\n"));
        } else {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: WriteFile succeeded unexpectedly\r\n"));
        }

        if ( 0 == ReadFile(hFileArray[x],
                            FileDataBuffer,
                            min(dwBytesToWrite, FILE_DATA_SIZE),
                            &dwBytesWritten,
                            NULL) ) {
            dwLastError = GetLastError();
            LogText(TitleID,
                    hLogFile,
                    TEXT("ReadFile failed as expected saving data\r\n"));
        } else {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: ReadFile succeeded unexpectedly\r\n"));
        }

        CloseHandle(hFileArray[x]);
    }

    //
    // make sure the new drive is accessable
    //

    for ( x = 0; x < FileCount; x++ ) {

        sprintf(dataname, "Z:\\data%4.4x%4.4x.dat", IterationC, x);

        hFile = CreateFile(dataname,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if ( INVALID_HANDLE_VALUE == hFile ) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: CreateFile failed while saving data (cache)\r\n"));
            continue;
        }

        //
        // write some easily understood pattern of data to the file
        //

        dwBytesToWrite = FileSize;
        while ( dwBytesToWrite ) {

            if ( 0 == WriteFile(hFile,
                                FileDataBuffer,
                                min(dwBytesToWrite, FILE_DATA_SIZE),
                                &dwBytesWritten,
                                NULL) ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: WriteFile failed while saving data (cache)\r\n"));
                CloseHandle(hFile);
                break;
            }

            if ( dwBytesToWrite >= dwBytesWritten ) {
                dwBytesToWrite -= dwBytesWritten;
            } else {
                dwBytesToWrite = 0;
            }
        }
        CloseHandle(hFile);
    }

    //
    // log the number that were successfully cached
    //

    LogText(TitleID,
            hLogFile,
            TEXT("Saved: %d data files\r\n"),
            x);

    return;
}

void
UseTitlePersistentData(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    save the number and size of Title Persistent Data files specified in the 
    INI file for this instance of the game

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

--*/ 
{
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    BOOL bRetVal;
    int FileCount, FileSize, x;
    char dataname[MAX_PATH] = {0};
    HANDLE hFile;
    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;

    //
    // is there title persistant data space available?
    // look in T:\
    //
    
    if (FALSE == GetDiskFreeSpaceEx("T:\\",
                                   &FreeBytesAvailable,
                                   &TotalNumberOfBytes,
                                   &TotalNumberOfFreeBytes)) {
        OutputDebugString(TEXT("GetDiskFreeSpaceEx has failed! (T:\\)\n"));
        DebugBreak();
    }

    //
    // get the number and size of the files to cache from the INI file
    //

    FileSize = GetPrivateProfileIntA("TitleData","size", 1024, "testgame.ini");
    FileCount = GetPrivateProfileIntA("TitleData","count", 10, "testgame.ini");

    if (FreeBytesAvailable.QuadPart < (FileCount * FileSize)) {
        LogText(TitleID,
                hLogFile,
                TEXT("ERROR: out of Cache space\r\n"));
        return;
    }

    //
    // store them in the persistant area
    //
    
    for (x = 0; x < FileCount; x++) {
        
        sprintf(dataname, "T:\\data%4.4x.dat", x);

        hFile = CreateFile(dataname,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if (INVALID_HANDLE_VALUE == hFile) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR: CreateFile failed while saving data (title)\r\n"));
            continue;
        }

        //
        // write some easily understood pattern of data to the file
        //

        dwBytesToWrite = FileSize;
        while (dwBytesToWrite) {

            if (0 == WriteFile(hFile,
                               FileDataBuffer,
                               min(dwBytesToWrite, FILE_DATA_SIZE),
                               &dwBytesWritten,
                               NULL)) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: CreateFile failed while saving data (title)\r\n"));
                CloseHandle(hFile);
                break;
            }

            if (dwBytesToWrite >= dwBytesWritten) {
                dwBytesToWrite -= dwBytesWritten;
            } else {
                dwBytesToWrite = 0;
            }
        }
        CloseHandle(hFile);
    }
    
    //
    // log the number of files that were successfully cached
    //
    
    LogText(TitleID,
            hLogFile,
            TEXT("Saved: %d persistant files\r\n"),
            x);
    
    return;
}

void
CheckConfigData(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    check saved config data from hard disk (XSetValue / XGetValue)

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None
    
Note:

    BUGBUG - do the right thing with the binary values

--*/ 
{
    #define QUERY_BUFF_LEN  300
    ULONG ulType;
    char pBuffer[QUERY_BUFF_LEN] = {0};
    VOID *pValue;
    ULONG cbResultLength;

    pValue = (VOID *)pBuffer;

    if (ERROR_SUCCESS == XQueryValue(XC_TIMEZONE_BIAS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TIMEZONE_BIAS : 0x%8.8X\r\n"), 
                *(DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(_XC_TIMEZONE_BIAS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_STD_NAME, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_STD_NAME : %s\r\n"), 
                (char *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_STD_NAME,,,) Failed\n"));
        DebugBreak();
    }

    //
    // find right way to interpret binary values
    //

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_STD_DATE, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_STD_DATE : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_STD_DATE,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_STD_BIAS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_STD_BIAS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_STD_BIAS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_DLT_NAME, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_DLT_NAME : %s\r\n"), 
                (char *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_DLT_NAME,,,) Failed\n"));
        DebugBreak();
    }

    //
    // find right way to interpret binary values
    //

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_DLT_DATE, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_DLT_DATE : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_DLT_DATE,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_TZ_DLT_BIAS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_TZ_DLT_BIAS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_TZ_DLT_BIAS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_LANGUAGE, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_LANGUAGE : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_LANGUAGE,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_VIDEO_FLAGS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_VIDEO_FLAGS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_VIDEO_FLAGS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_AUDIO_FLAGS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_AUDIO_FLAGS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_AUDIO_FLAGS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_PARENTAL_CONTROL_GAMES, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_PARENTAL_CONTROL_GAMES : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_PARENTAL_CONTROL_GAMES,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_PARENTAL_CONTROL_PASSWORD, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_PARENTAL_CONTROL_PASSWORD : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_PARENTAL_CONTROL_PASSWORD,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_PARENTAL_CONTROL_MOVIES, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_PARENTAL_CONTROL_MOVIES : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_PARENTAL_CONTROL_MOVIES,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_ONLINE_IP_ADDRESS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_ONLINE_IP_ADDRESS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_ONLINE_IP_ADDRESS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_ONLINE_DNS_ADDRESS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_ONLINE_DNS_ADDRESS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_ONLINE_DNS_ADDRESS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_ONLINE_DEFAULT_GATEWAY_ADDRESS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS,,,) Failed\n"));
        DebugBreak();
    }

    if (ERROR_SUCCESS == XQueryValue(XC_ONLINE_SUBNET_ADDRESS, &ulType, pValue, QUERY_BUFF_LEN, &cbResultLength)){
        LogText(TitleID,
                hLogFile,
                TEXT("XQueryValue: XC_ONLINE_SUBNET_ADDRESS : 0x%8.8X\r\n"), 
                (DWORD *)pValue);
    } else {
        OutputDebugString(TEXT("XQueryValue(XC_ONLINE_SUBNET_ADDRESS,,,) Failed\n"));
        DebugBreak();
    }

}

void
CheckMachineStatePost(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    check and log the state of the machine after we save games, etc

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

--*/ 
{
    //
    // log the number of partitions, and the type / amount of space left on each
    //
    
    LogDiskStuff(TitleID, hLogFile);
    
    //
    // stop timer, log time
    //
    
    LogText(TitleID,
            hLogFile,
            TEXT("Run Time: %d milliseconds\r\n"), 
            EndTimer());

    //
    // what else...
    //
}

void
AltTitleTest(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    test the alternate title ID functions
    XMountAlternateTitle()
    XUnmountAlternateTitle()

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

Notes:
    
    assumes titles that have run before have mounted and put directories everywhere
    only 16 alt titles are allowed, so look for and check the last 16 titles
    
--*/
{
    TCHAR RootPath[] = {TEXT("U:\\")};
    char ListSequenceNumber[10] = {0};
    int x = 0;
    int y = 0;
    ULONG AlternateTitleID = 0;
    OCHAR chDrive = {0};
    NTSTATUS status;
    BOOL bRetVal;
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    TCHAR DeviceName[5];
    HANDLE hFind;
    int TitleCount = 0;
    int TotalFilesCounted = 0;
    int FileCount;
    int FilesCounted = 0;
    int IterationT;
    
    //
    // *** must be U:, or an MU drive ***
    // for each connected UData device...
    // for each game that has been run...
    // note - hiwords must match on IDs
    //

    FileCount = GetPrivateProfileIntA("SavedGame","count", 10, "testgame.ini");
    IterationT = GetPrivateProfileIntA("Iterations", "total", 1, "testgame.ini");
    FileCount *= IterationT;

    while ( UDataDrives[y] ) {
        
        //
        // initialize list sequence number index
        // 

        x = 0;

        //
        // grab the next UData drive from the list
        //

        RootPath[0] = UDataDrives[y];

        //
        // for all preceding titles, try mount/unmount
        //

        sprintf(ListSequenceNumber, "t%3.3d", x);

        //
        // figure out the alternate title...
        //

        while ( TitleCount < 16 &&
                TitleID != (AlternateTitleID =  (ULONG)GetPrivateProfileIntA("List", 
                                                        ListSequenceNumber, 
                                                        0, 
                                                        "testgame.ini")))
        {
            TitleCount++;
            FilesCounted = 0;

            //
            // returns a NT status, check it, this is the API test for this API...
            //

            status = XMountAlternateTitle((LPCSTR)RootPath,
                                          AlternateTitleID,
                                          &chDrive);

            if ( ERROR_SUCCESS != status ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR! : XMountAlternateTitle failed: alt ID:0x%8.8x : Root:%s\r\n"), 
                        AlternateTitleID,
                        RootPath);
                break;
            } else {
                //LogText(TitleID,
                //        hLogFile,
                //        TEXT("XMountAlternateTitle succeeded: alt ID:0x%8.8x : Root:%s : drive:%c\r\n"),
                //        AlternateTitleID,
                //        RootPath,
                //        chDrive);

                //
                // Should be ready to look at Drive X: now...
                // just make sure we found the data files we expected...
                //

                hFind = XFindFirstSaveGame("X:\\",
                                           &FindGameData);

                if ( INVALID_HANDLE_VALUE == hFind ) {
                    LogText(TitleID,
                            hLogFile,
                            TEXT("ERROR! : XFindFirstSaveGame failed: X:\\\r\n"));
                } else {

                    do {
                        ++FilesCounted;
                        bRetVal = XFindNextSaveGame(hFind, &FindGameData);
                    }while ( bRetVal );

                    //
                    // add more checking here if needed, counting 
                    // saved games is probably enough
                    //

                    if ( FilesCounted != FileCount ) {
                        LogText(TitleID,
                                hLogFile,
                                TEXT("ERROR! : %d saved games counted of %d games expected \r\n"),
                                FilesCounted,
                                FileCount);
                    }

                    XFindClose(hFind);
                }

                status = XUnmountAlternateTitle(chDrive);

                if ( ERROR_SUCCESS != status ) {
                    LogText(TitleID,
                            hLogFile,
                            TEXT("ERROR! : XUnmountAlternateTitle failed\r\n"));
                } else {
                    //LogText(TitleID,
                    //        hLogFile,
                    //        TEXT("XUnmountAlternateTitle succeeded\r\n"));
                }
            }

            sprintf(ListSequenceNumber, "t%3.3d", ++x);

            TotalFilesCounted += FilesCounted;
        } 

        //
        // point to next UData...
        //

        ++y;
    }
    
    //
    // summary log
    //

    LogText(TitleID,
            hLogFile,
            TEXT("Alt UData count: %d drives, %d titles, %d savegames\r\n"),
            y,
            TitleCount,
            TotalFilesCounted);

    //
    // for each TData...
    //

    FileCount = GetPrivateProfileIntA("TitleData","count", 10, "testgame.ini");
    RootPath[0] = 'T';
    x = 0;
    TotalFilesCounted = 0;

    //
    // for all preceding titles, try mount/unmount
    //

    sprintf(ListSequenceNumber, "t%3.3d", x);

    while ( TitleID != (AlternateTitleID =  (ULONG)GetPrivateProfileIntA("List", 
                                                    ListSequenceNumber, 
                                                    0, 
                                                    "testgame.ini"))) 
    {
        FilesCounted = 0;

        //
        // returns a NT status, check it, this is the API test for this API...
        //

        status = XMountAlternateTitle((LPCSTR)RootPath,
                                      AlternateTitleID,
                                      &chDrive);

        if ( ERROR_SUCCESS != status ) {
            LogText(TitleID,
                    hLogFile,
                    TEXT("ERROR! : XMountAlternateTitle failed: alt ID:0x%8.8x : Root:%s\r\n"), 
                    AlternateTitleID,
                    RootPath);
            break;
        } else {
           // LogText(TitleID,
           //         hLogFile,
           //         TEXT("XMountAlternateTitle succeeded: alt ID:0x%8.8x : Root:%s : drive : %c\r\n"),
           //         AlternateTitleID,
           //         RootPath,
           //         chDrive);

            //
            // Should be ready to look at Drive X: now...
            // just make sure we found the data files we expected...
            //

            hFind = FindFirstFile("W:\\data*.dat",
                                  &FindData);

            if ( INVALID_HANDLE_VALUE == hFind ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR! : FindFirstFile failed: X:\\data*.dat\r\n"));
            } else {

                do {
                    ++FilesCounted;
                    bRetVal = FindNextFile(hFind, &FindData);
                } while ( bRetVal );

                //
                // add more checking here if needed, counting 
                // saved games is probably enough
                //

                if ( FilesCounted != FileCount ) {
                    LogText(TitleID,
                            hLogFile,
                            TEXT("ERROR! : %d game data files counted of %d files expected \r\n"), 
                                 FilesCounted,
                                 FileCount);
                }
                FindClose(hFind);
            }

            status = XUnmountAlternateTitle(chDrive);

            if ( ERROR_SUCCESS != status ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("XUnmountAlternateTitle failed\r\n"));
            } else {
                //LogText(TitleID,
                //        hLogFile,
                //        TEXT("XUnmountAlternateTitle succeeded\r\n"));
            }
        }

        sprintf(ListSequenceNumber, "t%3.3d", ++x);

        TotalFilesCounted += FilesCounted;
    }

    //
    // summary log
    //

    LogText(TitleID,
            hLogFile,
            TEXT("Alt TData count: %d titles\r\n"),
            TotalFilesCounted);
}

void
NameMUs(
    DWORD TitleID, 
    HANDLE hLogFile
    )
/*++

Routine Description:

    test the XMUNameFromDriveLetter()

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

Notes:

    do this as simply as possible, I think the API may be in flux...
    use nice short data

--*/ 
{
    int y = 0;
    WCHAR buffer[100];
    DWORD RetVal;

    //
    // for each mounted MU...
    //

    while ( UDataDrives[y] ) {

        if ( (UDataDrives[y] == 'F') ||
             (UDataDrives[y] == 'G') ||
             (UDataDrives[y] == 'H') ||
             (UDataDrives[y] == 'I') ||
             (UDataDrives[y] == 'J') ||
             (UDataDrives[y] == 'K') ||
             (UDataDrives[y] == 'L') ||
             (UDataDrives[y] == 'M') ) {

            //
            // Read MU name, check to make sure it is what we had before
            //

            RetVal = XMUNameFromDriveLetter(UDataDrives[y],
                                            buffer,
                                            sizeof(buffer));

            if ( ERROR_SUCCESS != RetVal ) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: XMUNameFromDriveLetter failed while getting MU name: MU: %c winerror:%d\r\n"), 
                        UDataDrives[y],
                        RetVal);
            } else {
                LogText(TitleID,
                        hLogFile,
                        TEXT("successfully retrieved MU Name: MU %c Name : %ls\r\n"), 
                        UDataDrives[y],
                        buffer);
            }

        }

        //
        // try next
        //

        ++y;
    }
}

void
UseNickNames(
    DWORD TitleID, 
    HANDLE hLogFile
    )
/*++

Routine Description:


Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

Notes:



--*/ 
{
}

void
CheckTitleInfo(
    DWORD TitleID, 
    HANDLE hLogFile
    )
/*++

Routine Description:


Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

Notes:

lookk at these 2 APIs
    DWORD XMUNameFromPortSlot(DWORD dwPort, DWORD dwSlot, LPWSTR lpName, DWORD cchName);

    The private API:
    DWORD XReadMUMetaData(DWORD dwPort, DWORD dwSlot, LPVOID lpBuffer, DWORD dwByteOffset, DWORD dwNumberOfBytesToRead);


--*/ 
{
    DWORD cchName = 128;
    WCHAR Name[128] = {0};
    DWORD insertions;
    unsigned port, slot, i;
    char Drive[10] = {10};
    LPVOID buffer;
    DWORD dwRetVal;

    buffer = calloc(PAGE_SIZE, 1);
    if (NULL == buffer) {
        LogText(TitleID,
                hLogFile,
                TEXT("Allocation failed in CheckTitleInfo()! s\r\n"));
        return;
    }
    
    insertions = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    
    for(i=0; i<ARRAYSIZE(XMUMasks); i++){
        slot = i<XGetPortCount()?0:1;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i]){
            
            //
            // should already be mounted - stole this code from the mounting routien
            //XMountMU(port, slot, Drive);
            //

            dwRetVal = XMUNameFromPortSlot(port, slot, Name, cchName);
            if (ERROR_SUCCESS == dwRetVal) {
                LogText(TitleID,
                        hLogFile,
                        TEXT("successfully retrieved MU Name : %ls\r\n"), 
                        Name);
            } else {
                LogText(TitleID,
                        hLogFile,
                        TEXT("ERROR: XMUNameFromPortSlot failed while getting MU name: winerror:%d\r\n"), 
                        dwRetVal);
            }

            XReadMUMetaData(port, slot, buffer, 0, PAGE_SIZE);

            //
            // check the data (if there is any...)
            // not sure how to do this
            // 


        }
    }

    //
    // cleanup
    //

    free(buffer);
}

void
SetupNextRun(
    ULONG TitleID,
    HANDLE hLogFile
    )
/*++

Routine Description:

    modify the ini file as needed for the next test
    make any other adjustments needed before we terminate and reboot

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None

Notes:

    the test XBEs will be stored as their TestID.xbe
    there will be pre-made tdata\XXXXXXXX directories with the 
    same copy of testgame .ini


--*/ 
{
    char NewTitlePath[MAX_PATH] = {0};
    char ListSequenceNumber[10] = {0};
    TCHAR szIterationC[13] = {0};
    ULONG NextTitleID;
    int IterationC, IterationT;
    int x = 0;

    //
    // break if we have finished
    //
    
    if (0x0123fffe == TitleID) {
        OutputDebugString(TEXT("about to reboot after last test (ID == 0xfffffffe) has run"));
        DebugBreak();
    }
    
    //
    // figure out the next image...
    //
    
    IterationC = GetPrivateProfileIntA("Iterations", 
                                       "current", 
                                       1, 
                                       "testgame.ini");
    
    IterationT = GetPrivateProfileIntA("Iterations", 
                                       "total", 
                                       1, 
                                       "testgame.ini");
    LogText(TitleID,
            hLogFile,
            TEXT("This was iteration %d of %d \r\n"), 
            IterationC,
            IterationT);
    
    if (IterationC == IterationT) {
        
        //
        // reboot next game image
        //

        do {
            sprintf(ListSequenceNumber, "t%3.3d", x);
            x++;
        } while (TitleID != (ULONG)GetPrivateProfileIntA("List", 
                                                         ListSequenceNumber, 
                                                         0, 
                                                         "testgame.ini"));

        sprintf(ListSequenceNumber, "t%3.3d", x);
        NextTitleID = (ULONG)GetPrivateProfileIntA("List", 
                                                   ListSequenceNumber, 
                                                   0, 
                                                   "testgame.ini");
        
        //
        // reset counter for this game
        //
        
        _itot(1, szIterationC, 10);
        WritePrivateProfileString(TEXT("Iterations"), 
                                  TEXT("current"), 
                                  szIterationC,
                                  TEXT("testgame.ini"));


    } else {
        
        //
        // reboot current game image, increment iteration counter
        //

        ++IterationC;
        _itot(IterationC, szIterationC, 10);
        WritePrivateProfileString(TEXT("Iterations"), 
                                  TEXT("current"), 
                                  szIterationC,
                                  TEXT("testgame.ini"));
        NextTitleID = TitleID;

    }
    
    sprintf(NewTitlePath, 
            "%8.8x.xbe", 
            NextTitleID);
    
    LogText(TitleID,
            hLogFile,
            TEXT("Next Title: %s\r\n"), 
            NewTitlePath);
    
    //
    // pass some data to the next title
    // make the launch data a repeating sequence of the TitleID of the 
    // title to be launched
    //

    for (x = 0; x < (MAX_LAUNCH_DATA_SIZE / sizeof(NextTitleID)); x++) {
        *(ULONG *)&LaunchData.Data[x*(sizeof(NextTitleID))] = NextTitleID;
    }

    //
    // reboot to the next title
    //
    
    CloseHandle(hLogFile);
    OutputDebugString(TEXT("TestGame ending\n"));
    XWriteTitleInfoAndReboot(NewTitlePath, 
                             "\\device\\harddisk0\\partition2", 
                             LDT_TITLE, 
                             NextTitleID, 
                             &LaunchData);

}

void
LogText(
    ULONG TitleID,
    HANDLE hLogFile,
	TCHAR *format,
	...
    )
/*++

Routine Description:

    log formatted text

Arguments:

    HANDLE hLogFile
	char *string,
	char *format,
	...

Return Value:

    None
    
Notes

    limit input to 512 tchars
    if one is not careful bad things could happen...
    
--*/ 
{
    #define BUFFERLIMIT 512
    va_list arglist;
    DWORD NumberOfBytesWritten;
    PTCHAR buffer;
    DWORD NumberOfBytesToWrite;
    BOOL bRetval;

    //
    // format the log text, check to see if buffer length was exceded
    //

    buffer = calloc(BUFFERLIMIT, sizeof(TCHAR));
    if (NULL == buffer) {
        OutputDebugString(TEXT("Allocation failed!\n"));
        DebugBreak();
    }

    va_start(arglist, format);
    NumberOfBytesToWrite = _vsntprintf(buffer, BUFFERLIMIT, format, arglist);
    NumberOfBytesToWrite *= sizeof(TCHAR);
    
    if (BUFFERLIMIT - 1 < NumberOfBytesToWrite * sizeof(TCHAR)) {
        OutputDebugString(TEXT("buffer length exceeded, fix your code!\n"));
        DebugBreak();
    }
    
    //
    // append it to the file
    //

    bRetval = WriteFile(hLogFile,
                        buffer,
                        NumberOfBytesToWrite,
                        &NumberOfBytesWritten,
                        NULL);

    OutputDebugString(buffer);
    
    if (FALSE == bRetval) {
        OutputDebugString(TEXT("Writefile has failed!\n"));
        DebugBreak();
    }
    
    va_end(arglist);
    
    if (buffer) {
        free(buffer);
    }
}

void 
LogDiskStuff(
    DWORD TitleID, 
    HANDLE hLogFile
    )
/*++

Routine Description:

    log formatted text

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None
    
Notes

    limit input to 512 tchars
    if one is not careful bad things could happen...
    make this compile lists of devuces, such as legal savegame devices
    
--*/ 
{
    BOOL bRetVal;
    int x = 0;
    int DrivesIndex = 0;
    int UDataDrivesIndex = 0;

    LogText(TitleID,
            hLogFile,
            TEXT("device  type  installed             total           free           used\r\n"));

    for (x = 0; x < 26; x++) {
        _stprintf(TPO[x].DeviceName, TEXT("%c:\\"), DeviceLetters[x]);
        bRetVal = GetDiskFreeSpaceEx((LPCSTR)TPO[x].DeviceName,
                                     &TPO[x].FreeBytesAvailable,
                                     &TPO[x].TotalNumberOfBytes,
                                     &TPO[x].TotalNumberOfFreeBytes);

        if (bRetVal) {
            
            Drives[DrivesIndex] = DeviceLetters[x];

            //
            // take care of this in 1 place, rather than splitting it 
            // between here and some of the other functions that deal with drives
            //

            if ((DeviceLetters[x] == 'U') ||
                (DeviceLetters[x] == 'F') ||
                (DeviceLetters[x] == 'G') ||
                (DeviceLetters[x] == 'H') ||
                (DeviceLetters[x] == 'I') ||
                (DeviceLetters[x] == 'J') ||
                (DeviceLetters[x] == 'K') ||
                (DeviceLetters[x] == 'L') ||
                (DeviceLetters[x] == 'M')) {
                UDataDrives[UDataDrivesIndex++] = DeviceLetters[x];
            }
        }
        
        TPO[x].DriveType = 3; // GetDriveType(TPO[x].DeviceName);

        LogText(TitleID,
                hLogFile,
                TEXT("%s        %d         %s    %12I64d   %12I64d   %12I64d\r\n"), 
                TPO[x].DeviceName,
                TPO[x].DriveType,
                bRetVal ? TEXT("yes "): TEXT("no  "),
                TPO[x].TotalNumberOfBytes,
                TPO[x].TotalNumberOfFreeBytes,
                TPO[x].TotalNumberOfBytes.QuadPart - TPO[x].FreeBytesAvailable.QuadPart);

    }

}

void
MountMUs(
    DWORD TitleID, 
    HANDLE hLogFile
    )
/*++

Routine Description:

    log formatted text

Arguments:

    ULONG TestID
    HANDLE hLogFile

Return Value:

    None
    
Notes

    find and mount all MUs
    I believe the filesystem will now see them when they are present
    We will ignore unmounting them between titles 
    (unless this proves to be a bad thing to do)
    
--*/ 
{
    DWORD insertions;
    unsigned port, slot, i;
    char Drive[10] = {10};
    
    XInitDevices(sizeof(deviceTypes)/sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);

    Sleep(1000);    // time for things to init
    
    insertions = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    
    for(i=0; i<ARRAYSIZE(XMUMasks); i++){
        slot = i<XGetPortCount()?0:1;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i]){
            XMountMU(port, slot, Drive);
        }
    }
}

VOID
WINAPI
StartTimer(
    VOID
    )
/*++

Routine Description:

    Start the test timer and returns 

Arguments:

    None

Return Value:

    None

Notes:

    this uses a global variable so the calling function does not have to 
    maintain state. this is to make this as convienient and easy as possible 
    to use and un-clutter the calling function

--*/ 
{
    QueryPerformanceCounter(&PerformanceCount);
}

DWORD
WINAPI
EndTimer(
    VOID
    )
/*++

Routine Description:

    Ends the test timer and returns the elapsed time in Milliseconds

Arguments:

    None

Return Value:

    elapsed time in Milliseconds

--*/ 
{
    LARGE_INTEGER PerformanceCountFinish;

    QueryPerformanceCounter(&PerformanceCountFinish);
    return(DWORD)((PerformanceCountFinish.QuadPart - PerformanceCount.QuadPart) / (Frequency.QuadPart / 1000));
}

