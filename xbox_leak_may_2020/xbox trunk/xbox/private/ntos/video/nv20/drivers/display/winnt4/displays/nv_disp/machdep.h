//******************************************************************************
//
// Module Name: machdep.h
//
// Macros to hide Win32/Win64 machine dependent code.
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1998 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/


#ifdef _WIN64

#define	NV_BREAKPOINT()                                                     \
{                                                                           \
    __break(0x80016);                                                       \
}

#define NV_EXCHANGE(lock, key)                                              \
{                                                                           \
    key = _InterlockedExchange(lock, key);                                  \
}

#else // _WIN32

#define	NV_BREAKPOINT()                                                     \
{                                                                           \
    __asm int 3                                                             \
}

#define NV_EXCHANGE(lock, key)                                              \
{                                                                           \
    __asm mov  ebx, key                                                     \
    __asm mov  eax, (lock)                                                  \
    __asm xchg [eax], ebx                                                   \
    __asm mov  key, ebx                                                     \
}
#endif
