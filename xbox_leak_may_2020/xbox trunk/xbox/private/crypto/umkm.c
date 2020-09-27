/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    umkm.c

Abstract:

    Implements memory allocation functions required by RSA32K.LIB


--*/

#ifdef KMODE_RNG

#include <ntos.h>

void*
__stdcall
RSA32Alloc(unsigned long size)
{
    return (void*)ExAllocatePoolWithTag(size, 'yrCX');
}

void
__stdcall
RSA32Free(void* mem)
{
    ExFreePool(mem);
}

#endif
