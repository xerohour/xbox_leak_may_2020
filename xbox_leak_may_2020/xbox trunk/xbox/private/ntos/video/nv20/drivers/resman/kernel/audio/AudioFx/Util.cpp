// ****************************************************************
//
//	Util.cpp			Conversion functions, memory stuff, etc.
//
//	Written by Gints Klimanis
//
// ****************************************************************
#include "util.h"

#ifndef AUDIOFX_STANDALONE
#include <AudioHw.h>

#ifndef BUILD_DRIVER
#include <CScratchDma.h>

// This is a dummy variable referenced by the Visual C++ compiler when
// compiling floating point code.  Since we can't use the Visual C++ run-time
// library, we create an instance of it here.
extern "C" int _fltused = 0x9875;
#endif // BUILD_DRIVER

#endif	// AUDIOFX_STANDALONE

#ifdef AUDIOFX_STANDALONE
// ***************************************************************
// afxScratchMemZero:	Clear some scratch memory.
//					NOTE: this function will disappear !
// *************************************************************** 
	void  
afxScratchMemZero(Int24 memBlock, long size)
{
memset((void *) memBlock, 0, size );
}	// ---- end afxScratchMemZero() ---- 

// ***************************************************************
// afxScratchMemSet:	Set some scratch memory.
//					NOTE: this function will disappear !
// *************************************************************** 
	void  
afxScratchMemSet(Int24 memBlock, long size, long value)
// value	Well, it's just the 8 bits, I think !
{
memset((void *) memBlock, value, size );
}	// ---- end afxScratchMemSet() ---- 

// ***************************************************************
// afxScratchMemRealloc:	Reallocate scratch memory.
// *************************************************************** 
	void  
afxScratchMemRealloc(Int24 *memBlock, long size, int clear)
{
afxScratchMemFree((void *) *memBlock);
*memBlock = (long) afxScratchMemAlloc(size);
if (*memBlock && clear)
	afxScratchMemZero(*memBlock, size);
}	// ---- end afxScratchMemRealloc() ---- 

// ***************************************************************
// afxSystemMemZero:	Clear some system memory.
//					NOTE: this function will disappear !
// *************************************************************** 
	void  
afxSystemMemZero(void *memBlock, long size)
{
memset(memBlock, 0, size );
}	// ---- end afxSystemMemZero() ---- 

// **********************************************************************************
// ByteSwap32:		Swap bytes in one 32-bit word
// ********************************************************************************** 
	void
ByteSwap32(long *p)
{
unsigned char x, y, *d = (unsigned char *) p;

x    = d[0];
d[0] = d[3];
d[3] = x;
y    = d[1];
d[1] = d[2];
d[2] = y;
}	// ---- end ByteSwap32() ---- 

// **********************************************************************************
// ByteSwapBuf32:		Do Endian byte swap on each 32-bit word in buffer
// ********************************************************************************** 
	void
ByteSwapBuf32(long *p, long count)
{
for (long i = 0; i < count; i++)
	{
	unsigned char x, y;
	unsigned char *b = (unsigned char *) p++;
	x    = b[0];
	b[0] = b[3];
	b[3] = x;
	y    = b[1];
	b[1] = b[2];
	b[2] = y;
	}
}	// ---- end ByteSwapBuf32() ---- 

// **********************************************************************************
// ByteSwap24:		Swap bytes in one 24-bit word
// ********************************************************************************** 
	void
ByteSwap24(Int24 *p)
{
unsigned char *b = (unsigned char *) p;
unsigned char x;

x    = b[0];
b[0] = b[2];
b[2] = x;
}	// ---- end ByteSwap24() ---- 

// **********************************************************************************
// ByteSwapBuf24:		Do byte swap on each 24-bit word in buffer
// ********************************************************************************** 
	void
ByteSwapBuf24(Int24 *p, long count)
{
unsigned char *b = (unsigned char *) p;
unsigned char x;
for (long i = 0; i < count; i++, b += 3)
	{
	x    = b[0];
	b[0] = b[2];
	b[2] = x;
	}
}	// ---- end ByteSwapBuf24() ---- 

#else		// AUDIOFX_STANDALONE

// system memory functions
void *afxSystemMemAlloc(U032 uSize)
{
    void *pLinAddr = NULL;
    aosAllocateMem(uSize, ALIGN_DONT_CARE, &pLinAddr, NULL);
    return pLinAddr;
}

void afxSystemMemFree(void *pLinAddr)
{
    aosFreeMem(pLinAddr);
}

void afxSystemMemCopy(void *pDest, void *pSrc, U032 uSize)
{
    aosMemCopy(pDest, pSrc, uSize);
}

void afxScratchMemZero(void *ptr, U032 uSize)
{
    aosZeroMemory(ptr, uSize);
}

// scratch memory functions
#define MAX_ALLOCATIONS     256
#define INVALID_OFFSET      ~0

CScratchDma *gpDma = NULL;

struct
{
    U032 uLinOffset;
    U032 uSize;
} gAllocMap[MAX_ALLOCATIONS];

