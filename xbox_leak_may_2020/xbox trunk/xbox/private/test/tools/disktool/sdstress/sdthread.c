/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    sdthread.c

Abstract:

     This the thread control C file.

Author / ported by:

    John Daly (johndaly) porting date 4/29/2000

Environment:

    X-Box

[Notes:]

    port from the NT sdstress for X-Box testing

Revision History:

    initial port: johndaly
        port from NT

--*/

//
// includes
//

#include "ctf.c"
#include "ctfioc.c"
#include "ctfmul.c"
#include "ctfwex.c"

unsigned long 
__stdcall 
SdStressThread (
    LPVOID pThreadData
    )
/*++

Routine Description:

    This is the intermediate SdStress thread function.  This function creates 
    the worker threads 

Arguments:

    pThreadData - Structure to THREAD_DATA (contains Source & Destination drive info)

Return Value:
    None    

--*/ 
{
    char szBuffer[500];
    char szSrcName[300], szDestName[300], WaitCount = 0;
    long retval;
    ULONG lFileSize;
    UINT RetryCount = 0;
    COPYTESTFILE_DATA CopyTestFileData;
    HANDLE hThread = NULL;
    DWORD Dummy, WaitStatus;
    LPTHREAD_START_ROUTINE lpTestFunction = NULL;
    unsigned long ReturnValue = 0;
    THREAD_DATA *ThreadData = (THREAD_DATA *) pThreadData;

    __try
    {
        //
        //Init structure
        //

        *ThreadData->ThreadExist = TRUE;
        CopyTestFileData.ThreadData = ThreadData;
        ThreadData->Count = 0;

        InterlockedIncrement (&TotalStartedThreadCount);

        srand (TotalStartedThreadCount /*+ (unsigned int)time()*/);

        //
        // Determine test method
        //

        switch ( ThreadData->TestType ) {
            
            case CTF:
            default:
                {
                    lpTestFunction = &CopyTestFile;
                    break;
                }

            case CTF_IOC:
                {
                    lpTestFunction = &CopyTestFileUsingIoC;
                    break;
                }

            case CTF_MULTIPLE:
                {
                    lpTestFunction = &CopyTestFileMultiple;
                    break;
                }

            case CTF_WFEX:
                {
                    lpTestFunction = &CopyTestFileUsingWriteFileEx;
                    break;
                }
        }

        //
        // determine block sizing based on drive types, Allocate memory for copying
        //
        // Under the new model, I need to figure out whether the source or target
        // drive has the highest granularity (BytesPerSector), since I will be setting
        // blocksize based on BytesPerSector instead of hardcoded values
        //

        if ( ThreadData->SrcDrive->BytesPerSector < ThreadData->DestDrive->BytesPerSector ) {

            CopyTestFileData.MinimumBlockSize = ThreadData->DestDrive->BytesPerSector;

        } else {

            CopyTestFileData.MinimumBlockSize = ThreadData->SrcDrive->BytesPerSector;

        }

        if ( bRandomize ) {

            CopyTestFileData.BlockSize = CopyTestFileData.MinimumBlockSize * MAX_BLOCK_MULTIPLIER;

        } else {

            CopyTestFileData.BlockSize = CopyTestFileData.MinimumBlockSize * BlockSizeMultiplier;
        }

        sprintf (szBuffer,
                 "Thread MinBlockSize: Src:%c, Dst:%c, Instance:%d = %d\n",
                 ThreadData->SrcDrive->Letter,
                 ThreadData->DestDrive->Letter,
                 ThreadData->ThreadInstance,
                 CopyTestFileData.MinimumBlockSize);

        Debug_Print (szBuffer);

        CopyTestFileData.pszDataBuffer = AllocateMemory (CopyTestFileData.BlockSize);
        CopyTestFileData.pszCompareBuffer = AllocateMemory (CopyTestFileData.BlockSize);

        if ( (NULL == CopyTestFileData.pszDataBuffer) || 
             (NULL == CopyTestFileData.pszCompareBuffer) ) {
            WriteTextConsole ("Cannot allocate memory.  Terminating thread...\n", 
                              ERRORCOLOR,
                              XLL_BREAK);     

            __leave;
        }

        sprintf (szBuffer,
                 "Thread started: Src:%c, Dst:%c, Instance:%d\n",
                 ThreadData->SrcDrive->Letter,
                 ThreadData->DestDrive->Letter,
                 ThreadData->ThreadInstance);

        Debug_Print (szBuffer);

        //
        // Determine Source and Destination test filenames
        //

        sprintf (szSrcName, "%c:\\SDDATA\\TESTFILE.DAT",
                 ThreadData->SrcDrive->Letter);

        sprintf (szDestName, "%c:\\SDDATA\\%c%ctfile.%d",
                 ThreadData->DestDrive->Letter,
                 ThreadData->SrcDrive->Letter,
                 ThreadData->DestDrive->Letter,
                 ThreadData->ThreadInstance);

        //
        // If this is a Removable or Fixed Disk, increment the counter
        //

        if ( ThreadData->SrcDrive->Type == SD_REMOVABLE || ThreadData->SrcDrive->Type == SD_FIXED ) {
            InterlockedIncrement (&FixedDiskThreadStillRunning);
        }

        //
        // Main loop
        //

        while ( ThreadData->Count < Passes ) {

            //
            // If source ISN'T CD_ROM
            //

            if ( ThreadData->SrcDrive->Type != SD_CDROM ) {

                CopyTestFileData.pszSname = szSrcName;
                CopyTestFileData.pszDname = szDestName;
                CopyTestFileData.lFileSize = TestFileSize;

                if ( bRandomize ) {
                    CopyTestFileData.BlockSize = CopyTestFileData.MinimumBlockSize * ((rand () % (MAX_BLOCK_MULTIPLIER - 1)) + 1);
                    sprintf (szBuffer, "Random block size: %d\n",
                             CopyTestFileData.BlockSize);
                    Debug_Print (szBuffer);
                }

                //
                // Create copying thread
                //

                hThread = CreateThread (NULL,
                                        0,
                                        lpTestFunction,
                                        &CopyTestFileData,
                                        0,
                                        &Dummy);

                //
                // Wait until thread is finished
                //

                if ( hThread == NULL ) {
                    WriteTextConsole ("Unable to create worker thread.\n",
                                      ERRORCOLOR,
                                      XLL_INFO);

                    __leave;
                } else {
                    WriteTextConsole ("worker thread created.\n",
                                      ERRORCOLOR,
                                      XLL_INFO);
                }

                WaitCount = 0;
                while ( WaitStatus = WaitForSingleObject (hThread, STD_THREAD_TIMEOUT) == WAIT_FAILED && 
                      WaitCount < MAX_WAIT_RETRIES ) {
                    Sleep (2000);
                    WaitCount ++;
                }

                //
                // this is bad
                // the thread will still be able to run, and our cleanup will free the 
                // buffers it uses, we want to debug it, we'll eventually crash if we 
                // continue to run
                // probably caused by low memory condition, should be rare that this is hit
                //

                if ( WaitStatus == WAIT_FAILED ) {

                    PrintError (GetLastError ());
                    WriteTextConsole ( "WaitForSingleObject failed.",
                                       ERRORCOLOR,
                                       XLL_BREAK);
                    OutputDebugStringA("WaitForSingleObject failed. contimuing from here will eventually crash.");
                    DebugBreak();
                    __leave;
                }

                //
                // we wait forever, this should probably not happen...
                // 

                if ( WaitStatus == WAIT_TIMEOUT ) {
                    WriteTextConsole ("WaitForSingleObject timeout (CTF), possible deadlock on thread.\n\n",
                                      ERRORCOLOR,
                                      XLL_BREAK);

                    __leave;

                }

                CloseHandle (hThread);
                hThread = NULL;

                //
                // Determine what happened during CopyTestFile
                //

                ParseReturnValue (&CopyTestFileData, ThreadData);

            } else {

                //
                // Source drive is CDROM
                //

                //
                // Get total number of files on CD
                //

                sprintf (szBuffer,
                         "%u directories containing files on the CDROM.\n",
                         ThreadData->SrcDrive->NumberOfFiles);

                Debug_Print (szBuffer);

                //
                // Find random file on CD to copy
                //

                for ( RetryCount = 0; RetryCount < MAXRETRIES; RetryCount ++ ) {

                    retval = GetRandomFile (ThreadData->SrcDrive->Letter,
                                            szSrcName,
                                            &lFileSize,
                                            ThreadData->SrcDrive->NumberOfFiles);

                    if ( retval && lFileSize > 2048 ) {
                        break;
                    }

                    Sleep (FAILED_GRF_STALL);
                }

                if ( !retval ) {
                    sprintf (szBuffer, "Src:%c, Dest:%c, Instance:%d SrcNmae:%s FileSize:%d NumFiles:%d - Could not locate suitable test file.\n",
                             ThreadData->SrcDrive->Letter,
                             ThreadData->DestDrive->Letter,
                             ThreadData->ThreadInstance,
                             szSrcName,
                             lFileSize,
                             ThreadData->SrcDrive->NumberOfFiles);

                    WriteTextConsole (szBuffer,
                                      ERRORCOLOR,
                                      XLL_BREAK);

                    __leave;
                }

                sprintf (szBuffer, "Src:%c, Dest:%c, Instance:%d - File %s (%u) selected.\n",
                         ThreadData->SrcDrive->Letter,
                         ThreadData->DestDrive->Letter,
                         ThreadData->ThreadInstance,
                         szSrcName,
                         lFileSize);

                WriteTextConsole (szBuffer, NORMALTEXTCOLOR, XLL_INFO);

                //
                // Source is CD, Destination is anything but Floppy
                //

                CopyTestFileData.pszSname = szSrcName;
                CopyTestFileData.pszDname = szDestName;
                CopyTestFileData.lFileSize = lFileSize;

                if ( bRandomize ) {
                    CopyTestFileData.BlockSize = CopyTestFileData.MinimumBlockSize * ((rand () % (MAX_BLOCK_MULTIPLIER - 1)) + 1);

                    sprintf (szBuffer, "Random block size (CD): %d\n",
                             CopyTestFileData.BlockSize);

                    Debug_Print (szBuffer);
                }

                else {
                    CopyTestFileData.BlockSize = CopyTestFileData.MinimumBlockSize;
                }

                //
                // Create the Thread
                //

                hThread = CreateThread (NULL,
                                        0,
                                        lpTestFunction,
                                        &CopyTestFileData,
                                        0,
                                        &Dummy);

                //
                // Wait for thread to finish
                //

                if ( hThread == NULL ) {
                    WriteTextConsole ("Unable to create worker thread.\n",
                                      ERRORCOLOR,
                                      XLL_BREAK);

                    __leave;
                } else {
                    WriteTextConsole ("worker thread created.\n",
                                      ERRORCOLOR,
                                      XLL_INFO);
                }

                WaitCount = 0;
                while ( WaitStatus = WaitForSingleObject (hThread, STD_THREAD_TIMEOUT) == WAIT_FAILED && 
                      WaitCount < MAX_WAIT_RETRIES ) {
                    Sleep (2000);
                    WaitCount ++;
                }

                //
                // this is bad
                // the thread will still be able to run, and our cleanup will free the 
                // buffers it uses, we want to debug it, we'll eventually crash if we 
                // continue to run
                // probably caused by low memory condition, should be rare that this is hit
                //

                if ( WaitStatus == WAIT_FAILED ) {

                    PrintError (GetLastError ());
                    WriteTextConsole ( "WaitForSingleObject failed.",
                                       ERRORCOLOR,
                                       XLL_BREAK);
                    OutputDebugStringA("WaitForSingleObject failed. continuing from here will eventually crash.");
                    DebugBreak();
                    __leave;
                }

                //
                // we wait forever, this should probably not happen...
                // 

                if ( WaitStatus == WAIT_TIMEOUT ) {
                    WriteTextConsole ("WaitForSingleObject timeout (CTF), possible deadlock on thread.\n\n",
                                      ERRORCOLOR,
                                      XLL_BREAK);

                    __leave;
                }

                CloseHandle (hThread);
                hThread = NULL;

                //
                // Determine what happened during CopyTestFile
                //

                ParseReturnValue (&CopyTestFileData,ThreadData);
            }

            if ( ThreadData->SrcDrive->Type != SD_REMOVABLE && ThreadData->SrcDrive->Type != SD_FIXED ) {

                if ( !TotalNumberHardDrives && !TotalNumberRemovableDrives ) {
                    ThreadData->Count ++;
                }

                if ( !FixedDiskThreadStillRunning && (TotalNumberHardDrives || TotalNumberRemovableDrives) ) {
                    __leave;
                }
            }

            else
                ThreadData->Count ++;
        }

        //
        // This is where we clean up the thread's data, and exit
        //

        sprintf (szBuffer,
                 "Thread is finished: Src:%c, Dst:%c, Instance:%d\n",
                 ThreadData->SrcDrive->Letter,
                 ThreadData->DestDrive->Letter,
                 ThreadData->ThreadInstance);

        WriteTextConsole(szBuffer,
                         PASSCOLOR,
                         XLL_INFO);

        //
        // sync
        //

        if ( ThreadData->SrcDrive->Type == SD_REMOVABLE || ThreadData->SrcDrive->Type == SD_FIXED ) {
            InterlockedDecrement (&FixedDiskThreadStillRunning);
        }
    }

    __finally
    {
        //
        // DeAllocateMemory does validity checking at function level
        //

        Debug_Print ("Memory Deallocation (SdStressThread)\n");

        DeAllocateMemory (CopyTestFileData.BlockSize,
                          CopyTestFileData.pszDataBuffer);

        DeAllocateMemory (CopyTestFileData.BlockSize,
                          CopyTestFileData.pszCompareBuffer);

        if ( hThread != NULL ) {
            CloseHandle (hThread);
        }

        *ThreadData->ThreadExist = FALSE;

        if ( !InterlockedDecrement (&TotalFinishedThreadCount) ) {
                WriteTextConsole ("FATAL ERROR:Unable to InterlockedDecrement(&TotalFinishedThreadCount).\n",
                                  ERRORCOLOR,
                                  FALSE);
                ReturnValue = EXIT_PROCESS;
        }

    }

    if ( ReturnValue != EXIT_PROCESS ) {
        return(ReturnValue);
    } else {
        ExitThread (EXIT_PROCESS);
    }
}

