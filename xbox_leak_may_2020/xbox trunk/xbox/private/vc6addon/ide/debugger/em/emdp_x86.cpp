/**** EMDPDEV.C - Debugger end Execution Model (x86 dependent code)       **
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1990, Microsoft Corp                                     *
 *                                                                         *
 *  Created: October 15, 1990 by David W. Gray                             *
 *                                                                         *
 *  Revision History:                                                      *
 *                                                                         *
 *  Purpose:                                                               *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#define TARGET_i386
#include "emdp_plt.h"
#include "str_x86.h"

typedef struct _XMMI128 {   
    union {        
        float     fs[4];
        __int64   dl[2];
        double    fd[2];       
    } u;
} XMMI128, *PXMMI128;  

typedef struct _XMMI_AREA {
    XMMI128  Xmmi[8];
} XMMI_AREA, *PXMMI_AREA;

typedef struct _FLOATING_EXTENDED_SAVE_AREA {
    USHORT  ControlWord;
    USHORT  StatusWord;
    USHORT  TagWord;
    USHORT  ErrorOpcode;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    ULONG   MXCsr;
    ULONG   Reserved2;
    UCHAR   X87RegisterArea[128];
    UCHAR   Reserved3[128];
    UCHAR   Reserved4[224];
} FLOATING_EXTENDED_SAVE_AREA, *PFLOATING_EXTENDED_SAVE_AREA;

RD X86Rgrd[] = {
#include "regs_x86.h"
};
const unsigned CX86Rgrd = (sizeof(X86Rgrd)/sizeof(X86Rgrd[0]));

RGFD X86Rgfd[] = {
#include "flag_x86.h"
};
const unsigned CX86Rgfd = (sizeof(X86Rgfd)/sizeof(X86Rgfd[0]));





typedef struct _L_DOUBLE {
    BYTE b[10];
} L_DOUBLE, FAR *LPL_DOUBLE;

#pragma pack(push, 1)
// this is how MMX registers are stored within the 10 bytes of FPU regs
typedef struct _L_MMX {
	__int64 MMValue;
	WORD MMExponent;
} L_MMX, *LPL_MMX;

// and 3DNow floats are like this
typedef struct _L_3DNOW {
	float Float[2];
	WORD MMExponent;
} L_3DNOW, *LPL_3DNOW;
#pragma pack(pop)


#define CEXM_MDL_native 0x20


#ifdef TARGET32
#define SIZEOF_STACK_OFFSET sizeof(LONG)
#else // TARGET32
#define SIZEOF_STACK_OFFSET sizeof(WORD)
#endif // TARGET32

LPVOID X86SwFunctionTableAccess(LPVOID lpvhpid, DWORD   AddrBase);
void   X86UpdateSpecialRegisters (HPRC hprc, HTHD hthd);

XOSD
X86GetAddr (
    HPID   hpid,
    HTID   htid,
    ADR    adr,
    LPADDR lpaddr
    );

XOSD
X86SetAddr (
    HPID   hpid,
    HTID   htid,
    ADR    adr,
    LPADDR lpaddr
    );

LPVOID
X86DoGetReg(
    LPVOID lpregs1,
    DWORD ireg,
    LPVOID lpvRegValue
    );

XOSD
X86GetRegValue (
    HPID hpid,
    HTID htid,
    DWORD ireg,
    LPVOID lpvRegValue
    );

LPVOID
X86DoSetReg(
    LPVOID   lpregs1,
    DWORD    ireg,
    LPVOID   lpvRegValue
    );

XOSD
X86SetRegValue (
    HPID hpid,
    HTID htid,
    DWORD ireg,
    LPVOID lpvRegValue
    );

XOSD
X86DoGetFrame(
    HPID hpid,
    HTID uhtid,
    DWORD wValue,
    DWORD lValue
    );

XOSD
X86DoGetFrameEH(
    HPID hpid,
    HTID htid,
    LPEXHDLR *lpexhdlr,
    LPDWORD cAddrsAllocated
);

XOSD
X86UpdateChild (
    HPID hpid,
    HTID htid,
    DMF dmfCommand
    );

void
X86CopyFrameRegs(
    LPTHD lpthd,
    LPBPR lpbpr
    );

void
X86AdjustForProlog(
    HPID hpid,
    HTID htid,
    PADDR origAddr,
    CANSTEP *CanStep
    );

XOSD
X86DoGetFunctionInfo(
    HPID hpid,
    LPGFI lpgfi
    );

XOSD
X86GetFrameWithSrc(
	HPID	hpid,
	HTID	htid,
	UINT	cFrames,
	LPHTID	lphtid
	);

CPU_POINTERS X86Pointers = {
    sizeof(CONTEXT),        //  size_t SizeOfContext;
    X86Rgfd,                //  RGFD * Rgfd;
    X86Rgrd,                //  RD   * Rgrd;
    CX86Rgfd,               //  int    CRgfd;
    CX86Rgrd,               //  int    CRgrd;

    X86GetAddr,             //  PFNGETADDR          pfnGetAddr;
    X86SetAddr,             //  PFNSETADDR          pfnSetAddr;
    X86DoGetReg,            //  PFNDOGETREG         pfnDoGetReg;
    X86GetRegValue,         //  PFNGETREGVALUE      pfnGetRegValue;
    X86DoSetReg,            //  PFNSETREG           pfnDoSetReg;
    X86SetRegValue,         //  PFNSETREGVALUE      pfnSetRegValue;
    XXGetFlagValue,         //  PFNGETFLAG          pfnGetFlagValue;
    XXSetFlagValue,         //  PFNSETFLAG          pfnSetFlagValue;
    X86DoGetFrame,          //  PFNGETFRAME         pfnGetFrame;
    X86DoGetFrameEH,        //  PFNGETFRAMEEH       pfnGetFrameEH;
    X86UpdateChild,         //  PFNUPDATECHILD      pfnUpdateChild;
    X86CopyFrameRegs,       //  PFNCOPYFRAMEREGS    pfnCopyFrameRegs;
    X86AdjustForProlog,     //  PFNADJUSTFORPROLOG  pfnAdjustForProlog;
    X86DoGetFunctionInfo,   //  PFNGETFUNCTIONINFO  pfnGetFunctionInfo;
	X86GetFrameWithSrc,		//	PFNGETFRAMEWITHSRC  pfnGetFrameWithSrc;
};



XOSD
X86GetAddr (
    HPID   hpid,
    HTID   htid,
    ADR    adr,
    LPADDR lpaddr
    )

/*++

Routine Description:

    This function will get return a specific type of address.

Arguments:

    hpid   - Supplies the handle to the process to retrive the address from

    htid   - Supplies the handle to the thread to retrieve the address from

    adr    - Supplies the type of address to be retrieved

    lpaddr - Returns the requested address

Return Value:

    XOSD error code

--*/

