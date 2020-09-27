/*
 * Test different copy speeds
 */

#include <xtl.h>
#include <stdio.h>
#include <mmintrin.h>

ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

#define CHECK_RESULTS 0
#define BLOCK_SIZE    (511)
#define ITERATIONS    100000

#define PAGE_SIZE 0x1000

/*
 * Helpers
 */

#if CHECK_RESULTS

    #define INIT(x)        Init(x)
    #define PREP(x)        Prep(x)
    #define VERIFY(x, y)   Verify(x, y)
    #define COPYSIZE(x)    (x)

#else            
    
    #define INIT(x)
    #define PREP(x)  
    #define VERIFY(x, y)
    #define COPYSIZE(x)    (BLOCK_SIZE)

#endif

void Init(
    DWORD *pArray
    )
{
    DWORD i;

    for (i = 0; i < BLOCK_SIZE / 4; i++)
    {
        pArray[i] = 0xFFFFFFFF - i;
    }

    pArray[i] = 0;
}

void Prep(
    DWORD *pArray
    )
{
    memset(pArray, 0, (BLOCK_SIZE + 1) * 4);
}

void Verify(
    DWORD *pArray,
    DWORD Size
    )
{
    DWORD i;

    for (i = 0; i < Size; i++)
    {
        if (pArray[i] != 0xFFFFFFFF - i)
        {
            _asm int 3;
        }
    }

    if (pArray[i] != 0)
    {
        _asm int 3;
    }
}

/*
 * Copy methods
 */

//----------------------------------------------------------------------------
// Uses SSE to copy chunks of memory around.
//
void FastDWORDCopy(
    void *pDest, 
    void *pSource, 
    unsigned Count
    )
{
    _asm
    {
        // int 3

        mov eax, pSource
        mov ebx, pDest

        mov ecx, Count   
        shr ecx, 4

        mov ecx, Count
        and ecx, 0xFFFFFFF0
        shr ecx, 4

        jz copy_16

    copy_64_loop:

        movq mm0, [eax]
        movq mm1, [eax + 8]
        movq mm2, [eax + 16]
        movq mm3, [eax + 24]
        movq mm4, [eax + 32]
        movq mm5, [eax + 40]
        movq mm6, [eax + 48]
        movq mm7, [eax + 56]
            
        movntq [ebx],      mm0
        movntq [ebx + 8],  mm1
        movntq [ebx + 16], mm2
        movntq [ebx + 24], mm3     
        movntq [ebx + 32], mm4
        movntq [ebx + 40], mm5
        movntq [ebx + 48], mm6
        movntq [ebx + 56], mm7     

        add eax, 0x40
        add ebx, 0x40

        dec ecx
        jnz copy_64_loop

    copy_16:

        mov ecx, Count
        and ecx, 0xC
        shr ecx, 2

        jz copy_4

    copy_16_loop:
    
        movq mm0, [eax]
        movq mm1, [eax + 8]

        movntq [ebx],      mm0
        movntq [ebx + 8],  mm1

        add eax, 0x10
        add ebx, 0x10

        dec ecx
        jnz copy_16_loop

    copy_4:

        mov ecx, Count
        and ecx, 0x03

        jz done

    copy_4_loop:

        mov edx, [eax]
        mov [ebx], edx
        
        add eax, 0x04
        add ebx, 0x04

        dec ecx
        jnz copy_4_loop

    done:
    }
}

/*
 * Test
 */

void __cdecl main()
{
    _int64 StartTime;
    _int64 StopTime;

    DWORD *pSource;
    DWORD *pDest;

    DWORD i, SourceOffset, DestinationOffset;

    pSource = (DWORD *)malloc((BLOCK_SIZE + 32) * 4);
    pDest   = (DWORD *)XPhysicalAlloc((BLOCK_SIZE + 32) * 4, 
                                      MAXULONG_PTR, 
                                      PAGE_SIZE,
                                      PAGE_READWRITE | PAGE_WRITECOMBINE);

    pSource = (DWORD *)(((DWORD)pSource + 15) & ~15);
    pDest   = (DWORD *)(((DWORD)pDest   + 15) & ~15);

    INIT(pSource);

    DbgPrint("\n");

    // memcpy

    for (DestinationOffset = 0; DestinationOffset <= 0; DestinationOffset += 2)
    {
        for (SourceOffset = 0; SourceOffset <= 0; SourceOffset += 2)
        {
            QueryPerformanceCounter((LARGE_INTEGER *)&StartTime);

            for (i = 0; i < ITERATIONS; i++)
            {
                PREP(pDest);

                memcpy((BYTE *)pDest + DestinationOffset, 
                       (BYTE *)pSource + SourceOffset, 
                       COPYSIZE(i) * 4);

                VERIFY(pDest, COPYSIZE(i));
            }

            QueryPerformanceCounter((LARGE_INTEGER *)&StopTime);

            DbgPrint("memcpy   [src: %02d  dst: %02d] - %d ticks\n", 
                     SourceOffset, 
                     DestinationOffset, 
                     (DWORD)(StopTime - StartTime));

            // SSE

            QueryPerformanceCounter((LARGE_INTEGER *)&StartTime);

            for (i = 0; i < ITERATIONS; i++)
            {
                PREP(pDest);

                FastDWORDCopy((BYTE *)pDest + DestinationOffset, 
                              (BYTE *)pSource + SourceOffset, 
                              COPYSIZE(i));

                VERIFY(pDest, COPYSIZE(i));
            }

            QueryPerformanceCounter((LARGE_INTEGER *)&StopTime);

            DbgPrint("fastcopy [src: %02d  dst: %02d] - %d ticks\n", 
                     SourceOffset,
                     DestinationOffset,
                     (DWORD)(StopTime - StartTime));
        }
    }

    // Spin forever.
    DbgPrint("\nDone\n");

    for (;;)
        Sleep(10);
}
