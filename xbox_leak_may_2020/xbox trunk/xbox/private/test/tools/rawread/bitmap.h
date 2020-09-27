#ifndef _INC_BITMAP
#define _INC_BITMAP

#include "defs.h"

// Invalid page
#define BITMAP_PAGE_ERROR       0xFFFFFFFF

// Compression bitmap info
typedef struct tagIMGBITMAP
{
    DWORD                       dwSize;             // Size of entire bitmap in sectors
    DWORD                       dwPage;             // Current page in memory
    BOOL                        fDirty;             // Set TRUE when modified
    FILEHANDLE                  hFile;              // File handle
    DWORD                       dwFileOffset;       // Sector offset to the begining of the bitmap in the file
    LPBYTE                      lpbBuffer;          // Bit buffer
} IMGBITMAP, *PIMGBITMAP, far *LPIMGBITMAP;

#ifdef __cplusplus
extern "C" {
#endif

extern int InitBitmap(LPIMGBITMAP, DWORD, FILEHANDLE, DWORD, BOOL);
extern int ClusterInMemory(LPIMGBITMAP, DWORD);
extern int FlushBitmapBuffer(LPIMGBITMAP);
extern int GetBitmapBit(LPIMGBITMAP, DWORD, LPBOOL);
extern int SetBitmapBit(LPIMGBITMAP, DWORD);
extern int DiscardBitmap(LPIMGBITMAP);

#ifdef __cplusplus
}
#endif

#endif // _INC_BITMAP