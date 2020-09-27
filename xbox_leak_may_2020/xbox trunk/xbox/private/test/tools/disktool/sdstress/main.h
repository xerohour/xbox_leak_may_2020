/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    main.h

Abstract:

    This the MAIN SdStress - Console header file.

Author / ported by:

    John Daly (johndaly) porting date 4/29/2000

Revision History:

    initial port: johndaly 4/29/2000
        port from NT

--*/

//
// General definitions
//

namespace SDStressNameSpace {

    #define MAXDRIVES 15
    #define MAXHANDLES 2000
    #define MEGABYTE 1048576
    #define FLOPSIZE 512
    #define SD_MAXDIRS 1024 // list of dirs containing files - only used when there is removable media
    #define SCEN3REDO 32
    #define MAXRETRIES 500
    #define SUBDIRLISTSIZE 300
    #define ESC_POLL_TIME 15000
    #define NO_LOG 0
    #define MAXDISKBLOCKSIZE 131072
    #define HDFLOPCLUSTERS 2371
    #define HD144CLUSTERS 2847
    #define LS120CLUSTERS 61503
    #define KILL_THREADS 0x0000FFFF
    #define STOP_THREADS 0x0F0F0F0F
    #define THREAD_WAIT_MULTIPLIER 3600 * 12
    #define INITIAL_PMTE_RESTART_TIMEOUT 20000
    #define MAX_WAIT_RETRIES 100
    #define MAX_BLOCK_MULTIPLIER 256
    #define EXIT_PROCESS             0
    
    //
    // Time definitions
    //
    
    #define THREAD_INITIAL_SYNC     600000
    #define THREAD_TIMEOUT          30000
    #define VERIFY_THREAD_TIMEOUT   INFINITE   // was 60000, but we were having problems
    #define STD_THREAD_TIMEOUT      INFINITE
    #define COMPLETION_TIMEOUT      60000
    #define TERMINATION_TIMEOUT     240000
    //#define FAILED_GRF_STALL        50000
    #define FAILED_GRF_STALL        1000
    
    //
    // Test type definitions
    //
    
    #define CTF 1
    #define CTF_IOC 2
    #define CTF_MULTIPLE 4
    #define CTF_WFEX 8
    
    //
    // Kernel Debugger switch
    //
    
    #define KDENABLED 1
    
    //
    // Color Definitions
    //
    
    #define ERRORCOLOR 12
    #define NORMALTEXTCOLOR 13 
    #define INVERSENORMALCOLOR 208
    #define DRIVETYPECOLOR 208
    #define DRIVESELECTED 13
    #define CMDLINECOLOR 13
    #define HEADERCOLOR 11
    #define PASSCOLOR 10
    #define CDFILESELECT 9
    #define ABORTCOLOR 8
    #define DEBUGCOLOR 14
    #define CMDCOLOR 11
    #define WARNCOLOR 14
    #define TITLECOLOR 10
    
    //
    // Drive Types
    //
    
    #define SD_FIXED 1
    #define SD_CDROM 2
    #define SD_NETWORK 3
    #define SD_FLOPPY144 4
    #define SD_FLOPPY12 5
    #define SD_LS120 6
    #define SD_REMOVABLE 7
    #define SD_VOID 0
    
    //
    // Error codes
    //
    
    #define E_CDROMCOPY 12
    #define E_COPY 4
    #define E_TESTFILE 1
    #define E_MEMORY 2
    #define E_VERIFY 3
    #define E_VERIFYREAD 5
    #define E_VERIFYCOPY 6
    #define E_NOHANDLE 7
    #define E_DELETE 8
    #define E_DISKSPACE 9
    #define E_ACCESSSdStress 14
    #define E_NOPOINTER 11
    #define E_SEEK 13
    #define E_SDABORT 10
    #define E_DEALLOC 15
    #define E_THREAD 16
    