{
    HPRC        hprc;
    HTHD        hthd;
    LPTHD       lpthd = NULL;
    XOSD        xosd = xosdNone;
    HEMI        hemi = emiAddr(*lpaddr);
    HMDI        hmdi;
    LPMDI       lpmdi;
    BOOL        fVhtid;

    assert ( lpaddr != NULL );
    assert ( hpid != NULL );

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }

    fVhtid = ((DWORD)htid & 1);
    if (fVhtid) {
        htid = (HTID) ((DWORD)htid & ~1);
    }

    hthd = HthdFromHtid(hprc, htid);

    if ( hthd != hthdNull ) {
        lpthd = (LPTHD) LLLock ( hthd );
    }

    _fmemset ( lpaddr, 0, sizeof ( ADDR ) );

    if (!fVhtid) {
        switch ( adr ) {
        case adrPC:
            if ( lpthd && !(lpthd->drt & drtCntrlPresent) ) {
                UpdateRegisters ( hprc, hthd );
            }
            break;

        case adrBase:
        case adrStack:
        case adrData:
            if ( lpthd && !(lpthd->drt & drtAllPresent )) {
                UpdateRegisters ( hprc, hthd );
            }
            break;
        }
    }

    switch ( adr ) {

        case adrPC:
            if (!fVhtid) {
                AddrInit(lpaddr, 0, (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegCs,
                     (UOFFSET) ((PCONTEXT) (lpthd->regs))->Eip, lpthd->fFlat,
                     lpthd->fOff32, FALSE, lpthd->fReal);
            } else {
                AddrInit(lpaddr, 0, (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegCs,
                         (UOFFSET) lpthd->StackFrame.AddrPC.Offset,
                         lpthd->fFlat, lpthd->fOff32, FALSE, lpthd->fReal);
            }
            SetEmi ( hpid, lpaddr );
            break;

        case adrBase:
			// Danger: ugly code alert to do with virtual frames [apennell] VS98:6296
			// It turns out that ImageHlp sometimes gives us a zero as the result,
			// mostly gives us the correct value, but on no-EBP functions it gives
			// us a value that is 4 too small. This code handles all of these cases.
            if (!fVhtid) {
				if (lpthd->offBetterTopFrame) {
					AddrInit(lpaddr, 0, (SEGMENT) 0,
						lpthd->offBetterTopFrame, lpthd->fFlat,
						lpthd->fOff32, FALSE, lpthd->fReal);
				} else {
					AddrInit(lpaddr, 0, (SEGMENT) 0,
						(UOFFSET) ((PCONTEXT) (lpthd->regs))->Ebp, lpthd->fFlat,
						lpthd->fOff32, FALSE, lpthd->fReal);
				}
            } else {
				UOFFSET uBase = (UOFFSET)lpthd->StackFrame.AddrFrame.Offset;
				PFPO_DATA pFpo = (PFPO_DATA)lpthd->StackFrame.FuncTableEntry;
				if (pFpo && !pFpo->fUseBP)
					uBase += 4;
                AddrInit(lpaddr, 0, (SEGMENT) 0,
                          uBase,
                         lpthd->fFlat, lpthd->fOff32, FALSE, lpthd->fReal);
            }
            SetEmi ( hpid, lpaddr );
            break;

        case adrData:
            AddrInit(lpaddr, 0, (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegDs, 0,
                     lpthd->fFlat, lpthd->fOff32, FALSE, lpthd->fReal);
            SetEmi ( hpid, lpaddr );
            break;

        case adrTlsBase:
            /*
             * If -1 then we have not gotten a value from the DM yet.
             */

            assert(hemi != 0);

            if (hemi == 0) {
                return xosdBadAddress;
            }

            if (hemi != emiAddr(lpthd->addrTls)) {
                hmdi = LLFind( LlmdiFromHprc( hprc ), 0, (LPBYTE) &hemi,
                                                                      emdiEMI);
                assert(hmdi != 0);

                if (hmdi == 0) {
                    return xosdBadAddress;
                }

                lpmdi = (LPMDI) LLLock( hmdi );

                GuardTL();
                SendRequestX( dmfQueryTlsBase, hpid, htid, sizeof(OFFSET),
                             &lpmdi->lpBaseOfDll);

                lpthd->addrTls = *((LPADDR) LpDmMsg->rgb);
                ReleaseTL();
                emiAddr(lpthd->addrTls) = hemi;
                LLUnlock( hmdi );

            }

            *lpaddr = lpthd->addrTls;
            emiAddr(*lpaddr) = 0;
            break;

        case adrStack:
            if (!fVhtid) {
                AddrInit(lpaddr, 0, (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegSs,
                         (UOFFSET) ((PCONTEXT) (lpthd->regs))->Esp, lpthd->fFlat,
                         lpthd->fOff32, FALSE, lpthd->fReal);
            } else {
                AddrInit(lpaddr, 0, (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegSs,
                         (UOFFSET)lpthd->StackFrame.AddrStack.Offset,
                         lpthd->fFlat, lpthd->fOff32, FALSE, lpthd->fReal);
            }
            SetEmi ( hpid, lpaddr );
            break;

        default:
            assert ( FALSE );
            break;
    }

    if ( hthd != hthdNull ) {
        LLUnlock ( hthd );
    }

    return xosd;
}                               /* GetAddr() */


XOSD
X86SetAddr (
    HPID   hpid,
    HTID   htid,
    ADR    adr,
    LPADDR lpaddr
    )
{
    HPRC  hprc;
    HTHD  hthd;
    LPTHD lpthd = NULL;

    assert ( lpaddr != NULL );
    assert ( hpid != NULL );

    assert ( ((DWORD)htid & 1) == 0 );

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }

    hthd = HthdFromHtid(hprc, htid);


    if ( hthd != hthdNull ) {
        lpthd = (LPTHD) LLLock ( hthd );
    }

    switch ( adr ) {
        case adrPC:
            if ( !( lpthd->drt & drtCntrlPresent) ) {
                UpdateRegisters ( hprc, hthd );
            }
            break;


        case adrBase:
        case adrStack:
        case adrData:
            if ( !(lpthd->drt & drtAllPresent) ) {
                UpdateRegisters ( hprc, hthd );
            }
            break;

    }
    switch ( adr ) {
        case adrPC:
            ((PCONTEXT) (lpthd->regs))->SegCs = GetAddrSeg ( *lpaddr );
            ((PCONTEXT) (lpthd->regs))->Eip = GetAddrOff ( *lpaddr );
            lpthd->drt = (DRT) (lpthd->drt | drtCntrlDirty);
            break;

        case adrBase:
            ((PCONTEXT) (lpthd->regs))->Ebp = GetAddrOff ( *lpaddr );
            lpthd->drt = (DRT) (lpthd->drt | drtAllDirty);
            break;

        case adrStack:
            ((PCONTEXT) (lpthd->regs))->SegSs = GetAddrSeg ( *lpaddr );
            ((PCONTEXT) (lpthd->regs))->Esp = GetAddrOff ( *lpaddr );
            lpthd->drt = (DRT) (lpthd->drt | drtAllDirty);
            break;

        case adrData:
        case adrTlsBase:
        default:
            assert ( FALSE );
            break;
    }

    if ( hthd != hthdNull ) {
        LLUnlock ( hthd );
    }

    return xosdNone;
}                               /* SetAddr() */


XOSD
X86SetAddrFromCSIP (
    HTHD hthd
    )
{

    ADDR addr = {0};
    LPTHD lpthd;

    assert ( hthd != hthdNull && hthd != hthdInvalid );

    lpthd = (LPTHD) LLLock ( hthd );

    GetAddrSeg ( addr ) = (SEGMENT) ((PCONTEXT) (lpthd->regs))->SegCs;
    GetAddrOff ( addr ) = (UOFFSET) ((PCONTEXT) (lpthd->regs))->Eip;
    emiAddr ( addr ) =  0;
    ADDR_IS_FLAT ( addr ) = TRUE;

    LLUnlock ( hthd );

    return xosdNone;
}


LPVOID
X86DoGetReg(
    LPVOID lpregs1,
    DWORD ireg,
    LPVOID lpvRegValue
    )

/*++

Routine Description:

    This routine is used to extract the value of a single register from
    the debuggee.

Arguments:

    lpregs      - Supplies pointer to the register set for the debuggee
    ireg        - Supplies the index of the register to be read
    lpvRegValue - Supplies the buffer to place the register value in

Return Value:

    return-value - lpvRegValue + size of register on sucess and NULL on
                failure
--*/

{
    int         i;
    LPCONTEXT  lpregs = (LPCONTEXT) lpregs1;
    DWORD dwProcessorFlags = *(PDWORD)lpvRegValue; //no other way to pass the processor flags in 6.0

	assert( sizeof(L_MMX)==10 );			// if this fires, struct packing is wrong

    switch ( ireg ) {

    case CV_REG_AL:
        *( (LPBYTE) lpvRegValue ) = (BYTE) lpregs->Eax;
        break;

    case CV_REG_CL:
        *( (LPBYTE) lpvRegValue ) = (BYTE) lpregs->Ecx;
        break;

    case CV_REG_DL:
        *( (LPBYTE) lpvRegValue ) = (BYTE) lpregs->Edx;
        break;

    case CV_REG_BL:
        *( (LPBYTE) lpvRegValue ) = (BYTE) lpregs->Ebx;
        break;

    case CV_REG_AH:
        *( (LPBYTE) lpvRegValue ) = (BYTE) (lpregs->Eax >> 8);
        break;

    case CV_REG_CH:
        *( (LPBYTE) lpvRegValue ) = (BYTE) (lpregs->Ecx >> 8);
        break;

    case CV_REG_DH:
        *( (LPBYTE) lpvRegValue ) = (BYTE) (lpregs->Edx >> 8);
        break;

    case CV_REG_BH:
        *( (LPBYTE) lpvRegValue ) = (BYTE) (lpregs->Ebx >> 8);
        break;

    case CV_REG_AX:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Eax;
        break;

    case CV_REG_CX:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Ecx;
        break;

    case CV_REG_DX:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Edx;
        break;

    case CV_REG_BX:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Ebx;
        break;

    case CV_REG_SP:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Esp;
        break;

    case CV_REG_BP:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Ebp;
        break;

    case CV_REG_SI:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Esi;
        break;

    case CV_REG_DI:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Edi;
        break;

    case CV_REG_IP:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->Eip;
        break;

    case CV_REG_FLAGS:
        *( (LPWORD) lpvRegValue ) = (WORD) lpregs->EFlags;
        break;

    case CV_REG_ES:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegEs;
        break;

    case CV_REG_CS:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegCs;
        break;

    case CV_REG_SS:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegSs;
        break;

    case CV_REG_DS:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegDs;
        break;

    case CV_REG_FS:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegFs;
        break;

    case CV_REG_GS:
        *( (LPWORD) lpvRegValue ) = (SEG16) lpregs->SegGs;
        break;

    case CV_REG_EAX:
        *( (LPLONG) lpvRegValue ) = lpregs->Eax;
        break;

    case CV_REG_ECX:
        *( (LPLONG) lpvRegValue ) = lpregs->Ecx;
        break;

    case CV_REG_EDX:
        *( (LPLONG) lpvRegValue ) = lpregs->Edx;
        break;

    case CV_REG_EBX:
        *( (LPLONG) lpvRegValue ) = lpregs->Ebx;
        break;

    case CV_REG_ESP:
        *( (LPLONG) lpvRegValue ) = lpregs->Esp;
        break;

    case CV_REG_EBP:
        *( (LPLONG) lpvRegValue ) = lpregs->Ebp;
        break;

    case CV_REG_ESI:
        *( (LPLONG) lpvRegValue ) = lpregs->Esi;
        break;

    case CV_REG_EDI:
        *( (LPLONG) lpvRegValue ) = lpregs->Edi;
        break;

    case CV_REG_EIP:
        *( (LPLONG) lpvRegValue ) = lpregs->Eip;
        break;

    case CV_REG_EFLAGS:
        *( (LPLONG) lpvRegValue ) = lpregs->EFlags;
        break;

    case CV_REG_ST0:
    case CV_REG_ST1:
    case CV_REG_ST2:
    case CV_REG_ST3:
    case CV_REG_ST4:
    case CV_REG_ST5:
    case CV_REG_ST6:
    case CV_REG_ST7:

//        i = (lpregs->FloatSave.StatusWord >> 11) & 0x7;
//        i = (i + ireg - CV_REG_ST0) % 8;

          i = ireg - CV_REG_ST0;

        *( (LPL_DOUBLE) lpvRegValue ) =
          ((LPL_DOUBLE)(lpregs->FloatSave.RegisterArea))[ i ];
        break;

	case CV_REG_MM0:
	case CV_REG_MM1:
	case CV_REG_MM2:
	case CV_REG_MM3:
	case CV_REG_MM4:
	case CV_REG_MM5:
	case CV_REG_MM6:
	case CV_REG_MM7:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX) {
            LPL_MMX pReg = &((LPL_MMX)(lpregs->FloatSave.RegisterArea))[ ireg - CV_REG_MM0 ];
            *( (__int64*) lpvRegValue ) = pReg->MMValue; 
        } else {
            return NULL;
        }
		break;

    case CV_REG_EMM0L:
    case CV_REG_EMM1L:
    case CV_REG_EMM2L:
    case CV_REG_EMM3L:
    case CV_REG_EMM4L:
    case CV_REG_EMM5L:
    case CV_REG_EMM6L:
    case CV_REG_EMM7L:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3) {
            *((PDWORDLONG)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_EMM0L].u.dl[0];
        } else {
            lpvRegValue = NULL;
        }
		break;

    case CV_REG_EMM0H:
    case CV_REG_EMM1H:
    case CV_REG_EMM2H:
    case CV_REG_EMM3H:
    case CV_REG_EMM4H:
    case CV_REG_EMM5H:
    case CV_REG_EMM6H:
    case CV_REG_EMM7H:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3) {
            *((PDWORDLONG)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_EMM0H].u.dl[1];
        } else {
            lpvRegValue = NULL;
        }
		break;

    case CV_REG_XMM0L:
    case CV_REG_XMM1L:
    case CV_REG_XMM2L:
    case CV_REG_XMM3L:
    case CV_REG_XMM4L:
    case CV_REG_XMM5L:
    case CV_REG_XMM6L:
    case CV_REG_XMM7L:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3) {
            *((double *)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0L].u.fd[0];
        } else {
            lpvRegValue = NULL;
        }
		break;

    case CV_REG_XMM0H:
    case CV_REG_XMM1H:
    case CV_REG_XMM2H:
    case CV_REG_XMM3H:
    case CV_REG_XMM4H:
    case CV_REG_XMM5H:
    case CV_REG_XMM6H:
    case CV_REG_XMM7H:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3) {
            *((double *)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0H].u.fd[1];
        } else {
            lpvRegValue = NULL;
        }
		break;

    case CV_REG_XMM0:
    case CV_REG_XMM1:
    case CV_REG_XMM2:
    case CV_REG_XMM3:
    case CV_REG_XMM4:
    case CV_REG_XMM5:
    case CV_REG_XMM6:
    case CV_REG_XMM7:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2) {
            *((PXMMI128)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0];
        } else {
            lpvRegValue = NULL;
        }
		break;

    case CV_REG_XMM00:
	case CV_REG_XMM01:
	case CV_REG_XMM02:
	case CV_REG_XMM03:
	case CV_REG_XMM10:
	case CV_REG_XMM11:
	case CV_REG_XMM12:
	case CV_REG_XMM13:
    case CV_REG_XMM20:
	case CV_REG_XMM21:
	case CV_REG_XMM22:
	case CV_REG_XMM23:
	case CV_REG_XMM30:
	case CV_REG_XMM31:
	case CV_REG_XMM32:
	case CV_REG_XMM33:
    case CV_REG_XMM40:
	case CV_REG_XMM41:
	case CV_REG_XMM42:
	case CV_REG_XMM43:
	case CV_REG_XMM50:
	case CV_REG_XMM51:
	case CV_REG_XMM52:
	case CV_REG_XMM53:
    case CV_REG_XMM60:
	case CV_REG_XMM61:
	case CV_REG_XMM62:
	case CV_REG_XMM63:
	case CV_REG_XMM70:
	case CV_REG_XMM71:
	case CV_REG_XMM72:
	case CV_REG_XMM73:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2) {
			*((PFLOAT)lpvRegValue) = ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[(ireg - CV_REG_XMM00) >> 2].u.fs[(ireg - CV_REG_XMM00) & 0x3];
        } else {
            lpvRegValue = NULL;
        }
		break;

	case CV_REG_MM00:
	case CV_REG_MM01:
	case CV_REG_MM10:
	case CV_REG_MM11:
	case CV_REG_MM20:
	case CV_REG_MM21:
	case CV_REG_MM30:
	case CV_REG_MM31:
	case CV_REG_MM40:
	case CV_REG_MM41:
	case CV_REG_MM50:
	case CV_REG_MM51:
	case CV_REG_MM60:
	case CV_REG_MM61:
	case CV_REG_MM70:
	case CV_REG_MM71:
		if (dwProcessorFlags & PROCESSOR_FLAGS_I386_3DNOW) {
            LPL_3DNOW pReg = &((LPL_3DNOW)(lpregs->FloatSave.RegisterArea))[ (ireg - CV_REG_MM00)>>1 ];
			*((PFLOAT)lpvRegValue) = pReg->Float[ireg&1];
		} else {
			lpvRegValue = NULL;
		}
		break;

    case CV_REG_MXCSR:
        if (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2) {
			*((LPLONG)lpvRegValue ) =  ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->MXCsr;
		} else {
            lpvRegValue = NULL;
        }
        break;

    case CV_REG_CTRL:
        *( (LPLONG) lpvRegValue ) =  lpregs->FloatSave.ControlWord;
        break;

    case CV_REG_STAT:
        *( (LPLONG) lpvRegValue ) =  lpregs->FloatSave.StatusWord;
        break;

    case CV_REG_TAG:
        *( (LPLONG) lpvRegValue ) =  lpregs->FloatSave.TagWord;
        break;

    case CV_REG_FPIP:
        *( (LPWORD) lpvRegValue ) =  (OFF16) lpregs->FloatSave.ErrorOffset;
        break;

    case CV_REG_FPEIP:
        *( (LPLONG) lpvRegValue ) =  lpregs->FloatSave.ErrorOffset;
        break;

    case CV_REG_FPCS:
        *( (LPWORD) lpvRegValue ) =  (SEG16) lpregs->FloatSave.ErrorSelector;
        break;

    case CV_REG_FPDO:
        *( (LPLONG) lpvRegValue ) =  (OFF16) lpregs->FloatSave.DataOffset;
        break;

    case CV_REG_FPEDO:
        *( (LPLONG) lpvRegValue ) =  lpregs->FloatSave.DataOffset;
        break;

    case CV_REG_FPDS:
        *( (LPWORD) lpvRegValue ) =  (SEG16) lpregs->FloatSave.DataSelector;
        break;

