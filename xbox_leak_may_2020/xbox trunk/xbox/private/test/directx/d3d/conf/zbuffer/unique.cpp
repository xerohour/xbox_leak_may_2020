#include <d3d.h>
#include "ZBuffer.h"

bool Unique(DWORD dwVersion, PBUFFER *pBuffer, LPPIXELFORMAT pFormat)
{
    bool bFound = false;
    PBUFFER pTemp;

    if (dwVersion >= 0x0800)
        return true;

    for (pTemp = *pBuffer; pTemp && !bFound; pTemp = pTemp->pNext)
        if ((pFormat->dwStencilBitMask == pTemp->dwStencilBitMask) &&
            (pFormat->dwZBitMask == pTemp->dwZBitMask))
            bFound = true;

    if (!bFound)
    {
        pTemp = (PBUFFER) malloc(sizeof(BUFFER));
        pTemp->dwStencilBitMask = pFormat->dwStencilBitMask;
        pTemp->dwZBitMask = pFormat->dwZBitMask;
        pTemp->pNext = *pBuffer;
        *pBuffer = pTemp;
    }

    return !bFound;
}
