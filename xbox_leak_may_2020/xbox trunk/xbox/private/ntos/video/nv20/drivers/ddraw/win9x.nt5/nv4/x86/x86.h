#ifndef _x86_h
#define _x86_h
/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: x86.h                                                             *
*   x86 architexture specific macros - used mainly to generate assembly     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 02/10/98 - wrote it.                    *
*                                                                           *
\***************************************************************************/

/*****************************************************************************
 * constants
 *****************************************************************************/

/*
 * registers
 */
#define rEAX            0
#define rECX            1
#define rEDX            2
#define rEBX            3
#define rESP            4
#define rEBP            5
#define rESI            6
#define rEDI            7

#define rAX             0
#define rCX             1
#define rDX             2
#define rBX             3
#define rSP             4
#define rBP             5
#define rSI             6
#define rDI             7

#define rAL             0
#define rCL             1
#define rDL             2
#define rBL             3
#define rAH             4
#define rCH             5
#define rDH             6
#define rBH             7

#define rMM0            0
#define rMM1            1
#define rMM2            2
#define rMM3            3
#define rMM4            4
#define rMM5            5
#define rMM6            6
#define rMM7            7

#define rXMM0           0
#define rXMM1           1
#define rXMM2           2
#define rXMM3           3
#define rXMM4           4
#define rXMM5           5
#define rXMM6           6
#define rXMM7           7

#define rST0            0
#define rST1            1
#define rST2            2
#define rST3            3
#define rST4            4
#define rST5            5
#define rST6            6
#define rST7            7

/*
 * op codes
 *  alphabetically sorted
 */
#define xADD_r_rm(r,rm)         x86s(2) x86b(0x03) x86b(mREG(r) | (rm))
#define xADD_r_i32(r,a)         x86s(6) x86b(0x03) x86b(mREG(r) | mOFS32) x86d(a)
#define xADD_rm_imm8(rm,i)      x86s(3) x86b(0x83) x86b(mREG(0) | (rm)) x86b(i)
#define xADD_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(0) | (rm)) x86d(i)
#define xADD_rm_r(rm,r)         x86s(2) x86b(0x01) x86b(mREG(r) | (rm))
#define xADD_i32_r(a,r)         x86s(6) x86b(0x01) x86b(mREG(r) | mOFS32) x86d(a)

#define xAND_r_rm(r,rm)         x86s(2) x86b(0x23) x86b(mREG(r) | (rm))
#define xAND_r_i32(r,a)         x86s(6) x86b(0x23) x86b(mREG(r) | mOFS32) x86d(a)
#define xAND_rm_imm8(rm,i)      x86s(3) x86b(0x83) x86b(mREG(4) | (rm)) x86b(i)
#define xAND_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(4) | (rm)) x86d(i)

#define xCALL_rm(rm)            x86s(2) x86b(0xff) x86b(mREG(2) | (rm))

#define xCMP_r_rm(r,rm)         x86s(2) x86b(0x3b) x86b(mREG(r) | (rm))
#define xCMP_r_i32(r,a)         x86s(6) x86b(0x3b) x86b(mREG(r) | mOFS32) x86d(a)
#define xCMP_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(7) | (rm)) x86d(i)

#define xDEC_r(r)               x86s(1) x86b(0x48 | (r))
#define xDEC_rm(rm)             x86s(2) x86b(0xff) x86b(mREG(1) | (rm))
#define xDEC_i32(a)             x86s(6) x86b(0xff) x86b(mREG(1) | mOFS32) x86d(a)

#define xFADD_i32(a)            x86s(6) x86b(0xd8) x86b(mREG(0) | mOFS32) x86d(a)
#define xFADDP_st(r)            x86s(2) x86b(0xde) x86b(0xc0 + (r))

#define xFISTP_rm32(rm)         x86s(2) x86b(0xdb) x86b(mREG(3) | (rm))
#define xFISTP_i32(a)           x86s(6) x86b(0xdb) x86b(mREG(3) | mOFS32) x86d(a)

