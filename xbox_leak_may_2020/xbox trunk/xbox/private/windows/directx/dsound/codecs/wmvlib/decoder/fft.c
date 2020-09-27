//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       fft.c
//
//  The FFT used for the DctIV and DctIV
//
//--------------------------------------------------------------------------

#include "bldsetup.h"

#include <math.h>
//#include <assert_wma.h>
#include <stdio.h>
#include <limits.h>
#include "msaudio.h"
#include "macros.h"
#include "AutoProfile.h"

// Define to get Split Radix FFt algorithm - which is expected to be 30% faster than Radix 2
//#define SPLIT_RADIX_FFT

#if defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER)


// Now separately build each processor
//#    include "../mips/transform_mips.c"

#elif ( defined(WMA_TARGET_SH3) || defined(WMA_TARGET_SH4) ) && defined(BUILD_INTEGER)

// Now separately build each processor
//#    include "../sh3/transform_sh3.c"

#elif defined(WMA_TARGET_SH4) && defined(BUILD_INT_FLOAT)

// Now separately build each processor
//#    include "../sh4/transform_sh4.c"

#else

// Some Algorithm Counters to match to journal article operation counts
#if defined(_DEBUG) && 0

    static int cButterfly0 = 0;         // R2 butterfly with no multiplies
    static int cButterfly1 = 0;         // R4 butterfly with no multiplies
    static int cButterfly2 = 0;         // R2 or R4 butterfly with a complex multiply
    static int cTrig  = 0;              // number of sin/cos recursions
    static int cAdditions = 0;          // number of Data additions (float or I32)
    static int cMultiplications = 0;    // number of Data mutliplications (float or I32)
    static int cIndexing = 0;           // count array indexing/incrementing
    static int cLoops = 0;              // count loops
    static int cLoopIndexing = 0;       // count extra loop indexing operations
    static int cTrigAdditions = 0;      // Additions for trig recursions
    static int cTrigMultiplications = 0; // multiplications for trig recursions

#   define DEBUG_COUNT_BTFLY0(a,m,i) cButterfly0++; cAdditions += a; cMultiplications += m; cIndexing += i;
#   define DEBUG_COUNT_BTFLY1(a,m,i) cButterfly1++; cAdditions += a; cMultiplications += m; cIndexing += i;
#   define DEBUG_COUNT_BTFLY2(a,m,i) cButterfly2++; cAdditions += a; cMultiplications += m; cIndexing += i;
#   define DEBUG_COUNT_TRIG(a,m)     cTrig++;  cTrigAdditions += a;  cTrigMultiplications += m;
#   define DEBUG_COUNT_LOOP(i)       cLoops++; cLoopIndexing += i;

#else // _DEBUG

#   define DEBUG_COUNT_BTFLY0(a,m,i)
#   define DEBUG_COUNT_BTFLY1(a,m,i)
#   define DEBUG_COUNT_BTFLY2(a,m,i)
#   define DEBUG_COUNT_TRIG(a,m)
#   define DEBUG_COUNT_LOOP(i)

#endif // _DEBUG

static const double dPI = PI;       // 3.1415926535897932384626433832795;

// the following defines do most of the computational work of the FFT, so they should be done efficently
// note need ur and ui to be defined unless assembly routines which should not need them
// note pointers will be incremented as a side effect of these macros
// define these in assembly for any processor whose compiler needs further optimization

#if !defined(SPLIT_RADIX_FFT)


#define FFTBUTTERFLY0(pxk,pxi) \
/* Butterfly at angle == 0 */ \
    ur = *pxk - *pxi;               /* ur = px[k] - px[i];      */ \
    *pxk++ += *pxi;                 /* px[k]   += px[i];        */ \
    *pxi++ = ur;                    /* px[i]   = ur;            */ \
    ui = *pxk - *pxi;               /* ui = px[k+1] - px[i+1];  */ \
    *pxk++ += *pxi;                 /* px[k+1] += px[i+1];      */ \
    *pxi++ = ui;                    /* px[i+1] = ui;            */
// leave pxi and pxk incremented by 2
// The SH-3 compiler generates very good code for fftbutterfly0

#define FFTBUTTERFLY(pxk,pxi,CR,SI)     \
/* Butterfly at a non-zero angle */     \
    ur = *pxk - *pxi;           /* ur = px[k] - px[i];    */ \
    *pxk++ += *pxi++;           /* px[k]   += px[i];      */ \
    ui = *pxk - *pxi;           /* ui = px[k+1] - px[i+1];*/ \
    *pxk++ += *pxi--;           /* px[k+1] += px[i+1];    */ \
    *pxi++ = MULT_BP2(CR,ur) - MULT_BP2(SI,ui);  /* px[i]   = cr * ur - (si) * ui; */ \
    *pxi++ = MULT_BP2(CR,ui) + MULT_BP2(SI,ur);  /* px[i+1] = cr * ui + (si) * ur; */ \
// leave pxi and pxk incremented by 2

// *************************************************************************************
// Spilt radix inverse FFT which scales by 2/N in a step-wise fashion
// *************************************************************************************

#if defined(BUILD_INTEGER)

#define FFTBUTTERFLY0_N(pxk,pxi) \
/* Butterfly at angle == 0 with inputs divided by 2*/ \
    ur = (tk = *pxk>>1) - (ti = *pxi>>1);   /* ur = px[k] - px[i];      */ \
    *pxk++ = tk + ti;                       /* px[k]   += px[i];        */ \
    *pxi++ = ur;                            /* px[i]   = ur;            */ \
    ui = (tk = *pxk>>1) - (ti = *pxi>>1);   /* ui = px[k+1] - px[i+1];  */ \
    *pxk++ = tk + ti;                       /* px[k+1] += px[i+1];      */ \
    *pxi++ = ui;                    /* px[i+1] = ui;            */
// leave pxi and pxk incremented by 2
// The SH-3 compiler generates very good code for fftbutterfly0

#define FFTBUTTERFLY_N(pxk,pxi,CR,SI)       \
/* Butterfly at a non-zero angle with inputs divided by 2*/     \
    ur = (tk = *pxk>>1) - (ti = *pxi++>>1); /* ur = px[k] - px[i];    */ \
    *pxk++ = tk + ti;                       /* px[k]   += px[i];      */ \
    ui = (tk = *pxk>>1) - (ti = *pxi-->>1); /* ui = px[k+1] - px[i+1];*/ \
    *pxk++ = tk + ti;                       /* px[k+1] += px[i+1];    */ \
    *pxi++ = MULT_BP2(CR,ur) - MULT_BP2(SI,ui);  /* px[i]   = cr * ur - (si) * ui; */ \
    *pxi++ = MULT_BP2(CR,ui) + MULT_BP2(SI,ur);  /* px[i+1] = cr * ui + (si) * ur; */ \
// leave pxi and pxk incremented by 2

#else // must be BUILD_INT_FLOAT

#define FFTBUTTERFLY0_N(pxk,pxi) FFTBUTTERFLY0(pxk,pxi)

#define FFTBUTTERFLY_N(pxk,pxi,CR,SI) FFTBUTTERFLY(pxk,pxi,CR,SI)

#endif

#ifndef _M_IX86
// the following tables save floating point conversions and trig function calls
// compiler is unwilling to evaluate a constant expression of the form cos(PI/4) as a constant
static const BP1Type icosPIbynp[16] = { 
        BP1_FROM_FLOAT(-0.999999999999996),   // cos(dPI/1)
        BP1_FROM_FLOAT(-0.000000043711390),   // cos(dPI/2)
        BP1_FROM_FLOAT(0.707106765732237),    // cos(dPI/4)
        BP1_FROM_FLOAT(0.923879528329380),    // cos(dPI/8)
        BP1_FROM_FLOAT(0.980785279337272),    // cos(dPI/16)
        BP1_FROM_FLOAT(0.995184726404418),    // cos(dPI/32)
        BP1_FROM_FLOAT(0.998795456138147),    // cos(dPI/64)
        BP1_FROM_FLOAT(0.999698818679443),    // cos(dPI/128)
        BP1_FROM_FLOAT(0.999924701834954),    // cos(dPI/256)
        BP1_FROM_FLOAT(0.999981175281554),    // cos(dPI/512)
        BP1_FROM_FLOAT(0.999995293809314),    // cos(dPI/1024)
        BP1_FROM_FLOAT(0.999998823451636),    // cos(dPI/2048)
        BP1_FROM_FLOAT(0.999999705862866),    // cos(dPI/4096)
        BP1_FROM_FLOAT(0.999999926465714),    // cos(dPI/8192)
        BP1_FROM_FLOAT(0.999999981616428),    // cos(dPI/16384)
        BP1_FROM_FLOAT(0.999999995404107) };  // cos(dPI/32768)
