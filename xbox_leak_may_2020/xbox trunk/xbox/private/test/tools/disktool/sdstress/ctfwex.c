/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    ctfwex.c

Abstract:

     copy test / file IO / use WriteFileEx for async writes

Author / ported by:

    John Daly (johndaly) porting date 4/29/2000

Environment:

    X-Box

[Notes:]

    port from the NT sdstress for X-Box testing
    
    Limited support for asynchronous file writes is available.  
    As before, a file must be opened for overlapped I/O.  
    When you do a write with the ending byte offset <= the file size, 
    then the write will be asynchronous.  
    If the ending byte offset > file size, then the write will be synchronous.
    To look at both behaviors, the file will be written 2 times.

Revision History:

    initial port: johndaly
        port from NT
    WriteFileEx/async modification from ctfioc.c

--*/

unsigned long 
__stdcall 
CopyTestFileUsingWriteFileEx (
    LPVOID pCopyTestFileData
    )
/*++

Routine Description:

    Test Scenario 2: Use IoCompletionPorts

Arguments:

    COPYTESTFILEDATA structure which contains all information required for this function

Return Value:

    Always 0 (actual return value is placed in structure)

--*/ 
{
    HANDLE hSfile = INVALID_HANDLE_VALUE;
    HANDLE hDfile = INVALID_HANDLE_VALUE;
    DWORD PassCreateFlag = CREATE_ALWAYS;
    BOOL bSuccess;
    DWORD dwAccessed;
    DWORD lFileCount = 0;
    DWORD RetryCount = 0;
    char szBuffer[200] = {0};
    OVERLAPPED sOverLapped;
    int Pass;
    BOOL PassFlag = TRUE;

    //
    // Init the data structure
    //

    COPYTESTFILE_DATA *CopyTestFileData=(COPYTESTFILE_DATA *)pCopyTestFileData;

    //
    // initialize logging
    //

    xSetComponent(hSdStressLog, "disk", "sdstress" );
    xSetFunctionName( hSdStressLog, "CopyTestFileUsingWriteFileEx" );
    xStartVariation( hSdStressLog, "variation1" );

    __try
    {
        //
        // start the test
        //

        //
        // first pass through, the write will be extending the file 
        // (byte offset > the file size), which will cause synchronous IO
        // second pass through, it will be writing into existing file 
        // (byte offset <= the file size), which will allow asynchronous operation
        //

        for ( Pass = 0; Pass < 2; Pass++ ) {
            switch ( Pass ) {
                case 0:
                    PassCreateFlag = CREATE_ALWAYS;
                    break;

                case 1:
                    PassCreateFlag = OPEN_EXISTING;
                    break;
            }

            sprintf (szBuffer, 
                     "CopyTestFileUsingWriteFileEx entered for Thread Src:%c, Dst:%c, Instance:%d Pass:%d\n",
                     CopyTestFileData->ThreadData->SrcDrive->Letter,
                     CopyTestFileData->ThreadData->DestDrive->Letter,
                     CopyTestFileData->ThreadData->ThreadInstance,
                     Pass);
            Debug_Print (szBuffer);

            //
            // Zero the Overlap structure
            //

            memset (&sOverLapped, 0, sizeof (OVERLAPPED));

            hSfile = CreateFileA(CopyTestFileData->pszSname,
                                 GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FileMode,
                                 NULL);

            hDfile = CreateFileA(CopyTestFileData->pszDname,
                                 GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 PassCreateFlag,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING | FileMode,
                                 NULL);

            //
            // Note
            // getting bad hDfile, sharing violation, suspecting pending writes
            // try re-trying
            //

            if ( hDfile == INVALID_HANDLE_VALUE || hSfile == INVALID_HANDLE_VALUE ) {
                CopyTestFileData->ErrorCode=GetLastError();
                CopyTestFileData->retval=E_NOHANDLE;

                __leave;

            }

            while ( lFileCount <= CopyTestFileData->lFileSize / CopyTestFileData->BlockSize - 1 ) {

                bSuccess = ReadFile (hSfile,
                                     CopyTestFileData->pszDataBuffer,
                                     CopyTestFileData->BlockSize,
                                     &dwAccessed,
                                     NULL);

                if ( !bSuccess || 
                     (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                     (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {

                    CopyTestFileData->ErrorCode = GetLastError();

                    if ( CopyTestFileData->ErrorCode != ERROR_IO_PENDING ) {
                        CopyTestFileData->retval = E_COPY;

                        __leave;
                    }
                }

                RetryCount = 0;
                while ( RetryCount < MAXRETRIES ) {
                    bSuccess = WriteFileEx(hDfile,
                                           CopyTestFileData->pszDataBuffer,
                                           CopyTestFileData->BlockSize,
                                           &sOverLapped,
                                           WriteFileExCompletionRoutine);

                    if ( !bSuccess ) {
                        CopyTestFileData->ErrorCode = GetLastError ();

                        if ( (CopyTestFileData->ErrorCode != ERROR_INVALID_USER_BUFFER &&
                              CopyTestFileData->ErrorCode != ERROR_NOT_ENOUGH_QUOTA &&
                              CopyTestFileData->ErrorCode != ERROR_WORKING_SET_QUOTA &&
                              CopyTestFileData->ErrorCode != ERROR_NOT_ENOUGH_MEMORY) ||
                             RetryCount == MAXRETRIES ) {

                            CopyTestFileData->retval = E_COPY;

                            __leave;
                        }
                    } else {
                        break;
                    }

                    RetryCount ++;
                }

                //
                // note: need to SleepEx or wait to get completion routine to fire
                //

                SleepEx(1, TRUE);

                RetryCount = 0;

                sOverLapped.Offset += CopyTestFileData->BlockSize;

                if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
                    break;
                } else {
                    lFileCount ++;
                }
            }

            //
            // flush to blow away any pending IO before we finish up and kill the overlap
            // (I looked at FlushFileBuffers, it appears to do this)
            // this gets us re-synchronized and we avoid 'ERROR_SHARING_VIOLATION' error
            //

            FlushFileBuffers (hDfile);
            FlushFileBuffers (hSfile);
            Close_Handle (&hSfile);
            Close_Handle (&hDfile);

            hSfile = CreateFileA(CopyTestFileData->pszSname,
                                 GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FileMode,
                                 NULL);

            hDfile = CreateFileA(CopyTestFileData->pszDname,
                                 GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FileMode,
                                 NULL);

            if ( hDfile == INVALID_HANDLE_VALUE || hSfile == INVALID_HANDLE_VALUE ) {
                CopyTestFileData->ErrorCode=GetLastError();
                CopyTestFileData->retval=E_NOHANDLE;

                __leave;
            }

            lFileCount = 0;
            CopyTestFileData->Offset = 0;

            while ( lFileCount <= CopyTestFileData->lFileSize / CopyTestFileData->BlockSize - 1 ) {

                bSuccess=ReadFile(hSfile,
                                  CopyTestFileData->pszCompareBuffer,
                                  CopyTestFileData->BlockSize,
                                  &dwAccessed,
                                  NULL);

                if ( !bSuccess || (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                     (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {
                    CopyTestFileData->ErrorCode=GetLastError();
                    CopyTestFileData->retval=E_COPY;

                    __leave;
                }

                bSuccess=ReadFile(hDfile,
                                  CopyTestFileData->pszDataBuffer,
                                  CopyTestFileData->BlockSize,
                                  &dwAccessed,
                                  NULL);

                if ( !bSuccess || (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                     (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {
                    CopyTestFileData->ErrorCode=GetLastError();
                    CopyTestFileData->retval=E_COPY;

                    __leave;
                }

                CopyTestFileData->Offset = lFileCount * CopyTestFileData->BlockSize;

                //
                // debug code to catch some memory corruption
                //

                ValidateCOPYTESTFILE_DATA (CopyTestFileData); 

                // end of debug code

                //
                // check buffers
                //

                if ( FALSE == VerifyData(CopyTestFileData) ) {
                    xLog (hSdStressLog, 
                          XLL_FAIL, 
                          "VerifyData failure in CopyTestFile.");
                    PassFlag = FALSE;
                }

                if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
                    break;
                } else {
                    lFileCount++;
                }
            }

            CopyTestFileData->retval=0;

            //
            // flush to blow away any pending IO before we finish up and kill the overlap
            // (I looked at FlushFileBuffers, it appears to do this)
            // this gets us re-synchronized and we avoid 'ERROR_SHARING_VIOLATION' error
            //

            FlushFileBuffers (hDfile);
            FlushFileBuffers (hSfile);
            Close_Handle (&hSfile);
            Close_Handle (&hDfile);

        }
    }

    __finally
    {
        if ( INVALID_HANDLE_VALUE != hDfile ) {
            FlushFileBuffers (hDfile);
            Close_Handle (&hDfile);
        }

        if ( INVALID_HANDLE_VALUE != hSfile ) {
            FlushFileBuffers (hSfile);
            Close_Handle (&hSfile);
        }

        if ( PassFlag == TRUE ) {
            xLog (hSdStressLog, 
                  XLL_PASS, 
                  "CopyTestFileUsingWriteFileEx Passed");
        }

        if ( CopyTestFileData->ErrorCode ) {
            if ( bDebug && KDENABLED ) {
                //DebugBreak();
                ++GlobalErrorCounter;
                sprintf (szBuffer, 
                         "CopyTestFileUsingWriteFileEx : hit an error Source: %s Dest: %s : code=%d : GlobalErrorCounter=%d\n",
                         CopyTestFileData->pszSname,
                         CopyTestFileData->pszDname,
                         CopyTestFileData->ErrorCode,
                         GlobalErrorCounter);
                Debug_Print (szBuffer);
            }
            xLog (hSdStressLog, 
                  (CopyTestFileData->ErrorCode == ERROR_DISK_FULL) ? XLL_WARN : XLL_FAIL, 
                  "Error detected in CopyTestFileUsingWriteFileEx: Error : %d Source: %s Dest: %s",
                  CopyTestFileData->ErrorCode,
                  CopyTestFileData->pszSname,
                  CopyTestFileData->pszDname);
        }
    }

    //
    // close logging
    //

    xEndVariation( hSdStressLog);

    return(0);
}

VOID
CALLBACK 
WriteFileExCompletionRoutine(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    )
/*++

Routine Description:

    Callback for WriteFileEx

Arguments:

    DWORD dwErrorCode,                // completion code (irrelevant in this case)
    DWORD dwNumberOfBytesTransfered,  // number of bytes transferred
    LPOVERLAPPED lpOverlapped         // I/O information buffer

Return Value:

    none
    
Notes:

    doing anything in here will delay completion of the pending IO, so put a 
    wait in here if you want to experiment with stacking them up...
    other filesystem tests make sure that this things fires correctly, I did
    some experimenting with it while writing this code and have seen that
    it works, so we'll let it do nothing instead of doing a bunch of verification

--*/ 
{
    //Sleep(100);
    return;
}
