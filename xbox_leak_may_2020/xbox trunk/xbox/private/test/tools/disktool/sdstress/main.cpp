/*++
SdStress - Console version
main.cpp
This the MAIN SdStress - C file.

Copyright (c) 1996, 1997  Microsoft Corporation

Module Name:

   SdStress - main.cpp

Abstract:

   SdStress - Storage Device Stress Utility

Author:

   Vincent Geglia (VincentG)
   ported to X-Box by John Daly

Notes:

Revision History:

1.9 - added WriteFileEx scenario

1.8 - ported to xbox

1.7 - Removed PMTE support and fixed bug in data corruption detection routine

1.63 - Removed limit on /T switch

1.62 - Changed limit on /T switch to 1000 instead of 10
       Fixed bug with code path when writing the test file fails

1.61 - Fixed regression bug with /B switch

1.6  - Fixed bug where sector size is hardcoded to 512.  Now, I look at both drives, and
       set the minimum block size to that of the device with the larger granularity

1.5  - Fixed handle leak problem on scenario 2
       Added string table for title
       Added structured exception handling for scenario 3

1.4  - Fixed bug running under Win9x where SdStress AVs when WFSO fails
       Fixed bug running under Win9x where SdStress AVs when CTRL-BREAK is hit

       (It seems that Win95 behavior for CreateThread and TerminateThread differs
       from NT in the way that Win95 may return a handle, but it may not be validated
       immediately, especially if the system is under s tremendous amount of stress. In
       addition, NT terminates all children of a parent thread when TerminateThread is
       called, but Win95 doesn't do this - result is AV with old design.  SdStress now
       simply kills the app in Win95 without cleaning up.  This is a dirty fix, but
       then again Win95 won't be around forever  :->  )

1.3  - Fixed bug in GetNumberOfFiles and GetRandomFile functions
       Added SetErrorMode so we don't see those annoying dialog boxes talking about missing
         disks, CD-ROMs, etc.
       Fixed bug in loop where CD threads run forever if only other drive is removable

1.2  - Added /LOG: switch to change logfile name.
       Added /PMTE switch to get events from DieterA's PM test shell
       Removed /JD switch in favor of /PTME
       Added Structured Exception Handling
       Optimized SDTHREAD.C by removing some code repetition
       Changed KillAllThreads to NOT force-terminate worker threads - Windows does this for me
       Removed bAbortThreads global variable.  It is not necessary to use this method
         because of the same reason above - Windows kills all my threads for me
       Created 3 new source files (ctf.c, ctfioc.c, and ctfmul.c) - these new files
         contain scenario 1, 2 and 3, respectively.
       Fixed insufficient disk space bug

1.1  - Added /JD switch for JoeDai (thread pause after x seconds)
       Change status reading for CD-ROM so it doesn't look like a bug
       Added thread synchronization when we start, with a timeout in case something goes awry
       Added a check to make sure we don't have too many threads running
       "Quieted output" (moved some output to DEBUG output)
       Set a maximum test file size of 10MB
       Changed shared variables that use -- and ++ to InterlockedDecrement and InterlockedIncrement
       Increased number of concurrent threads to 2000 (instead of MAXIMUM_WAIT_OBJECTS)
       Added logging from "TerminateThread" so ABORTs don't look like PASS

1.0  - Disabled Test Scenario 2 under Win95 - it's not supported
       Added checking on all Waits
       Added CTRL-C handler to call KillAllThreads ()
       Disabled Scenario 2 for if OS != WinNT

0.7  - Fixes as per code review:
       DeallocGetRandomFile () - changed retval to void
       KillAllThreads () - Added WaitForMultipleObjects - to allow a 30 second timeout, if
                           threads are shut down by then, TerminateThread is called.
       All exits from main do the necessary cleanup
       Moved InitThread to StartThreads
       Change DISKBLOCKSIZE, CDBLOCKSIZE to same naming convention
       defined 131072 as MAXDISKBLOCKSIZE
       Added CreateDiskInfo for DiskInfo structure creating, and validating
       Added #define for total clusters for different media
       Changed all SEV1 to SEV2 logging

0.61 - Fixed GetRandomFile function problems again, also changed random seed generation algorithm.
       Added <ESC> key polling every 15 seconds
       Added Cache Write Through switch
       Write CD file selection to screen

0.6  - Fixed the CPU hogging bug by replacing if (kbhit) with WaitForMultipleObject.  The <ESC> key no longer functions.
       Fixed GetRandomFile function problems (hanging up when file size = 0 is selected)
       Raised GetRandomFile retry count from 15 to 500.

--*/

//
// General Includes
//

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <ntddcdrm.h>

#include <xtl.h>
#include <stddef.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <xtestlib.h>
#include <xlog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>


#include "main.h"

#include "resource.h"

//
// SdStress_C dependencies
//

HANDLE SdStressHeapHandle;

#include "sdstress.h"
#include "console.c"
#include "sdthread.c"

void
InitVaribles(
    void
    )
