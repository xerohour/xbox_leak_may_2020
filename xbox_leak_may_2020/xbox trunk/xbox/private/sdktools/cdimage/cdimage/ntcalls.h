
BOOL
InitializeNtDllPointers(
    VOID
    );

BOOL
MyGetFileSectorInfo(
    IN  HANDLE hFile,
    OUT PULONG pSectorSize,
    OUT PULONG pClusterSize
    );

BOOL
MyIsDebuggerPresent(
    VOID
    );


