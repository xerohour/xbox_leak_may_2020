/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    mach.c

Abstract:

    This file contains the x86 specific code for dealing with
    machine dependent issues that invlove registers, instruction
    disassembly, function calling and other interesting things.

Author:

    Jim Schaad (jimsch)

Environment:

    Win32 - User

Notes:

--*/
#include "precomp.h"
#pragma hdrstop

#include "i386excp.h"

extern CRITICAL_SECTION csContinueQueue;

/**********************************************************************/

extern LPDM_MSG LpDmMsg;

/**********************************************************************/

#if    DBG
static char * rgszTrace[] = {
    "Trace", "BreakPoint", "Cannot Trace", "Soft Int", "Call"
};
#endif // DBG

#define MAXL 20L

#define BIT20(b) (b & 0x07)
#define BIT53(b) (b >> 3 & 0x07)
#define BIT76(b) (b >> 6 & 0x03)


static int	mod;        /* mod of mod/rm byte */



//
// Stuff for debug registers
//


typedef struct _DR7 *PDR7;
typedef struct _DR7 {
    DWORD       L0      : 1;
    DWORD       G0      : 1;
    DWORD       L1      : 1;
    DWORD       G1      : 1;
    DWORD       L2      : 1;
    DWORD       G2      : 1;
    DWORD       L3      : 1;
    DWORD       G3      : 1;
    DWORD       LE      : 1;
    DWORD       GE      : 1;
    DWORD       Pad1    : 3;
    DWORD       GD      : 1;
    DWORD       Pad2    : 1;
    DWORD       Pad3    : 1;
    DWORD       Rwe0    : 2;
    DWORD       Len0    : 2;
    DWORD       Rwe1    : 2;
    DWORD       Len1    : 2;
    DWORD       Rwe2    : 2;
    DWORD       Len2    : 2;
    DWORD       Rwe3    : 2;
    DWORD       Len3    : 2;
} DR7;


#define RWE_EXEC        0x00
#define RWE_WRITE       0x01
#define RWE_RESERVED    0x02
#define RWE_READWRITE   0x03


DWORD       LenMask[ MAX_DEBUG_REG_DATA_SIZE + 1 ] = DEBUG_REG_LENGTH_MASKS;


BOOL
IsRet(
    HTHDX hthd,
    LPADDR  addr
    )
{
    BYTE instr;
    DWORD cBytes;
    if (!AddrReadMemory( hthd->hprc, hthd, addr, &instr, 1, &cBytes )) {
        return FALSE;
    }

    return  ((instr == 0xc2) || (instr == 0xc3) ||
                 (instr == 0xca) || (instr == 0xcb)) ?
                 RETURN_USER :
                 RETURN_NONE;
}



void
IsCallEx(
    HTHDX   hthd,
    LPADDR  lpaddr,
    LPINT   lpf,
    BOOL    fStepOver,
	LPADDR	addrDest
    )
/*++

Routine Description:

    This function checks to see if the specified instruction is
    a call instruction.

Arguments:

    hthd        - Supplies the handle to the current thread

    lpaddr      - Supplies the address to check for the call instruction at

    lpf         - Returns TRUE if is a call instruction

    fStepOver   - Supplies TRUE if doing a step over

	addrDest    - If this is a call instruction, gives the destination.
				  At this time this only works for "simple calls",
				  i.e., E8 calls.

Return Value:

    None.

--*/

