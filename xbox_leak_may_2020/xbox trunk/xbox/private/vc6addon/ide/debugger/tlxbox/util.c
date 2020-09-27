/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    util.c

Abstract:

    This file contains a set of general utility routines for the
    Debug Monitor module

Author:

    Jim Schaad (jimsch) 9-12-92

Environment:

    Win32 user mode

--*/

#include "precomp.h"
#pragma hdrstop



extern EXPECTED_EVENT   masterEE, *eeList;

extern HTHDX        thdList;
extern HPRCX        prcList;
extern CRITICAL_SECTION csThreadProcList;
extern char  abEMReplyBuf[];      // Buffer for EM to reply to us in


static  HPRCX   HprcRead;
static  HANDLE  HFileRead = 0;          // Read File handle
static  LPB     LpbMemory = 0;          // Read File Address
static  ULONG   CbOffset = 0;           // Offset of read address





BOOL
AddrWriteMemory(
    HPRCX       hprc,
    HTHDX       hthd,
    LPADDR      paddr,
    LPVOID      lpv,
    DWORD       cb,
    LPDWORD     pcbWritten
    )
/*++

Routine Description:

    This function is used to do a verified write to memory.  Most of the
    time it will just do a simple call to WriteMemory but some times
    it will do validations of writes.

Arguments:

    hprc - Supplies the handle to the process

    paddr  - Supplies the address to be written at

    lpv    - Supplies a pointer to the bytes to be written

    cb     - Supplies the count of bytes to be written

    pcbWritten - Returns the number of bytes actually written

Return Value:

    TRUE if successful and FALSE otherwise

--*/

{
    BOOL        fRet;
    ADDR        addr;

    /*
     * Can't resolve linker indices from here.
     */

    assert(!(ADDR_IS_LI(*paddr)));
    if (ADDR_IS_LI(*paddr)) {
        return FALSE;
    }

    /*
     * Make a local copy to mess with
     */

    addr = *paddr;
    if (!ADDR_IS_FLAT(addr)) {
        fRet = TranslateAddress(hprc, hthd, &addr, TRUE);
        assert(fRet);
        if (!fRet) {
            return fRet;
        }
    }

    return DbgWriteMemory(hprc, (LPVOID) GetAddrOff(addr),
                              lpv, cb, pcbWritten);

}                               /* AddrWriteMemory() */


BOOL
AddrReadMemory(
    HPRCX       hprc,
    HTHDX       hthd,
    LPADDR      paddr,
    LPVOID      lpv,
    DWORD       cb,
    LPDWORD     lpRead
    )
/*++

Routine Description:

    Read data from a process, using a full ADDR packet.

Arguments:

    hprc - Supplies the process structure

    hthd - Supplies the thread structure.  This must be valid if the
            address is not flat; otherwise the thread is not used.

    paddr  - Supplies the address to read from

    lpv    - Supplies a pointer to the local buffer

    cb     - supplies the count of bytes to read

    lpRead - Returns the number of bytes actually read

Return Value:

    TRUE if successful and FALSE otherwise

--*/

{
    BOOL        fRet;
    ADDR        addr;
#ifndef KERNEL
    PBREAKPOINT bp;
    DWORD       offset;
    BP_UNIT     instr;
#endif

    /*
     * We can't resolve linker indices from here.
     */

    assert(!(ADDR_IS_LI(*paddr)));
    if (ADDR_IS_LI(*paddr)) {
        return FALSE;
    }

    /*
     * Make a local copy to mess with
     */

    addr = *paddr;
    if (!ADDR_IS_FLAT(addr)) {
        fRet = TranslateAddress(hprc, hthd, &addr, TRUE);
        assert(fRet);
        if (!fRet) {
            return fRet;
        }
    }

    if (!DbgReadMemory(hprc, (LPVOID) GetAddrOff(addr), lpv, cb, lpRead)) {
        return FALSE;
    }

#if 0
#ifndef KERNEL
    /* The memory has been read into the buffer now sanitize it : */
    /* (go through the entire list of breakpoints and see if any  */
    /* are in the range. If a breakpoint is in the range then an  */
    /* offset relative to the start address and the original inst */
    /* ruction is returned and put into the return buffer)        */

    for (bp=bpList->next; bp; bp=bp->next) {
        if (BPInRange(hprc, hthd, bp, &addr, *lpRead, &offset, &instr)) {
            if (offset < 0) {
                memcpy(lpv, ((char *) &instr) - offset,
                       sizeof(BP_UNIT) + offset);
            } else if (offset + sizeof(BP_UNIT) > *lpRead) {
                memcpy(((char *)lpv)+offset, &instr, *lpRead - offset);
            } else {
                *((BP_UNIT UNALIGNED *)((char *)lpv+offset)) = instr;
            }
        }
    }
#endif  // !KERNEL
#endif

    return TRUE;
}                               /* AddrReadMemory() */


