/*
 * nv10vpexec.c
 *
 * Copyright 2000 NVIDIA, Corporation.  All rights reserved.
 * 
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/*
** This file is shared between OpenGL and D3D so don't mess with it!
*/

/*
** This file should only be compiled if it's needed.  The normal OpenGL ifdefs:
**
**      #if defined(GL_NV_vertex_program) && __NV_ACCEL>=10
**      #ifdef __GL_USE_OG_INTEL_ASM
**
** and any OpenGL includes like:
**
**      #include "context.h"
**
** can't be used since this is used by D3D as well.
**
*/

#include <stddef.h>
#include <float.h>

#if defined(IS_OPENGL)
#include "nvassert.h"
#else
//#define assert()
#endif

#include "vtxpgmconsts.h"
#include "vtxpgmcomp.h"
#include "x86.h"

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(VIEW_AS_INT)
#define VIEW_AS_INT(x) (*(int *) &(x))
#endif

#define INCLUDE_MULTIPLY_BY_ZERO_CODE 1

#define FIX_SLT_ZERO_COMPARE 1

typedef enum ExprKind_Enum {
    E_NONE, E_MEM, E_MEMIX, E_IREG, E_FREG, E_AREG,
} ExprKind;

// Scalar types:

typedef struct ExprMem_Rec {
    ExprKind kind;
    int offset;
    int Negate;
} ExprMem;

typedef struct ExprAReg_Rec {
    ExprKind kind;
} ExprAReg;

typedef struct ExprIReg_Rec {
    ExprKind kind;
    int regNo;
} ExprIReg;

typedef struct ExprFReg_Rec {
    ExprKind kind;
    int regNo;
} ExprFReg;

typedef union Expr_Rec {
    ExprKind kind;
    ExprMem eMem;
    ExprAReg aReg;
    ExprIReg iReg;
    ExprFReg fReg;
} Expr;

// Vector types:

typedef struct Vector_Rec {
    Expr x;
    Expr y;
    Expr z;
    Expr w;
} Vector;

typedef struct Code_Rec {
    unsigned int ilcCount, ilcMax;
    unsigned char *ilcData;
    VtxProgCompileX86 *environment;
} Code;

typedef struct Coord_Rec {
    float x, y, z, w;
} Coord;

#if defined(IS_OPENGL)
    #define FP_BITS_LOG0 0xFF800000 // use -inf
#else
    #define FP_BITS_LOG0 0xFF7FFFFF // use a large negative number
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define OP_LIT_128_MINUS_EPSILON (128.0f - 1.0f/256.0f)
#define F_EXP_BIAS 127
#define LOCAL_VAR_SIZE 32

struct VtxProgImports_Rec VtxProgImports;

static char watermark[] = "wa\0\0t\2\ffer\0\0\0ma\0\ff\frk\0\0#0";

int __internal_setup(int i) { return watermark[i]; }

/*
** lLitOpFun()
*/

#if defined(IS_OPENGL)
static void lLitOpFun(Coord *a)
#else
static void _cdecl lLitOpFun(Coord *a)
#endif
{
    Coord *t = a + 1;
    float x, y, w;

    //if (vinst->dst.mask & DST_X_BIT)
        t->x = 1.0f;
    //if (vinst->dst.mask & (DST_Y_BIT | DST_Z_BIT)) {
    //    if (vinst->src.Signed) {
    //        x = -a->x;
    //        y = -a->y;
    //        w = -a->w;
    //    } else {
            x = a->x;
            y = a->y;
            w = a->w;
    //    }
        if (x < 0.0f)
            x = 0.0f;
        if (y < 0.0f)
            y = 0.0f;
    //    if (vinst->dst.mask & DST_Y_BIT)
            t->y = x;
    //    if (vinst->dst.mask & DST_Z_BIT) {
            if (x > 0.0f) {
                if (y == 0.0f) {
                    if (w == 0.0f) {
                        t->z = 1.0f;
                    } else {
                        t->z = 0.0f;
                    }
                } else {
                    if (w > OP_LIT_128_MINUS_EPSILON) {
                        w = OP_LIT_128_MINUS_EPSILON;
                    } else {
                        if (w < -OP_LIT_128_MINUS_EPSILON) {
                            w = -OP_LIT_128_MINUS_EPSILON;
                        }
                    }
                    t->z = VtxProgImports.expf(w*VtxProgImports.logf(y));
                    //////////////t->z = __GL_EXPF(w*__GL_LOGF(y));
                }
            } else {
                t->z = 0.0f;
            }
    //    }
    //}
    //if (vinst->dst.mask & DST_W_BIT)
        t->w = 1.0f;
} // lLitOpFun

///////////////////////////////////////////////////////////////////////////////

/*
** lIsNegated()
*/

static int lIsNegated(Vector *v)
{
    if (v->x.kind == E_MEM || v->x.kind == E_MEMIX) {
        return v->x.eMem.Negate;
    } else {
        return FALSE;
    }
} // lIsNegated
  
/*
** lGenSrcReg()
*/

static void lGenSrcReg(VtxProgCompileX86 *env, Expr *v, SrcReg *r)
{
    int lreg = r->reg;

    switch (lreg) {
    case VA_0:  case VA_1:  case VA_2:  case VA_3:
    case VA_4:  case VA_5:  case VA_6:  case VA_7:
    case VA_8:  case VA_9:  case VA_10: case VA_11:
    case VA_12: case VA_13: case VA_14: case VA_15:
        v->kind = E_MEM;
        v->eMem.offset = (lreg - VA_0)*sizeof(float)*4 + env->attrib_offset;
        v->eMem.Negate = FALSE;
        break;
    case VR_0:  case VR_1:  case VR_2:  case VR_3:
    case VR_4:  case VR_5:  case VR_6:  case VR_7:
    case VR_8:  case VR_9:  case VR_10: case VR_11:
    case VR_12: case VR_13: case VR_14: case VR_15:
        v->kind = E_MEM;
        v->eMem.offset = (lreg - VR_0)*sizeof(float)*4 + env->result_offset;
        v->eMem.Negate = FALSE;
        break;
    case ARL:
        v->kind = E_AREG;
        break;
    case PR_0:  case PR_1:  case PR_2:  case PR_3:
    case PR_4:  case PR_5:  case PR_6:  case PR_7:
    case PR_8:  case PR_9:  case PR_10: case PR_11:
    case PR_12: case PR_13: case PR_14: case PR_15:
    case PR_16: case PR_17: case PR_18: case PR_19:
    case PR_20: case PR_21: case PR_22: case PR_23:
    case PR_24: case PR_25: case PR_26: case PR_27:
    case PR_28: case PR_29: case PR_30: case PR_31:
    case PR_32: case PR_33: case PR_34: case PR_35:
    case PR_36: case PR_37: case PR_38: case PR_39:
    case PR_40: case PR_41: case PR_42: case PR_43:
    case PR_44: case PR_45: case PR_46: case PR_47:
    case PR_48: case PR_49: case PR_50: case PR_51:
    case PR_52: case PR_53: case PR_54: case PR_55:
    case PR_56: case PR_57: case PR_58: case PR_59:
    case PR_60: case PR_61: case PR_62: case PR_63:
    case PR_64: case PR_65: case PR_66: case PR_67:
    case PR_68: case PR_69: case PR_70: case PR_71:
    case PR_72: case PR_73: case PR_74: case PR_75:
    case PR_76: case PR_77: case PR_78: case PR_79:
    case PR_80: case PR_81: case PR_82: case PR_83:
    case PR_84: case PR_85: case PR_86: case PR_87:
    case PR_88: case PR_89: case PR_90: case PR_91:
    case PR_92: case PR_93: case PR_94: case PR_95:
        if (r->AddrReg) {
            v->kind = E_MEMIX;
            v->eMem.offset = r->addrRegOffset*sizeof(float)*4 + env->param_offset;
            v->eMem.Negate = FALSE;
        } else {
            v->kind = E_MEM;
            v->eMem.offset = (lreg - PR_0)*sizeof(float)*4 + env->param_offset;
            v->eMem.Negate = FALSE;
        }
        break;
    case TR_0:  case TR_1:  case TR_2:  case TR_3:
    case TR_4:  case TR_5:  case TR_6:  case TR_7:
    case TR_8:  case TR_9:  case TR_10: case TR_11:
    case TR_12: case TR_13: case TR_14: case TR_15:
        v->kind = E_MEM;
        v->eMem.offset = (lreg - TR_0)*sizeof(float)*4 + env->temp_offset;
        v->eMem.Negate = FALSE;
        break;
    default:
        assert(!"lGenSrcReg() - Bad register number");
        break;
    }
} // lGenSrcReg

/*
** lGenDstReg()
*/

static void lGenDstReg(VtxProgCompileX86 *env, Expr *v, Register_t freg)
{
    switch (freg) {
    case VA_0:  case VA_1:  case VA_2:  case VA_3:
    case VA_4:  case VA_5:  case VA_6:  case VA_7:
    case VA_8:  case VA_9:  case VA_10: case VA_11:
    case VA_12: case VA_13: case VA_14: case VA_15:
        v->kind = E_MEM;
        v->eMem.offset = (freg - VA_0)*sizeof(float)*4 + env->attrib_offset;
        v->eMem.Negate = FALSE;
        break;
    case VR_0:  case VR_1:  case VR_2:  case VR_3:
    case VR_4:  case VR_5:  case VR_6:  case VR_7:
    case VR_8:  case VR_9:  case VR_10: case VR_11:
    case VR_12: case VR_13: case VR_14: case VR_15:
        v->kind = E_MEM;
        v->eMem.offset = (freg - VR_0)*sizeof(float)*4 + env->result_offset;
        v->eMem.Negate = FALSE;
        break;
    case ARL:
        v->kind = E_AREG;
    case PR_0:  case PR_1:  case PR_2:  case PR_3:
    case PR_4:  case PR_5:  case PR_6:  case PR_7:
    case PR_8:  case PR_9:  case PR_10: case PR_11:
    case PR_12: case PR_13: case PR_14: case PR_15:
    case PR_16: case PR_17: case PR_18: case PR_19:
    case PR_20: case PR_21: case PR_22: case PR_23:
    case PR_24: case PR_25: case PR_26: case PR_27:
    case PR_28: case PR_29: case PR_30: case PR_31:
    case PR_32: case PR_33: case PR_34: case PR_35:
    case PR_36: case PR_37: case PR_38: case PR_39:
    case PR_40: case PR_41: case PR_42: case PR_43:
    case PR_44: case PR_45: case PR_46: case PR_47:
    case PR_48: case PR_49: case PR_50: case PR_51:
    case PR_52: case PR_53: case PR_54: case PR_55:
    case PR_56: case PR_57: case PR_58: case PR_59:
    case PR_60: case PR_61: case PR_62: case PR_63:
    case PR_64: case PR_65: case PR_66: case PR_67:
    case PR_68: case PR_69: case PR_70: case PR_71:
    case PR_72: case PR_73: case PR_74: case PR_75:
    case PR_76: case PR_77: case PR_78: case PR_79:
    case PR_80: case PR_81: case PR_82: case PR_83:
    case PR_84: case PR_85: case PR_86: case PR_87:
    case PR_88: case PR_89: case PR_90: case PR_91:
    case PR_92: case PR_93: case PR_94: case PR_95:
        v->kind = E_MEM;
        v->eMem.offset = (freg - PR_0)*sizeof(float)*4 + env->param_offset;
        v->eMem.Negate = FALSE;
        break;
    case TR_0:  case TR_1:  case TR_2:  case TR_3:
    case TR_4:  case TR_5:  case TR_6:  case TR_7:
    case TR_8:  case TR_9:  case TR_10: case TR_11:
    case TR_12: case TR_13: case TR_14: case TR_15:
        v->kind = E_MEM;
        v->eMem.offset = (freg - TR_0)*sizeof(float)*4 + env->temp_offset;
        v->eMem.Negate = FALSE;
        break;
    default:
        assert(!"lGenDstReg() - Bad register number");
        break;
    }
} // lGenDstReg

