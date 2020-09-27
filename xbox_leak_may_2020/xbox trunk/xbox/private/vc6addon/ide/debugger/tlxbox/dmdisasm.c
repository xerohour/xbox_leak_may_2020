/**** DMDISASM.C - EM Lego disassembler interface                          *
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1995, Microsoft Corp                                     *
 *                                                                         *
 *  Created: January 1, 1996 by Kent Forschmiedt
 *                                                                         *
 *  Revision History:                                                      *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "precomp.h"
#pragma hdrstop

#include <simpldis.h>


#define MAXL     20
#define CCHMAX   256

#if defined(TARGET_i386)
#define SIMPLE_ARCH_CURRENT Simple_Arch_X86
#elif defined(TARGET_MIPS)
#define SIMPLE_ARCH_CURRENT Simple_Arch_Mips
#elif defined(TARGET_ALPHA)
#define SIMPLE_ARCH_CURRENT Simple_Arch_AlphaAxp
#elif defined(TARGET_PPC)
#define SIMPLE_ARCH_CURRENT Simple_Arch_PowerPc
#else
#error "Undefined processor"
#endif


int
CvRegFromSimpleReg(
    MPT     mpt,
    int     regInstr
    )
{
    switch (mpt) {
    case mptix86:
        switch(regInstr) {
            case SimpleRegEax: return CV_REG_EAX;
            case SimpleRegEcx: return CV_REG_ECX;
            case SimpleRegEdx: return CV_REG_EDX;
            case SimpleRegEbx: return CV_REG_EBX;
            case SimpleRegEsp: return CV_REG_ESP;
            case SimpleRegEbp: return CV_REG_EBP;
            case SimpleRegEsi: return CV_REG_ESI;
            case SimpleRegEdi: return CV_REG_EDI;
        }
        break;

    case mptmips:
        return (regInstr + CV_M4_IntZERO);

    case mptdaxp:
        return (regInstr + CV_ALPHA_IntV0);

    case mptmppc:
        return (regInstr + CV_PPC_GPR0);

    }
    return (0);
}




#if 0
ULONGLONG
QwGetreg(
    PVOID   pv,
    int     regInstr
    )
{
    HTHDX       hthdx = (HTHDX)pv;
    XOSD        xosd;
    ULONGLONG   retVal;

	_asm int 3
	EnsureHthdReg(hthdx, CONTEXT_FULL | CONTEXT_FLOATING_POINT);
    retVal = GetRegValue(&hthdx->ctx,
                         CvRegFromSimpleReg(MPT_CURRENT, regInstr)
                         );

    return retVal;
}

XOSD
disasm (
    HTHDX  hthd,
    LPSDI  lpsdi,
    PVOID  Memory,
    int    Size
    )
{
    XOSD        xosd      = xosdNone;
    int         cbUsed;
    int         Bytes;
    SIMPLEDIS   Sdis;

    Bytes = SimplyDisassemble(
        Memory,                    // code ptr
        Size,                      // bytes
        GetAddrOff(lpsdi->addr),
        SIMPLE_ARCH_CURRENT,
        &Sdis,
        NULL,
        NULL,
        NULL,
        QwGetreg,
        (PVOID)hthd
        );

    if (Bytes < 0) {
        cbUsed = -Bytes;
        xosd = xosdGeneral;
    } else {
        cbUsed = Bytes;
    }

    return xosd;
}

DWORD
BranchUnassemble(
    HTHDX   hthd,
    PVOID   Memory,
    DWORD   Size,
    LPADDR  Addr,
    BOOL   *IsBranch,
    BOOL   *TargetKnown,
    BOOL   *IsCall,
    BOOL   *IsTable,
    LPADDR  Target
    )
{
    int         cbUsed;
    int         Bytes;
    SIMPLEDIS   Sdis;
    DWORD       dwTarget = 0;


    Bytes = SimplyDisassemble(
        Memory,                    // code ptr
        Size,                      // bytes
        GetAddrOff(*Addr),
        SIMPLE_ARCH_CURRENT,
        &Sdis,
        NULL,
        NULL,
        NULL,
        QwGetreg,
        (PVOID)hthd
        );

    if (Bytes < 0) {
        *IsBranch = FALSE;
        *IsTable = FALSE;
        *IsCall = FALSE;
        *TargetKnown = FALSE;
        return 0;
    }

    *IsBranch = Sdis.IsBranch;
    *IsCall = Sdis.IsCall;
    *IsTable = FALSE;

    if (*IsBranch) {
        //
        // when do we know the branch target, and when do we not?
        //
        if (Sdis.dwJumpTable) {
            *IsTable = TRUE;
            dwTarget = Sdis.dwJumpTable;
        } else {
            *IsTable = FALSE;
            dwTarget = Sdis.dwBranchTarget;
        }
    }

    if (*IsCall) {
        *IsCall = TRUE;
        dwTarget = Sdis.dwBranchTarget;
    }

    *TargetKnown = !!dwTarget;

    if (*TargetKnown) {
        AddrInit( Target, 0, 0, dwTarget, TRUE, TRUE, FALSE, FALSE );
    }

    return Bytes;

}
#endif