#if 0
BOOL
SanitizedMemoryRead(
    HPRCX      hprc,
    HTHDX      hthd,
    LPADDR     paddr,
    LPVOID     lpb,
    DWORD      cb,
    LPDWORD    lpcb
    )

/*++

Routine Description:

    This routine is provided to do the actual read of memory.  This allows
    multiple routines in the DM to do the read through a single common
    interface.  This routine will correct the read memory for any breakpoints
    currently set in memory.

Arguments:

    hprc        - Supplies the process handle for the read

    hthd        - Supplies the thread handle for the read

    paddr       - Supplies the address to read memory from

    lpb         - Supplies the buffer to do the read into

    cb          - Supplies the number of bytes to be read

    lpcb        - Returns the number of bytes actually read

Return Value:

    TRUE on success and FALSE on failure

--*/

{
    DWORD       offset;
    BP_UNIT     instr;
    BREAKPOINT  *bp;

    if (!AddrReadMemory(hprc, hthd, paddr, lpb, cb, lpcb)) {
        return FALSE;
    }

#ifndef KERNEL
    /* The memory has been read into the buffer now sanitize it : */
    /* (go through the entire list of breakpoints and see if any  */
    /* are in the range. If a breakpoint is in the range then an  */
    /* offset relative to the start address and the original inst */
    /* ruction is returned and put into the return buffer)        */

    for (bp=bpList->next; bp; bp=bp->next) {
        if (BPInRange(hprc, hthd, bp, paddr, *lpcb, &offset, &instr)) {
            if (offset < 0) {
                memcpy(lpb, ((char *) &instr) - offset,
                       sizeof(BP_UNIT) + offset);
            } else if (offset + sizeof(BP_UNIT) > *lpcb) {
                memcpy(((char *)lpb)+offset, &instr, *lpcb - offset);
            } else {
                *((BP_UNIT UNALIGNED *)((char *)lpb+offset)) = instr;
            }
        }
    }
#endif  // !KERNEL

    return TRUE;
}

#endif

#if 0

ULONG
SetReadPointer(
    ULONG    cbOffset,
    int      iFrom
    )

/*++

Routine Description:

    This routine is used to deal with changing the location of where
    the next read should occur.  This will take effect on the current
    file pointer or debuggee memory pointer address.

Arguments:

    cbOffset    - Supplies the offset to set the file pointer at

    iFrom       - Supplies the type of set to be preformed.

Return Value:

    The new file offset

--*/

{
    if (LpbMemory == NULL) {
        CbOffset = SetFilePointer(HFileRead, cbOffset, NULL, iFrom);
    } else {
        switch( iFrom ) {
        case FILE_BEGIN:
            CbOffset = cbOffset;
            break;

        case FILE_CURRENT:
            CbOffset += cbOffset;
            break;

        default:
            assert(FALSE);
            break;
        }
    }

    return CbOffset;
}                               /* SetReadPointer() */


VOID
SetPointerToFile(
    HANDLE   hFile
    )

/*++

Routine Description:

    This routine is called to specify which file handle should be used for
    doing reads from

Arguments:

    hFile - Supplies the file handle to do future reads from

Return Value:

    None.

--*/

{
    HFileRead = hFile;
    HprcRead = NULL;
    LpbMemory = NULL;

    return;
}                               /* SetPointerToFile() */



VOID
SetPointerToMemory(
    HPRCX       hprc,
    LPVOID      lpv
    )

/*++

Routine Description:

    This routine is called to specify where in debuggee memory reads should
    be done from.

Arguments:

    hProc - Supplies the handle to the process to read memory from

    lpv   - Supplies the base address of the dll to read memory at.

Return Value:

    None.

--*/

{
    HprcRead = hprc;
    LpbMemory = lpv;
    HFileRead = NULL;

    return;
}                               /* SetPointerToMemory() */


BOOL
DoRead(
    LPVOID           lpv,
    DWORD            cb
    )

/*++

Routine Description:

    This routine is used to preform the actual read operation from either
    a file handle or from the dlls memory.

Arguments:

    lpv - Supplies the pointer to read memory into

    cb  - Supplies the count of bytes to be read

Return Value:

    TRUE If read was fully successful and FALSE otherwise

--*/

