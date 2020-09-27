// this file contains the display driver -to- miniport IOCTL codes
// and NvAlloc..() macros for the display driver client


typedef HANDLE NVDESCRIPTOR;
#define NVOPEN_INVALID_DESCRIPTOR 0

#define Nv1GetDeviceOsName(fd, ps)              \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV1_IOCTL_GET_DEVICE_OS_NAME,    \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV1_GET_DEVICE_OS_NAME_STRUCT),  \
        &cbReturned                             \
    );                                          \
}                                               

#define Nv1AllocContextDma(fd, ps)              \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV1_IOCTL_ALLOC_CONTEXT_DMA,     \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV1_ALLOC_CONTEXT_DMA_STRUCT),   \
        &cbReturned                             \
    );                                          \
}                                               

#define Nv1FreeContextDma(fd, ps)               \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV1_IOCTL_FREE_CONTEXT_DMA,      \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV1_FREE_CONTEXT_DMA_STRUCT),    \
        &cbReturned                             \
    );                                          \
}

#define Nv3AllocChannelPio(fd, ps)              \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV3_IOCTL_ALLOC_CHANNEL_PIO,     \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV3_ALLOC_CHANNEL_PIO_STRUCT),   \
        &cbReturned                             \
    );                                          \
}                                               

#define Nv3FreeChannelPio(fd, ps)               \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV3_IOCTL_FREE_CHANNEL_PIO,      \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV3_FREE_CHANNEL_PIO_STRUCT),    \
        &cbReturned                             \
    );                                          \
}

#define Nv3NtAllocChannelDma(fd, ps)              \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV3_IOCTL_ALLOC_CHANNEL_DMA,     \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NVOS07_PARAMETERS),   \
        &cbReturned                             \
    );                                          \
}                                               

#define Nv3FreeChannelDma(fd, ps)               \
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV3_IOCTL_FREE_CHANNEL_DMA,      \
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV3_FREE_CHANNEL_DMA_STRUCT),    \
        &cbReturned                             \
    );                                          \
}

#define Nv3NtDmaFlowControl(fd, ps)               		\
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV3_IOCTL_DMA_FLOW_CONTROL,      		\
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NVOS08_PARAMETERS),    		\
        &cbReturned                             \
    );                                          \
}

#define Nv1AllocMemory(fd, ps)               	\
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV1_IOCTL_ALLOC_MEMORY,      	\
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV1_ALLOC_MEMORY_STRUCT),    	\
        &cbReturned                             \
    );                                          \
}

#define Nv1FreeMemory(fd, ps)               	\
{                                               \
    DWORD cbReturned;                           \
    VOID *pps = ps;                             \
    EngDeviceIoControl(                         \
        (HANDLE)(fd),                           \
        (DWORD)NV1_IOCTL_FREE_MEMORY,      		\
        (&pps),                                 \
        sizeof(PVOID),                          \
        (ps),                                   \
        sizeof(NV1_FREE_MEMORY_STRUCT),    		\
        &cbReturned                             \
    );                                          \
}

