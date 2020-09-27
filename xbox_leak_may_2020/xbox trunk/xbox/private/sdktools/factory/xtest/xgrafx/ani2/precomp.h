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

#ifndef XSS_TEST
inline void* MemAlloc(UINT cBytes)
#else
inline void* MemAlloc_(UINT cBytes)
#endif
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
#ifndef XSS_TEST
    return MemAlloc(cBytes);
#else
    return MemAlloc_(cBytes);
#endif
}

#ifndef XSS_TEST
inline void MemFree(void* pv)
#else
inline void MemFree_(void* pv)
#endif
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

