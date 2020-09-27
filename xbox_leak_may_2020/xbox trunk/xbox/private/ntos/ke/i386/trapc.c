/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    trapc.c

Abstract:

    This module contains some trap handling code written in C.
    Only by the kernel.

Author:

    Ken Reneris     6-9-93

Revision History:

--*/

#include    "ki.h"

#define REG(field)          ((ULONG)(&((KTRAP_FRAME *)0)->field))
#define GETREG(frame,reg)   ((PULONG) (((ULONG) frame)+reg))[0]

typedef struct {
    UCHAR   RmDisplaceOnly;     // RM of displacment only, no base reg
    UCHAR   RmSib;              // RM of SIB
    UCHAR   RmDisplace;         // bit mask of RMs which have a displacement
    UCHAR   Disp;               // sizeof displacement (in bytes)
} KMOD, *PKMOD;

static UCHAR RM32[] = {
    /* 000 */   REG(Eax),
    /* 001 */   REG(Ecx),
    /* 010 */   REG(Edx),
    /* 011 */   REG(Ebx),
    /* 100 */   REG(HardwareEsp),
    /* 101 */   REG(Ebp),       // SIB
    /* 110 */   REG(Esi),
    /* 111 */   REG(Edi)
};

static UCHAR RM8[] = {
    /* 000 */   REG(Eax),       // al
    /* 001 */   REG(Ecx),       // cl
    /* 010 */   REG(Edx),       // dl
    /* 011 */   REG(Ebx),       // bl
    /* 100 */   REG(Eax) + 1,   // ah
    /* 101 */   REG(Ecx) + 1,   // ch
    /* 110 */   REG(Edx) + 1,   // dh
    /* 111 */   REG(Ebx) + 1    // bh
};

static KMOD MOD32[] = {
    /* 00 */     5,     4,   0x20,   4,
    /* 01 */  0xff,     4,   0xff,   1,
    /* 10 */  0xff,     4,   0xff,   4,
    /* 11 */  0xff,  0xff,   0x00,   0
} ;

NTSTATUS
Ki386CheckDivideByZeroTrap (
    IN  PKTRAP_FRAME    TrapFrame
    )
/*++

Routine Description:

    This function gains control when the x86 processor generates a
    divide by zero trap.  The x86 design generates such a trap on
    divide by zero and on division overflows.  In order to determine
    which expection code to dispatch, the divisor of the "div" or "idiv"
    instruction needs to be inspected.

Arguments:

    TrapFrame - Trap frame of the divide by zero trap

Return Value:

    exception code dispatch

--*/
{
    ULONG       operandsize, operandmask, i, accum;
    PUCHAR      istream, pRM;
    UCHAR       ibyte, rm;
    PKMOD       Mod;
    BOOLEAN     fPrefix;
    NTSTATUS    status;

    status = STATUS_INTEGER_DIVIDE_BY_ZERO;

    try {

        //
        // read instruction prefixes
        //

        fPrefix = TRUE;
        pRM = RM32;
        operandsize = 4;
        operandmask = 0xffffffff;
        istream = (PUCHAR) TrapFrame->Eip;
        while (fPrefix) {
            ibyte = *istream++;
            switch (ibyte) {
                case 0x2e:  // cs override
                case 0x36:  // ss override
                case 0x3e:  // ds override
                case 0x26:  // es override
                case 0x64:  // fs override
                case 0x65:  // gs override
                case 0xF3:  // rep
                case 0xF2:  // rep
                case 0xF0:  // lock
                    break;

                case 0x66:
                    // 16 bit operand override
                    operandsize = 2;
                    operandmask = 0xffff;
                    break;

                case 0x67:
                    // 16 bit address size override
                    // this is some non-flat code
                    goto try_exit;

                default:
                    fPrefix = FALSE;
                    break;
            }
        }

        //
        // Check instruction opcode
        //

        if (ibyte != 0xf7  &&  ibyte != 0xf6) {
            // this is not a DIV or IDIV opcode
            goto try_exit;
        }

        if (ibyte == 0xf6) {
            // this is a byte div or idiv
            operandsize = 1;
            operandmask = 0xff;
        }

        //
        // Get Mod R/M
        //

        ibyte = *istream++;
        Mod = MOD32 + (ibyte >> 6);
        rm  = (UCHAR)(ibyte & 7);

        //
        // put register values into accum
        //

        if (operandsize == 1  &&  (ibyte & 0xc0) == 0xc0) {
            pRM = RM8;
        }

        accum = 0;
        if (rm != Mod->RmDisplaceOnly) {
            if (rm == Mod->RmSib) {
                // get SIB
                ibyte = *istream++;
                i = (ibyte >> 3) & 7;
                if (i != 4) {
                    accum = GETREG(TrapFrame, RM32[i]);
                    accum = accum << (ibyte >> 6);    // apply scaler
                }
                i = ibyte & 7;
                accum = accum + GETREG(TrapFrame, RM32[i]);
            } else {
                // get register's value
                accum = GETREG(TrapFrame, pRM[rm]);
            }
        }

        //
        // apply displacement to accum
        //

        if (Mod->RmDisplace & (1 << rm)) {
            if (Mod->Disp == 4) {
                i = *((PULONG) istream);
            } else {
                ibyte = *istream;
                i = (signed long) ((signed char) ibyte);    // sign extend
            }
            accum += i;
        }

        //
        // if this is an effective address, go get the data value
        //

        if (Mod->Disp) {
            switch (operandsize) {
                case 1:  accum = *((PUCHAR) accum);    break;
                case 2:  accum = *((PUSHORT) accum);  break;
                case 4:  accum = *((PULONG) accum);    break;
            }
        }

        //
        // accum now contains the instruction operand, see if the
        // operand was really a zero
        //

        if (accum & operandmask) {
            // operand was non-zero, must be an overflow
            status = STATUS_INTEGER_OVERFLOW;
        }

try_exit: ;
    } except (EXCEPTION_EXECUTE_HANDLER) {
        // do nothing...
    }

    return status;
}
