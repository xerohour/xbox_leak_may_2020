#ifndef WIN32
#include <malloc.h>
#include <memory.h>
#endif // WIN32

#include "defs.h"
#include "mem.h"


LPVOID MemAlloc(UINT uBytes)
{

#if defined(WIN32) || defined(WIN16)

    return (LPVOID)LocalAlloc(LPTR, uBytes);

#else // WIN32

    return _fmalloc((size_t)uBytes);

#endif // _WIN32

}


LPVOID MemReAlloc(LPVOID lpvBlock, UINT uBytes)
{
    LPVOID                  lpvNewBlock;
    
    // Be nice
    if(!lpvBlock)
    {
        return MemAlloc(uBytes);
    }
    
    // Attempt to reallocate
    if(!(lpvNewBlock = MemAlloc(uBytes)))
    {
        return NULL;
    }

    MemCopy(lpvNewBlock, lpvBlock, min(uBytes, MemSize(lpvBlock)));
    MemFree(lpvBlock);

    return lpvNewBlock;
}


void MemFree(LPVOID lpvBlock)
{

#if defined(WIN32) || defined(WIN16)

    LocalFree((HLOCAL)lpvBlock);

#else // WIN32

    _ffree(lpvBlock);

#endif // WIN32

}


void MemCopy(LPVOID lpvDest, LPVOID lpvSrc, UINT uBytes)
{

#ifdef WIN32

    CopyMemory(lpvDest, lpvSrc, uBytes);

#else // WIN32

    _fmemcpy(lpvDest, lpvSrc, (size_t)uBytes);

#endif // WIN32

}


void MemSet(LPVOID lpvBlock, BYTE bFill, UINT uBytes)
{

#ifdef WIN32

    FillMemory(lpvBlock, uBytes, bFill);

#else // WIN32

    _fmemset(lpvBlock, bFill, (size_t)uBytes);

#endif // WIN32

}


int MemCmp(LPVOID lpvBlockA, LPVOID lpvBlockB, UINT dwSize)
{

#ifdef WIN32

    LPBYTE                  lpba, lpbb;

    lpba = (LPBYTE)lpvBlockA;
    lpbb = (LPBYTE)lpvBlockB;

    while(dwSize--)
    {
        if(*lpba != *lpbb)
        {
            return *lpba - *lpbb;
        }

        lpba++;
        lpbb++;
    }

    return 0;

#else // WIN32

    return _fmemcmp(lpvBlockA, lpvBlockB, (size_t)dwSize);

#endif // WIN32

}


UINT MemSize(LPVOID lpvBlock)
{

#ifdef WIN32

    return LocalSize((HLOCAL)lpvBlock);

#else

    return _fmsize(lpvBlock);

#endif // WIN32

}


void MemBogify(LPVOID lpvBlock, DWORD dwSize)
{
    LPBYTE                  lpb = (LPBYTE)lpvBlock;

    while(dwSize)
    {
        *lpb = ~*lpb;

        dwSize--;
        lpb++;
    }
}