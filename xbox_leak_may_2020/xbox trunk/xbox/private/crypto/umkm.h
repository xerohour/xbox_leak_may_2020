 /*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    umkm.h

Abstract:


--*/


#ifndef _UMKM_H
#define _UMKM_H

#ifdef KMODE_RNG

//#include <rsa_sys.h>

// 
// kernel mode
//
#define ALLOC(cb) RSA32Alloc(cb)
#define FREE(pv) RSA32Free(pv)

void* __stdcall RSA32Alloc(unsigned long cb);
void __stdcall RSA32Free(void* pv);


#else

// 
// user mode
//

#define ALLOC(cb) LocalAlloc(0, cb)
#define FREE(pv) LocalFree(pv)

#endif  // KMODE_RSA32

#endif  // _UMKM_H