long 
GetNumberOfFiles (
    char DriveLetter
    )
/*++

Routine Description:

    This function will retrieve the number of files on a CDROM,
    

Arguments:

    DriveLetter - Contains the letter of the drive to retrieve files

Return Value:

    Number of files on the CD-ROM (or 0 if failed)
    
Note:

    . and .. no longer exist on xbox
    

--*/ 
{
    int depth = 0;
    DWORD SubDirCount = 0;
    long FileCount = 0;
    char FindPath[300] = {0};
    char *strptr = NULL;

    //
    // start in root
    //
    
    sprintf (FindPath, "%c:\\", DriveLetter);
    strcat (FindPath, "*.*");
    hFile[0] = _findfirst (FindPath, &FileInfo[0]);
    
    //
    // if something went wrong...
    //

    if ( hFile[0] == -1L )
    {
        OutputDebugStringA("Warning! findfirst failed in root!\n");
        return(0);
    }
    
    while (TRUE) {

        //
        // are there directories in this directory?
        // if yes, data recurse into them immediatly
        //

        if ( FileInfo[depth].attrib & _A_SUBDIR ) {

            //
            // fix path string
            //

            if (strptr = strstr (FindPath, "*.*")) {
                *strptr = '\0';
            }
            strcat (FindPath, FileInfo[depth].name);
            strcat (FindPath, "\\*.*");
            
            //
            // find first file in subdir
            //

            if (depth < MAX_DEPTH - 1) {
                ++depth;
                hFile[depth] = _findfirst (FindPath, &FileInfo[depth]);
            } else {
                //
                // in this case we have overflowed our data area, I will need
                // to know about it so I can fix the code to hold more FileInfo entries, etc
                //
                OutputDebugStringA("SDStress : GetNumberOfFiles : MAX_DEPTH exceeded");
                DebugBreak();
                goto clear; // this should let you fail gracefully
            }

            //
            // empty dir - or error, add code to check errno
            //
            
            if ( hFile[depth] == -1L )
            {
                goto clear; // we are done with this directory
            }

            continue; // continue this look with the new current fileinfo

        } else {
            
            //
            // there are files in this directory if we got here
            // add to pszSubDirList
            // only make this check in the first visit to this directory
            //
            
            ++FileCount;

            if (FALSE == FileFoundFlag[depth]) {
                FileFoundFlag[depth] = TRUE;
                if (SubDirCount < SD_MAXDIRS - 1) {
                    if (strptr = strstr (FindPath, "*.*")) {
                        *strptr = '\0';
                    }
                    strcpy(pszSubDirList[SubDirCount++], FindPath);
                } else {

                    //
                    // we have run out of reasons to be looking through this 
                    // media, but keep going, even though we won't add new dirs
                    //
                }
            }

            //
            // look for next file / directory
            //

            if (-1L == _findnext (hFile[depth], &FileInfo[depth])) {
                goto clear;
            } else {
                continue;
            }
        }
        
        //
        // done with this directory, clear state
        //

        clear:
            
            //
            // truncate directory\*.* off of FindPath so that findpath matches current dir
            //

            if (strptr = strstr (FindPath, "*.*")) {
                *strptr = '\0';
            }
            strptr = FindPath + strlen(FindPath) - 1;
            if (strptr > FindPath) {    // skip terminating '\' - is there always one when this code is hit?
                --strptr;   
                while ((*strptr != '\\') && (strptr > FindPath)){
                    --strptr;
                }
            }
            *++strptr = '\0';   // leave trailing '\'

            //
            // close this level and back out to next level
            //

            _findclose(hFile[depth]);
            memset(&FileInfo[depth], 0, sizeof(FileInfo[0]));
            FileFoundFlag[depth] = FALSE;
            --depth;
            
            //
            // are we are done
            //

            if (depth < 0) {
                break;
            }

            //
            // perform resume scan in level backed into
            //

            if (-1L == _findnext (hFile[depth], &FileInfo[depth])) {
                goto clear;
            } else {
                continue;
            }
    }
    
    return SubDirCount;
}

