        TITLE   "Reader/Writer Lock Routines"
;++
;
;  Copyright (c) 2001  Microsoft Corporation
;
;  Module Name:
;
;     rwlocka.asm
;
;  Abstract:
;
;     This module implements functions to support executive reader/writer locks.
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

        EXTRNP  _KeReleaseSemaphore,4
        EXTRNP  _KeSetEvent,3
        EXTRNP  _KeWaitForSingleObject,5

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

ReadWriteLock equ     [esp + 4]

;++
;
; VOID
; ExAcquireReadWriteLockExclusive(
;     IN PERWLOCK ReadWriteLock
;     )
;
; Routine Description:
;
;     This function acquires a reader/writer lock for exclusive access.
;
; Arguments:
;
;     ReadWriteLock - Supplies the address of the reader/writer lock.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _ExAcquireReadWriteLockExclusive, 1

;
; Load the read/writer lock address into a register and disable interrupts to
; synchronize access to the lock.
;

        mov     ecx, ReadWriteLock
        cli

;
; Increment the lock count.  If the count doesn't transition from negative one
; to zero, then a reader or writer already owns the lock.  Otherwise, we own the
; write lock, so ReadersEntryCount should be zero.
;

        inc     dword ptr ErwLockCount[ecx]
        jnz     ContentionExclusive

        sti
        stdRET  _ExAcquireReadWriteLockExclusive

;
; Block until all of the readers or the writer release the lock.
;

ContentionExclusive:
        inc     dword ptr ErwWritersWaitingCount[ecx]
        sti
        add     ecx, ErwWriterEvent
.errnz (WrExecutive - 0)
        xor     edx, edx
        stdCall _KeWaitForSingleObject,<ecx,edx,edx,edx,edx>
        stdRET  _ExAcquireReadWriteLockExclusive

stdENDP _ExAcquireReadWriteLockExclusive

;++
;
; VOID
; ExAcquireReadWriteLockShared(
;     IN PERWLOCK ReadWriteLock
;     )
;
; Routine Description:
;
;     This function acquires a reader/writer lock for shared access.
;
; Arguments:
;
;     ReadWriteLock - Supplies the address of the reader/writer lock.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _ExAcquireReadWriteLockShared, 1

;
; Load the read/writer lock address into a register and disable interrupts to
; synchronize access to the lock.
;

        mov     ecx, ReadWriteLock
        cli

;
; Increment the lock count.  If the count doesn't transition from negative one
; to zero, then a reader or writer already owns the lock.  Otherwise, we own the
; read lock, so increment the number of readers.
;

        inc     dword ptr ErwLockCount[ecx]
        jnz     ContentionShared

ContentionNoWriters:
        inc     dword ptr ErwReadersEntryCount[ecx]
        sti
        stdRET  _ExAcquireReadWriteLockShared

;
; Check if there's a writer that already owns the lock or if there are any
; waiting writers.  If not, we can share the lock with the existing reader.
; Otherwise, block until the writer completes or the next writer has completed.
;

ContentionShared:
        xor     edx, edx
        cmp     dword ptr ErwReadersEntryCount[ecx], edx
        je      ContentionBlockForWriter
        cmp     dword ptr ErwWritersWaitingCount[ecx], edx
        je      ContentionNoWriters

ContentionBlockForWriter:
        inc     dword ptr ErwReadersWaitingCount[ecx]
        sti
        add     ecx, ErwReaderSemaphore
.errnz (WrExecutive - 0)
        stdCall _KeWaitForSingleObject,<ecx,edx,edx,edx,edx>
        stdRET  _ExAcquireReadWriteLockShared

stdENDP _ExAcquireReadWriteLockShared

;++
;
; VOID
; ExReleaseReadWriteLock(
;     IN PERWLOCK ReadWriteLock
;     )
;
; Routine Description:
;
;     This function releases a reader/writer lock.
;
; Arguments:
;
;     ReadWriteLock - Supplies the address of the reader/writer lock.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _ExReleaseReadWriteLock, 1

;
; Load the read/writer lock address into a register and disable interrupts to
; synchronize access to the lock.
;

        mov     ecx, ReadWriteLock
        cli

;
; Decrement the lock count.  If the count is greater than or equal to zero, then
; there are either shared readers using the lock or there are waiters for the
; lock.
;

        dec     dword ptr ErwLockCount[ecx]
        jge     ReleaseContention

;
; Zero out the number of readers that have entered the lock.  Either a writer
; is releasing the lock, in which case this count is already zero, or else the
; last reader is releasing the lock, in which case we want the count to
; decrement to zero.
;

        mov     dword ptr ErwReadersEntryCount[ecx], 0

ReleaseExit:
        sti
        stdRET  _ExReleaseReadWriteLock

;
; Check if the lock is owned by a single writer or one or more readers.
;

ReleaseContention:
        cmp     dword ptr ErwReadersEntryCount[ecx], 0
        mov     eax, dword ptr ErwReadersWaitingCount[ecx]
        mov     edx, dword ptr ErwWritersWaitingCount[ecx]
        je      ReleaseByWriter

;
; The lock is owned by a reader, so decrement the number of readers that have
; entered the lock.  If the reader count reaches zero, then there must be a
; waiting writer that needs to be signaled.
;

ReleaseByReader:
        dec     dword ptr ErwReadersEntryCount[ecx]
        jnz     ReleaseExit

ReleaseWriter:
        dec     dword ptr ErwWritersWaitingCount[ecx]
        sti
        add     ecx, ErwWriterEvent
        stdcall _KeSetEvent,<ecx,EVENT_INCREMENT,0>
        stdRET  _ExReleaseReadWriteLock

;
; The lock is owned by a writer.  If there aren't any readers waiting for the
; lock, then there must be another writer.  Otherwise, unblock as many readers
; as are currently queued up for the lock.
;

ReleaseByWriter:
        test    eax, eax
        jz      ReleaseWriter

ReleaseReaders:
        mov     dword ptr ErwReadersEntryCount[ecx], eax
        xor     edx, edx
        mov     dword ptr ErwReadersWaitingCount[ecx], edx
        sti
        add     ecx, ErwReaderSemaphore
        stdcall _KeReleaseSemaphore,<ecx,EVENT_INCREMENT,eax,edx>
        stdRET  _ExReleaseReadWriteLock

stdENDP _ExReleaseReadWriteLock

_TEXT   ENDS

        END