/*
** lGenAddOffset()
*/

static void lGenAddOffset(Expr *e, int fOffset)
{
    switch (e->kind) {
    case E_IREG:
    case E_FREG:
        assert(!"lGenAddOffset(I/FREG)");
        break;
    case E_MEM:
    case E_MEMIX:
        e->eMem.offset += fOffset;
        break;
    default:
        assert(!"lGenAddOffset()");
        break;
    }
} // lGenAddOffset

//////////////////////// X86 Code Generation Functions ////////////////////////

/*
** lGenLoadIReg()
*/

static void lGenLoadIReg(Code *fCode, Expr *e, int DoNegate)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_IREG:
        break;
    case E_FREG:
        assert(!"lGenLoadIReg(E_FREG)");
        break;
    case E_MEM:
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(e->eMem.offset);  // mov  eax, offset[gc]
            if (DoNegate && e->eMem.Negate) {
                xXOR_rm_imm(rmREG(rEAX), 0x80000000);               // xor  eax, 0x80000000
            }
            e->kind = E_IREG;
            e->iReg.regNo = rEAX;
        break;
    case E_MEMIX:
            xMOV_r_rm(rEAX, rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);  // mov  eax, offset[gc,esi]
            if (DoNegate && e->eMem.Negate) {
                xXOR_rm_imm(rmREG(rEAX), 0x80000000);               // xor  eax, 0x80000000
            }
            e->kind = E_IREG;
            e->iReg.regNo = rEAX;
        break;
    default:
        assert(!"lGenLoadIReg()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenLoadIReg

/*
** lGenLoad()
*/

static void lGenLoad(Code *fCode, Expr *e, int IRegOK, int DoNegate)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_IREG:
        if (!IRegOK)
            assert(!"lGenLoad(E_IREG, FLOAT)");
        break;
    case E_FREG:
        break;
    case E_MEM:
        if (IRegOK) {
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(e->eMem.offset);  // mov  eax, offset[gc]
            if (DoNegate && e->eMem.Negate) {
                xXOR_rm_imm(rmREG(rEAX), 0x80000000);               // xor  eax, 0x80000000
            }
            e->kind = E_IREG;
            e->iReg.regNo = rEAX;
        } else {
            xFLD_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);          // fld  offset[gc]
            if (DoNegate && e->eMem.Negate) {
                xFCHS;                                              // xchs
            }
            e->kind = E_FREG;
        }            
        break;
    case E_MEMIX:
            xFLD_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);    // fld  offset[gc,esi]
            if (DoNegate && e->eMem.Negate) {
                xFCHS;                                              // xchs
            }
            e->kind = E_FREG;
        break;
    default:
        assert(!"lGenLoad()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenLoad

/*
** lGenStore()
*/

static void lGenStore(Code *fCode, Expr *v, Expr *e, int DontFree)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (v->kind) {
    case E_MEM:
        switch (e->kind) {
        case E_IREG:
            xMOV_rm_r(rmIND32(rEBX), e->iReg.regNo) xOFS32(v->eMem.offset); // mov  offset[gc], eReg
            break;
        case E_FREG:
            if (DontFree) {
                xFST_rm(rmIND32(rEBX)) xOFS32(v->eMem.offset);              // fst  offset[gc]
            } else {
                xFSTP_rm(rmIND32(rEBX)) xOFS32(v->eMem.offset);             // fstp offset[gc]
            }
            break;
        default:
            assert(!"lGenStore(E_MEM, BAD)");
        }
        break;
    default:
        assert(!"lGenStore()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenStore

/*
** lGenNop() - Insert NOP instruction.
*/

static void lGenNop(Code *fCode)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xNOP;       // nop
    X86_CODE_RESTORE(fCode);
} // lGenNop

/*
** lGenChs() - Change Sign of TOS.
*/

static void lGenChs(Code *fCode)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xFCHS;      // xchs
    X86_CODE_RESTORE(fCode);
} // lGenChs

/*
** lGenFxch() - Swap TOS with TOS - 1.
*/

static void lGenFxch(Code *fCode)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xFXCH_st(1);        // fxch
    X86_CODE_RESTORE(fCode);
} // lGenFxch

/*
** lGenAdd() - Add E to TOS.
*/

static void lGenAdd(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        xFADDP_st(1);   // faddp st(1)
        break;
    case E_MEM:
        xFADD_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);         // fadd  offset[gc]
        break;
    case E_MEMIX:
        xFADD_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);   // fadd  offset[gc,esi]
        break;
    default:
        assert(!"lGenAdd()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenAdd

/*
** lGenSub() - Subtract E from TOS.
*/

static void lGenSub(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        xFSUBP_st(1);   // fsubp st(1)
        break;
    case E_MEM:
        xFSUB_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);         // fsub  offset[gc]
        break;
    case E_MEMIX:
        xFSUB_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);   // fsub  offset[gc,esi]
        break;
    default:
        assert(!"lGenSub()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenSub

/*
** lGenSubR() - Reverse Subtract E from TOS.
*/

static void lGenSubR(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        xFSUBRP_st(1);   // fsubrp st(1)
        break;
    case E_MEM:
        xFSUBR_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);        // fsubr  offset[gc]
        break;
    case E_MEMIX:
        xFSUBR_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);  // fsubr  offset[gc,esi]
        break;
    default:
        assert(!"lGenAdd()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenSubR

/*
** lGenAddVariant() - Add E to TOS, possibly negating either or both.
*/

static void lGenAddVariant(Code *fCode, Expr *e, int NegateA, int NegateB)
{
    if (NegateA) {
        if (NegateB) {
            lGenAdd(fCode, e);
            lGenChs(fCode);
        } else {
            lGenSubR(fCode, e);
        }
    } else {
        if (NegateB) {
            lGenSub(fCode, e);
        } else {
            lGenAdd(fCode, e);
        }
    }
} // lGenAddVariant

/*
** lGenFcomp() - Compare E to TOS.  Pop everything.
*/

static void lGenFcomp(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        xFCOMPP();                                          // fcompp
        break;
    case E_MEM:
        xFCOMP_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);        // fcomp  offset[gc]
        break;
    case E_MEMIX:
        xFCOMP_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);  // fcomp  offset[gc,esi]
        break;
    default:
        assert(!"lGenFcomp()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenFcomp

/*
** lGenPushFloatComp() - Push a 1.0 or 0.0 on the stack based on fpcc.
**         Uses 0[esp] to move data to fp stack.
*/

static void lGenPushFloatComp(Code *fCode, int fmask, int fSNZ)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xFNSTSW;                            // fnstsw ax
    xTEST_rm_imm8(rmREG(rAH), fmask);   // test   ah, mask
    if (fSNZ) {
        xSETNZ_rm8(rmREG(rEAX));        // setnz al
    } else {
        xSETZ_rm8(rmREG(rEAX));         // setz  al
    }
    xAND_rm_imm(rmREG(rEAX), 1);        // and   eax, 1
    xMOV_rm_r(rmSIB, rEAX) xSIB_esp;    // mov   [esp], eax
    xFILD_rm(rmSIB) xSIB_esp;           // fild  [esp]
    X86_CODE_RESTORE(fCode);
} // lGenPushFloatComp

#if defined(FIX_SLT_ZERO_COMPARE)

/*
** lGenSgeVariantSignedZero() - Compare EAX to +/-E treating +0.0 > -0.0 and return
**         a 1.0 or 0.0 on TOS.  Use SLT or SGE depending on IsSLT.
**
** Method: Xor sign bit with the lower 31 bits and use signed int compare.
**         Sign/Magnitude int shown.  Also works for fp. Sign/BiasedExp/Mantissa.
**
**  Value   S/M   Mask  xor(MSK,Val)  Value
**    1     001    000       001        1
**   +0     000    000       000        0
**   -0     100    011       111       -1
**   -1     101    011       110       -2
*/

static void lGenSgeVariantSignedZero(Code *fCode, Expr *e, int NegateE, int IsSLT)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);

    // Convert EAX arg into EDX:

    xMOV_rm_r(rmREG(rEDX), rEAX);       // mov edx, eax
    xSAR_rm_imm8(rmREG(rEDX), 30);      // sar edx, 30
    xSHR_rm_1(rmREG(rEDX));             // shr edx
    xXOR_rm_r(rmREG(rEDX), rEAX);       // xor edx, eax

    // Load second arg int EAX and convert using ECX as scratch:

    X86_CODE_RESTORE(fCode);
    lGenLoadIReg(fCode, e, NegateE);
    X86_CODE_LOAD(fCode);

    xMOV_rm_r(rmREG(rECX), rEAX);       // mov ecx, eax
    xSAR_rm_imm8(rmREG(rECX), 30);      // sar ecx, 30
    xSHR_rm_1(rmREG(rECX));             // shr ecx
    xXOR_rm_r(rmREG(rEAX), rECX);       // xor eax, ecx

    // Compare and use SLT or SGE to get result:

    xXOR_rm_r(rmREG(rECX), rECX);       // xor ecx, ecx ; For result
    xCMP_r_rm(rEDX, rmREG(rEAX));       // cmp eax, edx
    if (IsSLT) {
        xSETL_rm8(rmREG(rCL));          // setl cl
    } else {
        xSETGE_rm8(rmREG(rCL));         // setge cl
    }

    // Load the sucker onto the fp stack:

    xMOV_rm_r(rmSIB, rECX) xSIB_esp;    // mov   [esp], ecx
    xFILD_rm(rmSIB) xSIB_esp;           // fild  [esp]

    X86_CODE_RESTORE(fCode);
} // lGenSgeVariantSignedZero

