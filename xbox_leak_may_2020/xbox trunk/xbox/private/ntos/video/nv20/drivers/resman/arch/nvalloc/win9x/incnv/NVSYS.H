#ifndef _NVSYS_H_
#define _NVSYS_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVSYS.H                                                           *
*   Windows Resource Manager defines and prototypes.                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    01/10/95 - wrote it.                    *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------
#define DEFAULT_DEVICEHANDLE 0

//---------------------------------------------------------------------------
//
//  Typedefs.
//
//---------------------------------------------------------------------------

//
// Standard types not defined in windows.h
//
typedef          char      CHAR;
typedef          int       INT;
typedef          int       SHORT;
typedef          char     *PCHAR;
typedef          int      *PSHORT;
typedef          void     *PVOID;
typedef          char far *LPCHAR;
typedef          int  far *LPSHORT;
typedef unsigned char      UCHAR;
typedef unsigned int       USHORT;
typedef unsigned long      ULONG;
typedef unsigned char     *PUCHAR;
typedef unsigned int      *PUSHORT;
typedef unsigned long     *PULONG;
typedef unsigned char far *LPUCHAR;
typedef unsigned int  far *LPUSHORT;
typedef unsigned long far *LPULONG;
typedef struct _def_lrect
{
   LONG     left;
   LONG     top;
   LONG     right;
   LONG     bottom;
} LRECT;
typedef LRECT      * PLRECT;
typedef LRECT FAR  * LPLRECT;
//
// Callback type.
//
typedef VOID (FAR PASCAL * NVSYSCALLBACK)(DWORD, DWORD, WORD, DWORD, WORD);

//---------------------------------------------------------------------------
//
//  Typedefs.
//
//---------------------------------------------------------------------------

typedef struct _def_hw_info      FAR * LPHWINFO;
typedef struct _def_channel_info FAR * LPCHANNELINFO;
typedef struct _def_client_info  FAR * LPCLIENTINFO;
typedef struct _def_rmParams     FAR * LPRMPARAMS;

typedef CANVASREGION FAR * LPCANVASREGION;

//---------------------------------------------------------------------------
//
//  Prototypes.
//
//---------------------------------------------------------------------------

//
// System client entrypoints.
//
LPVOID FAR PASCAL NvSysOpen(U032);
LPVOID FAR PASCAL NvSysOpenDma(DWORD, DWORD, DWORD);
BOOL   FAR PASCAL NvSysClose(LPVOID, U032);
BOOL   FAR PASCAL NvSysInstallCallback(LPVOID, NVSYSCALLBACK);
//
// Internal helper routines.  Subject to change at my whim.
// Use configuration API instead.
//
BOOL  FAR PASCAL NvSysAllocPages(LPVOID FAR *, LPDWORD, DWORD, U032);
BOOL  FAR PASCAL NvSysFreePages(DWORD, U032);
BOOL  FAR PASCAL NvSysLockPages(WORD, DWORD, DWORD, U032);
BOOL  FAR PASCAL NvSysUnlockPages(WORD, DWORD, DWORD, U032);
VOID  FAR PASCAL NvSysMoveCursor(INT, INT, U032);
VOID  FAR PASCAL NvSysMoveCursorMulti(INT, INT, U032, U032);
BOOL  FAR PASCAL NvSysSetCursor(LPBYTE, INT, INT, U032);
DWORD FAR PASCAL NvSysSetColorCursor(LPBYTE, LPBYTE, INT, INT, U032);
DWORD FAR PASCAL NvSysSetColorCursor555(LPBYTE, LPBYTE, INT, INT);
VOID  FAR PASCAL NvSysSetCursorColor(DWORD, DWORD);
VOID  FAR PASCAL NvSysShowHideCursor(INT, U032);
VOID  FAR PASCAL NvSysDmaPushOperation(DWORD, LPVOID);
DWORD FAR PASCAL NvSysGetVBlankStatus(VOID);
DWORD FAR PASCAL NvSysGetGEStatus(VOID);
DWORD FAR PASCAL NvSysGetScreenInfo(U032);
DWORD FAR PASCAL NvSysGetRamInfo(U032);
DWORD FAR PASCAL NvSysGetBufferInfo(U032);
VOID  FAR PASCAL NvSysClearScreen(U032);
VOID  FAR PASCAL NvSysGetHardwarePointers(DWORD FAR *, DWORD FAR *, DWORD FAR *, DWORD FAR *, U032);
DWORD FAR PASCAL NvSysGetCurrentBufferFlatPointer(U032);
VOID  FAR PASCAL NvSysDisplayModeHook(DWORD, U032);
//
// Internal Resource Manager entrypoints.
//
BOOL  FAR PASCAL NvSysClientExit(INT);
BOOL  FAR PASCAL NvSysConnect(HINSTANCE, LPVOID FAR *, LPVOID FAR *, LPVOID FAR *, U032);
BOOL  FAR PASCAL NvSysDisconnect(HINSTANCE);
BOOL  FAR PASCAL NvSysSetCanvas(HWND);
BOOL  FAR PASCAL NvSysSetCanvasRect(LPLRECT);
BOOL  FAR PASCAL NvSysSetCanvasClip(LPLRECT, WORD, LPLRECT);
BOOL  FAR PASCAL NvSysSetCanvasRegion(HWND, LPLRECT, WORD, LPLRECT);
#endif // _NVSYS_H_
