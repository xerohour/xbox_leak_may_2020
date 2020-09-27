/*
    FILE:   cmntypes.h
    DATE:   10/12/1999

    This file contains common type definitions needed in other
    include files and source files so that they don't need to be
    defined in multiple places.
*/

// A few constants
#define TRUE    1
#define FALSE   0
#define NULL    0

// Need these for C To ASM conversion reason.
#ifndef FAR
#ifdef _WIN32
#define FAR
#define CFUNC   __cdecl
#define WINAPI  _stdcall
#else   // !_WIN32

#if     defined(LINUX) | defined(MAC)

#define FAR 
#define CFUNC
#define WINAPI

#else   // !LINUX
#define FAR __far
#define CFUNC   __cdecl  _loadds
#define WINAPI  _far _pascal  _loadds
#endif  // !LINUX
#endif  // !WIN32
#endif  // FAR

// A few basic types
#ifndef ULONG
typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef UCHAR  FAR *    LPCHAR;
typedef USHORT FAR *    LPUSHORT;
typedef ULONG  FAR *    LPULONG;
#endif  // ULONG



#ifdef LINUX
#define __STRUCTURE_PACKING__ __attribute__((packed))
#else
#define __STRUCTURE_PACKING__
#endif