/*++

Routine Description:

    This function initializes the application's variables

Arguments:

    None

Return Value:

    None

Notes:
    ini file options:
    B=n - Specifies block size (multiples of 512, maximum 128K)
    C=n - Specify the CD thread multiplier
    DEBUG={0|1} - Breaks into debugger when error occurs
    D=n - Specify the Disk thread multiplier
    P=n - Specify the number of passes (per thread)
    R={0|1} - Randomize block size
    S=l - Selects the specified drive for stress
    T=n - Test file size (in megabytes)
    WT={0|1} - Perform I/O operations using cache write through
    Z1={0|1} - Perform first test scenario (CopyTestFile)
    Z2={0|1} - Perform second test scenario (CopyTestFileUsingIoC)
    Z3={0|1} - Perform third test scenario (CopyTestFileMultiple)
    Z4={0|1} - Perform fourth test scenario (CopyTestFileUsingWriteFileEx)
    ZA={0|1} - Perform ALL test scenarios
    SS={0|1|2} - sets the spindle speed of the DVD

    Usage: [SDSTRESS]
       S=CDEF
       R=1
       ZA=1
    The above example will stress drives C,D,E, and F using a
    randomized block size, and all scenarios.

--*/ 
{
    int count;

    //
    // BUGBUG
    // don't check this in!
    //

//    XSetFileCacheSize( 2048 * 4096 );
    
    //
    // Zero DiskInfo structures, DriveSelected arrays
    //

    for ( count = 0; count <= MAXDRIVES; count++ ) {
        DriveSelected[count]  =  0;
        DiskInfo[count]  =  0;
    }

    //
    // Zero Thread existance array, and thread handle arrays
    //

    for ( count = 0; count <= MAXHANDLES; count++ ) {
        ThreadExist[count]  =  FALSE;
        ThreadHandles[count]  =  0;
    }

    //
    // Zero global variables
    //

    TotalStartedThreadCount = 0;
    TotalFinishedThreadCount = 0;
    FixedDiskThreadStillRunning = 0;
    TotalNumberHardDrives = 0;
    TotalNumber144Drives = 0;
    TotalNumber12Drives = 0;
    TotalNumberLS120Drives = 0;
    TotalNumberNetDrives = 0;
    TotalNumberCDROMDrives = 0;
    TotalNumberRemovableDrives = 0;
    MaximumThreadCount = 0;
    bPmte = FALSE;

    //
    // Set test defaults
    //

    bDebug = 0;
    bRandomize = 0;
    TestScenarios = 0;
    Passes = 1;
    CDThreads = 1;
    DiskThreads = 1;
    TestFileSize = 1 * MEGABYTE;
    BlockSizeMultiplier = 1;
    FileMode = 0;

}

void
AnalyzeCmdLine(
    void
    )
/*++

Routine Description:

    This function is for determining which Command Line switches
    have been selected.

Arguments:

    argc - "argc" passed from main()
    argv - "argv" passed from main()

Return Value:

    None

--*/ 
{
    #define BuffLen 200
    char iniBuffer[BuffLen] = {0};
    unsigned char count;

    //
    // logging stuff
    //

    xSetComponent(hSdStressLog, "disk", "sdstress" );
    xSetFunctionName( hSdStressLog, "AnalyzeCmdLine" );
    xStartVariation( hSdStressLog, "StartTest" );

    xLog (hSdStressLog, 
          XLL_PASS, 
          "Analyzing Command line parameters");

    //
    // Enable debugger output for SdStress
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "DEBUG",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/DEBUG switch specified - DEBUG mode active");
        bDebug = TRUE;
    }

    //
    // Check for Randomize block size switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "R",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/R switch specified - Randomizing block sizes");
        bRandomize = TRUE;
    }

    //
    // Check Disk thread multiplier switch
    //

    DiskThreads = GetPrivateProfileIntA("sdstress",
                                        "D",
                                        0,
                                        "testini.ini");

    xLog (hSdStressLog, 
          XLL_INFO, 
          "/D switch specified - %d simultaneous disk thread(s) selected",
          DiskThreads);

    //
    // Check CD Thread multiplier switch
    //

    CDThreads = GetPrivateProfileIntA("sdstress",
                                      "C",
                                      0,
                                      "testini.ini");
    xLog (hSdStressLog, 
          XLL_INFO, 
          "/C switch specified - %d simultaneous CD thread(s) selected",
          CDThreads);

    //
    // Check TestFileSize switch
    //

    TestFileSize = GetPrivateProfileIntA("sdstress",
                                         "T",
                                         1,
                                         "testini.ini");
    TestFileSize *= MEGABYTE;
    xLog (hSdStressLog, 
          XLL_INFO, 
          "/T switch specified - New test file size is %ld bytes",
          TestFileSize);

    //
    // Check NumberOfPasses switch
    //

    Passes = GetPrivateProfileIntA("sdstress",
                                   "P",
                                   1,
                                   "testini.ini");
    xLog (hSdStressLog, 
          XLL_INFO, 
          "/P switch specified - Number of test passes is %d",
          Passes);

    //
    // Check copy block size switch
    //

    BlockSizeMultiplier = GetPrivateProfileIntA("sdstress",
                                                "B",
                                                1,
                                                "testini.ini");

    if ( BlockSizeMultiplier && bRandomize ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/R switch already specified, ignoring /B switch.");
        BlockSizeMultiplier = 1;
    } else if ( BlockSizeMultiplier > MAX_BLOCK_MULTIPLIER ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/B switch specified, but illegal block multiplier.  Ignoring...");
        BlockSizeMultiplier = 1;
    } else {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/B switch specified - Block size multiplier is set to %d",
              BlockSizeMultiplier);
    }

    //
    // Drive selection switch - selection go into the DriveSelected array
    //

    if ( GetPrivateProfileStringA("sdstress",
                                  "S",
                                  "c",
                                  iniBuffer,
                                  BuffLen,
                                  "testini.ini"
                                 ) ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/S switch specified, the following drives are selected for stress:");
        //
        // _strupr was broken, so use this hack
        //

        char *cp;
        for ( cp = iniBuffer; *cp; ++cp ) {
            if ( 'a' <= *cp && *cp <= 'z' )
                *cp += 'A' - 'a';
        }
        count = 0;
        while ( iniBuffer[count] ) {
            if ( iniBuffer[count] >= 'A' && iniBuffer[count] <= 'Z' ) {
                DriveSelected[count] = iniBuffer[count];
                xLog (hSdStressLog, 
                      XLL_INFO, 
                      "--> Drive %c: is selected.", iniBuffer[count]);
            }
            count++;
        }
    }

    //
    // Force test scenario #1 switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "Z1",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/Z1 switch specified, test scenario one is selected (CopyTestFile)");
        TestScenarios = TestScenarios | CTF;
    }

    //
    // Force test scenario #2 switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "Z2",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/Z2 switch specified, test scenario two is selected (CopyTestFileUsingIoC)");
        TestScenarios = TestScenarios | CTF_IOC;
    }

    //
    // Force test scenario #3 switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "Z3",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/Z3 switch specified, test scenario three is selected (CopyTestFileMultiple)");
        TestScenarios = TestScenarios | CTF_MULTIPLE;
    }

    //
    // Force test scenario #4 switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "Z4",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/Z4 switch specified, test scenario four is selected (CopyTestFileUsingWriteFileEx)");
        TestScenarios = TestScenarios | CTF_WFEX;
    }

    //
    // Use all test scenarios switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "ZA",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/ZA switch specified, all test scenarios are selected");
        TestScenarios = TestScenarios | 0xff;
    }

    //
    // FILE_FLAG_WRITE_THROUGH switch
    //

    if ( GetPrivateProfileIntA("sdstress",
                               "WT",
                               0,
                               "testini.ini") ) {
        xLog (hSdStressLog, 
              XLL_INFO, 
              "/WT switch specified, I/O performed with cache write through");
        FileMode = FILE_FLAG_WRITE_THROUGH;
    }

    //
    // Spindle Speed switch
    //

    SpindleSpeed = GetPrivateProfileIntA("sdstress",
                               "SS",
                               0,
                               "testini.ini");
    
    xLog (hSdStressLog, 
          XLL_INFO, 
          "/SS switch specified, Spindle Speed set to : %d", 
          SpindleSpeed);

    //
    // close this variation
    //

    xEndVariation( hSdStressLog);

}

