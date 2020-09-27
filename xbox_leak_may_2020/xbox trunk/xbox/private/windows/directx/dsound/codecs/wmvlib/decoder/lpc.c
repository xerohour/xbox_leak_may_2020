/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.cpp

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 16-November-1998

Revision History:

    Sil Sanders (sils) 8-Feb-00 - combine Integer and Float versions and simplify


*************************************************************************/
#if defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)

#include "bldsetup.h"

#if !defined(_WIN32_WCE) && !defined(HITACHI)
#include <time.h>
#endif  // _WIN32_WCE
#include <math.h>
#include <limits.h>
#include "msaudio.h"
#include "stdio.h"
#include "AutoProfile.h"
#include "float.h"


// *** one of these may be set by platform in macros.h ***
//// in performance figures an X86 tick = 1/1193182 seconds = 838 ns (500MHz Pentium Dell 610)
//// and a SH4 Tick = 80ns (Dreamcast SH4 200Mhz)
//// X86 tested with wmadec_s Release build w/ PROFILE and wmatest_sh4 Release Build with PROFILE

// **************** warning ***************************
// every time you change one of these PLATFORM_LPC defines below, you must rebuilt all!
// ****************************************************

// A pruned or decomposed FFT - fast (old default)
// But uses an excess amount of cache
//#define PLATFORM_LPC_PRUNED_NONREDUNDANT
//// X86 Tough_32s_22:  1,376,277               Tough_16m_16:    556,708
//// SH4 Tough_32s_22: 22,842,016               Tough_16m_16:  8,488,652

// A pruned or decomposed FFT - both fast and V4 numerically compatabible
//#define PLATFORM_LPC_PRUNED_STRAIGHT
//// X86 Tough_32s_22:  1,490,909               Tough_16m_16:    545,381
//// SH4 Tough_32s_22: 23,573,152               Tough_16m_16:  8.603.243

// PLATFORM_LPC_DIRECT using trig recurrsion
//#define PLATFORM_LPC_DIRECT
//// X86 Tough_32s_22:  5,300,299               Tough_16m_16:  1,533,753
//// SH4 Tough_32s_22: 43,303,596               Tough_16m_16: 12,927,913

// PLATFORM_LPC_LITDFT is theoretically the fastest but isnt.  
// Above methods are very clever but this does limited damage to the cache.
//#define PLATFORM_LPC_LITDFT
//// X86 Tough_32s_32:  2,868,037 = 1,281,738+1,560,625+1,186,050-1,160,376
//// SH4 Tough_32s_32: 41,381,752 

// PLATFORM_LPC_FOLDED is derived from ARM's implementation
//#define PLATFORM_LPC_FOLDED

// PLATFORM_LPC_FOLDED_PRUNED uses ARM's Folded Trick with otherwise PLATFORM_LPC_PRUNED_NONREDUNDANT
// Fast and uses little extra memory and so is the default
//#define PLATFORM_LPC_FOLDED_PRUNED

#if !(defined(PLATFORM_LPC_PRUNED_NONREDUNDANT) || defined(PLATFORM_LPC_DIRECT) \
   || defined(PLATFORM_LPC_LITDFT) || defined(PLATFORM_LPC_PRUNED_STRAIGHT)     \
   || defined(PLATFORM_LPC_FOLDED) || defined(PLATFORM_LPC_FOLDED_PRUNED) )
//  set the default if none is otherwise selected
#   define PLATFORM_LPC_FOLDED_PRUNED
#endif

#if defined(PI) != (defined(PLATFORM_LPC_PRUNED_NONREDUNDANT) +defined(PLATFORM_LPC_DIRECT) \
                  + defined(PLATFORM_LPC_LITDFT) + defined(PLATFORM_LPC_PRUNED_STRAIGHT)    \
                 +  defined(PLATFORM_LPC_FOLDED) + defined(PLATFORM_LPC_FOLDED_PRUNED))
#       pragma COMPILER_MESSAGE(__FILE__ "(84) : Warning - Multiple LPC Platforms selected.")
#endif

#if defined(PLATFORM_LPC_FOLDED_PRUNED)
// use a smaller trig table and calculate the 6 lpc constants from base sin/cos values
// so far, only implemented for PLATFORM_LPC_FOLDED_PRUNED
#   if !(defined(PLATFORM_LPC_SMALL_TRIG_TABLE) || defined(PLATFORM_LPC_NO_TRIG_TABLE))
// these two alternatives are essentially the same speed on the SH3.  NO_TRIG_TABLE saves rom/cache space.
//#     define PLATFORM_LPC_SMALL_TRIG_TABLE
#       define PLATFORM_LPC_NO_TRIG_TABLE
#   endif
#endif

// LPC in V4 has a couple of bugs 
// Define the symbol below to replicate that bug as well as V4 style computations for comparision
// Can be used with any of the methods above
//#define REPLICATE_V4_LPC

#if defined(V4V5_COMPARE_MODE) && !defined(REPLICATE_V4_LPC)
#define REPLICATE_V4_LPC
#endif

//#define REPLICATE_ARM_NON_TRUNCATED_LPC

#define MAX_LP_SPEC_SIZE 2048


#if defined(V4V5_COMPARE_MODE) && defined(BUILD_INT_FLOAT) && 0

// To be sure, use old version of lpc in lpc_float.c
// no longer exists #include        "lpc_float.c"

#else // so not V4V5_COMPARE_MODE 


#include "lpcConst.h"

//function prototype, used to eliminate Mac warnings
Void prvPq2lpc(CAudioObject* pau, LpType* p, LpType* q, LpType* lpc, Int order);
Void prvConvolve_odd(CAudioObject* pau, LpType* in1, Int l1, LpType* in2, Int l2, LpType* out, Int *lout);
Void prvConvolve(CAudioObject* pau, LpType *in1, Int l1, LpType *in2, Int l2, LpType *out, Int *lout);
Void prvLsp2pq(CAudioObject* pau, U8 *lsfQ, LpType *p, LpType *q, Int order);


// *****************************************************************************
//
// Conversion from LSF thru Parcor to LPC
//
// *****************************************************************************

// Parcor to LPC conversion
Void prvPq2lpc(CAudioObject* pau, LpType* p, LpType* q, LpType* lpc, Int order)
{
  I32 i;
  
  for(i=1; i<=order/2; i++) {
    lpc[i-1]= -DIV2(p[i]+q[i]);
    lpc[order-i]= -DIV2(p[i]-q[i]);
  }
}

/* symmetric convolution */
Void prvConvolve_odd(CAudioObject* pau, LpType* in1, Int l1, LpType* in2, Int l2, LpType* out, Int *lout)
{
  // In BUILD_INTEGER mode, in1 assumed to have FRACT_BITS_LSP and in2 to have FRACT_BITS_LP. Output will have
  // FRACT_BITS_LP.
  I32   i,j;
  LpType ret[100];
  
  for (i=0; (i<l1)&&(i<l2); i++) {
    ret[i]=0;
    for (j=0; j<=i; j++) {
      ret[i] += MULT_LSP(in1[j],in2[i-j]);
    }
  }
  
  for (i=l1; i<(l1+l2)/2; i++) {
    ret[i]=0;
    for (j=0; j<l1; j++) {
      ret[i] += MULT_LSP(in1[j],in2[i-j]);
    }
  }
  
  *lout=(l1+l2)-1;
  for (i=0; i<(*lout)/2; i++) {
    out[i]=ret[i];
    out[(*lout-1)-i]=ret[i];
  }
  out[*lout/2]=ret[*lout/2];
}

/* symmetric convolution */
Void prvConvolve(CAudioObject* pau, LpType *in1, Int l1, LpType *in2, Int l2, LpType *out, Int *lout)
{
  // In BUILD_INTEGER mode, in1 assumed to have FRACT_BITS_LSP and in2 to have FRACT_BITS_LP. Output will have
  // FRACT_BITS_LP.
  I32   i,j;
  LpType ret[100];
  
  for (i=0; (i<l1)&&(i<l2); i++) {
    ret[i]=0;
    for (j=0; j<=i; j++) {
      ret[i] += MULT_LSP(in1[j],in2[i-j]);
    }
  }
  
  for (i=l1; i<(l1+l2)/2; i++) {
    ret[i]=0;
    for (j=0; j<l1; j++) {
      ret[i] += MULT_LSP(in1[j],in2[i-j]);
    }
  }
  
  *lout=(l1+l2)-1;
  for (i=0; i<(*lout)/2; i++) {
    out[i]=ret[i];
    out[(*lout-1)-i]=ret[i];
  }
}

