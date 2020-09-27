#pragma once

#ifdef STARTUPANIMATION

#pragma code_seg("INIT")
#pragma data_seg("INIT_RW")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

extern "C"
{
    #include <ntos.h>
    #include <nturtl.h>
};

#endif // STARTUPANIMATION

#include <xtl.h>
#include <xgraphics.h>

#include <stdio.h>

#ifdef _DEBUG
extern int gcMemAllocs;

#ifdef STARTUPANIMATION
extern "C" int gcMemAllocsContiguous;
#endif // STARTUPANIMATION
#endif

inline void* MemAlloc(UINT cBytes)
{
#ifdef _DEBUG
    gcMemAllocs++;
#endif

#ifndef STARTUPANIMATION
    void* p = LocalAlloc(LMEM_FIXED, cBytes);
    if (p)
    {
        ZeroMemory(p, cBytes);
    }
    return p;
#else // STARTUPANIMATION
    void* p = ExAllocatePoolWithTag(cBytes, 'KD3D');
    if (p)
    {
        // Last-minute hack to fix start-up animation hangs [andrewgo]
        ZeroMemory(p, cBytes);
    }
    return p;
#endif // STARTUPANIMATION
}

inline void* MemAllocNoZero(ULONG cBytes)
{
    return MemAlloc(cBytes);
}

inline void MemFree(void* pv)
{
#ifdef _DEBUG
    if (gcMemAllocs <= 0)
    {
        __asm int 3;
    }
    gcMemAllocs--;
#endif

#ifndef STARTUPANIMATION
    LocalFree(pv);
#else // STARTUPANIMATION
    ExFreePool(pv);
#endif // STARTUPANIMATION
}

#ifdef STARTUPANIMATION
extern "C"
void *MemAllocContiguous(size_t Size, DWORD Alignment);
extern "C"
void MemFreeContiguous(void *pv);
#endif // STARTUPANIMATION

#ifdef BINARY_RESOURCE
#include "shaders.h"
#endif // BINARY_RESOURCE

#include "fastmath.h"