char 
GetRandomFile (
    char DriveLetter, 
    char *pszRndFile, 
    unsigned long *FileLength, 
    long NumberOfFiles
    )
/*++

Routine Description:

    This function retrieves a file from a CDROM based on a random number

Arguments:

    DriveLetter - Contains the drive letter of the CDROM reader
    pszRndFile - Pointer to buffer where this function will place the file name and path
    FileLength - Pointer to variable to hold the size of the selected file
    NumberOfFiles - Contains the number of directories containing files on the
        CDROM/DVDROM (produced from GetNumberOfFiles)

Return Value:

    0 - Function failed
    1 - Function succeeded
    
    note: the prior incarnation of this function used a critical section, 
    I don't see the need for that since we are only reading the list

--*/ 
{
    long FileCount = 0, SelectFile;
    DWORD LastError;
    __int3264 hFile;
    char FindPath[300];
    char ErrorString[200];
    struct _finddata_t FileInfo;
    char *strptr = NULL;
    
    if ( 0 == NumberOfFiles )   {
        return(0);
    }

    //
    // select a random directory from the ones known to contain files
    //

    if (NumberOfFiles > 1) {
        SelectFile = rand () % (NumberOfFiles);
    } else {
        SelectFile = 0;
    }

    strcpy (FindPath, pszSubDirList[SelectFile]);
    strcat(FindPath, "*.*");
    
    //
    // this means something is wrong!
    //

    while ( -1L == (hFile = _findfirst (FindPath, &FileInfo)))
    {
        LastError = GetLastError();
        // added extra debug crap to track down stress bug
        // safe to stomp on filepath now
        //_asm int 3
        ++GlobalErrorCounter;
        sprintf (ErrorString, 
         "GetRandomFile : hit an error Source: %s errno=%d : lasterror=%d : GlobalErrorCounter=%d\n",
         FindPath,
         errno,
         LastError,
         GlobalErrorCounter);
        Debug_Print (ErrorString);

    }
    
    //
    // count the files in the directory
    // not neccessarily an efficient approach, but we want to do lots of IO...
    //

    do {
        if (!(FileInfo.attrib & _A_SUBDIR )) {
            FileCount++;
        }
    } while ( _findnext (hFile, &FileInfo) == 0 );
    
    _findclose (hFile);

    //
    // randomly pick one to return
    //
    
    hFile = _findfirst (FindPath, &FileInfo);
    if ( hFile == -1L )
    {
        // this has been seen in low memory situations, so don't break
        OutputDebugStringA("ERROR! findfirst failed during count!\n");
        //DebugBreak();
        return(0);
    }
    
    if (FileCount > 1) {
        SelectFile = rand () % (FileCount - 1) + 1;
    } else {
        SelectFile = 1;
    }

    FileCount = 0;
    while( FileCount < SelectFile) {
        if (!(FileInfo.attrib & _A_SUBDIR )) {
            FileCount++;
        }
        _findnext (hFile, &FileInfo);
    }
    
    *FileLength = FileInfo.size;
    if (strptr = strstr (FindPath, "*.*")) {
        *strptr = '\0';
    }
    sprintf (pszRndFile, "%s%s",
             FindPath,
             FileInfo.name);
    
    _findclose (hFile);
    return(1);
}

