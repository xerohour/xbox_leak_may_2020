        TITLE   "File Object Lock Routines"
;++
;
;  Copyright (c) 2001  Microsoft Corporation
;
;  Module Name:
;
;     filelock.asm
;
;  Abstract:
;
;     This module implements functions to support locking FILE_OBJECTs for
;     synchronous I/O operations.
;
;  Environment:
;
;     Kernel mode only.
;
;--

        .586p
        .xlist
INCLUDE ks386.inc
INCLUDE callconv.inc
        .list

        EXTRNP  _KeSetEvent,3
        EXTRNP  _KeWaitForSingleObject,5

_TEXT SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

;++
;
; VOID
; FASTCALL
; IopAcquireFileObjectLock(
;     IN PFILE_OBJECT FileObject
;     )
;
; Routine Description:
;
;     This function acquires a file object's synchronous I/O lock.
;
; Arguments:
;
;     FileObject - Specifies the file object to lock.
;
; Return Value:
;
;     None.
;
;--
cPublicFastCall IopAcquireFileObjectLock, 1

;
; Increment the lock count.  If the count doesn't transition from negative one
; to zero, then another thread already owns the lock and we need to block.
;

        inc     dword ptr FoLockCount[ecx]
        jnz     AcquireContention
        fstRET  IopAcquireFileObjectLock

AcquireContention:
        xor     eax, eax
        add     ecx, FoLock
.errnz (WrExecutive - 0)
        stdCall _KeWaitForSingleObject,<ecx,eax,eax,eax,eax>
        fstRET  IopAcquireFileObjectLock

fstENDP IopAcquireFileObjectLock

;++
;
; VOID
; FASTCALL
; IopReleaseFileObjectLock(
;     IN PFILE_OBJECT FileObject
;     )
;
; Routine Description:
;
;     This function releases a file object's synchronous I/O lock.
;
; Arguments:
;
;     FileObject - Specifies the file object to unlock.
;
; Return Value:
;
;     None.
;
;--
cPublicFastCall IopReleaseFileObjectLock, 1

;
; Decrement the lock count.  If the lock count is greater than or equal to zero,
; then there are waiters for this lock, so release a waiter.
;

        dec     dword ptr FoLockCount[ecx]
        jge     ReleaseWaiter
        fstRET  IopReleaseFileObjectLock

ReleaseWaiter:
        add     ecx, FoLock
        stdCall _KeSetEvent,<ecx,0,0>
        fstRET  IopReleaseFileObjectLock

fstENDP IopReleaseFileObjectLock

_TEXT ENDS

        END
