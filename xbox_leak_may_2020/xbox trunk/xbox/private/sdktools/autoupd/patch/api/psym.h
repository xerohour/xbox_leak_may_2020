
#ifndef _PATCH_PSYM_H_
#define _PATCH_PSYM_H_

#ifdef __cplusplus
extern "C" {
#endif

BOOL
GetImageSymbolRiftInfoFromPsymFiles(
    IN HANDLE                  OldFileHandle,
    IN PUCHAR                  OldFileMapped,
    IN ULONG                   OldFileSize,
    IN PIMAGE_NT_HEADERS       OldFileNtHeader,
    IN LPCSTR                  OldFileSymPath,
    IN ULONG                   OldFileOriginalChecksum,
    IN ULONG                   OldFileOriginalTimeDate,
    IN ULONG                   OldFileIndex,
    IN HANDLE                  NewFileHandle,
    IN PUCHAR                  NewFileMapped,
    IN ULONG                   NewFileSize,
    IN PIMAGE_NT_HEADERS       NewFileNtHeader,
    IN LPCSTR                  NewFileSymPath,
    IN ULONG                   SymbolOptionFlags,
    IN HANDLE                  SubAllocator,
    IN PRIFT_TABLE             RiftTable,
    IN PPATCH_SYMLOAD_CALLBACK SymLoadCallback,
    IN PVOID                   SymLoadContext
    );

#ifdef __cplusplus
}
#endif

#endif /* _PATCH_PSYM_H_ */

