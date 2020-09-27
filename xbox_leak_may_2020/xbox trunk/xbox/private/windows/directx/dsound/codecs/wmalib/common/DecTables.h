//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       tables_rc.h
//
//--------------------------------------------------------------------------

#ifndef _DECTABLES_H_
#define _DECTABLES_H_

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "macros.h"
#include "constants.h"

#ifdef USE_SIN_COS_TABLES
extern const Int g_SinTableForReconstruction[(2048+1024+512+256+128+64)];
#endif

//for high rates only
extern const U16    g_rgiBarkFreq	[];

extern const U16    g_rgiHuffDecTblMsk[];

extern const U16    g_rgiHuffDecTbl16smOb[];
extern const U16    gRun16smOb[];   //can be a byte
extern const U16    gLevel16smOb[]; //can be a byte

extern const U16    g_rgiHuffDecTbl16ssOb[];
extern const U16    gRun16ssOb[];   //can be a byte
extern const U16    gLevel16ssOb[]; //can be a byte

//For FFT & adaptToSubFrameConfig
typedef struct {
    BP1Type sin_PIby4cSB;		// sin( pi/(4*cSB) ) * nF2BP1 for DCT and inverseTransform
    BP1Type cos_PIby4cSB;		// cos( pi/(4*cSB) ) * nF2BP1 for DCT and inverseTransform
	BP1Type sin_3PIby4cSB;		// sin( 3*pi/(4*cSB) ) * nF2BP1 for DCT
	BP1Type cos_3PIby4cSB;		// cos( 3*pi/(4*cSB) ) * nF2BP1 for DCT
	BP1Type sin_PIbycSB;		// sin( pi/cSB ) * nF2BP1 for DCT
	BP1Type cos_PIbycSB;		// cos( pi/cSB ) * nF2BP1 for DCT
	BP1Type sin_2PIbycSB;		// sin( pi/(cSB/2) ) * nF2BP1 for FFT
	BP1Type cos_2PIbycSB;		// cos( pi/(cSB/2) ) * nF2BP1 for FFT
	BP1Type sin_PIby2cSB;		// sin( pi/(2*cSB) ) * nF2BP1 for inverseTransform
	BP1Type cos_PIby2cSB;		// cos( pi/(2*cSB) ) * nF2BP1 for inverseTransform
	BP1Type two_sin_PIbycSB;	// 2 * sin( pi/cSB ) * nF2BP1 for DCT
#ifdef ENABLE_LPC
    BP2Type sin_4PIbycSB;       // sin( 4*pi/cSB ) * nF2BP2 for LPC
    BP2Type cos_4PIbycSB;       // sin( 4*pi/cSB ) * nF2BP2 for LPC
#endif
}SinCosTable;

#define SINCOSTABLE_ENTRIES 17
extern const SinCosTable * const rgSinCosTables[SINCOSTABLE_ENTRIES];

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
	typedef I32 MaskPowerType;
#	define MASK_POWER_FRAC_BITS 28
#	define MASK_POWER_SCALE (1<<MASK_POWER_FRAC_BITS)
#	define MASK_POWER_FROM_FLOAT(flt) ((MaskPowerType)((flt)*MASK_POWER_SCALE))
#	define FLOAT_FROM_MASK_POWER(typ) ((typ)/((Float)MASK_POWER_SCALE))

	typedef I32 DBPowerType;
#	define DBPOWER_FRAC_BITS QUANTSTEP_FRACT_BITS
#	define DBPOWER_FROM_FLOAT(flt) ((DBPowerType)((flt)*(1<<DBPOWER_FRAC_BITS)))
#	define FLOAT_FROM_DBPOWER(typ) ((typ)*(((Float)1.0)/(1<<DBPOWER_FRAC_BITS)))
#else
	typedef Float MaskPowerType;
#	define MASK_POWER_FRAC_BITS 0
#	define MASK_POWER_SCALE 1
#	define MASK_POWER_FROM_FLOAT(flt) ((MaskPowerType)(flt))
#	define FLOAT_FROM_MASK_POWER(typ) ((Float)(typ))

	typedef Float DBPowerType;
#	define DBPOWER_FROM_FLOAT(flt) ((DBPowerType)(flt))
#	define FLOAT_FROM_DBPower(typ) ((Float)(typ))
#endif

#if defined(SKIMP_ON_ROM)
#	define MASK_MINUS_POWER_TABLE_SIZE 40
#else
#	define MASK_MINUS_POWER_TABLE_SIZE 72
#endif
extern const MaskPowerType rgiMaskMinusPower10[MASK_MINUS_POWER_TABLE_SIZE];
#define MASK_PLUS_POWER_TABLE_SIZE 50
extern const MaskPowerType rgiMaskPlusPower10[MASK_PLUS_POWER_TABLE_SIZE];


#define DBPOWER_TABLE_SIZE 128
#define DBPOWER_TABLE_OFFSET 18
extern const DBPowerType rgDBPower10[DBPOWER_TABLE_SIZE];
#ifdef BUILD_INT_FLOAT
extern const Float rgfltDBPowerExponentScale[((DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE)>>3)+1];
#endif

#ifdef ENABLE_ALL_ENCOPT
//for all other rates
extern const U16    g_rgiHuffDecTbl44smOb[];
extern const U16    gRun44smOb[];   //can be a byte
extern const U16    gLevel44smOb[];

extern const U16    g_rgiHuffDecTbl44ssOb[];
extern const U16    gRun44ssOb[];
extern const U16    gLevel44ssOb[]; //can be a byte

extern const U16    g_rgiHuffDecTbl44smQb[];
extern const U16    gRun44smQb[];
extern const U16    gLevel44smQb[];  //can be a byte

extern const U16    g_rgiHuffDecTbl44ssQb[];
extern const U16    gRun44ssQb[];
extern const U16    gLevel44ssQb[];  //can be a byte

extern const U16    g_rgiHuffDecTblNoisePower[];

extern const float  g_rgfltLsfReconLevel [LPCORDER] [16];

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)

#   define INVQUADROOT_FRACTION_TABLE_LOG2_SIZE 8
#   define INVQUADROOT_FRACTION_TABLE_SIZE (1<<INVQUADROOT_FRACTION_TABLE_LOG2_SIZE)

    extern const UInt g_InvQuadRootFraction[1+INVQUADROOT_FRACTION_TABLE_SIZE];
    extern const UInt g_InvQuadRootExponent[BITS_LP_SPEC_POWER+1];

#   define INVERSE_FRACTION_TABLE_LOG2_SIZE 8
#   define INVERSE_FRACTION_TABLE_SIZE (1<<INVERSE_FRACTION_TABLE_LOG2_SIZE)
    extern const UInt g_InverseFraction[1+INVERSE_FRACTION_TABLE_SIZE];

#   define SQRT_FRACTION_TABLE_LOG2_SIZE 8
#   define SQRT_FRACTION_TABLE_SIZE (1<<SQRT_FRACTION_TABLE_LOG2_SIZE)
    extern const UInt g_SqrtFraction[1+SQRT_FRACTION_TABLE_SIZE];

#endif

#endif // ENABLE_ALL_ENCOPT

#define PWRTORMS_ZEROPT     60
#define PWRTORMS_SIZE       (PWRTORMS_ZEROPT*2 + 1)
extern const UInt           g_rgfiPwrToRMS[];

#ifdef __cplusplus
}
#endif
#endif