BOOL
AnalyzeDrives(
    void
    )
/*++

Routine Description:

    This function will analyze all of the selected drives, and gather
    information about them. This function will also toss out drives that
    are not valid.

Arguments:

    None

Return Value:

    None

--*/ 
{
    int count;
    char szBuffer[100];
    DWORDLONG dwlRequiredSpace = 0L;

    //
    // Set up prototype variable
    //

    WriteTextConsole ("\nDetermining disk types:\n", HEADERCOLOR, XLL_INFO);

    //
    // Cycle through DriveSelected to see which drive letters have been selected
    //

    for ( count = 0;count <= MAXDRIVES; count ++ ) {

        //
        // See if drive is valid (0 = not valid)
        //

        if ( DriveSelected[count] ) {

            //
            // Get drive information, allocate and fill DISKINFO structure for each drive.
            //

            CreateDiskInfo (DriveSelected[count], count);
        }
    }

    //
    // Determine disk space requirements.
    //

    WriteTextConsole("\nDetermining disk space requirements:\n", HEADERCOLOR, XLL_INFO);

    for ( count = 0; count <= MAXDRIVES; count ++ ) {
        if ( (DiskInfo[count]) && DiskInfo[count]->Type ) {
            switch ( DiskInfo[count]->Type ) {
                
                case SD_FIXED:
                case SD_REMOVABLE:
                case SD_LS120:
                case SD_NETWORK:
                    {

                        dwlRequiredSpace = TestFileSize +
                                           (DiskThreads * (TestFileSize) * (TotalNumberHardDrives)) +
                                           ((TotalNumber12Drives+TotalNumber144Drives) * FLOPSIZE * DiskThreads) +
                                           ((TotalNumberLS120Drives) * DiskThreads * (TestFileSize)) +
                                           ((TotalNumberCDROMDrives) * CDThreads * (1 * MEGABYTE));

                        break;
                    }

                case SD_CDROM:
                    {
                        dwlRequiredSpace = 0;
                        break;
                    }

                case SD_FLOPPY12:
                case SD_FLOPPY144:
                    {
                        dwlRequiredSpace = FLOPSIZE +
                                           (DiskThreads * FLOPSIZE * TotalNumberHardDrives) +
                                           ((TotalNumber12Drives + TotalNumber144Drives) * FLOPSIZE * DiskThreads) +
                                           ((TotalNumberLS120Drives) * DiskThreads * (TestFileSize)) +
                                           ((TotalNumberCDROMDrives) * CDThreads * 2048);  // Hardcoding 2048 is bad
                    }
                default:
                    {
                        break;
                    }
            }

            //
            // Show available / required space comparison
            //

            sprintf (szBuffer,
                     "Drive %c: - Required space is %I64d\n",
                     DiskInfo[count]->Letter,
                     dwlRequiredSpace);

            WriteTextConsole (szBuffer, NORMALTEXTCOLOR, FALSE);

            //
            // was BUGBUG - this check turned off until tracy fixes the filesystem
            // turned back on - make sure it works
            //

            if ( dwlRequiredSpace > DiskInfo[count]->TotalFreeSpace.QuadPart ) {
                sprintf (szBuffer, "Drive %c: does not have enough free space.  Ignoring.\n", DiskInfo[count]->Letter);
                WriteTextConsole (szBuffer, ERRORCOLOR, XLL_WARN);
                sprintf (szBuffer, "Drive %c: free space:%I64d  Required space:%I64d \n",
                         DiskInfo[count]->Letter,
                         DiskInfo[count]->TotalFreeSpace.QuadPart,
                         dwlRequiredSpace);
                WriteTextConsole (szBuffer, ERRORCOLOR, XLL_WARN);
                if ( !DeleteDiskInfo (DiskInfo[count]) ) {
                    return(FALSE);
                }
            }
        }
    }


    //
    // Check to make sure current disk configuration is valid
    //

    return(ValidateTestConfiguration ());
}

