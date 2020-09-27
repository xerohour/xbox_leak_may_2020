

DWORD
AdjustWorkingSetAndLockImage(
    DWORD MinimumRingBufferSize,
    DWORD IndividualBufferSize
    );


DWORD
InitializeBufferAllocator(
    DWORD RequestedRingBufferSize,
    DWORD MinimumRingBufferSize,
    DWORD IndividualBufferSize
    );


PVOID
AllocateBuffer(
    UINT nBufferSize,
    BOOL bZeroMemory
    );


VOID
ReleaseBuffer(
    PVOID pBuffer
    );


DWORD
GetLargestFreeBufferSize(
    VOID
    );

