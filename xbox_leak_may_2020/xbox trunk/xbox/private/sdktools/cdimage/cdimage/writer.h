
typedef
VOID
(*WRITE_COMPLETION)(
    PVOID     pBuffer,
    DWORDLONG dwlOffset,
    DWORD     dwSize,
    PVOID     pUserContext
    );


HANDLE
CreateWriteContext(
    LPCSTR    pFileName,            //  Filename to create (CREATE_ALWAYS)
    DWORD     dwSectorSize,         //  Optional
    DWORDLONG dwInitialSize,        //  Optional -- grow file before writing
    DWORD     nMaxActiveWrites      //  How far can we get ahead of ourself
    );


VOID
IssueWrite(
    HANDLE           hContext,      //  From CreateWriteContext
    PVOID            pBuffer,       //  From AllocateBuffer, we ReleaseBuffer
    DWORDLONG        dwlOffset,     //  Offset where write is to occur
    DWORD            dwSize,        //  Valid size of data in pBuffer
    WRITE_COMPLETION pCompletion,   //  Optional completion callback function
    PVOID            pUserContext   //  Passed to pCompletion routine
    );


VOID
FlushWriter(
    HANDLE hContext                 //  Waits for all queued writes to complete
    );


VOID
AbortWriter(                        //  stops issuing new writes, waits for
    HANDLE hContext                 //  outstanding writes to complete, sets
    );                              //  filesize to zero, then deletes file.


VOID
CloseWriteContext(
    HANDLE    hContext,             //  Flushes, sets EOF, cleans up, closes
    DWORDLONG dwlFileSize           //  Optional, sets file size
    );


LPOVERLAPPED
AllocateOverlappedWithEvent(        //  Allocate OVERLAPPED structure and
    VOID                            //  create hEvent member.
    );


VOID
RecycleOverlappedWithEvent(
    LPOVERLAPPED pOverlapped
    );


HANDLE
GetFileHandleFromWriteContext(      //  OVERLAPPED, NO_BUFFERING semantics
    HANDLE hContext
    );


VOID
QueueMaybeWrite(
    HANDLE           hContext,
    PVOID            pBuffer,
    DWORDLONG        dwlOffset,
    DWORD            dwSize,
    WRITE_COMPLETION pCompletion,
    PVOID            pUserContext,
    DWORD            dwMaxQueuedBytes
    );

VOID
FlushMaybeQueue(
    HANDLE hContext
    );

VOID
DiscardMaybeQueue(
    HANDLE hContext
    );

LPSTR
GetRootPath(
    LPCSTR pszRelativeFileName,
    LPSTR  pszRootPathBuffer
    );

LPWSTR
GetRootPathW(
    LPCWSTR pszRelativeFileName,
    LPWSTR  pszRootPathBuffer
    );

DWORD
GetSectorSizeByName(
    IN LPCSTR pszAnyPathName
    );

DWORD
GetSectorSizeByNameW(
    IN LPCWSTR pszAnyPathName
    );

DWORD
GetSectorSize(
    IN HANDLE  hFile           OPTIONAL,
    IN LPCWSTR pszAnyPathNameW OPTIONAL,
    IN LPCSTR  pszAnyPathName  OPTIONAL
    );

