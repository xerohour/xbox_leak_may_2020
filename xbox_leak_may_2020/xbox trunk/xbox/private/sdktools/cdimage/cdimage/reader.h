

//
//  Following codes are returned by WaitForRead
//

#define STATUS_SUCCESS     0x00000000   //  Succeeded, more data remaining
#define STATUS_END_OF_FILE 0xC0000011   //  Succeeded, but no more remaining
                                        //  Any other value is GLE error code


HANDLE
CreateReadContext(
    LPCWSTR   pUnicodeName,             // primary filename to open if hFile == NULL
    LPCSTR    pFileName,                // secondary filename to open if hFile == NULL
    HANDLE    hFile,                    // optional (OVERLAPPED)
    DWORD     dwSectorSize,             // optional (queried from system)
    DWORD     dwBufferSize,             // optional (defaults to sector size)
    DWORD     nMaxReadAhead,            // optional (defaults to no maximum)
    HANDLE    hEventAllReadsIssued,     // optional (signaled when all reads issued)
    DWORDLONG dwlInitialOffset,         // optional (defaults to begin of file)
    DWORDLONG dwlFileSize               // optional (defaults to GetFileSize())
    );


DWORD
WaitForRead(
    HANDLE hContext,                //  From CreateReadContext
    PVOID  *pBuffer,                //  Returned buffer (need to ReleaseBuffer)
    DWORD  *dwSize                  //  Valid size of data in pBuffer
    );


VOID
StopIssuingReads(                   //  Don't issue any more reads, but don't
    HANDLE hContext                 //  wait on anything (like close does).
    );


VOID
CloseReadContext(
    HANDLE hContext,                //  Cancels outstanding reads, cleans up
    BOOL   bCloseFile               //  close file handle too?
    );


HANDLE
GetFileHandleFromReadContext(       //  OVERLAPPED, NO_BUFFERING semantics
    HANDLE hContext
    );


DWORDLONG
GetFileSizeFromReadContext(
    HANDLE hContext
    );


DWORDLONG
GetFileSize64(
    IN HANDLE hFile
    );

