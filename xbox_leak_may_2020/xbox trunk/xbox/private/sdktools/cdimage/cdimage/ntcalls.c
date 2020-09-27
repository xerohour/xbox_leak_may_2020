
#include "precomp.h"
#pragma hdrstop

#ifndef NTSTATUS
    typedef LONG NTSTATUS;                          // from ntdef.h
#endif

#ifndef NT_SUCCESS
    #define NT_SUCCESS(Status) \
                ((NTSTATUS)(Status) >= 0)           // from ntdef.h
#endif

#ifndef IO_STATUS_BLOCK

    typedef struct _IO_STATUS_BLOCK {               // from ntioapi.h
        NTSTATUS Status;
        ULONG Information;
    } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#endif

#ifndef FILE_FS_SIZE_INFORMATION

    typedef struct _FILE_FS_SIZE_INFORMATION {      // from ntioapi.h
        LARGE_INTEGER TotalAllocationUnits;
        LARGE_INTEGER AvailableAllocationUnits;
        ULONG SectorsPerAllocationUnit;
        ULONG BytesPerSector;
    } FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

#endif // FILE_FS_SIZE_INFORMATION

#ifndef FS_INFORMATION_CLASS

    typedef enum _FSINFOCLASS {                     // from ntioapi.h
        FileFsVolumeInformation = 1,
        FileFsLabelInformation,
        FileFsSizeInformation,
        FileFsDeviceInformation,
        FileFsAttributeInformation,
        FileFsControlInformation,
        FileFsQuotaQueryInformation,
        FileFsQuotaSetInformation,
        FileFsMaximumInformation
    } FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

#endif

typedef
NTSYSAPI
NTSTATUS
(NTAPI *NTQUERYVOLUMEINFORMATIONFILE)(
    IN  HANDLE               FileHandle,
    OUT PIO_STATUS_BLOCK     IoStatusBlock,
    OUT PVOID                FsInformation,
    IN  ULONG                Length,
    IN  FS_INFORMATION_CLASS FsInformationClass
    );

NTQUERYVOLUMEINFORMATIONFILE NtQueryVolumeInformationFile;

BOOL
MyGetFileSectorInfo(
    IN  HANDLE hFile,
    OUT PULONG pSectorSize,
    OUT PULONG pClusterSize
    )
    {
    FILE_FS_SIZE_INFORMATION FsInfo;
    IO_STATUS_BLOCK          IoStatusBlock;
    NTSTATUS                 Status;

    if ( NtQueryVolumeInformationFile == NULL ) {
        return FALSE;
        }

    Status = NtQueryVolumeInformationFile( hFile,
                                           &IoStatusBlock,
                                           &FsInfo,
                                           sizeof( FsInfo ),
                                           FileFsSizeInformation );

    if ( NT_SUCCESS( Status )) {
        *pSectorSize  = FsInfo.BytesPerSector;
        *pClusterSize = FsInfo.BytesPerSector * FsInfo.SectorsPerAllocationUnit;
        }
    else {
        *pSectorSize  = 0;
        *pClusterSize = 0;
        }

    return NT_SUCCESS( Status );
    }


typedef
BOOL
(APIENTRY *ISDEBUGGERPRESENT)(          // defined in kernel32.dll only on NT
    VOID
    );

ISDEBUGGERPRESENT IsDebuggerPrsent;

BOOL
MyIsDebuggerPresent(
    VOID
    )
    {
    return (( IsDebuggerPrsent != NULL ) ? IsDebuggerPrsent() : FALSE );
    }


BOOL
InitializeNtDllPointers(
    VOID
    )
    {
    HMODULE hKernel32 = GetModuleHandle( "KERNEL32.DLL" );
    HMODULE hNtDll    = GetModuleHandle( "NTDLL.DLL" );

    if ( hKernel32 != NULL ) {
        IsDebuggerPrsent =
            (ISDEBUGGERPRESENT)
            GetProcAddress( hKernel32, "IsDebuggerPresent" );
        }

    if ( hNtDll != NULL ) {
        NtQueryVolumeInformationFile =
            (NTQUERYVOLUMEINFORMATIONFILE)
            GetProcAddress( hNtDll, "NtQueryVolumeInformationFile" );
        }

    return (( NtQueryVolumeInformationFile != NULL ) && ( IsDebuggerPrsent != NULL ));
    }



