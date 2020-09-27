#ifndef _INC_IMAGE
#define _INC_IMAGE

#include "bitmap.h"
#include "bootsec.h"
#include "defs.h"
#include "diskutil.h"

// Maximum number of image fragments
#define MAX_IMAGE_FRAGMENTS     0xFF

#pragma pack(1)

typedef struct tagLEADSECTOR
{
    DWORD                       dwClusterCount;     // Count of clusters in the image
    BYTE                        bSequence;          // Image fragment sequence number
    BYTE                        bFragCount;         // Count of fragments in the image
    DWORD                       dwClustersPerFrag;  // Count of clusters per fragment
    char                        szNextFrag[13];     // 8.3 DOS filename of next fragment in sequence
    BYTE                        Reserved1;          // Reserved - formally compression bitmap size
    WORD                        wFlags;             // Options used when image was created
    DWORD                       Reserved2;          // Reserved
    DWORD                       dwCompBitmapSize;   // Count of sectors in the compression bitmap for this fragment
    DWORD                       dwAllocBitmapSize;  // Count of sectors in the allocation bitmap for this fragment
    DWORD                       dwFirstCluster;     // First cluster in this fragment
    DWORD                       dwLastCluster;      // Last cluster in this fragment
    BOOL                        fCompleted;         // Was image fragment completed successfully?
    char                        szComment[0x100];   // NULL-terminated comment string
} LEADSECTOR, *PLEADSECTOR, FAR *LPLEADSECTOR;

typedef struct tagIMAGESTRUCT
{
    WORD                        wFlags;                                         // Options/flags
    DWORD                       dwMaxFragSize;                                  // Maximum fragment size (in megabytes)
    char                        aszFragNames[MAX_IMAGE_FRAGMENTS][MAX_PATH];    // Fragment names
    LEADSECTOR                  ls;                                             // Lead sector
    DWORD                       dwCurrentCluster;                               // Current cluster
    FILEHANDLE                  fhImage;                                        // Current image fragment file handle
    LPBYTE                      lpbReadBuffer;                                  // Buffer to read into
    LPBYTE                      lpbWriteBuffer;                                 // Buffer to write from
    IMGBITMAP                   ibmpCompression;                                // Compression bitmap
    IMGBITMAP                   ibmpAllocation;                                 // Allocation bitmap

#ifdef WIN32

    HWND                        hWnd;                                           // Main window handle

#endif // WIN32

} IMAGESTRUCT, *PIMAGESTRUCT, FAR *LPIMAGESTRUCT;

typedef struct tagRRBOOTSECTOR
{
    BGBOOTSECTOR                bsOriginal;         // The original boot sector
    DWORD                       dwSectorSize;       // Sector size (in bytes)
    DWORD                       dwSectorsPerCluster;// Cluster size (in sectors)
    DWORD                       dwReservedSectors;  // Count of reserved sectors
    DWORD                       dwFATCount;         // Count of FATs
    DWORD                       dwFATSize;          // FAT size (each, in sectors)
    DWORD                       dwBitsPerFATEntry;  // Count of bits in each FAT entry
    DWORD                       dwRootDirEntries;   // Count of root directory entries (FAT12/FAT16 only)
    DWORD                       dwPreDataSectors;   // Count of sectors prior to first cluster
    DWORD                       dwTotalSectors;     // Count of sectors
    DWORD                       dwUsableSectors;    // Count of usable sectors
    DWORD                       dwTotalClusters;    // Count of clusters
    char                        szVolumeLabel[12];  // Volume label
    char                        szFSType[9];        // File system type
} RRBOOTSECTOR, *PRRBOOTSECTOR, FAR *LPRRBOOTSECTOR;

typedef struct tagDRIVEINFO
{
    BYTE                        bDrive;             // 0-based drive number
    DPB                         dpb;                // DPB
    RRBOOTSECTOR                bs;                 // Boot sector
    DWORD                       dwUnusedClusters;   // Count of unallocated clusters
    DWORD                       dwBadClusters;      // Count of bad clusters
    LPDWORD                     adwBadClusters;     // Bad cluster list
} DRIVEINFO, *PDRIVEINFO, FAR *LPDRIVEINFO;

typedef struct tagEVERYTHING
{
    LPIMAGESTRUCT               lpImage;            // Image information
    LPDRIVEINFO                 lpDrive;            // Drive information
} EVERYTHING, *PEVERYTHING, FAR *LPEVERYTHING;

#pragma pack()

// Image signatures
extern const char               sig11[];
extern const char               sig30[];

__inline DWORD SectorToCluster(LPRRBOOTSECTOR lpbs, DWORD dwSector)
{
    return (dwSector - lpbs->dwPreDataSectors) / (lpbs->dwSectorsPerCluster + 2);
}

__inline DWORD ClusterToSector(LPRRBOOTSECTOR lpbs, DWORD dwCluster)
{
    return lpbs->dwPreDataSectors + ((dwCluster - 2) * lpbs->dwSectorsPerCluster);
}

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes
extern int ReadImageHeader(FILEHANDLE, LPLEADSECTOR, LPDWORD, BOOL);
extern int WriteImageHeader(FILEHANDLE, LPLEADSECTOR, LPDWORD);
extern BOOL CanRecoverImage(FILEHANDLE, BYTE);
extern int GetClustersInFragment(LPLEADSECTOR, LPIMGBITMAP, LPDWORD);
extern int GetDriveInfo(BYTE, LPDRIVEINFO);
extern int ConvertBootSector(LPBGBOOTSECTOR, LPRRBOOTSECTOR);
extern void GetDefaultFragNames(LPSTR, LPIMAGESTRUCT, DWORD, DWORD);
extern int GetFragNames(LPIMAGESTRUCT, DWORD, DWORD);
extern FILEHANDLE OpenFragmentForRead(BYTE, LPIMAGESTRUCT);
extern FILEHANDLE OpenFragmentForWrite(BYTE, LPIMAGESTRUCT);
extern BOOL ReadSectorEx(BYTE, LPVOID, DWORD, DWORD, DWORD);
extern BOOL WriteSectorEx(BYTE, LPVOID, DWORD, DWORD, DWORD);

#ifdef __cplusplus
}
#endif

#endif // _INC_IMAGE