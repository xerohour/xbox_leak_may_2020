#ifndef _INC_DISKUTIL
#define _INC_DISKUTIL

#include "defs.h"

// Function type modifiers
#define DISKUTILAPI             FAR PASCAL LOADDS

// Number of retries when an absolute disk read/write fails
#define ABSRETRIES              2

// Maximum number of clusters that can be passed to ReadSector/WriteSector
#define SECTORIO_MAX_SECTORS    64

// DOS version macros
#define IS_DOS7_KERNEL(ver)     (ver >= 0x700)
#define IS_FAT32_KERNEL(ver)    (ver >= 0x70A)

#pragma pack(1)

// DOS parameter block
typedef struct tagDPB
{
    BYTE                        bDrive;
    BYTE                        bUnit;
    WORD                        wSectorSize;
    BYTE                        bClusterMask;
    BYTE                        bClusterShift;
    WORD                        wFirstFAT;
    BYTE                        bFATCount;
    WORD                        wOldRootEntries;
    WORD                        wOldFirstSector;
    WORD                        wOldMaxCluster;
    WORD                        wOldFATSize;
    WORD                        wOldDirSector;
    DWORD                       dwDriverAddr;
    BYTE                        bMedia;
    BYTE                        bFlags;
    DWORD                       dwNextDPB;
    WORD                        wOldNextFree;
    DWORD                       dwFreeCount;
    WORD                        wExtFlags;
    WORD                        wFSInfoSector;
    WORD                        wBackupBootSector;
    DWORD                       dwFirstSector;
    DWORD                       dwMaxCluster;
    DWORD                       dwFATSize;
    DWORD                       dwRootCluster;
    DWORD                       dwNextFree;
} DPB, *PDPB, far *LPDPB;

#pragma pack()

#ifdef WIN32

#define InitDiskUtil        InitDiskUtil32
#define FreeDiskUtil        FreeDiskUtil32
#define ResetDrive          ResetDrive32
#define ReadSector          ReadSector32
#define WriteSector         WriteSector32
#define GetDosVersion       GetDosVersion32
#define LockVolume          LockVolume32
#define UnlockVolume        UnlockVolume32
#define GetDriveMapping     GetDriveMapping32
#define GetDPB              GetDPB32

#endif // WIN32

#ifdef WIN16

#define InitDiskUtil        InitDiskUtil16
#define FreeDiskUtil        FreeDiskUtil16
#define ResetDrive          ResetDrive16
#define ReadSector          ReadSector16
#define WriteSector         WriteSector16
#define GetDosVersion       GetDosVersion16
#define LockVolume          LockVolume16
#define UnlockVolume        UnlockVolume16
#define GetDriveMapping     GetDriveMapping16
#define GetDPB              GetDPB16

#endif // WIN16

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL DISKUTILAPI InitDiskUtil(void);
extern BOOL DISKUTILAPI FreeDiskUtil(void);
extern BOOL DISKUTILAPI ResetDrive(void);
extern BOOL DISKUTILAPI ReadSector(BYTE, LPVOID, DWORD, WORD);
extern BOOL DISKUTILAPI WriteSector(BYTE, LPVOID, DWORD, WORD);
extern WORD DISKUTILAPI GetDosVersion(void);
extern BOOL DISKUTILAPI LockVolume(BYTE);
extern BOOL DISKUTILAPI UnlockVolume(BYTE);
extern BOOL DISKUTILAPI GetDriveMapping(BYTE, LPBYTE, LPBYTE);
extern BOOL DISKUTILAPI GetDPB(BYTE, LPDPB);

#ifdef __cplusplus
}
#endif

#endif // _INC_DISKUTIL