#else // defined(FIX_SLT_ZERO_COMPARE)

/*
** lGenSgeVariant() - Compare E to TOS and return a 1 if GE, else a 0.
**
**        Possibly negating either or both arg.
*/

static void lGenSgeVariant(Code *fCode, Expr *e, int NegateA, int NegateB, int IsSLT)
{
    if (NegateA) {
        lGenChs(fCode);
        if (NegateB) {
            lGenLoad(fCode, e, FALSE, TRUE);
            lGenFxch(fCode);
            lGenFcomp(fCode, e);
        } else {
            lGenFcomp(fCode, e);
        }
    } else {
        if (NegateB) {
            lGenLoad(fCode, e, FALSE, TRUE);
            lGenFxch(fCode);
            lGenFcomp(fCode, e);
        } else {
            lGenFcomp(fCode, e);
        }
    }
    lGenPushFloatComp(fCode, 0x01, IsSLT);
} // lGenSgeVariant

#endif // defined(FIX_SLT_ZERO_COMPARE)

/*
** lGenMax() - Set TOS =  MAX(TOS, E).
*/

static void lGenMinMax(Code *fCode, Expr *e, int IsMax)
{
    X86_CODE_DECLARE;
    unsigned int loc;

    switch (e->kind) {
    case E_FREG:
        assert(!"lGenMinMax(E_FREG)");
        break;
    case E_MEM:
        lGenLoad(fCode, e, FALSE, TRUE);
        X86_CODE_LOAD(fCode);
        xFCOM_st(1);                            //      fcom   st(1)
        xFNSTSW;                                //      fnstsw ax
        if (IsMax) {
            xTEST_rm_imm8(rmREG(rAH), 0x41);    //      test   ah, 41  JLE (TOS <= st(1))
            xLABEL(loc);
            xJNZ(0);                            //      jne    lab
        } else {
            xTEST_rm_imm8(rmREG(rAH), 0x01);    //      test   ah, 01  JGE (TOS >= st(1))
            xLABEL(loc);
            xJZ(0);                             //      je     lab
        }
        xFXCH_st(1);                            //      fxch
        xTARGET_b8(loc);                        // lab:
        xFSTP_st(0);                            //      fstp   st(0)
        X86_CODE_RESTORE(fCode);
        break;
    default:
        assert(!"lGenMinMax()");
        break;
    }
} // lGenMinMax

/*
** lGenMul() - Multiply TOS by E.
*/

static void lGenMul(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        assert(!"lGenMul(E_FREG)");
        break;
    case E_MEM:
        xFMUL_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);         // fmul  offset[gc]
        break;
    case E_MEMIX:
        xFMUL_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);   // fmul  offset[gc,esi]
        break;
    default:
        assert(!"lGenMul()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenMul

#if defined(INCLUDE_MULTIPLY_BY_ZERO_CODE)

/*
** lGenTestForZeroMul() - Special case multiply:  If either value is +/- 0.0, result is 0.0
*/

static void lGenTestForZeroMul(Code *fCode, Expr *a, Expr *b, int FNegate)
{
    int zero1, zero2, done;
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (a->kind) {
    case E_IREG:
        assert(!"lGenTestForZeroMul(E_IREG)");
        break;
    case E_FREG:
        assert(!"lGenTestForZeroMul(E_FREG)");
        break;
    case E_MEM:
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(a->eMem.offset);       // mov eax, offset[gc]
        break;
    case E_MEMIX:
        xMOV_r_rm(rEAX, rmSIB32) xSIB32(rEBX, rESI, a->eMem.offset); // mov eax, offset[gc,esi]
        break;
    default:
        assert(!"lGenTestForZeroMul()");
        break;
    }
    // Mask off sign bit:
    xAND_rm_imm(rmREG(rEAX), 0x7fffffff);       //   and  eax, 0x7fffffff
    xLABEL(zero1);
    xJZ(0);                                     //   jz   zero

    switch (b->kind) {
    case E_IREG:
        assert(!"lGenTestForZeroMul(E_IREG)");
        break;
    case E_FREG:
        assert(!"lGenTestForZeroMul(E_FREG)");
        break;
    case E_MEM:
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(b->eMem.offset);       // mov eax, offset[gc]
        break;
    case E_MEMIX:
        xMOV_r_rm(rEAX, rmSIB32) xSIB32(rEBX, rESI, b->eMem.offset); // mov eax, offset[gc,esi]
        break;
    default:
        assert(!"lGenTestForZeroMul()");
        break;
    }
    // Mask off sign bit:
    xAND_rm_imm(rmREG(rEAX), 0x7fffffff);       //   and  eax, 0x7fffffff
    xLABEL(zero2);
    xJZ(0);                                     //   jz   zero
    X86_CODE_RESTORE(fCode);

    // Things are OK.  Do the multiply.

    lGenLoad(fCode, a, FALSE, FALSE);
    lGenMul(fCode, b);
    if (FNegate)
        lGenChs(fCode);

    X86_CODE_LOAD(fCode);
    xLABEL(done);
    xJMP8(0);
    xTARGET_b8(zero1);                         // zero:
    xTARGET_b8(zero2);                         // zero:
    xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_zero_offset); //   fld   gc->float_zero
    xTARGET_b8(done);                            // done:
    X86_CODE_RESTORE(fCode);
} // lGenTestForZeroMul

#endif // INCLUDE_MULTIPLY_BY_ZERO_CODE

/*
** lGenDivr() - Divide TOS into E.
*/

static void lGenDivr(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    switch (e->kind) {
    case E_FREG:
        assert(!"lGenDivr(E_FREG)");
        break;
    case E_MEM:
        xFDIVR_rm(rmIND32(rEBX)) xOFS32(e->eMem.offset);        // fdivr  offset[gc]
        break;
    case E_MEMIX:
        xFDIVR_rm(rmSIB32) xSIB32(rEBX, rESI, e->eMem.offset);  // fdivr  offset[gc,esi]
        break;
    default:
        assert(!"lGenDivr()");
        break;
    }
    X86_CODE_RESTORE(fCode);
} // lGenDivr

#if defined(NV_INTERNAL_OPCODES)

/*
** lGenRccr() - Divide TOS into E, clamped.
*/

static void lGenRccr(Code *fCode, Expr *e)
{
    X86_CODE_DECLARE;
    int test_neg, get_answer, done;

    // TOS = 1.0f/x;

    lGenDivr(fCode, e);

    X86_CODE_LOAD(fCode);

    // iexp = ((VIEW_AS_INT(f) & 0x7f800000) >> 23) - F_EXP_BIAS;
    // if (iexp >= 64) {
    //     VIEW_AS_INT(f) = ((64 + F_EXP_BIAS) << 23) |
    //                           (VIEW_AS_INT(f) & 0x80000000);
    // } else {
    //     if (iexp <= -64) {
    //         VIEW_AS_INT(f) = ((-64 + F_EXP_BIAS) << 23) |
    //                               (VIEW_AS_INT(f) & 0x80000000);
    //     }
    // }

    xFST_rm(rmSIB) xSIB_esp;                    //    fst  [esp]
    xMOV_r_rm(rEAX, rmSIB) xSIB_esp;            //    mov  eax, [esp]
    xAND_rm_imm(rmREG(rEAX), 0x7fffffff);       //    and  eax, 0x7fffffff

    // Clamp min magnitude to 0x1f800000;

    xCMP_rm_imm(rmREG(rEAX), 0x1f800000);       //    cmp  eax, 0x1f800000
    xLABEL(test_neg);
    xJG(0)                                      //    jl   test_neg
    xMOV_r_rm(rEAX, rmSIB) xSIB_esp;            //    mov  eax, [esp]
    xAND_rm_imm(rmREG(rEAX), 0x80000000);       //    and  eax, 0x80000000
    xOR_rm_imm(rmREG(rEAX), 0x1f800000);        //    or   eax, 0x1f800000
    xLABEL(get_answer);
    xJMP8(0);                                   //    jmp  get_answer

    // Clamp max magnitude to 0x5f800000;

    xTARGET_b8(test_neg);                       // test_neg:
    xCMP_rm_imm(rmREG(rEAX), 0x5f800000);       //    cmp  eax, 0x5f800000
    xLABEL(done);
    xJL(0)                                      //    jl   done
    xMOV_r_rm(rEAX, rmSIB) xSIB_esp;            //    mov  eax, [esp]
    xAND_rm_imm(rmREG(rEAX), 0x80000000);       //    and  eax, 0x80000000
    xOR_rm_imm(rmREG(rEAX), 0x5f800000);        //    or   eax, 0x5f800000

    xTARGET_b8(get_answer);                     // get_answer:
    xMOV_rm_r(rmSIB, rEAX) xSIB_esp;            //    mov  eax, [esp]
    xFSTP_st(0);                                //    fstp st(0)
    xFLD_rm(rmSIB) xSIB_esp;                    //    fld  [esp]

    // Answer in TOS:

    xTARGET_b8(done);                            // done:

    X86_CODE_RESTORE(fCode);
} // lGenRccr
#endif // NV_INTERNAL_OPCODES

/*
** lGenAbsSqrt() - Take the Square Root of ABS(TOS).
*/

static void lGenAbsSqrt(Code *fCode)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xFABS();
    xFSQRT();
    X86_CODE_RESTORE(fCode);
} // lGenSqrt

/*
** lGenIFloor() - ireg = ifloor(TOS).  Pops TOS and scratches esp(0-7).
**
** Arguments:
**     [tos]  = x
**
** Results:
**     [esp]  = floor(x)
**
** Scratches:
*/

static void lGenIFloor(Code *fCode, int iReg, int PopFP)
{
    X86_CODE_DECLARE;

    X86_CODE_LOAD(fCode);
    xMOV_rm_imm(rmREG(iReg), 0x047f);   // mov   iReg, 0x047f ; Round -INF, 24b
    xMOV_rm_r(rmSIB, iReg) xSIB_esp;    // mov   [esp], iReg
    xFSTCW_rm(rmSIB8) xSIB8_esp(4);     // fstcw 4[esp]     ; Save old FPCW
    xFLDCW_rm(rmSIB) xSIB_esp;          // fldcw [esp]      ; Load Round -INF, 24
    if (PopFP) {
        xFISTP_rm(rmSIB) xSIB_esp;      // fistp [esp]      ; floor(x)
    } else {
        xFIST_rm(rmSIB) xSIB_esp;       // fist  [esp]      ; floor(x)
    }
    xFLDCW_rm(rmSIB8) xSIB8_esp(4);     // fldcw 4[esp]     ; Restore FPCW
//  xMOV_r_rm(iReg, rmSIB) xSIB_esp;    // mov   iReg, [esp]
    X86_CODE_RESTORE(fCode);
} // lGenIFloor

////////////////////////// NV20 Level Code Generation /////////////////////////

/*
** lGenFloatOne()
*/