{
    int     mode_32;
    int     opsize_32;
    DWORD   cBytes;
    DWORD   cb;
    char    membuf [ MAXL ];
    char    *pMem;
    UCHAR   opcode, savedOpcode;
    int     fPrefix;
    int     fRepPrefix;
    int     ttt;
    int     mode;
    int     rm;
    BOOL    fAddrSet;
    ULONG   rgul[2];
    USHORT  rgus[2];
    ADDR    addrSp;
    UOFFSET     uNextEip = GetAddrOff(*lpaddr);
	OFFSET	offDest = 0;

	if (addrDest) {
		memset (addrDest, 0, sizeof (*addrDest));
	}
	
    /*
     * local addressing mode
     */

    mode_32 = opsize_32 = hthd->fAddrOff32;

    /*
     * Read enough bytes to get the longest possible instruction
     */

    if (!AddrReadMemory( hthd->hprc,
                         hthd,
                         lpaddr,
                         membuf,
                         MAXL,
                         &cBytes) ||
            (cBytes == 0)) {
        goto done;
    }

    DPRINT(1, ("(IsCall?) EIP=%08x Type=", *lpaddr));

    /*
     *  point to begin of instruction
     */

    pMem = membuf;

    /*
     * read and process any prefixes first
     */

    fPrefix = TRUE;
    fRepPrefix = FALSE;

    do {
        opcode = (UCHAR) *pMem++;   /* get opcode */

        /*
         *  Operand size prefix
         */

        if (opcode == 0x66) {
            opsize_32 = !opsize_32;
        }
        /*
         * Address size prefix
         */

        else if (opcode == 0x67) {
            mode_32 = !mode_32;
        }
        /*
         *  REP and REPNE prefix
         */
        else if ((opcode & ~1) == 0xf2) {
            fRepPrefix = TRUE;
        }
        /*
         *  LOCK prefix   (0xf0)
         *  Segment Override (0x26, 0x36, 0x2e, 0x3e, 0x64, 0x65)
         */
        else if ( opcode != 0xf0 &&
                  (opcode & ~0x18) != 0x26 &&
                  (opcode & ~1) != 0x64 ) {
            fPrefix = FALSE;
        }
    } while ( fPrefix );

    /*
     *  Now start checking for the instructions which must be treated
     *  in a special manner.   Any instruction which does not respect
     *  the trace bit (either due to flag munging or faulting) needs
     *  to be treated specially.  Also all call ops need to be treated
     *  specially (step in vs step over).  Finally interupts need to
     *  be treated specially since they could cause faults in 16-bit mode
     */

    fAddrSet = FALSE;

    savedOpcode = opcode;

    /*
     *  Break point instruction
     */
    if (opcode == 0xcc) {
        *lpf = INSTR_BREAKPOINT;
    }
    // NOTENOTE -- missing the INTO instruction
    /*
     *  all other interrrupt instructions
     */
    else if (opcode == 0xcd) {
        opcode = (UCHAR) *pMem++;

        /*
         * Is this really a 2-byte version of INT 3 ?
         */
        if (opcode == 0x3) {
            *lpf = INSTR_BREAKPOINT;
        }
        /*
         * Is this a funky 16-bit floating point instruction?  if so then
         *      we need to make sure and step over it
         */
        else if (!ADDR_IS_FLAT(*lpaddr) &&
                 (0x34 <= opcode) && (opcode <= 0x3c)) {
            if (opcode == 0x3C) {
                pMem++;
            }
            opcode = *pMem++;
            mode = opcode & 0xc0;
            rm = opcode & 0x03;
            switch ( mode) {
            case 0:
                if (rm == 0x6) {
                    pMem += 2;
                }
                break;

            case 1:
                pMem += 1;
                break;

            case 2:
                pMem += 2;
                break;
            }
            *lpf = INSTR_CANNOT_TRACE;
            GetAddrOff(*lpaddr) += pMem - membuf;
            fAddrSet = TRUE;
        }
        /*
         *   This is an FWAIT instr -- 2 bytes long
         */
        else if (!ADDR_IS_FLAT(*lpaddr) && opcode == 0x3d) {
            *lpf = INSTR_CANNOT_TRACE;
            GetAddrOff(*lpaddr) += 2;
            fAddrSet = TRUE;
        }
        /*
         *  This is a 0x3f interrupt -- I think this is for
         *      overlays in dos
         */
        else if (!ADDR_IS_FLAT(*lpaddr) && (opcode == 0x3f)) {
            if (fStepOver) {
                *lpf = INSTR_CANNOT_TRACE;
                AddrInit(&addrSp, 0, SsSegOfHthdx(hthd), STACK_POINTER(hthd),
                         FALSE, FALSE, FALSE, hthd->fAddrIsReal);
                if (!AddrReadMemory(hthd->hprc,
                                    hthd,
                                    &addrSp,
                                    rgus,
                                    4,
                                    &cb) ||
                        (cb != 4) ) {
                    goto done;
                }
                AddrInit(lpaddr, 0, rgus[1], (UOFF32) rgus[0], FALSE, FALSE,
                         FALSE, hthd->fAddrIsReal);
                fAddrSet = TRUE;
            }
        }
        /*
         *  OK its really an interrupt --- deal with it
         */
        else {
            if (!fStepOver && hthd->fAddrIsReal) {
                *lpf = INSTR_CANNOT_TRACE;
                AddrInit(&addrSp, 0, 0, opcode*4, FALSE, FALSE, FALSE, TRUE);
                if (!AddrReadMemory(hthd->hprc,
                                    hthd,
                                    &addrSp,
                                    rgus,
                                    4,
                                    &cb) ||
                        (cb != 4) ) {
                    goto done;
                }
                AddrInit(lpaddr, 0, rgus[1], (UOFF32) rgus[0], FALSE, FALSE,
                         FALSE, TRUE);
                fAddrSet = TRUE;
            }
        }
    }
    /*
     *  Now check for various call instructions
     */
    else if (opcode == 0xe8) {  /* near direct call */
        *lpf = INSTR_IS_CALL;
		if (opsize_32) {
			offDest = *(LONG*) pMem;
		} else {
			offDest = *(SHORT*) pMem;
		}
        pMem += (1 + opsize_32) * 2;
    } else if (opcode == 0x9a) { /* far direct call */
        *lpf = INSTR_IS_CALL;
		if (opsize_32) {
			offDest = *(LONG*) pMem;
		} else {
			offDest = *(SHORT*) pMem;
		}
        pMem += (2 + opsize_32) * 2;
    } else if (opcode == 0xff) {

		//
		// If you hit this assert you will need to figure out how to get
		// the destination.  Assert is here mostly to tell you that it
		// is NYI.
		//
		
		assert (addrDest == NULL);
        opcode = *pMem++;       /* compute the modRM bits for instruction */
        ttt = BIT53(opcode);
        if ((ttt & ~1) == 2) {  /* indirect call */
            *lpf = INSTR_IS_CALL;

            mod = BIT76(opcode);
            if (mod != 3) {    /* non-register operand */
                rm = BIT20( opcode );
                if (mode_32) {
                    if (rm == 4) {
                        rm = BIT20(*pMem++); /* get base from SIB */
                    }

                    if (mod == 0) {
                        if (rm == 5) {
                            pMem += 4; /* long direct address */
                        }
                    } else if (mod == 1) {
                        pMem++; /* register with byte offset */
                    } else {
                        pMem += 4; /* register with long offset */
                    }
                } else {        /* 16-bit mode */
                    if (mod == 0) {
                        if (rm == 6) {
                            pMem += 2; /* short direct address */
                        }
                    } else {
                        pMem += mod; /* reg, byte, word offset */
                    }
                }
            }
        }
    }
    /*
     * Now catch all of the repeated instructions
     *
     *  INSB  (0x6c) INSW  (0x6d) OUTSB (0x6e) OUTSW (0x6f)
     *  MOVSB (0xa4) MOVSW (0xa5) CMPSB (0xa6) CMPSW (0xa7)
     *  STOSB (0xaa) STOSW (0xab)
     *  LODSB (0xac) LODSW (0xad) SCASB (0xae) SCASW (0xaf)
     */
    else if (fRepPrefix && (((opcode & ~3) == 0x6c) ||
                            ((opcode & ~3) == 0xa4) ||
                            ((opcode & ~1) == 0xaa) ||
                            ((opcode & ~3) == 0xac))) {
        if (fStepOver) {
            *lpf = INSTR_CANNOT_TRACE;
        } else {
            /*
             *  Cannot trace the ins/outs instructions
             */
            if ((opcode & ~3) == 0x6c) {
                *lpf = INSTR_CANNOT_TRACE;
            }
        }
    }
    /*
     *  Now catch IO instructions -- these will generally fault and
     *  be interpreted.
     */
    else if ((opcode & ~3) == 0x6c) {
        *lpf = INSTR_CANNOT_TRACE;
    }
    /*
     *  Now catch other instructions which change registers
     */
    else if ((opcode == 0xfa) || (opcode == 0xfb) ||
             (opcode == 0x9d) || (opcode == 0x9c)) {
        *lpf = INSTR_CANNOT_TRACE;
    }
    /*
     * Now catch irets
     */
    else if (opcode == 0xcf) {
        *lpf = INSTR_CANNOT_TRACE;
        AddrInit(&addrSp, 0, SsSegOfHthdx(hthd), STACK_POINTER(hthd),
                 hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE,
                 hthd->fAddrIsReal);
        if (opsize_32) {
            if (!AddrReadMemory(hthd->hprc,
                                hthd,
                                &addrSp,
                                rgul,
                                8,
                                &cb) ||
                    (cb != 8) ) {
                goto done;
            }
            AddrInit(lpaddr, 0, (SEGMENT) rgul[1], rgul[0],
                     hthd->fAddrIsFlat, TRUE, FALSE, FALSE);
        } else {
            if (!AddrReadMemory(hthd->hprc,
                                hthd,
                                &addrSp,
                                rgus,
                                4,
                                &cb) ||
                    (cb != 4) ) {
                goto done;
            }
            AddrInit(lpaddr, 0, rgus[1], (UOFF32) rgus[0], FALSE, FALSE,
                     FALSE, hthd->fAddrIsReal);
        }
        fAddrSet = TRUE;
    }
    /*
     *  Assume that we want to just trace the instruction
     */
    else {
        *lpf = INSTR_TRACE_BIT;
        goto done;
    }

    /*
     *
     */

    DPRINT(1, ("%s", rgszTrace[*lpf]));

    /*
     * Have read enough bytes?   no -- expect somebody else to blow up
     */

    if (cBytes < (DWORD)(pMem - membuf)) {
        *lpf = INSTR_TRACE_BIT;
        goto done;
    }

    if (!fAddrSet) {
        GetAddrOff(*lpaddr) += pMem - membuf;
    }

	if (addrDest) {
		*addrDest = *lpaddr;
		GetAddrOff (*addrDest) += offDest;
	}

    /*
     * Dump out the bytes for later checking
     */

#if DBG
    if (FVerbose) {
        DWORD  i;
        DPRINT(1, ("length = %d  bytes=", cBytes & 0xff));
        for (i=0; i<cBytes; i++) {
            DPRINT(1, (" %02x", membuf[i]));
        }
    }
#endif

 done:
#if 0
    if (IsChicago()) {
        // Check for return to Win95 system code - can't step that code
        if ((savedOpcode == 0xc2) || (savedOpcode == 0xc3)) {
            ADDR addr = *lpaddr;    // initialize all fields

            addr.addr.off = hthd->context.Esp;
            AddrReadMemory(hthd->hprc, hthd, &addr, &uNextEip, sizeof(uNextEip), &cBytes);
        }
        if (IsInSystemDll(uNextEip)) {
            *lpf = INSTR_CANNOT_STEP;
        }
    }
#endif
    hthd->fIsCallDone = TRUE;
    hthd->addrIsCall = *lpaddr;
    hthd->iInstrIsCall = *lpf;
    return;
}                               /* IsCall() */