BOOL
DeleteDiskInfo (
    DISKINFO *DskInfo
    )
/*++

Routine Description:

    This function deletes a drive's info structure.

Arguments:

    DISKINFO structure - Contains information about drive

Return Value:

    Value from ValidateTestConfiguration ()

--*/ 
{

    //
    // Update the tally of drive types
    //

    switch ( DskInfo->Type ) {
        case SD_CDROM:
            TotalNumberCDROMDrives --;
            break;
        case SD_FIXED:
            TotalNumberHardDrives --;
            break;
        case SD_NETWORK:
            TotalNumberNetDrives --;
            break;
        case SD_FLOPPY144:
            TotalNumber144Drives --;
            break;
        case SD_FLOPPY12:
            TotalNumber12Drives --;
            break;
        case SD_LS120:
            TotalNumberLS120Drives --;
            break;
        case SD_REMOVABLE:
            TotalNumberRemovableDrives --;
            break;
        default:
            break;
    }

    //
    // Make sure drive configuration is still valid after deleting a drive from the list
    //

    if ( ValidateTestConfiguration () == FALSE ) {
        return(FALSE);
    }

    //
    // Invalidated drives get a type of SD_VOID
    //

    return(TRUE);
}

BOOL
CreateTestFiles(
    void
    )
/*++

Routine Description:

    This function creates the test files on all selected devices.

Arguments:

    None

Return Value:

    TRUE if success
    FALSE if failed

--*/ 
{
    CHAR PathName[50];
    char szBuffer[100];
    BOOL bSuccess;
    UINT count;
    HANDLE hFile;
    long ErrorCode;
    ULONG lCount = 0;
    DWORD dwAccessed;

    xSetComponent(hSdStressLog, "disk", "sdstress" );
    xSetFunctionName( hSdStressLog, "CreateTestFiles" );
    xStartVariation( hSdStressLog, "variation1" );

    //
    // Loop to fill buffer
    //

    while ( lCount < TestFileSize ) {
        pszDataImage[lCount] = rand () % 255;
        lCount++;
    }

    xLog (hSdStressLog, 
          XLL_INFO, 
          "Creating files used for test:");

    //
    // Main loop for test file generation
    //

    for ( count = 0; count <= MAXDRIVES; count ++ ) {

        //
        // IF the DiskInfo structure is valid
        //

        if ( (DiskInfo[count]) && DiskInfo[count]->Type ) {
            sprintf (szBuffer, "Drive %c: - ", DiskInfo[count]->Letter);

            //
            // If the target is CD-ROM, then do not attempt to create a file.
            //

            if ( DiskInfo[count]->Type == SD_CDROM ) {
                xLog (hSdStressLog, 
                      XLL_INFO, 
                      "%sDrive is READ-ONLY (CDROM).  No test file created.",
                      szBuffer);
            }

            //
            // Otherwise, create a test file
            //

            else {
                xLog (hSdStressLog, 
                      XLL_INFO, 
                      "Creating %c:\\SDDATA\\TESTFILE.DAT.",
                      DiskInfo[count]->Letter);

                //
                // Assemble path to test directory
                //

                sprintf (PathName, "%c:\\SDDATA", DiskInfo[count]->Letter);

                //
                // Try to go there.  If unable, try to MAKE the directory
                //

                struct _stat _s;

                if ( _stat (PathName, &_s) && _mkdir (PathName) ) {

                    //
                    // If directory cannot be made, then abort this attempt, and toss out drive.
                    // Retrieve the last error
                    //

                    ErrorCode = GetLastError();

                    xLog (hSdStressLog, 
                          XLL_WARN, 
                          "Drive %c: - Error creating testfile, ignoring drive. GetLastError() = %d",
                          DiskInfo[count]->Letter, 
                          ErrorCode);

                    //
                    // Delete structure pertaining to drive
                    //

                    DeleteDiskInfo (DiskInfo[count]);
                }

                //
                // Append testfile name on the end of the pathname.
                //

                strcat (PathName, "\\TESTFILE.DAT");

                //
                // Create the file, always
                //

                hFile = CreateFile(PathName,
                                   GENERIC_WRITE,
                                   0,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
                                   NULL);

                //
                // If it fails,  toss out the drive.
                //

                if ( hFile == INVALID_HANDLE_VALUE ) {

                    ErrorCode = GetLastError();
                    xLog (hSdStressLog, 
                          XLL_FAIL, 
                          "Drive %c: - Error creating testfile, ignoring drive. GetLastError() = %d",
                          DiskInfo[count]->Letter,
                          ErrorCode);
                    DeleteDiskInfo (DiskInfo[count]);
                    DiskInfo[count] = NULL;
                    break;
                }

                //
                // Write test file to disk
                //

                bSuccess = WriteFile(hFile,
                                     pszDataImage,
                                     DiskInfo[count]->TestFileSize,
                                     &dwAccessed,
                                     NULL);

                //
                // IF it fails, toss out the drive.
                //

                if ( dwAccessed != DiskInfo[count]->TestFileSize || !bSuccess ) {

                    ErrorCode = GetLastError();
                    xLog (hSdStressLog, 
                          XLL_FAIL, 
                          "Drive %c: - Error creating testfile, ignoring drive. GetLastError() = %d",
                          DiskInfo[count]->Letter,
                          ErrorCode);
                    DeleteDiskInfo (DiskInfo[count]);
                    DiskInfo[count] = NULL;
                    break;
                }

                //
                // Flush and Close file
                //

                xLog (hSdStressLog, 
                      XLL_PASS, 
                      "Drive %c: - created testfile, using drive.",
                      DiskInfo[count]->Letter);

                FlushFileBuffers (hFile);
                CloseHandle (hFile);
            }
        }
    }

    xEndVariation( hSdStressLog);
    return(TRUE);
}