static void lGenFloatOne(VtxProgCompileX86 *env, Expr *r)
{
    r->kind = E_MEM;
    r->eMem.Negate = FALSE;
    r->eMem.offset = env->float_one_offset;
} // lGenFloatOne

/*
** lGenScalarSrcReg()
*/

static void lGenScalarSrcReg(VtxProgCompileX86 *env, Expr *r, ScalarSrcReg *reg)
{
    Expr v;
    int ii;

    lGenSrcReg(env, &v, &reg->reg);
    switch (v.kind) {
    case E_MEM:
        r->kind = E_MEM;
        r->eMem.Negate = reg->Signed;
        ii = reg->com;
        r->eMem.offset = v.eMem.offset + (ii & 3)*sizeof(float);
        break;
    default:
        assert(!"lGenScalarSrcReg()");
        break;
    }
} // lGenScalarSrcReg

/*
** lGenSwizzleSrcReg()
*/

static void lGenSwizzleSrcReg(VtxProgCompileX86 *env, Vector *a, SwizzleSrcReg *reg)
{
    Expr v;
    int ii;

    lGenSrcReg(env, &v, &reg->reg);
    switch (v.kind) {
    case E_MEM:
        a->x.kind = a->y.kind = a->z.kind = a->w.kind = E_MEM;
        a->x.eMem.Negate = a->y.eMem.Negate = a->z.eMem.Negate = a->w.eMem.Negate = reg->Signed;
        ii = reg->com4;
        a->x.eMem.offset = v.eMem.offset + (ii & 3)*sizeof(float);
        a->y.eMem.offset = v.eMem.offset + ((ii >> 2) & 3)*sizeof(float);
        a->z.eMem.offset = v.eMem.offset + ((ii >> 4) & 3)*sizeof(float);
        a->w.eMem.offset = v.eMem.offset + ((ii >> 6) & 3)*sizeof(float);
        break;
    case E_MEMIX:
        a->x.kind = a->y.kind = a->z.kind = a->w.kind = E_MEMIX;
        a->x.eMem.Negate = a->y.eMem.Negate = a->z.eMem.Negate = a->w.eMem.Negate = reg->Signed;
        ii = reg->com4;
        a->x.eMem.offset = v.eMem.offset + (ii & 3)*sizeof(float);
        a->y.eMem.offset = v.eMem.offset + ((ii >> 2) & 3)*sizeof(float);
        a->z.eMem.offset = v.eMem.offset + ((ii >> 4) & 3)*sizeof(float);
        a->w.eMem.offset = v.eMem.offset + ((ii >> 6) & 3)*sizeof(float);
        break;
    default:
        assert(!"lGenSwizzleSrcReg()");
        break;
    }
} // lGenSwizzleSrcReg

/*
** lGenStoreMaskedDstReg()
*/

static void lGenStoreMaskedDstReg(Code *fCode, MaskedDstReg *reg, Vector *a)
{
    int lmask = reg->mask;
    Expr v;

    lGenDstReg(fCode->environment, &v, reg->reg);
    lGenAddOffset(&v, 12);
    if (lmask & DST_W_BIT) {
        lGenLoad(fCode, &a->w, TRUE, TRUE);
        lGenStore(fCode, &v, &a->w, FALSE);
    }
    lGenAddOffset(&v, -4);
    if (lmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, TRUE, TRUE);
        lGenStore(fCode, &v, &a->z, FALSE);
    }
    lGenAddOffset(&v, -4);
    if (lmask & DST_Y_BIT) {
        lGenLoad(fCode, &a->y, TRUE, TRUE);
        lGenStore(fCode, &v, &a->y, FALSE);
    }
    lGenAddOffset(&v, -4);
    if (lmask & DST_X_BIT) {
        lGenLoad(fCode, &a->x, TRUE, TRUE);
        lGenStore(fCode, &v, &a->x, FALSE);
    }
} // lGenStoreMaskedDstReg

/*
** lGenStoreMaskedDstRegScalar()
*/

static void lGenStoreMaskedDstRegScalar(Code *fCode, MaskedDstReg *reg, Expr *a)
{
    int lmask = reg->mask & DST_XYZW_BITS;
    Expr v;

    lGenDstReg(fCode->environment, &v, reg->reg);
    if (lmask & DST_X_BIT) {
        lmask &= ~DST_X_BIT;
        lGenLoad(fCode, a, TRUE, TRUE);
        lGenStore(fCode, &v, a, lmask);
    }
    lGenAddOffset(&v, 4);
    if (lmask & DST_Y_BIT) {
        lmask &= ~DST_Y_BIT;
        lGenLoad(fCode, a, TRUE, TRUE);
        lGenStore(fCode, &v, a, lmask);
    }
    lGenAddOffset(&v, 4);
    if (lmask & DST_Z_BIT) {
        lmask &= ~DST_Z_BIT;
        lGenLoad(fCode, a, TRUE, TRUE);
        lGenStore(fCode, &v, a, lmask);
    }
    lGenAddOffset(&v, 4);
    if (lmask & DST_W_BIT) {
        lGenLoad(fCode, a, TRUE, TRUE);
        lGenStore(fCode,&v, a, FALSE);
    }
} // lGenStoreMaskedDstRegScalar

/*
** lGenAddOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenAddOp(Code *fCode, Vector *r, Vector *a, Vector *b, int fmask)
{
    int NegateA = lIsNegated(a);
    int NegateB = lIsNegated(b);

    if (fmask & DST_X_BIT) {
        lGenLoad(fCode, &a->x, FALSE, FALSE);
        lGenAddVariant(fCode, &b->x, NegateA, NegateB);
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        lGenLoad(fCode, &a->y, FALSE, FALSE);
        lGenAddVariant(fCode, &b->y, NegateA, NegateB);
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, FALSE, FALSE);
        lGenAddVariant(fCode, &b->z, NegateA, NegateB);
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        lGenLoad(fCode, &a->w, FALSE, FALSE);
        lGenAddVariant(fCode, &b->w, NegateA, NegateB);
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenAddOp

/*
** lGenAlrOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenAlrOp(Code *fCode, Expr *a)
{
    X86_CODE_DECLARE;
    int test_neg, done1, done2;

    assert(LOCAL_VAR_SIZE >= 8);

    // Min usable value: -63, since A0.x+63 -> 0
    // Max usable value: 159, since A0.x-64 -> 95  (159 = 95 + 64)
    // Out of range values changed to -64, so we can index full speed
    // into 128 registers of 0's that are padded on each end.

    lGenLoad(fCode, a, FALSE, TRUE);
    lGenIFloor(fCode, rESI, 1);
    X86_CODE_LOAD(fCode);
    xMOV_r_rm(rESI, rmSIB) xSIB_esp;//   mov   esi, [esp]

    xCMP_rm_imm(rmREG(rESI), 159);  //   cmp  esi, 159 ; Is value > 159?
    xLABEL(test_neg);
    xJLE(0);                        //   jle  test_neg
    xMOV_rm_imm(rmREG(rESI), -64);  //   mov  esi, -64
    xLABEL(done1);
    xJMP8(0);                       //   jmp  get_answer

    xTARGET_b8(test_neg);           // test_neg:
    xCMP_rm_imm(rmREG(rESI), -63);  //   cmp  esi, -63 ; Is value < -63?
    xLABEL(done2);
    xJGE(0);                        //   jge  test_neg
    xMOV_rm_imm(rmREG(rESI), -64);  //   mov  esi, -64

    xTARGET_b8(done1);              // done:
    xTARGET_b8(done2);
    xSHL_rm_imm8(rmREG(rESI), 4);   //   shl  esi, 4  ; 16 bytes/register
    X86_CODE_RESTORE(fCode);
} // lGenAlrOp

/*
** lGenDotProductOp() - Simple version:  Assumes all args in memory, leaves result on TOS
*/

static void lGenDotProductOp(Code *fCode, Expr *v, Vector *a, Vector *b, int size)
{
    int Negate = lIsNegated(a) ^ lIsNegated(b);

    lGenLoad(fCode, &a->x, FALSE, FALSE);
    lGenMul(fCode, &b->x);

    lGenLoad(fCode, &a->y, FALSE, FALSE);
    lGenMul(fCode, &b->y);

    lGenLoad(fCode, &a->z, FALSE, FALSE);
    lGenMul(fCode, &b->z);

    if (size == 4) {
        lGenLoad(fCode, &a->w, FALSE, FALSE);
        lGenMul(fCode, &b->w);
        lGenAdd(fCode, &a->z);
    } else {
        if (size == 0) {
            // DPH
            lGenAdd(fCode, &b->w);
        }
    }

    lGenAdd(fCode, &a->y);
    lGenAdd(fCode, &a->x);
    if (Negate)
        lGenChs(fCode);

    v->kind = E_FREG;
    v->fReg.regNo = 0;
} // lGenDotProductOp

/*
** lGenDstOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenDstOp(Code *fCode, Vector *r, Vector *a, Vector *b, int fmask)
{
    X86_CODE_DECLARE;

    int NegateA = lIsNegated(a);
    int NegateB = lIsNegated(b);

    if (fmask & DST_X_BIT) {
        X86_CODE_LOAD(fCode);
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        X86_CODE_RESTORE(fCode);
        r->x.kind = E_FREG; // Could do: xor eax, eax
    } else {
        r->x.kind = E_NONE;
    }

    if (fmask & DST_Y_BIT) {
#if defined(INCLUDE_MULTIPLY_BY_ZERO_CODE)
        //
        // Special case multiply:  If either value is +/- 0.0, result is 0.0
        //

        lGenTestForZeroMul(fCode, &a->y, &b->y, NegateA ^ NegateB);
        r->y.kind = E_FREG;
#else
        lGenLoad(fCode, &a->y, FALSE, FALSE);
        lGenMul(fCode, &b->y);
        if (NegateA ^ NegateB)
            lGenChs(fCode);
        r->y.kind = E_FREG;
#endif // INCLUDE_MULTIPLY_BY_ZERO_CODE
    } else {
        r->y.kind = E_NONE;
    }

    if (fmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, FALSE, TRUE);
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }

    if (fmask & DST_W_BIT) {
        lGenLoad(fCode, &b->w, FALSE, TRUE);
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenDstOp

/*
** lGenLitOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenLitOp(Code *fCode, Vector *r, Vector *a, int fmask)
{
    X86_CODE_DECLARE;

    // Create a temp area on the stack and store all args there:

    lGenLoad(fCode, &a->x, FALSE, TRUE);
    lGenLoad(fCode, &a->y, FALSE, TRUE);
    lGenLoad(fCode, &a->z, FALSE, TRUE);
    lGenLoad(fCode, &a->w, FALSE, TRUE);

    assert(LOCAL_VAR_SIZE >= 32);

    X86_CODE_LOAD(fCode);
    xFSTP_rm(rmSIB8) xSIB8_esp(12);             // fstp 12[esp]  w
    xFSTP_rm(rmSIB8) xSIB8_esp(8);              // fstp 8[esp]   z
    xFSTP_rm(rmSIB8) xSIB8_esp(4);              // fstp 4[esp]   y
    xFSTP_rm(rmSIB) xSIB_esp;                   // fstp [esp]    x

    // Call a "C" function to compute this one:

    xPUSH_r(rESP);                              // push esp
    xMOV_rm_imm(rmREG(rEAX), (int) &lLitOpFun); // mov  eax, &lLitOpFun
    xCALL_rm(rmREG(rEAX));
    xADD_rm_imm8(rmREG(rESP), 4);

    // Load the needed results and pop the stack:

    if (fmask & DST_X_BIT) {
        xFLD_rm(rmSIB8) xSIB8_esp(16);          // fld  16[esp]  x' == 1.0f
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        xFLD_rm(rmSIB8) xSIB8_esp(20);          // fld  20[esp]  y'
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        xFLD_rm(rmSIB8) xSIB8_esp(24);          // fld  24[esp]  z'
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        xFLD_rm(rmSIB8) xSIB8_esp(28);          // fld  28[esp]  w' == 1.0f
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
    X86_CODE_RESTORE(fCode);

} // lGenLitOp

/*
** lGenMinMaxOp() - Simple version:  Assumes all args in memory,
**         leaves result on TOS.
*/