static const BP1Type isinPIbynp[16] = { 
        BP1_FROM_FLOAT(0.000000087422780),    // sin(-dPI/1)
        BP1_FROM_FLOAT(-0.999999999999999),   // sin(-dPI/2)
        BP1_FROM_FLOAT(-0.707106796640858),   // sin(-dPI/4)
        BP1_FROM_FLOAT(-0.382683442461104),   // sin(-dPI/8)
        BP1_FROM_FLOAT(-0.195090327375064),   // sin(-dPI/16)
        BP1_FROM_FLOAT(-0.098017143048367),   // sin(-dPI/32)
        BP1_FROM_FLOAT(-0.049067675691754),   // sin(-dPI/64)
        BP1_FROM_FLOAT(-0.024541229205697),   // sin(-dPI/128)
        BP1_FROM_FLOAT(-0.012271538627189),   // sin(-dPI/256)
        BP1_FROM_FLOAT(-0.006135884819899),   // sin(-dPI/512)
        BP1_FROM_FLOAT(-0.003067956848339),   // sin(-dPI/1024)
        BP1_FROM_FLOAT(-0.001533980228972),   // sin(-dPI/2048)
        BP1_FROM_FLOAT(-0.000766990340086),   // sin(-dPI/4096)
        BP1_FROM_FLOAT(-0.000383495198243),   // sin(-dPI/8192)
        BP1_FROM_FLOAT(-0.000191747602647),   // sin(-dPI/16384)
        BP1_FROM_FLOAT(-0.000095873801764) }; // sin(-dPI/32768)
#endif // _M_IX86


// *************************************************************************************
//  Radix 2 FFT
//  when BUILD_INTEGER, does a scale by 2/N FFT
//  when BUILD_INT_FLOAT, does a regular FFT
// *************************************************************************************

        
void prvFFT4DCT(CoefType data[], Int nLog2np, FftDirection fftDirection)
{
    I32 np = (1<<nLog2np);
    CoefType *px = data;
    I32 i, j, k, l, m, n;
    CoefType *pxk, *pxi;
    CoefType ur, ui;
#if defined(BUILD_INTEGER)
    CoefType tk, ti;
#endif

    // defining the following COSSIN structure allows us to pass a pointer to a set of values in asm code
    // but WinCE/SH-3 compiler seems to do a particularly bad job of optimizing access to structure members!
    // struct COSSIN { I32 CR2,SI2,CR1,SI1,STEP,CR,SI;  } cs;
    BP2Type CR2,SI2,CR1,SI1,STEP,CR,SI;
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,FFT_PROFILE);

#ifdef _M_IX86
    // Pentium sin/cos are fast
    CR = BP2_FROM_FLOAT(cos(dPI/np));
    STEP = BP2_FROM_FLOAT(2*sin(-dPI/np));
#else // _M_IX86
    // other platforms' sin/cos unlikely to be that optimized, so do lookup
    {
        if ( nLog2np < 16 )
        {
            CR = BP2_FROM_BP1(icosPIbynp[nLog2np]);         // CR = (I32)(cos(PI/np) * NF2BP2)
#ifdef BUILD_INTEGER
            STEP = isinPIbynp[nLog2np];                     // STEP = (I32)(2*sin(-PI/np) * NF2BP2)
#else
            STEP = BP2_FROM_FLOAT(2*isinPIbynp[nLog2np]);   // STEP = (I32)(2*sin(-PI/np) * NF2BP2)
#endif
        }
        else
        {
            CR = BP2_FROM_FLOAT(cos(dPI/np));
            STEP = BP2_FROM_FLOAT(2*sin(-dPI/np));
        }
    }
#endif // _M_IX86
    
    if (fftDirection == FFT_INVERSE) STEP *= -1;
    
    SI = DIV2(STEP);                                    // SI = (I32)(sin(-PI/np) * NF2BP2)

    ///  comments with three /// show example indexing sequences for np=1024 and times the loop is executed [1x]

    n = 2 * np;                             /// n: 2048
    m = n;

    while (m > 4) {                         /// m: {2048, ... 16, 8} [9x]
        l = m >> 1;
        CR2 = BP2_FROM_FLOAT(1);                                // cos(0)
        SI2 = 0;                                                // sin(0)
        CR1 = BP2_FROM_FLOAT(1) - MULT_BP2(STEP,SI);    // cos(2*2*PI/m)
        SI = SI1 = MULT_BP2(STEP,CR);                   // sin(2*2*PI/m)
        CR = CR1;
        STEP = MUL2(SI1);

        // Trivial butterflies (angle = 0) 
        for (k = 0; k < n; k += m) {        /// k: {0} {0,1024} ... {0,8,16...2044} [512x]
            i = k + l;                      /// i: {1024} {1024,2048} ... {4,12,20...2040}
            pxk = &px[k];
            pxi = &px[i];
            // +1/-1 butterfly 
            FFTBUTTERFLY0_N(pxk,pxi);                   // update px[i], px[i+1] and px[k], px[k+1], leave pointers +2
             // +1/-1/W_k butterfly 
            FFTBUTTERFLY_N(pxk,pxi,CR1,SI1);    // update px[i+2], px[i+2+1] and px[k+2], px[k+2+1], leave pointers +2
            /* Butterfly at a non-zero angle */ 
            //  ur = *pxk - *pxi;           /* ur = px[k] - px[i];    */ \
            //  *pxk++ += *pxi++;           /* px[k]   += px[i];      */ \
            //  ui = *pxk - *pxi;           /* ui = px[k+1] - px[i+1];*/ \
            //  *pxk++ += *pxi--;           /* px[k+1] += px[i+1];    */ \
            //  *pxi++ = MULT_BP2(CR,ur) - MULT_BP2(SI,ui);  /* px[i]   = cr * ur - (si) * ui; */ \
            //  *pxi++ = MULT_BP2(CR,ui) + MULT_BP2(SI,ur);  /* px[i+1] = cr * ui + (si) * ur; */ \
            // leave pxi and pxk incremented by 2
            DEBUG_COUNT_BTFLY0(4,0,4);
            DEBUG_COUNT_BTFLY1(6,4,6);
        }
        // Nontrivial butterflies 
        for (j = 4; j < l; j += 4) {        /// j: {4,8...1020} {4,8,...510} ... {4} {} [~510x]

            // recurrsion to next sin and cosine values where Theta = 2*PI/m;
            // CR2 = cos(-j*Theta);  SI2 = sin(-j*Theta); CR1 = cos((-j-2)*Theta); SI1 = sin((-j-2)*Theta);
            // recursion: cos(a-2b) = cos(a) - 2*sin(b)*cos(a-b)
            // and:       sin(a-2b) = sin(a) + 2*sin(b)*sin(a-b)
            CR2 -= MULT_BP2(STEP,SI1);
            SI2 += MULT_BP2(STEP,CR1);
            CR1 -= MULT_BP2(STEP,SI2);
            SI1 += MULT_BP2(STEP,CR2);

            // do main butterflies
            for (k = j; k <= n; k += m) {   /// k: {{4}{8}...{1020}} {{4,1028}{8,1032}...{508,1020}} ... {{4,20...2044}} [2048x]
                pxk = &px[k];
                pxi = &px[k + l];
                // +1/-1/W_k butterfly 
                FFTBUTTERFLY_N(pxk,pxi,CR2,SI2);        // update px[i], px[i+1] and px[k], px[k+1], leave pointers at +2
                // +1/-1/W_k butterfly 
                FFTBUTTERFLY_N(pxk,pxi,CR1,SI1);        // update px[i+2], px[i+2+1] and px[k+2], px[k+2+1], leave pointers at +2
                /* Butterfly at a non-zero angle */     \
                //  ur = *pxk - *pxi;           /* ur = px[k] - px[i];    */ \
                //  *pxk++ += *pxi++;           /* px[k]   += px[i];      */ \
                //  ui = *pxk - *pxi;           /* ui = px[k+1] - px[i+1];*/ \
                //  *pxk++ += *pxi--;           /* px[k+1] += px[i+1];    */ \
                //  *pxi++ = MULT_BP2(CR,ur) - MULT_BP2(SI,ui);  /* px[i]   = cr * ur - (si) * ui; */ \
                //  *pxi++ = MULT_BP2(CR,ui) + MULT_BP2(SI,ur);  /* px[i+1] = cr * ui + (si) * ur; */ \
                // leave pxi and pxk incremented by 2
                DEBUG_COUNT_BTFLY1(6,4,6);
                DEBUG_COUNT_BTFLY1(6,4,6);
                DEBUG_COUNT_LOOP(2);
            }
            DEBUG_COUNT_TRIG(2,2);
            DEBUG_COUNT_TRIG(2,2);
            DEBUG_COUNT_LOOP(2);
        }
        m  = l;
        DEBUG_COUNT_TRIG(2,2);
        DEBUG_COUNT_LOOP(2);
    }
    if (m > 2) {
        // m normally exits the loop above == 4, so normally do this except when called with np = 1 or 2
        for (j = 0; j < n; j += 4) {        /// j: {0,4...2044}  [512x]
            pxi = (pxk = px+j) + 2;
            
            // +1/-1 butterfly 
            FFTBUTTERFLY0(pxk,pxi);
            DEBUG_COUNT_BTFLY0(4,0,4);
            DEBUG_COUNT_LOOP(2);
        }
    }
    if (n > 4) {
        I32 n2, n21;
        CoefType tmp;

        n2 = np / 2;                        /// n2: 512
        n21 = np + 1;                       /// n21: 1025
        j = 0;                              
        for (i = 0; i < np; i += 4) {       /// i,j: 0,0; 4,1024; 8,512; 12,1536; ... 2044,??? [255x]

            if (i < j) {
                // swap 4 pairs of values (2 complex pairs with 2 others)
                // px[i] <-> px[j]; px[i+1] <-> px[j+1]
                // px[i+1+n21] <-> px[j+1+n21];  px[i+1+n21+1] <-> px[j+1+n21+1]
                pxi = &px[i];
                pxk = &px[j];
                tmp = *pxi; 
                *pxi++ = *pxk;
                *pxk++ = tmp;
                tmp = *pxi;
                *pxi = *pxk;
                *pxk = tmp;
                pxi  += n21;
                pxk  += n21;
                tmp = *pxi;
                *pxi++ = *pxk;
                *pxk++ = tmp;
                tmp = *pxi;
                *pxi = *pxk;
                *pxk = tmp;
            }

            // swap 2 pairs of values (1 complex pair with another)
            // px[i+2] <-> px[j+np];  px[i+3] <-> px[j+np+1]
            pxi = &px[i+2];
            pxk = &px[j+np];
            tmp = *pxi; 
            *pxi++ = *pxk;
            *pxk++ = tmp;
            tmp = *pxi;
            *pxi = *pxk;
            *pxk = tmp;

            k = n2;                         
            while (k <= j) {                /// k: {1024} {1024,512} {1024} {1024,512,256} ...
                j -= k;
                k = k / 2;
            }
            j += k;                         /// j: {1024} {512} {1536} {256} ...
        }
    }
    if (fftDirection == FFT_INVERSE) // Normalization to match Intel library
        for (i = 0; i < 2 * np; i++) data[i] /= np;
    FUNCTION_PROFILE_STOP(&fp);
}