#if defined(REPLICATE_V4_LPC) && defined(BUILD_INT_FLOAT)
#define LSF_DECODE(i,lsfQ) -2.0F*(Float)cos(2.0*PI*(g_rgfltLsfReconLevel [i] [lsfQ[i]]) )
#define LP_DECODE(i,lsfQ)  LSF_DECODE((i),(lsfQ))
#else
#define LSF_DECODE(i,lsfQ) g_rgiLsfReconLevel[i][lsfQ[i]]
#if defined(BUILD_INTEGER)
#define LP_DECODE(i,lsfQ) (g_rgiLsfReconLevel[i][lsfQ[i]] >> (FRACT_BITS_LSP-FRACT_BITS_LP))
#else
#define LP_DECODE(i,lsfQ) LSF_DECODE((i),(lsfQ))
#endif // BUILD_INTEGER
#endif

// Only updates
#define ARRAY_RANGE_FMAX(a,b,c,d,init,i) if ((init)) d = -FLT_MAX; \
for ((i)=(b);(i)<(c);(i)++) (d) = ((d) < (a)[i]) ? (a)[i] : (d);
#define ARRAY_RANGE_FMIN(a,b,c,d,init,i) if ((init)) d = FLT_MAX; \
for ((i)=(b);(i)<(c);(i)++) (d) = ((d) > (a)[i]) ? (a)[i] : (d);

// Quantized LSF to PARCOR

Void prvLsp2pq(CAudioObject* pau, U8 *lsfQ, LpType *p, LpType *q, Int order)
{
    I32     i;
    LpType  long_seq[100];
    LspType short_seq[3];
    Int     long_length, short_length;
    
#if 0 // only to create preload for g_rgiLsfReconLevel in lpcconst.h
    static int first = 1;
    if (first)
    {
        for( i = 0; i<LPCORDER; i++ )
        {
            printf("LP_CONST16(");
            for( first = 0; first < 15; first++ )
                printf("%.10f,", g_rgfltLsfReconLevel [i] [first] != 0 ? -2.0F*(Float)cos(2.0*PI*(g_rgfltLsfReconLevel [i] [first]) ) : 0.0f );
            printf("%.10f),\n", g_rgfltLsfReconLevel [i] [first] != 0 ? -2.0F*(Float)cos(2.0*PI*(g_rgfltLsfReconLevel [i] [first]) ) : 0.0f );
        }
        first = 0;
    }
#endif
    
    short_length = 3; 
    long_length  = 2;
    
    short_seq[0] = short_seq[2] = LSP_FROM_FLOAT(1);
    long_seq[0]  = long_seq[1]  = LP_FROM_FLOAT(1);
    
    for (i = 0; i < order; i+= 2) {
        short_seq[1]= LSF_DECODE(i,lsfQ);   // -2.0F*(Float)cos(2.0*PI*lsp[i])
        prvConvolve(pau, short_seq, short_length, long_seq, long_length, 
            long_seq, &long_length);
    }
    
    for (i=1; i<=order/2; i++) {
        p[i] = long_seq[i];
    }
    
    long_length = 3;
    long_seq[0] = LP_FROM_FLOAT(1);
    long_seq[1] = LP_DECODE(1,lsfQ);             // -2.0F*(Float)cos(2.0*PI*lsp[1])
    long_seq[2] = LP_FROM_FLOAT(1);
    
    for (i = 3; i < order; i += 2) {
        short_seq[1]= LSF_DECODE(i,lsfQ);   // -2.0F*(Float)cos(2.0*PI*lsp[i])
        prvConvolve_odd(pau, short_seq, short_length, long_seq, long_length, 
            long_seq, &long_length);
    }
    
    for (i=1; i<=order/2; i++) {
        q[i]=long_seq[i]-long_seq[i-1];
    }
}

// Quantized LSF to LPC
Void    auLsp2lpc(CAudioObject* pau, U8 *lsfQ, LpType *lpc, Int order)
{
  LpType p[LPCORDER+2],q[LPCORDER+2];
  
  prvLsp2pq(pau, lsfQ,p,q,order);
  
  prvPq2lpc(pau, p,q,lpc,order);
}



// ************************************************************************************
//
// InverseQuadRoot(x) = (1/x)^(1/4) 
// where the 1/4 is one squareroot combined with the flattenfactor
//
// Do this three different ways:  Encoder, Integer Decoder, IntFloat Decoder
//
// ************************************************************************************

#ifdef BUILD_INTEGER

#ifndef LPCPOWER32

U8 gLZLTable[128];

void gLZLTableInit(void)
{
   int i,j;
   U8 mask;

   for(i=0;i<128;i++)
   {
	   mask=0x80;
	   for(j=0;j<8;j++)
	   {
         if((2*i)&mask)
			 break;
		 mask>>=1;
	   }

	   
	   gLZLTable[i]=j;

       mask=0x80;
	   for(j=0;j<8;j++)
	   {
         if((2*i+1)&mask)
			 break;
		 mask>>=1;
	   }

       gLZLTable[i]|=j<<4;
   }

}
#endif


#else   // so must be BUILD_INT_FLOAT

//Tables for inverse quad root calculation
UInt g_InvQuadRootExponent[0x100];
UInt g_InvQuadRootMantissa[1<<(PRECESSION_BITS_FOR_INVQUADROOT)];

//Build tables for fast inverse quad root calculation
Void prvInitInverseQuadRootTable (CAudioObject* pau)
{
    //Iterate through floating point numbers with 12 bits of presition
    //The folowing loop is equivalent to 
    //for (Float i = 1.0; i < 1.999756;i+=1.000244)
    Int i;
    for (i = 0; i < 1<<(PRECESSION_BITS_FOR_INVQUADROOT); i++){          
        Int fltInt = 0x3F800000 | (i << (23-PRECESSION_BITS_FOR_INVQUADROOT));//Build floating point      
        Float f = *(Float *)&fltInt;                       //number in the form of 1.i * pow(2,0);
        Float invQuadRoot = (Float)(1/sqrt(sqrt(f)));  //Calculate inverse quad root
        fltInt = *(Int *)&invQuadRoot;          
        g_InvQuadRootMantissa[i] = fltInt;             //Store value in table
    }
    
    //Iterate through floating point exponents from pow(2,-126)...pow(2,127) by powers of 2
    for (i = 1; i < 255; i++){
        Int fltInt = (i << 23);                             //Build floating point number in the form
        Float f = *(Float *)&fltInt;            //of 1.0 * pow(2,(i-125))
        f = (Float)(1/sqrt(sqrt(f)));           //calculate inverse quad root / 2
        g_InvQuadRootExponent[i] = *(Int *)&f;  //Store in table   
    }
    SETUP_LPC_SPEC_TRIG(pau);
}

INLINE WeightType InverseQuadRootF(Float f){
    Float frac = (*(Float *)&(g_InvQuadRootMantissa[((*(Int *)&f) &   0x7FFFFF) >> (23-PRECESSION_BITS_FOR_INVQUADROOT)]));
    Float exp  = (*(Float *)&(g_InvQuadRootExponent[((*(Int *)&f) & 0x7F800000) >> 23]));
    return (frac * exp);
}

INLINE WeightType InverseQuadRootI(Int f){
    Float frac = (*(Float *)&(g_InvQuadRootMantissa[(f &   0x7FFFFF) >> (23-PRECESSION_BITS_FOR_INVQUADROOT)]));
    Float exp  = (*(Float *)&(g_InvQuadRootExponent[(f & 0x7F800000) >> 23]));
    //fprintf(stdout, "%.20lf\n", ((double)*(Float*)(&f)));
    //fprintf(stdout, "%.20lf\n", ((double)frac * (double)exp));
    return (frac * exp);
}

// there appears to be no effect of calling The I suffix or the F suffix.  
// lpc_float.c called the I suffix.
#define InverseQuadRoot(X) InverseQuadRootI(*(Int*)&X)

#endif // of BUILD_INTEGER and BUILD_INT_FLOAT variations


// *****************************************************************************
//
// Support for Integer and Float versions of LPC to Spectrum 
//
// *****************************************************************************

#if defined(BUILD_INTEGER)
 
INLINE LpSpecPowerType square(LpSpecType x) {
    /* x must be less than sqrt(2^12)*2^19 to avoid overflow */
    if ( abs(x) < 0x01000000 )
        return (LpSpecPowerType) 
        (MULT_HI_DWORD(x<<((BITS_PER_DWORD-FRACT_BITS_LP_SPEC)>>1),x<<((BITS_PER_DWORD-FRACT_BITS_LP_SPEC+1)>>1)));
    return (LpSpecPowerType) 
        (MULT_HI_DWORD(x,x) << (BITS_PER_DWORD-FRACT_BITS_LP_SPEC));
}

