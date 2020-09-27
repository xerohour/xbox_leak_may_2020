/*** bm.h - Memory Manager routines
*
*	Copyright <C> 1990, Microsoft Corporation
*
* Purpose:  handle the near and far memory requests of dlls & linked list
*
*
*************************************************************************/

BOOL PASCAL        BMInit ( VOID );
HMEM PASCAL LOADDS BMAlloc ( UINT );
HMEM PASCAL LOADDS BMRealloc ( HMEM, UINT );
VOID PASCAL LOADDS BMFree ( HMEM );
LPV  PASCAL LOADDS BMLock ( HMEM );
VOID PASCAL LOADDS BMUnlock ( HMEM );
BOOL PASCAL LOADDS BMIsLocked ( HMEM );

#ifndef BMHANDLES
#define BMLock(HMEM) ((LPV)(HMEM))
#define BMUnlock(HMEM)
#endif
