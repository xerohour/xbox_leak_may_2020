//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//
#pragma once

extern void* DirectMusicAllocI(size_t cb);
extern void  DirectMusicFreeI(void *pv);
extern void* DirectMusicPhysicalAllocI(size_t dwSize);
extern void DirectMusicPhysicalFreeI(void* lpAddress);

static inline void * __cdecl operator new(size_t cb)
{
    return DirectMusicAllocI(cb);
}

static inline void __cdecl operator delete(void *pv)
{
    DirectMusicFreeI(pv);
}

static inline void * __cdecl operator new[](size_t cb)
{
    return DirectMusicAllocI(cb);
}

static inline void __cdecl operator delete[](void *pv)
{
    DirectMusicFreeI(pv);
}