INLINE LpSpecPowerType square64(LpSpecType x) {
    return (LpSpecPowerType)(((U64)x * (U64)x) >> (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER));
}

#if defined(_DEBUG)

static LpSpecPowerType DebugSquare(LpSpecType x) {
    UInt y;
    Float f2 = (float)x/(LP_SPEC_SCALE);
    Float f3 = f2*f2;
    Float f4 = f3*LP_SPEC_SCALE;
    Float dif,rel;
    y = (UInt) square(x);

    dif = (Float)fabs(f4-y);
    rel = (Float)fabs(dif/f4);
    if ((f4 > 10.0f) && (rel > .2f)) {
        assert(WMAB_FALSE);
    }
    return y;
}


static Int MultBp2Debug(int x, int y) {
    Float fx,fy,fa,rel,dif;
    Int a;
    a = MULT_BP2(x,y);
    fx = (Float)x;
    fy = (Float)y;
    fa = fx*fy/(1<<30);
    dif = (Float)fabs(fa-a);
    rel = (Float)fabs(dif/fa);
    if ((fa > 5.0f) && (rel > .5f)) {
        assert(WMAB_FALSE);
    }
    return a;
}

#if defined (LPCPOWER32)
#   define SQUARE(x) DebugSquare(x)
#   define SUM_SQUARES(x,y) (DebugSquare(x)+DebugSquare(y))
#elif defined(U64SQUARE32SR)
#   define SQUARE(x) U64SQUARE32SR(x, (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER))
#ifdef U64SUM_SQUARES32SR
#   define SUM_SQUARES(x,y) U64SUM_SQUARES32SR(x,y, (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER))
#else
#   define SUM_SQUARES(x,y) (square64(x)+square64(y))
#endif
#else 
#   define SQUARE(x) square64(x) 
#   define SUM_SQUARES(x,y) (square64(x)+square64(y))
#endif // LPCPOWER32
#   define MULT_BP2X(x,y) MultBp2Debug(x,y)

#else // so not _DEBUG

#if defined (LPCPOWER32)
#   define SQUARE(x) square(x)
#   define SUM_SQUARES(x,y) (square(x)+square(y))
#elif defined(U64SQUARE32SR)
#   define SQUARE(x) U64SQUARE32SR(x, (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER))
#ifdef U64SUM_SQUARES32SR
#   define SUM_SQUARES(x,y) U64SUM_SQUARES32SR(x,y, (2 * FRACT_BITS_LP_SPEC - FRACT_BITS_LP_SPEC_POWER))
#else
#   define SUM_SQUARES(x,y) (square64(x)+square64(y))
#endif
#else
#   define SQUARE(x) square64(x) 
#   define SUM_SQUARES(x,y) (square64(x)+square64(y))
#endif // LPCPOWER32
#   define MULT_BP2X(x,y) MULT_BP2(x,y)

#endif  // end of _DEBUG and not _DEBUG variations

#else // so must be BUILD_INT_FLOAT

#   define SQUARE(x) ((x)*(x))
#   define SUM_SQUARES(x,y) ((x)*(x)+(y)*(y))
#   define MULT_BP2X(x,y) ((x)*(y))

#endif

#if defined(REPLICATE_V4_LPC)
#       define SQRT2_2  BP2_FROM_FLOAT(0.7071067811865)
#       define SQRT2    BP2_FROM_FLOAT(1.414213562373)
#else
#       define SQRT2_2  BP2_FROM_FLOAT(0.70710678118654752440084436210485)
#       define SQRT2    BP2_FROM_FLOAT(1.4142135623730950488016887242097)
#endif

// Main LpcSpectrum function only calls the following function which can be optimized on a platform-by-platform basis
#if defined(PLATFORM_SPECIFIC_INVERSEQUADROOT)
extern WeightType InverseQuadRootOfSumSquares(LpSpecType F1, LpSpecType F2 );
#else

#if defined(BUILD_INTEGER) && !defined(LPCPOWER32)
// here we have a limited (256) size lookup table and will interpolate values between them.
// note that two times we call this with pF2==NULL, for F[0] and for F[2048>>xxx]

#if 0
U8 gLZLTable[128];

void gLZLTableInit(void)
{
   int i,j;
   U8 mask;

   for(i=0;i<128;i++)
   {
	   mask=0x80;
	   for(j=0;j<8;j++)
	   {
         if((2*i)&mask)
			 break;
		 mask>>=1;
	   }

	   
	   gLZLTable[i]=j;

       mask=0x80;
	   for(j=0;j<8;j++)
	   {
         if((2*i+1)&mask)
			 break;
		 mask>>=1;
	   }

       gLZLTable[i]|=j<<4;
   }

}

#endif


#if defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER)


#include "macros_mips_lpc.h"

#else

//INLINE
WeightType InverseQuadRootOfSumSquares(LpSpecType F1, LpSpecType F2 )
{
    // LpSpecPowerType is U64.
    LpSpecPowerType f = SUM_SQUARES( F1, F2 );
    U32 uiMSF = (U32)(f>>32);
    Int iExp = 0;
    int iMSF8;
    U32 uiFrac1;
	U8 index;
    if ( uiMSF==0 ) {
        iExp = 32;
        uiMSF = (U32)f;
    }
    if (uiMSF==0) 
        return(0xFFFFFFFF);
    // normalize the most significant fractional part
    while( (uiMSF & 0xFf000000)==0 ) {
        iExp += 8;
        uiMSF <<= 8;
    }


	index=(U8)(uiMSF>>24);
	iExp+=((gLZLTable[index>>1]>>((index&1)<<2)))&0xf;


    // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
    iExp++;
    //MONITOR_COUNT(gMC_ScaleInverseQuadRoot,iExp);
    MONITOR_RANGE(gMC_ScaleInverseQuadRoot,iExp);
    // get all 32 bits from source
#if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
    uiMSF = (U32)((f<<iExp)>>32);
#else
    uiMSF = (iExp>32) ? (U32)(f<<(iExp-32)) : (U32)(f>>(32-iExp));
#endif
    // split into top INVQUADROOT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
    iMSF8 = uiMSF>>(32-INVQUADROOT_FRACTION_TABLE_LOG2_SIZE);
    uiMSF <<= INVQUADROOT_FRACTION_TABLE_LOG2_SIZE;
    // lookup and interpolate - tables are set up to return correct binary point for WeightType
    uiFrac1  = g_InvQuadRootFraction[iMSF8++];
    uiFrac1 -= MULT_HI_UDWORD( uiMSF, uiFrac1 - g_InvQuadRootFraction[iMSF8] );
    return MULT_HI_UDWORD( uiFrac1, g_InvQuadRootExponent[ iExp ] );
}

#endif

#else   // defined(BUILD_INTEGER) && !defined(LPCPOWER32)

INLINE WeightType InverseQuadRootOfSumSquares(LpSpecType F1, LpSpecType F2 )
{
    LpSpecPowerType f = SUM_SQUARES( F1, F2 );
    return InverseQuadRoot( f );
}

#endif  // defined(BUILD_INTEGER) && !defined(LPCPOWER32)

#endif  // PLATFORM_SPECIFIC_INVERSEQUADROOT

#if defined(_DEBUG) && defined(LPC_COMPARE)  //compare results

#   ifdef UNDER_CE
        extern void WMADebugMessage(const char* pszFmt, ... );
#       define MYPRINTF WMADebugMessage
#   else
#       define MYPRINTF printf
#   endif