void
IsCall(
	HTHDX	hthd,
	LPADDR	lpaddr,
	LPINT	lpf,
	BOOL	fStepOver
	)
{
	IsCallEx (hthd, lpaddr, lpf, fStepOver, NULL);
}



XOSD
SetupFunctionCall(
    LPEXECUTE_OBJECT_DM lpeo,
    LPEXECUTE_STRUCT    lpes
    )
/*++

Routine Description:

    This function contains the machine dependent code for initializing
    the function call system.

Arguments:

    lpeo   - Supplies a pointer to the Execute Object for the Function call

    lpes   - Supplies a pointer to the Execute Struct from the DM

Return Value:

    XOSD error code

--*/

{
    CONTEXT     context;
    OFFSET      off;
    int         cb;
    ULONG       ul;
    HPRCX       hprc = lpeo->hthd->hprc;
    ADDR        addr;

    /*
     *  Can only execute functions on the current stopped thread.  Therefore
     *  assert that the current thread is stopped.
     */

    assert(lpeo->hthd->tstate & ts_stopped);
    if (!(lpeo->hthd->tstate & ts_stopped)) {
        return xosdBadThread;
    }

    /*
     * Can copy the context from the cached context in the thread structure
     */

	EnsureHthdReg(lpeo->hthd, CONTEXT_CONTROL);
    context = lpeo->hthd->ctx;

    /*
     *  Now get the current stack offset
     */

    lpeo->addrStack.addr.off = context.Esp;
    lpeo->addrStack.addr.seg = (SEGMENT) context.SegSs;
    if (!lpeo->hthd->fAddrOff32) {
        lpeo->addrStack.addr.off &= 0xffff;
    }

    /*
     *  Put the return address onto the stack.  If this is a far address
     *  then it needs to be a far return address.  Else it must be a
     *  near return address.
     */

    if (lpeo->hthd->fAddrOff32) {
        if (lpes->fFar) {
            assert(FALSE);          /* Not used for Win32 */
        }

        off = context.Esp - 4;
        if (DbgWriteMemory(hprc, (char *) off, &lpeo->addrStart.addr.off,
                               4, &cb) == 0 ||
            (cb != 4)) {
            return xosdUnknown;
        }
    } else {
        if (lpes->fFar) {
            off = context.Esp - 4;
            ul = (lpeo->addrStart.addr.seg << 16) | lpeo->addrStart.addr.off;
            addr = lpeo->addrStack;
            GetAddrOff(addr) -= 4;
            TranslateAddress(hprc, lpeo->hthd, &addr, TRUE);
            if ((DbgWriteMemory(hprc, (char *) GetAddrOff(addr),
                                    &ul, 4, &cb) == 0) ||
                (cb != 4)) {
                return xosdUnknown;
            }
        } else {
            off = context.Esp & 0xffff - 2;
            addr = lpeo->addrStack;
            GetAddrOff(addr) -= 2;
            TranslateAddress(hprc, lpeo->hthd, &addr, TRUE);
            if ((DbgWriteMemory(hprc, (char *) GetAddrOff(addr),
                                    &lpeo->addrStart.addr.off, 2, &cb) == 0) ||
                (cb != 2)) {
                return xosdUnknown;
            }
        }
    }

    /*
     *  Set the new stack pointer and starting address in the context and
     *  write them back to the thread.
     */

    HthdReg(lpeo->hthd,Esp) = off;
    HthdReg(lpeo->hthd,Eip) = lpeo->addrStart.addr.off;

    lpeo->hthd->fContextDirty = TRUE;

    return xosdNone;
}                               /* SetupFunctionCall() */




BOOL
CompareStacks(
    LPEXECUTE_OBJECT_DM       lpeo
    )

/*++

Routine Description:

    This routine is used to determine if the stack pointers are currect
    for terminating function evaluation.

Arguments:

    lpeo        - Supplies the pointer to the DM Execute Object description

Return Value:

    TRUE if the evaluation is to be terminated and FALSE otherwise

--*/

