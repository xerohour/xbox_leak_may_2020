/*****************************************************************************
*
*  MODULE: memmgt.H
*  AUTHOR: JohnMil
*  DATE:   2-4-92
*
*  Copyright (c) 1991,1992 Microsoft Corporation
*
*  Header file for Memmgt.C that houses the setup and cleanup functions for
*  the Win32 Memory Management component.
*
*  History:
*
*	Nov 1, 1991	JohnMil	Initial Version
*	Feb 29, 1992	JohnMil renamed, modified for BadMan 2.0
*   April 2, 1992	TomMcG revisions for new/changed flags
*
****************************************************************************/

// All possible flags for GlobalAlloc()
#define MM_GPFLAGS 	( GMEM_FIXED 	   | GMEM_MOVEABLE   | GMEM_NOCOMPACT |	\
					  GMEM_NODISCARD   | GMEM_ZEROINIT   | 					\
					  GMEM_DISCARDABLE | GMEM_NOT_BANKED | GMEM_DDESHARE  |	\
					  GMEM_SHARE	   | GMEM_NOTIFY     | GMEM_LOWER )

// All possible flags for GlobalReAlloc()
#define MM_GRFLAGS	( GMEM_DISCARDABLE | GMEM_MODIFY   | GMEM_MOVEABLE |	\
					  GMEM_NODISCARD   | GMEM_ZEROINIT )

// All possible flags for LocalAlloc()
#define MM_LPFLAGS 	( LMEM_FIXED 	 | LMEM_MOVEABLE | LMEM_NOCOMPACT   |	\
					  LMEM_NODISCARD | LMEM_ZEROINIT | LMEM_DISCARDABLE )

// All possible flags for LocalReAlloc()
#define MM_LRFLAGS	( LMEM_DISCARDABLE | LMEM_MODIFY | LMEM_MOVEABLE |		\
					  LMEM_ZEROINIT )

#define GMEM_DEF_FLAGS (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define LMEM_DEF_FLAGS (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define MM_GLOBAL 1
#define MM_LOCAL 0


/*********************** Function Prototypes ******************************/

HANDLE	LMemHandleCreate(HANDLE hLog, HANDLE hConOut, int CaseNo);
HANDLE	GMemHandleCreate(HANDLE hLog, HANDLE hConOut, int CaseNo);
HANDLE	HeapHandleCreate(HANDLE hLog, HANDLE hConOut, int CaseNo);
LPSTR LMemPointerCreate(HANDLE hLog, HANDLE hConOut, int CaseNo,LPVOID *SCInfo);
LPSTR GMemPointerCreate(HANDLE hLog, HANDLE hConOut, int CaseNo,LPVOID *SCInfo);
LPSTR HeapPointerCreate(HANDLE hLog, HANDLE hConOut, int CaseNo,LPVOID *SCInfo);
LPVOID VirtAddrCreate(HANDLE hLog, HANDLE hConOut, int CaseNo,LPVOID *SCInfo);