unsigned 
char * 
AllocateMemory (
    DWORD Size
    )
/*++

Routine Description:

    This function does all of the dynamic memory allocation for the
    copying functions (multiple of sector size, aligned)

Arguments:

    Parameters:  Size - Number of bytes to allocate

Return Value:

    FALSE - Failure
    TRUE - Success

--*/ 
{
    return((unsigned char *) VirtualAlloc (NULL, 
                                           Size,
                                           MEM_COMMIT,
                                           PAGE_READWRITE));

}

BOOL 
DeAllocateMemory (
    DWORD Size, 
    LPVOID pszBuffer
    )
/*++

Routine Description:

    This function deallocates memory allocated by AllocateMemory.

Arguments:

    Size - Number of bytes allocated
    pszBuffer - Pointer to buffer allocated by AllocateMemory

Return Value:

    FALSE - Failure
    TRUE - Success

Note:

    ignore size, just blow away the enitire allocation
    
--*/ 
{
    if ( pszBuffer ) {
        return(VirtualFree (pszBuffer, 0, MEM_RELEASE));
    }

    return(FALSE);
}

long 
ParseReturnValue (
    COPYTESTFILE_DATA *CopyTestFileData, 
    THREAD_DATA *ThreadData
    )
/*++

Routine Description:

    This function shows the result of the copy operation based on retval

Arguments:

    COPYTESTFILEDATA - Contains the scenario's results
    ThreadData - Contains the count information

Return Value:

    Return Value: Retval

--*/ 
{
    char szBuffer[200];

    switch ( CopyTestFileData->retval ) {
    
    case E_SDABORT:
        {
            sprintf (szBuffer,
                     "ABORTED on thread: Src:%c, Dst:%c, Instance:%d\n",
                     ThreadData->SrcDrive->Letter,
                     ThreadData->DestDrive->Letter,
                     ThreadData->ThreadInstance);

            WriteTextConsole(szBuffer, ABORTCOLOR, XLL_WARN);
            break;
        }

    case NULL:
        {
            if ( ThreadData->SrcDrive->Type == SD_CDROM ) {
                sprintf (szBuffer,
                         "PASS on thread: Src:%c, Dst:%c, Instance:%d\n",
                         ThreadData->SrcDrive->Letter,
                         ThreadData->DestDrive->Letter,
                         ThreadData->ThreadInstance);
            }

            else {
                sprintf (szBuffer,
                         "PASS (%u/%u) on thread: Src:%c, Dst:%c, Instance:%d\n",
                         ThreadData->Count+1,
                         Passes,
                         ThreadData->SrcDrive->Letter,
                         ThreadData->DestDrive->Letter,
                         ThreadData->ThreadInstance);
            }

            WriteTextConsole(szBuffer, PASSCOLOR, XLL_PASS);
            break;
        }

    default:
        {
            sprintf (szBuffer,
                     "FAILURE (%u/%u) on thread: Src:%c, Dst:%c, Instance:%d\n",
                     ThreadData->Count+1,
                     Passes,
                     ThreadData->SrcDrive->Letter,
                     ThreadData->DestDrive->Letter,
                     ThreadData->ThreadInstance);

            WriteTextConsole(szBuffer, ERRORCOLOR, XLL_FAIL);
            PrintSDError(CopyTestFileData->retval);
            if ( CopyTestFileData->ErrorCode )
                PrintError(CopyTestFileData->ErrorCode);
            break;
        }

    }

    return(CopyTestFileData->retval);
}