BOOL
ValidateTestConfiguration(
    void
    )
/*++

Routine Description:

   This function will verify the drive configuration is acceptable

Arguments:

    None

Return Value:

    TRUE if success
    FALSE if not success

--*/ 
{

    //
    // IF any CD-ROMs exist without a target, the test shall exit
    //

    if ( !TotalNumber144Drives &&
         !TotalNumber12Drives &&
         !TotalNumberLS120Drives &&
         !TotalNumberNetDrives &&
         !TotalNumberRemovableDrives &&
         !TotalNumberHardDrives ) {
        WriteTextConsole ("Invalid test configuration (Drive Selection) \n\n",
                          ERRORCOLOR,
                          XLL_WARN);

        return(FALSE);
    }

    return(TRUE);
}

BOOL
AllocGetRandomFile (
    void
    )
/*++

Routine Description:

    This function allocates the memory used by GetRandomFile

Arguments:

    None

Return Value:

    TRUE if no error
    FALSE if error

--*/ 
{
    int count;

    for ( count = 0; count < SD_MAXDIRS; count ++ ) {

        pszSubDirList[count] = (char *)HeapAlloc(SdStressHeapHandle, HEAP_ZERO_MEMORY, SUBDIRLISTSIZE);

        if ( !pszSubDirList[count] ) {
            return(FALSE);
        }
    }
    return(TRUE);
}

void
DeallocGetRandomFile (
    void
    )
/*++

Routine Description:

    This function deallocates the memory used by GetRandomFile

Arguments:

    None

Return Value:

    None

--*/ 
{
    int count;

    if (NULL == SdStressHeapHandle) {
        return;
    }

    for ( count = 0; count < SD_MAXDIRS; count ++ ) {
        if ( pszSubDirList[count] ) {
            HeapFree (SdStressHeapHandle, 0, pszSubDirList[count]);
        }
    }
}

void
StartThreads (
    char TestType
    )