#else // so must be SPLIT_RADIX_FFT

//****************************************************************************************************************
//
// A Split Radix FFT for the DCT -
// See WMAConcepts.xls - Sheet SRFFT32 for how this works.
// See also Sorensen & Heldeman, IEEE Trans ASSP, Vol ASSP-34, #1, 2/86, pp152-156.
// And also G. M. Blair, Electronics & Comm Engr Journal, August 1995, pp169-177.
//
//****************************************************************************************************************

#define BP2Const6(a,b,c,d,e,f) BP2_FROM_FLOAT(a),BP2_FROM_FLOAT(b),BP2_FROM_FLOAT(c), \
                               BP2_FROM_FLOAT(d),BP2_FROM_FLOAT(e),BP2_FROM_FLOAT(f)

static const BP2Type rgcbp2SrFFTTrig[] = {
//        STEP1             STEP3       
//        2sin(2pi/2^k) Cos(2pi/2^k)  Sin(2pi/2^k)  2sin(6pi/2^k)  Cos(6pi/2^k)  Sin(6pi/2^k)
BP2Const6(1.41421356237,0.70710678119,0.70710678119,1.41421356237,-0.70710678119,0.70710678119),    // K=3
BP2Const6(0.76536686473,0.92387953251,0.38268343237,1.84775906502, 0.38268343237,0.92387953251),    // K=4
BP2Const6(0.39018064403,0.98078528040,0.19509032202,1.11114046604, 0.83146961230,0.55557023302),    // K=5
BP2Const6(0.19603428066,0.99518472667,0.09801714033,0.58056935451, 0.95694033573,0.29028467725),    // K=6
BP2Const6(0.09813534865,0.99879545621,0.04906767433,0.29346094891, 0.98917650996,0.14673047446),    // k=7
BP2Const6(0.04908245705,0.99969881870,0.02454122852,0.14712912720, 0.99729045668,0.07356456360),    // k=8
BP2Const6(0.02454307657,0.99992470184,0.01227153829,0.07361444588, 0.99932238459,0.03680722294),    // k=9
BP2Const6(0.01227176930,0.99998117528,0.00613588465,0.03681345981, 0.99983058180,0.01840672991),    // k=10
BP2Const6(0.00613591353,0.99999529381,0.00306795676,0.01840750956, 0.99995764455,0.00920375478),    // k=11
BP2Const6(0.00306796037,0.99999882345,0.00153398019,0.00920385224, 0.99998941108,0.00460192612),    // k=12
BP2Const6(0.00153398064,0.99999970586,0.00076699032,0.00460193830, 0.99999735277,0.00230096915),    // k=13
BP2Const6(0.00076699038,0.99999992647,0.00038349519,0.00230097067, 0.99999933819,0.00115048534),    // k=14
BP2Const6(0.00038349519,0.99999998162,0.00019174760,0.00115048553, 0.99999983455,0.00057524276),    // k=15
BP2Const6(0.00019174760,0.99999999540,0.00009587380,0.00057524279, 0.99999995864,0.00028762139),    // k=16
0,0,0,0,0,0 };


#if defined(_DEBUG) && 0
// some code to show looping 
FILE* fileFFTDump = NULL;
Int* pFFTIndex = NULL;
#define DUMP_FFT_OPEN(msg,np)                          \
    if (fileFFTDump == NULL )                          \
        fileFFTDump = fopen("DUMP_FFT.TXT","wt");      \
    if (fileFFTDump != NULL )                          \
        fprintf( fileFFTDump, msg, np );               \
    pFFTIndex = wmvrealloc(pFFTIndex,2*np*sizeof(Int));\
    memset( pFFTIndex,0,2*np*sizeof(Int));
#define DUMP_FFT_SET(i,v) pFFTIndex[i] = v;
#define DUMP_FFT_SHOW(msg,k,np)                         \
    {   int i;                                          \
        if (fileFFTDump != NULL ) {                     \
            fprintf( fileFFTDump, msg, k, np );         \
            for( i=0; i<2*np; i += 2 ) {                \
                fprintf( fileFFTDump, "%d,  %d,\n",     \
                    i, pFFTIndex[i] );                  \
            }                                           \
        }                                               \
        memset( pFFTIndex, 0, 2*np*sizeof(Int) );       \
    }
#else
#define DUMP_FFT_OPEN(msg,np)
#define DUMP_FFT_SET(i,v)
#define DUMP_FFT_SHOW(msg,k,np)
#endif

#ifdef BUILD_INTEGER
#   define INTDIV2(a) ((a)>>1)
#else
#   define INTDIV2(a) (a)
#endif

