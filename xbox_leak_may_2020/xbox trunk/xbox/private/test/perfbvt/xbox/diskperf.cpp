/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    diskperf.cpp

Abstract:

    Disk related perf BVT tests

--*/

#include "precomp.h"

//
// This is a hidden test function to set file cache size
//
DECLARE_TEST_FUNCTION(SetFileCacheSize)
{
    UINT fileCacheSize = testparam ? atoi(testparam) : 256;
    if (fileCacheSize && XSetFileCacheSize(fileCacheSize*1024)) {
        Print("File cache size set to %dKB...\n", fileCacheSize);
    }

    return 0;
}


//
// Calculate disk read/write speed (in 0.001 MB/s unit)
//
inline DWORD CalcRWSpeed(DWORD bytecount, DWORD msecs) {
    return (DWORD) ((((ULONGLONG) bytecount) * 1000 * 1000) /
                    (((ULONGLONG) msecs) * 1024 * 1024));
}


#define TESTFILENAME    "z:\\perftest.dat"
#define TESTFILESIZE    (32*1024*1024)
#define FILEBUFSIZE     (32*1024)
#define READBUFSIZE     (32*1024)
#define WRITEBUFSIZE    (8*1024)

#define DISK_FUNCTION_COMMON_PROLOG(mode, flags) \
        ULONG time; \
        HANDLE file; \
        DWORD total=0, count, cb; \
        BOOL ok; \
        VOID* buf = valloc(FILEBUFSIZE); \
        ASSERT(buf != NULL); \
        memset(buf, 0xff, FILEBUFSIZE); \
        time = ReadTimestamp(); \
        file = CreateFile( \
                    testparam ? testparam : TESTFILENAME, \
                    mode, \
                    0, \
                    NULL, \
                    (mode == GENERIC_WRITE) ? CREATE_ALWAYS : OPEN_EXISTING, \
                    FILE_ATTRIBUTE_NORMAL|flags, \
                    NULL); \
        ASSERT(file != INVALID_HANDLE_VALUE)

#define DISK_FUNCTION_COMMON_EPILOG() \
        time = ReadTimestamp() - time; \
        CloseHandle(file); \
        vfree(buf); \
        LogEntry(curtestname, CalcRWSpeed(total, time), "MB/s"); \
        return 0

//
// Write disk file sequentially, in variable size blocks
//
DECLARE_TEST_FUNCTION(DiskWriteVariable)
{
    // Fix the random number sequence
    Srand('DSKW');

    DISK_FUNCTION_COMMON_PROLOG(GENERIC_WRITE, FILE_FLAG_SEQUENTIAL_SCAN);

    do {
        count = Rand() % WRITEBUFSIZE + 1;
        ok = WriteFile(file, buf, count, &cb, NULL) && (cb == count);
        ASSERT(ok);
        total += count;
    } while (total < TESTFILESIZE);

    DISK_FUNCTION_COMMON_EPILOG();
}


//
// Write a 32MB disk file seqentially, and in 8KB blocks
//
DECLARE_TEST_FUNCTION(DiskWrite8KBlock)
{
    DISK_FUNCTION_COMMON_PROLOG(
        GENERIC_WRITE,
        FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_WRITE_THROUGH|FILE_FLAG_NO_BUFFERING);

    // Set file size first
    //  This reduces disk access to update the FAT directory

    DWORD pos1 = SetFilePointer(file, TESTFILESIZE, NULL, FILE_BEGIN);
    ok = SetEndOfFile(file);
    DWORD pos0 = SetFilePointer(file, 0, NULL, FILE_BEGIN);
    ASSERT(pos1 != 0xffffffff && ok && pos0 != 0xffffffff);

    do {
        count = WRITEBUFSIZE;
        ok = WriteFile(file, buf, count, &cb, NULL) && (cb == count);
        ASSERT(ok);
        total += count;
    } while (total < TESTFILESIZE);

    DISK_FUNCTION_COMMON_EPILOG();
}


//
// Read a 32MB file sequentially, in 32KB blocks
//
DECLARE_TEST_FUNCTION(DiskReadSeq32KBlock)
{
    DISK_FUNCTION_COMMON_PROLOG(
        GENERIC_READ,
        FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_NO_BUFFERING);

    do {
        count = READBUFSIZE;
        ok = ReadFile(file, buf, count, &cb, NULL) && (cb == count);
        ASSERT(ok);
        total += count;
    } while (total < TESTFILESIZE);

    DISK_FUNCTION_COMMON_EPILOG();
}


//
// Read a 32MB file sequentially, in variable size blocks
//
DECLARE_TEST_FUNCTION(DiskReadSeqVariable)
{
    DISK_FUNCTION_COMMON_PROLOG(GENERIC_READ, FILE_FLAG_SEQUENTIAL_SCAN);

    do {
        count = Rand() % READBUFSIZE + 1;
        if (count > TESTFILESIZE-total)
            count = TESTFILESIZE-total;
        ok = ReadFile(file, buf, count, &cb, NULL) && (cb == count);
        ASSERT(ok);
        total += count;
    } while (total < TESTFILESIZE);

    DISK_FUNCTION_COMMON_EPILOG();
}