/*++

Routine Description:

    This function initializes the worker threads

Arguments:

    char TestType

Return Value:

    None

--*/ 
{
    UCHAR SourceCount, DestCount, Instance;
    char Buffer[200];
    THREAD_DATA *pThreadData;
    LONG ThreadCounter = 0, ThreadHandleCounter = 0, counter = 0;
    DWORD ThreadID, ErrorCode, RetryCount;
    int WaitCount = 0;
    DWORD WaitStatus;

    switch ( TestType ) {
        case CTF:
            {
                WriteTextConsole ("** Test Scenario 1 - CopyTestFile **",
                                  HEADERCOLOR,
                                  FALSE);

                break;
            }

        case CTF_IOC:
            {
                WriteTextConsole ("** Test Scenario 2 - CopyTestFileUsingIoC **",
                                  HEADERCOLOR,
                                  FALSE);
                break;
            }

        case CTF_MULTIPLE:
            {
                WriteTextConsole ("** Test Scenario 3 - CopyTestFileMultiple **",
                                  HEADERCOLOR,
                                  FALSE);
                break;
            }

        case CTF_WFEX:
            {
                WriteTextConsole ("** Test Scenario 4 - CopyTestFileUsingWriteFileEx **",
                                  HEADERCOLOR,
                                  FALSE);
                break;
            }

        default:
            {
                WriteTextConsole ("Unknown Test scenario selected.  Ignoring...",
                                  ERRORCOLOR,
                                  FALSE);
                return;
            }
    }

    WriteTextConsole("\nInitializing threads\n", HEADERCOLOR, FALSE);

    //
    // NOTE - this can create a LOT of threads, basically :
    // (source drives) * (destination drives) * (thread multiplier)
    //
    // every thread that gets started here (SdStressThread()) creates 1 worker 
    // thread (the test)
    // this can easily result in hundreds of threads that use a LOT of memory
    // 
    
    //
    // for the number of source drives (all readable drives)
    //

    for ( SourceCount = 0; SourceCount <= MAXDRIVES; SourceCount ++ ) {

        if ( DiskInfo[SourceCount] && DiskInfo[SourceCount]->Type != SD_VOID ) {

            //
            // for the number of destination drives (all writeable drives)
            //

            for ( DestCount = 0; DestCount <= MAXDRIVES; DestCount ++ ) {

                if ( DiskInfo[DestCount] && DiskInfo[DestCount]->Type != SD_CDROM && DiskInfo[DestCount]->Type != SD_VOID ) {

                    if ( DiskInfo[SourceCount]->Type != SD_CDROM ) {
                        ThreadCounter = DiskThreads;
                    } else {
                        ThreadCounter = CDThreads;
                    }

                    //
                    // for the DiskThreads or CDThreads entry in testini.ini...
                    //
                    
                    for ( Instance = 1; Instance <= ThreadCounter; Instance++ ) {

                        //
                        // Make sure we're not exceeding maximum thread count
                        //

                        if ( ThreadHandleCounter >= MAXHANDLES ) {
                            WriteTextConsole ("Maximum number of threads exceeded - Ignoring additional threads.\n",
                                              ERRORCOLOR,
                                              FALSE);

                            goto SyncThreads;
                        }

                        pThreadData = (THREAD_DATA *)HeapAlloc( SdStressHeapHandle, HEAP_ZERO_MEMORY, sizeof(THREAD_DATA));
                        if ( NULL == pThreadData ) {
                            printf ("FATAL ERROR:  Cannot allocate memory.\n\n");
                            goto CleanUpAndExit;
                        }

                        pThreadData->SrcDrive = DiskInfo[SourceCount];
                        pThreadData->DestDrive = DiskInfo[DestCount];
                        pThreadData->ThreadInstance = Instance;
                        pThreadData->ThreadExist = &ThreadExist[ThreadHandleCounter];
                        pThreadData->TestType = TestType;

                        ThreadHandles[ThreadHandleCounter] = CreateThread (NULL,
                                                                           0,
                                                                           &SdStressThread,
                                                                           pThreadData,
                                                                           0,
                                                                           &ThreadID);

                        sprintf (Buffer,
                                 "Thread init'd - Src:%c, Dst:%c, Instance:%d\n",
                                 pThreadData->SrcDrive->Letter,
                                 pThreadData->DestDrive->Letter,
                                 pThreadData->ThreadInstance);

                        WriteTextConsole (Buffer,
                                          CMDLINECOLOR,
                                          FALSE);

                        if ( !ThreadHandles[ThreadHandleCounter] ) {
                            PrintError (GetLastError());
                            WriteTextConsole ("Could not create thread.\n",
                                              ERRORCOLOR,
                                              FALSE);
                        }

                        else {
                            ThreadHandleCounter ++;
                        }
                    }
                }
            }
        }
    }

    SyncThreads:

    WriteTextConsole ("\n",
                      HEADERCOLOR,
                      FALSE);

    //
    // Set MaximumThreadCount (global variable)
    // at this point, we know how many threads we tried to start, 
    // and the started threads have incremented TotalThreadCount
    // spin until ThreadHandleCounter == TotalThreadCount
    //

    MaximumThreadCount = ThreadHandleCounter;

    do {
        sprintf (Buffer,
                 "started %d Threads - : %d Threads reported starting.\n",
                 MaximumThreadCount,
                 TotalStartedThreadCount);
        Debug_Print(Buffer);
        Sleep(2000);
    } while ( MaximumThreadCount > TotalStartedThreadCount );

    Debug_Print ("Worker threads all accounted for.\n");

    RetryCount = 0;

    //
    // wait for all the threads to finish
    // TotalStartedThreadCount = the counter incremeted by the spawned threads
    // TotalFinishedThreadCount = the counter decremented by the spawned threads
    // waits 24 hours by default
    //
    
    while ( TotalStartedThreadCount + TotalFinishedThreadCount ) {
        Sleep (2000);

        RetryCount ++;

        if ( RetryCount > THREAD_WAIT_MULTIPLIER ) {
            WriteTextConsole ("Thread termination timeout expired.  Possible deadlock.\n\n",
                              ERRORCOLOR,
                              FALSE);

            OutputDebugStringA("SDSTRESS will probably crash if you see this, let johndaly know.");
            DebugBreak();
//            break;
        }

    }

    CleanUpAndExit:

    //
    // clean up thread handles
    //

    for ( counter = 0; counter < ThreadHandleCounter; counter++ ) {
        if ( ThreadHandles[counter] ) {
            CloseHandle(ThreadHandles[counter]);
        }
        ThreadHandles[counter] = NULL;
    }

    return;
}

#define MY_DRIVE_FIXED 3
#define MY_DRIVE_CDROM 5

void
CreateDiskInfo (
    UCHAR DriveLetter,
    INT count
    )