void prvFFT4DCT(CoefType data[], Int nLog2np, FftDirection fftDirection)
{
    Int np = (1<<nLog2np);
    Int np2 = np<<1;
    Int np2m7 = np2-7;
    CoefType *pxk, *pxi, *px0, *px1, *px2, *px3;
    CoefType *px = data;
    I32 i, j, k, iOffset, iStride, iStridem1;
    Int n2k, n2km1, n2km2, n2kp2, idx;
    CoefType tmp, ur0, ui0, ur1, ui1;
    BP2Type bp2Step1, bp2Cos1, bp2Sin1, bp2Step3, bp2Cos3, bp2Sin3;
    BP2Type bp2Cos1p, bp2Sin1p, bp2Cos3p, bp2Sin3p;
    BP2Type bp2Cos1T, bp2Sin1T, bp2Cos3T, bp2Sin3T;
    CoefType ur2, ui2, ur3, ui3, urp, uip, urm, uim;
    const BP2Type* pbp2Trig = rgcbp2SrFFTTrig;
    INTEGER_ONLY( Int n2kp1; )

    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,FFT_PROFILE);
    DUMP_FFT_OPEN("//FFT np = %d\n", np );

    
    assert(fftDirection == FFT_FORWARD); // not implemented inverse yet.
    
    // bit reverse (same code as Radix 2)
    if (np > 2) {
        I32 n2, n21;

        n2 = np / 2;                        /// n2: 512
        n21 = np + 1;                       /// n21: 1025
        j = 0;                              
        for (i = 0; i < np; i += 4) {       /// i,j: 0,0; 4,1024; 8,512; 12,1536; ... 2044,??? [255x]

            if (i < j) {
                // swap 4 pairs of values (2 complex pairs with 2 others)
                // px[i] <-> px[j]; px[i+1] <-> px[j+1]
                // px[i+1+n21] <-> px[j+1+n21];  px[i+1+n21+1] <-> px[j+1+n21+1]
                pxi = &px[i];
                pxk = &px[j];
                tmp = *pxi; 
                *pxi++ = *pxk;
                *pxk++ = tmp;
                tmp = *pxi;
                *pxi = *pxk;
                *pxk = tmp;
                pxi  += n21;
                pxk  += n21;
                tmp = *pxi;
                *pxi++ = *pxk;
                *pxk++ = tmp;
                tmp = *pxi;
                *pxi = *pxk;
                *pxk = tmp;
            }

            // swap 2 pairs of values (1 complex pair with another)
            // px[i+2] <-> px[j+np];  px[i+3] <-> px[j+np+1]
            pxi = &px[i+2];
            pxk = &px[j+np];
            tmp = *pxi; 
            *pxi++ = *pxk;
            *pxk++ = tmp;
            tmp = *pxi;
            *pxi = *pxk;
            *pxk = tmp;

            k = n2;                         
            while (k <= j) {                /// k: {1024} {1024,512} {1024} {1024,512,256} ...
                j -= k;
                k = k / 2;
            }
            j += k;                         /// j: {1024} {512} {1536} {256} ...
        }
    }
 
    // Length 2 butterflies
    for( iOffset = 0, iStride = 8; 
            iOffset < np2; 
                iOffset = (iStride<<1) - 4, iStride <<= 2 )
    {
        iStridem1 = iStride-1;
        for( idx = iOffset, px1 = (px0 = px+iOffset) + 2;
                idx < np2; 
                    idx += iStride, px0 += iStridem1, px1 += iStridem1 )
        {
            ur0 = *px0;
            ur1 = *px1; 
            *px0++ = ur0 + ur1;
            *px1++ = ur0 - ur1;
            ui0 = *px0;
            ui1 = *px1; 
            *px0 = ui0 + ui1;
            *px1 = ui0 - ui1;
            DUMP_FFT_SET(idx,2);
            DUMP_FFT_SET(idx+2,1);
            DEBUG_COUNT_BTFLY0(4,0,4);
            DEBUG_COUNT_LOOP(2);
        }
        DEBUG_COUNT_LOOP(6);
    }
    DUMP_FFT_SHOW("k = %d.\n", 1, np );

    // Radix 4 like Butterflies - either with ot without multiplies
    n2k = 2;
    for( k=2; k<=nLog2np; k++ )
    {
        n2k <<= 1;
        n2km1 = n2k>>1;
        n2km2 = n2k>>2;
        n2kp2 = n2k<<2;
#ifdef BUILD_INTEGER
        n2kp1 = n2k<<1;
        // we must scale all the px's by 2 that will not be accessed in the remainder of this main loop
        for( iOffset = n2kp1, iStride = n2k<<3; 
                iOffset < np2; 
                    iOffset = (iStride<<1) - n2kp1, iStride <<= 2 )
        {
            for( idx = iOffset; idx < np2; idx += iStride )
            {
                for( i = 0, px0 = px+idx; i < n2kp1; i++ )
                    *px0++ >>= 1;
            }
        }
#endif 
        // Trivial Butterflies - j==0 - no multiplies since Cos==1 and Sin==0
        for( iOffset = 0, iStride = n2kp2; 
                iOffset < np2; 
                    iOffset = (iStride - n2k)<<1, iStride <<= 2 )
        {
            px3 = (px2 = (px1 = (px0 = px+iOffset) + n2km1) + n2km1) + n2km1;
            iStridem1 = iStride-1;
            for( idx = iOffset; 
                    idx < np2m7; 
                        idx += iStride, px0 += iStridem1, px1 += iStridem1, px2 += iStridem1, px3 += iStridem1 )
            {
                urp = (ur0=INTDIV2(*px2++)) + (ur1=INTDIV2(*px3++));
                urm = ur0 - ur1;
                uip = (ui0=INTDIV2(*px2--)) + (ui1=INTDIV2(*px3--));
                uim = ui0 - ui1;
                *px2++  = (ur0 = INTDIV2(*px0)) - urp;
                *px0++  = ur0 + urp;
                *px3++  = (ur1 = INTDIV2(*px1)) - uim;
                *px1++  = ur1 + uim;
                *px2    = (ui0 = INTDIV2(*px0)) - uip;
                *px0    = ui0 + uip;
                *px3    = (ui1 = INTDIV2(*px1)) + urm;
                *px1    = ui1 - urm;
                DUMP_FFT_SET(idx,3);
                DUMP_FFT_SET(idx+n2km1,1);
                DUMP_FFT_SET(idx+2*n2km1,1);
                DUMP_FFT_SET(idx+3*n2km1,1);
                DEBUG_COUNT_BTFLY1(12,0,12);
                DEBUG_COUNT_LOOP(2);
            }
            DEBUG_COUNT_LOOP(9);
        }
        // Now the non-trivial butterflies
        if ( n2km2 > 1 )
        {
            if ( *pbp2Trig != 0 )
            {   // normal case with k <= 16
                bp2Step1 = *pbp2Trig++;     // 2*sin(2*pi/2^k)
                bp2Cos1  = *pbp2Trig++;     // cos(2*pi/2^k) 
                bp2Sin1  = *pbp2Trig++;     // sin(2*pi/2^k)
                bp2Step3 = *pbp2Trig++;     // 2*sin(6*pi/2^k)
                bp2Cos3  = *pbp2Trig++;     // cos(6*pi/2^k) 
                bp2Sin3  = *pbp2Trig++;     // sin(6*pi/2^k)
            }
            else
            {   // k > 16 is not normal - but cleaniness before smallness
                bp2Step1 = BP2_FROM_FLOAT( 2*sin(2*PI/n2k) );
                bp2Cos1  = BP2_FROM_FLOAT( cos(2*PI/n2k) );
                bp2Sin1  = BP2_FROM_FLOAT( sin(2*PI/n2k) );
                bp2Step3 = BP2_FROM_FLOAT( 2*sin(6*PI/n2k) );
                bp2Cos3  = BP2_FROM_FLOAT( cos(6*PI/n2k)  );
                bp2Sin3  = BP2_FROM_FLOAT( sin(6*PI/n2k) );
            }
            bp2Cos1p = bp2Cos3p = BP2_FROM_FLOAT(1.0);
            bp2Sin1p = bp2Sin3p = BP2_FROM_FLOAT(0.0);
            for( j = 1; j<n2km2;  j++ )
            {
                //assert( fabs(FLOAT_FROM_BP2(bp2Sin1) - sin((2*PI*j)/n2k)) < 0.0001 );
                //assert( fabs(FLOAT_FROM_BP2(bp2Sin3) - sin((6*PI*j)/n2k)) < 0.0001 );
                for( iOffset = j<<1, iStride = n2kp2; 
                        iOffset < np2; 
                            iOffset = (iStride - n2k + j)<<1, iStride <<= 2 )
                {
                    px3 = (px2 = (px1 = (px0 = px+iOffset) + n2km1) + n2km1) + n2km1;
                    iStridem1 = iStride-1;
                    for( idx = iOffset; 
                            idx < np2m7; 
                        idx += iStride, px0 += iStridem1, px1 += iStridem1, px2 += iStridem1, px3 += iStridem1 )
                    {
#if !defined(WMA_TARGET_SH4)
                        // The pentium prefers this way, but still does not generate wonderful code
                        ur0 = INTDIV2(*px2++);  ui0 = INTDIV2(*px2--);
                        ur2 = MULT_BP2(bp2Cos1,ur0) + MULT_BP2(bp2Sin1,ui0);
                        ui2 = MULT_BP2(bp2Cos1,ui0) - MULT_BP2(bp2Sin1,ur0);
                        ur1 = INTDIV2(*px3++);  ui1 = INTDIV2(*px3--);
                        ur3 = MULT_BP2(bp2Cos3,ur1) + MULT_BP2(bp2Sin3,ui1);
                        ui3 = MULT_BP2(bp2Cos3,ui1) - MULT_BP2(bp2Sin3,ur1);
                        urp     = ur2 + ur3;
                        *px2++  = (ur0 = INTDIV2(*px0)) - urp;
                        *px0++  = ur0 + urp;
                        uim     = ui2 - ui3;
                        *px3++  = (ur1 = INTDIV2(*px1)) - uim;
                        *px1++  = ur1 + uim;
                        uip     = ui2 + ui3;
                        *px2    = (ui0 = INTDIV2(*px0)) - uip;
                        *px0    = ui0 + uip;
                        urm     = ur2 - ur3;
                        *px3    = (ui1 = INTDIV2(*px1)) + urm;
                        *px1    = ui1 - urm;
#else
                        // some platforms might prefer this expression
                        ur0 = INTDIV2(*px2++);  ui0 = INTDIV2(*px2--);
                        ur2 = MULT_BP2(bp2Cos1,ur0) + MULT_BP2(bp2Sin1,ui0);
                        ui2 = MULT_BP2(bp2Cos1,ui0) - MULT_BP2(bp2Sin1,ur0);
                        ur1 = INTDIV2(*px3++); ui1 = INTDIV2(*px3--);
                        ur3 = MULT_BP2(bp2Cos3,ur1) + MULT_BP2(bp2Sin3,ui1);
                        ui3 = MULT_BP2(bp2Cos3,ui1) - MULT_BP2(bp2Sin3,ur1);
                        urp = ur2 + ur3;
                        urm = ur2 - ur3;
                        uip = ui2 + ui3;
                        uim = ui2 - ui3;
                        *px2++  = (ur0 = INTDIV2(*px0)) - urp;
                        *px0++  = ur0 + urp;
                        *px3++  = (ur1 = INTDIV2(*px1)) - uim;
                        *px1++  = ur1 + uim;
                        *px2    = (ui0 = INTDIV2(*px0)) - uip;
                        *px0    = ui0 + uip;
                        *px3    = (ui1 = INTDIV2(*px1)) + urm;
                        *px1    = ui1 - urm;
#endif
                        DUMP_FFT_SET(idx,4);
                        DUMP_FFT_SET(idx+n2km1,1);
                        DUMP_FFT_SET(idx+2*n2km1,1);
                        DUMP_FFT_SET(idx+3*n2km1,1);
                        DEBUG_COUNT_BTFLY2(16,8,12);
                        DEBUG_COUNT_LOOP(2);
                    }
                    DEBUG_COUNT_LOOP(10);
                }
                if ( (j+1) < n2km2 )
                {   // Trig Recurrsion for both 2*pi/2^k and 6*pi/2^k
                    // sin(a+b) = sin(a-b) + 2*sin(b)*cos(a)
                    // cos(a+b) = cos(a-b) - 2*sin(b)*sin(a)
                    // Lay these out like this as ahint to optimizer to overlap operations
                    bp2Sin1T = bp2Sin1p + MULT_BP2(bp2Step1,bp2Cos1);
                    bp2Cos1T = bp2Cos1p - MULT_BP2(bp2Step1,bp2Sin1);
                    bp2Sin3T = bp2Sin3p + MULT_BP2(bp2Step3,bp2Cos3);
                    bp2Cos3T = bp2Cos3p - MULT_BP2(bp2Step3,bp2Sin3);
                    bp2Sin1p = bp2Sin1;  bp2Sin1 = bp2Sin1T;
                    bp2Cos1p = bp2Cos1;  bp2Cos1 = bp2Cos1T;
                    bp2Sin3p = bp2Sin3;  bp2Sin3 = bp2Sin3T;
                    bp2Cos3p = bp2Cos3;  bp2Cos3 = bp2Cos3T;
                    DEBUG_COUNT_TRIG(2,2);
                    DEBUG_COUNT_TRIG(2,2);
                }
                DEBUG_COUNT_LOOP(2);
            }
        }
        DEBUG_COUNT_LOOP(7);
        DUMP_FFT_SHOW("k = %d.\n", k, np );
    }
    FUNCTION_PROFILE_STOP(&fp);
}