unsigned 
long 
__stdcall 
VerifyData (
    LPVOID CTFD
    )
/*++

Routine Description:

    This function is used by the copy threads to verify the data read 
    and written

Arguments:

    CTFD (CopyTestFileData) - Contains copy thread information

Return Value:

    None (well, not used at least)

--*/ 
{
    unsigned long count = 0, lFileCount = 0;
    char szBuffer[200 + MAX_PATH + MAX_PATH];
    ULONG PassFailFlag = TRUE;

    COPYTESTFILE_DATA *CopyTestFileData = (COPYTESTFILE_DATA *) CTFD;

    CopyTestFileData->ErrorCode = NULL;
    CopyTestFileData->retval = 0;

    if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
        lFileCount = CopyTestFileData->lFileSize;
    }

    else {
        lFileCount = CopyTestFileData->BlockSize;
    }

    //
    // what are we comparing...
    //

    sprintf (szBuffer,
             "VerifyData:Source: %s\nVerifyData:Destination: %s\n",
             CopyTestFileData->pszSname, CopyTestFileData->pszDname);
    
    // too noisy
//    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);

    //
    // on the compares, only dump at the start and the end of the 
    // corrupted region to make the logs smaller
    //

    if ( CopyTestFileData->ThreadData->SrcDrive->Type == SD_CDROM ) {
        while ( count < lFileCount ) {
            if ( CopyTestFileData->pszDataBuffer[count] != CopyTestFileData->pszCompareBuffer[count] ) {
                if ((NULL != CopyTestFileData->ErrorCode) && (E_VERIFYCOPY != CopyTestFileData->retval)) {
                    sprintf (szBuffer,
                             "Verify failure begin: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count + (lFileCount * CopyTestFileData->BlockSize),
                             CopyTestFileData->pszCompareBuffer[count],
                             CopyTestFileData->pszDataBuffer[count]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);

                    CopyTestFileData->ErrorCode=NULL;
                    CopyTestFileData->retval=E_VERIFYCOPY;
                    PassFailFlag = FALSE;
                }
            } else {
                if ((NULL == CopyTestFileData->ErrorCode) && (E_VERIFYCOPY == CopyTestFileData->retval)) {
                    sprintf (szBuffer,
                             "Verify failure end: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count - 1 + (lFileCount * CopyTestFileData->BlockSize),
                             CopyTestFileData->pszCompareBuffer[count - 1],
                             CopyTestFileData->pszDataBuffer[count - 1]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                }
            }
            count ++;
        }
    }

    else {
        int SourceTargetFlag = 0;
        while ( count < lFileCount ) {
            if ( CopyTestFileData->pszDataBuffer[count] != pszDataImage[count + CopyTestFileData->Offset] ) {
                if ((NULL != CopyTestFileData->ErrorCode) && 
                    (E_VERIFYCOPY != CopyTestFileData->retval) &&
                    (SourceTargetFlag & 0x1)) {
                    sprintf (szBuffer,
                             "Verify failure begin from target: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count + (lFileCount * CopyTestFileData->BlockSize),
                             pszDataImage[count + CopyTestFileData->Offset],
                             CopyTestFileData->pszDataBuffer[count]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                    CopyTestFileData->ErrorCode=NULL;
                    CopyTestFileData->retval=E_VERIFYCOPY;
                    SourceTargetFlag &= 0x1;
                    PassFailFlag = FALSE;
                }
            } else {
                if ((NULL == CopyTestFileData->ErrorCode) && 
                    (E_VERIFYCOPY == CopyTestFileData->retval) &&
                    (SourceTargetFlag & 0x1)) {
                    sprintf (szBuffer,
                             "Verify failure end from target: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count - 1 + (lFileCount * CopyTestFileData->BlockSize),
                             pszDataImage[count - 1 + CopyTestFileData->Offset],
                             CopyTestFileData->pszDataBuffer[count - 1]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                    SourceTargetFlag ^= 0x1;
                }
            }

            if ( CopyTestFileData->pszCompareBuffer[count] != pszDataImage[count + CopyTestFileData->Offset] ) {
                if ((NULL != CopyTestFileData->ErrorCode) && 
                    (E_VERIFYCOPY != CopyTestFileData->retval) &&
                    (SourceTargetFlag & 0x2)) {
                    sprintf (szBuffer,
                             "Verify failure begin from source: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count + (lFileCount * CopyTestFileData->BlockSize),
                             pszDataImage[count + CopyTestFileData->Offset],
                             CopyTestFileData->pszCompareBuffer[count]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                    CopyTestFileData->ErrorCode=NULL;
                    CopyTestFileData->retval=E_VERIFYCOPY;
                    SourceTargetFlag &= 0x2;
                    PassFailFlag = FALSE;
                }
            } else {
                if ((NULL == CopyTestFileData->ErrorCode) && 
                    (E_VERIFYCOPY == CopyTestFileData->retval) &&
                    (SourceTargetFlag & 0x2)) {
                    sprintf (szBuffer,
                             "Verify failure begin from source: Offset 0x%X, Exp:0x%X, Actual:0x%X\n",
                             count - 1 + (lFileCount * CopyTestFileData->BlockSize),
                             pszDataImage[count - 1 + CopyTestFileData->Offset],
                             CopyTestFileData->pszCompareBuffer[count - 1]);
                    WriteTextConsole (szBuffer, ERRORCOLOR, FALSE);
                    SourceTargetFlag ^= 0x2;
                }
            }
            count ++;
        }
    }

    return(PassFailFlag);
}

BOOL 
Close_Handle (
    HANDLE *phHandle
    )
/*++

Routine Description:

    This function closes handles and sets them to INVALID_HANDLE_VALUE

Arguments:

    phHandle - Pointer to handle to close

Return Value:

    TRUE if success
    FALSE if not success

--*/ 
{
    if ( *phHandle == INVALID_HANDLE_VALUE ) {
        return(FALSE);
    }

    CloseHandle (*phHandle);

    *phHandle = INVALID_HANDLE_VALUE;

    return(TRUE);
}

BOOL 
ValidateCOPYTESTFILE_DATA (
    COPYTESTFILE_DATA *pTestFileData
    )
/*++

Routine Description:

    This function validates COPYTESTFILE_DATA structure to help track down some memory corruption

Arguments:

    COPYTESTFILE_DATA *pTestFileData

Return Value:

    TRUE if success
    FALSE if not success

--*/ 
{
    DWORD verify_count = 0;
    unsigned char verify_char;
    
    //
    // first, try derefrencing the COPYTESTFILE_DATA data
    //
    
    char *pszSname = pTestFileData->pszSname;          
    char *pszDname = pTestFileData->pszDname;          
    DWORD lFileSize = pTestFileData->lFileSize;         
    DWORD BlockSize = pTestFileData->BlockSize;
    DWORD MinimumBlockSize = pTestFileData->MinimumBlockSize;  
    long ErrorCode = pTestFileData->ErrorCode;         
    long retval = pTestFileData->retval;            
    THREAD_DATA *ThreadData = pTestFileData->ThreadData;        
    unsigned char *pszCompareBuffer = pTestFileData->pszCompareBuffer;  
    unsigned char *pszDataBuffer = pTestFileData->pszDataBuffer;     
    unsigned Offset = pTestFileData->Offset;            

    //
    // walk the THREAD_DATA 
    //

    DISKINFO *SrcDrive = pTestFileData->ThreadData->SrcDrive;
    DISKINFO *DestDrive = pTestFileData->ThreadData->DestDrive;
    UINT ThreadInstance = pTestFileData->ThreadData->ThreadInstance;
    BOOL *ThreadExist = pTestFileData->ThreadData->ThreadExist;
    char TestType = pTestFileData->ThreadData->TestType;
    long Passes = pTestFileData->ThreadData->Passes;
    UINT Count = pTestFileData->ThreadData->Count;

    //
    // walk the DISKINFO
    //
    
    char Letter = pTestFileData->ThreadData->SrcDrive->Letter;
    UINT Type = pTestFileData->ThreadData->SrcDrive->Type;
    ULARGE_INTEGER TotalFreeSpace = pTestFileData->ThreadData->SrcDrive->TotalFreeSpace;
    ULARGE_INTEGER TotalNumberOfBytes = pTestFileData->ThreadData->SrcDrive->TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes = pTestFileData->ThreadData->SrcDrive->TotalNumberOfFreeBytes;
    DWORD BytesPerSector = pTestFileData->ThreadData->SrcDrive->BytesPerSector;
    DWORD TestFileSize = pTestFileData->ThreadData->SrcDrive->TestFileSize;
    long NumberOfFiles = pTestFileData->ThreadData->SrcDrive->NumberOfFiles;

    Letter = pTestFileData->ThreadData->DestDrive->Letter;
    Type = pTestFileData->ThreadData->DestDrive->Type;
    TotalFreeSpace = pTestFileData->ThreadData->DestDrive->TotalFreeSpace;
    TotalNumberOfBytes = pTestFileData->ThreadData->DestDrive->TotalNumberOfBytes;
    TotalNumberOfFreeBytes = pTestFileData->ThreadData->DestDrive->TotalNumberOfFreeBytes;
    BytesPerSector = pTestFileData->ThreadData->DestDrive->BytesPerSector;
    TestFileSize = pTestFileData->ThreadData->DestDrive->TestFileSize;
    NumberOfFiles = pTestFileData->ThreadData->DestDrive->NumberOfFiles;

    //
    // walk the data buffers
    //

    while (verify_count < pTestFileData->BlockSize)
    {
        verify_char = pszCompareBuffer[verify_count];
        ++verify_count;
    }

    verify_count = 0;
    
    while (verify_count < pTestFileData->BlockSize)
    {
        verify_char = pszDataBuffer[verify_count];
        ++verify_count;
    }

    return(TRUE);
}