/*++

Routine Description:

    This function creates a DiskInfo structure for a particular drive

Arguments:

    UCHAR DriveLetter
    INT count

Return Value:

    None

--*/ 
{
    char szBuffer[200];
    CHAR szPath[50];

    sprintf (szPath,
             "%c:\\",
             DriveLetter);

    DiskInfo[count] = (DISKINFO *)HeapAlloc(SdStressHeapHandle, HEAP_ZERO_MEMORY, sizeof(DISKINFO));

    //
    // this is bad...
    // consider re-writing to fail gracefully
    //

    if ( NULL == DiskInfo[count] ) {
        WriteTextConsole ("Memory Allocation Failure!.\n\n",
                          ERRORCOLOR,
                          XLL_FAIL);
    }

    //
    // Put drive letter in structure
    //

    DiskInfo[count]->Letter = DriveLetter;

    //
    // get sector size
    //

    DiskInfo[count]->BytesPerSector = XGetDiskSectorSize(szPath);

    //
    // Determine drive type
    //
    // WARNING!!!!
    //
    // Using hard coded data from xbox specs
    // yes, I know a little table would be more efficient...
    //

    // assumes you are using the hack to re-map the CD to this drive
    if ( ((L'A' == szPath[0]) || (L'a' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_CDROM;

    // unused
    //if (((L'B' == szPath[0]) || (L'b' == szPath[0]))) DiskInfo[count]->Type = MY_DRIVE_CDROM;

    // assumes you are using the hack to re-map the dafault title partition to this drive
    if ( ((L'C' == szPath[0]) || (L'c' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // this will change in retail configurations
    if ( ((L'D' == szPath[0]) || (L'd' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // should point to 'c:\devkit' - or nothing
    if ( ((L'E' == szPath[0]) || (L'e' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // MUs - little hard disks!
    if ( ((L'F' == szPath[0]) || (L'f' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'G' == szPath[0]) || (L'g' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'H' == szPath[0]) || (L'h' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'I' == szPath[0]) || (L'i' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'J' == szPath[0]) || (L'j' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'K' == szPath[0]) || (L'k' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'L' == szPath[0]) || (L'l' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    if ( ((L'M' == szPath[0]) || (L'm' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    
    // unused
    //if ( ((L'N' == szPath[0]) || (L'n' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;
    //if ( ((L'O' == szPath[0]) || (L'o' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // unused
    //if (((L'P' == szPath[0]) || (L'p' == szPath[0]));
    //if (((L'Q' == szPath[0]) || (L'q' == szPath[0]));
    //if (((L'R' == szPath[0]) || (L'r' == szPath[0]));
    //if (((L'S' == szPath[0]) || (L's' == szPath[0]));

    // title data
    if ( ((L'T' == szPath[0]) || (L't' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // user data
    if ( ((L'U' == szPath[0]) || (L'u' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // unused
    //if (((L'V' == szPath[0]) || (L'v' == szPath[0]));
    //if (((L'W' == szPath[0]) || (L'w' == szPath[0]));

    //can map to useres data region
    if ( ((L'X' == szPath[0]) || (L'x' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // can map to title data region
    if ( ((L'Y' == szPath[0]) || (L'y' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    // utility partition
    if ( ((L'Z' == szPath[0]) || (L'z' == szPath[0])) ) DiskInfo[count]->Type = MY_DRIVE_FIXED;

    //
    // Show user what info SdStress has found.
    //

    //
    // Fill structure with cluster information / free space
    //

    if (DiskInfo[count]->Type != MY_DRIVE_CDROM &&
        !GetDiskFreeSpaceEx(szPath,
                            &DiskInfo[count]->TotalFreeSpace,
                            &DiskInfo[count]->TotalNumberOfBytes,
                            &DiskInfo[count]->TotalNumberOfFreeBytes) ) {

        sprintf (szBuffer,
                 "Error accessing Drive %c. Error : %d Ignoring...",
                 DriveLetter,
                 GetLastError());
        DeleteDiskInfo(DiskInfo[count]);
        DiskInfo[count] = NULL;
        return;
    }

    switch ( DiskInfo[count]->Type ) {
        
        case MY_DRIVE_FIXED:
            {
                TotalNumberHardDrives ++;
                DiskInfo[count]->Type = SD_FIXED;
                sprintf(szBuffer, "Drive %c: is a Fixed Disk", DiskInfo[count]->Letter);
                DiskInfo[count]->TestFileSize = TestFileSize;
                break;
            }

        case MY_DRIVE_CDROM:
            {
                TotalNumberCDROMDrives ++;
                DiskInfo[count]->Type = SD_CDROM;
                sprintf(szBuffer, "Drive %c: is a CD-ROM", DiskInfo[count]->Letter);
                DiskInfo[count]->TestFileSize = 0;

                //
                // Get number of files on the CDROM
                //

                WriteTextConsole ("Retrieving number of files on CDROM...",
                                  NORMALTEXTCOLOR,
                                  FALSE);

                DiskInfo[count]->NumberOfFiles = GetNumberOfFiles(DiskInfo[count]->Letter);

                if ( !DiskInfo[count]->NumberOfFiles ) {
                    WriteTextConsole ("\nNo files on CD! Ignoring...", ERRORCOLOR, FALSE);
                    DeleteDiskInfo (DiskInfo[count]);
                    DiskInfo[count] = NULL;
                }
                break;
            }

            //
            // If drive type cannot be determined, do not use it. Get rid of DiskInfo structure pertaining to that drive.
            //

        default:
            {
                sprintf (szBuffer, "Drive %c: is of an unknown type. Ignoring", DiskInfo[count]->Letter);
                WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                DeleteDiskInfo (DiskInfo[count]);
                break;
            }
    }

    //
    // Show user free space.
    // Also make sure drive wasn't deleted because either not supported or unknown type.
    //

    if ( (DiskInfo[count]) && DiskInfo[count]->Type ) {
        sprintf (szBuffer, "%c with %I64u bytes free space remaining.\n",
                 DiskInfo[count]->Letter,
                 DiskInfo[count]->TotalFreeSpace);

        WriteTextConsole (szBuffer, NORMALTEXTCOLOR, FALSE);
    }

}

VOID
WINAPI
SdStressStartTest(
    HANDLE LogHandle
    )
/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/ 
{
    char szTitle[100] = "SdStress - Storage Device Stress utility, Version ";
    char szBuffer[200];

    //
    // initialize global logging handle
    //

    hSdStressLog = LogHandle;

    __try
    {
        //
        // create a heap
        //

        SdStressHeapHandle = HeapCreate( 0, 0, 0 );

        if ( NULL == SdStressHeapHandle ) {
            OutputDebugString( TEXT("savegame: Unable to create heap\n") );

            __leave;
        }

        //
        // initialize random numbers - be deterministic
        //

        srand ((unsigned) 1);

        //
        // Initialize Variables
        //

        InitVaribles ();

        //
        // This is where it begins, showing app's title, analyzing system components.
        //

        strcat (szTitle, VER_PRODUCTVERSION_STR);
        WriteTextConsole (szTitle, TITLECOLOR, FALSE);

        WriteTextConsole ("OS: Xbox\n", ERRORCOLOR, FALSE);

        //
        // Allocate memory for GetRandomFile function.
        //

        if ( !AllocGetRandomFile () ) {
            WriteTextConsole ("\n\nMemory allocation error!\n",
                              ERRORCOLOR,
                              FALSE);

            __leave;
        }

        //
        // this is kind of a lie, it really looks in the ini file
        //

        AnalyzeCmdLine ();

        pszDataImage = AllocateMemory (TestFileSize);

        if ( !pszDataImage ) {
            WriteTextConsole ("\n\nMemory allocation error!\n",
                              ERRORCOLOR,
                              FALSE);

            __leave;
        }

        if ( !AnalyzeDrives () ) {
            __leave;
        }

        if ( CreateTestFiles () == FALSE ) {
            __leave;
        }

        SetSpindleSpeed();

        //
        // Create the worker threads, if test scenario #1 is selected
        //

        if ( (TestScenarios & CTF) == CTF || !TestScenarios ) {
            StartThreads (CTF);
        }
        
        if ( (TestScenarios & CTF_IOC) == CTF_IOC ) {
            StartThreads (CTF_IOC);
        }
        
        if ( (TestScenarios & CTF_MULTIPLE) == CTF_MULTIPLE ) {
            StartThreads (CTF_MULTIPLE);
        }

        if ( (TestScenarios & CTF_WFEX) == CTF_WFEX ) {
            StartThreads (CTF_WFEX);
        }
    }

    __finally
    {
        //
        // Deallocate memory from GetRandomFile (Check done at function level)
        //

        DeallocGetRandomFile ();

        //
        // Deallocate memory for pszDataImage (Check done at function level)
        //

        DeAllocateMemory (TestFileSize, pszDataImage);

        //
        // check and destroy heap
        //

        if ( SdStressHeapHandle ) {
            HeapDestroy( SdStressHeapHandle );
        }

        //
        // print out error count
        //

        sprintf (szBuffer, "************Total Error Count : GlobalErrorCounter=%d\n", GlobalErrorCounter);
        Debug_Print (szBuffer);


    }
}

VOID
WINAPI
SetSpindleSpeed(
    VOID
    )
/*++

Routine Description:

    Sets the spindle speed of the DVD drive
    
Arguments:

    None

Return Value:

    None

--*/ 
{
    BOOL status;
    HANDLE fileHandle;
    ULONG returned;
    OCHAR    string[100];
    NTSTATUS NTStatus;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    
    soprintf (string, OTEXT("\\Device\\CdRom0"));

    RtlInitObjectString(&VolumeString, string);
    InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

    NTStatus = NtCreateFile(&fileHandle,
                            SYNCHRONIZE|GENERIC_READ,
                            &ObjA,
                            &IoStatusBlock,
                            0,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ, FILE_OPEN,
                            FILE_SYNCHRONOUS_IO_NONALERT);

    if ( NT_SUCCESS(NTStatus)) {

    status = DeviceIoControl(fileHandle,
                             IOCTL_CDROM_SET_SPINDLE_SPEED,
                             &SpindleSpeed,
                             sizeof(SpindleSpeed),
                             NULL,
                             0,
                             &returned,
                             NULL);
    }
    
    //
    // Clean up
    //
    
    if (fileHandle) {
        NtClose(fileHandle);
    }

    return;
}

VOID
WINAPI
SdStressEndTest(
    VOID
    )
/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/ 
{
    OutputDebugString( TEXT("SDSTRESS: EndTest is called\n") );
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( sdstress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( sdstress )
EXPORT_TABLE_ENTRY( "StartTest", SdStressStartTest )
EXPORT_TABLE_ENTRY( "EndTest", SdStressEndTest )
END_EXPORT_TABLE( sdstress )