//
// Read a file using random access, in variable size blocks
//
DECLARE_TEST_FUNCTION(DiskReadRandomAccess)
{
    DISK_FUNCTION_COMMON_PROLOG(GENERIC_READ, 0);

    do {
        DWORD pos = Rand() % TESTFILESIZE;
        pos = SetFilePointer(file, pos, NULL, FILE_BEGIN);
        ASSERT(pos != 0xffffffff);

        count = Rand() % READBUFSIZE + 1;
        if (count > TESTFILESIZE-pos)
            count = TESTFILESIZE-pos;
        ok = ReadFile(file, buf, count, &cb, NULL) && (cb == count);
        ASSERT(ok);
        total += count;
    } while (total < TESTFILESIZE);

    DISK_FUNCTION_COMMON_EPILOG();
}


//
// File allocation test
//  win2k-1: < 0.1s
//  win2k-2: 76+s
//  xbox: 0.2 - 1.3s (depending on what's on the drive)
//
#define MIN_FILEALLOC_SIZE (128*1024*1024)
#define MAX_FILEALLOC_SIZE (384*1024*1024)

DECLARE_TEST_FUNCTION(FileAllocation)
{
    Srand('FATA');
    ULONG starttime = ReadTimestamp();

    CHAR* testfilename = testparam ? testparam : TESTFILENAME;
    UINT loopcnt = 5;
    while (loopcnt--) {
        HANDLE file = CreateFile(
                        testfilename,
                        GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,
                        NULL);
        ASSERT(file != INVALID_HANDLE_VALUE);

        DWORD fileoffset;
        DWORD filesize = MIN_FILEALLOC_SIZE +
                         Rand() % (MAX_FILEALLOC_SIZE-MIN_FILEALLOC_SIZE+1);

        fileoffset = SetFilePointer(file, filesize, NULL, FILE_BEGIN);
        ASSERT(fileoffset == filesize);

        BOOL seteofok = SetEndOfFile(file);
        ASSERT(seteofok);

        CloseHandle(file);
    }

    DeleteFile(testfilename);
    return ReadTimestamp() - starttime;
}


//
// Directory and file enumeration tests
//
#define DEFAULT_TESTDIR "z:\\perftest.dir"
#define NUM_TESTFILES 2048

VOID GenTestFilename(CHAR* buf, UINT index) {
    static const CHAR hexdigits[] = "0123456789abcdef";
    for (INT digit=0; digit < 8; digit++) {
        *buf++ = hexdigits[index&0xf];
        index = index >> 4;
    }
    *buf++ = '\0';
}

#define DIRECTORY_FUNCTION_COMMON_PROLOG() \
        CHAR* testdir = testparam ? testparam : DEFAULT_TESTDIR; \
        INT testdirlen = strlen(testdir); \
        ASSERT(testdirlen < MAX_PATH-10); \
        CHAR testfilename[MAX_PATH]; \
        CHAR* p = testfilename; \
        strcpy(p, testdir); \
        p += testdirlen; \
        *p++ = '\\'; \
        BOOL ok; \
        UINT i; \
        HANDLE file; \
        DWORD filesize, cb; \
        VOID* buf = valloc(FILEBUFSIZE); \
        ASSERT(buf != NULL); \
        ULONG starttime = ReadTimestamp()

#define DIRECTORY_FUNCTION_COMMON_EPILOG() \
        ULONG stoptime = ReadTimestamp(); \
        vfree(buf); \
        return (stoptime - starttime)

//
// Create the test directory
// and then create 4096 files of sizes between 0 and 4KB
//
//  win2k-1: 18+s
//  win2k-2: 27+s
//  xbox: 280+s (with default file cache size 64KB)
//      ~11s (with file cache size set to 512KB)
//
DECLARE_TEST_FUNCTION(CreateDirAndFiles)
{
    Srand('DIRS');

    DIRECTORY_FUNCTION_COMMON_PROLOG();

    ok = CreateDirectory(testdir, NULL);
    ASSERT(ok);

    for (i=0; i < NUM_TESTFILES; i++) {
        GenTestFilename(p, i);
        file = CreateFile(
                    testfilename,
                    GENERIC_WRITE,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,
                    NULL);
        ASSERT(file != INVALID_HANDLE_VALUE);

        filesize = Rand() % (WRITEBUFSIZE+1);
        ok = WriteFile(file, buf, filesize, &cb, NULL);
        ASSERT(ok && cb == filesize);

        CloseHandle(file);
    }

    DIRECTORY_FUNCTION_COMMON_EPILOG();
}