#endif  // both !SPLIT_RADIX_FFT and SPLIT_RADIX_FFT


#if defined(_DEBUG)
//*****************************************************************************************
//
// auDctIV
// DCT type IV transform
//
//*****************************************************************************************
//
// define DCT_SIN_PRINT to print sin values as they are used
//#define DCT_SIN_PRINT -1
// define DCT_IN_PRINT to prinf DCT input coefs (positive value to do so for just one frame)
//#define DCT_IN_PRINT 10100
// define DCT_PRE_PRINT to print DCT coefs prior to calling FFT
//#define DCT_PRE_PRINT -1
// define DCT_POST_PRINT to print DCT coefs after calling FFT
//#define DCT_POST_PRINT -1
// define DCT_OUT_PRINT to print DCT output coefs
//#define DCT_OUT_PRINT 10100
// define this to be smaller than 2048 is you only want a sample
#define DCT_MOD_COEF_TO_PRINT 128
#   if defined(DCT_IN_PRINT) || defined(DCT_PRE_PRINT)  || defined(DCT_POST_PRINT) || defined(DCT_OUT_PRINT)
#       pragma COMPILER_MESSAGE(__FILE__ "(725) : Warning - DCT Debug Code Enabled.")
        int bPrintDctAtFrame = 0;       // used by main program to control the printing
        extern int g_ulOutputSamples;   // main must create and update
		void DebugDctPrintCoefs(CAudioObject* pau, int def, int id, float fac, CoefType* pfCoef)
        {   int dcti; float fmax = 0;                                                     
            if ( ((int)pau->m_iFrameNumber==def) || (def < 0) || (bPrintDctAtFrame&(1<<id)) )          
            {   /* MyOutputDbgStr(0, "DCT %4s %4d\n",id,n); matlab does not like */  
				static char* szID[]= { "DCT in  ", "DCT pre ", "DCT post", "DCT out ", "Wgt Fact", "WF down ", "WF up  ", "WF same" };
                for( dcti = 0; dcti < pau->m_cSubbandAdjusted; dcti++ ) {    
					float f = id<4 ? FLOAT_FROM_COEF(pfCoef[dcti])/((Float)fac) :
								     FLOAT_FROM_WEIGHT(pfCoef[dcti]);
                    if ( fmax < fabs(f) )       
                        fmax = (float)fabs(f);
                    if ( dcti < DCT_MOD_COEF_TO_PRINT || (dcti%DCT_MOD_COEF_TO_PRINT)==0 ) 
#						ifdef UNDER_CE
							MyOutputDbgStr(0, 
#						else
							printf(
#						endif
						"%4d.%1d   %4d  %+15.7f\n",          
                            pau->m_iFrameNumber, pau->m_iCurrSubFrame, dcti, f );
                }
#				ifdef UNDER_CE
					MyOutputDbgStr(0, szID[id]);
					MyOutputDbgStr(0, 
#				else
					printf(szID[id]);
					printf(
#				endif
					" %4d  %+15.7f  %7d\n", pau->m_cSubbandAdjusted, fmax, g_ulOutputSamples );
#				ifndef UNDER_CE
					fflush(stdout); 
#				endif
            }
        }
#       define DCT_PRINT(def,id,fac,pfCoef)  DebugDctPrintCoefs(pau,def,id,fac,pfCoef)
#   else
#       define DCT_PRINT(def,id,fac,pfCoef)
#   endif
#   if defined(DCT_IN_PRINT)
#       define DCT_DOPRINT_IN(fac,pfCoef) DCT_PRINT(DCT_IN_PRINT,0,fac,pfCoef)
#   else
#       define DCT_DOPRINT_IN(fac,pfCoef)
#   endif
#   if defined(DCT_PRE_PRINT)
#       define DCT_DOPRINT_PRE(fac,pfCoef) DCT_PRINT(DCT_PRE_PRINT,1,n,fac,pfCoef)
#   else
#       define DCT_DOPRINT_PRE(fac,pfCoef)
#   endif
#   if defined(DCT_POST_PRINT)
#       define DCT_DOPRINT_POST(fac,pfCoef) DCT_PRINT(DCT_POST_PRINT,2,n,fac,pfCoef)
#   else
#       define DCT_DOPRINT_POST(fac,pfCoef)
#   endif
#   if defined(DCT_OUT_PRINT)
#       define DCT_DOPRINT_OUT(fac,pfCoef) DCT_PRINT(DCT_OUT_PRINT,3,n,fac,pfCoef)
#   else
#       define DCT_DOPRINT_OUT(fac,pfCoef)
#   endif
#   if defined(DCT_SIN_PRINT)
#       pragma COMPILER_MESSAGE(__FILE__ "(774) : Warning - DCT Debug Code Enabled.")
#       define DEBUG_DCT_SIN(id,i,fac,sinv)                                     \
            if ( (pau->m_iFrameNumber==DCT_SIN_PRINT) || (DCT_SIN_PRINT < 0) )  \
            {   printf("%2d  %4d  %+15.8f\n", id, i, sinv/fac );                \
                fflush(stdout);                                                 \
            }
#   else
#       define DEBUG_DCT_SIN(id,i,fac,sinv)
#   endif
#else
#       define DCT_DOPRINT_IN(fac,pfCoef)
#       define DCT_DOPRINT_PRE(fac,pfCoef)
#       define DCT_DOPRINT_POST(fac,pfCoef)
#       define DCT_DOPRINT_OUT(fac,pfCoef)
#       define DEBUG_DCT_SIN(id,i,fac,sinv)
#endif

#ifdef INTEGER_ENCODER
#define INTEGER_ENCODER_ONLY(a) a
#else
#define INTEGER_ENCODER_ONLY(a)
#endif

WMARESULT auDctIV(CAudioObject* pau, CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude)
{
    CoefType *piCoefTop, *piCoefBottom, *piCoefBottomOut;
    CoefType iTr, iTi, iBr, iBi;
    BP1Type CR, CI, UR, UI, STEP, CR1, CI1, CR2, CI2;
    Int iFFTSize, i, cSB, nLog2SB;
    BP1Type  iFac;
    const SinCosTable* pSinCosTable;
    double fac;
    const double dPI = PI;
    const double dPI3Q = 3*PI/4;                        // PI (-1/4 + 1)
    const double dPIby4 = PI/4;
#if defined(BUILD_INTEGER)
    const double d1p0   = 1.0 * NF2BP1;
    const double d2p0   = 2.0 * NF2BP1;
    const double d1Hp0  = 0.5 * NF2BP1;
    const double d1Qp0  = 0.25* NF2BP1;
    Int nFacExponent;
    U32 iMagnitude = 0;
#else  // must be BUILD_INT_FLOAT
    const double d1p0   = 1.0;
    const double d2p0   = 2.0;
    const double d1Hp0  = 0.5;
    const double d1Qp0  = 0.25;
#endif  // BUILD_INTEGER or BUILD_INT_FLOAT

#ifdef INTEGER_ENCODER
    Int iScaleFactor;
#endif

    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,DCTIV_PROFILE);

    // m_cSubbandAdjusted below deals with the need to scale transform results to compensate the fact 
    // that we're inv transforming coefficients from a transform that was twice or half our size

    cSB = pau->m_cSubbandAdjusted;
    iFFTSize = cSB/2;
    nLog2SB = LOG2( cSB );

    piCoefTop          = rgiCoef;               
    piCoefBottom       = rgiCoef + cSB - 1;
    piCoefBottomOut    = rgiCoef + cSB - 1;     // separate pointer now obsolete but still used

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
    prvWmaShowFrames(pau, "DCT", " %4d 0x%08x 0x%08x", cSB, piCoefTop, piCoefBottom);
#endif

    // fltAfterScaleFactor:  V2: 2/cSB == 1/iFFTSize and V1: sqrt(2/cSB)
#if defined(BUILD_INTEGER)
    fac = fltAfterScaleFactor * cSB * d1Qp0;
    for( nFacExponent = 0; fabs(fac) > d1p0; nFacExponent++ )
        fac /= 2.0f;        // SH warning CBE4717 here is ignorable
#else   // must be BUILD_INT_FLOAT
    fac = fltAfterScaleFactor;
#endif

    // initialize sin/cos recursion
    // note this style of recurrsion is more accurate than Numerical Recipies 5.5.6
    if ( 64 <= cSB && cSB <= 2048 )
    {
        pSinCosTable = rgSinCosTables[cSB>>7];
#       if defined(BUILD_INTEGER)
            iFac = (I32)ROUNDD( fac );
#       else  // must be BUILD_INT_FLOAT
            iFac = (BP1Type)fac;
#       endif // BUILD_INTEGER or BUILD_INT_FLOAT
        // initial cosine/sine values
        CR =  MULT_BP1(iFac,pSinCosTable->cos_PIby4cSB);        // CR = (I32)(fac*cos(-PI/(4*m_cSubband)) * NF2BP1)
        CI = -MULT_BP1(iFac,pSinCosTable->sin_PIby4cSB);        // CI = (I32)(fac*sin(-PI/(4*m_cSubband)) * NF2BP1)
        // prior cosine/sine values to init Pre-FFT recurrsion trig( -PI/(4*M) - (-PI/M ) = trig( 3*PI/(4*M) )
        CR1 =  MULT_BP1(iFac,pSinCosTable->cos_3PIby4cSB);  // CR = (I32)(fac*cos(+3*PI/(4*m_cSubband)) * NF2BP1)
        CI1 =  MULT_BP1(iFac,pSinCosTable->sin_3PIby4cSB);  // CI = (I32)(fac*sin(+3*PI/(4*m_cSubband)) * NF2BP1)
        // rotation step for both recurrsions
        STEP = -pSinCosTable->two_sin_PIbycSB;              // STEP = 2*sin(-PI/m_cSubband) 
        // prior cosine/sine values to init Post-FFT recurrsion
        CR2 =  pSinCosTable->cos_PIbycSB;                   // CR = (I32)(cos( PI/m_cSubband) * NF2BP1)
        CI2 =  pSinCosTable->sin_PIbycSB;                   // CI = (I32)(sin( PI/m_cSubband) * NF2BP1)
    }
    else
    {   // not normally needed in decoder, here for generality
        double dA, dSB;

        dA = -dPIby4 / (dSB=cSB);
        CR = (BP1Type)( fac * cos( dA ) );
        CI = (BP1Type)( fac * sin( dA ) );
        dA = dPI3Q / dSB;
        CR1 = (BP1Type)( fac * cos( dA ) );
        CI1 = (BP1Type)( fac * sin( dA ) );
        dA = -dPI / dSB;
        STEP = (BP1Type)( d2p0 * sin( dA ) );
        CR2  = (BP1Type)( d1p0 * cos( dA ) );
        CI2  = -STEP/2;  
    }

    // Include the next code-block to verify changes to the lookup-table
#if 0
    {
#       if defined(BUILD_INTEGER)
            const BP1Type Theshold = 1;
#       else
            const BP1Type Theshold = BP1_FROM_FLOAT(0.001F*fac);
#       endif
        assert(BPX_ABS(CR - (BP1Type)( fac * cos(-dPIby4 / cSB) )) <= Theshold);
        assert(BPX_ABS(CI - (BP1Type)( fac * sin(-dPIby4 / cSB) )) <= Theshold);
        assert(BPX_ABS(CR1 - (BP1Type)( fac * cos(dPI3Q / cSB) )) <= Theshold);
        assert(BPX_ABS(CI1 - (BP1Type)( fac * sin(dPI3Q / cSB) )) <= Theshold);
        assert(BPX_ABS(STEP - (BP1Type)( d2p0 * sin(-dPI / cSB) )) <= Theshold);
        assert(BPX_ABS(CR2  - (BP1Type)( d1p0 * cos(-dPI / cSB) )) <= Theshold);
        assert(BPX_ABS(CI2  - DIV2((-(BP1Type)( d2p0 * sin(-dPI / cSB) )))) <= Theshold);
    }
#endif

    DCT_DOPRINT_IN(1.0,rgiCoef);

    for (i = iFFTSize/2; i > 0; i--) {
        iBi = piCoefBottom[0];
        piCoefBottomOut[0] = piCoefTop[1];

        iTr = piCoefTop[0];
        piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi);
        piCoefTop[1] = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr);

        DEBUG_DCT_SIN(0,i,fac,CI);
        MONITOR_RANGE(gMR_DCTCoefIntoFFT,FLOAT_FROM_COEF(piCoefTop[0]));
        MONITOR_RANGE(gMR_DCTCoefIntoFFT,FLOAT_FROM_COEF(piCoefTop[1]));
        // rotate angle by -b = -pi/cSubband
        // recursion: cos(a-b) = cos(a+b) - 2*sin(b)*sin(a)
        // and:       sin(a-b) = sin(a+b) + 2*sin(b)*cos(a)
        UR = CR1 - MULT_BP1(STEP,CI);
        UI = CI1 + MULT_BP1(STEP,CR);
        CR1 = CR;  CR = UR;
        CI1 = CI;  CI = UI;

        piCoefTop += 2;
        piCoefBottom -= 2;
        piCoefBottomOut -= 2;
    }

    for (i = iFFTSize/2; i > 0; i--) {
        iTr = piCoefTop[0];
        iTi = piCoefTop[1];
        piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi);
        piCoefTop[1] = MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr);

        DEBUG_DCT_SIN(1,i,fac,CI);
        MONITOR_RANGE(gMR_DCTCoefIntoFFT,FLOAT_FROM_COEF(piCoefTop[0]));
        MONITOR_RANGE(gMR_DCTCoefIntoFFT,FLOAT_FROM_COEF(piCoefTop[1]));
        // rotate angle by b = -pi/cSubband
        UR = CR1 - MULT_BP1(STEP,CI);
        UI = CI1 + MULT_BP1(STEP,CR);
        CR1 = CR;  CR = UR;
        CI1 = CI;  CI = UI;

        piCoefTop += 2;
    }

    DCT_DOPRINT_PRE((fac/d1Hp0),rgiCoef);

    //Call the integer FFT explicitly
    pau->aupfnFFT(rgiCoef, nLog2SB - 1, FFT_FORWARD);

    DCT_DOPRINT_POST(1.0,rgiCoef);