void LpcCompareDebug( CAudioObject* pau, Int iFreq, Int iF, Int iT, LpSpecType* F, const LpType* rgLpcCoef )
{
    //to compare results find the line that reads
    //Float fltTemp = (iFreq + 0.5F) * PI / pau->m_cSubband;
    //and remove the + 0.5F
    static Float max = 0.001f;  //.00001f;
    static Int cnt = 0;
    static Float sum = 0;
    Float old_way, new_way;
    Float R = 1;
    Float I = 0;
    int j;
    cnt++;
    for (j = 0; j < LPCORDER; j++)  
    {
        R -= FLOAT_FROM_LP(rgLpcCoef [j]) * pau->m_rgfltLpsSpecCos [j] [iT];
        I -= FLOAT_FROM_LP(rgLpcCoef [j]) * pau->m_rgfltLpsSpecSin [j] [iT];
    }
    old_way = (Float)(1/pow((R * R + I * I),pau->m_fltFlatenFactor/2));
#   if defined(PLATFORM_LPC_FOLDED) || defined(PLATFORM_LPC_PRUNED_NONREDUNDANT) || defined( PLATFORM_LPC_FOLDED_PRUNED)
        new_way = FLOAT_FROM_WEIGHT(F[iF]);
#   else
        new_way = FLOAT_FROM_WEIGHT(InverseQuadRoot(F[iF]));
#   endif
    sum += (Float)fabs(old_way-new_way);
    if (fabs(old_way-new_way)  > max) 
    {
        max = (Float)fabs(old_way-new_way);
        MYPRINTF("max error=%f, %%=%f, Frame=%4d, iFreq=%4d, Size=%4d, ave err=%f\n", 
            max, 100*max/old_way, pau->m_iFrameNumber, iFreq, 
#           ifdef REPLICATE_V4_LPC
                pau->m_cFrameSampleHalf, 
#           else
                (pau->m_fV5Lpc) ? pau->m_cSubband : pau->m_cFrameSampleHalf, 
#           endif
            sum/cnt );
    }
}
#define LPC_COMPARE_DEBUG( pau, iFreq, iF, iT, F, rgLpcCoef ) LpcCompareDebug( pau, iFreq, iF, iT, F, rgLpcCoef )
#pragma COMPILER_MESSAGE(__FILE__ "(704) : Warning - LPC_COMPARE Mode - output to stdout!")
#else
#define LPC_COMPARE_DEBUG( pau, iFreq, iF, iT, F, rgLpcCoef )
#endif

// define LPC_SPEC_PRINT_FRAME to get a printout of particular frame
//#define LPC_SPEC_PRINT_FRAME 2
#if defined(_DEBUG) && defined(LPC_SPEC_PRINT_FRAME)
#define LPC_SPEC_PRINT(a,b,c,d,e)                               \
        if ( pau->m_iFrameNumber == LPC_SPEC_PRINT_FRAME )      \
                printf("%4d  %d  %4d  %10.6f  %10.6f\n",a,b,c,d,e);
#else
#define LPC_SPEC_PRINT(a,b,c,d,e)
#endif

// define LPC_FT_PRINT_FRAME to get a printout of particular frame
//#define LPC_FT_PRINT_FRAME 2
#if defined(_DEBUG) && defined(LPC_FT_PRINT_FRAME)
#define LPC_FT_PRINT(a,b,c,d,e,f)                               \
        if ( pau->m_iFrameNumber == LPC_FT_PRINT_FRAME )        \
                printf("%4d  %d  %4d  %10.6f  %10.6f  %10.6f\n",a,b,c,d,e,f);
#else
#define LPC_FT_PRINT(a,b,c,d,e,f)
#endif

//#define PRINT_WEIGHT_FACTOR 100000000
#	ifdef PRINT_WEIGHT_FACTOR
		// this function must be defined in fft.c
		extern void DebugDctPrintCoefs(CAudioObject* pau, int def, int id, float fac, CoefType* pfCoef);
#		define WF_PRINT(pfCoef)  DebugDctPrintCoefs(pau,PRINT_WEIGHT_FACTOR,4,1.0f*(1<<WEIGHTFACTOR_FRACT_BITS),pfCoef)
#   else
#       define WF_PRINT(pfCoef)
#   endif

#if defined(PLATFORM_LPC_FOLDED_PRUNED) && !defined(V4V5_COMPARE_MODE)

// ************************************************************************************
// ************************* This is the one we normally use **************************
// ************************************************************************************

//#pragma COMPILER_MESSAGE(__FILE__ "(735) : Warning - building PLATFORM_LPC_FOLDED_PRUNED LPC spectrum")

// ************************************************************************************
//
// LPC to Spectrum using a "Pruned" FFT and redundant calculation removal
//
// See Sorensen & Burrus, IEEE Trans Signal Processing V41 #3 March 93
// and the references sited there for a treatment of pruned and Transform-decomposited DFT
// Unclear how this particular implementation relates to those articles.
//
// Originally designed and implemented by Wei-ge and Marc.  
// Restructured by Sil to be a single routine.  
// Later restructured again to use only a small stack array (inspired by ARM's technique).
//
// The use of pointers instead of array indexes speeds up the SH4 and has no effect on the X86
//
// Cache usage: constants 14kb (note SH3 cache size is 16kb and we need 8k of WF plus constants)
//
// ************************************************************************************


#if defined(PLATFORM_LPC_NO_TRIG_TABLE) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)

#if defined(PLATFORM_SPECIFIC_DOLPC4)

extern void prvDoLpc4( const Int k, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, const BP2Type S1, const BP2Type C1 );

#else // defined(PLATFORM_SPECIFIC_DOLPC4)

#if !(defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER))
void prvDoLpc4( const Int k, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, const BP2Type S1, const BP2Type C1 ) 
{
    BP2Type    CmS, CpS;
    LpSpecType T2, T4, T6, T7, T8, T9, TA, TB;
    LpSpecType D, E;

    BP2Type    C2, S2, C3, S3;

//#define DOLPC4_BREAK 243
#if defined(DOLPC4_BREAK) && defined(_DEBUG)
    if ( k==DOLPC4_BREAK || (iSizeBy2-k)==DOLPC4_BREAK || (iSizeBy2+k)==DOLPC4_BREAK || ((iSizeBy2<<1)-k)==DOLPC4_BREAK )
    {
        DEBUG_BREAK();
    }
    
#endif
    CmS = C1 - S1;                              // cnst4[i*6+4];
    CpS = C1 + S1;                              // cnst4[i*6+5];
    assert( BP2_FROM_FLOAT(1.0) <= CpS && CpS < BP2_FROM_FLOAT(1.5) );
    T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3]);     // F[2048+j]    F[3072-j]
    T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3]);     // F[2048+j]    F[3072-j]

    S2 = MUL2(MULT_BP2X(C1,S1));                        // sin(2x)
    C2 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S1,S1)); // cos(2x)
    CmS = C2 - S2;                              // cnst4[i*6+2];
    CpS = C2 + S2;                              // cnst4[i*6+3];
    T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1]);     // F[1024+j]    F[2048-j]
    T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]);     // F[1024+j]    F[2048-j]

    S3 = MULT_BP2X(S1,C2) + MULT_BP2X(C1,S2);   // sin(3x) = sin(x+2x)
    C3 = MULT_BP2X(C1,C2) - MULT_BP2X(S1,S2);   // cos(2x) = cos(x+2x)
    CmS = C3 - S3;                              // old cnst4[i*6];
    CpS = C3 + S3;                              // old cnst4[i*6+1];
    T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5]);     // F[3072+j]    F[4096-j]
    T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5]);     // F[3072+j]    F[4096-j]

    TA = pTmp[6] + pTmp[7];                                   // F[j]      +  F[1024-j];
    TB = pTmp[6] - pTmp[7];                                   // F[j]      -  F[1024-j];

    D  = DIV2(+ T7  + T8 + T9 + TA);
    E  = DIV2(+ T4  + T6 + T2 + TB);
    pWF[k]           = InverseQuadRootOfSumSquares(D, E);              // F[j]

    D  = DIV2(- T7  + T6 - T2 + TA);
    E  = DIV2(+ T4  + T8 - T9 - TB);
    pWF[iSizeBy2-k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024-j]

    D  = DIV2(- T7  - T6 + T2 + TA);
    E  = DIV2(- T4  + T8 - T9 + TB);
    pWF[iSizeBy2+k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024+j]

    D  = DIV2(- T4  + T6 + T2 - TB);
    E  = DIV2(+ T7  - T8 - T9 + TA);
    pWF[(iSizeBy2<<1)-k]  = InverseQuadRootOfSumSquares(D, E);         // F[2048-j]

    //INTEGER_ONLY( assert( F[j]>=0 && F[(1024>>iShrink)-j]>=0 && F[(1024>>iShrink)+j]>=0 && F[(2048>>iShrink)-j]>=0 ) );

    //** This block uses 36 adds and 20 mults plus either 4 more mults or 4 shifts
}
#else
void (*prvDoLpc4)( const Int k, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, const BP2Type S1, const BP2Type C1 ) ;
#endif

#endif  // defined(PLATFORM_SPECIFIC_DOLPC4)
#else   // defined(PLATFORM_LPC_NO_TRIG_TABLE) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)

