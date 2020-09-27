
#ifndef _PATCH_PESTUFF_H_
#define _PATCH_PESTUFF_H_

#ifdef __cplusplus
extern "C" {
#endif

PIMAGE_NT_HEADERS
__fastcall
GetNtHeader(
    IN PVOID MappedFile,
    IN ULONG MappedFileSize
    );

BOOL
NormalizeCoffImage(
    IN OUT PIMAGE_NT_HEADERS NtHeader,
    IN OUT PUCHAR            MappedFile,
    IN     ULONG             FileSize,
    IN     ULONG             OptionFlags,
    IN     PVOID             OptionData,
    IN     ULONG             NewFileCoffBase,
    IN     ULONG             NewFileCoffTime
    );

BOOL
TransformCoffImage(
    IN OUT PULONG            TransformOptions,
    IN OUT PIMAGE_NT_HEADERS NtHeader,
    IN OUT PUCHAR            OldFileMapped,
    IN     ULONG             OldFileSize,
    IN     ULONG             NewFileResTime,
    IN OUT PRIFT_TABLE       RiftTable,
    IN OUT PUCHAR OPTIONAL   HintMap,
    ...
    );

BOOL
GenerateRiftTable(
    IN HANDLE OldFileHandle,
    IN PUCHAR OldFileMapped,
    IN ULONG  OldFileSize,
    IN ULONG  OldFileOriginalChecksum,
    IN ULONG  OldFileOriginalTimeDate,
    IN HANDLE NewFileHandle,
    IN PUCHAR NewFileMapped,
    IN ULONG  NewFileSize,
    IN ULONG  OptionFlags,
    IN PVOID  OptionData,
    IN ULONG  OldFileIndex,
    IN PVOID  RiftTable
    );

BOOL
AddRiftEntryToTable(
    IN PRIFT_TABLE RiftTable,
    IN ULONG       OldRva,
    IN ULONG       NewRva
    );

VOID
InitImagehlpCritSect(
    VOID
    );

VOID
UnloadImagehlp(
    VOID
    );

#define X86_OPCODE_NOP  0x90
#define X86_OPCODE_LOCK 0xF0

BOOL
SmashLockPrefixesInMappedImage(
    IN PUCHAR            MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader,
    IN UCHAR             NewOpCode      // X86_OPCODE_NOP or X86_OPCODE_LOCK
    );

USHORT
ChkSum(
    IN USHORT  Initial,
    IN PUSHORT Buffer,
    IN ULONG   Bytes
    );

PVOID
__fastcall
ImageDirectoryMappedAddress(
    IN  PIMAGE_NT_HEADERS NtHeader,
    IN  ULONG             DirectoryIndex,
    OUT PULONG OPTIONAL   DirectorySize,
    IN  PUCHAR            MappedBase,
    IN  ULONG             MappedSize
    );


#ifdef __cplusplus
}
#endif

#endif // _PATCH_PESTUFF_H_