#ifdef INTEGER_ENCODER
    // TODO:  put this in asm for SH4, SH3 and casio
    if (nFacExponent > 0)
    {   // This scaling needed for encoding only (we think hopefully)
        const int c_iMaxScaleUp = 15;

        iScaleFactor = nFacExponent;

        // Limit pre-scale to +c_iMaxScaleUp (to avoid overflow)
        if (iScaleFactor > c_iMaxScaleUp)
        {
            nFacExponent = (iScaleFactor - c_iMaxScaleUp);
            iScaleFactor = c_iMaxScaleUp;
        }
        else
            nFacExponent = 0;

#ifdef _DEBUG
        if (iScaleFactor > 0)
        {
            I32 iIntMax = INT_MAX >> iScaleFactor;  
            piCoefTop      = rgiCoef; 
            for( i = cSB; i > 0; i--, piCoefTop++ )
            {
                assert(-iIntMax <= (I32)(*piCoefTop)  && (I32)(*piCoefTop) <= iIntMax );
            }
        }
#endif
    }

#endif

    // post FFT demodulation 
    // using R[i], I[i] as real and imaginary parts of complex point i
    // and C(i), S(i) are cos(i*pi/cSubband) and sin(i*pi/cSubband), and N = cSubband/2
    // R[0] = C(0) R[0] - S(0) I[0]
    // I[0] = -S(-(N-1)) R[N-1] - C(-(N-1)) I[N-1]
    // R[1] = C(1) R[1] - S(-1) I[N-1]
    // I[1] = -S(-(N-2)) R[N-2] - C(-(N-2)) I[N-2]
    // ...
    // R[N-2] = C(-(N-2)) R[N-2] - S(-(N-2)) I[N-2]
    // I[N-2] = -S(-1) R[1] - C(-1) I[1]
    // R[N-1] = C(-(N-1)) R[N-1] - S(-(N-1)) I[N-1]
    // R[N-1] = -S(0) R[0] - C(0) I[0]
    // and where 90 - angle trig formulas reveal:
    // C(-(N-i)) == -S(-i) and S(-(N-i)) = -C(-i)

    piCoefTop      = rgiCoef;           //reuse this pointer; start from head;
    piCoefBottom   = rgiCoef + cSB - 2; //reuse this pointer; start from tail;
    CR = BP1_FROM_FLOAT(1);             //one
    CI = 0;                             //zero

    for (i = iFFTSize/2; i > 0; i--) {
        iTr = piCoefTop[0] INTEGER_ENCODER_ONLY( <<iScaleFactor );
        iTi = piCoefTop[1] INTEGER_ENCODER_ONLY( <<iScaleFactor );
        iBr = piCoefBottom[0] INTEGER_ENCODER_ONLY( <<iScaleFactor );
        iBi = piCoefBottom[1] INTEGER_ENCODER_ONLY( <<iScaleFactor );

        piCoefTop[0] =  MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi);
        piCoefBottom[1] =  MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi);
        INTEGER_ONLY( iMagnitude |= abs(piCoefTop[0]) );
        INTEGER_ONLY( iMagnitude |= abs(piCoefBottom[1]) );

        DEBUG_DCT_SIN(2,i,d1p0,CI);
        MONITOR_RANGE(gMR_DCTCoefOut,FLOAT_FROM_COEF(piCoefTop[0]));
        MONITOR_RANGE(gMR_DCTCoefOut,FLOAT_FROM_COEF(piCoefBottom[1]));
        // rotate angle by -b = -pi/cSubband
        // recursion: cos(a-b) = cos(a+b) - 2*sin(b)*sin(a)
        // and:       sin(a-b) = sin(a+b) + 2*sin(b)*cos(a)
        UR = CR2 - MULT_BP1(STEP,CI);
        UI = CI2 + MULT_BP1(STEP,CR);
        CR2 = CR;  CR = UR;
        CI2 = CI;  CI = UI;

        // note that cos(-(cSubband/2 - i)*pi/cSubband ) = -sin( -i*pi/cSubband )
        piCoefTop[1] = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi);
        piCoefBottom[0] = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi);
        
        INTEGER_ONLY( iMagnitude |= abs(piCoefTop[1]) );
        INTEGER_ONLY( iMagnitude |= abs(piCoefBottom[0]) );

        MONITOR_RANGE(gMR_DCTCoefOut,FLOAT_FROM_COEF(piCoefTop[1]));
        MONITOR_RANGE(gMR_DCTCoefOut,FLOAT_FROM_COEF(piCoefBottom[0]));

        piCoefTop += 2;
        piCoefBottom -= 2;
    }