void prvDoLpc4( const Int k, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, const Int iShrink ) 
{
    const BP2Type *pC4;
    BP2Type    CmS, CpS;
    LpSpecType T2, T4, T6, T7, T8, T9, TA, TB;
    LpSpecType D, E;


    // if we folded the constants, we could just increment rather than recalculate this pointer
    pC4 = &lpc_cnst4[k*(6<<iShrink)];

    CmS = *pC4++;   // old cnst4[i*6];
    CpS = *pC4++;   // old cnst4[i*6+1];
    T9 = MULT_BP2X(CmS,pTmp[4]) + MULT_BP2X(CpS,pTmp[5]);     // F[3072+j]    F[4096-j]
    T2 = MULT_BP2X(CpS,pTmp[4]) - MULT_BP2X(CmS,pTmp[5]);     // F[3072+j]    F[4096-j]

    CmS = *pC4++;   // cnst4[i*6+2];
    CpS = *pC4++;   // cnst4[i*6+3];
    T7 = MULT_BP2X(CmS,pTmp[0]) + MULT_BP2X(CpS,pTmp[1]);     // F[1024+j]    F[2048-j]
    T4 = MULT_BP2X(CpS,pTmp[0]) - MULT_BP2X(CmS,pTmp[1]);     // F[1024+j]    F[2048-j]

    CmS = *pC4++;   // cnst4[i*6+4];
    CpS = *pC4;     // cnst4[i*6+5];
    assert( BP2_FROM_FLOAT(1.0) <= CpS && CpS < BP2_FROM_FLOAT(1.5) );
    T8 = MULT_BP2X(CmS,pTmp[2]) + MULT_BP2X(CpS,pTmp[3]);     // F[2048+j]    F[3072-j]
    T6 = MULT_BP2X(CpS,pTmp[2]) - MULT_BP2X(CmS,pTmp[3]);     // F[2048+j]    F[3072-j]

    TA = pTmp[6] + pTmp[7];                                   // F[j]      +  F[1024-j];
    TB = pTmp[6] - pTmp[7];                                   // F[j]      -  F[1024-j];

    D  = DIV2(+ T7  + T8 + T9 + TA);
    E  = DIV2(+ T4  + T6 + T2 + TB);
    pWF[k]           = InverseQuadRootOfSumSquares(D, E);              // F[j]

    D  = DIV2(- T7  + T6 - T2 + TA);
    E  = DIV2(+ T4  + T8 - T9 - TB);
    pWF[iSizeBy2-k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024-j]

    D  = DIV2(- T7  - T6 + T2 + TA);
    E  = DIV2(- T4  + T8 - T9 + TB);
    pWF[iSizeBy2+k]  = InverseQuadRootOfSumSquares(D, E);              // F[1024+j]

    D  = DIV2(- T4  + T6 + T2 - TB);
    E  = DIV2(+ T7  - T8 - T9 + TA);
    pWF[(iSizeBy2<<1)-k]  = InverseQuadRootOfSumSquares(D, E);              // F[2048-j]

    //INTEGER_ONLY( assert( F[j]>=0 && F[(1024>>iShrink)-j]>=0 && F[(1024>>iShrink)+j]>=0 && F[(2048>>iShrink)-j]>=0 ) );

    //** This block uses 36 adds and 20 mults plus either 4 more mults or 4 shifts
}
#endif
   

#if defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM) && defined(PLATFORM_LPC_NO_TRIG_TABLE) 
    extern void prvLpcJLoopBottom( int j, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, 
                            BP2Type *pS1, BP2Type *pC1, BP2Type *pS1p, BP2Type *pC1p, const BP2Type STEP1 ); 
#else
    // use this to generate optimized assembly to start your own asm version
    void prvLpcJLoopBottom( int j, const LpSpecType* pTmp, WeightType* pWF, const Int iSizeBy2, 
                            BP2Type *pS1, BP2Type *pC1, BP2Type *pS1p, BP2Type *pC1p, const BP2Type STEP1 ) 
    {
        BP2Type S1 = *pS1;
        BP2Type C1 = *pC1;
        BP2Type S2, C2, SLS1, SLC1, CLS1, CLC1;
        int iSizeBy8 = iSizeBy2>>2;
        const BP2Type SL8     = BP2_FROM_FLOAT(0.38268343236508977172845998403040);     // sim(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type CL8     = BP2_FROM_FLOAT(0.92387953251128675612818318939679);     // cos(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type SL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // sim(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
        const BP2Type CL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // cos(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
        DEBUG_ONLY( int iSize = iSizeBy2<<1; )
        DEBUG_ONLY( int iSizeBy4 = iSizeBy2>>1; )
        DEBUG_ONLY( int iShrink = LOG2(MAX_LP_SPEC_SIZE/iSize); )

        assert( fabs(FLOAT_FROM_BP2(S1)-sin(PI*j/iSize)) < 0.0001 
             && fabs(FLOAT_FROM_BP2(C1)-cos(PI*j/iSize)) < 0.0001 );

        prvDoLpc4( j, pTmp, pWF, iSizeBy2, S1, C1 );

        S2 = (SLC1 = MULT_BP2X(SL8,C1)) - (CLS1 = MULT_BP2X(CL8,S1));
        C2 = (CLC1 = MULT_BP2X(CL8,C1)) + (SLS1 = MULT_BP2X(SL8,S1));
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8-j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8-j,1)))<0.0001 );
        prvDoLpc4( iSizeBy8 - j, pTmp+8,  pWF, iSizeBy2, S2, C2 );

        S2 = SLC1 + CLS1;
        C2 = CLC1 - SLS1;
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8+j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8+j,1)))<0.0001 );
        prvDoLpc4( iSizeBy8 + j, pTmp+16, pWF, iSizeBy2, S2, C2 );

        S2 = MULT_BP2X(SL4,C1) - MULT_BP2X(CL4,S1);
        C2 = MULT_BP2X(CL4,C1) + MULT_BP2X(SL4,S1);
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy4-j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy4-j,1)))<0.0001 );
        prvDoLpc4( (iSizeBy2>>1) - j, pTmp+24, pWF, iSizeBy2, S2, C2 );

		// rotate angle by b = pi/iSize
		// recursion: cos(a+b) = cos(a-b) + 2*sin(b)*sin(a)
		// and:       sin(a+b) = sin(a-b) - 2*sin(b)*cos(a)
		*pC1 = *pC1p - MULT_BP2X(STEP1,S1);
		*pS1 = *pS1p + MULT_BP2X(STEP1,C1);
		*pC1p = C1;
		*pS1p = S1;
    }
#   endif // defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM)



WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    // put often used variables near the top for easier access in platforms like the SH3 and SH4
    LpSpecType original[LPCORDER];
    LpSpecType *pFb;      
    Int i, j;
    LpSpecType t1pO7, t1mO7, tO1pO9, tO1mO9, tO0pO8, tO0mO8, tO4pO6, tO4mO6;
    LpSpecType t1pO7pO3, t1pO7mO3, tO1pO5pO9, tO0pO2pO4pO6pO8;
    LpSpecType tS2x; 
    LpSpecType tCpS1x, tCmS1x, tCpS2x, tCmS2x;
    LpSpecType CpS1, CmS1, CpS2, CmS2;
    LpSpecType D, E, F, G, H, I, J;
    
    LpSpecType* rgwtLpcSpec;
    WeightType wtLpcSpecMax, wtTemp;
    Int iFreq, iF, iLoopLimit;
    Int iShrink, iStride; 
    LpSpecType Tmp[32];
    WeightType* pWF = (WeightType*)INTEGER_OR_INT_FLOAT( (I32 *)ppcinfo->m_rguiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    Int iSize, iSizeBy2, iSizeBy4, iSizeBy8, iSizeBy16;

#   if defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
        BP2Type SLC1, CLS1, CLC1, SLS1;
        BP2Type S4, C4;
        BP2Type S1, C1;
        const BP2Type *pC4, *pC1;
        Int pCInc4, pCInc1;

        const BP2Type SL16    = BP2_FROM_FLOAT(0.19509032201612826784828486847702);     // sim(pi/16)  = sin(pi*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type CL16    = BP2_FROM_FLOAT(0.98078528040323044912618223613424);     // cos(pi/16)  = sin(pi*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type SL8     = BP2_FROM_FLOAT(0.38268343236508977172845998403040);     // sim(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type CL8     = BP2_FROM_FLOAT(0.92387953251128675612818318939679);     // cos(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type SL3by16 = BP2_FROM_FLOAT(0.55557023301960222474283081394853);     // sim(pi3/16) = sin(pi*3*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type CL3by16 = BP2_FROM_FLOAT(0.83146961230254523707878837761791);     // cos(pi3/16) = sin(pi*3*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type SL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // sim(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
        const BP2Type CL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // cos(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
#   elif defined(PLATFORM_LPC_NO_TRIG_TABLE)
#       if !defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM)
            BP2Type SLC1, CLS1, CLC1, SLS1;
#       endif
        BP2Type CT, ST;
        const SinCosTable* pSinCosTable;
        BP2Type S4, C4, S4p, C4p, STEP4;
        BP2Type S1, C1, S1p, C1p, STEP1;

        const BP2Type SL16    = BP2_FROM_FLOAT(0.19509032201612826784828486847702);     // sim(pi/16)  = sin(pi*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type CL16    = BP2_FROM_FLOAT(0.98078528040323044912618223613424);     // cos(pi/16)  = sin(pi*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type SL8     = BP2_FROM_FLOAT(0.38268343236508977172845998403040);     // sim(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type CL8     = BP2_FROM_FLOAT(0.92387953251128675612818318939679);     // cos(pi/8)   = sin(pi*iSizeBy8*(1<<iShrink)/2048)
        const BP2Type SL3by16 = BP2_FROM_FLOAT(0.55557023301960222474283081394853);     // sim(pi3/16) = sin(pi*3*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type CL3by16 = BP2_FROM_FLOAT(0.83146961230254523707878837761791);     // cos(pi3/16) = sin(pi*3*iSizeBy16*(1<<iShrink)/2048)
        const BP2Type SL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // sim(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
        const BP2Type CL4     = BP2_FROM_FLOAT(0.70710678118654752440084436210485);     // cos(pi/4)   = sin(pi*iSizeBy4*(1<<iShrink)/2048)
#   else
        Int k, jj;
        Int iMapLpcIdxOffset[4];
        LpSpecType* pTmp;
        const BP2Type* pC4;
        Int pCInc4;
#   endif
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);
    