static void lGenMinMaxOp(Code *fCode, Vector *r, Vector *a,
                         Vector *b, int fmask, int IsMax)
{
    if (fmask & DST_X_BIT) {
        lGenLoad(fCode, &a->x, FALSE, TRUE);
        lGenMinMax(fCode, &b->x, IsMax);
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        lGenLoad(fCode, &a->y, FALSE, TRUE);
        lGenMinMax(fCode, &b->y, IsMax);
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, FALSE, TRUE);
        lGenMinMax(fCode, &b->z, IsMax);
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        lGenLoad(fCode, &a->w, FALSE, TRUE);
        lGenMinMax(fCode, &b->w, IsMax);
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenMinMaxOp

/*
** lGenMulOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenMulOp(Code *fCode, Vector *r, Vector *a, Vector *b, int fmask)
{
    int Negate = lIsNegated(a) ^ lIsNegated(b);

    if (fmask & DST_X_BIT) {
        lGenLoad(fCode, &a->x, FALSE, FALSE);
        lGenMul(fCode, &b->x);
        if (Negate)
            lGenChs(fCode);
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        lGenLoad(fCode, &a->y, FALSE, FALSE);
        lGenMul(fCode, &b->y);
        if (Negate)
            lGenChs(fCode);
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, FALSE, FALSE);
        lGenMul(fCode, &b->z);
        if (Negate)
            lGenChs(fCode);
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        lGenLoad(fCode, &a->w, FALSE, FALSE);
        lGenMul(fCode, &b->w);
        if (Negate)
            lGenChs(fCode);
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenMulOp

/*
** lGenSgeOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenSgeOp(Code *fCode, Vector *r, Vector *a, Vector *b, int fmask, int IsSLT)
{
    int NegateA = lIsNegated(a);
    int NegateB = lIsNegated(b);

    assert(LOCAL_VAR_SIZE >= 4);

    if (fmask & DST_X_BIT) {
#if defined(FIX_SLT_ZERO_COMPARE)
        lGenLoadIReg(fCode, &a->x, NegateA);
        lGenSgeVariantSignedZero(fCode, &b->x, NegateB, IsSLT);
#else
        lGenLoad(fCode, &a->x, FALSE, FALSE);
        lGenSgeVariant(fCode, &b->x, NegateA, NegateB, IsSLT);
#endif
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
#if defined(FIX_SLT_ZERO_COMPARE)
        lGenLoadIReg(fCode, &a->y, NegateA);
        lGenSgeVariantSignedZero(fCode, &b->y, NegateB, IsSLT);
#else
        lGenLoad(fCode, &a->y, FALSE, FALSE);
        lGenSgeVariant(fCode, &b->y, NegateA, NegateB, IsSLT);
#endif
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
#if defined(FIX_SLT_ZERO_COMPARE)
        lGenLoadIReg(fCode, &a->z, NegateA);
        lGenSgeVariantSignedZero(fCode, &b->z, NegateB, IsSLT);
#else
        lGenLoad(fCode, &a->z, FALSE, FALSE);
        lGenSgeVariant(fCode, &b->z, NegateA, NegateB, IsSLT);
#endif
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
#if defined(FIX_SLT_ZERO_COMPARE)
        lGenLoadIReg(fCode, &a->w, NegateA);
        lGenSgeVariantSignedZero(fCode, &b->w, NegateB, IsSLT);
#else
        lGenLoad(fCode, &a->w, FALSE, FALSE);
        lGenSgeVariant(fCode, &b->w, NegateA, NegateB, IsSLT);
#endif
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenSgeOp

/*
** lGenMadOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenMadOp(Code *fCode, Vector *r, Vector *a, Vector *b, Vector *c, int fmask)
{
    int NegateAB = lIsNegated(a) ^ lIsNegated(b), NegateC = lIsNegated(c);

    if (fmask & DST_X_BIT) {
        lGenLoad(fCode, &a->x, FALSE, FALSE);
        lGenMul(fCode, &b->x);
        lGenAddVariant(fCode, &c->x, NegateAB, NegateC);
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        lGenLoad(fCode, &a->y, FALSE, FALSE);
        lGenMul(fCode, &b->y);
        lGenAddVariant(fCode, &c->y, NegateAB, NegateC);
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        lGenLoad(fCode, &a->z, FALSE, FALSE);
        lGenMul(fCode, &b->z);
        lGenAddVariant(fCode, &c->z, NegateAB, NegateC);
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        lGenLoad(fCode, &a->w, FALSE, FALSE);
        lGenMul(fCode, &b->w);
        lGenAddVariant(fCode, &c->w, NegateAB, NegateC);
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
} // lGenMadOp

#if defined(NV_INTERNAL_OPCODES)

/*
** lGenRccOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenRccOp(Code *fCode, Expr *r, Expr *a)
{
    Expr e;

    lGenLoad(fCode, a, FALSE, TRUE);
    lGenFloatOne(fCode->environment, &e);
    lGenRccr(fCode, &e);
    r->kind = E_FREG;
} // lGenRccOp

#endif // NV_INTERNAL_OPCODES

/*
** lGenRcpOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenRcpOp(Code *fCode, Expr *r, Expr *a)
{
    Expr e;

    lGenLoad(fCode, a, FALSE, TRUE);
    lGenFloatOne(fCode->environment, &e);
    lGenDivr(fCode, &e);
    r->kind = E_FREG;
} // lGenRcpOp

/*
** lGenRsqOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenRsqOp(Code *fCode, Expr *r, Expr *a)
{
    Expr e;

    lGenLoad(fCode, a, FALSE, TRUE);
    lGenAbsSqrt(fCode);
    lGenFloatOne(fCode->environment, &e);
    lGenDivr(fCode, &e);
    r->kind = E_FREG;
} // lGenRsqOp

/*
** lGenLogOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenLogOp(Code *fCode, Vector *r, Expr *a, int fmask)
{
    X86_CODE_DECLARE;
    int zero, done;

    // Load x into EAX:

    lGenLoadIReg(fCode, a, FALSE);
    X86_CODE_LOAD(fCode);

    assert(LOCAL_VAR_SIZE >= 4);

    // Mask off sign bit:

    xAND_rm_imm(rmREG(rEAX), 0x7fffffff);       //   and  eax, 0x7fffffff
    xLABEL(zero);
    xJZ(0);                                     //   jz   zero

    // tr.x = ((VIEW_AS_INT(x) & 0x7f800000) >> 23) - F_EXP_BIAS;

    if (fmask & DST_X_BIT) {
        xMOV_r_rm(rEDX, rmREG(rEAX));           //   mov  edx, eax
        xSHR_rm_imm8(rmREG(rEDX), 23);          //   shr  edx, 23
        xSUB_rm_imm8(rmREG(rEDX), F_EXP_BIAS);  //   sub  edx, F_EXP_BIAS
        xMOV_rm_r(rmSIB, rEDX) xSIB_esp;        //   mov  [esp], edx
        xFILD_rm(rmSIB) xSIB_esp;               //   fild [esp]
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }

    // VIEW_AS_INT(tr.y) = (VIEW_AS_INT(x) & 0x007fffff) | 0x3f800000;

    if (fmask & DST_Y_BIT) {
        xMOV_r_rm(rEDX, rmREG(rEAX));           //   mov  edx, eax
        xAND_rm_imm(rmREG(rEDX), 0x007fffff);   //   and  edx, 0x007fffff
        xOR_rm_imm(rmREG(rEDX), 0x3f800000);    //   or   edx, 0x3f800000
        xMOV_rm_r(rmSIB, rEDX) xSIB_esp;        //   mov  [esp], edx
        xFLD_rm(rmSIB) xSIB_esp;                //   fld  [esp]
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }

    // tr.z = __GL_LOGF(x)*LOG_E_BASE_2;

    if (fmask & DST_Z_BIT) {
        xMOV_rm_r(rmSIB, rEAX) xSIB_esp;        //   mov  [esp], eax
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        xFLD_rm(rmSIB) xSIB_esp;                //   fld  [esp]
        xFYL2X();                               //   fyl2x
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    xLABEL(done);
    xJMP8(0);                                   //   jmp  done
    xTARGET_b8(zero);                           // zero:

    // tr.x = F_NEG_INFINITY;
                
    if (fmask & (DST_X_BIT | DST_Z_BIT)) {
        xMOV_rm_imm(rmREG(rEAX), FP_BITS_LOG0); //   mov  eax, 0xff800000
        xMOV_rm_r(rmSIB, rEAX) xSIB_esp;        //   mov  [esp], eax
    }

    if (fmask & DST_X_BIT) {
        xFLD_rm(rmSIB) xSIB_esp;                //   fld  [esp]
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }

    // tr.y = 1.0f;
                
    if (fmask & DST_Y_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }

    // tr.z = F_NEG_INFINITY;

    if (fmask & DST_Z_BIT) {
        xFLD_rm(rmSIB) xSIB_esp;                //   fld  [esp]
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }

    xTARGET_b8(done);                           // done:

    // tr.w = 1.0f;

    if (fmask & DST_W_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }

    X86_CODE_RESTORE(fCode);
} // lGenLogOp

/*
** lGenExpOp() - Simple version:  Assumes all args in memory, leaves result on TOS.
*/

static void lGenExpOp(Code *fCode, Vector *r, Expr *a, int fmask)
{
    X86_CODE_DECLARE;
    int nottoobig, computeit, computeit2, done_a, done_b;
    float f;

    // Load x into EAX:

    lGenLoadIReg(fCode, a, FALSE);
    X86_CODE_LOAD(fCode);

    assert(LOCAL_VAR_SIZE >= 12);

    // if (floor(x) > FLT_MAX_EXP)

    f = FLT_MAX_EXP + 1;
    xCMP_rm_imm(rmREG(rEAX), VIEW_AS_INT(f));       // cmp  eax, 0xXXXXXXXX
    xLABEL(nottoobig);
    xJL(0);                                         //   jl   nottoobig

    // tr.x = F_POS_INFINITY;
    // tr.y = 0.0f;
    // tr.z = F_POS_INFINITY;
    // tr.w = 1.0f;

    if (fmask & (DST_X_BIT | DST_Z_BIT)) {
        xMOV_rm_imm(rmREG(rEAX), 0x7f800000);       //   mov  eax, 0x7f800000
        xMOV_rm_r(rmSIB, rEAX) xSIB_esp;            //   mov  [esp], eax
    }
    if (fmask & (DST_Y_BIT)) {
        xMOV_rm_imm(rmREG(rEAX), 0x00000000);       //   mov  eax, 0x00000000
        xMOV_rm_r(rmSIB8, rEAX) xSIB8_esp(4);       //   mov  4[esp], eax
    }
    if (fmask & DST_X_BIT) {
        xFLD_rm(rmSIB) xSIB_esp;                    //   fld  [esp]
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        xFLD_rm(rmSIB8) xSIB8_esp(4);               //   fld  4[esp]
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        xFLD_rm(rmSIB) xSIB_esp;                   //   fld  [esp]
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }

    xLABEL(done_a);
    xJMP(0);                                        //   jmp  done;  Can be bigger that 128 bytes
    xTARGET_b8(nottoobig);                          // nottoobig:

    // if (x >= 0) goto computeit

    xTEST_rm_imm(rmREG(rEAX), 0x80000000);          //   tst  eax, 0x80000000
    xLABEL(computeit);
    xJZ(0);                                         //   jz   computeit

    // if (floor(x) < FLT_MIN_EXP)

    xMOV_rm_r(rmREG(rEDX), rEAX);                   //   mov  edx, eax
    xAND_rm_imm(rmREG(rEDX), 0x7fffffff);           //   and  edx, 0x7fffffff
    f = FLT_MIN_EXP;
    VIEW_AS_INT(f) = VIEW_AS_INT(f) & 0x7fffffff;
    xCMP_rm_imm(rmREG(rEDX), VIEW_AS_INT(f));       // cmp  eax, 0xXXXXXXXX
    xLABEL(computeit2);
    xJL(0);                                         //   jl   computeit

    // X is too small:  X < FLT_MIN_EXP
    //
    // tr.x = tr.y = tr.z = tr.w = { 0.0f, floor(x), 0.0f, 1.0f };

    if (fmask & DST_X_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_zero_offset); //   fld   gc->float_zero
        r->x.kind = E_FREG;
    } else {
        r->x.kind = E_NONE;
    }
    if (fmask & DST_Y_BIT) {
        xMOV_rm_r(rmSIB8, rEAX) xSIB8_esp(8);           //   mov   8[esp], x
        xFLD_rm(rmSIB8) xSIB8_esp(8);                   //   fld   8[esp]      ; TOS = x
        X86_CODE_RESTORE(fCode);
        lGenIFloor(fCode, rEAX, 0);                     //   [esp] = floor(x), [tos] = x
        X86_CODE_LOAD(fCode);
        xFILD_rm(rmSIB) xSIB_esp;                       //   fld   [esp];  x, floor(x)
        xFSUBP_st(1);                                   //   fsubp [tos - 1];  x - floor(x)
        r->y.kind = E_FREG;
    } else {
        r->y.kind = E_NONE;
    }
    if (fmask & DST_Z_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_zero_offset); //   fld   gc->float_zero
        r->z.kind = E_FREG;
    } else {
        r->z.kind = E_NONE;
    }
    if (fmask & DST_W_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }

    xLABEL(done_b);
    xJMP8(0);                                       //   jmp  done
    xTARGET_b8(computeit);                          // computeit:
    xTARGET_b8(computeit2);                         // computeit2:

    // f = floor(x);
    // ii = f;
    // ii = (ii << 23) + 0x3f800000;
    // tr.x = __GL_VIEW_AS_FLOAT(ii);
    // tr.y = x - f;
    // tr.z = tr.x*__GL_EXPF(tr.y*LOG_2_BASE_E);
    // tr.w = 1.0f;

    xMOV_rm_r(rmSIB8, rEAX) xSIB8_esp(8);           //   mov  8[esp], x
    xFLD_rm(rmSIB8) xSIB8_esp(8);                   //   fld  8[esp]      ; TOS = x
    X86_CODE_RESTORE(fCode);

    lGenIFloor(fCode, rEAX, 1);                     //   [esp] = ifloor(TOS)

    X86_CODE_LOAD(fCode);
    xMOV_r_rm(rEAX, rmSIB) xSIB_esp;                //   mov  eax, [esp]
    xSHL_rm_imm8(rmREG(rEAX), 23);                  //   shl  eax, 23
    xADD_rm_imm(rmREG(rEAX), 0x3f800000);           //   or   eax, 0x3f800000
    xMOV_rm_r(rmSIB8, rEAX) xSIB8_esp(4);           //   mov  4[esp], eax; float 2**ii

    xFLD_rm(rmSIB8) xSIB8_esp(4);                   //   fld  4[esp];  X: 2.0**ii
    xFILD_rm(rmSIB) xSIB_esp;                       //   fild [esp];      floor(x)
    xFSUBR_rm(rmSIB8) xSIB8_esp(8);                 //   fsubr 8[esp]; Y: x - floor(x)
    xFLD_st(0);                                     //   fld  st(0);   Z: x - floor(x)
    xF2XM1();                                       //   fyl2x;           2**(x - floor(x)) - 1.0
    xFADD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); // fadd  1,0f;      2**(x - floor(x))
    xFMUL_st(2);                                    //   fmul  st(2);  Z: (2**ii)*(2**(x - floor(x)))

    if (fmask & DST_X_BIT) {
        r->x.kind = E_FREG;
    } else {
        xFXCH_st(2);                                //   fxch st(2)
        xFSTP_st(0);                                //   fstp st(0); Pop
        xFXCH_st(1);                                //   fxch st(1)
        r->x.kind = E_NONE;
    }

    if (fmask & DST_Y_BIT) {
        r->y.kind = E_FREG;
    } else {
        xFXCH_st(1);                                //   fxch st(1)
        xFSTP_st(0);                                //   fstp st(0); Pop
        r->y.kind = E_NONE;
    }
    
    if (fmask & DST_Z_BIT) {
        r->z.kind = E_FREG;
    } else {
        xFSTP_st(0);                                //   fstp st(0); Pop
        r->z.kind = E_NONE;
    }

    if (fmask & DST_W_BIT) {
        xFLD_rm(rmIND32(rEBX)) xOFS32(fCode->environment->float_one_offset); //   fld   gc->float_one
        r->w.kind = E_FREG;
    } else {
        r->w.kind = E_NONE;
    }
    
    xTARGET_jmp(done_a);                            // done_a:
    xTARGET_b8(done_b);                             // done_b:

    X86_CODE_RESTORE(fCode);
} // lGenExpOp