#define lpsr ((PKSPECIAL_REGISTERS)lpregs)
    case CV_REG_GDTR:
        *( (LPDWORD) lpvRegValue ) = lpsr->Gdtr.Base;
        break;

    case CV_REG_GDTL:
        *( (LPWORD) lpvRegValue ) = lpsr->Gdtr.Limit;
        break;

    case CV_REG_IDTR:
        *( (LPDWORD) lpvRegValue ) = lpsr->Idtr.Base;
        break;

    case CV_REG_IDTL:
        *( (LPWORD) lpvRegValue ) = lpsr->Idtr.Limit;
        break;

    case CV_REG_LDTR:
        *( (LPWORD) lpvRegValue ) = lpsr->Ldtr;
        break;

    case CV_REG_TR:
        *( (LPWORD) lpvRegValue ) = lpsr->Tr;
        break;

    case CV_REG_CR0:
        *( (LPDWORD) lpvRegValue ) = lpsr->Cr0;
        break;

    case CV_REG_CR2:
        *( (LPDWORD) lpvRegValue ) = lpsr->Cr2;
        break;

    case CV_REG_CR3:
        *( (LPDWORD) lpvRegValue ) = lpsr->Cr3;
        break;

    case CV_REG_CR4:
        *( (LPDWORD) lpvRegValue ) = lpsr->Cr4;
        break;
