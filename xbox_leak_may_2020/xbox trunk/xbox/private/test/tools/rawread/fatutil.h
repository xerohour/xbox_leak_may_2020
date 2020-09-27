#ifndef _INC_FATUTIL
#define _INC_FATUTIL

#include "defs.h"
#include "image.h"
#include "bitmap.h"

typedef BOOL (CALLBACK *WALKFATCALLBACK)(DWORD, DWORD, BYTE, LPVOID);

#ifdef __cplusplus
extern "C" {
#endif

extern DWORD FATtoDWORD(LPBYTE, DWORD, DWORD, BYTE);
extern int WalkFAT(LPDRIVEINFO, BYTE, DWORD, DWORD, WALKFATCALLBACK, LPVOID);
extern int FindUnallocatedClusters(LPEVERYTHING, DWORD, DWORD, LPIMGBITMAP, LPDWORD, BOOL);
extern int FindBadClusters(LPEVERYTHING, DWORD, DWORD, LPDWORD FAR *, LPDWORD, BOOL);

#ifdef __cplusplus
}
#endif

#endif // _INC_FATUTIL