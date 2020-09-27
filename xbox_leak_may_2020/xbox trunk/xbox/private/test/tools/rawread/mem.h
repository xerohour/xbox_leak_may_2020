#ifndef _INC_MEM
#define _INC_MEM

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern LPVOID MemAlloc(UINT);
extern LPVOID MemReAlloc(LPVOID, UINT);
extern void MemFree(LPVOID);
extern void MemCopy(LPVOID, LPVOID, UINT);
extern void MemSet(LPVOID, BYTE, UINT);
extern int MemCmp(LPVOID, LPVOID, UINT);
extern UINT MemSize(LPVOID);
extern void MemBogify(LPVOID, DWORD);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _INC_MEM