#ifdef REPLICATE_V4_LPC
    iSize = pau->m_cFrameSampleHalf;
#elif defined(REPLICATE_ARM_NON_TRUNCATED_LPC)
    iSize = pau->m_cSubband;
#else
    if (pau->m_fV5Lpc)
        iSize = pau->m_cSubband;
    else 
        iSize = pau->m_cFrameSampleHalf;
#endif
    iShrink = LOG2(MAX_LP_SPEC_SIZE/iSize);     // for smaller transforms, shrink or expand indexing
    iStride = MAX_LP_SPEC_SIZE/iSize;           // for smaller transforms, stride past unused (lpc_compare only)
    iSizeBy2 = iSize>>1;
    iSizeBy4 = iSizeBy2>>1;
    iSizeBy8 = iSizeBy4>>1;
    iSizeBy16 = iSizeBy8>>1;
    // iSize can be 2048, 1024, 512, 256, or 128.
    //      at 32000 Hz: 2048, 1024, 512 and 256
    //      at 22050 Hz: 1024, 512, 256 and 128
    //      at 16000 Hz: 512, 256, and 128
    //      at 11025 Hz: 512, 256, and 128
    //      at  8000 Hz: 512.

    
        
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    
    //for (i = 0; i < LPCORDER; i++) 
    //fprintf(stdout, "%.20lf\n", (double) FLOAT_FROM_LP(rgLpcCoef[i])); 
    
    for (i = 0; i < LPCORDER; i++) 
        original[i] = -LP_SPEC_FROM_LP(rgLpcCoef[i]); 
      
                                        // F[128]
    Tmp[14]  = (t1pO7 = LP_SPEC_FROM_FLOAT(1) + original[7]) + (tS2x = MULT_BP2X(SQRT2,original[3]));
                                        // F[256]  
    Tmp[22]  = (t1mO7 = LP_SPEC_FROM_FLOAT(1) - original[7]) + original[3];
    Tmp[30]  = t1mO7;                   // F[384]
    Tmp[31]  = t1pO7 - tS2x;            // F[640]
    Tmp[23]  = t1mO7 - original[3];     // F[768]
    Tmp[15]  = t1mO7;                   // F[896]
    
                                        // F[1152] 
    Tmp[8]  = (tO1pO9 = original[1] + original[9]) + (tS2x = MULT_BP2X(SQRT2,original[5]));
                                        // F[1280] 
    Tmp[16]  = (tO1mO9 = original[1] - original[9]) + original[5];
    Tmp[24]  = tO1mO9;                  // F[1408]
    Tmp[25]  = tO1pO9 - tS2x;           // F[1664]
    Tmp[17]  = tO1mO9 - original[5];    // F[1792]
    Tmp[9]  = tO1mO9;                   // F[1920] 
    
                                        // F[2176] 
    Tmp[10]  = (tO0pO8 = original[0] + original[8]) + (tS2x = MULT_BP2X(SQRT2,original[4]));
                                        // F[2304] 
    Tmp[18]  = (tO0mO8 = original[0] - original[8]) + original[4];
    Tmp[26]  = tO0mO8;                  // F[2432]
    Tmp[27]  = tO0pO8 - tS2x;           // F[2688]
    Tmp[19]  = tO0mO8 - original[4];    // F[2816] 
    Tmp[11]  = tO0mO8;                  // F[2944]
    
                                        // F[3200] 
    Tmp[12]  = original[2] + (tS2x = MULT_BP2X(SQRT2,original[6]));
    Tmp[20]  = original[2] + original[6]; // F[3328]
    Tmp[28]  = original[2];             // F[3456] 
    Tmp[29]  = original[2] - tS2x;      // F[3712]
    Tmp[21]  = original[2] - original[6]; // F[3840]
    Tmp[13]  = original[2];             // F[3968]
    
    tO4pO6 = original[4] + original[6];
    tO4mO6 = original[4] - original[6];
    t1pO7pO3 = t1pO7 + original[3];
    t1pO7mO3 = t1pO7 - original[3];
    tO1pO5pO9 = tO1pO9 + original[5];
    tO0pO2pO4pO6pO8 = tO0pO8 + tO4pO6 + original[2];

    D  = t1pO7pO3 + tO1pO5pO9 + tO0pO2pO4pO6pO8;    // F[0]
    E  = t1pO7mO3 + (tS2x = MULT_BP2X(SQRT2_2, tO0pO8 - tO4mO6 - original[2]));     // F[512]
    F  = t1pO7pO3 - tO1pO5pO9;                      // F[1024]
    G  = t1pO7mO3 - tS2x;                           // F[1536]
    H  = -tO1pO9 + original[5] + (tS2x = MULT_BP2X(SQRT2_2,tO0pO8 - tO4pO6 + original[2]));  // F[2560]
    I  =  tO0pO8 + tO4mO6 - original[2];            // F[3072]
    J  =  tO1pO9 - original[5] + tS2x;              // F[3584]
    
    // j==0 and i==0 below.
    pFb   =  (LpSpecType*) &pWF[0];      
    *pFb  =  InverseQuadRootOfSumSquares(D, 0);     // F[0]
    pFb  +=  iSizeBy4;   
    *pFb  =  InverseQuadRootOfSumSquares(E, J);     // F[512]    b: F[512]    a: F[3584]
    pFb  +=  iSizeBy4;    
    *pFb  =  InverseQuadRootOfSumSquares(F, I);     // F[1024]    b: F[1024]   a: F[3072]
    pFb  +=  iSizeBy4;    
    *pFb  =  InverseQuadRootOfSumSquares(G, H);     // F[1536]   b: F[1536]   a: F[2560]

    // k - example:  iSize = 0x100 = 256, j==0
    //         i:  0   1   2   3 
    //    kdx         10  20  30 
    // 80-kdx         70  60  50  
    // 80+kdx         90  a0  b0
    //100-kdx         f0  e0  d0  

#   if defined(PLATFORM_LPC_NO_TRIG_TABLE) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)

        assert( fabs(FLOAT_FROM_BP2(SL16)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy16,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(CL16)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy16,1)))<0.0001 );

        prvDoLpc4( iSizeBy16, Tmp+8, pWF, iSizeBy2, SL16, CL16 );

        assert( fabs(FLOAT_FROM_BP2(SL8)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(CL8)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8,1)))<0.0001 );

        prvDoLpc4( iSizeBy8,  Tmp+16, pWF, iSizeBy2, SL8,  CL8 );

        assert( fabs(FLOAT_FROM_BP2(SL3by16)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8 + iSizeBy16,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(CL3by16)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8 + iSizeBy16,1)))<0.0001 );

        prvDoLpc4( iSizeBy8 + iSizeBy16, Tmp+24, pWF, iSizeBy2, SL3by16, CL3by16 );