{
    if (lpeo->hthd->fAddrOff32) {
        if (lpeo->ctxSav.Esp <= HthdReg(lpeo->hthd,Esp)) {
            return TRUE;
        }
    } else if ((lpeo->addrStack.addr.off <= (HthdReg(lpeo->hthd,Esp) & 0xffff)) &&
               (lpeo->addrStack.addr.seg == (SEGMENT) HthdReg(lpeo->hthd,SegSs))) {
        return TRUE;
    }
    return FALSE;
}                               /* CompareStacks() */

#ifndef KERNEL
#if 0

void
ProcessGetDRegsCmd(
    HPRCX hprc,
    HTHDX hthd,
    LPDBB lpdbb
    )
{
    LPDWORD   lpdw = (LPDWORD)LpDmMsg->rgb;
    CONTEXT   cxt;
    int       rs = 0;

    DEBUG_PRINT( "ProcessGetDRegsCmd :\n");

	_asm int 3
	LpDmMsg->xosdRet = xosdGeneral;
#if 0
    if (hthd == 0) {
        rs = 0;
    } else {
        cxt.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (!GetThreadContext(hthd->rwHand, &cxt)) {
            LpDmMsg->xosdRet = xosdUnknown;
            rs = 0;
        } else {
            lpdw[0] = hthd->context.Dr0;
            lpdw[1] = hthd->context.Dr1;
            lpdw[2] = hthd->context.Dr2;
            lpdw[3] = hthd->context.Dr3;
            lpdw[4] = hthd->context.Dr6;
            lpdw[5] = hthd->context.Dr7;
            LpDmMsg->xosdRet = xosdNone;
            rs = sizeof(CONTEXT);
        }
    }
#endif

    Reply( rs, LpDmMsg, lpdbb->hpid );
    return;
}                             /* ProcessGetDRegsCmd() */


void
ProcessSetDRegsCmd(
    HPRCX hprc,
    HTHDX hthd,
    LPDBB lpdbb
    )
{
    XOSD        xosd = xosdNone;
	_asm int 3
#if 0
    LPDWORD     lpdw = (LPDWORD)(lpdbb->rgbVar);

    Unreferenced(hprc);

    DPRINT(5, ("ProcessSetDRegsCmd : "));

    hthd->context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    hthd->context.Dr0 = lpdw[0];
    hthd->context.Dr1 = lpdw[1];
    hthd->context.Dr2 = lpdw[2];
    hthd->context.Dr3 = lpdw[3];
    hthd->context.Dr6 = lpdw[4];
    hthd->context.Dr7 = lpdw[5];


    if (hthd->fWowEvent) {
        WOWSetThreadContext(hthd, &hthd->context);
    } else {
        SetThreadContext(hthd->rwHand, &hthd->context);
    }
#endif

    Reply(0, &xosd, lpdbb->hpid);

    return;
}                               /* ProcessSetDRegsCmd() */
#endif // 0


DWORDLONG
GetFunctionResult(
    PCALLSTRUCT pcs
    )
{
    return pcs->context.Eax;
}

#if 0
VOID
vCallFunctionHelper(
    HTHDX hthd,
    FARPROC lpFunction,
    int cArgs,
    va_list vargs
    )
{
    int i;
    DWORD dw;

    //
    // set up the args to SuspendThread
    //

    for (i = 0; i < cArgs; i++) {

        DWORD dw;
        DWORD arg = va_arg(vargs, DWORD);

        HthdReg(hthd,Esp) -= 4;
        DbgWriteMemory(hthd->hprc,
                           (PVOID)HthdReg(hthd,Esp),
                           &arg,
                           sizeof(arg),
                           &dw);
    }

    HthdReg(hthd,Esp) -= 4;
    DbgWriteMemory(hthd->hprc,
                       (PVOID)HthdReg(hthd,Esp),
                       &(PC(hthd)),
                       sizeof(DWORD),
                       &dw);

    PC(hthd) = (DWORD)lpFunction;
    hthd->fContextDirty = TRUE;
}
#endif // 0

#endif // !KERNEL

BOOL
ProcessFrameStackWalkNextCmd(HPRCX hprc,
                             HTHDX hthd,
                             PCONTEXT context,
                             LPVOID pctxPtrs)

{
    return FALSE;
}


#if 0

XOSD disasm ( LPSDI lpsdi, void*Memory, int Size );
BOOL ParseAddr ( char*, ADDR* );
BOOL ParseNumber( char*, DWORD*, int );


typedef struct _BTNODE {
    char    *Name;
    BOOL    IsCall;
    BOOL    TargetAvail;
} BTNODE;


BTNODE BranchTable[] = {
    { "call"    ,   TRUE    ,   TRUE    },
    { "ja"      ,   FALSE   ,   TRUE    },
    { "jae"     ,   FALSE   ,   TRUE    },
    { "jb"      ,   FALSE   ,   TRUE    },
    { "jbe"     ,   FALSE   ,   TRUE    },
    { "jcxz"    ,   FALSE   ,   TRUE    },
    { "je"      ,   FALSE   ,   TRUE    },
    { "jecxz"   ,   FALSE   ,   TRUE    },
    { "jg"      ,   FALSE   ,   TRUE    },
    { "jge"     ,   FALSE   ,   TRUE    },
    { "jl"      ,   FALSE   ,   TRUE    },
    { "jle"     ,   FALSE   ,   TRUE    },
    { "jmp"     ,   FALSE   ,   TRUE    },
    { "jne"     ,   FALSE   ,   TRUE    },
    { "jno"     ,   FALSE   ,   TRUE    },
    { "jnp"     ,   FALSE   ,   TRUE    },
    { "jns"     ,   FALSE   ,   TRUE    },
    { "jo"      ,   FALSE   ,   TRUE    },
    { "jp"      ,   FALSE   ,   TRUE    },
    { "js"      ,   FALSE   ,   TRUE    },
    { "loop"    ,   FALSE   ,   FALSE   },
    { "loope"   ,   FALSE   ,   FALSE   },
    { "loopne"  ,   FALSE   ,   FALSE   },
    { "loopnz"  ,   FALSE   ,   FALSE   },
    { "loopz"   ,   FALSE   ,   FALSE   },
    { "ret"     ,   FALSE   ,   FALSE   },
    { "retf"    ,   FALSE   ,   FALSE   },
    { "retn"    ,   FALSE   ,   FALSE   },
    { NULL      ,   FALSE   ,   FALSE   }
};