BOOL AllocMapStoreEntry(U032 uLinOffset, U032 uSize)
{
    for (U032 uCnt = 0; uCnt < MAX_ALLOCATIONS; uCnt++)
    {
        if (gAllocMap[uCnt].uLinOffset == INVALID_OFFSET)
        {
            gAllocMap[uCnt].uLinOffset = uLinOffset;
            gAllocMap[uCnt].uSize = uSize;
            return TRUE;
        }
    }
    
    aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: AllocMapStoreEntry - ERROR! increase MAX_ALLOCATIONS\n");
    return FALSE;
}

BOOL AllocMapGetSize(U032 uLinOffset, U032 *pSize)
{
    for (U032 uCnt = 0; uCnt < MAX_ALLOCATIONS; uCnt++)
    {
        if (gAllocMap[uCnt].uLinOffset == uLinOffset)
        {
            *pSize = gAllocMap[uCnt].uSize;
            return TRUE;
        }
    }
    
    aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: AllocMapGetSize - bad linear offset!\n");
    return FALSE;
}

BOOL AllocMapFreeEntry(U032 uLinOffset)
{
    for (U032 uCnt = 0; uCnt < MAX_ALLOCATIONS; uCnt++)
    {
        if (gAllocMap[uCnt].uLinOffset == uLinOffset)
        {
            gAllocMap[uCnt].uLinOffset = INVALID_OFFSET;
            gAllocMap[uCnt].uSize = 0;
            return TRUE;
        }
    }
    
    aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: AllocMapFreeEntry - bad linear offset!\n");
    return FALSE;
}

// called from audiofx object
void afxScratchMemInitialize(CScratchDma *pDma)
{
    gpDma = pDma;
    
    for (U032 uCnt = 0; uCnt < MAX_ALLOCATIONS; uCnt++)
    {
        gAllocMap[uCnt].uLinOffset = INVALID_OFFSET;
        gAllocMap[uCnt].uSize = 0;
    }
}

void afxScratchMemShutdown()
{
    // check for memory leaks here
    gpDma = NULL;
}


BOOL afxScratchMemAlloc(U032 uSize, U032 *pOffset)
{
    BOOL bStatus = FALSE;
    
    if ( (gpDma) && (RM_OK == gpDma->Allocate(uSize, pOffset)) )
        bStatus = TRUE;
    
    if (bStatus == (BOOL)TRUE)
        bStatus = AllocMapStoreEntry(*pOffset, uSize);
    
    return bStatus;
}

void afxScratchMemFree(U032 uOffset)
{
    U032 uSize;
    
    if ( AllocMapGetSize(uOffset, &uSize) == (BOOL)TRUE )
    {
        if (gpDma)
            gpDma->Free(uOffset, uSize);
        
        AllocMapFreeEntry(uOffset);
        
    }
}

void afxScratchMemWrite(U032 uOffset, void *pSource, U032 uSize)
{
    if (gpDma)
        gpDma->Write(uOffset, pSource, uSize);
    else
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: afxScratchMemCopy - scratch DMA not initialized!\n");
}

void afxScratchMemRead(void *pDest, U032 uOffset, U032 uSize)
{
    if (gpDma)
        gpDma->Read(pDest, uOffset, uSize);
    else
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: afxScratchMemCopy - scratch DMA not initialized!\n");
    
}

BOOL  afxScratchMemRealloc(U032 *pOffset, U032 uNewSize, BOOL bClear)
{
    U032 uOldSize;
    BOOL bStatus = FALSE;
    
    if ( AllocMapGetSize(*pOffset, &uOldSize) == (BOOL)TRUE )
    {
        if (uOldSize > uNewSize)
        {
            U032 uExtraOffset = *pOffset + uNewSize;
            if (gpDma)
            {
                gpDma->Free(uExtraOffset, uOldSize - uNewSize);
                // update the alloc map
                AllocMapFreeEntry(*pOffset);
                AllocMapStoreEntry(*pOffset, uNewSize);
                bStatus = TRUE;
            }
        }
        else
        {
            // free and allocate again
            afxScratchMemFree(*pOffset);
            bStatus = afxScratchMemAlloc(uNewSize, pOffset);
        }
    }
    
    if ( (bStatus == (BOOL)TRUE) && (bClear == (BOOL)TRUE) )
        afxScratchMemZero(*pOffset, uNewSize);
    
    return bStatus;
}
void afxScratchMemZero(U032 uOffset, U032 uSize)
{
    afxScratchMemSet(uOffset, uSize, 0);
}

void afxScratchMemSet(U032 uOffset, U032 uSize, U032 uValue)
{
    if (gpDma)
        gpDma->Put(uOffset, uSize, uValue);
}

#endif


// ***************************************************************
// FloatToInt24:	Convert float to 24-bit fractional int
// *************************************************************** 
	Int24 
FloatToInt24(float x)
{
#ifdef AUDIOFX_STANDALONE
return ((Int24)(k24Norm*x));
#else
long lTemp = DTOL(k24Norm*x);
return (Int24)lTemp;
#endif
}	// ---- end FloatToInt24() ---- 

// ***************************************************************
// DoubleToInt24:	Convert double to 24-bit fractional int
// *************************************************************** 
	Int24 
DoubleToInt24(double x)
{
#ifdef AUDIOFX_STANDALONE
return ((Int24)(k24Norm*x));
#else
long lTemp = DTOL(k24Norm*x);
return (Int24)lTemp;
#endif
}	// ---- end DoubleToInt24() ---- 
