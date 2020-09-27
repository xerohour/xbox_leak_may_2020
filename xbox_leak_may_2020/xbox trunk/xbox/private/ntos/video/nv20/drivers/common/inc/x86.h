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

#ifndef _x86_h
#define _x86_h

#ifdef __cplusplus
extern "C" {
#endif

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
#define xADC_rEAX_imm(i)        x86s(5) x86b(0x15) x86d(i)
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

#define xCMOVA_r_i32(r,a)       x86s(7) x86b(0x0f) x86b(0x47) x86b(mREG(r) | mOFS32) x86d(a)
#define xCMOVA_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x47) x86b(mREG(r) | (rm))
#define xCMOVB_r_i32(r,a)       x86s(7) x86b(0x0f) x86b(0x42) x86b(mREG(r) | mOFS32) x86d(a)
#define xCMOVB_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x42) x86b(mREG(r) | (rm))
#define xCMOVG_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x4f) x86b(mREG(r) | (rm))
#define xCMOVL_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x4c) x86b(mREG(r) | (rm))

#define xCMP_r_rm(r,rm)         x86s(2) x86b(0x3b) x86b(mREG(r) | (rm))
#define xCMP_r_i32(r,a)         x86s(6) x86b(0x3b) x86b(mREG(r) | mOFS32) x86d(a)
#define xCMP_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(7) | (rm)) x86d(i)

#define xDEC_r(r)               x86s(1) x86b(0x48 | (r))
#define xDEC_rm(rm)             x86s(2) x86b(0xff) x86b(mREG(1) | (rm))
#define xDEC_i32(a)             x86s(6) x86b(0xff) x86b(mREG(1) | mOFS32) x86d(a)

#define xDIV_rm(rm)             x86s(2) x86b(0xf7) x86b(mREG(6) | (rm))

#define xFABS()                 x86s(2) x86b(0xd9) x86b(0xe1)

#define xFADD_rm(rm)            x86s(2) x86b(0xd8) x86b(mREG(0) | (rm))
#define xFADD_i32(a)            x86s(6) x86b(0xd8) x86b(mREG(0) | mOFS32) x86d(a)
#define xFADD_st(r)             x86s(2) x86b(0xd8) x86b(0xc0 + (r))
#define xFADDP_st(r)            x86s(2) x86b(0xde) x86b(0xc0 + (r))

#define xFDECSTP                x86s(2) x86b(0xd9) x86b(0xf6)

#define xFCHS                   x86s(2) x86b(0xd9) x86b(0xe0)

#define xFCOM_rm(rm)            x86s(2) x86b(0xd8) x86b(mREG(2) | (rm))
#define xFCOM_i32(a)            x86s(6) x86b(0xd8) x86b(mREG(2) | mOFS32) x86d(a)
#define xFCOM_st(r)             x86s(2) x86b(0xd8) x86b(0xd0 + (r))
#define xFCOMI_st(r)            x86s(2) x86b(0xdb) x86b(0xf0 + (r))
#define xFCOMIP_st(r)           x86s(2) x86b(0xdf) x86b(0xf0 + (r))
#define xFCOMP_rm(rm)           x86s(2) x86b(0xd8) x86b(mREG(3) | (rm))
#define xFCOMPP()               x86s(2) x86b(0xde) x86b(0xd9)

#define xFDIV_rm(rm)            x86s(2) x86b(0xd8) x86b(mREG(6) | (rm))
#define xFDIVR_rm(rm)           x86s(2) x86b(0xd8) x86b(mREG(7) | (rm))

#define xFFREE_st(r)            x86s(2) x86b(0xdd) x86b(0xc0 + (r))

#define xFILD_rm(rm)            x86s(2) x86b(0xdb) x86b(mREG(0) | (rm))
#define xFILD_rm16(rm)          x86s(2) x86b(0xdf) x86b(mREG(0) | (rm))
#define xFILD_rm64(rm)          x86s(2) x86b(0xdf) x86b(mREG(5) | (rm))

#define xFINCSTP                x86s(2) x86b(0xd9) x86b(0xf7)

#define xFIST_rm(rm)            x86s(2) x86b(0xdb) x86b(mREG(2) | (rm))
#define xFISTP_rm(rm)           x86s(2) x86b(0xdb) x86b(mREG(3) | (rm))
#define xFISTP_i32(a)           x86s(6) x86b(0xdb) x86b(mREG(3) | mOFS32) x86d(a)

#define xFLD_rm(rm)             x86s(2) x86b(0xd9) x86b(mREG(0) | (rm))
#define xFLD_rm32(rm)           x86s(2) x86b(0xd9) x86b(mREG(0) | (rm))
#define xFLD_rm64(rm)           x86s(2) x86b(0xdd) x86b(mREG(0) | (rm))
#define xFLD_i32(a)             x86s(6) x86b(0xd9) x86b(mREG(0) | mOFS32) x86d(a)
#define xFLD_st(r)              x86s(2) x86b(0xd9) x86b(0xc0 + (r))
#define xFLD1                   x86s(2) x86b(0xd9) x86b(0xe8)
#define xFLDZ                   x86s(2) x86b(0xd9) x86b(0xee)

#define xFMUL_st(r)             x86s(2) x86b(0xd8) x86b(0xc8 + (r))
#define xFMUL_rm(rm)            x86s(2) x86b(0xd8) x86b(mREG(1) | (rm))
#define xFMUL_i32(a)            x86s(6) x86b(0xd8) x86b(mREG(1) | mOFS32) x86d(a)
#define xFMULP_st(r)            x86s(2) x86b(0xde) x86b(0xc8 + (r))
#define xFNSTSW                 x86s(2) x86b(0xdf) x86b(0xe0)

#define xFSQRT()                x86s(2) x86b(0xd9) x86b(0xfa)

#define xFSUB_rm(rm)            x86s(2) x86b(0xd8) x86b(mREG(4) | (rm))
#define xFSUBP_st(r)            x86s(2) x86b(0xde) x86b(0xe8 + (r))
#define xFSUBR_rm(rm)           x86s(2) x86b(0xd8) x86b(mREG(5) | (rm))
#define xFSUBRP_st(r)           x86s(2) x86b(0xde) x86b(0xe0 + (r))

#define xFST_rm(rm)             x86s(2) x86b(0xd9) x86b(mREG(2) | (rm))
#define xFSTP_rm(rm)            x86s(2) x86b(0xd9) x86b(mREG(3) | (rm))
#define xFSTP_i32(a)            x86s(6) x86b(0xd9) x86b(mREG(3) | mOFS32) x86d(a)
#define xFSTP_st(r)             x86s(2) x86b(0xdd) x86b(0xd8 + (r))

#define xFLDCW_rm(rm)           x86s(2) x86b(0xd9) x86b(mREG(5) | (rm))
#define xFSTCW_rm(rm)           x86s(2) x86b(0xd9) x86b(mREG(7) | (rm))


#define xFXCH_st(r)             x86s(2) x86b(0xd9) x86b(0xc8 + (r))

#define xFYL2X()                x86s(2) x86b(0xd9) x86b(0xf1)
#define xF2XM1()                x86s(2) x86b(0xd9) x86b(0xf0)

#define xINC_r(r)               x86s(1) x86b(0x40 | (r))
#define xINC_rm(rm)             x86s(2) x86b(0xff) x86b(mREG(0) | (rm))

#define xIMUL_r_rm(r,rm)        x86s(3) x86b(0x0f) x86b(0xaf) x86b(mREG(r) | (rm))
#define xIMUL_r_i32(r,a)        x86s(7) x86b(0x0f) x86b(0xaf) x86b(mREG(r) | mOFS32) x86d(a)
#define xIMUL_r_r_imm(r1,r2,i)  x86s(6) x86b(0x69) x86b(0xc0 | ((r1) << 3) | (r2)) x86d(i)

#define xINT3                   x86s(1) x86b(0xcc)

#define xJA(disp)               { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x77) x86b(_x) }
#define xJA32(disp)             { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x87) x86d(_x) }
#define xJAE(disp)              { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x73) x86b(_x) }
#define xJAE32(disp)            { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x83) x86d(_x) }
#define xJB(disp)               { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x72) x86b(_x) }
#define xJB32(disp)             { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x82) x86d(_x) }
#define xJBE(disp)              { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x76) x86b(_x) }
#define xJBE32(disp)            { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x86) x86d(_x) }
#define xJG(disp)               { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x7f) x86b(_x) }
#define xJG32(disp)             { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x8f) x86d(_x) }
#define xJGE(disp)              { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x7d) x86b(_x) }
#define xJGE32(disp)            { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x8d) x86d(_x) }
#define xJL(disp)               { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x7c) x86b(_x) }
#define xJL32(disp)             { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x8c) x86d(_x) }
#define xJLE(disp)              { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x7e) x86b(_x) }
#define xJLE32(disp)            { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x8e) x86d(_x) }
#define xJNZ(disp)              { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x75) x86b(_x) }
#define xJNZ32(disp)            { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x85) x86d(_x) }
#define xJZ(disp)               { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0x74) x86b(_x) }
#define xJZ32(disp)             { unsigned int _x = (disp)-(ilcCount+6); x86s(6) x86b(0x0f) x86b(0x84) x86d(_x) }


#define xJMP8(disp)             { unsigned int _x = (disp)-(ilcCount+2); x86s(2) x86b(0xeb) x86b(_x) }
#define xJMP(disp)              { unsigned int _x = (disp)-(ilcCount+5); x86s(5) x86b(0xe9) x86d(_x) }

#define xLEA_r_rm(r,rm)         x86s(2) x86b(0x8d) x86b(mREG(r) | (rm))

#define xMOV_r_rm(r,rm)         x86s(2) x86b(0x8b) x86b(mREG(r) | (rm))                     // mov eax,[ebx]
#define xMOV_r_i32(r,a)         x86s(6) x86b(0x8b) x86b(mREG(r) | mOFS32) x86d(a)           // mov eax,[1234h]
#define xMOV_rm_imm(rm,i)       x86s(6) x86b(0xc7) x86b(mREG(0) | (rm)) x86d(i)             // mov ebp,12345678
#define xMOV_r_imm(rm,i)        x86s(5) x86b(0xb8 | (rm)) x86d(i)                           // mov eax,12345678
#define xMOV_rm_r(rm,r)         x86s(2) x86b(0x89) x86b(mREG(r) | (rm))                     // mov [ebx],eax
#define xMOV_i32_r(a,r)         x86s(6) x86b(0x89) x86b(mREG(r) | mOFS32) x86d(a)           // mov [1234h],eax
#define xMOV_r_rm8(r,rm)        x86s(2) x86b(0x8a) x86b(mREG(r) | (rm))                     // mov al,[ebx]
#define xMOV_rm8_r(rm,r)        x86s(2) x86b(0x88) x86b(mREG(r) | (rm))                     // mov [ebx],al
#define xMOVSB                  x86s(1) x86b(0xa4)
#define xMOVSD                  x86s(1) x86b(0xa5)
#define xMOVSX_r_rm8(r,rm)      x86s(3) x86b(0x0f) x86b(0xbe) x86b(mREG(r) | (rm))
#define xMOVSX_r_rm16(r,rm)     x86s(3) x86b(0x0f) x86b(0xbf) x86b(mREG(r) | (rm))
#define xMOVZX_r_rm8(r,rm)      x86s(3) x86b(0x0f) x86b(0xb6) x86b(mREG(r) | (rm))
#define xMOVZX_r_rm16(r,rm)     x86s(3) x86b(0x0f) x86b(0xb7) x86b(mREG(r) | (rm))

#define xNEG_rm(rm)             x86s(2) x86b(0xf7) x86b(mREG(3) | (rm))

#define xNOP                    x86s(1) x86b(0x90)

#define xNOT_rm(rm)             x86s(2) x86b(0xf7) x86b(mREG(2) | (rm))

#define xOR_r_rm(r,rm)          x86s(2) x86b(0x0b) x86b(mREG(r) | (rm))
#define xOR_r_i32(r,a)          x86s(6) x86b(0x0b) x86b(mREG(r) | mOFS32) x86d(a)
#define xOR_rm_r(rm,r)          x86s(2) x86b(0x09) x86b(mREG(r) | (rm))
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

#define xREP                    x86s(1) x86b(0xf3)
#define xRET                    x86s(1) x86b(0xc3)
#define xRET_imm(i)             x86s(3) x86b(0xc2) x86w(i)

#define xSAHF                   x86s(1) x86b(0x9e)

#define xSAL_rm_imm8(rm,i)      x86s(3) x86b(0xc1) x86b(mREG(4) | (rm)) x86b(i)
#define xSAR_rm_imm8(rm,i)      x86s(3) x86b(0xc1) x86b(mREG(7) | (rm)) x86b(i)

#define xSBB_r_rm(r,rm)         x86s(2) x86b(0x1b) x86b(mREG(r) | (rm))
#define xSBB_r_i32(r,a)         x86s(6) x86b(0x1b) x86b(mREG(r) | mOFS32) x86d(a)
#define xSBB_rm_r(rm,r)         x86s(2) x86b(0x19) x86b(mREG(r) | (rm))

#define xSETAE_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x93) x86b(rm)
#define xSETG_rm8(rm)           x86s(3) x86b(0x0f) x86b(0x9f) x86b(rm)
#define xSETGE_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x9d) x86b(rm)
#define xSETL_rm8(rm)           x86s(3) x86b(0x0f) x86b(0x9c) x86b(rm)
#define xSETLE_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x9e) x86b(rm)
#define xSETNZ_rm8(rm)          x86s(3) x86b(0x0f) x86b(0x95) x86b(rm)
#define xSETZ_rm8(rm)           x86s(3) x86b(0x0f) x86b(0x94) x86b(rm)

#define xSHL_rm_1(rm)           x86s(2) x86b(0xd1) x86b(mREG(4) | (rm))
#define xSHL_rm_CL(rm)          x86s(2) x86b(0xd3) x86b(mREG(4) | (rm))
#define xSHL_rm_imm8(rm,i)      x86s(3) x86b(0xc1) x86b(mREG(4) | (rm)) x86b(i)
#define xSHR_rm_1(rm)           x86s(2) x86b(0xd1) x86b(mREG(5) | (rm))
#define xSHR_rm_CL(rm)          x86s(2) x86b(0xd3) x86b(mREG(5) | (rm))
#define xSHR_rm_imm8(rm,i)      x86s(3) x86b(0xc1) x86b(mREG(5) | (rm)) x86b(i)

#define xSTOSD                 x86s(1) x86b(0xab)

#define xSUB_r_rm(r,rm)         x86s(2) x86b(0x2b) x86b(mREG(r) | (rm))
#define xSUB_r_i32(r,a)         x86s(6) x86b(0x2b) x86b(mREG(r) | mOFS32) x86d(a)
#define xSUB_rm_r(rm,r)         x86s(2) x86b(0x29) x86b(mREG(r) | (rm))
#define xSUB_i32_r(a,r)         x86s(6) x86b(0x29) x86b(mREG(r) | mOFS32) x86d(a)
#define xSUB_rm_imm8(rm,i)      x86s(3) x86b(0x83) x86b(mREG(5) | (rm)) x86b(i)
#define xSUB_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(5) | (rm)) x86d(i)

#define xTEST_rm_r(rm,r)        x86s(2) x86b(0x85) x86b(mREG(r) | (rm))
#define xTEST_r_rm(r,rm)        x86s(2) x86b(0x85) x86b(mREG(r) | (rm))
#define xTEST_i32_r(a,r)        x86s(6) x86b(0x85) x86b(mREG(r) | mOFS32) x86d(a)
#define xTEST_r_i32(r,a)        x86s(6) x86b(0x85) x86b(mREG(r) | mOFS32) x86d(a)
#define xTEST_rm_imm8(rm,i)     x86s(3) x86b(0xf6) x86b(mREG(0) | (rm)) x86b(i)
#define xTEST_rm_imm(rm,i)      x86s(6) x86b(0xf7) x86b(mREG(0) | (rm)) x86d(i)

#define xXOR_r_rm(r,rm)         x86s(2) x86b(0x33) x86b(mREG(r) | (rm))
#define xXOR_r_i32(r,a)         x86s(6) x86b(0x33) x86b(mREG(r) | mOFS32) x86d(a)
#define xXOR_rm_r(rm,r)         x86s(2) x86b(0x31) x86b(mREG(r) | (rm))
#define xXOR_i32_r(a,r)         x86s(6) x86b(0x31) x86b(mREG(r) | mOFS32) x86d(a)
#define xXOR_rm_imm(rm,i)       x86s(6) x86b(0x81) x86b(mREG(6) | (rm)) x86d(i)

/*
 * mmx opcodes
 */
#define xMOVD_r_rm(r,rm)        x86s(3) x86b(0x0f) x86b(0x6e) x86b(mREG(r) | (rm))
#define xMOVD_r_i32(r,a)        x86s(7) x86b(0x0f) x86b(0x6e) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVD_rm_r(rm,r)        x86s(3) x86b(0x0f) x86b(0x7e) x86b(mREG(r) | (rm))
#define xMOVD_i32_r(a,r)        x86s(7) x86b(0x0f) x86b(0x7e) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVNTQ_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0xe7) x86b(mREG(r) | (rm))
#define xMOVQ_r_rm(r,rm)        x86s(3) x86b(0x0f) x86b(0x6f) x86b(mREG(r) | (rm))
#define xMOVQ_rm_r(rm,r)        x86s(3) x86b(0x0f) x86b(0x7f) x86b(mREG(r) | (rm))
#define xMOVQ_i64_r(a,r)        x86s(7) x86b(0x0f) x86b(0x7f) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVQ_r_i64(r,a)        x86s(7) x86b(0x0f) x86b(0x6f) x86b(mREG(r) | mOFS32) x86d(a)
#define xPADDB_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0xfc) x86b(mREG(r) | (rm))
#define xPADDB_r_i64(r,a)       x86s(7) x86b(0x0f) x86b(0xfc) x86b(mREG(4) | mOFS32) x86d(a)
#define xPADDW_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0xfd) x86b(mREG(r) | (rm))
#define xPADDW_r_i64(r,a)       x86s(7) x86b(0x0f) x86b(0xfd) x86b(mREG(4) | mOFS32) x86d(a)
#define xPADDD_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0xfe) x86b(mREG(r) | (rm))
#define xPADDD_r_i64(r,a)       x86s(7) x86b(0x0f) x86b(0xfe) x86b(mREG(4) | mOFS32) x86d(a)

#define xPMOVMSKB_r_r(r1, r2)   x86s(3) x86b(0x0f) x86b(0xd7) x86b(mREG(r1) | rmREG(r2))

#define xPUNPCKLBW_r_rm(r,rm)   x86s(3) x86b(0x0f) x86b(0x60) x86b(mREG(r) | (rm))
#define xPUNPCKLBW_r_i128(r,a)  x86s(7) x86b(0x0f) x86b(0x60) x86b(mREG(r) | mOFS32) x86d(a)
#define xPUNPCKLWD_r_rm(r,rm)   x86s(3) x86b(0x0f) x86b(0x61) x86b(mREG(r) | (rm))
#define xPUNPCKLWD_r_i128(r,a)  x86s(7) x86b(0x0f) x86b(0x61) x86b(mREG(r) | mOFS32) x86d(a)
#define xPUNPCKLDQ_r_rm(r,rm)   x86s(3) x86b(0x0f) x86b(0x62) x86b(mREG(r) | (rm))
#define xPUNPCKLDQ_r_i128(r,a)  x86s(7) x86b(0x0f) x86b(0x62) x86b(mREG(r) | mOFS32) x86d(a)

#define xEMMS                   x86s(2) x86b(0x0f) x86b(0x77)

/*
 * katmai opcodes
 */
#define xADDPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x58) x86b(mREG(r) | (rm))
#define xADDSS_r_rm(r,rm)       x86s(4) x86b(0xf3) x86b(0x0f) x86b(0x58) x86b(mREG(r) | (rm))

#define xCOMISS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x2f) x86b(mREG(r) | (rm))
#define xCMPPSZ_r_rm(r,rm)      x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(0)
#define xCMPPSL_r_rm(r,rm)      x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(1)
#define xCMPPSLE_r_rm(r,rm)     x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(2)
#define xCMPPSNZ_r_rm(r,rm)     x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(4)
#define xCMPPSGE_r_rm(r,rm)     x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(5)
#define xCMPPSG_r_rm(r,rm)      x86s(4) x86b(0x0f) x86b(0xc2) x86b(mREG(r) | (rm)) x86b(6)

#define xCVTPS2PI_r_rm(r,rm)    x86s(3) x86b(0x0f) x86b(0x2c) x86b(mREG(r) | (rm))

#define xDIVPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x5e) x86b(mREG(r) | (rm))

#define xMAXPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x5f) x86b(mREG(r) | (rm))
#define xMAXPS_r_i128(r,a)      x86s(7) x86b(0x0f) x86b(0x5f) x86b(mREG(r) | mOFS32) x86d(a)

#define xMINPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x5d) x86b(mREG(r) | (rm))
#define xMINPS_r_i128(r,a)      x86s(7) x86b(0x0f) x86b(0x5d) x86b(mREG(r) | mOFS32) x86d(a)

#define xMOVAPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x28) x86b(mREG(r) | (rm))
#define xMOVAPS_r_i128(r,a)     x86s(7) x86b(0x0f) x86b(0x28) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVAPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x29) x86b(mREG(r) | (rm))
#define xMOVAPS_i128_r(a,r)     x86s(7) x86b(0x0f) x86b(0x29) x86b(mREG(r) | mOFS32) x86d(a)

#define xMOVHLPS_r_r(r1,r2)     x86s(3) x86b(0x0f) x86b(0x12) x86b(mREG(r1) | rmREG(r2)) // special movlps
#define xMOVLHPS_r_r(r1,r2)     x86s(3) x86b(0x0f) x86b(0x16) x86b(mREG(r1) | rmREG(r2)) // special movhps

#define xMOVHPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x16) x86b(mREG(r) | (rm))
#define xMOVHPS_r_i64(r,a)      x86s(7) x86b(0x0f) x86b(0x16) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVHPS_i64_r(a,r)      x86s(7) x86b(0x0f) x86b(0x17) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVHPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x17) x86b(mREG(r) | (rm))

#define xMOVLPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x12) x86b(mREG(r) | (rm))
#define xMOVLPS_r_i64(r,a)      x86s(7) x86b(0x0f) x86b(0x12) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVLPS_i64_r(a,r)      x86s(7) x86b(0x0f) x86b(0x13) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVLPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x13) x86b(mREG(r) | (rm))

#define xMOVMSKPS_r_r(r1,r2)    x86s(3) x86b(0x0f) x86b(0x50) x86b(mREG(r1) | rmREG(r2))

#define xMOVNTPS_i128_r(a,r)    x86s(7) x86b(0x0f) x86b(0x2b) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVNTPS_rm_r(rm,r)     x86s(3) x86b(0x0f) x86b(0x2b) x86b(mREG(r) | (rm))

#define xMOVUPS_r_rm(r,rm)      x86s(3) x86b(0x0f) x86b(0x10) x86b(mREG(r) | (rm))
#define xMOVUPS_r_i128(r,a)     x86s(7) x86b(0x0f) x86b(0x10) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVUPS_rm_r(rm,r)      x86s(3) x86b(0x0f) x86b(0x11) x86b(mREG(r) | (rm))
#define xMOVUPS_i128_r(a,r)     x86s(7) x86b(0x0f) x86b(0x11) x86b(mREG(r) | mOFS32) x86d(a)

#define xMOVSS_r_rm(r,rm)       x86s(4) x86b(0xf3) x86b(0x0f) x86b(0x10) x86b(mREG(r) | (rm))
#define xMOVSS_r_i32(r,a)       x86s(8) x86b(0xf3) x86b(0x0f) x86b(0x10) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVSS_i32_r(a,r)       x86s(8) x86b(0xf3) x86b(0x0f) x86b(0x11) x86b(mREG(r) | mOFS32) x86d(a)

#define xMULPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x59) x86b(mREG(r) | (rm))
#define xMULPS_r_i128(r,a)      x86s(7) x86b(0x0f) x86b(0x59) x86b(mREG(r) | mOFS32) x86d(a)

#define xMULSS_r_rm(r,rm)       x86s(4) x86b(0xf3) x86b(0x0f) x86b(0x59) x86b(mREG(r) | (rm))

#define xRCPPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x53) x86b(mREG(r) | (rm))

#define xPREFETCH_rm8(h,rm,ofs) x86s(4) x86b(0x0f) x86b(0x18) x86b(mREG(h) | (rm)) x86b(ofs)

#define xPSHUFW_r_i64_imm(r,a,i) x86s(8) x86b(0xf) x86b(0x70) x86b(mREG(r) | mOFS32) x86d(a) x86b(i)
#define xPSHUFW_r_rm_imm(r,rm,i) x86s(4) x86b(0xf) x86b(0x70) x86b(mREG(r) | (rm)) x86b(i)

#define xSHUFPS_r_i64_imm(r,a,i) x86s(8) x86b(0x0f) x86b(0xc6) x86b(mREG(r) | mOFS32) x86d(a) x86b(i)
#define xSHUFPS_r_rm_imm(r,rm,i) x86s(4) x86b(0x0f) x86b(0xc6) x86b(mREG(r) | (rm)) x86b(i)

#define xSUBPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x5c) x86b(mREG(r) | (rm))
#define xSUBSS_r_rm(r,rm)       x86s(4) x86b(0xf3) x86b(0x0f) x86b(0x5c) x86b(mREG(r) | (rm))
#define xXORPS_r_rm(r,rm)       x86s(3) x86b(0x0f) x86b(0x57) x86b(mREG(r) | (rm))

 /*
 * Pentium 4 opcodes
 *
 * Note: Pentium 4 contains many equivalents to MMX instructions that use XMM registers instead of MM registers.
 * These new instructions are suffixed with an X after the opcode name.
 * example:
 *      MOVD    XMM0, ECX
 *  is
 *      xMOVDX_r_rm(rXMM0, rmREG(rECX))
 *
 *  do not write xMOVD_r_rm(rXMM0, rmREG(rECX)), as this actually generates MOVD    MM0, ECX   
 *
 * You would think that the macros would be smart enough to distinguish between XMM and MM registers, but they 
 * aren't. Sorry.
 */

#define xMOVDX_r_rm(r,rm)       x86s(4) x86b(0x66) x86b(0x0f) x86b(0x6e) x86b(mREG(r) | (rm))
#define xMOVDX_r_i32(r,a)       x86s(8) x86b(0x66) x86b(0x0f) x86b(0x6e) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVDX_rm_r(rm,r)       x86s(4) x86b(0x66) x86b(0x0f) x86b(0x7e) x86b(mREG(r) | (rm))
#define xMOVDX_i32_r(a,r)       x86s(8) x86b(0x66) x86b(0x0f) x86b(0x7e) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVSD_r_rm(r,rm)       x86s(4) x86b(0xf2) x86b(0x0f) x86b(0x10) x86b(mREG(r) | (rm))
#define xMOVSD_r_i128(r,a)      x86s(8) x86b(0xf2) x86b(0x0f) x86b(0x10) x86b(mREG(r) | mOFS32) x86d(a)
#define xMOVSD_rm_r(rm,r)       x86s(4) x86b(0xf2) x86b(0x0f) x86b(0x11) x86b(mREG(r) | (rm))
#define xMOVSD_i128_r(a,r)      x86s(8) x86b(0xf2) x86b(0x0f) x86b(0x11) x86b(mREG(r) | mOFS32) x86d(a)
#define xPADDBX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0xfc) x86b(mREG(r) | (rm))
#define xPADDBX_r_i64(r,a)      x86s(8) x86b(0x66) x86b(0x0f) x86b(0xfc) x86b(mREG(4) | mOFS32) x86d(a)
#define xPADDWX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0xfd) x86b(mREG(r) | (rm))
#define xPADDWX_r_i64(r,a)      x86s(8) x86b(0x66) x86b(0x0f) x86b(0xfd) x86b(mREG(4) | mOFS32) x86d(a)
#define xPADDDX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0xfe) x86b(mREG(r) | (rm))
#define xPADDDX_r_i64(r,a)      x86s(8) x86b(0x66) x86b(0x0f) x86b(0xfe) x86b(mREG(4) | mOFS32) x86d(a)
#define xPSHUFLW_r_i128_imm(r,a,i)  x86s(9) x86b(0xf2) x86b(0x0f) x86b(0x70) x86b(mREG(r) | mOFS32) x86d(a) x86b(i)
#define xPSHUFLW_r_rm_imm(r,rm,i)   x86s(5) x86b(0xf2) x86b(0x0f) x86b(0x70) x86b(mREG(r) | (rm)) x86b(i)
#define xPSHUFHW_r_i128_imm(r,a,i)  x86s(9) x86b(0xf3) x86b(0x0f) x86b(0x70) x86b(mREG(r) | mOFS32) x86d(a) x86b(i)
#define xPSHUFHW_r_rm_imm(r,rm,i)   x86s(5) x86b(0xf3) x86b(0x0f) x86b(0x70) x86b(mREG(r) | (rm)) x86b(i)
#define xPUNPCKLBWX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0x60) x86b(mREG(r) | (rm))
#define xPUNPCKLBWX_r_i128(r,a)     x86s(8) x86b(0x66) x86b(0x0f) x86b(0x60) x86b(mREG(r) | mOFS32) x86d(a)
#define xPUNPCKLWDX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0x61) x86b(mREG(r) | (rm))
#define xPUNPCKLWDX_r_i128(r,a)     x86s(8) x86b(0x66) x86b(0x0f) x86b(0x61) x86b(mREG(r) | mOFS32) x86d(a)
#define xPUNPCKLDQX_r_rm(r,rm)      x86s(4) x86b(0x66) x86b(0x0f) x86b(0x62) x86b(mREG(r) | (rm))
#define xPUNPCKLDQX_r_i128(r,a)     x86s(8) x86b(0x66) x86b(0x0f) x86b(0x62) x86b(mREG(r) | mOFS32) x86d(a)

 /*
 * 3dnow opcodes
 */
#define xFEMMS                  x86s(2) x86b(0x0f) x86b(0x0e)
#define xPFMUL_r_rm(r,rm)       x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0xb4)
#define xPFSUB_r_rm(r,rm)       x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0x9a)
#define xPFADD_r_rm(r,rm)       x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0x9e)
#define xPUNPCKHDQ_r_rm(r,rm)   x86s(3) x86b(0x0f) x86b(0x6a) x86b(mREG(r) | (rm))
#define xPUNPCKLDQ_r_rm(r,rm)   x86s(3) x86b(0x0f) x86b(0x62) x86b(mREG(r) | (rm))
#define xPFRCP_r_rm(r,rm) x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0x96)
#define xPFRCPIT1_r_rm(r,rm) x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0xa6)
#define xPFRCPIT2_r_rm(r,rm) x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0xb6)
#define xPSWAPD_r_rm(r,rm) x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0xbb)
#define xPFNACC_r_rm(r,rm) x86s(4) x86b(0x0f) x86b(0x0f) x86b(mREG(r) | (rm)) x86b(0x8a)

/*****************************************************************************
 * macros
 *****************************************************************************/

#if defined(IS_OPENGL)

#define ILC_GROW_TO(x) assert(!"can't grow ilcCount")

#else

/*****************************************************************************
 * externals
 *****************************************************************************/
void ILCGrowTo(unsigned long dwNeed);

#define ILC_GROW_TO(x) ILCGrowTo(x)

#endif

/*
 * emission
 */
#define x86s(x)                 if ((ilcCount+(x)) > ilcMax) ILC_GROW_TO(ilcCount+(x));
#define x86b(x)                 ilcData[ilcCount]=(unsigned char) (x); ilcCount++;
#define x86w(x)					*(unsigned short *) (ilcData + ilcCount) = (unsigned short) (x); ilcCount += 2;
#define x86d(x)                 *(unsigned int *) (ilcData+ilcCount) = (unsigned int) (x); ilcCount+=4;

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

#define times1                  0
#define times2                  1
#define times4                  2
#define times8                  3

#if !defined(IS_OPENGL)
////// Don't use these macro names!  They are used everywhere as vars and members!
#define x1                      times1
#define x2                      times2
#define x4                      times4
#define x8                      times8
#else
////// Don't use these macro names!  They are used everywhere as vars and members!
#endif // !defined(IS_OPENGL)


// xSIBID is used to write expressions like "mov    eax, [ebx*4+0x01234567]" (index + displacement)
// Note that you can't use EBP as a base in an SIB expression the way you think you can:
// 
// mov  eax, [ebp+edx*4] != xMOV_r_rm    (rEAX, rmSIB) xSIB(rEBP,rEDX,x4)
// Specifying EBP in xSIB like this is interpreted as a 32 bit displacement. So you
// have to write:
// xMOV_r_rm    (rEAX, rmSIB8) xSIB(rEBP, rEDX, x4) xOFS8(0) which is technically:
// mov eax, [ebp+edx*4+0]

#define xSIB(r1,r2,s)           x86s(1) x86b((r1) | ((r2)<<3) | ((s)<<6))
#define xSIB32(r1,r2,e)         x86s(1) x86b((r1) | ((r2)<<3) | ((times1)<<6)) xOFS32(e)
#define xSIBID(r2,s)            x86s(1) x86b((rEBP) | ((r2)<<3) | ((s)<<6))
#define xSIB_esp                xSIB(rESP, rESP, times1)
#define xSIB8_esp(e)            xSIB_esp xOFS8(e)
#define xSIB32_esp(e)           xSIB_esp xOFS32(e)

#define x16r                    x86s(1) x86b(0x66)
#define x16m                    x86s(1) x86b(0x67)

#define xDS                     x86s(1) x86b(0x3e)

#define xOFS8(e)                x86s(1) x86b(e)
#define xOFS32(e)               x86s(4) x86d(e)

#define mMEM8(x)               ((unsigned int) &(x))
#define mMEM32(x)              ((unsigned int) &(x))
#define mMEM64(x)              ((unsigned int) &(x))
#define mMEM128(x)             ((unsigned int) &(x))

/*
 * jumps * branches
 */
#define xLABEL(x)               (x) = ilcCount;
#define xTARGET_b8(x)           *(unsigned char *) (ilcData + (x) + 1) = (unsigned char) (ilcCount - ((x) + 2));
#define xTARGET_b32(x)          *(unsigned int *) (ilcData + (x) + 2) = (unsigned int) (ilcCount - ((x) + 6));
#define xTARGET_jmp(x)          *(unsigned int *) (ilcData + (x) + 1) = (unsigned int) (ilcCount - ((x) + 5));

/*
 * inline instructions not supported by built-in asm
 */
#define rdtsc  __asm _emit 0x0f __asm _emit 0x31
#define sfence __asm _emit 0x0f __asm _emit 0xae __asm _emit 0xf8

// For tracking codegen position across function calls
#define X86_CODE_DECLARE            \
    unsigned int ilcCount, ilcMax;  \
    unsigned char *ilcData

#define X86_CODE_LOAD(p)            \
    ilcCount = (p)->ilcCount;       \
    ilcMax = (p)->ilcMax;           \
    ilcData = (p)->ilcData

#define X86_CODE_RESTORE(p)         \
    (p)->ilcCount = ilcCount;       \
    (p)->ilcMax = ilcMax;           \
    (p)->ilcData = ilcData

#ifdef __cplusplus
}
#endif

#endif // x86.h