#undef lpsr

    case CV_REG_DR0:
        *( (PULONG) lpvRegValue ) = lpregs->Dr0;
        break;

    case CV_REG_DR1:
        *( (PULONG) lpvRegValue ) = lpregs->Dr1;
        break;

    case CV_REG_DR2:
        *( (PULONG) lpvRegValue ) = lpregs->Dr2;
        break;

    case CV_REG_DR3:
        *( (PULONG) lpvRegValue ) = lpregs->Dr3;
        break;

    case CV_REG_DR6:
        *( (PULONG) lpvRegValue ) = lpregs->Dr6;
        break;

    case CV_REG_DR7:
        *( (PULONG) lpvRegValue ) = lpregs->Dr7;
        break;

    }

    switch ( ireg ) {

    case CV_REG_AL:
    case CV_REG_CL:
    case CV_REG_DL:
    case CV_REG_BL:
    case CV_REG_AH:
    case CV_REG_CH:
    case CV_REG_DH:
    case CV_REG_BH:

        lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof ( BYTE ) ;
        break;

    case CV_REG_AX:
    case CV_REG_CX:
    case CV_REG_DX:
    case CV_REG_BX:
    case CV_REG_SP:
    case CV_REG_BP:
    case CV_REG_SI:
    case CV_REG_DI:
    case CV_REG_IP:
    case CV_REG_FLAGS:
    case CV_REG_ES:
    case CV_REG_CS:
    case CV_REG_SS:
    case CV_REG_DS:
    case CV_REG_FS:
    case CV_REG_GS:
    case CV_REG_FPCS:
    case CV_REG_FPDS:
    case CV_REG_CTRL:
    case CV_REG_STAT:
    case CV_REG_TAG:
    case CV_REG_FPIP:
    case CV_REG_FPDO:

    case CV_REG_GDTL:
    case CV_REG_IDTL:
    case CV_REG_LDTR:
    case CV_REG_TR:

        lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof ( WORD ) ;

        break;

    case CV_REG_EAX:
    case CV_REG_ECX:
    case CV_REG_EDX:
    case CV_REG_EBX:
    case CV_REG_ESP:
    case CV_REG_EBP:
    case CV_REG_ESI:
    case CV_REG_EDI:
    case CV_REG_EIP:
    case CV_REG_EFLAGS:
    case CV_REG_FPEIP:
    case CV_REG_FPEDO:

    case CV_REG_CR0:
    case CV_REG_CR1:
    case CV_REG_CR2:
    case CV_REG_CR3:
    case CV_REG_CR4:

    case CV_REG_DR0:
    case CV_REG_DR1:
    case CV_REG_DR2:
    case CV_REG_DR3:
    case CV_REG_DR4:
    case CV_REG_DR5:
    case CV_REG_DR6:
    case CV_REG_DR7:

    case CV_REG_GDTR:
    case CV_REG_IDTR:

        lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof ( LONG ) ;
        break;

    case CV_REG_ST0:
    case CV_REG_ST1:
    case CV_REG_ST2:
    case CV_REG_ST3:
    case CV_REG_ST4:
    case CV_REG_ST5:
    case CV_REG_ST6:
    case CV_REG_ST7:

        lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof ( L_DOUBLE ) ;
        break;

	case CV_REG_MM0:
	case CV_REG_MM1:
	case CV_REG_MM2:
	case CV_REG_MM3:
	case CV_REG_MM4:
	case CV_REG_MM5:
	case CV_REG_MM6:
	case CV_REG_MM7:
        lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX)? (LPBYTE) (lpvRegValue) + sizeof( __int64 ) : NULL;
        break;

    case CV_REG_EMM0L:
    case CV_REG_EMM1L:
    case CV_REG_EMM2L:
    case CV_REG_EMM3L:
    case CV_REG_EMM4L:
    case CV_REG_EMM5L:
    case CV_REG_EMM6L:
    case CV_REG_EMM7L:
    case CV_REG_EMM0H:
    case CV_REG_EMM1H:
    case CV_REG_EMM2H:
    case CV_REG_EMM3H:
    case CV_REG_EMM4H:
    case CV_REG_EMM5H:
    case CV_REG_EMM6H:
    case CV_REG_EMM7H:
        lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3)? (LPBYTE) (lpvRegValue) + sizeof( DWORDLONG ) : NULL;
		break;

    case CV_REG_XMM0L:
    case CV_REG_XMM1L:
    case CV_REG_XMM2L:
    case CV_REG_XMM3L:
    case CV_REG_XMM4L:
    case CV_REG_XMM5L:
    case CV_REG_XMM6L:
    case CV_REG_XMM7L:
    case CV_REG_XMM0H:
    case CV_REG_XMM1H:
    case CV_REG_XMM2H:
    case CV_REG_XMM3H:
    case CV_REG_XMM4H:
    case CV_REG_XMM5H:
    case CV_REG_XMM6H:
    case CV_REG_XMM7H:
        lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3)? (LPBYTE) (lpvRegValue) + sizeof( double ) : NULL;
		break;

	case CV_REG_XMM0:
	case CV_REG_XMM1:
	case CV_REG_XMM2:
	case CV_REG_XMM3:
	case CV_REG_XMM4:
	case CV_REG_XMM5:
	case CV_REG_XMM6:
	case CV_REG_XMM7:
		lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2)? (LPBYTE) (lpvRegValue) + sizeof( XMMI128 ) : NULL;
		break;

    case CV_REG_XMM00:
	case CV_REG_XMM01:
	case CV_REG_XMM02:
	case CV_REG_XMM03:
	case CV_REG_XMM10:
	case CV_REG_XMM11:
	case CV_REG_XMM12:
	case CV_REG_XMM13:
    case CV_REG_XMM20:
	case CV_REG_XMM21:
	case CV_REG_XMM22:
	case CV_REG_XMM23:
	case CV_REG_XMM30:
	case CV_REG_XMM31:
	case CV_REG_XMM32:
	case CV_REG_XMM33:
    case CV_REG_XMM40:
	case CV_REG_XMM41:
	case CV_REG_XMM42:
	case CV_REG_XMM43:
	case CV_REG_XMM50:
	case CV_REG_XMM51:
	case CV_REG_XMM52:
	case CV_REG_XMM53:
    case CV_REG_XMM60:
	case CV_REG_XMM61:
	case CV_REG_XMM62:
	case CV_REG_XMM63:
	case CV_REG_XMM70:
	case CV_REG_XMM71:
	case CV_REG_XMM72:
	case CV_REG_XMM73:
		lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2)? (LPBYTE) (lpvRegValue) + sizeof( float ) : NULL;
		break;

	case CV_REG_MXCSR:
		lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2)? (LPBYTE) (lpvRegValue) + sizeof( ULONG ) : NULL;
		break;
    
	case CV_REG_MM00:
	case CV_REG_MM01:
	case CV_REG_MM10:
	case CV_REG_MM11:
	case CV_REG_MM20:
	case CV_REG_MM21:
	case CV_REG_MM30:
	case CV_REG_MM31:
	case CV_REG_MM40:
	case CV_REG_MM41:
	case CV_REG_MM50:
	case CV_REG_MM51:
	case CV_REG_MM60:
	case CV_REG_MM61:
	case CV_REG_MM70:
	case CV_REG_MM71:
		lpvRegValue = (dwProcessorFlags & PROCESSOR_FLAGS_I386_3DNOW)? (LPBYTE) (lpvRegValue) + sizeof( float ) : NULL;
		break;

    default:
        lpvRegValue = NULL;
        break;
    }

    return lpvRegValue;
}                               /* DoGetReg() */


LPVOID
X86DoSetReg(
    LPVOID   lpregs1,
    DWORD    ireg,
    LPVOID   lpvRegValue
    )
/*++

Routine Description:

    This routine is used to set a specific register in a threads
    context

Arguments:

    lpregs      - Supplies pointer to register context for thread
    ireg        - Supplies the index of the register to be modified
    lpvRegValue - Supplies the buffer containning the new data

Return Value:

    return-value - the pointer the the next location where a register
        value could be.

--*/

