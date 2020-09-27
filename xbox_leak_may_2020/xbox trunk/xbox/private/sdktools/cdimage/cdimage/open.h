
HANDLE
CreateOpenContext(
    UINT nMaxOpenAhead
    );


BOOL
QueueOpenFile(
    HANDLE    hOpenContext,
    LPCWSTR   pszUnicodeName,
    LPCSTR    pszFileName,        // used if pszUnicodeName is NULL
    DWORD     dwSectorSize,
    DWORD     dwBufferSize,
    DWORDLONG dwFileSize,
    UINT      nMaxReadAhead,
    PVOID     pUserContext
    );


HANDLE
WaitForOpen(
    HANDLE  hOpenContext,
    LPCWSTR *pszUnicodeName,
    LPCSTR  *pszFileName,
    PVOID   *ppUserContext
    );


VOID
CloseOpenContext(
    HANDLE hOpenContext
    );