#if defined(BUILD_INTEGER)
    if ( nFacExponent > 0 )
    {   // This scaling needed in v1 bit-streams
        piCoefTop      = rgiCoef; 
        iMagnitude <<= nFacExponent;
        for( i = cSB; i > 0; i-- )
        {
            *piCoefTop++ <<= nFacExponent;
        }
    }
#endif

    if (NULL != piMagnitude)
        *piMagnitude = INTEGER_OR_INT_FLOAT( iMagnitude, 0 );

    DCT_DOPRINT_OUT(1.0,rgiCoef);
#if defined(DCT_OUT_PRINT) && defined(_DEBUG)
    if ( pau->m_iFrameNumber==DCT_OUT_PRINT)
    { DEBUG_BREAK(); }
#endif
    FUNCTION_PROFILE_STOP(&fp);

    return WMA_OK;
}



#endif // WMA_TARGET_MIPS, WMA_TARGET_SH3, WMA_TARGET_SH4 and everything else


//#define PLATFORM_LPC_LITDFT

#if defined(PLATFORM_LPC_LITDFT) 

// A Split Radix FFT for the LPC -
// Size (which is 32) must match that required by LPC
// See WMAConcepts.xls - Sheet SRFFT32 for how this works.
// See also Sorensen & Heldeman, IEEE Trans ASSP, Vol ASSP-34, #1, 2/86, pp152-156.