{
    int         i;
    LPCONTEXT   lpregs = (LPCONTEXT) lpregs1;

    switch ( ireg ) {

    case CV_REG_AL:
        lpregs->Eax = (lpregs->Eax & 0xFFFFFF00) | *( (LPBYTE) lpvRegValue );
        break;

    case CV_REG_CL:
        lpregs->Ecx = (lpregs->Ecx & 0xFFFFFF00) | *( (LPBYTE) lpvRegValue );
        break;

    case CV_REG_DL:
        lpregs->Edx = (lpregs->Edx & 0xFFFFFF00) | *( (LPBYTE) lpvRegValue );
        break;

    case CV_REG_BL:
        lpregs->Ebx = (lpregs->Ebx & 0xFFFFFF00) | *( (LPBYTE) lpvRegValue );
        break;

    case CV_REG_AH:
        lpregs->Eax = (lpregs->Eax & 0xFFFF00FF) |
          (((WORD) *( (LPBYTE) lpvRegValue )) << 8);
        break;

    case CV_REG_CH:
        lpregs->Ecx = (lpregs->Ecx & 0xFFFF00FF) |
          (((WORD) *( (LPBYTE) lpvRegValue )) << 8);
        break;

    case CV_REG_DH:
        lpregs->Edx = (lpregs->Edx & 0xFFFF00FF) |
          (((WORD) *( (LPBYTE) lpvRegValue )) << 8);
        break;

    case CV_REG_BH:
        lpregs->Ebx = (lpregs->Ebx & 0xFFFF00FF) |
          (((WORD) *( (LPBYTE) lpvRegValue )) << 8);
        break;

    case CV_REG_AX:
        lpregs->Eax = (lpregs->Eax & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_CX:
        lpregs->Ecx = (lpregs->Ecx & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_DX:
        lpregs->Edx = (lpregs->Edx & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_BX:
        lpregs->Ebx = (lpregs->Ebx & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_SP:
        lpregs->Esp = (lpregs->Esp & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_BP:
        lpregs->Ebp = (lpregs->Ebp & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_SI:
        lpregs->Esi = (lpregs->Esi & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_DI:
        lpregs->Edi = (lpregs->Edi & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_IP:
        lpregs->Eip = (lpregs->Eip & 0xFFFF0000) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FLAGS:
        lpregs->EFlags = (lpregs->EFlags & 0xFFFF0000 ) | *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_ES:
        lpregs->SegEs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_CS:
        lpregs->SegCs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_SS:
        lpregs->SegSs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_DS:
        lpregs->SegDs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FS:
        lpregs->SegFs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_GS:
        lpregs->SegGs = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_EAX:
        lpregs->Eax = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_ECX:
        lpregs->Ecx = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EDX:
        lpregs->Edx = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EBX:
        lpregs->Ebx = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_ESP:
        lpregs->Esp = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EBP:
        lpregs->Ebp = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_ESI:
        lpregs->Esi = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EDI:
        lpregs->Edi = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EIP:
        lpregs->Eip = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_EFLAGS:
        lpregs->EFlags = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_ST0:
    case CV_REG_ST1:
    case CV_REG_ST2:
    case CV_REG_ST3:
    case CV_REG_ST4:
    case CV_REG_ST5:
    case CV_REG_ST6:
    case CV_REG_ST7:
//        i = (lpregs->FloatSave.StatusWord >> 11) & 0x7;
//        i = (i + ireg - CV_REG_ST0) % 8;
        i = ireg - CV_REG_ST0;
        memcpy(&lpregs->FloatSave.RegisterArea[10*(i)], lpvRegValue, 10);
        break;

	case CV_REG_MM0:
	case CV_REG_MM1:
	case CV_REG_MM2:
	case CV_REG_MM3:
	case CV_REG_MM4:
	case CV_REG_MM5:
	case CV_REG_MM6:
	case CV_REG_MM7:
		{
		LPL_MMX pReg = &((LPL_MMX)(lpregs->FloatSave.RegisterArea))[ ireg - CV_REG_MM0 ];
		pReg->MMValue = *(__int64*)lpvRegValue;
		// emulate an MMX instruction, which means wack all 1s into the exponent
		// set TOS to 0, and zero all the tags too
		pReg->MMExponent = 0xFFFF;
		lpregs->FloatSave.TagWord &= ~0xFFFF;
		lpregs->FloatSave.StatusWord &= ~0x3800;

#if 0 //v-vadimp - not clear if this is required
        //
		// v-vadimp
		//
		// in case this is a Willamette machine do a similar thing for the corresponding EMM register (additionally requires the high 48 bits set to zero)
		// there is no need to check for architecture as setting the extended context area just won't do anything on older machines
		//
        struct _L_EMMX {
	        __int64 MMValue;
            union {
	            WORD MMExponent;
                __int64 HiBits;
            };
        } pRegEx = (LPL_EMMX)&(((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_MM0]);
		pRegEx->MMValue = *(__int64*)lpvRegValue;
        
		pRegEx->HiBits = 0; // do not change the order of assignments since HiBits and MMExponent interleave!!!
		pRegEx->MMExponent = 0xFFFF; 
		
		((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->TagWord &= ~0xFFFF;
		((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->StatusWord &= ~0x3800;
#endif

		}
		break;

    case CV_REG_EMM0L:
    case CV_REG_EMM1L:
    case CV_REG_EMM2L:
    case CV_REG_EMM3L:
    case CV_REG_EMM4L:
    case CV_REG_EMM5L:
    case CV_REG_EMM6L:
    case CV_REG_EMM7L:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_EMM0L].u.dl[0] = *((PDWORDLONG)lpvRegValue);
        ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->TagWord &= ~0xFFFF;
        ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->StatusWord &= ~0x3800;
        break;

    case CV_REG_EMM0H:
    case CV_REG_EMM1H:
    case CV_REG_EMM2H:
    case CV_REG_EMM3H:
    case CV_REG_EMM4H:
    case CV_REG_EMM5H:
    case CV_REG_EMM6H:
    case CV_REG_EMM7H:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_EMM0H].u.dl[1] = *((PDWORDLONG)lpvRegValue);
        ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->TagWord &= ~0xFFFF;
        ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->StatusWord &= ~0x3800;
        break;

    case CV_REG_XMM0L:
    case CV_REG_XMM1L:
    case CV_REG_XMM2L:
    case CV_REG_XMM3L:
    case CV_REG_XMM4L:
    case CV_REG_XMM5L:
    case CV_REG_XMM6L:
    case CV_REG_XMM7L:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0L].u.fd[0] = *((double *)lpvRegValue);
        break;

    case CV_REG_XMM0H:
    case CV_REG_XMM1H:
    case CV_REG_XMM2H:
    case CV_REG_XMM3H:
    case CV_REG_XMM4H:
    case CV_REG_XMM5H:
    case CV_REG_XMM6H:
    case CV_REG_XMM7H:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0H].u.fd[1] = *((double *)lpvRegValue);
        break;

    case CV_REG_XMM0:
    case CV_REG_XMM1:
    case CV_REG_XMM2:
    case CV_REG_XMM3:
    case CV_REG_XMM4:
    case CV_REG_XMM5:
    case CV_REG_XMM6:
    case CV_REG_XMM7:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[ireg - CV_REG_XMM0] = *((PXMMI128)lpvRegValue);
        break;

    case CV_REG_XMM00:
    case CV_REG_XMM01:
    case CV_REG_XMM02:
    case CV_REG_XMM03:
    case CV_REG_XMM10:
    case CV_REG_XMM11:
    case CV_REG_XMM12:
    case CV_REG_XMM13:
    case CV_REG_XMM20:
    case CV_REG_XMM21:
    case CV_REG_XMM22:
    case CV_REG_XMM23:
    case CV_REG_XMM30:
    case CV_REG_XMM31:
    case CV_REG_XMM32:
    case CV_REG_XMM33:
    case CV_REG_XMM40:
    case CV_REG_XMM41:
    case CV_REG_XMM42:
    case CV_REG_XMM43:
    case CV_REG_XMM50:
    case CV_REG_XMM51:
    case CV_REG_XMM52:
    case CV_REG_XMM53:
    case CV_REG_XMM60:
    case CV_REG_XMM61:
    case CV_REG_XMM62:
    case CV_REG_XMM63:
    case CV_REG_XMM70:
    case CV_REG_XMM71:
    case CV_REG_XMM72:
    case CV_REG_XMM73:
        ((PXMMI_AREA)(((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->Reserved3))->Xmmi[(ireg - CV_REG_XMM00) >> 2].u.fs[(ireg - CV_REG_XMM00) & 0x3] = *((PFLOAT)lpvRegValue);
        break;
    
    case CV_REG_MXCSR:
        ((PFLOATING_EXTENDED_SAVE_AREA)(lpregs->ExtendedRegisters))->MXCsr = *((LPLONG)lpvRegValue );
        break;

	case CV_REG_MM00:
	case CV_REG_MM01:
	case CV_REG_MM10:
	case CV_REG_MM11:
	case CV_REG_MM20:
	case CV_REG_MM21:
	case CV_REG_MM30:
	case CV_REG_MM31:
	case CV_REG_MM40:
	case CV_REG_MM41:
	case CV_REG_MM50:
	case CV_REG_MM51:
	case CV_REG_MM60:
	case CV_REG_MM61:
	case CV_REG_MM70:
	case CV_REG_MM71:
		{
		LPL_3DNOW pReg = &((LPL_3DNOW)(lpregs->FloatSave.RegisterArea))[ (ireg - CV_REG_MM00)>>1 ];
		pReg->Float[ireg&1] = *(float*)lpvRegValue;
		}
		break;

    case CV_REG_CTRL:
        lpregs->FloatSave.ControlWord = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_STAT:
        lpregs->FloatSave.StatusWord = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_TAG:
        lpregs->FloatSave.TagWord = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FPIP:
        lpregs->FloatSave.ErrorOffset = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FPEIP:
        lpregs->FloatSave.ErrorOffset = *( (LPLONG) lpvRegValue );
        break;

    case CV_REG_FPCS:
        lpregs->FloatSave.ErrorSelector = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FPDO:
        lpregs->FloatSave.DataOffset = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FPEDO:
        lpregs->FloatSave.DataOffset = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_FPDS:
        lpregs->FloatSave.DataSelector = *( (LPWORD) lpvRegValue );
        break;

#define lpsr ((PKSPECIAL_REGISTERS)lpregs)
    case CV_REG_GDTR:
        lpsr->Gdtr.Base = *( (LPDWORD) lpvRegValue );
        break;

    case CV_REG_GDTL:
        lpsr->Gdtr.Limit = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_IDTR:
        lpsr->Idtr.Base = *( (LPDWORD) lpvRegValue );
        break;

    case CV_REG_IDTL:
        lpsr->Idtr.Limit = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_LDTR:
        lpsr->Ldtr = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_TR:
        lpsr->Tr = *( (LPWORD) lpvRegValue );
        break;

    case CV_REG_CR0:
        lpsr->Cr0 = *( (LPDWORD) lpvRegValue );
        break;

    case CV_REG_CR2:
        lpsr->Cr2 = *( (LPDWORD) lpvRegValue );
        break;

    case CV_REG_CR3:
        lpsr->Cr3 = *( (LPDWORD) lpvRegValue );
        break;

    case CV_REG_CR4:
        lpsr->Cr4 = *( (LPDWORD) lpvRegValue );
        break;
#undef lpsr

    case CV_REG_DR0:
        lpregs->Dr0 = *( (PULONG) lpvRegValue );
        break;

    case CV_REG_DR1:
        lpregs->Dr1 = *( (PULONG) lpvRegValue );
        break;

    case CV_REG_DR2:
        lpregs->Dr2 = *( (PULONG) lpvRegValue );
        break;

    case CV_REG_DR3:
        lpregs->Dr3 = *( (PULONG) lpvRegValue );
        break;

    case CV_REG_DR6:
        lpregs->Dr6 = *( (PULONG) lpvRegValue );
        break;

    case CV_REG_DR7:
        lpregs->Dr7 = *( (PULONG) lpvRegValue );

    }


    switch ( ireg ) {

    case CV_REG_AL:
    case CV_REG_CL:
    case CV_REG_DL:
    case CV_REG_BL:
    case CV_REG_AH:
    case CV_REG_CH:
    case CV_REG_DH:
    case CV_REG_BH:

        lpvRegValue = (LPBYTE) lpvRegValue + sizeof ( BYTE );
        break;

    case CV_REG_AX:
    case CV_REG_CX:
    case CV_REG_DX:
    case CV_REG_BX:
    case CV_REG_SP:
    case CV_REG_BP:
    case CV_REG_SI:
    case CV_REG_DI:
    case CV_REG_IP:
    case CV_REG_FLAGS:
    case CV_REG_ES:
    case CV_REG_CS:
    case CV_REG_SS:
    case CV_REG_DS:
    case CV_REG_FS:
    case CV_REG_GS:
    case CV_REG_CTRL:
    case CV_REG_STAT:
    case CV_REG_TAG:
    case CV_REG_FPIP:
    case CV_REG_FPCS:
    case CV_REG_FPDO:
    case CV_REG_FPDS:
    case CV_REG_GDTL:
    case CV_REG_IDTL:
    case CV_REG_LDTR:
    case CV_REG_TR:

        lpvRegValue = (LPBYTE) lpvRegValue + sizeof ( WORD );
        break;

    case CV_REG_EAX:
    case CV_REG_ECX:
    case CV_REG_EDX:
    case CV_REG_EBX:
    case CV_REG_ESP:
    case CV_REG_EBP:
    case CV_REG_ESI:
    case CV_REG_EDI:
    case CV_REG_EIP:
    case CV_REG_EFLAGS:
    case CV_REG_FPEIP:
    case CV_REG_FPEDO:
    case CV_REG_CR0:
    case CV_REG_CR1:
    case CV_REG_CR2:
    case CV_REG_CR3:
    case CV_REG_CR4:
    case CV_REG_DR0:
    case CV_REG_DR1:
    case CV_REG_DR2:
    case CV_REG_DR3:
    case CV_REG_DR4:
    case CV_REG_DR5:
    case CV_REG_DR6:
    case CV_REG_DR7:
    case CV_REG_GDTR:
    case CV_REG_IDTR:

        lpvRegValue = (LPBYTE) lpvRegValue + sizeof ( LONG );
        break;

    case CV_REG_ST0:
    case CV_REG_ST1:
    case CV_REG_ST2:
    case CV_REG_ST3:
    case CV_REG_ST4:
    case CV_REG_ST5:
    case CV_REG_ST6:
    case CV_REG_ST7:

        lpvRegValue = (LPBYTE) lpvRegValue + sizeof ( L_DOUBLE );
        break;

	case CV_REG_MM0:
	case CV_REG_MM1:
	case CV_REG_MM2:
	case CV_REG_MM3:
	case CV_REG_MM4:
	case CV_REG_MM5:
	case CV_REG_MM6:
	case CV_REG_MM7:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( __int64 );
		break;

	case CV_REG_EMM0L:
	case CV_REG_EMM1L:
	case CV_REG_EMM2L:
	case CV_REG_EMM3L:
	case CV_REG_EMM4L:
	case CV_REG_EMM5L:
	case CV_REG_EMM6L:
	case CV_REG_EMM7L:
	case CV_REG_EMM0H:
	case CV_REG_EMM1H:
	case CV_REG_EMM2H:
	case CV_REG_EMM3H:
	case CV_REG_EMM4H:
	case CV_REG_EMM5H:
	case CV_REG_EMM6H:
	case CV_REG_EMM7H:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( DWORDLONG );
		break;

    case CV_REG_XMM0L:
	case CV_REG_XMM1L:
	case CV_REG_XMM2L:
	case CV_REG_XMM3L:
	case CV_REG_XMM4L:
	case CV_REG_XMM5L:
	case CV_REG_XMM6L:
	case CV_REG_XMM7L:
	case CV_REG_XMM0H:
	case CV_REG_XMM1H:
	case CV_REG_XMM2H:
	case CV_REG_XMM3H:
	case CV_REG_XMM4H:
	case CV_REG_XMM5H:
	case CV_REG_XMM6H:
	case CV_REG_XMM7H:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( double );
		break;

	case CV_REG_XMM0:
	case CV_REG_XMM1:
	case CV_REG_XMM2:
	case CV_REG_XMM3:
	case CV_REG_XMM4:
	case CV_REG_XMM5:
	case CV_REG_XMM6:
	case CV_REG_XMM7:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( XMMI128 );
		break;

	case CV_REG_XMM00:
	case CV_REG_XMM01:
	case CV_REG_XMM02:
	case CV_REG_XMM03:
	case CV_REG_XMM10:
	case CV_REG_XMM11:
	case CV_REG_XMM12:
	case CV_REG_XMM13:
	case CV_REG_XMM20:
	case CV_REG_XMM21:
	case CV_REG_XMM22:
	case CV_REG_XMM23:
	case CV_REG_XMM30:
	case CV_REG_XMM31:
	case CV_REG_XMM32:
	case CV_REG_XMM33:
	case CV_REG_XMM40:
	case CV_REG_XMM41:
	case CV_REG_XMM42:
	case CV_REG_XMM43:
	case CV_REG_XMM50:
	case CV_REG_XMM51:
	case CV_REG_XMM52:
	case CV_REG_XMM53:
	case CV_REG_XMM60:
	case CV_REG_XMM61:
	case CV_REG_XMM62:
	case CV_REG_XMM63:
	case CV_REG_XMM70:
	case CV_REG_XMM71:
	case CV_REG_XMM72:
	case CV_REG_XMM73:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( float );
		break;

	case CV_REG_MXCSR:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( DWORD );
		break;

	case CV_REG_MM00:
	case CV_REG_MM01:
	case CV_REG_MM10:
	case CV_REG_MM11:
	case CV_REG_MM20:
	case CV_REG_MM21:
	case CV_REG_MM30:
	case CV_REG_MM31:
	case CV_REG_MM40:
	case CV_REG_MM41:
	case CV_REG_MM50:
	case CV_REG_MM51:
	case CV_REG_MM60:
	case CV_REG_MM61:
	case CV_REG_MM70:
	case CV_REG_MM71:
		lpvRegValue = (LPBYTE) (lpvRegValue) + sizeof( float );
		break;

    default:

        lpvRegValue = NULL;
        break;
    }

    return lpvRegValue;
}                               /* DoSetReg() */



LPVOID
X86DoSetFrameReg(
    HPID hpid,
    HTID htid,
    LPTHD lpthd,
    PKNONVOLATILE_CONTEXT_POINTERS contextPtrs,
    DWORD ireg,
    LPVOID lpvRegValue
    )
{
    return X86DoSetReg( (LPCONTEXT) lpthd->regs, ireg, lpvRegValue);
}


XOSD
X86DoGetFrame(
    HPID hpid,
    HTID uhtid,
    DWORD wValue,
    DWORD lValue
    )
{
    HPRC hprc = ValidHprcFromHpid(hpid);
    HTHD hthd;
    LPTHD lpthd;
    HTID htid = (HTID)((DWORD)uhtid & ~1);
    HTID vhtid = (HTID)((DWORD)uhtid | 1);
    DWORD i;
    XOSD xosd;
    BOOL fGoodFrame;
    BOOL fReturnHtid = FALSE;
    ADDRESS_MODE  mode;
    LPPRC lpprc;

    if (!hprc) {
        return xosdBadProcess;
    }

    lpprc = (LPPRC)LLLock(hprc);

    if (wValue == (DWORD)-1) {
        wValue = 1;
        fReturnHtid = TRUE;
    }

    hthd = HthdFromHtid(hprc, htid);
    if (hthd == NULL) {
        return xosdBadThread;
    }

    lpthd = (LPTHD) LLLock( hthd );
    assert(lpthd != NULL);



    if ( uhtid == htid ) {

        //
        // first frame -
        // Get regs and clear STACKFRAME struct
        //

        if (lpthd->drt & (drtCntrlDirty|drtAllDirty)) {
            SendRequestX(dmfWriteReg,hpid,htid,sizeof(CONTEXT),lpthd->regs);
            lpthd->drt = (DRT) (lpthd->drt & ( ~(drtCntrlDirty | drtAllDirty) ) );
        }
        UpdateRegisters( hprc, hthd );

        ZeroMemory( &lpthd->StackFrame, sizeof(STACKFRAME) );
        memcpy (lpthd->frameRegs, lpthd->regs, sizeof(CONTEXT) );
        lpthd->frameNumber = 0;

        //
        // set the addressing mode
        //
        if (lpthd->fFlat || lpprc->dmi.fAlwaysFlat ) {
            mode = AddrModeFlat;
        } else
        if (lpthd->fReal) {
            mode = AddrModeReal;
        } else
        if (lpthd->fOff32) {
            mode = AddrMode1632;
        } else {
            mode = AddrMode1616;
        }

        //
        // setup the program counter
        //
        lpthd->StackFrame.AddrPC.Offset       = ((PCONTEXT) (lpthd->regs))->Eip;
        lpthd->StackFrame.AddrPC.Segment      = (WORD)((PCONTEXT) (lpthd->regs))->SegCs;
        lpthd->StackFrame.AddrPC.Mode         = mode;

        //
        // setup the frame pointer
        //
        lpthd->StackFrame.AddrFrame.Offset    = ((PCONTEXT) (lpthd->regs))->Ebp;
        lpthd->StackFrame.AddrFrame.Segment   = (WORD)((PCONTEXT) (lpthd->regs))->SegSs;
        lpthd->StackFrame.AddrFrame.Mode         = mode;

        //
        // setup the stack pointer
        //
        lpthd->StackFrame.AddrStack.Offset    = ((PCONTEXT) (lpthd->regs))->Esp;
        lpthd->StackFrame.AddrStack.Segment   = (WORD)((PCONTEXT) (lpthd->regs))->SegSs;
        lpthd->StackFrame.AddrStack.Mode         = mode;
    }

    fGoodFrame = FALSE;
    xosd = xosdNone;
    for (i = 0; xosd == xosdNone && ((wValue != 0)? (i < wValue) : 1); i++) {

        if (StackWalk( IMAGE_FILE_MACHINE_I386,
                       hpid,
                       htid,
                       &lpthd->StackFrame,
                       lpthd->frameRegs,
                       SwReadMemory,
                       X86SwFunctionTableAccess,
                       SwGetModuleBase,
                       SwTranslateAddress
                       ))
        {
            xosd = xosdNone;

			if (lpthd->frameNumber == 0) {
				lpthd->offBetterTopFrame = lpthd->StackFrame.AddrFrame.Offset;
			}
				
            lpthd->frameNumber++;
            fGoodFrame = TRUE;
        } else {
            xosd = xosdEndOfStack;
        }
    }

    if (fGoodFrame) {
        *(LPHTID)lValue = fReturnHtid? htid : vhtid;
    }

    LLUnlock( hthd );
    LLUnlock( hprc );

    return xosd;
}



PFPO_DATA
SwSearchFpoData(
    DWORD     key,
    PFPO_DATA base,
    DWORD     num
    )
{
    PFPO_DATA  lo = base;
    PFPO_DATA  hi = base + (num - 1);
    PFPO_DATA  mid;
    DWORD      half;

    while (lo <= hi) {
        if (half = num / 2) {
            mid = lo + (num & 1 ? half : (half - 1));
            if ((key >= mid->ulOffStart)&&
                       (key < (mid->ulOffStart+mid->cbProcSize))) {
                return mid;
            }
            if (key < mid->ulOffStart) {
                hi = mid - 1;
                num = num & 1 ? half : half-1;
            }
            else {
                lo = mid + 1;
                num = half;
            }
        }
        else if (num) {
            if ((key >= lo->ulOffStart)&&
                                     (key < (lo->ulOffStart+lo->cbProcSize))) {
                return lo;
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
    return(NULL);
}

LPVOID
X86SwFunctionTableAccess(
    LPVOID  lpvhpid,
    DWORD   AddrBase
    )
{
    HMDI        hmdi  = 0;
    LPMDI       lpmdi = 0;
    DWORD       off;
    HPID        hpid = (HPID)lpvhpid;
    PFPO_DATA   pFpo = NULL;

    hmdi = SwGetMdi( hpid, AddrBase );

    if (hmdi) {

        lpmdi = (LPMDI) LLLock( hmdi );

        if (lpmdi) {
            if (lpmdi->lpDebug) {

                off = ConvertOmapToSrc( lpmdi, AddrBase );

                if (off) {
                    AddrBase = off;
                }

                pFpo = SwSearchFpoData( AddrBase-lpmdi->lpBaseOfDll,
                                        (PFPO_DATA) lpmdi->lpDebug->lpRtf,
                                        lpmdi->lpDebug->cRtf
                                      );
            }
            LLUnlock( hmdi );
        }
    }

    return (LPVOID)pFpo;
}

XOSD
X86DoGetFunctionInfo(
    HPID hpid,
    LPGFI lpgfi
    )
{
    PFPO_DATA pFpo;

    assert(!ADDR_IS_LI(*lpgfi->lpaddr));
    assert(ADDR_IS_FLAT(*lpgfi->lpaddr));

    pFpo = (PFPO_DATA)X86SwFunctionTableAccess((LPVOID)hpid, GetAddrOff(*(lpgfi->lpaddr)));

    if (pFpo) {
        AddrInit(&lpgfi->lpFunctionInformation->AddrStart,
                 0, (SEGMENT) 0, pFpo->ulOffStart,
                 TRUE, TRUE, FALSE, FALSE);
        AddrInit(&lpgfi->lpFunctionInformation->AddrEnd,
                 0, (SEGMENT) 0, pFpo->ulOffStart + pFpo->cbProcSize,
                 TRUE, TRUE, FALSE, FALSE);
        AddrInit(&lpgfi->lpFunctionInformation->AddrPrologEnd,
                 0, (SEGMENT) 0, pFpo->ulOffStart + pFpo->cbProlog,
                 TRUE, TRUE, FALSE, FALSE);
        return xosdNone;
    } else {
        return xosdUnknown;
    }
}


DWORD
SwTranslateAddress(
    LPVOID    lpvhpid,
    LPVOID    lpvhtid,
    LPADDRESS lpaddress
    )
{
    XOSD               xosd;

        const DWORD        dwPigLen = sizeof(ADDR) + 4;
        const DWORD        dwTotalLen = sizeof(SSS) + sizeof(IOCTLGENERIC) + dwPigLen;

    ADDR               addr;
    BYTE               buf[dwTotalLen];
    HPID               hpid = (HPID)lpvhpid;
    HTID               htid = (HTID)lpvhtid;
    LPSSS              lpsss = (LPSSS)buf;
    PIOCTLGENERIC      pig   = (PIOCTLGENERIC)lpsss->rgbData;


    ZeroMemory( &addr, sizeof(addr) );
    addr.addr.off     = lpaddress->Offset;
    addr.addr.seg     = lpaddress->Segment;
    addr.mode.fFlat   = lpaddress->Mode == AddrModeFlat;
    addr.mode.fOff32  = lpaddress->Mode == AddrMode1632;
    addr.mode.fReal   = lpaddress->Mode == AddrModeReal;

    memcpy( pig->data, &addr, sizeof(addr) );

    lpsss->ssvc           = (SSVC)ssvcGeneric;
        lpsss->cbSend             = sizeof(IOCTLGENERIC) + dwPigLen;
    pig->length           = dwPigLen;
    pig->ioctlSubType     = IG_TRANSLATE_ADDRESS;
    xosd = SystemService(hpid, htid, dwTotalLen, lpsss);

    if (xosd == xosdNone) {
        addr = *((LPADDR)pig->data);

        lpaddress->Offset   = addr.addr.off;
        lpaddress->Segment  = (WORD)addr.addr.seg;

        if (addr.mode.fFlat) {
            lpaddress->Mode = AddrModeFlat;
        } else
        if (addr.mode.fOff32) {
            lpaddress->Mode = AddrMode1632;
        } else
        if (addr.mode.fReal) {
            lpaddress->Mode = AddrModeReal;
        }

        return TRUE;
    }

    return FALSE;
}


XOSD
X86DoGetFrameEH(
    HPID hpid,
    HTID htid,
    LPEXHDLR *lpexhdlr,
    LPDWORD cAddrsAllocated
)
/*++

Routine Description:

    Fill lpexhdlr with the addresses of all exception handlers for this frame

Arguments

    hpid            - current hpid

    htid            - current htid (may be virtual)

    lpexhdlr        - where to store address

    cAddrsAllocated - how many addresses are currently allocated

Return Value:

    xosd status

--*/
{
    return(xosdUnsupported);
}




//////////////////////////////////////////////////////////////////
//  part 2.  Additional target dependent
//////////////////////////////////////////////////////////////////

XOSD
X86UpdateChild (
    HPID hpid,
    HTID htid,
    DMF dmfCommand
    )
/*++

Routine Description:

    This function is used to cause registers to be written back to
    the child as necessary before the child is executed.

Arguments:

    hprc        - Supplies a process handle

    hthdExec    - Supplies the handle of the thread to update

    dmfCommand  - Supplies the command about to be executed.

Return Value:

    XOSD error code

--*/

{
    HPRC  hprc;
    HTHD  hthd;
    HTHD  hthdExec;
    XOSD  xosd  = xosdNone;
    HLLI  llthd;
    LPPRC lpprc;
    PST pst;
    HLLI    hllmdi;
    HMDI    hmdi;

    hprc = HprcFromHpid(hpid);
    hthdExec = HthdFromHtid(hprc, htid);

    llthd = LlthdFromHprc ( hprc );

    xosd = ProcessStatus(hpid, &pst);

    if (xosd != xosdNone) {
        return xosd;
    }

    if (pst.dwProcessState == pstDead || pst.dwProcessState == pstExited) {
        return xosdBadProcess;
    }

    else if (pst.dwProcessState != pstRunning)

    {
        for ( hthd = LLNext ( llthd, hthdNull );
              hthd != hthdNull;
              hthd = LLNext ( llthd, hthd ) ) {

            LPTHD lpthd = (LPTHD) LLLock ( hthd );

            if ( lpthd->drt & (drtCntrlDirty | drtAllDirty) ) {
                assert(lpthd->drt & drtAllPresent);
                SendRequestX (dmfWriteReg,
                              hpid,
                              lpthd->htid,
                              sizeof ( CONTEXT ),
                              lpthd->regs
                             );

                lpthd->drt = (DRT) (lpthd->drt & ~(drtCntrlDirty | drtAllDirty) ) ;
            }
            if ( lpthd->drt & drtSpecialDirty ) {
                assert(lpthd->drt & drtSpecialPresent);
                if (lpthd->dwcbSpecial) {

                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr0 =
                                                 ((PCONTEXT) (lpthd->regs))->Dr0;
                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr1 =
                                                 ((PCONTEXT)  (lpthd->regs))->Dr1;
                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr2 =
                                                 ((PCONTEXT) (lpthd->regs))->Dr2;
                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr3 =
                                                 ((PCONTEXT) (lpthd->regs))->Dr3;
                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr6 =
                                                 ((PCONTEXT) (lpthd->regs))->Dr6;
                    ((PKSPECIAL_REGISTERS)(lpthd->pvSpecial))->KernelDr7 =
                                                 ((PCONTEXT) (lpthd->regs))->Dr7;

                    SendRequestX(dmfWriteRegEx,
                                 hpid,
                                 lpthd->htid,
                                 lpthd->dwcbSpecial,
                                 lpthd->pvSpecial
                                );
                } else {

                    DWORD DR[6];
                    DR[0] = ((PCONTEXT) (lpthd->regs) )->Dr0;
                    DR[1] = ((PCONTEXT) (lpthd->regs) )->Dr1;
                    DR[2] = ((PCONTEXT) (lpthd->regs) )->Dr2;
                    DR[3] = ((PCONTEXT) (lpthd->regs) )->Dr3;
                    DR[4] = ((PCONTEXT) (lpthd->regs) )->Dr6;
                    DR[5] = ((PCONTEXT) (lpthd->regs) )->Dr7;
                    SendRequestX (dmfWriteRegEx,
                                  hpid,
                                  lpthd->htid,
                                  sizeof ( DR ),
                                  DR
                                 );
                }
                lpthd->drt = (DRT) (lpthd->drt & ~drtSpecialDirty);
            }

            lpthd->fRunning = TRUE;

            LLUnlock ( hthd );

            if ( xosd != xosdNone ) {
                break;
            }
        }
        lpprc = (LPPRC) LLLock(hprc);
        lpprc->fRunning = TRUE;
        hllmdi = lpprc->llmdi;

        while ( hmdi = LLFind ( hllmdi, NULL, NULL, emdiNLG ) ) {
            LPMDI   lpmdi = (LPMDI) LLLock ( hmdi );
            NLG     nlg = lpmdi->nlg;

            FixupAddr ( hpid, htid, &nlg.addrNLGDispatch );
            FixupAddr ( hpid, htid, &nlg.addrNLGDestination );
            FixupAddr ( hpid, htid, &nlg.addrNLGReturn );

//          SwapNlg ( &nlg );

            xosd = SendRequestX(dmfNonLocalGoto,
                                HpidFromHprc ( hprc ),
                                NULL,
                                sizeof ( nlg ),
                                &nlg
                                );

            lpmdi->fSendNLG = FALSE;
            LLUnlock ( hmdi );
        }

        LLUnlock(hprc);
    }

    return xosd;
}




XOSD
X86GetRegValue (
    HPID hpid,
    HTID htid,
    DWORD ireg,
    LPVOID lpvRegValue
    )
{
    HPRC        hprc;
    HTHD        hthd;
    LPTHD       lpthd;
    LPPRC       lpprc;
    LPCONTEXT   lpregs;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }

    if ( (DWORD)htid & 1 ) {
        return GetFrameRegValue(hpid, (HTID)((DWORD)htid & ~1), ireg, lpvRegValue);
    }

    hthd = HthdFromHtid(hprc, htid);
    assert ( hthd != hthdNull );

    lpthd = (LPTHD) LLLock ( hthd );
    lpprc = (LPPRC) LLLock( hprc );

    lpregs = (LPCONTEXT) (lpthd->regs);

    if (lpthd->fRunning) {
        UpdateRegisters( lpthd->hprc, hthd );
        X86UpdateSpecialRegisters( lpthd->hprc, hthd );

        if (lpprc->dmi.fAlwaysFlat || ( ((LPCONTEXT) (lpthd->regs))->SegCs == lpprc->selFlatCs) ) {
            lpthd->fFlat = lpthd->fOff32 = TRUE;
            lpthd->fReal = FALSE;
        } else {
            /*
             *  BUGBUG -- jimsch -- some one might eventually catch on
             *  that this is incorrect.  We are not checking to see if the
             *  current address is really a 16-bit WOW address but assuming
             *  that it is.  This will be a problem for people who are doing
             *  real 16:32 programming (on WOW) and people who are doing
             *  real mode program -- but so what
             */
            lpthd->fFlat = lpthd->fOff32 = lpthd->fReal = FALSE;
        }
    } else {

        if ( !(lpthd->drt & drtAllPresent) ) {

            switch ( ireg ) {


            case CV_REG_CS:
            case CV_REG_IP:
            case CV_REG_SS:
            case CV_REG_BP:

            case CV_REG_EIP:
            case CV_REG_EBP:
                if (!(lpthd->drt & drtCntrlPresent)) {
                    UpdateRegisters( lpthd->hprc, hthd );
                }
                break;

            default:

                UpdateRegisters ( lpthd->hprc, hthd );
                break;
            }
        }


        if ( !(lpthd->drt & drtSpecialPresent) ) {

            switch ( ireg ) {

            case CV_REG_GDTR:
            case CV_REG_GDTL:
            case CV_REG_IDTR:
            case CV_REG_IDTL:
            case CV_REG_LDTR:
            case CV_REG_TR:

            case CV_REG_CR0:
            case CV_REG_CR1:
            case CV_REG_CR2:
            case CV_REG_CR3:
            case CV_REG_CR4:

            case CV_REG_DR0:
            case CV_REG_DR1:
            case CV_REG_DR2:
            case CV_REG_DR3:
            case CV_REG_DR4:
            case CV_REG_DR5:
            case CV_REG_DR6:
            case CV_REG_DR7:

                X86UpdateSpecialRegisters( lpthd->hprc, hthd );
                break;

            default:
                break;
            }
        }
    }

    switch ( ireg ) {

        case CV_REG_GDTR:
        case CV_REG_GDTL:
        case CV_REG_IDTR:
        case CV_REG_IDTL:
        case CV_REG_LDTR:
        case CV_REG_TR:

        case CV_REG_CR0:
        case CV_REG_CR1:
        case CV_REG_CR2:
        case CV_REG_CR3:
        case CV_REG_CR4:

            lpregs = (LPCONTEXT) lpthd->pvSpecial;
            break;

        default:
            break;
    }


    LLUnlock( hprc );
    LLUnlock( hthd );

	if ( ireg==CV_ALLREG_TEB )
	{
		// get @TEB pseudo-register
		*( (LPLONG)lpvRegValue ) = lpthd->uoffTEB;
	}
	else if ( ireg==CV_ALLREG_ERR || ireg==CV_ALLREG_TIMER )
	{
		// get @ERR psuedo-register, have to ask the DM
		// get @CLK psuedo-register, have to ask the DM
        GuardTL();
		XOSD xosd = SendRequestX( dmfGetSpecialReg, hpid, htid, sizeof(ireg),
                             &ireg);

        if (xosd==xosdNone)
            *( (LPLONG) lpvRegValue ) = *(DWORD*)LpDmMsg->rgb;
        ReleaseTL();
        if(xosd != xosdNone)
            return xosd;
	}
	else
	{
		*(PDWORD)lpvRegValue = lpprc->dmi.Processor.Flags;
		lpvRegValue = X86DoGetReg ( lpregs, ireg & 0xff, lpvRegValue );
	}

    if ( lpvRegValue == NULL ) {
        LLUnlock ( hthd );
        return xosdInvalidParameter;
    }
    ireg = ireg >> 8;

    if ( ireg != CV_REG_NONE ) {
        *(PDWORD)lpvRegValue = lpprc->dmi.Processor.Flags;
        lpvRegValue = X86DoGetReg ( lpregs, ireg, lpvRegValue );
        if ( lpvRegValue == NULL ) {
            return xosdInvalidParameter;
        }
    }

    return xosdNone;

}                        /* GetRegValue */





XOSD
X86SetRegValue (
    HPID hpid,
    HTID htid,
    DWORD ireg,
    LPVOID lpvRegValue
    )
{
    XOSD        xosd = xosdNone;
    HPRC        hprc;
    HTHD        hthd;
    LPTHD       lpthd;
    LPVOID      lpregs = NULL;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }
    hthd = HthdFromHtid(hprc, htid);
    assert ( hthd != hthdNull );

    lpthd = (LPTHD) LLLock ( hthd );

    switch ( ireg ) {
        case CV_REG_GDTR:
        case CV_REG_GDTL:
        case CV_REG_IDTR:
        case CV_REG_IDTL:
        case CV_REG_LDTR:
        case CV_REG_TR:

        case CV_REG_CR0:
        case CV_REG_CR1:
        case CV_REG_CR2:
        case CV_REG_CR3:
        case CV_REG_CR4:

            lpregs = lpthd->pvSpecial;
            // fall thru

        case CV_REG_DR0:
        case CV_REG_DR1:
        case CV_REG_DR2:
        case CV_REG_DR3:
        case CV_REG_DR4:
        case CV_REG_DR5:
        case CV_REG_DR6:
        case CV_REG_DR7:

            if (!(lpthd->drt & drtSpecialPresent)) {
                X86UpdateSpecialRegisters( lpthd->hprc, hthd );
            }
            break;

        default:

            lpregs = lpthd->regs;

            if ( !(lpthd->drt & drtAllPresent) ) {
                UpdateRegisters ( lpthd->hprc, hthd );
            }
            break;
    }

	if (ireg==CV_ALLREG_TIMER)
	{
		SHREG shreg;
		shreg.hReg = (WORD)ireg;
		shreg.Byte4 = *(PDWORD)lpvRegValue;

		XOSD xosd = SendRequestX( dmfSetSpecialReg, hpid, htid, sizeof(shreg),
                        &shreg);

		if (xosd!=xosdNone)
			lpvRegValue = NULL;
	}
	else
	    lpvRegValue = X86DoSetReg ( (LPCONTEXT) lpregs, ireg & 0xff, lpvRegValue );

    if ( lpvRegValue == NULL ) {
        LLUnlock ( hthd );
        return xosdInvalidParameter;
    }

    ireg = ireg >> 8;
    if ( ireg != 0 ) {
        lpvRegValue = X86DoSetReg ( (LPCONTEXT) lpregs, ireg, lpvRegValue );
    }
    if ( lpvRegValue == NULL ) {
        LLUnlock ( hthd );
        return xosdInvalidParameter;
    }


    switch ( ireg ) {
        case CV_REG_GDTR:
        case CV_REG_GDTL:
        case CV_REG_IDTR:
        case CV_REG_IDTL:
        case CV_REG_LDTR:
        case CV_REG_TR:

        case CV_REG_CR0:
        case CV_REG_CR1:
        case CV_REG_CR2:
        case CV_REG_CR3:
        case CV_REG_CR4:

        case CV_REG_DR0:
        case CV_REG_DR1:
        case CV_REG_DR2:
        case CV_REG_DR3:
        case CV_REG_DR4:
        case CV_REG_DR5:
        case CV_REG_DR6:
        case CV_REG_DR7:

            lpthd->drt = (DRT) (lpthd->drt | drtAllDirty);
            break;

        default:

            lpthd->drt = (DRT) (lpthd->drt | drtAllDirty);
            break;
    }



    LLUnlock ( hthd );

    return xosd;

}


void
X86UpdateSpecialRegisters (
    HPRC hprc,
    HTHD hthd
    )
{

    LPTHD lpthd = (LPTHD) LLLock ( hthd );

    GuardTL();
    SendRequest ( dmfReadRegEx, HpidFromHprc ( hprc ), HtidFromHthd ( hthd ) );

    if (lpthd->dwcbSpecial) {
        //
        // in kernel mode...
        //
        _fmemcpy ( lpthd->pvSpecial, LpDmMsg->rgb, lpthd->dwcbSpecial );
        ((PCONTEXT) (lpthd->regs))->Dr0 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr0;
        ((PCONTEXT) (lpthd->regs))->Dr1 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr1;
        ((PCONTEXT) (lpthd->regs))->Dr2 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr2;
        ((PCONTEXT) (lpthd->regs))->Dr3 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr3;
        ((PCONTEXT) (lpthd->regs))->Dr6 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr6;
        ((PCONTEXT) (lpthd->regs))->Dr7 = ((PKSPECIAL_REGISTERS)(LpDmMsg->rgb))->KernelDr7;
    } else {
        //
        // User mode
        //
        ((PCONTEXT) (lpthd->regs))->Dr0 = ((LPDWORD)(LpDmMsg->rgb))[0];
        ((PCONTEXT) (lpthd->regs))->Dr1 = ((LPDWORD)(LpDmMsg->rgb))[1];
        ((PCONTEXT) (lpthd->regs))->Dr2 = ((LPDWORD)(LpDmMsg->rgb))[2];
        ((PCONTEXT) (lpthd->regs))->Dr3 = ((LPDWORD)(LpDmMsg->rgb))[3];
        ((PCONTEXT) (lpthd->regs))->Dr6 = ((LPDWORD)(LpDmMsg->rgb))[4];
        ((PCONTEXT) (lpthd->regs))->Dr7 = ((LPDWORD)(LpDmMsg->rgb))[5];
    }
    ReleaseTL();

    lpthd->drt = (DRT) (lpthd->drt & (~drtSpecialDirty) );
    lpthd->drt = (DRT) (lpthd->drt | drtSpecialPresent);

    LLUnlock ( hthd );

}




void
X86CopyFrameRegs(
    LPTHD lpthd,
    LPBPR lpbpr
    )
{
    ((PCONTEXT) (lpthd->regs))->SegCs   = lpbpr->segCS;
    ((PCONTEXT) (lpthd->regs))->SegSs   = lpbpr->segSS;
    ((PCONTEXT) (lpthd->regs))->Eip     = lpbpr->offEIP;
    ((PCONTEXT) (lpthd->regs))->Ebp     = lpbpr->offEBP;
}


void
X86AdjustForProlog(
    HPID hpid,
    HTID htid,
    PADDR origAddr,
    CANSTEP *CanStep
    )
{
    Unreferenced(hpid);
    Unreferenced(htid);
    Unreferenced(origAddr);
    Unreferenced(CanStep);
}


XOSD
X86GetFrameWithSrc(
	HPID	hpid,
	HTID	htid,
	UINT	cFrames,
	LPHTID	lphtid
	)
{
	HTID	hvtid;
	ADDR	addr = {0};
	XOSD	xosd;
	UINT	i;

	union {
		ADDR	addr;
		CANSTEP	CanStep;
	} buf;

	*lphtid = NULL;
	
	xosd = X86DoGetFrame (hpid, htid, 1, (LPARAM) &hvtid);

	if (xosd != xosdNone) {
		return xosd;
	}


	for (
		xosd = X86DoGetFrame (hpid, htid, 1, (LPARAM) &hvtid), i = 0;
		xosd == xosdNone, i < cFrames;
		xosd = X86DoGetFrame (hpid, hvtid, 1, (LPARAM) &hvtid), i++
		)
	{

		xosd = X86GetAddr (hpid, hvtid, adrPC, &addr);

		if (xosd != xosdNone) {
			return xosd;
		}

		buf.addr = addr;
			
		xosd = CallDB (dbcCanStep,
					   hpid,
					   htid,
					   CEXM_MDL_native,
					   NULL,
					   &buf);

		if (buf.CanStep.Flags == CANSTEP_YES) {
			*lphtid = hvtid;
			return xosdNone;
		}
	}

	return xosdEndOfStack;
}


		
	
