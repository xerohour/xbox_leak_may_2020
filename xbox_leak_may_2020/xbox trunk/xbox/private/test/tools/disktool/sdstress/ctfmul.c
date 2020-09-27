/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    ctfmul.c

Abstract:

     copy test multiple file

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

unsigned long 
__stdcall 
CopyTestFileMultiple (
    LPVOID pCopyTestFileData
    )
/*++

Routine Description:

    Test Scenario 3: Issue many reads, and writes

Arguments:

    COPYTESTFILEDATA structure which contains all information required for 
    this function

Return Value:

    Always 0 (actual return value is placed in structure)

--*/ 
{
    HANDLE hSfile, hDfile;
    BOOL bSuccess;
    DWORD dwAccessed, SFP_RetVal = 0;
    DWORD lFileCount = 0;
    UINT subcount = 0;
    BOOL PassFlag = TRUE;
    char szBuffer[200];

    //
    // Init the data structure
    //

    COPYTESTFILE_DATA *CopyTestFileData=(COPYTESTFILE_DATA *)pCopyTestFileData;

    //
    // initialize logging
    //

    xSetComponent(hSdStressLog, "disk", "sdstress" );
    xSetFunctionName( hSdStressLog, "CopyTestFileMultiple" );
    xStartVariation( hSdStressLog, "variation1" );

    __try {

        //
        // start the test
        //

        sprintf (szBuffer, 
                 "CopyTestFileMultiple entered for Thread Src:%c, Dst:%c, Instance:%d\n",
                 CopyTestFileData->ThreadData->SrcDrive->Letter,
                 CopyTestFileData->ThreadData->DestDrive->Letter,
                 CopyTestFileData->ThreadData->ThreadInstance);
        Debug_Print (szBuffer);

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
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FileMode,
                             NULL);

        if ( hDfile == INVALID_HANDLE_VALUE || hSfile == INVALID_HANDLE_VALUE ) {
            CopyTestFileData->ErrorCode = GetLastError();
            CopyTestFileData->retval = E_NOHANDLE;

            __leave;
        }

        while ( lFileCount <= CopyTestFileData->lFileSize / CopyTestFileData->BlockSize - 1 ) {
            for ( subcount = 1; subcount<=SCEN3REDO; subcount++ ) {
                if ( subcount > 1 && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize ) {

                    SFP_RetVal = SetFilePointer (hSfile,
                                                 (CopyTestFileData->BlockSize * -1),
                                                 NULL,
                                                 FILE_CURRENT);

                } else if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
                    SFP_RetVal = SetFilePointer (hSfile,
                                                 0,
                                                 NULL,
                                                 FILE_BEGIN);
                }

                if ( SFP_RetVal == -1 ) {
                    CopyTestFileData->ErrorCode = GetLastError();
                    CopyTestFileData->retval = E_SEEK;

                    __leave;
                }

                bSuccess=ReadFile(hSfile,
                                  CopyTestFileData->pszDataBuffer,
                                  CopyTestFileData->BlockSize,
                                  &dwAccessed,
                                  NULL);

                if ( !bSuccess || (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                     (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {
                    CopyTestFileData->ErrorCode = GetLastError();
                    CopyTestFileData->retval = E_COPY;

                    __leave;
                }

//                FlushFileBuffers (hSfile);
            }

            for ( subcount=1; subcount<=SCEN3REDO; subcount++ ) {
                if ( subcount > 1 && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize ) {

                    SFP_RetVal = SetFilePointer (hDfile,
                                                 (CopyTestFileData->BlockSize * -1),
                                                 NULL,
                                                 FILE_CURRENT);

                } else if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
                    SFP_RetVal = SetFilePointer (hDfile,
                                                 0,
                                                 NULL,
                                                 FILE_BEGIN);
                }

                if ( SFP_RetVal == -1 ) {
                    CopyTestFileData->ErrorCode = GetLastError();
                    CopyTestFileData->retval = E_SEEK;

                    __leave;
                }

                bSuccess=WriteFile(hDfile,
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
            }

            if ( CopyTestFileData->lFileSize < CopyTestFileData->BlockSize ) {
                break;
            } else {
                lFileCount ++;
            }
        }

        Close_Handle(&hSfile);
        Close_Handle(&hDfile);

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
            CopyTestFileData->ErrorCode = GetLastError();
            CopyTestFileData->retval = E_NOHANDLE;

            __leave;
        }

        lFileCount = 0;
        CopyTestFileData->Offset = 0;

        while ( lFileCount <= CopyTestFileData->lFileSize/CopyTestFileData->BlockSize - 1 ) {
            bSuccess = ReadFile (hSfile,
                                 CopyTestFileData->pszCompareBuffer,
                                 CopyTestFileData->BlockSize,
                                 &dwAccessed,
                                 NULL);

            if ( !bSuccess || (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                 (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {
                CopyTestFileData->ErrorCode = GetLastError();
                CopyTestFileData->retval = E_VERIFYREAD;
                Close_Handle (&hSfile);
                Close_Handle (&hDfile);

                __leave;
            }

            bSuccess = ReadFile (hDfile,
                                 CopyTestFileData->pszDataBuffer,
                                 CopyTestFileData->BlockSize,
                                 &dwAccessed,
                                 NULL);

            if ( !bSuccess || (dwAccessed < CopyTestFileData->lFileSize && CopyTestFileData->lFileSize < CopyTestFileData->BlockSize) ||
                 (dwAccessed != CopyTestFileData->BlockSize && CopyTestFileData->lFileSize > CopyTestFileData->BlockSize) ) {
                CopyTestFileData->ErrorCode = GetLastError();
                CopyTestFileData->retval = E_VERIFYREAD;
                Close_Handle (&hSfile);
                Close_Handle (&hDfile);

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
                lFileCount ++;
            }
        }

        CopyTestFileData->retval=0;
    }

    __finally {

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
                  "CopyTestFileMultiple Passed");
        }

        if ( CopyTestFileData->ErrorCode ) {
            if ( bDebug && KDENABLED ) {
                //DebugBreak();
                ++GlobalErrorCounter;
                sprintf (szBuffer, 
                         "CopyTestFileMultiple : hit an error Source: %s Dest: %s : code=%d : GlobalErrorCounter=%d\n",
                         CopyTestFileData->pszSname,
                         CopyTestFileData->pszDname,
                         CopyTestFileData->ErrorCode,
                         GlobalErrorCounter);
                Debug_Print (szBuffer);
            }
            xLog (hSdStressLog, 
                  (CopyTestFileData->ErrorCode == ERROR_DISK_FULL) ? XLL_WARN : XLL_FAIL, 
                  "Error detected in CopyTestFileMultiple: Error : %d Source: %s Dest: %s",
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