#define xFLD_rm32(rm)           x86s(2) x86b(0xd9) x86b(mREG(0) | (rm))
#define xFLD_i32(a)             x86s(6) x86b(0xd9) x86b(mREG(0) | mOFS32) x86d(a)
#define xFLD_st(r)              x86s(2) x86b(0xd9) x86b(0xc0 + (r))
#define xFLD1                   x86s(2) x86b(0xd9) x86b(0xe8)

#define xFMUL_st(r)             x86s(2) x86b(0xd8) x86b(0xc8 + (r))
#define xFMUL_rm32(rm)          x86s(2) x86b(0xd8) x86b(mREG(1) | (rm))
#define xFMUL_i32(a)            x86s(6) x86b(0xd8) x86b(mREG(1) | mOFS32) x86d(a)
#define xFMULP_st(r)            x86s(2) x86b(0xde) x86b(0xc8 + (r))

#define xFSUBP_st(r)            x86s(2) x86b(0xde) x86b(0xe8 + (r))
#define xFSUBRP_st(r)           x86s(2) x86b(0xde) x86b(0xe0 + (r))

#define xFSTP_rm(rm)            x86s(2) x86b(0xd9) x86b(mREG(3) | (rm))
#define xFSTP_i32(a)            x86s(6) x86b(0xd9) x86b(mREG(3) | mOFS32) x86d(a)

#define xFXCH_st(r)             x86s(2) x86b(0xd9) x86b(0xc8 + (r))

#define xINC_r(r)               x86s(1) x86b(0x40 | (r))
#define xINC_rm(rm)             x86s(2) x86b(0xff) x86b(mREG(0) | (rm))

#define xIMUL_r_rm(r,rm)        x86s(3) x86b(0x0f) x86b(0xaf) x86b(mREG(r) | (rm))
#define xIMUL_r_i32(r,a)        x86s(7) x86b(0x0f) x86b(0xaf) x86b(mREG(r) | mOFS32) x86d(a)

#define xINT3                   x86s(1) x86b(0xcc)

#define xJA(disp)               { DWORD _x = (disp)-(ilcCount+2); x86s(2) x86b(0x77) x86b(_x) }
#define xJA32(disp)             { DWORD _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x87) x86d(_x) }
#define xJL(disp)               { DWORD _x = (disp)-(ilcCount+2); x86s(2) x86b(0x7c) x86b(_x) }
#define xJL32(disp)             { DWORD _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x8c) x86d(_x) }
#define xJNZ(disp)              { DWORD _x = (disp)-(ilcCount+2); x86s(2) x86b(0x75) x86b(_x) }
#define xJNZ32(disp)            { DWORD _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x85) x86d(_x) }
#define xJZ(disp)               { DWORD _x = (disp)-(ilcCount+2); x86s(2) x86b(0x74) x86b(_x) }
#define xJZ32(disp)             { DWORD _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x84) x86d(_x) }

#define xJMP(disp)              { DWORD _x = (disp)-(ilcCount+5); x86s(5) x86b(0xe9) x86d(_x) }

#define xLEA_r_rm(r,rm)         x86s(2) x86b(0x8d) x86b(mREG(r) | (rm))

#define xMOV_r_rm(r,rm)         x86s(2) x86b(0x8b) x86b(mREG(r) | (rm))                     // mov eax,[ebx]
#define xMOV_r_i32(r,a)         x86s(6) x86b(0x8b) x86b(mREG(r) | mOFS32) x86d(a)           // mov eax,[1234h]
#define xMOV_rm_imm(rm,i)       x86s(6) x86b(0xc7) x86b(mREG(0) | (rm)) x86d(i)               // mov ebp,12345678
#define xMOV_rm_r(rm,r)         x86s(2) x86b(0x89) x86b(mREG(r) | (rm))                     // mov [ebx],eax
#define xMOV_i32_r(a,r)         x86s(6) x86b(0x89) x86b(mREG(r) | mOFS32) x86d(a)           // mov [1234h],eax
#define xMOV_r_rm8(r,rm)        x86s(2) x86b(0x8a) x86b(mREG(r) | (rm))                     // mov al,[ebx]
#define xMOV_rm8_r(rm,r)        x86s(2) x86b(0x88) x86b(mREG(r) | (rm))                     // mov [ebx],al

