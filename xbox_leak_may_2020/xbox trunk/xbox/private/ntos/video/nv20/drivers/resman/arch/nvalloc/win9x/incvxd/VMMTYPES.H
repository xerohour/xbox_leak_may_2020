/*************************************************************************************
*
*	File: vmmtypes.h
*
*	Revision History:
*		Jeff Westerinen (jsw) - 05/97
*			- Changed QWORD to QUADWORD due to conflict with typedef in basedef.h
*
*************************************************************************************/

#ifndef _VMMTYPES_H_
#define _VMMTYPES_H_

#ifndef NULL
#define NULL               0
#endif
#ifndef VOID
#define VOID               void
#endif
#ifndef PASCAL
#define PASCAL             _pascal
#endif
#ifndef CDECL
#define CDECL              _cdecl
#endif

#ifndef BOOL
#define BOOL                unsigned long
#endif
#ifndef FALSE
#define FALSE              0
#endif
#ifndef TRUE
#define TRUE               (~FALSE)
#endif

#ifndef LONG
#define LONG               long
#endif
typedef unsigned char      BYTE;
typedef unsigned short     WORD;
typedef unsigned long      DWORD;
typedef double             QUADWORD;
typedef unsigned int       UINT;
typedef char *             PSTR;
typedef BYTE *             PBYTE;
typedef int *              PINT;
typedef UINT *             PUINT;
typedef WORD *             PWORD;
typedef DWORD *            PDWORD;
typedef QUADWORD *         PQWORD;

typedef union
{
    QUADWORD Desc64;
    DWORD Desc32[2];
    WORD  Desc16[4];
    BYTE  Desc8[8];
} DESCRIPTOR, *PDESCRIPTOR;

typedef union
{
    QUADWORD HandleAddress;
    struct
    {
        DWORD Handle;
        DWORD Address;
    } ha;
} PAGEALLOC, *PPAGEALLOC;

#ifndef LOBYTE
#define LOBYTE(w)          ((BYTE)(w))
#endif
#ifndef HIBYTE
#define HIBYTE(w)          ((BYTE)(((UINT)(w) >> 8) & 0xFF))
#endif
#ifndef LOWORD
#define LOWORD(l)          ((WORD)(DWORD)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)          ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#endif
#ifndef MAKELONG
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif

#endif // _VMMTYPES_H_