#   else

        iMapLpcIdxOffset[0] = 0;  
        iMapLpcIdxOffset[1] = iSizeBy8;  
        iMapLpcIdxOffset[2] = iSizeBy8;  
        iMapLpcIdxOffset[3] = iSizeBy4;

        pTmp = Tmp+8;
        for( i = 1, jj = iSizeBy16; i<4; i++, pTmp += 8, jj = (i&1)*(iSizeBy16) ) 
        {
            k = iMapLpcIdxOffset[i] - jj;

            prvDoLpc4( k, pTmp, pWF, iSizeBy2, iShrink );
        }

#   endif

#   if defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
        // 2 items per row, sample every fourth row.
        pC4 = &lpc_trig2048[(8<<iShrink)];
        pCInc4 = (8<<iShrink)-1;  // 1 of them gets done with ++
        // pC4 = lpc_Trigs[iShrink] + (2*4);
        // pCInc4 = 8-1;

        pC1 = &lpc_trig2048[(2<<iShrink)];
        pCInc1 = (2<<iShrink)-1;    // 1 of them gets done with ++;
        // pC1 = lpc_Trigs[iShrink] + 2;
        // pCInc1 = 1;

#   elif defined(PLATFORM_LPC_NO_TRIG_TABLE)

        assert( 64 <= iSize && iSize <= 2048 );

        pSinCosTable = rgSinCosTables[iSize>>7];
        S4  = pSinCosTable->sin_4PIbycSB;
        C4  = pSinCosTable->cos_4PIbycSB;
        S4p = 0;
        C4p = BP2_FROM_FLOAT(1.0f);
        STEP4 = MUL2(S4);

        S1  = BP2_FROM_BP1(pSinCosTable->sin_PIbycSB);
        C1  = BP2_FROM_BP1(pSinCosTable->cos_PIbycSB);
        S1p = 0;
        C1p = BP2_FROM_FLOAT(1.0f);
        STEP1 = BP2_FROM_BP1(pSinCosTable->two_sin_PIbycSB);

#   else   // so must be big lpc constants table

        pC4 = &lpc_cnst3[1*(4<<iShrink)];
        pCInc4 = (4<<iShrink)-3;  // 3 of them get done with ++

#   endif  // defined(PLATFORM_LPC_NO_TRIG_TABLE)

    for (j=1; j<iSizeBy16; j++ )
    {
#       if defined(PLATFORM_LPC_NO_TRIG_TABLE) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
#           if !defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
                BP2Type S2, C2;
#           endif
            BP2Type C8, S8;

#           if defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
                S4 = *pC4++;                        // sin(x) = sin(pi*4*j/iSize)
                C4 = *pC4;                          // cos(x) = cos(pi*4*j/iSize)
                pC4 += pCInc4;
#           endif  // PLATFORM_LPC_SMALL_TRIG_TABLE

            assert( fabs(FLOAT_FROM_BP2(S4)-sin(PI*4*j/iSize)) < 0.0001 
                 && fabs(FLOAT_FROM_BP2(C4)-cos(PI*4*j/iSize)) < 0.0001 );

            CmS1 = -C4 + S4;                    // cnst3[i*(4<<iShrink)+3];
            CpS1 =  C4 + S4;                    // cnst3[i*(4<<iShinrk)+1];

            S8 = MUL2(MULT_BP2X(C4,S4));                        // sin(2x)
            C8 = BP2_FROM_FLOAT(1.0f) - MUL2(MULT_BP2X(S4,S4)); // cos(2x)
            CmS2 = -C8 + S8;                    // cnst3[i*(4<<iShrink)+2];
            CpS2 =  C8 + S8;                    // cnst3[i*(4<<iShink)];

#           if defined(PLATFORM_LPC_NO_TRIG_TABLE)
		        // rotate angle by b = 4*pi/iSize
		        // recursion: cos(a+b) = cos(a-b) + 2*sin(b)*sin(a)
		        // and:       sin(a+b) = sin(a-b) - 2*sin(b)*cos(a)
		        CT = C4p - MULT_BP2X(STEP4,S4);
		        ST = S4p + MULT_BP2X(STEP4,C4);
		        C4p = C4;  C4 = CT;
		        S4p = S4;  S4 = ST;
#           endif  // PLATFORM_LPC_NO_TRIG_TABLE

#       else    // PLATFORM_LPC_NO_TRIG_TABLE || PLATFORM_LPC_SMALL_TRIG_TABLE

            CpS2 = *pC4++;              // cnst3[i*(4<<iShink)];
            CpS1 = *pC4++;              // cnst3[i*(4<<iShinrk)+1];
            CmS2 = *pC4++;              // cnst3[i*(4<<iShrink)+2];
            CmS1 = *pC4;                // cnst3[i*(4<<iShrink)+3];
            pC4 += pCInc4;

#       endif
        assert( BP2_FROM_FLOAT(1) <= CpS2 && CpS2 <= BP2_FROM_FLOAT(1.5) );     

                                                        // F[j]
        Tmp[6]  = LP_SPEC_FROM_FLOAT(1) + (tCpS2x = MULT_BP2X(CpS2,original[7])) + (tCpS1x = MULT_BP2X(CpS1,original[3]));
                                                        // F[256-j]
        Tmp[14] = LP_SPEC_FROM_FLOAT(1) + (tCmS2x = MULT_BP2X(CmS2,original[7])) + tCpS1x;
                                                        // F[256+j]
        Tmp[22] = LP_SPEC_FROM_FLOAT(1) - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[3]));    
        Tmp[30] = LP_SPEC_FROM_FLOAT(1) - tCmS2x + tCmS1x;  // F[512-j]
        Tmp[31] = LP_SPEC_FROM_FLOAT(1) + tCpS2x - tCpS1x;  // F[512+j]
        Tmp[23] = LP_SPEC_FROM_FLOAT(1) + tCmS2x - tCpS1x;  // F[768-j]
        Tmp[15] = LP_SPEC_FROM_FLOAT(1) - tCpS2x + tCmS1x;  // F[768+j]
        Tmp[7]  = LP_SPEC_FROM_FLOAT(1) - tCmS2x - tCmS1x;  // F[1024-j]
        
                                                        // F[1024+j]
        Tmp[0]  = original[1] + (tCpS2x = MULT_BP2X(CpS2,original[9])) + (tCpS1x = MULT_BP2X(CpS1,original[5]));
                                                        // F[1280-j]  
        Tmp[8]  = original[1] + (tCmS2x = MULT_BP2X(CmS2,original[9])) + tCpS1x;
                                                        // F[1280+j]  
        Tmp[16] = original[1] - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[5]));
        Tmp[24] = original[1] - tCmS2x + tCmS1x;            // F[1536-j]
        Tmp[25] = original[1] + tCpS2x - tCpS1x;            // F[1536+j]
        Tmp[17] = original[1] + tCmS2x - tCpS1x;            // F[1792-j]
        Tmp[9]  = original[1] - tCpS2x + tCmS1x;            // F[1792+j]
        Tmp[1]  = original[1] - tCmS2x - tCmS1x;            // F[2048-j
        
                                                        // F[2048+j]  
        Tmp[2]  = original[0] + (tCpS2x = MULT_BP2X(CpS2,original[8])) + (tCpS1x = MULT_BP2X(CpS1,original[4]));
                                                        // F[2304-j]  
        Tmp[10] = original[0] + (tCmS2x = MULT_BP2X(CmS2,original[8])) + tCpS1x;
                                                        // F[2304+j]  
        Tmp[18] = original[0] - tCpS2x - (tCmS1x = MULT_BP2X(CmS1,original[4]));
        Tmp[26] = original[0] - tCmS2x + tCmS1x;            // F[2560-j]
        Tmp[27] = original[0] + tCpS2x - tCpS1x;            // F[2560+j]
        Tmp[19] = original[0] + tCmS2x - tCpS1x;            // F[2816-j]
        Tmp[11] = original[0] - tCpS2x + tCmS1x;            // F[2816+j]
        Tmp[3]  = original[0] - tCmS2x - tCmS1x;            // F[3072-j]
        
                                                        // F[3072+j] 
                                                        // F[3328-j]
        Tmp[4]  =  Tmp[12]  = original[2] + (tCpS1x = MULT_BP2X(CpS1,original[6]));
                                                        // F[3328+j] 
        Tmp[20] = original[2] - (tCmS1x = MULT_BP2X(CmS1,original[6]));
        Tmp[28] = original[2] + tCmS1x;                   // F[3584-j]
        Tmp[29] = original[2] - tCpS1x;                   // F[3584+j]
        Tmp[21] = original[2] - tCpS1x;                   // F[3840-j]
        Tmp[13] = original[2] + tCmS1x;                   // F[3840+j]
        Tmp[5]  = original[2] - tCmS1x;                   // F[4096-j]
        
        //** this block uses 52 adds and 14 mults??
       

        // example:  iSize = 0x100 = 256.
        //            j==1         |   j==2          |  j==3
        //       i:  0   1   2   3 |   0   1   2   3 |  0
        //    k      1  1f  21  3f |   2  1e  22  3e |  3 ...
        // 80+k     81  9f  a1  bf |  82  9e  a2  be | 83 ...
        // 80-k     7f  61  5f  41 |  7e  62  5e  42 | 7e ...
        //100-k     ff  e1  df  c1 |  fe  e2  de  c2 | fd ...


