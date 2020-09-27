/*
 *
 * dbf.h
 *
 * Debug memory functions
 *
 */

#ifndef _DBF_H
#define _DBF_H

#define MHAlloc(cb) ExAllocatePoolWithTag(PagedPool, cb, 'cvsm')
//PVOID        MHAlloc        ( size_t );
PVOID        MHRealloc      ( LPVOID, size_t );
//VOID         MHFree         ( LPVOID );
#define MHFree(pv) ExFreePoolWithTag(pv, 'cvsm');

HLLI         LLInit         ( DWORD,
                               LLF,
                               LPFNKILLNODE,
                               LPFNFCMPNODE );
HLLE         LLCreate       ( HLLI );
VOID         LLAdd          ( HLLI, HLLE );
BOOL         LLDelete       ( HLLI, HLLE );
HLLE         LLNext         ( HLLI, HLLE );
HLLE         LLFind         ( HLLI,
                               HLLE,
                               LPVOID,
                               DWORD );
PVOID        LLLock         ( HLLE );
//VOID         LLUnlock       ( HLLE );
#define LLUnlock(hlle)

#endif // _DBF_H