{
    DWORD       cbRead;

    if (LpbMemory) {
        if ( !DbgReadMemory( HprcRead, LpbMemory+CbOffset, lpv, cb, &cbRead ) ||
                (cb != cbRead) ) {
            return FALSE;
        }
        CbOffset += cb;
    } else if ((ReadFile(HFileRead, lpv, cb, &cbRead, NULL) == 0) ||
            (cb != cbRead)) {
        return FALSE;
    }
    return TRUE;
}                               /* DoRead() */
#endif // 0


BOOL
AreAddrsEqual(
    HPRCX     hprc,
    HTHDX     hthd,
    LPADDR    paddr1,
    LPADDR    paddr2
    )

/*++

Routine Description:

    This function is used to compare to addresses for equality

Arguments:

    hprc    - Supplies process for address context

    hthd    - Supplies thread for address context

    paddr1  - Supplies a pointer to an ADDR structure

    paddr2  - Supplies a pointer to an ADDR structure

Return Value:

    TRUE if the addresses are equivalent

--*/

{
    ADDR        addr1;
    ADDR        addr2;

    /*
     *  Step 1.  Addresses are equal if
     *          - Both addresses are flat
     *          - The two offsets are the same
     */

    if ((ADDR_IS_FLAT(*paddr1) == TRUE) &&
        (ADDR_IS_FLAT(*paddr1) == ADDR_IS_FLAT(*paddr2)) &&
        (paddr1->addr.off == paddr2->addr.off)) {
        return TRUE;
    }

    /*
     * Step 2.  Address are equal if the linear address are the same
     */

    addr1 = *paddr1;
    addr2 = *paddr2;

    if (addr1.addr.off == addr2.addr.off) {
        return TRUE;
    }

    return FALSE;
}                               /* AreAddrsEqual() */




