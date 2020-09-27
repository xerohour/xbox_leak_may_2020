/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    Macros.h

Abstract:

    Processor-specific macros, defines, and data types.

Author:

    Raymond Cheng (raych)       Jul 29, 1999

Revision History:


*************************************************************************/

#ifndef __COMMON_MACROS_H
#define __COMMON_MACROS_H

#ifndef DISABLE_ASSERTS
#define DISABLE_ASSERTS
#endif // DISABLE_ASSERTS

// If BUILD_INTEGER or BUILD_INT_FLOAT is not defined by project set default state

#ifdef SDE_WANTS_ASSERTS
// This should be defined only when an SDE is watching and never when checking in or for builds.
// a compiler warning message is on wmaos.c
#   ifdef DISABLE_ASSERTS
#       undef DISABLE_ASSERTS
#   endif
#elif !defined(DISABLE_ASSERTS)
// DISABLE ASSERTS because of a known issue. V4-encoded streams can contain garbage
// at the end which we will detect as an invalid bitstream.
#   define DISABLE_ASSERTS
#endif

#ifdef BUILD_INT_FLOAT
    // build Floating version of integer tree (otherwise, build integer version)
#   if defined(BUILD_INTEGER)
#       undef BUILD_INTEGER
#   endif
#elif !defined(BUILD_INTEGER)
    // This is default for decoder if neither is defined
#   define BUILD_INTEGER
#endif  

#ifdef ENCODER
// not integer build if encoder
#undef BUILD_INTEGER
#if !defined(INTEGER_ENCODER) && !defined(BUILD_INT_FLOAT)
// except for integer encoder, assume float for the encoder
#define BUILD_INT_FLOAT
#endif  // INTEGER_ENCODER
#endif  // ENCODER

#include "constants.h"

#ifdef UNDER_CE
#include <windef.h>
#include <kfuncs.h>
#include <dbgapi.h>
#define assert ASSERT
#else
// The following are ANSI C includes, and so should be safe for all platforms
#include <assert.h>     // If this doesn't exist, you must write an "assert" macro
#endif
#include <string.h>
#include <stdlib.h>

#ifdef DISABLE_ASSERTS
#undef assert
#define assert(a)
#endif  // DISABLE_ASSERTS


// **************************************************************************
// Platform-Specific #ifdefs
// **************************************************************************
#ifdef __QNX__
#undef _M_IX86
#endif
#if defined(_M_IX86) && !defined(INTEGER_ENCODER)
//#ifdef __QNX__
//#define WMA_TARGET_QNX_X86
//#else
#define WMA_TARGET_X86
//#endif
#endif

#ifdef _MIPS_
#define WMA_TARGET_MIPS
#endif

#if defined(_SH3_) && !defined(_SH4_)
#define WMA_TARGET_SH3
#endif

#ifdef _SH4_
#define WMA_TARGET_SH4
#endif

#ifdef _Embedded_x86
#define WMA_TARGET_Embedded_x86
#endif


#ifdef S_SUNOS5
#define WMA_TARGET_S_SUNOS5
#endif

#ifdef _ARM_
#define WMA_TARGET_ARM
#endif

// It's often nice to be able to compare the output between optimized versions
// and the ANSI version, to verify that the optimizations are in sync. Alas,
// some optimizations sacrifice reproducibility for speed. The following #define allows
// us to disable those optimizations to verify the main algorithms.
//#define WMA_ANSI_COMPATIBILITY_MODE


// ======================================================
// Intel x86 Platform
// ======================================================
#if defined(WMA_TARGET_X86)

#if !defined(UNDER_CE) 
#define USE_SIN_COS_TABLES
#endif

// Override the ROUNDF/ROUNDD macros below
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif  //LITTLE_ENDIAN
#define PLATFORM_SPECIFIC_ROUND
#define PLATFORM_SPECIFIC_BITCOPY
#define PLATFORM_SPECIFIC_FNPTR //mmx, katmai

#ifdef WMA_ANSI_COMPATIBILITY_MODE
#undef PLATFORM_SPECIFIC_ROUND
#endif // WMA_ANSI_COMPATIBILITY_MODE

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef unsigned __int64 U64;
typedef __int64 I64;
// Default all other typedefs to standard definitions (see below)
                                                               
// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#if _MSC_VER >= 1200            /* VC6.0 == 1200, VC5.0 == 1100 */
#define INLINE __forceinline
#else  /* _MSC_VER */
#define INLINE __inline
#endif /* _MSC_VER */
// ======================================================
// QNX in X86 Platform
// ======================================================
#elif defined(WMA_TARGET_QNX_X86)

// Override the ROUNDF/ROUNDD macros below
#define LITTLE_ENDIAN
#define PLATFORM_SPECIFIC_ROUND
#define PLATFORM_SPECIFIC_BITCOPY

#ifdef WMA_ANSI_COMPATIBILITY_MODE
#undef PLATFORM_SPECIFIC_ROUND
#endif // WMA_ANSI_COMPATIBILITY_MODE

// Do the fundamental typedefs: {U|I}{8|16|32|64}
#ifdef __QNX__
#include "../qnx/qnx.h"
#else
typedef __int64 U64;
typedef __int64 I64;
#endif
// Default all other typedefs to standard definitions (see below)

// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE                       
#define INLINE __inline

// ======================================================
// MIPS (Casio) Platform
// ======================================================
#elif defined(WMA_TARGET_MIPS)

#define LITTLE_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef unsigned __int64 U64;
typedef __int64 I64;
// Default all other typedefs to standard definitions (see below)

// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#define INLINE __forceinline


// ======================================================
//SH3 (HP WinCE and Hitachi) Platform
// ======================================================
#elif defined(WMA_TARGET_SH3)

#define LITTLE_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
#ifdef NOINT64 
#include "int64.h"
typedef UINT64 U64;
typedef  INT64 I64;
#else
typedef unsigned __int64 U64;
typedef __int64 I64;
#endif
// Default all other typedefs to standard definitions (see below)

// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#ifdef HITACHI
#pragma inline(quickRand, RandStateClear, LOG2, NormUInt, Align2FracBits, ROUNDF, ROUNDD, bitCpy)
#define INLINE static
#else
#define INLINE __forceinline
#endif
// ======================================================
//SH4 (HP WinCE and Hitachi) Platform
// ======================================================
#elif defined(WMA_TARGET_SH4)

#define LITTLE_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
#ifdef NOINT64 
#include "int64.h"
typedef UINT64 U64;
typedef  INT64 I64;
#else
typedef __int64 U64;
typedef __int64 I64;
#endif
// Default all other typedefs to standard definitions (see below)

// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#ifdef HITACHI
#pragma inline(quickRand, RandStateClear, LOG2, NormUInt, Align2FracBits, ROUNDF, ROUNDD, bitCpy)
#define INLINE static
#else
#define INLINE __forceinline
#endif
// ======================================================
//ARM Platform
// ======================================================
#elif defined(WMA_TARGET_ARM)

#define LITTLE_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef unsigned __int64 U64;
typedef __int64 I64;
// Default all other typedefs to standard definitions (see below)

// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#define INLINE __forceinline

// ======================================================
//Linux in X86 Platform
// ======================================================
#elif defined(WMA_TARGET_Embedded_x86)
#undef BIG_ENDIAN
#undef  LITTLE_ENDIAN
#define LITTLE_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef  unsigned long long U64;
typedef  long long  I64;
// Default all other typedefs to standard definitions (see below)
// 
// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#define INLINE inline  

// ======================================================
//Sun Solaris
// ======================================================
#elif defined(WMA_TARGET_S_SUNOS5)

#undef  LITTLE_ENDIAN
#undef  BIG_ENDIAN
#define BIG_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef  unsigned long long int  U64 ;
typedef  long long int  I64 ;

#define  PLATFORM_SPECIFIC_I32
typedef unsigned int U32;
typedef int I32;
// Default all other typedefs to standard definitions (see below)
// 
// Do compiler directives
#define PLATFORM_SPECIFIC_INLINE
#define INLINE inline  


// ======================================================
//MacOS Platform
// ======================================================
#elif defined(WMA_TARGET_MACOS)

#undef  LITTLE_ENDIAN
#undef  BIG_ENDIAN
#define BIG_ENDIAN

// Do the fundamental typedefs: {U|I}{8|16|32|64}
typedef  unsigned long long     U64;
typedef  long long              I64;

// Default all other typedefs to standard definitions (see below)
// 


// ======================================================
// ANSI-C version (no platform-specific details
// ======================================================
#else

#define WMA_TARGET_ANSI

// Default endian
#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#define LITTLE_ENDIAN
#endif

// Do the fundamental typedefs: {U|I}{8|16|32|64}
#ifdef NOINT64 
#include "int64.h"
typedef UINT64 U64;
#else
#ifdef __QNX__
typedef unsigned long long int U64;
typedef long long int I64;
#else
#ifdef macintosh
typedef unsigned long long U64;
typedef long long I64;
#else
typedef unsigned __int64 U64;
typedef __int64 I64;
#endif
#endif
#endif //NOINT64
// Default all other typedefs to standard definitions (see below)

#endif // Platform-specific #ifdefs



// **************************************************************************
// Default Fundamental Typedefs
// **************************************************************************
#ifndef PLATFORM_SPECIFIC_I32
#ifdef HITACHI
typedef unsigned long U32;
typedef long I32;
#else
typedef unsigned long int U32;
typedef long int I32;
#endif
#endif

#ifndef PLATFORM_SPECIFIC_I16
typedef unsigned short U16;
typedef short I16;
#endif

#ifndef PLATFORM_SPECIFIC_I8
typedef unsigned char U8;
typedef signed char I8;
#endif



// **************************************************************************
// Common Typedefs (not expected to change by platform)
// **************************************************************************
#ifndef _WMARESULT_DEFINED
#define _WMARESULT_DEFINED
typedef I32 WMARESULT;

// SUCCESS codes
static const WMARESULT WMA_OK               = 0x00000000;
static const WMARESULT WMA_S_FALSE          = 0x00000001;
static const WMARESULT WMA_S_BUFUNDERFLOW   = 0x00000002;
static const WMARESULT WMA_S_NEWPACKET      = 0x00000003;
static const WMARESULT WMA_S_NO_MORE_FRAME  = 0x00000004;
static const WMARESULT WMA_S_DEFAULT        = 0x00000005;
static const WMARESULT WMA_S_SWITCHCHMODE   = 0x00000006; // Internal error, corrected internally

// ERROR codes
static const WMARESULT WMA_E_FAIL           = 0x80004005;
static const WMARESULT WMA_E_OUTOFMEMORY    = 0x8007000E;
static const WMARESULT WMA_E_INVALIDARG     = 0x80070057;
static const WMARESULT WMA_E_NOTSUPPORTED   = 0x80040000;
static const WMARESULT WMA_E_LOSTPACKET     = 0x80040001;
static const WMARESULT WMA_E_BROKEN_FRAME   = 0x80040002;
static const WMARESULT WMA_E_BUFFEROVERFLOW = 0x80040003;
static const WMARESULT WMA_E_ONHOLD         = 0x80040004; 
//ON_HOLD: the decoder can't continue operating because of sth, most
//       likely no more data for flushing, getbits or peekbits. all functions
//       stop *immediately*.

static const WMARESULT WMA_E_BUFFERUNDERFLOW =0x80040005;
static const WMARESULT WMA_E_INVALIDRECHDR  = 0x80040006;
static const WMARESULT WMA_E_SYNCERROR      = 0x80040007;
static const WMARESULT WMA_E_NOTIMPL        = 0x80040008;
static const WMARESULT WMA_E_FLUSHOVERFLOW  = 0x80040009; // Too much input data provided to flush


#endif // _WMARESULT_DEFINED

#define WMAB_TRUE 1
#define WMAB_FALSE 0

#define own // used as "reserved word" to indicate ownership or transfer to distinguish from const

typedef double Double;
typedef float Float;
typedef void Void;
typedef U32 UInt;
typedef I32 Int;
typedef I32 Bool; // In Win32, BOOL is an "int" == 4 bytes. Keep it this way to reduce problems.


// **************************************************************************
// Overridable Compiler Directives
// **************************************************************************
#ifndef PLATFORM_SPECIFIC_INLINE
// This should hopefully work for most compilers. It works for VC
#define INLINE __inline
#endif  // PLATFORM_SPECIFIC_INLINE

#ifndef PLATFORM_SPECIFIC_COMPILER_MESSAGE
#define COMPILER_MESSAGE(x)         message(x)
#endif

#ifndef PLATFORM_SPECIFIC_COMPILER_PACKALIGN
#define COMPILER_PACKALIGN(x)       pack(x)
#endif

#ifndef PLATFORM_SPECIFIC_COMPILER_PACKALIGN_DEFAULT
#define COMPILER_PACKALIGN_DEFAULT  pack()
#endif