    struct DISKINFO {
         char Letter;
         UINT Type;
         ULARGE_INTEGER TotalFreeSpace;
         ULARGE_INTEGER TotalNumberOfBytes;
         ULARGE_INTEGER TotalNumberOfFreeBytes;
         DWORD BytesPerSector;
         DWORD TestFileSize;
         long NumberOfFiles;
    };
    
    struct THREAD_DATA 
    {
         DISKINFO *SrcDrive;
         DISKINFO *DestDrive;
         UINT ThreadInstance;
         BOOL *ThreadExist;
         char TestType;
         long Passes;
         UINT Count;
    };
    
    struct COPYTESTFILE_DATA {
         char *pszSname;
         char *pszDname;
         DWORD lFileSize;
         DWORD BlockSize;
         DWORD MinimumBlockSize;
         long ErrorCode;
         long retval;
         THREAD_DATA *ThreadData;
         unsigned char *pszCompareBuffer;
         unsigned char *pszDataBuffer;
         unsigned Offset;
    };
    
    //
    // Global Data
    //
    
    HANDLE hSdStressLog;
    unsigned char *pszDataImage;
    DWORD FileMode;
    UINT BlockSizeMultiplier;
    char TestScenarios;
    ULONG SpindleSpeed;
    BOOL bDebug;
    BOOL bRandomize;
    UCHAR DriveSelected[MAXDRIVES+1];
    HANDLE ThreadHandles[MAXHANDLES+1] = {0};
    BOOL ThreadExist[MAXHANDLES+1];
    ULONG Passes;
    ULONG CDThreads;
    ULONG DiskThreads;
    ULONG TestFileSize;
    LONG TotalStartedThreadCount;
    LONG TotalFinishedThreadCount;
    LONG MaximumThreadCount;
    LONG FixedDiskThreadStillRunning;
    ULONG TotalNumberHardDrives;
    ULONG TotalNumber144Drives;
    ULONG TotalNumber12Drives;
    ULONG TotalNumberLS120Drives;
    ULONG TotalNumberRemovableDrives;
    ULONG TotalNumberNetDrives;
    ULONG TotalNumberCDROMDrives;
    HINSTANCE NtLog;
    CRITICAL_SECTION _GetRandomFile;
    CRITICAL_SECTION _WriteTextConsole;
    BOOL bPmte;
    LPVOID TestMgr;
    char *pszSubDirList[SD_MAXDIRS] = {0};
    // struct _CONSOLE_SCREEN_BUFFER_INFO ConsoleDefaultState;
    struct DISKINFO *DiskInfo[MAXDRIVES+1] = {0};
    char g_LogFileName[100] = {0};

    #define MAX_DEPTH (_MAX_DIR / 2) // a/b/c ...
    struct _finddata_t FileInfo[MAX_DEPTH] = {0};
    BYTE FileFoundFlag[MAX_DEPTH] = {0};
    __int3264 hFile[MAX_DEPTH] = {0};
    DWORD GlobalErrorCounter = 0;

}   //SDStressNameSpace

using namespace SDStressNameSpace;

void 
InitVaribles(
    void
    );

void 
AnalyzeCmdLine(
    void
    );

BOOL 
AnalyzeDrives(
    void
    );

BOOL 
DeleteDiskInfo (
    DISKINFO *DiskInfo
    );

BOOL 
CreateTestFiles(
    void
    );

BOOL 
ValidateTestConfiguration(
    void
    );

BOOL 
AllocGetRandomFile (
    void
    );

void 
DeallocGetRandomFile (
    void
    );

void 
CreateDiskInfo (
    UCHAR DriveLetter, 
    INT count
    );

void 
StartThreads (
    char TestType
    );

BOOL 
ValidateCOPYTESTFILE_DATA (
    COPYTESTFILE_DATA *pTestFileData
    );

VOID
WINAPI
SetSpindleSpeed(
    VOID
    );