#define NP 32
#define LOGNP 5
#define NP2 (2*NP)
#define SWAP(a,b) tmp=(a);(a)=(b);(b)=tmp

#define BP2_CONST4(a,b,c,d) BP2_FROM_FLOAT(a),BP2_FROM_FLOAT(b),BP2_FROM_FLOAT(c),BP2_FROM_FLOAT(d)

const BP2Type rgbp2SRFFT32[] = {
//     cos(2*PI*j/2^k) sin(2*PI*j/2^k) cos(6*PI*j/2^k) sin(6*PI*j/2^k)
BP2_CONST4(0.70710678119,0.70710678119,-0.70710678119,0.70710678119),   // k=3, j=1
BP2_CONST4(0.92387953251,0.38268343237,0.38268343237,0.92387953251),    // k=4, j=1
BP2_CONST4(0.70710678119,0.70710678119,-0.70710678119,0.70710678119),   // k=4, j=2
BP2_CONST4(0.38268343237,0.92387953251,-0.92387953251,-0.38268343237),  // k=4, j=3
BP2_CONST4(0.98078528040,0.19509032202,0.83146961230,0.55557023302),    // k=5, j=1
BP2_CONST4(0.92387953251,0.38268343237,0.38268343237,0.92387953251),    // k=5, j=2
BP2_CONST4(0.83146961230,0.55557023302,-0.19509032202,0.98078528040),   // k=5, j=3
BP2_CONST4(0.70710678119,0.70710678119,-0.70710678119,0.70710678119),   // k=5, j=4
BP2_CONST4(0.55557023302,0.83146961230,-0.98078528040,0.19509032202),   // k=5, j=5
BP2_CONST4(0.38268343237,0.92387953251,-0.92387953251,-0.38268343237),  // k=5, j=6
BP2_CONST4(0.19509032202,0.98078528040,-0.55557023302,-0.83146961230)   // k=5, j=7
};

static const I8 rgi8BitReverse32[2*12] = {
    2,32,  4,16,  6,48,  10,40, 12,24, 14,56, 
    18,36, 22,52, 26,44, 30,60, 38,50, 46,58 
};


void prvFFT32(CoefType data[])
{
    CoefType *px = data;
    I32 i, j, k, iOffset, iStride;
    Int n2k, n2km1, n2km2, n2kp2, idx0, idx1, idxR0, idxR1, idxR2, idxR3;
    CoefType tmp, ur0, ui0, ur1, ui1;
    BP2Type bp2Cos1, bp2Sin1, bp2Cos3, bp2Sin3;
    const BP2Type* pbp2Trig = rgbp2SRFFT32;
    CoefType ur2, ui2, ur3, ui3, urp, uip, urm, uim;

    //FUNCTION_PROFILE(fp);
    //FUNCTION_PROFILE_START(&fp,FFT32_PROFILE);

    // bit reverse
    for( i = 0; i < (2*12); i += 2 )
    {
        SWAP( px[rgi8BitReverse32[i]], px[rgi8BitReverse32[i+1]] );
        SWAP( px[rgi8BitReverse32[i]+1], px[rgi8BitReverse32[i+1]+1] );
    }

    // Length 2 butterflies
    for( iOffset = 0, iStride = 8; 
            iOffset < NP2; 
                iOffset = (iStride<<1) - 4, iStride <<= 2 )
    {
        for( idx0 = iOffset; idx0<NP2; idx0 += iStride-1 )
        {
            ur0 = px[idx0];
            ur1 = px[idx1 = idx0+2 ]; 
            px[idx0++] = ur0 + ur1;
            px[idx1++] = ur0 - ur1;
            ui0 = px[idx0];
            ui1 = px[idx1]; 
            px[idx0] = ui0 + ui1;
            px[idx1] = ui0 - ui1;
        }
    }

    // L Shaped Butterflies
    n2k = 2;
    for( k=2; k<=LOGNP; k++ )
    {
        n2k <<= 1;
        n2km1 = n2k>>1;
        n2km2 = n2k>>2;
        n2kp2 = n2k<<2;
        // Trivial Butterflies - j==0 - no multiplies since Cos==1 and Sin==0
        for( iOffset = 0, iStride = n2kp2; 
                iOffset < NP2; 
                    iOffset = (iStride - n2k)<<1, iStride <<= 2 )
        {
            for( idxR0 = iOffset; 
                    idxR0 < (NP2-7); 
                        idxR0 += iStride )
            {
                idxR1 = idxR0 + n2km1;
                idxR2 = idxR1 + n2km1;
                idxR3 = idxR2 + n2km1;
                urp = px[idxR2]   + px[idxR3];
                urm = px[idxR2]   - px[idxR3];
                uip = px[idxR2+1] + px[idxR3+1];
                uim = px[idxR2+1] - px[idxR3+1];
                px[idxR2]    = px[idxR0] - urp;
                px[idxR0]   += urp;
                px[idxR3]    = px[idxR1] - uim;
                px[idxR1]   += uim;
                px[idxR2+1]  = px[idxR0+1] - uip;
                px[idxR0+1] += uip;
                px[idxR3+1]  = px[idxR1+1] + urm;
                px[idxR1+1] -= urm;
            }
        }
        // Now the non-trivial butterflies
        for( j = 1; j<n2km2;  j++ )
        {
            bp2Cos1 = *pbp2Trig++;      // cos(2*pi*j/n2k)
            bp2Sin1 = *pbp2Trig++;      // sin(2*pi*j/n2k)
            bp2Cos3 = *pbp2Trig++;      // cos(3*2*pi*j/n2k)
            bp2Sin3 = *pbp2Trig++;      // sin(3*2*pi*j/n2k)
            assert( fabs(FLOAT_FROM_BP2(bp2Sin1) - sin((2*PI*j)/n2k)) < 0.0001 );
            for( iOffset = j<<1, iStride = n2kp2; 
                    iOffset < NP2; 
                        iOffset = (iStride - n2k + j)<<1, iStride <<= 2 )
            {
                for( idxR0 = iOffset; 
                        idxR0 < (NP2-7); 
                            idxR0 += iStride )
                {
                    idxR1 = idxR0 + n2km1;
                    idxR2 = idxR1 + n2km1;
                    idxR3 = idxR2 + n2km1;
                    ur2 = bp2Cos1*px[idxR2]   + bp2Sin1*px[idxR2+1];
                    ui2 = bp2Cos1*px[idxR2+1] - bp2Sin1*px[idxR2];
                    ur3 = bp2Cos3*px[idxR3]   + bp2Sin3*px[idxR3+1];
                    ui3 = bp2Cos3*px[idxR3+1] - bp2Sin3*px[idxR3];
                    urp = ur2 + ur3;
                    urm = ur2 - ur3;
                    uip = ui2 + ui3;
                    uim = ui2 - ui3;
                    px[idxR2]    = px[idxR0] - urp;
                    px[idxR0]   += urp;
                    px[idxR3]    = px[idxR1] - uim;
                    px[idxR1]   += uim;
                    px[idxR2+1]  = px[idxR0+1] - uip;
                    px[idxR0+1] += uip;
                    px[idxR3+1]  = px[idxR1+1] + urm;
                    px[idxR1+1] -= urm;
                }
            }
        }
    }
    //FUNCTION_PROFILE_STOP(&fp);
}


#undef SWAP

#endif  // defined(PLATFORM_LPC_LITDFT) || 1