DWORD
BranchUnassemble(
    void   *Memory,
    ADDR   *Addr,
    BOOL   *IsBranch,
    BOOL   *TargetKnown,
    BOOL   *IsCall,
    BOOL   *IsTable,
    ADDR   *Target
    )
{
    XOSD    xosd;
    DWORD   Consumed = 0;
    DWORD   i;
    int     s;
    char    *p;
    ADDR    Trgt;

    AddrInit( &Trgt, 0, 0, 0, TRUE, TRUE, FALSE, FALSE );

    *IsBranch = FALSE;
    *IsTable  = FALSE;

    Sdi.dop = dopOpcode| dopOperands | dopEA;
    Sdi.addr = *Addr;

    xosd = disasm( &Sdi, Memory, 16 );

    if ( xosd == xosdNone ) {

        *IsTable = Sdi.fJumpTable;

        for ( i=0; BranchTable[i].Name != NULL; i++ ) {

            s = strcmp( Sdi.lpch, BranchTable[i].Name );

            if ( s == 0 ) {

                *IsBranch = TRUE;
                *IsCall = BranchTable[i].IsCall;
                if (*IsTable) {
                    *Target = Sdi.addrEA0;
                    //
                    // We might know the target, but for this
                    // purpose, we don't want to deal with it.
                    //
                    *TargetKnown = FALSE;
                }
                else if (BranchTable[i].TargetAvail &&
                         (p = Sdi.lpch) &&
                         *(p += (_tcslen(p)+1)) ) {

                    Trgt = *Addr;
                    if ( ParseAddr( p, &Trgt ) ) {
                        *TargetKnown = TRUE;
                    } else {
                        AddrInit( &Trgt, 0, 0, 0, TRUE, TRUE, FALSE, FALSE );
                        *TargetKnown = FALSE;
                    }
                    *Target = Trgt;
                }
                else {
                    *Target = Trgt;
                    *TargetKnown = FALSE;
                }

                break;

            } else if ( s < 0 ) {

                break;
            }
        }

        Consumed = GetAddrOff( Sdi.addr ) - GetAddrOff(*Addr);
    }

    return Consumed;
}



BOOL
ParseAddr (
    char *szAddr,
    ADDR *Addr
    )
{

    char    *p;
    BOOL    fParsed;
    SEGMENT Segment;
    UOFF16  Off16;
    UOFF32  Off32;
    DWORD   Dword;

    assert( szAddr );
    assert( Addr );

    fParsed = FALSE;

    p = _tcschr( szAddr, ':' );

    if ( p ) {

        *p = '\0';
        p++;

        if ( ParseNumber( szAddr, &Dword, 16 ) ) {

            Segment = (SEGMENT)Dword;

            if ( ParseNumber( p, &Dword, 16 ) ) {

                Off16   = (UOFF16)Dword;
                fParsed = TRUE;

                GetAddrSeg(*Addr) = Segment;
                GetAddrOff(*Addr) = Off16;
            }
        }
    } else {

        if ( ParseNumber( szAddr, &Dword, 16 ) ) {

            Off32   = (UOFF32)Dword;
            fParsed = TRUE;

            GetAddrOff(*Addr) = Off32;
        }
    }

    return fParsed;
}


BOOL
ParseNumber (
    char  *szNumber,
    DWORD *Number,
    int    Radix
    )
{
    BOOL  fParsed = FALSE;
    char *p       = szNumber;
    char *q;

    assert( szNumber );
    assert( Number );

    if ( _tcslen(p) > 2 &&
         p[0]=='0' &&
         (p[1]=='x' || p[1]=='X') ) {

        p+=2;
        assert( Radix == 16 );
    }

    q = p;
    while ( *q && isxdigit(*q) ) {
        q++;
    }

    if ( !*q ) {
        *Number = _tcstoul( p, NULL, Radix );
        fParsed = TRUE;
    }

    return fParsed;
}

#endif // 0


#if 0
BOOL
SetupDebugRegister(
    HTHDX       hthd,
    int         Register,
    int         DataSize,
    DWORD       DataAddr,
    DWORD       BpType
    )
{
	_asm int 3
#if 0
    DWORD               Len;
    DWORD               rwMask;

#ifdef KERNEL
    KSPECIAL_REGISTERS  ksr;
    PDWORD  Dr0 = &ksr.KernelDr0;
    PDWORD  Dr1 = &ksr.KernelDr1;
    PDWORD  Dr2 = &ksr.KernelDr2;
    PDWORD  Dr3 = &ksr.KernelDr3;
    PDR7    Dr7 = (PDR7)&(ksr.KernelDr7);
#else
    CONTEXT     Context;
    PDWORD  Dr0 = &Context.Dr0;
    PDWORD  Dr1 = &Context.Dr1;
    PDWORD  Dr2 = &Context.Dr2;
    PDWORD  Dr3 = &Context.Dr3;
    PDR7    Dr7 = (PDR7)&(Context.Dr7);
#endif


#ifdef KERNEL
    if (!GetExtendedContext(hthd, &ksr))
#else
    Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (!GetThreadContext(hthd->rwHand, &Context))
#endif
    {
        return FALSE;
    }


    Len  = LenMask[ DataSize ];

    switch ( BpType ) {
        case bptpDataR:
            rwMask = RWE_READWRITE;
            break;

        case bptpDataW:
        case bptpDataC:
            rwMask = RWE_WRITE;
            break;

        case bptpDataExec:
            rwMask = RWE_EXEC;
            //
            // length must be 0 for exec bp
            //
            Len = 0;
            break;

        default:
            assert(!"Invalid BpType!!");
            break;
    }


    switch( Register ) {
      case 0:
        *Dr0          = DataAddr;
        Dr7->Len0     = Len;
        Dr7->Rwe0     = rwMask;
        Dr7->L0       = 0x01;
        break;
      case 1:
        *Dr1          = DataAddr;
        Dr7->Len1     = Len;
        Dr7->Rwe1     = rwMask;
        Dr7->L1       = 0x01;
        break;
      case 2:
        *Dr2          = DataAddr;
        Dr7->Len2     = Len;
        Dr7->Rwe2     = rwMask;
        Dr7->L2       = 0x01;
        break;
      case 3:
        *Dr3          = DataAddr;
        Dr7->Len3     = Len;
        Dr7->Rwe3     = rwMask;
        Dr7->L3       = 0x01;
        break;
    }

    Dr7->LE = 0x01;

#ifdef KERNEL
    ksr.KernelDr6 = 0;
    return SetExtendedContext(hthd, &ksr);
#else
    Context.Dr6 = 0;
    return SetThreadContext(hthd->rwHand, &Context);
#endif
#endif
}