HTHDX
HTHDXFromPIDTID(
    PID pid,
    TID tid
    )
{
    HTHDX hthd;

    EnterCriticalSection(&csThreadProcList);
    for ( hthd = thdList->next; hthd; hthd = hthd->next ) {
        if (hthd->tid == tid && hthd->hprc->pid == pid ) {
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);
    return hthd;
}



HTHDX
HTHDXFromHPIDHTID(
    HPID hpid,
    HTID htid
    )
{
    HTHDX hthd;

    EnterCriticalSection(&csThreadProcList);
    for(hthd = thdList->next; hthd; hthd = hthd->next) {
        if (hthd->htid == htid && hthd->hprc->hpid == hpid ) {
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);
    return hthd;
}




HPRCX
HPRCFromPID(
    PID pid
    )
{
    HPRCX hprc;

    EnterCriticalSection(&csThreadProcList);
    for( hprc = prcList->next; hprc; hprc = hprc->next) {
        if (hprc->pid == pid) {
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);
    return hprc;
}



HPRCX
HPRCFromHPID(
    HPID hpid
    )
{
    HPRCX hprc;

    EnterCriticalSection(&csThreadProcList);
    for ( hprc = prcList->next; hprc; hprc = hprc->next ) {
        if (hprc->hpid == hpid) {
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);
    return hprc;
}



HPRCX
HPRCFromRwhand(
    HANDLE rwHand
    )
{
    HPRCX hprc;

    EnterCriticalSection(&csThreadProcList);
    for ( hprc=prcList->next; hprc; hprc=hprc->next ) {
        if (hprc->rwHand==rwHand) {
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);
    return hprc;
}


void
FreeHthdx(
    HTHDX hthd
    )
{
    HTHDX *             ppht;
    BREAKPOINT *        pbp;
    BREAKPOINT *        pbpT;

    EnterCriticalSection(&csThreadProcList);

    /*
     *  Free all breakpoints unique to thread
     */

    ClearBPFlag(hthd);
    for (pbp = BPNextHthdPbp(hthd, NULL); pbp; pbp = pbpT) {
        pbpT = BPNextHthdPbp(hthd, pbp);
        RemoveBP(pbp);
    }


    for (ppht = &(hthd->hprc->hthdChild); *ppht; ppht = & ( (*ppht)->nextSibling ) ) {
        if (*ppht == hthd) {
            *ppht = (*ppht)->nextSibling;
            break;
        }
    }

    for (ppht = &(thdList->next); *ppht; ppht = & ( (*ppht)->next ) ) {
        if (*ppht == hthd) {
            *ppht = (*ppht)->next;
            break;
        }
    }
    LeaveCriticalSection(&csThreadProcList);

    MHFree(hthd);
}


VOID
ClearContextPointers(
    PKNONVOLATILE_CONTEXT_POINTERS ctxptrs
    )
/*++

  Routine -  Clear Context Pointers

  Purpose - clears the context pointer structure.

  Argument - lpvoid - pointer to context pointers structure;
             void on on architectures that don't have such.

--*/

{
    memset(ctxptrs, 0, sizeof (KNONVOLATILE_CONTEXT_POINTERS));
}

#if 0
/*** FGETEXPORT
 *
 * PURPOSE:
 *        Given the base address of a DLL in the debuggee's memory space, find
 *        the value (if any) of a specified export in that DLL.
 *
 * INPUT:
 *        pdi        DLLLOAD_ITEM structure for module
 *        hfile      Handle to disk file of DLL
 *        szExport   Name of symbol to search for in its export table
 *
 * OUPTUT:
 *        *plpvValue Address of the symbol, from the export table.
 *                        plpvValue may be NULL if caller doesn't care.
 *        return code        TRUE if symbol was found, FALSE if not
 */


BOOL
FGetExport(
    PDLLLOAD_ITEM pdi,
    HFILE       hfile,
    LPCTSTR     szExport,
    LPVOID*     plpvValue
    )
{
    IMAGE_DOS_HEADER        doshdr;
    IMAGE_NT_HEADERS        nthdr;
    IMAGE_EXPORT_DIRECTORY  exphdr;
    LONG                    inameFirst, inameLast, iname;    // must be signed
    UOFFSET                 uoffExpTable;
    UOFFSET                 uoffNameTable;
    UOFFSET                 uoffFuncTable;
    UOFFSET                 uoffOrdinalTable;
    UOFFSET                 uoffString;
    INT                     iRet;
    size_t                  cbValueRead;
    LPTSTR                  szValueRead;
    UOFFSET                 uoffBasePE;

    /*
    ** Check for both initial MZ (oldheader) or initial PE header.
    */

    VERIFY(CbReadDllHdr(hfile, 0, &doshdr, sizeof(doshdr)) == sizeof(doshdr));

    if ( doshdr.e_magic != IMAGE_DOS_SIGNATURE ) {
        return FALSE;
    }

    uoffBasePE = ( doshdr.e_lfanew );

    VERIFY(CbReadDllHdr(hfile, uoffBasePE, &nthdr, sizeof(nthdr)) ==
        sizeof(nthdr));

    uoffExpTable = nthdr.OptionalHeader.DataDirectory[
        IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    if (uoffExpTable == 0) {
        return FALSE;
    }

    uoffExpTable = FileOffFromVA(pdi, hfile, uoffBasePE, &nthdr, uoffExpTable);

    VERIFY(CbReadDllHdr(hfile, uoffExpTable, &exphdr, sizeof(exphdr)) ==
        sizeof(exphdr));

    if ( exphdr.NumberOfNames == 0L ) {
        return FALSE;
    }

    uoffNameTable = FileOffFromVA(pdi, hfile, uoffBasePE, &nthdr,
        (UOFFSET)exphdr.AddressOfNames);

    cbValueRead = _ftcslen(szExport) + 1;
    szValueRead = MHAlloc( cbValueRead * sizeof(TCHAR));
	iRet = 1;

    // Do a binary search through the export table
    inameFirst = 0;
    inameLast = exphdr.NumberOfNames - 1;
    while (inameFirst <= inameLast) {
        iname = (inameFirst + inameLast) / 2;

        VERIFY(CbReadDllHdr(hfile, uoffNameTable + (iname * sizeof(OFFSET)),
            &uoffString, sizeof(UOFFSET)) == sizeof(UOFFSET));

        uoffString = FileOffFromVA(pdi, hfile, uoffBasePE, &nthdr,
            uoffString);

        VERIFY(CbReadDllHdr(hfile, uoffString, szValueRead, cbValueRead) ==
            cbValueRead);

        iRet = _ftcsncmp( szValueRead, szExport, cbValueRead );

        if (iRet < 0) {
            inameFirst = iname + 1;
        } else if (iRet > 0) {
            inameLast = iname - 1;
        } else /* iRet == 0: match */ {
            /* if caller wants its value, get value */
            if (plpvValue) {
                USHORT            usOrdinal;

                /* read symbol value from export table */
                uoffOrdinalTable = FileOffFromVA(pdi, hfile, uoffBasePE,
                    &nthdr, (UOFFSET)exphdr.AddressOfNameOrdinals);
                VERIFY(CbReadDllHdr(hfile,
                    uoffOrdinalTable + (iname * sizeof(USHORT)),
                    &usOrdinal, sizeof(usOrdinal)) == sizeof(usOrdinal));

                uoffFuncTable = FileOffFromVA(pdi, hfile, uoffBasePE,
                    &nthdr, (UOFFSET)exphdr.AddressOfFunctions);
                VERIFY(CbReadDllHdr(hfile,
                    uoffFuncTable + (usOrdinal * sizeof(UOFFSET)),
                    plpvValue, sizeof(*plpvValue)) == sizeof(*plpvValue));
                *plpvValue = (LPVOID) ((UOFFSET)*plpvValue + pdi->offBaseOfImage);
            }
            break;
        }
    }
    MHFree( szValueRead );
    return !iRet;

}


UOFFSET
FileOffFromVA(
    PDLLLOAD_ITEM           pdi,
    HFILE                   hfile,
    UOFFSET                 uoffBasePE,
    const IMAGE_NT_HEADERS *pnthdr,
    UOFFSET                 va
    )

/*++

Routine Description:

    Given a virtual address, calculate the file offset at which it
    can be found in an EXE/DLL.

Arguments:

    pdi - Supplies the DLLLOAD_ITEM structure for this exe/dll

    hfile - Supplies a read handle to the exe/dll file

    uoffBasePE - Supplies offset of beginning of PE header in exe/dll

    pnthdr - Supplies ptr to NTHDR for the exe/dll

    va - Supplies virtual address to convert

Return Value:

    the file offset for the given va, 0 for failure

--*/

{
    UOFFSET                 uoffObjs;
    WORD                    iobj, cobj;
    IMAGE_SECTION_HEADER    isecthdr;
    PIMAGE_SECTION_HEADER   psect;
    UOFFSET                 uoffFile = 0;

    uoffObjs = uoffBasePE +
               FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
               pnthdr->FileHeader.SizeOfOptionalHeader;

    cobj = pnthdr->FileHeader.NumberOfSections;

    /*
     * If we have not yet read the section headers into the DLLINFO, do
     * so now.
     */
    if (pdi->Sections == NULL) {
        pdi->Sections = MHAlloc(cobj * sizeof(IMAGE_SECTION_HEADER));

        VERIFY(
        CbReadDllHdr(hfile, uoffObjs, pdi->Sections, cobj * sizeof(IMAGE_SECTION_HEADER))
        );
    }

    /*
     * Look for the address.
     */
    for (iobj=0; iobj<cobj; iobj++) {
        DWORD    offset, cbObject;

        offset = pdi->Sections[iobj].VirtualAddress;
        cbObject = pdi->Sections[iobj].Misc.VirtualSize;
        if (cbObject == 0) {
            cbObject = pdi->Sections[iobj].SizeOfRawData;
        }

        if (va >= offset && va < offset + cbObject) {
            // found it
            uoffFile = pdi->Sections[iobj].PointerToRawData + va - offset;
            break;
        }
    }
    assert(uoffFile);    // caller shouldn't have called with a bogus VA
    return uoffFile;
}


DWORD
CbReadDllHdr(
    HFILE hfile,
    UOFFSET uoff,
    LPVOID lpvBuf,
    DWORD cb
    )
{
    VERIFY(_llseek(hfile, uoff, 0) != HFILE_ERROR);
    assert((UINT)cb == cb);    // make sure we don't truncate value
    return _lread(hfile, lpvBuf, (UINT)cb);
}
#endif // 0


XOSD
DMSendRequestReply (
    DBC dbc,
    HPID hpid,
    HTID htid,
    DWORD cbInput,
    LPVOID lpInput,
    DWORD cbOutput,
    LPVOID lpOutput
    )
/*++

Routine Description:


Arguments:

    dbc - Supplies command to send to EM

    hpid - Supplies process handle

    htis - Supplies thread handle

    cbInput - Supplies size of packet to send

    lpInput - Supplies packet to send

    cbOutput - Supplies size of packet to receive

    lpOutput - Returns data from reply

Return Value:

    xosdNone or xosd error value

--*/
{
    XOSD xosd = xosdNone;

    if ( cbInput == 0 ) {
        RTP rtp;

        rtp.dbc  = dbc;
        rtp.hpid = hpid;
        rtp.htid = htid;
        rtp.cb   = 0;
        xosd = DmTlFunc
            ( tlfRequest, hpid, FIELD_OFFSET( RTP, rgbVar ), (LONG) (LPV) &rtp );
    }
    else {
        LPRTP lprtp = MHAlloc ( FIELD_OFFSET( RTP, rgbVar ) + cbInput );

        lprtp->dbc  = dbc;
        lprtp->hpid = hpid;
        lprtp->htid = htid;
        lprtp->cb   = cbInput;

        _fmemcpy ( lprtp->rgbVar, lpInput, cbInput );

        xosd = DmTlFunc
            ( tlfRequest, hpid, FIELD_OFFSET( RTP, rgbVar ) + cbInput, (LONG) lprtp );

        MHFree ( lprtp );

    }

    if (xosd == xosdNone && cbOutput != 0) {
        _fmemcpy(lpOutput, abEMReplyBuf, cbOutput);
    }

    return xosd;

} /* DMSendRequestReply */



XOSD
DMSendDebugPacket (
    DBC dbc,
    HPID hpid,
    HTID htid,
    DWORD cbInput,
    LPVOID lpInput
    )
/*++

Routine Description:


Arguments:

    dbc - Supplies command to send to EM

    hpid - Supplies process handle

    htid - Supplies thread handle

    cbInput - Supplies size of packet to send

    lpInput - Supplies packet to send

Return Value:

    xosdNone or xosd error value

--*/
{
    XOSD xosd = xosdNone;

    if ( cbInput == 0 ) {
        RTP rtp;

        rtp.dbc  = dbc;
        rtp.hpid = hpid;
        rtp.htid = htid;
        rtp.cb   = 0;
        xosd = DmTlFunc
            ( tlfDebugPacket, hpid, FIELD_OFFSET( RTP, rgbVar ), (LONG) (LPV) &rtp );
    }
    else {
        LPRTP lprtp = MHAlloc ( FIELD_OFFSET( RTP, rgbVar ) + cbInput );

        lprtp->dbc  = dbc;
        lprtp->hpid = hpid;
        lprtp->htid = htid;
        lprtp->cb   = cbInput;

        _fmemcpy ( lprtp->rgbVar, lpInput, cbInput );

        xosd = DmTlFunc
            ( tlfDebugPacket, hpid, FIELD_OFFSET( RTP, rgbVar ) + cbInput, (LONG) lprtp );

        MHFree ( lprtp );

    }

    return xosd;

} /* DMSendRequestReply */



DWORD
GetEndOfRange (
    HPRCX   hprc,
    HTHDX   hthd,
    DWORD   Addr
    )
/*++

Routine Description:

    Given an address, gets the end of the range for that address.

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

    Addr    -   Supplies the address

Return Value:

    DWORD   -   End of range

--*/

{
    ADDR AddrPC;

    AddrFromHthdx(&AddrPC, hthd);
    SetAddrOff( &AddrPC, Addr );

    DMSendRequestReply(
        dbcLastAddr,
        hprc->hpid,
        hthd->htid,
        sizeof(ADDR),
        &AddrPC,
        sizeof(DWORD),
        &Addr
        );

    Addr =  (*(DWORD *)abEMReplyBuf);

    // NOTENOTE : jimsch --- Is this correct?
    return (DWORD) Addr;
}




DWORD
GetCanStep (
    HPID    hpid,
    HTID    htid,
    LPADDR  Addr,
    LPCANSTEP CanStep
    )
/*++

Routine Description:


Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

    Addr    -   Supplies Address

Return Value:

    CANSTEP_YES or CANSTEP_NO (or CANSTEP_THUNK?)

--*/

{
    return DMSendRequestReply(
        dbcCanStep,
        hpid,
        htid,
        sizeof(ADDR),
        Addr,
        sizeof(CANSTEP),
        CanStep
        );
}


BOOL
CheckBpt(
    HTHDX       hthd,
    PBREAKPOINT pbp
    )
{
    DEBUG_EVENT de;

    if (pbp->bpNotify == bpnsStop) {
        return TRUE;
    } else if (pbp->bpNotify == bpnsCheck) {
        de.dwDebugEventCode = CHECK_BREAKPOINT_DEBUG_EVENT;
        de.dwProcessId = hthd->hprc->pid;
        de.dwThreadId  = hthd->tid;
        de.u.Exception.ExceptionRecord.ExceptionCode = EXCEPTION_BREAKPOINT;

        NotifyEM(&de, hthd, 0, pbp);

        return *(DWORD *)abEMReplyBuf;
    }
    return FALSE;
}


LPTSTR
MHStrdup(
    LPCTSTR s
    )
{
    int l = _tcslen(s);
    LPTSTR p = MHAlloc(l + sizeof(TCHAR));
    _tcscpy(p, s);
    return p;
}

/*** ISTHUNK
 *
 * PURPOSE:
 *      Determine if we are in a thunk
 *
 * INPUT:
 *      hthd            - Handle to thread
 *      uoffEIP         - address to check for a thunk
 *      lpf             - Type of thunk
 *      lpuoffThunkDest - Where the thunk is going to
 *
 * OUTPUT:
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 ****************************************************************************/

BOOL
IsThunk (
    HTHDX       hthd,
    UOFFSET     uoffset,
    LPINT       lpfThunkType,
    UOFFSET *   lpuoffThunkDest,
    LPDWORD     lpdwThunkSize
    )
{
    BYTE        rgbBuffer[CB_THUNK_MAX];
    DWORD       dwLength = CB_THUNK_MAX;
    INT         ThunkType = THUNK_NONE;
    UOFFSET     ThunkDest = 0;
    DWORD       ThunkSize = 0;
    BOOL        Is = FALSE;

    // Read until a read succeeds or there's no room left to read
    if (DbgReadMemory ( hthd->hprc, (LPVOID)uoffset, rgbBuffer, dwLength, &dwLength)) {

        // System or ilink thunks
        if (FIsDirectJump( rgbBuffer, dwLength, hthd, uoffset, &ThunkDest, &ThunkSize ) ||
            FIsIndirectJump( rgbBuffer, dwLength, hthd, uoffset, &ThunkDest, &ThunkSize )
        ) {
            Is = TRUE;
            if (IsInSystemDll(ThunkDest))
            	ThunkType = THUNK_SYSTEM;
            else 
            	ThunkType = THUNK_USER;
        } else {

            // Note: it is possible that the offset passed in is NOT the PC
            // for the current thread.  Some of the thunk checks below
            // may require valid registers.  The following thunks require
            // valid registers to determine the destination address for a
            // thunk.  If uoffset is NOT the PC, then do not check for
            // these thunks.  These thunks are actually C++ thunks.


            if ( PC(hthd) == uoffset &&
                ( FIsVCallThunk( rgbBuffer, dwLength, hthd, uoffset, &ThunkDest, &ThunkSize ) ||
                  FIsVTDispAdjustorThunk( rgbBuffer, dwLength, hthd, uoffset, &ThunkDest, &ThunkSize ) ||
                  FIsAdjustorThunk( rgbBuffer, dwLength, hthd, uoffset, &ThunkDest, &ThunkSize ) )
            ) {
                Is = TRUE;
                ThunkType = THUNK_USER;
            }
        }
    }

    if (lpuoffThunkDest) {
        *lpuoffThunkDest = ThunkDest;
    }
    if (lpdwThunkSize) {
        *lpdwThunkSize = ThunkSize;
    }
    if (lpfThunkType) {
        *lpfThunkType = ThunkType;
    }
    return Is;
}



DWORD
InterlockedSetFlag(
	LONG*	lpFlag
	)
/*++

Routine Description:

	This function implements a thread-safe test-and-set primitive.

Return Values:

	FLAG_ALREADY_SET - The flag had already been set.

	FLAG_SET - The flag was successfully set.

--*/
{
	DWORD	status;
	
	if (InterlockedIncrement (lpFlag) > 1) {
		InterlockedDecrement (lpFlag);

		status = FLAG_ALREADY_SET;
	} else {
		status = FLAG_SET;
	}

	return status;
}


DWORD
InterlockedClearFlag(
	LONG*	lpFlag
	)
/*++

Routine Description:

	This function implements a thread-safe test-and-clear primitive.

Return Values:

	FLAG_ALREADY_CLEARED - The flag had already been cleared.

	FLAG_CLEARED - The flag was successfully cleared.

--*/
{
	DWORD	status;
	
	if (InterlockedDecrement (lpFlag) < 0) {
		InterlockedIncrement (lpFlag);

		status = FLAG_ALREADY_CLEARED;
	} else {
		status = FLAG_CLEARED;
	}

	return status;
}



//
// Timer Event Queue package
//

void
_AddWaitingTimerEvent(
	HPRCX			hprc,
	TIMER_EVENT*	event
	)
{
	EnterCriticalSection (&hprc->TimerQueue.cs);

	event->next = hprc->TimerQueue.waitlist;
	hprc->TimerQueue.waitlist = event;

	LeaveCriticalSection (&hprc->TimerQueue.cs);
}
	
void
_AddToTimerEventQueue(
	HPRCX			hprc,
	TIMER_EVENT*	event
	)
/*++

Routine Description:

	Private function that adds an event to the event queue.

--*/
{
	TIMER_EVENT*	prev = NULL;
	TIMER_EVENT*	cur = NULL;

	event->ticks += GetTickCount ();
	EnterCriticalSection (&hprc->TimerQueue.cs);
	
	prev = NULL;
	cur = hprc->TimerQueue.head;

	do {
		if (cur == NULL || cur->ticks >= event->ticks) {
	
			if (prev) {
				prev->next = event;
			} else {
				hprc->TimerQueue.head = event;
			}
			event->next = cur;
			break;
		}

		prev = cur;
		if (cur) {
			cur = cur->next;
		}

	} while (cur);

	LeaveCriticalSection (&hprc->TimerQueue.cs);
}


TIMER_EVENT*
_RemoveFromQueue(
	HPRCX			hprc,
	TIMER_EVENT**	head,
	HANDLE			hEvent
	)
{
	TIMER_EVENT*	prev = NULL;
	TIMER_EVENT*	cur = NULL;

	EnterCriticalSection (&hprc->TimerQueue.cs);
	
	prev = NULL;
	cur = *head;

	while (cur && cur->handle != hEvent) {
		prev = cur;
		cur = cur->next;
	}

	if (cur) {
		if (prev) {
			prev->next = cur->next;
		} else {
			*head = cur->next;
			cur->next = NULL;
		}
	}

	LeaveCriticalSection (&hprc->TimerQueue.cs);

	return cur;
}

BOOL
CreateTimerEventQueue(
	TIMER_EVENT_QUEUE*	queue
	)
{
	queue->head = NULL;
	queue->waitlist = NULL;
	InitializeCriticalSection (&queue->cs);
	queue->count = 1;

	return TRUE;
}

BOOL
DeleteTimerEventQueue(
	TIMER_EVENT_QUEUE*	queue
	)
{
	//
	// Remove remaining items

	DeleteCriticalSection (&queue->cs);
	return TRUE;
}


HANDLE
CreateTimerEvent(
	HPRCX					hprc,
	TIMER_EVENT_CALLBACK	callback,
	LPVOID					param,
	DWORD					ticks,
	BOOL					fQueueEvent
	)
/*++

Routine Description:

	Create a timer event and place it on the queue.

Arguments:

	hprc - The process to create the timer event for.

	callback - The callback to call when the timer event fires.

	param - A param to pass to the callback.

	ticks - How long in milliseconds to wait before firing the timer event.

	fQueueEvent - If true, the event should be placed on the queue,
		otherwise, the even is not placed on the queue and the caller must
		manually place it on the queue.  Note that the number of ticks is
		from the time the event is placed on the queue -- not from when it
		was created.

--*/
{
	HANDLE			h = NULL;
	TIMER_EVENT*	event = NULL;
	

	event = (TIMER_EVENT*) MHAlloc (sizeof (TIMER_EVENT));

	event->callback = callback;
	event->param = param;
	event->ticks = ticks;


	h = (HANDLE) InterlockedIncrement (&hprc->TimerQueue.count);
	event->handle = h;

	if (fQueueEvent) {
		_AddToTimerEventQueue (hprc, event);
	} else {
		_AddWaitingTimerEvent (hprc, event);
	}

	return h;
}

BOOL
EnqueueTimerEvent(
	HPRCX			hprc,
	HANDLE			hEvent
	)
{
	TIMER_EVENT*	event = NULL;
	
	EnterCriticalSection (&hprc->TimerQueue.cs);

	event = _RemoveFromQueue (hprc, &hprc->TimerQueue.waitlist, hEvent);

	if (event) {
		_AddToTimerEventQueue (hprc, event);
	}
	
	LeaveCriticalSection (&hprc->TimerQueue.cs);

	return event ? TRUE : FALSE;
}


BOOL
RemoveTimerEvent(
	HPRCX	hprc,
	HANDLE	hEvent
	)
{
	BOOL			succ = FALSE;
	TIMER_EVENT*	event = NULL;
	
	if (hprc->TimerQueue.head == NULL) {
		return FALSE;
	}

	event = _RemoveFromQueue (hprc, &hprc->TimerQueue.head, hEvent);

	if (!event) {
		event = _RemoveFromQueue (hprc, &hprc->TimerQueue.waitlist, hEvent);
	}

	if (event) {
		MHFree (event);
		succ = TRUE;
	}

	return succ;
	
}

BOOL
FireOutstandingTimerEvents(
	HPRCX	hprc
	)
{
	BOOL			succ = FALSE;
	TIMER_EVENT*	cur = NULL;
	TIMER_EVENT*	nextT = NULL;
	DWORD			ticks;

	
	if (hprc->TimerQueue.head == NULL) {
		return FALSE;
	}

	EnterCriticalSection (&hprc->TimerQueue.cs);

	ticks = GetTickCount ();

	cur = hprc->TimerQueue.head ;
	
	while (cur && cur->ticks <= ticks) {
		hprc->TimerQueue.head = cur->next;
		nextT = cur->next;
		cur->next = NULL;

		cur->callback (hprc, cur->param);
		MHFree (cur);
		cur = nextT;
		succ = TRUE;
	}

	LeaveCriticalSection (&hprc->TimerQueue.cs);

	return succ;
}
