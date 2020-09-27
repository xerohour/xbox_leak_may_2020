// NV API definitions
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
typedef HANDLE NVDESCRIPTOR;
#define NV_DEVICE_NAME_LENGTH_MAX 127
#define NVARCH_INVALID_NVDESCRIPTOR 0
#define NVARCH_INVALID_CLIENT_HANDLE 0

// the following is used to open the RM -- this will go away when RM is in MP
#define NVAPI_IOCTL_OPEN    NV_IOCTL_CODE(0x0100)
#define NVAPI_IOCTL_CLOSE   NV_IOCTL_CODE(0x0101)
HANDLE  __cdecl NvOpen          (HANDLE);
VOID    __cdecl NvClose         (HANDLE);

// control codes
ULONG __cdecl NvAllocRoot       (HANDLE, ULONG, ULONG*);
ULONG __cdecl NvAllocDevice     (HANDLE, ULONG, ULONG, ULONG, PUCHAR);
ULONG __cdecl NvAllocContextDma (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvAllocChannelPio (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG);
ULONG __cdecl NvAllocChannelDma (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*);
//ULONG __cdecl NvAllocEvent      (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID);
ULONG __cdecl NvAllocMemory     (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG*);
ULONG __cdecl NvAllocObject     (HANDLE, ULONG, ULONG, ULONG, ULONG);
ULONG __cdecl NvAlloc           (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID);
ULONG __cdecl NvFree            (HANDLE, ULONG, ULONG, ULONG);
ULONG __cdecl NvDmaFlowControl  (HANDLE, ULONG, ULONG, ULONG, ULONG);
//ULONG     NvDmaPushMutexState (HANDLE, DWORD);
ULONG __cdecl NvArchHeap        (HANDLE, PVOID);
ULONG __cdecl NvConfigVersion   (HANDLE, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigGet       (HANDLE, ULONG, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigSet       (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigGetEx     (HANDLE, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvConfigSetEx     (HANDLE, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvConfigUpdate    (HANDLE, ULONG, ULONG, ULONG);
ULONG __cdecl NvGetDmaPushInfo  (HANDLE, ULONG, ULONG, ULONG, ULONG,ULONG);
ULONG         NvRmDebugControl  (HANDLE, ULONG, ULONG, PVOID);
ULONG __cdecl NvDirectMethodCall(HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG);

#ifdef __cplusplus
}
#endif // __cplusplus

