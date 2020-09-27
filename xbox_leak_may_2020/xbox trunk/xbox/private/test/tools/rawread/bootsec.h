#ifndef _INC_BOOTSEC
#define _INC_BOOTSEC

#include "defs.h"

#pragma pack(1)

// BIOS parameter block
typedef struct tagBPB
{
    WORD                        wSectorSize;
    BYTE                        bSectorsPerCluster;
    WORD                        wReservedSectors;
    BYTE                        bFATCount;
    WORD                        wRootDirEntries;
    WORD                        wTotalSectors;
    BYTE                        bMediaType;
    WORD                        wFATSize;
    WORD                        wSectorsPerTrack;
    WORD                        wHeadCount;
    DWORD                       dwHiddenSectors;
    DWORD                       dwBigTotalSectors;
} BPB, *PBPB, far *LPBPB;

// FAT32 BIOS parameter block
typedef struct tagBGBPB
{
    WORD                        wSectorSize;
    BYTE                        bSectorsPerCluster;
    WORD                        wReservedSectors;
    BYTE                        bFATCount;
    WORD                        wRootDirEntries;
    WORD                        wTotalSectors;
    BYTE                        bMediaType;
    WORD                        wFATSize;
    WORD                        wSectorsPerTrack;
    WORD                        wHeadCount;
    DWORD                       dwHiddenSectors;
    DWORD                       dwBigTotalSectors;
    // Start of FAT32 extensions
    DWORD                       dwBigFATSize;
    WORD                        wExtFlags;
    WORD                        wFSVer;
    DWORD                       dwRootCluster;
    WORD                        wFSInfoSector;
    WORD                        wBackupBootSector;
    WORD                        Reserved[6];
} BGBPB, *PBGBPB, far *LPBGBPB;

// Boot sector
typedef struct tagBOOTSECTOR
{
    BYTE                        abJump[3];
    BYTE                        abOEMName[8];
    BPB                         bpb;
    BYTE                        bDrive;
    BYTE                        Reserved;
    BYTE                        bBootSignature;
    DWORD                       dwVolumeID;
    BYTE                        abVolumeLabel[11];
    BYTE                        abFSType[8];
    BYTE                        code[450];
} BOOTSECTOR, *PBOOTSECTOR, far *LPBOOTSECTOR;

// FAT32 boot sector
typedef struct tagBGBOOTSECTOR
{
    BYTE                        abJump[3];
    BYTE                        abOEMName[8];
    BGBPB                       bpb;
    BYTE                        bDrive;
    BYTE                        bReserved;
    BYTE                        bBootSignature;
    DWORD                       dwVolumeID;
    BYTE                        abVolumeLabel[11];
    BYTE                        abFSType[8];
    BYTE                        code[422];
} BGBOOTSECTOR, *PBGBOOTSECTOR, far *LPBGBOOTSECTOR;

#pragma pack()

#endif // _INC_BOOTSEC