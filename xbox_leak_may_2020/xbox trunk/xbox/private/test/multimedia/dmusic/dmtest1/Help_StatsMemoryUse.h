#include "globals.h"

#define MEM_PHYSICAL TRUE
#define MEM_NORMAL   FALSE

HRESULT CreateHeaps(LPCSTR szHeap, DWORD dwLimitNormal, DWORD dwLimitPhysical);
HRESULT DestroyHeaps(void);
HRESULT InitializeDMusicWithCreatedHeaps(void);
HRESULT GetMemoryUsage(BOOL bPhysical, DWORD *pdwMemUsage);
HRESULT WaitForMemoryToStabilize(BOOL bPhysical, DWORD dwTargetRange, DWORD dwTargetDuration, DWORD dwTimeOut, BOOL *pbStabilized, DWORD *pdwMemory);
HRESULT ExpectMemoryToStabilize(BOOL bPhysical, DWORD dwTargetRange, DWORD dwTargetDuration, DWORD dwTimeOut, BOOL *pbStabilized, DWORD *pdwMemory);