#ifndef KERNEL

VOID
ClearAllDebugRegisters(
	HPRCX hprc
	)
{
#if 0
    HTHDX    hthd;
    CONTEXT     Context;

    Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    /*
    ** OSDebug only supports hardware breakpoints across all threads;
    ** NT supports them on a per-thread basis.  So we just set the
    ** specified breakpoint on all currently existing threads.
    */

    for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {

		if (!(hthd->tstate & ts_dead)) {

			if (!GetThreadContext(hthd->rwHand, &Context)) {
                continue;
			}

			Context.Dr7 = 0;
            SetThreadContext(hthd->rwHand, &Context);
		}
	}
#endif
}
#endif


VOID
ClearDebugRegister(
    HTHDX   hthd,
    int     Register
    )
{
	_asm int 3
#if 0
#ifdef KERNEL
    KSPECIAL_REGISTERS  ksr;
    PDWORD  Dr0 = &ksr.KernelDr0;
    PDWORD  Dr1 = &ksr.KernelDr1;
    PDWORD  Dr2 = &ksr.KernelDr2;
    PDWORD  Dr3 = &ksr.KernelDr3;
    PDR7    Dr7 = (PDR7)&(ksr.KernelDr7);
#else
    CONTEXT     Context;
    PDWORD  Dr0 = &Context.Dr0;
    PDWORD  Dr1 = &Context.Dr1;
    PDWORD  Dr2 = &Context.Dr2;
    PDWORD  Dr3 = &Context.Dr3;
    PDR7    Dr7 = (PDR7)&(Context.Dr7);
#endif


#ifdef KERNEL
    if (GetExtendedContext(hthd, &ksr))
#else
    Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (GetThreadContext(hthd->rwHand, &Context))
#endif
    {

        switch( Register ) {
          case 0:
            *Dr0          = 0;
            Dr7->Len0     = 0;
            Dr7->Rwe0     = 0;
            Dr7->L0       = 0;
            break;
          case 1:
            *Dr1          = 0;
            Dr7->Len1     = 0;
            Dr7->Rwe1     = 0;
            Dr7->L1       = 0;
            break;
          case 2:
            *Dr2          = 0;
            Dr7->Len2     = 0;
            Dr7->Rwe2     = 0;
            Dr7->L2       = 0;
            break;
          case 3:
            *Dr3          = 0;
            Dr7->Len3     = 0;
            Dr7->Rwe3     = 0;
            Dr7->L3       = 0;
            break;
        }

#ifdef KERNEL
        ksr.KernelDr6 = 0;
        SetExtendedContext(hthd, &ksr);
#else
        Context.Dr6 = 0;
        SetThreadContext( hthd->rwHand, &Context );
#endif
    }
#endif
}
#endif // 0

#ifndef KERNEL

BOOL
IsBogusWin95SSEvent(
    HTHDX           hthd,
    DEBUG_EVENT*    de,
    PDWORD          eventCode,
    PDWORD          subClass
    )
{
#if 0
    if (IsChicago () &&
        !PeeIsEventExpected(hthd, *eventCode, *subClass, FALSE)) {
        return TRUE;
    }
#endif

    return FALSE;
}

#else

BOOL
IsBogusWin95SSEvent(
    HTHDX           hthd,
    DEBUG_EVENT*    de,
    PDWORD          eventCode,
    PDWORD          subClass
    )
{
    return FALSE;
}

#endif

#if 0
BOOL
DecodeSingleStepEvent(
    HTHDX           hthd,
    DEBUG_EVENT*    de,
    PDWORD          eventCode,
    PDWORD          subClass
    )
/*++

Routine Description:


Arguments:

    hthd    - Supplies thread that has a single step exception pending

    de      - Supplies the DEBUG_EVENT structure for the exception

    eventCode - Returns the remapped debug event id

    subClass - Returns the remapped subClass id


Return Value:

    TRUE if event was a real single step or was successfully mapped
    to a breakpoint.  FALSE if a register breakpoint occurred which was
    not expected.

--*/
{
    DWORD       dr6 = 0;
    PBREAKPOINT bp;

#ifdef KERNEL

    KSPECIAL_REGISTERS ksr;

    GetExtendedContext( hthd, &ksr);
    dr6 = ksr.KernelDr6;

#else

	dr6 = 0;

#if 0 // XBUG -- fix for hardware BPs
    CONTEXT     Context;

    Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    DbgGetThreadContext( hthd, &Context);
    dr6 = Context.Dr6;

    Context.Dr6 = 0;
    DbgSetThreadContext( hthd, &Context );
#endif
#endif

#define DR6_HARDWARE_BP_FIRED_MASK  0x0f
#define DR6_BS_FLAG                 0x4000

    //
    // NOTE: SingleStep and watchpoints/databreakpoints are NOT mutually
    // exclusive.  This means DR6 may signal that both of these events happend.
    // In this case, if the breakpoint is going to be fired, we allow the
    // databp to take precedence.  Otherwise -- if the databp is not going to
    // be fired -- we allow the step to take precedence.
    //

    //
    // Algorithm:
    //
    //  If only BS flag is set or BP_FIRED is set, then do that event.
    //  Otherwise, allow the BP_FIRED event take precedence if we check
    //  the BP and it should be fired.  This will end up ignoring the SS
    //  event, which really does not seem like a problem.
    //

    if (dr6 & DR6_HARDWARE_BP_FIRED_MASK) {

        bp = GetWalkBPFromBits (hthd, dr6 & DR6_HARDWARE_BP_FIRED_MASK);

        if (bp) {
            if (CheckDataBP (hthd, bp)) {
                de->dwDebugEventCode = *eventCode = BREAKPOINT_DEBUG_EVENT;
                *subClass = (DWORD) bp;
                de->u.Exception.ExceptionRecord.ExceptionCode = *subClass;
                return TRUE;
            } else {
                if (!(dr6 & DR6_BS_FLAG)) {
                    *eventCode = BOGUS_WIN95_SINGLESTEP_EVENT;
                    de->dwDebugEventCode = BOGUS_WIN95_SINGLESTEP_EVENT;
                    *subClass = 0;
                    de->u.Exception.ExceptionRecord.ExceptionCode = 0;
                    return TRUE;
                }
            }
        }

    }

    if (dr6 & DR6_BS_FLAG) {

        if (IsBogusWin95SSEvent (hthd, de, eventCode, subClass)) {

            // Win95 will sometimes give us totally bogus single step events
            // out of the blue. Map them to a new internal event code, so we
            // can ignore them later.

            // Win95 bogus events are not limited to true "single step" events.
            // We also get bogus events without the DR6.BS flag set.

            de->dwDebugEventCode = *eventCode = BOGUS_WIN95_SINGLESTEP_EVENT;
            de->u.Exception.ExceptionRecord.ExceptionCode = *subClass = 0;

            return TRUE;

        } else {

            //
            // Normal single step event; the event should be correct.
            
            return TRUE;
        }
    }

    //
    // Note: we often get cases where dr6 == 0.  This is a single step.
    //

    return TRUE;
}
#endif // 0