#define xNOP                    x86s(1) x86b(0x90)

#define xNOT_rm(rm)             x86s(2) x86b(0xf7) x86b(mREG(2) | (rm))

#define xOR_r_rm(r,rm)          x86s(2) x86b(0x0b) x86b(mREG(r) | (rm))
#define xOR_r_i32(r,a)          x86s(6) x86b(0x0b) x86b(mREG(r) | mOFS32) x86d(a)
#define xOR_rm_r(r,rm)          x86s(2) x86b(0x09) x86b(mREG(r) | (rm))
#define xOR_i32_r(a,r)          x86s(6) x86b(0x09) x86b(mREG(r) | mOFS32) x86d(a)
#define xOR_rm_imm(rm,i)        x86s(6) x86b(0x81) x86b(mREG(1) | (rm)) x86d(i)

#define xPOP_r(r)               x86s(1) x86b(0x58 | (r))
#define xPOP_rm(rm)             x86s(2) x86b(0xff) x86b(0x30 | (rm))
#define xPOPAD                  x86s(1) x86b(0x61)

#define xPUSH_r(r)              x86s(1) x86b(0x50 | (r))
#define xPUSH_rm(rm)            x86s(2) x86b(0xff) x86b(mREG(6) | (rm))
#define xPUSH_i32(a)            x86s(6) x86b(0xff) x86b(mREG(6) | mOFS32) x86d(a)
#define xPUSH_imm(i)            x86s(5) x86b(0x68) x86d(i)
#define xPUSHAD                 x86s(1) x86b(0x60)

#define xRET                    x86s(1) x86b(0xc3)

#define xSBB_r_rm(r,rm)         x86s(2) x86b(0x1b) x86b(mREG(r) | (rm))
#define xSBB_r_i32(r,a)         x86s(6) x86b(0x1b) x86b(mREG(r) | mOFS32) x86d(a)
#define xSBB_rm_r(rm,r)         x86s(2) x86b(0x19) x86b(mREG(r) | (rm))

#define xSETG_rm8(rm)           x86s(3) x86b(0x0f) x86b(0x9f) x86b(rm)
#define xSETGE_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x9d) x86b(rm)
#define xSETNZ_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x95) x86b(rm)
#define xSETZ_rm8(rm)           x86s(3) x86b(0x0f) x86b(0x94) x86b(rm)

#define xSHL_rm_imm(rm,i)       x86s(3) x86b(0xc1) x86b(mREG(4) | (rm)) x86b(i)

#define xSHR_rm_imm(rm,i)       x86s(3) x86b(0xc1) x86b(mREG(5) | (rm)) x86b(i)

#define xSUB_r_rm(r,rm)         x86s(2) x86b(0x2b) x86b(mREG(r) | (rm))
#define xSUB_r_i32(r,a)         x86s(2) x86b(0x2b) x86b(mREG(r) | mOFS32) x86d(a)
#define xSUB_rm_r(rm,r)         x86s(2) x86b(0x29) x86b(mREG(r) | (rm))
#define xSUB_i32_r(a,r)         x86s(2) x86b(0x29) x86b(mREG(r) | mOFS32) x86d(a)
#define xSUB_rm_imm8(rm,i)      x86s(3) x86b(0x83) x86b(mREG(5) | (rm)) x86b(i)
#define xSUB_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(5) | (rm)) x86d(i)