/*
** lInitLiveRegs() - Initialize:
**         1) temp registers that are read before written to 0,0,0,0
**         2) output registers that aren't written to 0,0,0,1
*/

static void lInitLiveRegs(Code *fCode, ParsedProgram *parsed)
{
    X86_CODE_DECLARE;
    unsigned int ii, bits, addr, mask, maskbar;
    int HaveZero = 0, HaveOne = 0;

    X86_CODE_LOAD(fCode);

    // Clear A0 register:

    xXOR_rm_r(rmREG(rESI), rESI);       // xor esi, esi

    // Clear temp registers that are read before they are written to 0:

    bits = parsed->liveOnEntry & ((1 << __GL_NUMBER_OF_TEMPORARY_REGISTERS) - 1);
    if (bits) {
        HaveZero = 1;
        xXOR_r_rm(rEAX, rmREG(rEAX));
        for (ii = 0; bits; ii++, bits >>= 1) {
            if (bits & 1) {
                addr = ii*sizeof(float)*4 + fCode->environment->temp_offset;
                xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr);                   // mov  offset[gc], eax
                xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr + sizeof(float));   // mov  offset[gc], eax
                xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr + sizeof(float)*2); // mov  offset[gc], eax
                xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr + sizeof(float)*3); // mov  offset[gc], eax
            }
        }
    }

    // Set unset parts of result registers to 0/0/0/1:

    for (ii = 0; ii < __GL_NUMBER_OF_RESULT_REGISTERS; ii++) {
        if (parsed->resultRegsWritten[ii] != DST_XYZW_BITS) {

            // Only fill in partially written result regs:

            mask = (parsed->resultRegsWritten[ii] != DST_NONE_BITS);
            if (mask) {
                maskbar = DST_XYZW_BITS & ~mask;
                if ((maskbar & DST_XYZ_BITS) && !HaveZero) {
                    HaveZero = 1;
                    xXOR_r_rm(rEAX, rmREG(rEAX));
                }
                if ((maskbar & DST_W_BIT) && !HaveOne) {
                    HaveOne = 1;
                    xMOV_rm_imm(rmREG(rECX), 0x3f800000);
                }
                addr = ii*sizeof(float)*4 + fCode->environment->result_offset;
                if (maskbar & DST_X_BIT) {
                    xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr);                    // mov  offset[gc], eax
                }
                if (maskbar & DST_Y_BIT) {
                    xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr + sizeof(float));    // mov  offset[gc], eax
                }
                if (maskbar & DST_Z_BIT) {
                    xMOV_rm_r(rmIND32(rEBX), rEAX) xOFS32(addr + sizeof(float)*2);  // mov  offset[gc], eax
                }
                if (maskbar & DST_W_BIT) {
                    xMOV_rm_r(rmIND32(rEBX), rECX) xOFS32(addr + sizeof(float)*3);  // mov  offset[gc], ecx
                }
            }
        }
    }

    X86_CODE_RESTORE(fCode);
} // lInitLiveRegs

/*
** lGrowCode() - Increase the generated code buffer.
*/

static int lGrowCode(Code *fCode, int fSize)
{
    unsigned char *newData;
    unsigned int ii;

    newData = (unsigned char *) fCode->environment->malloc(fCode->environment->mallocArg, fSize);
    if (!newData)
        return FALSE;
    for (ii = 0; ii < fCode->ilcCount; ii++)
        newData[ii] = fCode->ilcData[ii];
    fCode->environment->free(fCode->environment->mallocArg, fCode->ilcData);
    fCode->ilcData = newData;
    fCode->ilcMax = fSize;
    return TRUE;
} // lGrowCode