ULONGLONG
GetRegValue(
    PCONTEXT regs,
    int cvindex
    )
{
    switch (cvindex) {
        case CV_REG_EAX:
            return regs->Eax;

        case CV_REG_EBX:
            return regs->Ebx;

        case CV_REG_ECX:
            return regs->Ecx;

        case CV_REG_EDX:
            return regs->Edx;

        case CV_REG_ESP:
            return regs->Esp;

        case CV_REG_EBP:
            return regs->Ebp;

        case CV_REG_ESI:
            return regs->Esi;

        case CV_REG_EDI:
            return regs->Edi;

        case CV_REG_EIP:
            return regs->Eip;

        default:
            assert(!"GetRegValue called with unrecognized index");
            return (ULONGLONG)0 - 1;

    }
}


//----------------------------------------------------------------------------
// Code related to exception handling below.

#ifndef KERNEL


/*
 * PrnTopOfExceptionStack()
 *
 * Returns the address of the Registration Node currently at the top of the NT
 * exception stack
 */

RN *
PrnTopOfExceptionStack(
    HTHDX hthd
    )
{
    RN *        prn;
    DWORD       cb;

    // read registration node from FS:0
    if ((DbgReadMemory(hthd->hprc, (char *)hthd->offTeb + except_list,
        &prn, sizeof(prn), &cb) ==0) ||
        (cb != sizeof(prn))
    )
    {
        // 0xFFFFFFFF is the value NT uses to indicate an empty stack, so
        // that's what we'll use if the read failed
        prn = (RN FAR *) 0xFFFFFFFF;
    }

    return prn;
}


// Helper function to add an entry to the exception handler list.
// Adds the lpAddr passed in to the EXHDLR allocating new memory if
// neccessary. Returns back the count of the number of elements
// allocated in EXHDLR

BOOL
AddToExHdlr(
    EXHDLR *pExHdlr,
    ADDR *lpAddr,
    DWORD *lpdwCountAlloc
    )
{
    assert(pExHdlr);
    assert(lpAddr);
    assert(lpdwCountAlloc);

    if (pExHdlr->count == *lpdwCountAlloc) {
        (*lpdwCountAlloc) *= 2;
        MHRealloc(pExHdlr, sizeof(EXHDLR) + (*lpdwCountAlloc) * sizeof(ADDR));
        if (pExHdlr == NULL) {
            return FALSE;
        }
    } else {
        assert(pExHdlr->count < *lpdwCountAlloc);
        if (pExHdlr->count > *lpdwCountAlloc) {
            return FALSE;
        }
    }

    pExHdlr->addr[pExHdlr->count] = (*lpAddr);
    pExHdlr->count++;

    return TRUE;
}

DWORD
GetSPFromNLGDest(
    HTHDX hthd,
    LPNLG_DESTINATION pNlgDest
    )
{
        Unreferenced(hthd);
        return (pNlgDest->uoffFramePointer);
}

void *
InfoExceptionDuringStep(
    HTHDX hthd
    )
{
        // Information that needs to be propagated from the step location
        // to the action handler when an exception occurs is passed in the
        // void * returned by this function.
        return PrnTopOfExceptionStack(hthd);
}