#ifdef _MSC_VER
#define MSVC_DISABLE_WARNING(x)     warning(disable:x)
#define MSVC_RESTORE_WARNING(x)     warning(default:x)
#define MSVC_CDECL                  __cdecl
#else   // _MSC_VER
// I'm hoping that compilers can take an empty #pragma (VC can)
#define MSVC_DISABLE_WARNING(x)
#define MSVC_RESTORE_WARNING(x)
#define MSVC_CDECL
#endif  // _MSC_VER


// **************************************************************************
// Macros Common to All Platforms
// **************************************************************************
// If you need to override the following for just one platform, #define a
// switch to override as in the case for ROUNDF/ROUNDD above in the
// platform-specific #ifdefs so that it is plain to see.

#define MULT_HI(a,b) (MULT_HI_DWORD(a,b)<<1)
#ifndef BUILD_INTEGER
    // these macros are not used by integer decoder. 
    // Where/when needed, provide cpu depended optimized versions as well as these generic ones.
#   define UMASR(a,b,c) (U32)((((U64)a)*((U64)b))>>c)
#   define MASR(a,b,c) (I32)((((I64)a)*((I64)b))>>(c))
#   define DASR(a,b,c) (I32)((((I64)a)*((I64)b))>>(c))
#   define DASL(a,b,c) (I32)((((I64)a)*((I64)b))<<(c))

    // The following macro has no perf-enhanced equivalents, although one can
    // easily be written a la x86 MULT_HI_DWORD:
    // I32HI,LOW = a*b; if (I32LOW & 0x80000000) I32HI += 1; return I32HI;
    // NOTE that it only rounds UP, and does not consider negative numbers.