//
// Enumerate the test directory and open/read each file
//  win2k-1: 37+s (after fresh reboot)
//  win2k-2: 5.5 - 7+s (after fresh reboot)
//  xbox: ~8s
//
DECLARE_TEST_FUNCTION(EnumDirAndReadFiles)
{
    DIRECTORY_FUNCTION_COMMON_PROLOG();

    HANDLE findhandle;
    WIN32_FIND_DATA finddata;

    strcpy(p, "*");
    findhandle = FindFirstFile(testfilename, &finddata);
    ASSERT(findhandle != INVALID_HANDLE_VALUE);

    i = 0;
    do {
        if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        strcpy(p, finddata.cFileName);
        file = CreateFile(
                    testfilename,
                    GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
        ASSERT(file != INVALID_HANDLE_VALUE);

        filesize = GetFileSize(file, NULL);
        ASSERT(filesize <= WRITEBUFSIZE);

        ok = ReadFile(file, buf, filesize, &cb, NULL);
        ASSERT(ok && cb == filesize);

        CloseHandle(file);
        i++;
    } while (FindNextFile(findhandle, &finddata));
    ASSERT(i == NUM_TESTFILES);

    FindClose(findhandle);
    DIRECTORY_FUNCTION_COMMON_EPILOG();
}
    
//
// Remove the test directory and all files underneath it
//  win2k-1: ~2.6s
//  win2k-2: ~1s
//  xbox: ~2.4s
//
DECLARE_TEST_FUNCTION(DeleteDirAndFiles)
{
    DIRECTORY_FUNCTION_COMMON_PROLOG();

    HANDLE findhandle;
    WIN32_FIND_DATA finddata;

    strcpy(p, "*");
    findhandle = FindFirstFile(testfilename, &finddata);
    ASSERT(findhandle != INVALID_HANDLE_VALUE);

    i = 0;
    do {
        if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        strcpy(p, finddata.cFileName);
        ok = DeleteFile(testfilename);
        ASSERT(ok);
    } while (FindNextFile(findhandle, &finddata));
    FindClose(findhandle);

    ok = RemoveDirectory(testdir);
    ASSERT(ok);

    DIRECTORY_FUNCTION_COMMON_EPILOG();
}

//
// Streaming files off DVD disk
//
#define BLOCK_SIZE      (2*1024)
#define SAMPLE_INTERVAL (256*1024*1024)
#define SAMPLE_PERIOD   (8*1024*1024)

BOOL DvdStreamFile(CHAR* filename, VOID* buf, UINT bufsize)
{
    ASSERT(bufsize % BLOCK_SIZE == 0);
    ASSERT(SAMPLE_INTERVAL % bufsize == 0);
    ASSERT(SAMPLE_PERIOD % bufsize == 0);

    HANDLE file = CreateFile(
                    filename,
                    GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL|
                        FILE_FLAG_NO_BUFFERING|
                        FILE_FLAG_SEQUENTIAL_SCAN,
                    NULL);

    if (file == INVALID_HANDLE_VALUE)
        return FALSE;

    LogEntry(filename, 0, "");

    BOOL done = FALSE;
    DWORD block = 0;

    while (TRUE) {
        // Seek to the beginning of the current block and
        // read a little data first to take the seek time out of consideration.
        LARGE_INTEGER pos;
        pos.QuadPart = (LONGLONG) block * bufsize;
        pos.LowPart = SetFilePointer(file, pos.LowPart, &pos.HighPart, FILE_BEGIN);
        if (pos.LowPart == 0xffffffff &&
            GetLastError() != NO_ERROR)
            break;

        DWORD cb;
        ReadFile(file, buf, bufsize, &cb, NULL);

        DWORD time = ReadTimestamp();
        DWORD count = SAMPLE_PERIOD / bufsize;
        while (count--) {
            if (!ReadFile(file, buf, bufsize, &cb, NULL) || cb != bufsize) {
                done = TRUE;
                break;
            }
        }
        if (done) break;

        time = ReadTimestamp() - time;

        CHAR msg[16];
        sprintf(msg, "%d", block * (bufsize / BLOCK_SIZE));

        LogEntry(msg, CalcRWSpeed(SAMPLE_PERIOD, time), "MB/s");
        block += SAMPLE_INTERVAL / bufsize;
    }

    CloseHandle(file);
    return TRUE;
}

DECLARE_TEST_FUNCTION(DvdStreamingTest)
{
    UINT bufsize = 16;
    LogEntry("DvdStreamingTest", bufsize*1000, "KB");
    bufsize *= 1024;

    VOID* buf = valloc(bufsize);
    ASSERT(buf != NULL);

    if (testparam) {
        // Stream files off DVD
        INT fileno = 1;
        CHAR filename[32];
        do {
            sprintf(filename, "a:\\video_ts\\vts_01_%d.vob", fileno++);
        } while (DvdStreamFile(filename, buf, bufsize));
    } else {
        // Raw disk access
        DvdStreamFile("cdrom0:", buf, bufsize);
    }

    vfree(buf);
    return 0;
}