EXHDLR *
GetExceptionCatchLocations(
    HTHDX hthd,
    LPVOID lpv
    )
{
    BOOL fFoundFinally = FALSE;     // have we found any try/finally's yet
                                    // which are not nested inside the active
                                    // function call.
    BOOL fFoundExcept = FALSE;      // have we found any try/except's
    STE             ste;
    DWORD           cb;
    SCOPE           scope;
    CHAR            rgbSig[4];      // signature, "XC00" or 0x19930520
    BYTE            rgbLH[10];      // Lang Hdlr: "mov eax, ...; jmp ..."
    FUNCINFO        funcinfo;       // C++ EH function info
    TBME            tbme;
    HT              ht;
    DWORD           itry;
    DWORD           icatch;
    INT             thunk;
    UOFFSET         uoffThunk;
    int             countAllocated = 32;    // Initial no of entries in ExHdlr.
    RN              rn;
    RN *            prn;
    RN *            prnOldTopOfStack = (RN *)lpv;
    BOOL            fInsideCall;
    ADDR            addr = {0};
    HPRCX           hprc = hthd->hprc;
    EXHDLR *        pExHdlr;

    pExHdlr = (EXHDLR *)MHAlloc(sizeof(EXHDLR) + countAllocated * sizeof(ADDR));
    pExHdlr->count = 0;  // None allocated yet.

    if (pExHdlr == NULL) {
        return NULL;
    }

    // initialize pieces of the addr correctly.
    AddrFromHthdx(&addr, hthd);

    prn = PrnTopOfExceptionStack(hthd);

    if (prnOldTopOfStack) {
            fInsideCall = TRUE;
    } else {
            fInsideCall = FALSE;
    }

    // Walk through all active registration nodes

    while ( ( prn != NULL ) && ( prn != (RN FAR *)0xFFFFFFFF ) ) {

        //
        // Remember if the registration node we've just reached is equal to
        // the old top of stack.
        //
        if (prn == prnOldTopOfStack) {
            fInsideCall = FALSE;
        }

        //
        // Read the registration node
        //

        if (!DbgReadMemory(hprc, prn, &rn, sizeof(rn), &cb)) {
            assert(FALSE); // this can only happen if exception list is bogus!
            break;
        }

        //
        // If lpfnLanguageHandler actually points to a thunk, figure out
        // where the thunk points
        //
        IsThunk(hthd, (ULONG) rn.lpfnLanguageHandler, &thunk, &uoffThunk, NULL);

        if (thunk != THUNK_NONE) {
            rn.lpfnLanguageHandler = (LPFN) uoffThunk;
        }

        //
        // The code below assumes that the implementation-dependent part of
        // the Registration Node matches the Microsoft C format.  To make
        // sure this is the case, we check for a special signature.  There
        // are two formats supported:
        //
        // (1)  SEH format: check for signature in front of the
        //              lpfnLanguageHandler.  In the C runtime,
        //              _except_handler2() is immediately preceeded by the
        //              bytes "XC00".
        // (2)  C++ EH format: the lpfnLanguageHandler points to:
        //                      mov eax, offset FuncInfo
        //                      jmp __CxxFrameHandler
        //              The FuncInfo structure begins with a magic number
        //              which consists of the number 0x19930520.
        //
        // If neither of these signatures is present, the node must have
        // been created either by assembly code or by some language product
        // other than Microsoft C, so we will skip over this registration
        // node since we don't know its format.
        //
        if (
            // lpfnLanguageHandler points to SEH handler?
            DbgReadMemory(hprc,
                    (LPVOID) ((ULONG)rn.lpfnLanguageHandler - sizeof(rgbSig)),
                    rgbSig, sizeof(rgbSig), &cb)
            &&
            (memcmp(rgbSig, "XC00", sizeof(rgbSig)) == 0)
           )
        {
            // Walk through all active scope table entries

            for (scope=rn.scopeCur; scope != scopeNil; scope=ste.scopeEnclosing) {
                // Read the scopetable entry (UNDONE: we're reading lots of
                // tiny chunks which are also close to each other -- we could
                // optimize this!)

                if (!DbgReadMemory(hprc,
                        (LPVOID)(rn.rgste + scope*sizeof(STE)), &ste, sizeof(ste),
                        &cb))
                {
                    // this can only happen if scope list is bogus!
                    assert(FALSE);
                    break;
                }

                // Set a breakpoint if this is a try/except or the first
                // try/finally which is not nested inside the function
                // call that we're stepping over

                if (ste.lpfnFilter || (!fFoundFinally && !fInsideCall)) {
                    // Set a breakpoint at the address of the handler

                    GetAddrOff( addr ) = (UOFFSET) ste.lpfnHandler;

                    // We are not interested in stopping if this is a nested
                    // handler
                    if (!fInsideCall) {
                        if (!AddToExHdlr(pExHdlr, &addr, &countAllocated)) {
                            break;
                        }
                    }
                }

                if (ste.lpfnFilter == NULL) {
                    fFoundFinally = TRUE;
                } else {
                    fFoundExcept = TRUE;
                }
            }
        }
        else
        if (
            // lpfnLanguageHandler points to C++ EH handler?
            DbgReadMemory(hprc,
                rn.lpfnLanguageHandler, rgbLH, sizeof(rgbLH), &cb)
            &&
            rgbLH[0] == 0xB8 &&             // "mov eax, ..."
            rgbLH[5] == 0xE9 &&             // "jmp ..."
            DbgReadMemory(hprc,
                *(LPVOID*)(rgbLH+1), &funcinfo, sizeof(funcinfo), &cb)
            &&
            funcinfo.dwMagic == 0x19930520
           ) {
            for (itry = 0; itry < funcinfo.dwTryBlocks; ++itry) {
                if (DbgReadMemory(hprc,
                                      funcinfo.ptbme + itry,
                                      &tbme,
                                      sizeof(tbme),
                                      &cb)
                ) {
                    for (icatch = 0; icatch < tbme.dwCatches; ++icatch) {
                        if (DbgReadMemory(hprc,
                                              tbme.pht + icatch,
                                              &ht,
                                              sizeof(ht),
                                              &cb)
                        ) {
                            // Set a breakpoint
                            GetAddrOff( addr ) = (UOFFSET) ht.lpfnCatch;

                            // We are not interested in stopping if this is a nested
                            // handler
                            if (!fInsideCall) {
                                if (!AddToExHdlr(pExHdlr, &addr, &countAllocated)) {
                                    break;
                                }
                            }

                            fFoundExcept = TRUE;
                        }
                    }
                }
            }
        }
        // Move to next registration node in chain

        prn = rn.prnNext;
    }

    // Now that we have the whole list, we should
    return pExHdlr;
}


BOOL
GetWndProcMessage(
    HTHDX   hthd,
    UINT*   pmsg
    )
/*++

Routine Description:

    This function is used to get the current Windows message (WM_CREATE, etc)
    from the stack. You must be at the first instruction of the WndProc and
    that instruction must be unexecuted.

Return Value:

    False on failure; True otherwise.

--*/
{

    ADDR    addr;
    BOOL    succ;
    ULONG   cbRead;

    AddrInit (&addr, 0, PcSegOfHthdx (hthd), STACK_POINTER (hthd) + 8,
              hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal);


    succ = AddrReadMemory (hthd->hprc, hthd, &addr, pmsg, 4, &cbRead);

    if (succ && cbRead == 4) {
        return TRUE;
    }

    return FALSE;
}


BOOL
GetSinglePVOIDArgument(
	HTHDX	hthd,
	PVOID*	Argument
	)
/*++

Routine Description:

	Given that we are sitting on a CALL instruction, and that the function
	is a stdcall function taking exactly one PVOID argument, retrieve that
	argument.

--*/
{
	//
	// Read the this pointer from the current ESP.
	//
	
	
	return DbgReadMemory (hthd->hprc,
						  (LPVOID) STACK_POINTER (hthd),
						  Argument,
						  sizeof (PVOID),
						  NULL);
}

BOOL
GetDoublePVOIDArgument(
	HTHDX	hthd,
	DWORD*	Argument1,
	DWORD*	Argument2
	)
/*++

Routine Description:

	Same as GetSinglePVOIDArgument except that for a function taking exactly
	two PVOID arguments.

--*/
{
	BOOL	fSucc = FALSE;
	
	fSucc = DbgReadMemory (hthd->hprc,
						  (LPVOID) STACK_POINTER (hthd),
						  Argument2,
						  sizeof (PVOID),
						  NULL);

	if (fSucc) {

		fSucc = DbgReadMemory (hthd->hprc,
							  (LPVOID) (STACK_POINTER (hthd) + sizeof (PVOID)),
							  Argument1,
							  sizeof (PVOID),
							  NULL);
	}

	return fSucc;
}


#endif // !KERNEL