/*
** vp_CompileX86() - Compile a vertex program into X86 instructions.
*/

int vp_CompileX86(VtxProgCompileX86 *environment, ParsedProgram *parsed,
                int numInstructions, VertexProgramOutput *outArgs)
{
    unsigned char *theCode = NULL;
    unsigned int  ilcCount, ilcMax;
    unsigned char *ilcData;
    Code lCode;

    Instruction *inst;
    Arl_Instruction *ainst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;
    Vector a, b, c, r;
    Expr e;
    int lop, lsize, pc;
    unsigned int ii;

    ilcCount = 0;
    ilcMax = 16385;
    ilcData = (unsigned char *) environment->malloc(environment->mallocArg, ilcMax);
    if (!ilcData)
        return FALSE;

    VtxProgImports.logf = environment->logf;
    VtxProgImports.expf = environment->expf;

    xPUSH_r(rEBP);
    xMOV_r_rm(rEBP, rmREG(rESP));
    //xPUSH_r(rEAX);
    xPUSH_r(rEBX);
    //xPUSH_r(rECX);
    //xPUSH_r(rEDX);
    xPUSH_r(rESI);          // Holds address register X 16
    xPUSH_r(rEDI);

    xMOV_r_rm(rEBX, rmIND8(rEBP)) xOFS8(8)          // mov  ebx, gc

    xADD_rm_imm8(rmREG(rESP), -LOCAL_VAR_SIZE);     // Alloc local stack space

    X86_CODE_RESTORE(&lCode);
    lCode.environment = environment;

    lInitLiveRegs(&lCode, parsed);

    inst = parsed->firstInstruction;
    for (pc = 0; pc < numInstructions; inst++, pc++) {

        // Make sure there's room for the next instruction:

        if (lCode.ilcCount + 200 >= lCode.ilcMax) {
            if (!lGrowCode(&lCode, ((lCode.ilcMax*3 >> 1) + 15) & ~15)) {
                environment->free(environment->mallocArg, lCode.ilcData);
                return FALSE;
            }
        }

        lGenNop(&lCode); // Mark instructions
        lop = inst->end_op.opcode;
        switch (lop) {

        // ARL op:

        case OP_ARL:
            ainst = (Arl_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &ainst->src);
            lGenAlrOp(&lCode, &a.x);
            break;

        // SCALAR ops:

        case OP_EXP:
            sinst = (Scalar_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &sinst->src);
            lGenExpOp(&lCode, &r, &a.x, sinst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &sinst->dst, &r);
            break;

        case OP_LOG:
            sinst = (Scalar_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &sinst->src);
            lGenLogOp(&lCode, &r, &a.x, sinst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &sinst->dst, &r);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_RCC:
            sinst = (Scalar_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &sinst->src);
            lGenRccOp(&lCode, &e, &a.x);
            lGenStoreMaskedDstRegScalar(&lCode, &sinst->dst, &e);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_RCP:
            sinst = (Scalar_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &sinst->src);
            lGenRcpOp(&lCode, &e, &a.x);
            lGenStoreMaskedDstRegScalar(&lCode, &sinst->dst, &e);
            break;

        case OP_RSQ:
            sinst = (Scalar_Instruction *) inst;
            lGenScalarSrcReg(environment, &a.x, &sinst->src);
            lGenRsqOp(&lCode, &e, &a.x);
            lGenStoreMaskedDstRegScalar(&lCode, &sinst->dst, &e);
            break;

        // VECTOR ops:

        case OP_LIT:
            vinst = (Vector_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &vinst->src);
            lGenLitOp(&lCode, &r, &a, vinst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &vinst->dst, &r);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_IMV:
#endif // NV_INTERNAL_OPCODES
        case OP_MOV:
            vinst = (Vector_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &vinst->src);
            lGenStoreMaskedDstReg(&lCode, &vinst->dst, &a);
            break;

        // BIN ops:

        case OP_ADD:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenAddOp(&lCode, &r, &a, &b, binst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        case OP_DP3:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenDotProductOp(&lCode, &e, &a, &b, 3);
            lGenStoreMaskedDstRegScalar(&lCode, &binst->dst, &e);
            break;

        case OP_DP4:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenDotProductOp(&lCode, &e, &a, &b, 4);
            lGenStoreMaskedDstRegScalar(&lCode, &binst->dst, &e);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_DPH:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenDotProductOp(&lCode, &e, &a, &b, 0);
            lGenStoreMaskedDstRegScalar(&lCode, &binst->dst, &e);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_DST:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenDstOp(&lCode, &r, &a, &b, binst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        case OP_MAX:
        case OP_MIN:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenMinMaxOp(&lCode, &r, &a, &b, binst->dst.mask, lop == OP_MAX);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        case OP_MUL:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenMulOp(&lCode, &r, &a, &b, binst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        case OP_SGE:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenSgeOp(&lCode, &r, &a, &b, binst->dst.mask, FALSE);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        case OP_SLT:
            binst = (Bin_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &binst->srcA);
            lGenSwizzleSrcReg(environment, &b, &binst->srcB);
            lGenSgeOp(&lCode, &r, &a, &b, binst->dst.mask, TRUE);
            lGenStoreMaskedDstReg(&lCode, &binst->dst, &r);
            break;

        // TRIops:

        case OP_MAD:
            tinst = (Tri_Instruction *) inst;
            lGenSwizzleSrcReg(environment, &a, &tinst->srcA);
            lGenSwizzleSrcReg(environment, &b, &tinst->srcB);
            lGenSwizzleSrcReg(environment, &c, &tinst->srcC);
            lGenMadOp(&lCode, &r, &a, &b, &c, tinst->dst.mask);
            lGenStoreMaskedDstReg(&lCode, &tinst->dst, &r);
            break;

        // ENDops:

        case OP_END:
            break;

        default:
            break;
        }
    }

    X86_CODE_LOAD(&lCode);

    xADD_rm_imm8(rmREG(rESP), LOCAL_VAR_SIZE);  // Free local stack space

    xPOP_r(rEDI)
    xPOP_r(rESI)
    //xPOP_r(rEDX)
    //xPOP_r(rECX)
    xPOP_r(rEBX)
    //xPOP_r(rEAX)

    xPOP_r(rEBP)
    xRET

    lsize = (ilcCount + 15) & ~15;
    theCode = (unsigned char *) environment->exec_malloc(environment->mallocArg, lsize);
    outArgs->residentSize = __internal_setup(lsize & 15); // so linker won't delete watermark
    if (theCode) {
        for (ii = 0; ii < ilcCount; ii++)
            theCode[ii] = ilcData[ii];
        outArgs->residentProgram = theCode;
        outArgs->residentSize = lsize;
    } else {
        outArgs->residentProgram = NULL;
        outArgs->residentSize = 0;
    }
    environment->free(environment->mallocArg, ilcData);

    return outArgs->residentProgram != NULL;
} // vp_CompileX86

/*
** vp_OutputCelsiusX86() - Compile a vertex program into X86 instructions.
*/

// Macros copied from nv32.h, nvhw.h, etc. so they don't have to be included by D3D:

#define __NV_COUNT_SHIFT                18
#define __NV_SUBCHANNEL_SHIFT           13
#define __NV_METHOD_SHIFT               0
#define __NV_METHOD(_SC,_M,_NUM)        (((_NUM) << __NV_COUNT_SHIFT) | ((_SC) << __NV_SUBCHANNEL_SHIFT) | (_M))

#define __NV056_SET_VERTEX4F(i)         (0x00000c18+(i)*4)
#define __NV056_SET_DIFFUSE_COLOR4F(i)  (0x00000c50+(i)*4)
#define __NV056_SET_DIFFUSE_COLOR3F(i)  (0x00000c60+(i)*4)
#define __NV056_SET_SPECULAR_COLOR4F(i) (0x00000c70+(i)*4)
#define __NV056_SET_SPECULAR_COLOR3F(i) (0x00000c80+(i)*4)
#define __NV056_SET_TEXCOORD0_2F(i)     (0x00000c90+(i)*4)
#define __NV056_SET_TEXCOORD0_4F(i)     (0x00000ca0+(i)*4)
#define __NV056_SET_TEXCOORD1_2F(i)     (0x00000cb8+(i)*4)
#define __NV056_SET_TEXCOORD1_4F(i)     (0x00000cc8+(i)*4)

#define C_METHOD(e, _M,_NUM)            __NV_METHOD((e)->channel_number, _M, _NUM)

#define OUT_OFFSET(n, k, e)             ((((n)*4) + (k))*sizeof(float) + (e)->result_offset)

int vp_OutputCelsiusX86(VtxProgCompileX86 *environment, ParsedProgram *parsed,
                        int numInstructions, VertexProgramOutput *outArgs)
{
    unsigned char *theCode = NULL;
    unsigned int  ilcCount, ilcMax;
    unsigned char *ilcData;
    Code lCode;

    unsigned int ii;
    int offset, lsize, mask, is_cube_map;

    ilcCount = 0;
    ilcMax = 1024;
    ilcData = (unsigned char *) environment->malloc(environment->mallocArg, ilcMax);
    if (!ilcData)
        return FALSE;
    offset = 0;
    lCode.environment = environment;

    xPUSH_r(rEBP);
    xMOV_r_rm(rEBP, rmREG(rESP));
    //xPUSH_r(rEAX);
    xPUSH_r(rEBX);
    //xPUSH_r(rECX);
    //xPUSH_r(rEDX);
    //xPUSH_r(rESI);          // Holds address register X 16
    xPUSH_r(rEDI);      // Points to output area

    xMOV_r_rm(rEBX, rmIND8(rEBP)) xOFS8(8)          // mov  ebx, gc
    xMOV_r_rm(rEDI, rmIND8(rEBP)) xOFS8(12)         // mov  edi, nvCurrent

    xADD_rm_imm8(rmREG(rESP), -LOCAL_VAR_SIZE);     // Alloc local stack space

    // Output Color:

    mask = parsed->resultRegsWritten[__GL_ATTRIB_OUTPUT_COL0];
    if (mask) {
        if (mask & 0x08) {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_DIFFUSE_COLOR4F(0),4)); // mov  eax, #SET_DIFFUSE_COLOR4F
        } else {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_DIFFUSE_COLOR3F(0),3)); // mov  eax, #SET_DIFFUSE_COLOR3F
        }
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4);                         // mov  nvCurrent[0], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(3,0,environment));      // mov  eax, o[COL0].x
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+4);                       // mov  nvCurrent[1], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(3,1,environment));      // mov  eax, o[COL0].y
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+8);                       // mov  nvCurrent[2], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(3,2,environment));      // mov  eax, o[COL0].z
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+12);                      // mov  nvCurrent[3], eax
        if (mask & 0x08) {
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(3,3,environment));  // mov  eax, o[COL0].w
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+16);                  // mov  nvCurrent[4], eax
            offset += 5;
        } else {
            offset += 4;
        }
    }

    // Output Secondary Color:

    mask = parsed->resultRegsWritten[__GL_ATTRIB_OUTPUT_COL1];
    if (mask) {
        if (mask & 0x08) {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_SPECULAR_COLOR4F(0),4)); // mov  eax, #SET_SPECULAR_COLOR4F
        } else {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_SPECULAR_COLOR3F(0),3)); // mov  eax, #SET_SPECULAR_COLOR3F
        }
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4);                         // mov  nvCurrent[0], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(4,0,environment));      // mov  eax, o[COL1].x
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+4);                       // mov  nvCurrent[1], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(4,1,environment));      // mov  eax, o[COL1].y
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+8);                       // mov  nvCurrent[2], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(4,2,environment));      // mov  eax, o[COL1].z
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+12);                      // mov  nvCurrent[3], eax
        if (mask & 0x08) {
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(4,3,environment));  // mov  eax, o[COL1].w
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+16);                  // mov  nvCurrent[4], eax
            offset += 5;
        } else {
            offset += 4;
        }
    }

    // Output Texture 0:

    mask = parsed->resultRegsWritten[__GL_ATTRIB_OUTPUT_TEX0];
    if (mask) {
        if (mask & 0x0c) {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_TEXCOORD0_4F(0),4));    // mov  eax, #SET_TEXCOORD0_4F
        } else {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_TEXCOORD0_2F(0),2));    // mov  eax, #SET_TEXCOORD0_2F
        }
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4);                         // mov  nvCurrent[0], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(9,0,environment));      // mov  eax, o[TEX0].x
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+4);                       // mov  nvCurrent[1], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(9,1,environment));      // mov  eax, o[TEX0].y
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+8);                       // mov  nvCurrent[2], eax
        if (mask & 0x0c) {
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(9,2,environment));  // mov  eax, o[TEX0].z
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+12);                  // mov  nvCurrent[3], eax
            // See if cube maps are enabled on texture 0:
            xTEST_rm_imm(rmIND32(rEBX), environment->enables_offset);
                         xOFS32(__GL_PROGRAM_ENABLE_TEXTURE0); // Fields are BACKWARDS in macro!!!
            xLABEL(is_cube_map);
            xJNZ(0);                                                             // jnz  is_cube_map
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(9,3,environment));  // mov  eax, o[TEX0].w
            xTARGET_b8(is_cube_map);                                             // is_cube_map:
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+16);                  // mov  nvCurrent[4], eax
            offset += 5;
        } else {
            offset += 3;
        }
    }

    // Output Texture 1:

    mask = parsed->resultRegsWritten[__GL_ATTRIB_OUTPUT_TEX1];
    if (mask) {
        if (mask & 0x0c) {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_TEXCOORD1_4F(0),4));    // mov  eax, #SET_TEXCOORD1_4F
        } else {
            xMOV_rm_imm(rmREG(rEAX),
                        C_METHOD(environment,__NV056_SET_TEXCOORD1_2F(0),2));    // mov  eax, #SET_TEXCOORD1_2F
        }
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4);                         // mov  nvCurrent[0], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(10,0,environment));     // mov  eax, o[TEX1].x
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+4);                       // mov  nvCurrent[1], eax
        xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(10,1,environment));     // mov  eax, o[TEX1].y
        xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+8);                       // mov  nvCurrent[2], eax
        if (mask & 0x0c) {
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(10,2,environment)); // mov  eax, o[TEX1].z
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+12);                  // mov  nvCurrent[3], eax
            // See if cube maps are enabled on texture 1:
            xTEST_rm_imm(rmIND32(rEBX), environment->enables_offset);
                         xOFS32(__GL_PROGRAM_ENABLE_TEXTURE1); // Fields are BACKWARDS in macro!!!
            xLABEL(is_cube_map);
            xJNZ(0);                                                             // jnz  is_cube_map
            xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(10,3,environment)); // mov  eax, o[TEX1].w
            xTARGET_b8(is_cube_map);                                             // is_cube_map:
            xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+16);                  // mov  nvCurrent[4], eax
            offset += 5;
        } else {
            offset += 3;
        }
    }

    // Output vertex data:

#if 000
    // invW = 1.0f/pRegs->result[0].w;
    // NV_CELSIUS_METHOD_4F(NV056_SET_VERTEX4F(0),
    //                      pRegs->result[__GL_ATTRIB_OUTPUT_HPOS].x*vp->xScale*invW + vp->xCenter - xAdjust,
    //                      pRegs->result[__GL_ATTRIB_OUTPUT_HPOS].y*vp->yScale*invW + vp->yCenter - yAdjust,
    //                      pRegs->result[__GL_ATTRIB_OUTPUT_HPOS].z*vp->zScale*invW + vp->zCenter,
    //                     invW);
    
    xFLD_rm(rmIND32(rEBX)) xOFS32(environment->float_one_offset);     // fld   1.0f
    xFDIV_rm(rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,3,environment));      // fdiv  o[HPOS].w
    xFLD_rm(rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,0,environment));       // fld   o[HPOS].x
    xFLD_rm(rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,1,environment));       // fld   o[HPOS].y
    xFLD_rm(rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,2,environment));       // fld   o[HPOS].z
    xFLD_rm(rmIND32(rEBX)) xOFS32(environment->vp_scale_offset);      // fld   vpScale.x
    xFLD_rm(rmIND32(rEBX)) xOFS32(environment->vp_scale_offset+4);    // fld   vpScale.y
    xFLD_rm(rmIND32(rEBX)) xOFS32(environment->vp_scale_offset+8);    // fld   vpScale.z  ; Sz  Sy  Sx  Oz  Oy  Ox  1/Ow
    xFMULP_st(3);                                                     // fmulp st(3)      ; Sy  Sx  Oz*Sz  Oy  Ox  1/Ow
    xFMULP_st(3);                                                     // fmulp st(3)      ; Sx  Oz*Sz  Oy*Sy  Ox  1/Ow
    xFMULP_st(3);                                                     // fmulp st(3)      ; Oz*Sz  Oy*Sy  Ox*Sx  1/Ow
    xFMULP_st(3);                                                     // fmul  st(3)      ; Oz*Sz/Ow  Oy*Sy  Ox*Sx  1/Ow
    xFADD_rm(rmIND32(rEBX)) xOFS32(environment->vp_center_offset+8);  // fadd  vpCenter.z ; Oz*Sz/Ow+Cz  Oy*Sy  Ox*Sx  1/Ow
    xFXCH_st(2);                                                      // fxch  st(2)      ; Ox*Sx  Oy*Sy  Oz*Sz/Ow+Cz  1/Ow
    xFMULP_st(3);                                                     // fmul  st(3)      ; Ox*Sx/Ow  Oy*Sy  Oz*Sz/Ow+Cz  1/Ow
    xFADD_rm(rmIND32(rEBX)) xOFS32(environment->vp_center_offset);    // fadd  vpCenter.x ; Ox*Sx/Ow+Cx  Oy*Sy  Zout  1/Ow
    xFADD_rm(rmIND32(rEBX)) xOFS32(environment->vp_adjust_offset);    // fsub  vpAdjust.x ; Ox*Sx/Ow+Cx-Ax  Oy*Sy  Zout  1/Ow
    xFXCH_st(1);                                                      // fxch  st(1)      ; Oy*Sy  Ox*Sx/Ow+Cx-Ax  Zout  1/Ow
    xFMULP_st(3);                                                     // fmul  st(3)      ; Oy*Sy/Ow  Xout  Zout  1/Ow
    xFADD_rm(rmIND32(rEBX)) xOFS32(environment->vp_center_offset+4);  // fadd  vpCenter.y ; Oy*Sy/Ow+Cy  Xout  Zout  1/Ow
    xFADD_rm(rmIND32(rEBX)) xOFS32(environment->vp_adjust_offset+4);  // fsub  vpAdjust.y ; Oy*Sy/Ow+Cy-Ay  Xout  Zout  1/Ow
    xFXCH_st(1);                                                      // fxch  st(1)      ; Xout  Yout  Zout  Wout
    xMOV_rm_imm(rmREG(rEAX), C_METHOD(__NV056_SET_VERTEX4F(0),4));    // mov   eax, #SET_VERTEX4F
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset);                    // mov   nvCurrent[0], eax
    xFSTP_rm(rmIND32(rEDI)) xOFS32(offset+4);                         // fstp  nvCurrent[1]
    xFSTP_rm(rmIND32(rEDI)) xOFS32(offset+8);                         // fstp  nvCurrent[2]
    xFSTP_rm(rmIND32(rEDI)) xOFS32(offset+12);                        // fstp  nvCurrent[3]
    xFSTP_rm(rmIND32(rEDI)) xOFS32(offset+16);                        // fstp  nvCurrent[4]
#endif
    xMOV_rm_imm(rmREG(rEAX),
                C_METHOD(environment,__NV056_SET_VERTEX4F(0),4));        // mov  eax, #SET_VERTEX4F
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4);                     // mov  nvCurrent[0], eax
    xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,0,environment));  // mov  eax, o[HPOS].x
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+4);                   // mov  nvCurrent[1], eax
    xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,1,environment));  // mov  eax, o[HPOS].y
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+8);                   // mov  nvCurrent[2], eax
    xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,2,environment));  // mov  eax, o[HPOS].z
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+12);                  // mov  nvCurrent[3], eax
    xMOV_r_rm(rEAX, rmIND32(rEBX)) xOFS32(OUT_OFFSET(0,3,environment));  // mov  eax, o[HPOS].w
    xMOV_rm_r(rmIND32(rEDI), rEAX) xOFS32(offset*4+16);                  // mov  nvCurrent[4], eax
    offset += 5;

    // Clean-up and go home:

    xADD_rm_imm8(rmREG(rESP), LOCAL_VAR_SIZE);  // Free local stack space

    xPOP_r(rEDI)
    //xPOP_r(rESI)
    //xPOP_r(rEDX)
    //xPOP_r(rECX)
    xPOP_r(rEBX)
    //xPOP_r(rEAX)

    xPOP_r(rEBP)
    xRET

    lsize = (ilcCount + 15) & ~15;
    theCode = (unsigned char *) environment->exec_malloc(environment->mallocArg, lsize);
    if (theCode) {
        for (ii = 0; ii < ilcCount; ii++)
            theCode[ii] = ilcData[ii];
        outArgs->residentProgram = theCode;
        outArgs->residentSize = lsize;
    } else {
        outArgs->residentProgram = NULL;
        outArgs->residentSize = 0;
        offset = 0;
    }
    environment->free(environment->mallocArg, ilcData);

    return offset;
} // vp_OutputCelsiusX86