#if defined(PLATFORM_LPC_NO_TRIG_TABLE) || defined(PLATFORM_LPC_SMALL_TRIG_TABLE)

#   if defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM) && defined(PLATFORM_LPC_NO_TRIG_TABLE)
        prvLpcJLoopBottom( j, Tmp, pWF, iSizeBy2, &S1, &C1, &S1p, &C1p, STEP1 );
#   else    //  defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM)
#       if defined(PLATFORM_LPC_SMALL_TRIG_TABLE)
            S1 = *pC1++;                        // sin(x) = sin(pi*4*j/iSize)
            C1 = *pC1;                          // cos(x) = cos(pi*4*j/iSize)
            pC1 += pCInc1;
#       endif  // PLATFORM_LPC_SMALL_TRIG_TABLE

        assert( fabs(FLOAT_FROM_BP2(S1)-sin(PI*j/iSize)) < 0.0001 
             && fabs(FLOAT_FROM_BP2(C1)-cos(PI*j/iSize)) < 0.0001 );

        prvDoLpc4( j, Tmp, pWF, iSizeBy2, S1, C1 );

        S2 = (SLC1 = MULT_BP2X(SL8,C1)) - (CLS1 = MULT_BP2X(CL8,S1));
        C2 = (CLC1 = MULT_BP2X(CL8,C1)) + (SLS1 = MULT_BP2X(SL8,S1));
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8-j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8-j,1)))<0.0001 );
        prvDoLpc4( iSizeBy8 - j, Tmp+8,  pWF, iSizeBy2, S2, C2 );

        S2 = SLC1 + CLS1;
        C2 = CLC1 - SLS1;
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8+j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy8+j,1)))<0.0001 );
        prvDoLpc4( iSizeBy8 + j, Tmp+16, pWF, iSizeBy2, S2, C2 );

        S2 = MULT_BP2X(SL4,C1) - MULT_BP2X(CL4,S1);
        C2 = MULT_BP2X(CL4,C1) + MULT_BP2X(SL4,S1);
        assert( fabs(FLOAT_FROM_BP2(S2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy4-j,0)))<0.0001 
             && fabs(FLOAT_FROM_BP2(C2)-FLOAT_FROM_BP2(TRIGTBL(iSizeBy4-j,1)))<0.0001 );
        prvDoLpc4( iSizeBy4 - j, Tmp+24, pWF, iSizeBy2, S2, C2 );

#       if defined(PLATFORM_LPC_NO_TRIG_TABLE)
		    // rotate angle by b = pi/iSize
		    // recursion: cos(a+b) = cos(a-b) + 2*sin(b)*sin(a)
		    // and:       sin(a+b) = sin(a-b) - 2*sin(b)*cos(a)
		    CT = C1p - MULT_BP2X(STEP1,S1);
		    ST = S1p + MULT_BP2X(STEP1,C1);
		    C1p = C1;  C1 = CT;
		    S1p = S1;  S1 = ST;
#       endif  // defined(PLATFORM_LPC_NO_TRIG_TABLE)
#   endif // defined(PLATFORM_SPECIFIC_LPC_JLOOP_BOTTOM)
#else

        pTmp = Tmp;
        for( i = 0, jj = j; i<4; i++, pTmp += 8, jj = -jj ) 
        {
            k = iMapLpcIdxOffset[i] + jj;

            prvDoLpc4( k, pTmp, pWF, iSizeBy2, iShrink );    
        }
    
#endif

    }
    //** this loop uses (512>>iShrink) * (36 adds and 24 mults)
    
    //** total to here: 
    //**   (46 adds and 14 mults) + (N/16) * (52 adds and 14 mults) + (N/4) * (36 adds and 24 mults)
    //** TA = 46 + 3.25N + 9N      = 46 + 12.250N
    //** TM = 14 + (14/16)N + 6*N  = 14 +  6.875N
    //** An FFT takes order N*log(N)
    //** so we may need to substitute an optimized FFT for this on some platforms.
    
    // The inverse quad root has already been computed. Copy to destination, find max etc remain here.
    
    rgwtLpcSpec  = INTEGER_OR_INT_FLOAT( (I32 *)ppcinfo->m_rguiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    iLoopLimit   = pau->m_cSubband;
    
    for (iFreq = 0, iF = 0; iFreq < iLoopLimit; iFreq++, iF += iStride) 
    {
        LPC_COMPARE_DEBUG( pau, iFreq, iFreq, iF, pWF, rgLpcCoef );

        wtTemp = pWF[iFreq];

        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
        
        INTEGER_ONLY( assert( wtTemp>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,wtTemp);                                
        MONITOR_RANGE(gMR_fltLPC_F3,pWF[iFreq]);
    }
    
	WF_PRINT(pWF);

    if (wtLpcSpecMax == WEIGHT_FROM_FLOAT(0.0F))
    {
        FUNCTION_PROFILE_STOP(&fp);
        return TraceResult(WMA_E_FAIL);
    }
    
#if defined(INTEGER_ENCODER)
    ppcinfo->m_iMaxWeight = wtLpcSpecMax;
#endif
    ppcinfo->m_wtMaxWeight = wtLpcSpecMax;
    
    FUNCTION_PROFILE_STOP(&fp);
    return WMA_OK;
}

#else  // defined(PLATFORM_LPC_FOLDED_PRUNED) && !defined(V4V5_COMPARE_MODE)
# include "lpc_variants.c"
#endif // defined(PLATFORM_LPC_FOLDED_PRUNED) && !defined(V4V5_COMPARE_MODE)

#endif // (V4V5_COMPARE_MODE && BUILD_INT_FLOAT) and then the opposite

#ifdef REPLICATE_V4_LPC
#   pragma COMPILER_MESSAGE(__FILE__ "(1139) : Warning - Built with REPLICATE_V4_LPC!")
#endif


#if defined(_DEBUG) && defined(LPC_COMPARE)

void prvSetupLpcSpecTrig(CAudioObject* pau) 
{
    // precompute trig functions used in the slow spectral method used for comparision
    Int iFreq, i; 
    
    for (i = 0; i < LPCORDER; i++)  {
        if ( pau->m_rgfltLpsSpecCos [i] == NULL )
            pau->m_rgfltLpsSpecCos [i] = (Float*) wmvalloc (sizeof (Float) * MAX_LP_SPEC_SIZE);
        if ( pau->m_rgfltLpsSpecSin [i] == NULL );
            pau->m_rgfltLpsSpecSin [i] = (Float*) wmvalloc (sizeof (Float) * MAX_LP_SPEC_SIZE);
        assert( pau->m_rgfltLpsSpecCos [i] != NULL && pau->m_rgfltLpsSpecSin [i] != NULL );
    }
    if ( pau->m_rgfltLpsSpecCos [LPCORDER-1] != NULL && pau->m_rgfltLpsSpecSin [LPCORDER-1] != NULL )
    {
        for (iFreq = 0; iFreq < MAX_LP_SPEC_SIZE; iFreq++)   
        {
#           if defined(REPLICATE_V4_LPC) || defined(V4V5_COMPARE_MODE)
                Float  Temp = (Float)((iFreq) * PI / MAX_LP_SPEC_SIZE);
#           else
                Double Temp = ((iFreq) * PI / MAX_LP_SPEC_SIZE);
#           endif
            for (i = 0; i < LPCORDER; i++)  
            {
                pau->m_rgfltLpsSpecCos [i] [iFreq] = (Float) cos ((i + 1) * Temp);
                pau->m_rgfltLpsSpecSin [i] [iFreq] = (Float) sin ((i + 1) * Temp);
            }
        }
    }
}

#endif  // _DEBUG


#endif //defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