#define xTEST_rm_r(rm,r)        x86s(2) x86b(0x85) x86b(mREG(r) | (rm))
#define xTEST_i32_r(a,r)        x86s(6) x86b(0x85) x86b(mREG(r) | mOFS32) x86d(a)
#define xTEST_r_i32(r,a)        x86s(6) x86b(0x85) x86b(mREG(r) | mOFS32) x86d(a)
#define xTEST_rm_imm(rm,i)      x86s(6) x86b(0xf7) x86b(mREG(0) | (rm)) x86d(i)

#define xXOR_r_rm(r,rm)         x86s(2) x86b(0x33) x86b(mREG(r) | (rm))
#define xXOR_r_i32(r,a)         x86s(6) x86b(0x33) x86b(mREG(r) | mOFS32) x86d(a)
#define xXOR_rm_r(rm,r)         x86s(2) x86b(0x31) x86b(mREG(r) | (rm))
#define xXOR_i32_r(a,r)         x86s(6) x86b(0x31) x86b(mREG(r) | mOFS32) x86d(a)
#define xXOR_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(6) | (rm)) x86d(i)

/*
 * mmx opcodes
 */
#define xMOVQ_r_rm(r,rm)        x86s(3) x86b(0x0f) x86b(0x6f) x86b(mREG(r) | (rm))
#define xMOVQ_rm_r(rm,r)        x86s(3) x86b(0x0f) x86b(0x7f) x86b(mREG(r) | (rm))

#define xEMMS                   x86s(2) x86b(0x0f) x86b(0x77)

/*
 * katmai opcodes
 */
#define xMOVAPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x28) x86b(mREG(r) | (rm))
#define xMOVAPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x29) x86b(mREG(r) | (rm))

#define xMOVUPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x10) x86b(mREG(r) | (rm))
#define xMOVUPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x11) x86b(mREG(r) | (rm))

#define xMOVHPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x16) x86b(mREG(r) | (rm))
#define xMOVHPS_r_i64(r,a)      x86s(7) x86b(0x0f) x86b(0x16) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVHPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x17) x86b(mREG(r) | (rm))

#define xMOVLPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x12) x86b(mREG(r) | (rm))
#define xMOVLPS_r_i64(r,a)      x86s(7) x86b(0x0f) x86b(0x12) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVLPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x13) x86b(mREG(r) | (rm))
/*****************************************************************************
 * macros
 *****************************************************************************/

/*
 * emission
 */
#define x86s(x)                 if ((ilcCount+(x)) > ilcMax) ILCGrowTo(ilcCount+(x));
#define x86b(x)                 ilcData[ilcCount]=(BYTE)(x);              ilcCount++;
#define x86d(x)                 *(DWORD*)(ilcData+ilcCount) = (DWORD)(x); ilcCount+=4;

/*
 * mod r/m
 */
#define mOFS32                  rmIND(rEBP)
#define mREG(x)                 ((x) << 3)

#define rmIND(x)                (0x00 | (x))
#define rmIND8(x)               (0x40 | (x))
#define rmIND32(x)              (0x80 | (x))
#define rmREG(x)                (0xc0 | (x))
#define rmSIB                   rmIND(rESP)
#define rmSIB8                  rmIND8(rESP)
#define rmSIB32                 rmIND32(rESP)

#define x1                      0
#define x2                      1
#define x4                      2
#define x8                      3
#define xSIB(r1,r2,s)           x86s(1) x86b((r1) | ((r2)<<3) | ((s)<<6))

#define x16r                    x86s(1) x86b(0x66)
#define x16m                    x86s(1) x86b(0x67)

#define xDS                     x86s(1) x86b(0x3e)

#define xOFS8(e)                x86s(1) x86b(e)
#define xOFS32(e)               x86s(4) x86d(e)

/*****************************************************************************
 * externals
 *****************************************************************************/
void ILCGrowTo (DWORD dwNeed);

#endif //x86.h