#   define MULT_HI_DWORD_ROUND(a,b)    (I32)((((I64)(a))*((I64)(b)) + ((I64)1 << 31))>>32)
#endif

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define checkRange(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define DELETE_ARRAY(ptr) if (ptr!=NULL) auFree (ptr); ptr = NULL;
#define DELETE_PTR(ptr) if (ptr!=NULL) auFree (ptr); ptr = NULL;
#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof((ar)[0]))

#  ifdef DEBUG_ONLY
// WCE already has this macro defined in the same way, avoid warning messages
#  undef DEBUG_ONLY
#  endif
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else   // _DEBUG
#define DEBUG_ONLY(x)
#endif  // _DEBUG

#if defined(LITTLE_ENDIAN)
#define DEFAULTENDIAN_TO_BIGENDIAN(i)   ByteSwap(i)
#define BIGENDIAN_TO_DEFAULTENDIAN(i)   ByteSwap(i)
#elif defined(BIG_ENDIAN)
#define DEFAULTENDIAN_TO_BIGENDIAN(i)   (i)
#define BIGENDIAN_TO_DEFAULTENDIAN(i)   (i)
#else
#error Must define the endian of the target platform
#endif // ENDIAN

#define SWAP_WORD( w )      (w) = ((w) << 8) | ((w) >> 8)

#define SWAP_DWORD( dw )    (dw) = ((dw) << 24) | ( ((dw) & 0xFF00) << 8 ) | ( ((dw) & 0xFF0000) >> 8 ) | ( ((dw) & 0xFF000000) >> 24);

// WMARESULT macros (stolen from winerror.h)
#define WMA_SUCCEEDED(Status) ((WMARESULT)(Status) >= 0)
#define WMA_FAILED(Status) ((WMARESULT)(Status)<0)


// Create types which are change implementation between BUILD_INTEGER and BUILD_INT_FLOAT
// Some platforms, like the X86 and the SH4, can implement either build efficently.
// Other platforms, with only emulated floating point support, are typically only build with BUILD_INTEGER
//  
// Each type defined below is either a float (BUILD_INT_FLOAT) and an I32 with some number of fractional bits (BUILD_INTEGER)
// Use float or and I32 with five fractional bits
// And create a type for trig functions (Float or I32 with 30 fractional bits)
#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)

// Coefficents type (output of inverse quantization, transformed by DCT and overlapped/added)
// Range is +/- 2^26 with five fractional bits
    typedef I32 CoefType;
#   define COEF_FRAC_BITS 5
#   define COEF_FRAC_SCALE (1<<COEF_FRAC_BITS)
#   define COEF_FROM_FLOAT(flt) ((CoefType)(flt*COEF_FRAC_SCALE))
#   define FLOAT_FROM_COEF(coef) (coef/((Float)COEF_FRAC_SCALE))
#   define COEF_FROM_INT(i) ((i)<<COEF_FRAC_BITS)
#   define INT_FROM_COEF(cf) ((cf)>>COEF_FRAC_BITS)

// Fractional Type with range -2.0 <= x < 2.0 used by FFT Trig Recurrsion 
    typedef I32 BP2Type;
#   define BP2_FRAC_BITS 30
#   define BP2_FRAC_SCALE NF2BP2
#   define BP2_FROM_FLOAT(flt) ((BP2Type)(flt*BP2_FRAC_SCALE))
#   define FLOAT_FROM_BP2(bp2) (bp2/((Float)BP2_FRAC_SCALE))
#   define BP2_FROM_BP1(bp1) ((bp1)>>1)
#   define MULT_BP2(a,b) MULT_HI_DWORD_DOWN((a),(b))

// Fractional type with range -1.0 <= x < 1.0 used by DCT Trig Recurrsion
    typedef I32 BP1Type;
#   define BP1_FRAC_BITS 31
#   define BP1_FRAC_SCALE NF2BP1
#   define BP1_FROM_FLOAT(flt) ((BP2Type)(flt*BP1_FRAC_SCALE))
#   define FLOAT_FROM_BP1(bp1) (bp1/((Float)BP1_FRAC_SCALE))
#   define MULT_BP1(a,b) MULT_HI((a),(b))

// Fraction is stored at 2^32 (BP0)
#define UBP0_FROM_FLOAT(x) ((U32)(x*4294967296.0F))

    typedef I32 FracBitsType;
    typedef struct {
    //I8 exponent;  Old exponent was shift from binary point at 24th position
        FracBitsType iFracBits;
        I32 iFraction;
    } FastFloat;

// Type for holding quantization magnitide returned by prvWeightedQuantization
    typedef FastFloat QuantFloat;

//****************************************************************************
//         Types, constants related to LPC.
//         Lot of these could be moved to constants.h or lpcConst.h for cleanliness
//****************************************************************************

//  LSP (aka LSF), used for inverse quantized LSF.
    typedef Int LspType;
#   define FRACT_BITS_LSP 30
#   define LSP_SCALE (1<<FRACT_BITS_LSP)
#   define LSP_FROM_FLOAT(a) ((LpType)((a)*LSP_SCALE))
#   define MULT_LSP(a,b) (I32)((((I64)(a))*((I64)(b)))>>FRACT_BITS_LSP)
// MULT_LSP may be redefined by processor specific code in macros_cpu.h

//  LP, used for P, Q, and LPC values
    typedef Int LpType;
#   ifndef MATCH_ARM_WAY
#       define FRACT_BITS_LP 27
#   else
#       define FRACT_BITS_LP 26
#   endif
#   define LP_SCALE (1<<FRACT_BITS_LP)
#   define FLOAT_FROM_LP(a) ((Float)((a)*(1.0f/LP_SCALE)))
#   define LP_FROM_FLOAT(a) ((LpType)((a)*LP_SCALE))

//  LP Spectrum: used during FFT of LPC values
//  LP Spectrum power: sum-squares of FFT output
    typedef Int LpSpecType;
#if defined (LPCPOWER32)
//               Uses 32-bit intermediates
//               LPC scale is 2^21.  Making it any larger causes overflows in the square(x) function 
//               in LPC_TO_SPECTRUM. But would like more frac bits, because the small values of the 
//               spectrum yield larger WeightFactors. And float version on the same sound uses 
//               IQR(1.575e-7) = 75.5795. Largest IQR of 1/LPC_SCALE is 38 or the result for values 
//               less than that which is 1/(2*LPC_SCALE) => 45.25.
#   define FRACT_BITS_LP_SPEC 21
    typedef UInt LpSpecPowerType;
#   define BITS_LP_SPEC_POWER 32
#   define FRACT_BITS_LP_SPEC_POWER 21
#   define QR_FRACTION_FRAC_BITS 27
#   define QR_EXPONENT_FRAC_BITS 25

#else // !LPCPOWER32
//                Uses 64-bit intermediates
#   ifndef MATCH_ARM_WAY
#       define FRACT_BITS_LP_SPEC 25
#   else
        //  ARM's use of 26 instead of 25 produces BIG relative errors compared to LPC_COMPARE
        //  at least when using the old version of the Lpc code.
//#       define FRACT_BITS_LP_SPEC 26
#       define FRACT_BITS_LP_SPEC 25
#   endif
    typedef U64 LpSpecPowerType;
#   define BITS_LP_SPEC_POWER 64
#   define FRACT_BITS_LP_SPEC_POWER 30 
#   define QR_FRACTION_FRAC_BITS 24
#   define QR_EXPONENT_FRAC_BITS 29
#endif // LPCPOWER32

#   define LP_SPEC_SCALE (1<<FRACT_BITS_LP_SPEC)
#   define LP_SPEC_POWER_SCALE ((float)(1L<<FRACT_BITS_LP_SPEC_POWER))
#   define LP_SPEC_FROM_FLOAT(a) ((LpSpecType)((a)*LP_SPEC_SCALE))
#   define LP_SPEC_POWER_FROM_FLOAT(a) ((LpSpecType)((a)*LP_SPEC_POWER_SCALE))
#   define FLOAT_FROM_LP_SPEC(a) ((Float)((a)*(1.0f/LP_SPEC_SCALE)))
#   define LP_SPEC_FROM_LP(a) ((LpSpecType)((a)>>(FRACT_BITS_LP-FRACT_BITS_LP_SPEC)))

//  Weights, computed from LPC (and Bark?).
    typedef UInt WeightType;
//               We see weight factor values upto 64.53440857: 7 bits enough to cover integer part, 
//               leaving upto 25 bits for fraction. However, only 21 bits of fraction are generated 
//               upstream, making other 4 bits zero, even if used.
#if defined (LPCPOWER32)
#   define WEIGHTFACTOR_FRACT_BITS     20
#else
#   ifndef MATCH_ARM_WAY
#       define WEIGHTFACTOR_FRACT_BITS     21
#   else
#       define WEIGHTFACTOR_FRACT_BITS     25
#   endif
#endif 
#   define WEIGHT_FROM_FLOAT(a) ((WeightType)((a)*(1<<WEIGHTFACTOR_FRACT_BITS)))
#   define FLOAT_FROM_WEIGHT(a) ((Float)((a)*(1.0f/(1<<WEIGHTFACTOR_FRACT_BITS))))

//****************************************************************************
//         End of types, constants related to LPC.
//****************************************************************************

    // Some utility functions with obvious meaning
#   define DIV2(a) ((a)>>1)
#   define MUL2(a) ((a)<<1)
#   define DIV4(a) ((a)>>2)
#   define MUL4(a) ((a)<<2)
#   define MUL8(a) ((a)<<3)

#   define INTEGER_ONLY(a) a
#   define INTEGER_OR_INT_FLOAT(a,b) a

#else // ************ so must be BUILD_INT_FLOAT *****************

// Coefficents type (output of inverse quantization, transformed by DCT and overlapped/added)
    typedef Float CoefType;
#   define COEF_FRAC_BITS 0
#   define COEF_FRAC_SCALE 1
#   define COEF_FROM_FLOAT(flt) ((CoefType)(flt))
#   define FLOAT_FROM_COEF(coef) ((Float)(coef))
#   define COEF_FROM_INT(i) (i)
#   define INT_FROM_COEF(cf) (cf)

// Fractional Type used by FFT Trig Recurrsion 
    typedef Float BP2Type;
#   define BP2_FRAC_BITS 0
#   define BP2_FRAC_SCALE 1
#   define BP2_FROM_FLOAT(flt) ((BP2Type)(flt))
#   define FLOAT_FROM_BP2(bp2) ((Float)(bp2))
#   define BP2_FROM_BP1(bp1) (bp1)
#   define MULT_BP2(a,b) ((a)*(b))

// Fractional type used by DCT Trig Recurrsion
    typedef Float BP1Type;
#   define BP1_FRAC_BITS 0
#   define BP1_FRAC_SCALE 1
#   define BP1_FROM_FLOAT(flt) ((BP1Type)(flt))
#   define FLOAT_FROM_BP1(bp1) ((Float)(bp1))
#   define MULT_BP1(a,b) ((a)*(b))

// Another form of floating point 
    typedef Float FastFloat;

// Type for holding quantization magnitide returned by prvWeightedQuantization
    typedef Float QuantFloat;

//****************************************************************************
//         Types, constants related to LPC.
//****************************************************************************

//  LSP (aka LSF), used for inverse quantized LSF.
    typedef Float LspType;
#   define LSP_FROM_FLOAT(a) ((LspType)(a))
#   define MULT_LSP(x,y) ((x)*(y))

// LP, used for P, Q, and LPC values
    typedef Float LpType;
#   define LP_FROM_FLOAT(a) ((LpType)(a))
#   define FLOAT_FROM_LP(a) ((Float)(a))

// LP Spectrum: used during FFT of LPC values
// LP Spectrum power: sum-squares of FFT output
    typedef Float LpSpecType;
    typedef Float LpSpecPowerType;
#   define LP_SPEC_FROM_FLOAT(a) ((LpSpecType)(a))
#   define FLOAT_FROM_LP_SPEC(a) ((Float)(a))
#   define LP_SPEC_FROM_LP(a)    ((LpSpecType)(a))

//  Weights, computed from LPC   (and Bark?).
    typedef Float WeightType;
#   define WEIGHT_FROM_FLOAT(a) ((WeightType)(a))
#   define FLOAT_FROM_WEIGHT(a) ((Float)(a))

//****************************************************************************
//         End of types, constants related to LPC.
//****************************************************************************

// Some utility functions with obvious meaning
#   define DIV2(a) ((a)*0.5f)
#   define MUL2(a) ((a)*2.0f)
#   define DIV4(a) ((a)*0.25f)
#   define MUL4(a) ((a)*4.0f)
#   define MUL8(a) ((a)*8.0f)

#   define INTEGER_ONLY(a)
#   define INTEGER_OR_INT_FLOAT(a,b) b

#endif  // either BUILD_INTEGER or BUILD_INT_FLOAT

// some utility macros that are type specific to avoid the compiler's type conversion rules.
#define COEF_ABS(a) (((a)<0) ? -(a) : (a))
#define BPX_ABS(a) (((a)<0) ? -(a) : (a))


#if defined(ENCODER) || defined(INTEGER_ENCODER) || !(defined(REFERENCE_RAND_24) || defined(REFERENCE_RAND_16))
    //****************************************************************************
    // quick and dirty rand generator, assuming low word multiply, only works for 
    // 32-bit machines see Numerical Recipes p.284.  
    // Note this is the referenced generator but the 24-bit or 16-bit versions can
    // be used for compliance comparisions with 24-bit or 16-bit implementations.
    //****************************************************************************

    typedef struct tagRandState {
        I32 iPrior;         // prior value
        U32 uiRand;         // current value
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        const U32 a = 1664525;
        const U32 c = 1013904223;
        I32 iTemp, iTemp1;
        //a*x + c has to be done with unsigned 32 bit
        ptRandState->uiRand =  a * ptRandState->uiRand + c;

        // uiRand values starting from a 0 seed are: 0x3c6ef35f, 0x47502932, 0xd1ccf6e9, 0xaaf95334, 0x6252e503, 0x9f2ec686, 0x57fe6c2d, ...

        // do not change the above - this reference generator has been extensively tested and has excellent randomness properties
        // a truism in the world of random number generator theory and practice is:
        // "any change, no matter how small, can and will change the properties of the generator and must be fully tested"
        // In case you don't know, it can easily take a person-month to fully test a generator.
    
        // Notwithstanding the above, it is fair to take a function of a random number to shape its range or distribution.
        // This we do below to give it a triangular distrbution between -2.5 and 2.5 to roughly approximate a Guassian distribution.

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = ((I32)(ptRandState->uiRand)) >> 2; 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += (iTemp>>2);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
    }

	// protect encoder from having these defined by mistake
#	ifdef REFERENCE_RAND_24
#		undef REFERENCE_RAND_24
#	endif
#	ifdef REFERENCE_RAND_16
#		undef REFERENCE_RAND_16
#	endif

#elif defined(REFERENCE_RAND_24)
    //****************************************************************************
    // Quick and dirty 24-bit rand generator.
    // Simulates 24-bits on a 32-bit reference machine.
    // Note this is the 24-bit referenced generator which can be used for compliance 
    // comparisions with 24-bit implementations by defining REFERENCE_RAN_24
    //****************************************************************************
    typedef struct tagRandState {
        I32 iPrior;         // prior value   (only top 24-bits used)
        U32 uiRand;         // current value (only top 24-bits used)
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        // OK to 512 lags, then ChiSquare is suspect at 1024 and rejects at 2048 (length 1024000)
        const U32 uLCa = 69857;         // 0x000110E1
        const U32 uLCc = 3546581;       // 0x00361DD5

        I32 iTemp, iTemp1;
        //a*x + c has to be done with unsigned 32 bit - but we similate a 24x24
        ptRandState->uiRand =  (uLCa * ((ptRandState->uiRand)>>8) + uLCc)<<8;

        // uiRand values starting from a 0 seed are: 0x361dd500, 0x78a60a00, 0xaaac9f00, 0xa0c59400, 0x7104e900, 0xc2fe9e00, 0x1fc6b300

        // do not change the above - see comment in 32-bit generator

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = (((I32)(ptRandState->uiRand)) >> 2) & 0xFFFFFF00; 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += ((iTemp>>2) & 0xFFFFFF00);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
    }


#elif defined(REFERENCE_RAND_16)
    //****************************************************************************
    // Quick and dirty 16-bit rand generator.
    // Simulates 16-bits on a 32-bit reference machine.
    // Note this is the 16-bit referenced generator for compliance comparisions 
    // with 16-bit implementations by defining REFERENCE_RAN_16.
    // The function actually returns 21 random bits at the top of the 32 
    // so is not a pure 16-bit generator, but does limit itself to a 16x16=>32 multiply
    //****************************************************************************

    // Background
    // A pure 16-bit Linear Congruent generator has limited sequence length.  
    // Use 13 A's and 3 C's choosen from good values in the middle of the range.
    // Tested OK up to lag 1024 (length 32768) and pretty good at 2048 and 2049.
    // Sequence length quite long.

    // these two arrays are defined once in dectables.c
#   define LCA_SET 13
#   define LCC_SET 3
    extern const unsigned int g_uWMALCA[LCA_SET];   // {1637,1033,1825,1621, 1657,1861,1229,1549, 2017,941,1409,1777, 1153};
    extern const unsigned int g_uWMALCC[LCC_SET];   // {13849,13841,13859};

    typedef struct tagRandState {
        I32 iPrior;         // prior value
        U16 uiRand;         // current value
        char bIdxLCA;       // index for uLCA
        char bIdxLCC;       // index for uLCC
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        // This 16-bit implementation returns a 32-bit result with the top 21 bits random.
        // But it base implementation is 16x16=>32, e.g. it requires only a single precision 16-bit multiply.

        I32 iTemp, iTemp1;
        U32 uTemp;

        //a*x + c is with unsigned 32 bit - but we similate a 16x16+16 =32
        uTemp =  (g_uWMALCA[ptRandState->bIdxLCA++] * (ptRandState->uiRand)) + g_uWMALCC[ptRandState->bIdxLCC++];

        // save bottom 16 bits
        ptRandState->uiRand  = (unsigned short)uTemp;

        // uiRand values starting from a 0 seed are: 0x3623, 0x259c, 0x5add, 0x5698, 0xb511, 0x78ae, 0x6af9, 0x09f2, 0xc49b, 0x4f3e, 0x4377, 0x1108

        // wrap indexes
        if (ptRandState->bIdxLCA>=LCA_SET) 
            ptRandState->bIdxLCA = 0;
        if (ptRandState->bIdxLCC>=LCC_SET)
            ptRandState->bIdxLCC = 0;

        // get best 19 bits into top
        uTemp <<= 13;           

        // do not change the above - see comment in 32-bit generator

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = (((I32)(uTemp)) >> 2); 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += (iTemp>>2);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
        ptRandState->bIdxLCA = 1;
        ptRandState->bIdxLCC = 2;
    }

#endif

//****************************************************************************
// log base 2 of a number which is a power of 2 
//****************************************************************************
INLINE I32 LOG2(U32 i)
{   // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    U32 iLog2 = 0;
    assert (i != 0);
    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}

#ifndef macintosh
#define log2 LOG2
#endif

//****************************************************************************
// Normalize a dynamically scaled unsigned int 
//****************************************************************************
INLINE void NormUInt( UInt* puiValue, Int* pcFracBits, const UInt uiTarget ){
    const UInt uiTarget2 = uiTarget>>1;
    register UInt uiV = *puiValue;
    register Int cFB = *pcFracBits;
    assert( uiV > 0 );    
    if (uiV == 0) return; // useful if asserts are disabled

    while ( uiV < uiTarget2 )
    {
        uiV <<= 2;        
        cFB += 2;
    }
    if ( uiV < uiTarget )
    {
        uiV <<= 1;        
        cFB += 1;
    }
    *puiValue = uiV;
    *pcFracBits = cFB;
}

//****************************************************************************
// Align a dynamically scaled int to a particular binary point position
//****************************************************************************
INLINE Int Align2FracBits( const Int iValue, const Int cFracBits, const Int cAlignFracBits )
{
    const Int iShift = cFracBits-cAlignFracBits;

    if (iShift < 0) {
        return (iValue << -iShift);
    } else if (iShift < 32) { 
        return (iValue >> iShift);
    } else {
        return 0;
    }
}


// V4 Compatibility Mode: This mode allows us to compare V5 encoder with V4 encoder
#ifdef V4V5_COMPARE_MODE
#define V4V5COMPARE_SNAPTOZERO(x)   if (fabs(x) <= 1.0e-15) {(x) = 0;}
#define ROUNDF_V4V5COMPARE(x)  ROUNDD(x)
typedef double V4V5COMPARE;
#else   // V4V5_COMPARE_MODE
#define V4V5COMPARE_SNAPTOZERO(x)
#define ROUNDF_V4V5COMPARE(x)  ROUNDF(x)
typedef float  V4V5COMPARE;
#endif  // V4V5_COMPARE_MODE

#ifdef INTEGER_ENCODER
typedef I32     FLOATORI32;
typedef U32     FLOATORU32;
typedef U16     FLOATORU16;
#define FLOATORU32_CONST(flop,fb)           ((U32)((flop)*(1 << (fb))))
#define FLOATORINT_TOFLOAT(x,fb)            ((Float)(x)/(Float)(1 << (fb)))
#define FLOATORINT_SELECTTOFLOAT(f,i,fb)    ((Float)(i)/(Float)(1 << (fb)))
#define FLOATORINT_ASSERT(x)                assert(x)
#define FLOATORINT_AB(a,b)                  (a)

// The following is used to allow non-integerized code to convert a FLOP
// result to a bit-discarded integer result
#define FLOATORU32_DISCARDBITSFROMFLT(x,fb) ((U32)((x)/(float)(1 << (fb))))
#define FLOATORU32_DISCARDBITSFROMU32(x,fb) ((U32)(x) >> (fb))
#define FLOATORU32_DISCARDBITSFROMI64(x,fb) ((I64)(x) >> (fb))
#define FLOATORU32_ADDFRACTBITS(i,fb)       ((i) << (fb))
#define FLOATORI32_DISCARDBITSFROMI32(x,fb) ((I32)(x) >> (fb))

#else   // INTEGER_ENCODER

typedef Float   FLOATORI32;
typedef Float   FLOATORU32;
typedef Float   FLOATORU16;
#define FLOATORU32_CONST(flop,fb)           (flop)
#define FLOATORINT_TOFLOAT(x,fb)            (x)
#define FLOATORINT_SELECTTOFLOAT(f,i,fb)    (f)
#define FLOATORINT_ASSERT(x)
#define FLOATORINT_AB(a,b)                  (b)

#define FLOATORU32_DISCARDBITSFROMFLT(x,fb) (x) // FLOP-to-FLOP, no conversion required
#define FLOATORU32_DISCARDBITSFROMU32(x,fb) ((Float)x)
#define FLOATORU32_DISCARDBITSFROMI64(x,fb) (x)
#define FLOATORU32_ADDFRACTBITS(i,fb)       (i) // FLOP, no fract bits required
#define FLOATORI32_DISCARDBITSFROMI32(x,fb) (x)
#endif  // INTEGER_ENCODER


// **************************************************************************
// Bring in platform-specific macros
// **************************************************************************
// Include them here because some of them use the global macros above
#include "wmaOS.h"
#if defined(WMA_TARGET_X86) || defined(WMA_TARGET_QNX_X86)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for Intel x86 Target")
#endif  // COMMONMACROS_OUTPUT_TARGET

//#ifdef __QNX__
#include "../x86/macros.h"
//#else
//#include "..\x86\macros.h"
//#endif

//---------------------------------------------------------------------------
#elif defined(WMA_TARGET_MIPS)

// Output target
#ifdef BUILD_INTEGER
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for MIPS integer Target")
#endif // COMMONMACROS_OUTPUT_TARGET
#else
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for MIPS floating point  Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#endif

#include "..\mips\macros_mips.h"

#elif defined(WMA_TARGET_SH3)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for SH3 (Hitachi & HP WinCE) Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#include "..\sh3\macros_sh3.h"

#elif defined(WMA_TARGET_SH4)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for SH4 (Hitachi WinCE) Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#ifndef HITACHI
static 
#endif
INLINE U32 ByteSwap(U32 i){ 
    U8 *p = (U8 *)&i;
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

#include "..\sh4\macros_SH4.h"

//---------------------------------------------------------------------------
#else

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for ANSI-C Target")
#endif // COMMONMACROS_OUTPUT_TARGET

// Bring in platform-independent macros
#define MULT_HI_DWORD_DOWN(a,b) (I32)((((I64)(a))*((I64)(b)))>>30)
#define MULT_HI_DWORD(a,b) (I32)((((I64)(a))*((I64)(b)))>>32)

// Unsigned version of multiply
#define MULT_HI_UDWORD(a,b) (U32)((((U64)(a))*((U64)(b)))>>32)

static INLINE U32 ByteSwap(U32 i){ 
    U8 *p = (U8 *)&i;
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

#endif // Platform-specific #ifdefs


// **************************************************************************
// Overridable Macros (optimized for some platforms, but not all)
// **************************************************************************
// The following can be overridden within a particular platform macro file
#ifndef PLATFORM_SPECIFIC_ROUND
#ifdef HITACHI
static I32 ROUNDF(float f) {
#else
static INLINE I32 ROUNDF(float f) {
#endif
    if (f < 0.0f)
        return (I32) (f - 0.5f);
    else
        return (I32) (f + 0.5f);
}

//---------------------------------------------------------------------------
#ifdef HITACHI
static I32 ROUNDD(double f) {
#else
static INLINE I32 ROUNDD(double f) {
#endif
    if (f < 0)
        return (I32) (f - 0.5);
    else
        return (I32) (f + 0.5);
}
#endif // PLATFORM_SPECIFIC_ROUND

//---------------------------------------------------------------------------
#ifndef PLATFORM_SPECIFIC_BITCOPY
//assuming bsrc is zeroed out
#ifdef HITACHI
static void bitCpy (const U8* pbSrc, Int iBitStartSrc, Int cBits, U8* pbDst)
#else
static INLINE void bitCpy (const U8* pbSrc, Int iBitStartSrc, Int cBits, U8* pbDst)
#endif
{
    const U8* pbSrcEnd;
    Int iShiftDown;
    U8 b2, b1;

    assert (pbSrc != NULL && pbDst != NULL);
    assert (iBitStartSrc < BITS_PER_DWORD && iBitStartSrc >= 0);
    assert (cBits >= 0);

    // Caller assumes we're copying DWORDs at a time (hangover from Intel)
    // Normalize pointers 
    pbSrc += iBitStartSrc / BITS_PER_BYTE;
    iBitStartSrc %= BITS_PER_BYTE;

    pbSrcEnd = pbSrc + (iBitStartSrc > 0) + 
                            (((cBits - iBitStartSrc) + 7) & ~7) / 8; //open
    iShiftDown = (BITS_PER_BYTE - iBitStartSrc);
    b1 = *pbSrc;
    while (pbSrc < pbSrcEnd) {
        pbSrc++;
        b2 = *pbSrc;
        *pbDst = (b1 << iBitStartSrc) | (b2 >> iShiftDown);
        b1 = b2;
        pbDst++;
    }
}
#endif // PLATFORM_SPECIFIC_BITCOPY

#ifndef PLATFORM_SPECIFIC_FNPTR
#define PLATFORM_SPECIFIC_FNPTR     //nothing for non-x86
#endif // PLATFORM_SPECIFIC_FNPTR


//**********************************************************************
// Support for FastFloat
//**********************************************************************

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
    // FastFloat
    // FastFloat is a quick way of handling values that exceed I32 range without incurring
    // the expense of floating point emulation on integer only platforms.
    // real value = iFraction * pow( 2, -iFracBits )
    // In debugger, iFraction*1.0F/(1<<iFracBits) works if 0<=iFracBits<31

    // Normalize a FastFloat
#ifdef HITACHI  
#   pragma inline(Norm4FastFloat)
#else   
    INLINE
#endif
static void Norm4FastFloat( FastFloat* pfflt )
    {   // use the faster Norm4FastFloatU when you know the value is positive
        register UInt uiF = abs(pfflt->iFraction);
        register Int iFB = 0;
        if ( uiF == 0 )
        {
            pfflt->iFracBits = 0;
            return;
        }
        while ( uiF < 0x1FFFFFFF )
        {
            uiF <<= 2;
            iFB +=  2;
        }
        if ( uiF < 0x3FFFFFFF )
        {
            iFB +=  1;
        }
        pfflt->iFraction <<= iFB;
        pfflt->iFracBits += iFB;
    }


#ifdef HITACHI
#   pragma inline(Norm4FastFloatU)
#else   
    INLINE
#endif
static void Norm4FastFloatU( FastFloat* pfflt )
    {   // same as above when we know value is positive (which we often do)
        register UInt uiF = pfflt->iFraction;
        register Int iFB = 0;
        assert( uiF > 0 );
        while ( uiF < 0x1FFFFFFF )
        {
            uiF  <<= 2;
            iFB +=  2;
        }
        if ( uiF < 0x3FFFFFFF )
        {
            uiF  <<= 1;
            iFB +=  1;
        }
        pfflt->iFraction = uiF;
        pfflt->iFracBits += iFB;
    }


#ifdef HITACHI
    #pragma inline(ffltMultiply)
#else   
    INLINE
#endif  
static FastFloat ffltMultiply( FastFloat ffltA, FastFloat ffltB )
    {
        FastFloat ffltP;
        ffltP.iFraction = MULT_HI( ffltA.iFraction, ffltB.iFraction );
        ffltP.iFracBits = (ffltA.iFracBits + ffltB.iFracBits - 31);
        Norm4FastFloat( &ffltP );
        return ffltP;
    }
#   define FASTFLOAT_MULT(a,b) ffltMultiply((a),(b))


#ifdef HITACHI
    #pragma inline(FastFloatFromFloat)
#else   
    INLINE
#endif  
static FastFloat FastFloatFromFloat(Float flt) {
        FastFloat fflt;
        Float fltScale = (Float)(1<<(31-24));
        fflt.iFracBits = 24;
        while( flt < -fltScale || fltScale < flt )
        {
            flt /= 2;
            fflt.iFracBits -= 1;
            fltScale *= 2;
        }
        fflt.iFraction = (I32)(flt*(1<<fflt.iFracBits));
        Norm4FastFloat( &fflt );
        return fflt;
    }   


#ifdef HITACHI  
    #pragma inline(FloatFromFastFloat)
#else   
    INLINE
#endif
static Float FloatFromFastFloat( FastFloat fflt )
    {
        assert( 0<= fflt.iFracBits && fflt.iFracBits <= 50 );
        if ( fflt.iFracBits > 30 )
            return fflt.iFraction/(1048576.0F*(1<<(fflt.iFracBits-20)));
        else
            return fflt.iFraction/((Float)(1<<fflt.iFracBits));

    }
#   define FASTFLOAT_FROM_FLOAT(flt) FastFloatFromFloat(flt)
#   define FLOAT_FROM_FASTFLOAT(fflt) FloatFromFastFloat(fflt)
#   define DOUBLE_FROM_FASTFLOAT(fflt) ((double)fflt.iFraction/(1<<fflt.iFracBits))


#ifdef HITACHI
    #pragma inline(FastFloatFromU64)
#else   
    INLINE
#endif  
static FastFloat FastFloatFromU64(U64 u64, Int cExp) 
    {
        FastFloat fflt;
        U32 uiMSF = (U32)(u64>>32);
        Int iExp = 0;
        if ( uiMSF==0 ) {
            iExp = 32;
            uiMSF = (U32)u64;
        }
        if (uiMSF==0) {
            fflt.iFracBits = 0;
            fflt.iFraction = 0;
            return fflt;
        }
        // normalize the most significant fractional part
        while( (uiMSF & 0xF0000000)==0 ) {
            iExp += 3;
            uiMSF <<= 3;
        }
        while( (uiMSF & 0xC0000000)==0 ) {
            iExp++;
            uiMSF <<= 1;
        }
        // number of fractional bits
        fflt.iFracBits = iExp+cExp-32;
#if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
        fflt.iFraction = (U32)((u64<<iExp)>>32);
#else
        fflt.iFraction = (iExp>32) ? (U32)(u64<<(iExp-32)) : (U32)(u64>>(32-iExp));
#endif
        return fflt;
    }   
#define FASTFLOAT_FROM_U64(u64,exp) FastFloatFromU64(u64,exp)


    typedef FastFloat QuantStepType;
#define DOUBLE_FROM_QUANTSTEPTYPE(qst) DOUBLE_FROM_FASTFLOAT(qst)
#define  FLOAT_FROM_QUANTSTEPTYPE(qst) FLOAT_FROM_FASTFLOAT(qst)
#define FASTFLOAT_FROM_QUANTSTEPTYPE(qst) (qst)

#else   // must be BUILD_INT_FLOAT

#   define FASTFLOAT_FROM_FLOAT(flt) (flt)
#   define FLOAT_FROM_FASTFLOAT(fflt) (fflt)
#   define FASTFLOAT_MULT(a,b) ((a)*(b))
#   define DOUBLE_FROM_FASTFLOAT(fflt) ((double)fflt)

#   define FASTFLOAT_FROM_U64(u64,exp) (((Float)(u64))/(((U64)1)<<exp))

    typedef Float QuantStepType;
#define DOUBLE_FROM_QUANTSTEPTYPE(qst) ((Double)(qst))
#define  FLOAT_FROM_QUANTSTEPTYPE(qst) (qst)
#define FASTFLOAT_FROM_QUANTSTEPTYPE(qst) ((FastFloat)(qst))


#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Define Macros to switch auReconMono and auSaveHistoryMono between Integer and Float

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)

#define ROUND_SATURATE_STORE(piDst,cf,mn,mx,iTmp)      \
    *piDst = (I16) checkRange (cf, mn, mx);            \
    iTmp = cf;

#else  // BUILD_INT_FLOAT

#ifdef PLATFORM_SPECIFIC_ROUND
// Combined Round and Saturate is faster in floating point
// But if the platform has special Round function, we must use it.
#define ROUND_AND_CHECK_RANGE(it,flt,mn,mx)            \
    it = (I32)ROUNDF_V4V5COMPARE(flt);                 \
    it = checkRange(it,mn,mx);
#else
// Combined Round and Saturate is faster in floating point
#define ROUND_AND_CHECK_RANGE(it,flt,mn,mx)            \
    if ( flt < (V4V5COMPARE)0.0 ) {                    \
        it = (I32)( flt - ((V4V5COMPARE)0.05) );       \
        if ( it < mn ) it = mn;                        \
    } else {                                           \
        it = (I32)( flt + ((V4V5COMPARE)0.05) );       \
        if ( it > mx ) it = mx;                        \
    }
#endif

#define ROUND_SATURATE_STORE(piDst,cf,mn,mx,iTmp)      \
    ROUND_AND_CHECK_RANGE( iTmp, cf, mn, mx );         \
    *piDst = (I16)iTmp;
        
#endif // Both BUILD_INTEGER and BUILD_INT_FLOAT

#if defined (SMOOTH_NMR) && !defined (PEAQ_MASK)
// We must always use PEAQ_MASK for SMOOTH_NMR to function. However, PEAQ_MASK can be used with older rate control as well.
#define PEAQ_MASK
#endif

#ifdef _XBOX
#define XFILE void
#define STDOUT (void *)
extern XFILE * _XFOPEN(char *filename, char *mode);
extern int  _XFSEEK(XFILE *strmnum, long offset, int origin);
extern int  _XFWRITE(const void *buffer, int size, int count, XFILE *strmnum);
extern long  _XFTELL(XFILE *strmnum);
extern int  _XFCLOSE(XFILE *strmnum);
extern int  _XFFLUSH();
extern int  _XFPRINTF(XFILE *strmnum, const char *format, ...);
#define FOPEN _XFOPEN
#define FSEEK _XFSEEK
#define FTELL _XFTELL
#define FCLOSE _XFCLOSE
#define FWRITE _XFWRITE
#define FFLUSH _XFFLUSH
#define FPRINTF _XFPRINTF
#else
#define XFILE FILE
#define STDOUT stdout
#define FOPEN fopen
#define FSEEK fseek
#define FTELL ftell
#define FCLOSE fclose
#define FWRITE fwrite
#define FFLUSH fflush
#define FPRINTF fprintf
#endif
#endif  // __COMMON_MACROS_H

