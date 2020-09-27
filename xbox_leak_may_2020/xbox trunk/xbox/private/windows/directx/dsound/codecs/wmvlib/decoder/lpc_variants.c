#include "bldsetup.h"


// included from lpc.c

// ***********************************************************************************
// ***********************************************************************************
//
// LPC to Spectrum 
//
// There are four versions of this routine.
// A: Direct computation
// B: One that calls the DCT's FFT routine instead of taking advantage of the zeros
// C: one that takes advantage of the large number of zeros to reduce compuatation
// D: like A but which attempts to duplicate the numerics of V4/V5
//
// ************************************************************************************
// ************************************************************************************

#if defined(PLATFORM_LPC_DIRECT) && !defined(V4V5_COMPARE_MODE)

#pragma COMPILER_MESSAGE(__FILE__ "(20) : Warning - building PLATFORM_LPC_DIRECT LPC spectrum.")

// ************************************************************************************
//
// LPC to Spectrum using direct computation 
//
// Not the fastest generally, but it takes the minimum amount of memory 
// and so flushes the cache the minimum of any of these methods.
//
// ************************************************************************************

WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    LpSpecType* F = INTEGER_OR_INT_FLOAT( ppcinfo->m_rgiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    Int i;
    
    WeightType* rgwtLpcSpec;
    WeightType  wtLpcSpecMax, wtTemp;
    Int iFreq, iF, iLoopLimit;
    
#ifdef REPLICATE_V4_LPC
    Int iSize = pau->m_cFrameSampleHalf;
#else
    Int iSize;
    if (pau->m_fV5Lpc)
        iSize = pau->m_cSubband;
    else 
        iSize = pau->m_cFrameSampleHalf;
#endif
    Int iStride = MAX_LP_SPEC_SIZE/iSize;
    BP2Type bp2Cos_i, bp2Sin_i, bp2Cos_i1, bp2Sin_i1, bp2Step_i;
    BP2Type bp2Cos_j, bp2Sin_j, bp2Cos_j1, bp2Sin_j1, bp2Step_j;
    BP2Type bp2CosT, bp2SinT;
    const SinCosTable* pSinCosTable;
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);
    
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    
    // sin(a+b) = sin(a-b) + 2*sin(b)*cos(a)
    // cos(a+b) = cos(a-b) - 2*sin(b)*sin(a)
    pSinCosTable = rgSinCosTables[iSize>>7];
    bp2Cos_j1 = bp2Cos_j  = bp2Cos_i = BP2_FROM_FLOAT(1);
    bp2Step_j = bp2Sin_j1 = bp2Sin_j = bp2Sin_i = BP2_FROM_FLOAT(0);
    bp2Cos_i1 =  BP2_FROM_BP1(pSinCosTable->cos_PIbycSB);                           // BP2_FROM_FLOAT( cos( -PI/iSize ) );
    bp2Sin_i1 = -BP2_FROM_BP1(pSinCosTable->sin_PIbycSB);                           // BP2_FROM_FLOAT( sin( -PI/iSize ) );
    bp2Step_i = INTEGER_OR_INT_FLOAT( pSinCosTable->sin_PIbycSB, -MUL2(bp2Sin_i1) );        // 2*sin(PI/iSize)
    for( i = 0, iF = 0; i<iSize; i++, iF += iStride )
    {
        LpSpecType R = 1;
        LpSpecType I = 0;
        int j;
        for (j = 0; j < LPCORDER; j++)  
        {
            R -= MULT_BP2( LP_SPEC_FROM_LP(rgLpcCoef [j]), bp2Cos_j );
            I -= MULT_BP2( LP_SPEC_FROM_LP(rgLpcCoef [j]), bp2Sin_j );
            bp2SinT = bp2Sin_j1 + MULT_BP2(bp2Step_j,bp2Cos_j);
            bp2CosT = bp2Cos_j1 - MULT_BP2(bp2Step_j,bp2Sin_j);
            bp2Sin_j1 = bp2Sin_j;  bp2Sin_j = bp2SinT;
            bp2Cos_j1 = bp2Cos_j;  bp2Cos_j = bp2CosT;
        }
        F[i] = SQUARE( R ) + SQUARE( I );
        INTEGER_ONLY( assert( F[i] >= 0 ) );
        LPC_FT_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,i,R,I,F[i]);
        bp2SinT = bp2Sin_i1 + MULT_BP2(bp2Step_i,bp2Cos_i);
        bp2CosT = bp2Cos_i1 - MULT_BP2(bp2Step_i,bp2Sin_i);
        bp2Sin_i1 = bp2Sin_i;  bp2Sin_j = bp2Sin_i = bp2SinT;
        bp2Cos_i1 = bp2Cos_i;  bp2Cos_j = bp2Cos_i = bp2CosT;
        bp2Sin_j1 = BP2_FROM_FLOAT(0);
        bp2Cos_j1 = BP2_FROM_FLOAT(1);
        bp2Step_j = MUL2( bp2Sin_j );
        // This takes 10*2*N multiplies and 10*2*N additions
        // Plus 2*N multiplies and N Additions
        // total = 22*N multiplies and 21*N additions
        // cache use is 4*N + 2048*10*4*2 = 160k +4N !!!
    }
    
    // the remainder should be identical to the other versions of this routine
    
    rgwtLpcSpec  = INTEGER_OR_INT_FLOAT( ppcinfo->m_rgiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    iLoopLimit   = pau->m_cSubband;
    
    for (iFreq = 0, iF = 0; iFreq < iLoopLimit; iFreq++, iF += iStride)     
    {
        LPC_COMPARE_DEBUG( pau, iFreq, iFreq, iF, F, rgLpcCoef );
        LPC_SPEC_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,iFreq,F[iFreq],InverseQuadRoot(F[iFreq]));
        
        rgwtLpcSpec [iFreq] = wtTemp = InverseQuadRoot(F[iFreq]);
        
        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
        
        INTEGER_ONLY( assert( wtTemp>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,wtTemp);                                                            
        MONITOR_RANGE(gMR_fltLPC_F3,F[iFreq]);
    }
    
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

#endif  // defined(PLATFORM_LPC_DIRECT) && !defined(V4V5_COMPARE_MODE)


#if defined(PLATFORM_LPC_LITDFT) && !defined(V4V5_COMPARE_MODE)

#pragma COMPILER_MESSAGE(__FILE__ "(142) : Warning - building PLATFORM_LPC_LITDFT LPC spectrum")

// ************************************************************************************
//
// LPC to Spectrum using Transform Decomposition DFT for a subset of input points
//
// See Sorensen & Burrus, "Efficent computation of the DFT with only a subset of input
// and ouput points", IEEE Trans. Signal Processing, Vol 41, No 3, March 1993, p1184-1200.
//
// Their algorithm is further reduced by taking advantage that the input is real.
//
// 
//
// ************************************************************************************

WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    // compute in place reduces cahce use
    LpSpecType* F = INTEGER_OR_INT_FLOAT( ppcinfo->m_rgiWeightFactor, ppcinfo->m_rgfltWeightFactor );
    LpSpecType* pF;
    Int i,j,k;
    
    LpSpecType* rgwtLpcSpec;
    WeightType wtLpcSpecMax, wtTemp;
    Int iFreq, iF, iLoopLimit;
    
#ifdef REPLICATE_V4_LPC
    Int iSize = pau->m_cFrameSampleHalf;
#else
    Int iSize;
    if (pau->m_fV5Lpc)
        iSize = pau->m_cSubband;
    else 
        iSize = pau->m_cFrameSampleHalf;
#endif
    Int iStride = MAX_LP_SPEC_SIZE/iSize;
    extern void prvFFT4DCT(CoefType data[], Int nLog2np, FftDirection fftDirection);
    extern void four1(float data[], unsigned long nn, int isign);
    const BP2Type* pTrig = &rgBP2LpcLitdftTrig[2*LPCORDER*iStride];
    DEBUG_ONLY( double dDiff; )
        
        // Sorensen & Burrus' parameters
        const Int iN   = iSize; 
    const Int iNhalf  = iN>>1;
#       define LL (LPCORDER+1)
#       define PP  32
#       define LOGPP 5
#       define LL2 (2*LL)
#       define PP2 (2*PP)
#       define PPH (PP>>1)
    const Int iQ   = iN>>LOGPP;     // 64 to 4.
    const int iQ2  = iQ<<1;
    LpSpecType X[LL], XX[PP2*2], *pX, *pXX;      
    extern void prvFFT32( LpSpecType XXD[] );
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);
    
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    assert( LL <= PP && (1<<LOG2(PP)) == PP);
    
#if defined(_DEBUG) && 0
    {       // this should match calculations in decoder\WMAConcepts.xls!SRFFT32
        LpSpecType XXD[PP2*2];      
        for( k = 0; k < 32; k++ )
        {
            XXD[2*k] = (((float)k)/PP)-(k>PPH ? 1 : 0);
            XXD[2*k+1] = 0;
        }
        prvFFT32( XXD );
    }
#endif
    
    pX = &X[0];
    *pX++ = LP_SPEC_FROM_FLOAT(1.0f);
    for (i = 1; i <= LPCORDER; i++) 
    {
        *pX++ = -(LP_SPEC_FROM_LP(rgLpcCoef[i-1]));
    }
    
    
    for( j = 1; j < iQ; j++ )
    {
        memset( XX+LL2, 0, sizeof(LpSpecType)*(PP2-LL2) );
        assert( (dDiff = fabs(FLOAT_FROM_BP2(pTrig[1])+sin((PI*j)/iN))) < 0.0001 );
        XX[0] = X[0];
        XX[1] = 0;
        pX = &X[1];
        pXX = &XX[2];
        for( k = 1; k<LL; k++, pX++ )
        {
            assert( (dDiff = fabs(FLOAT_FROM_BP2(pTrig[1])+sin((PI*j*k)/iN))) < 0.0001 );
            *pXX++ =  MULT_BP2(pTrig[0],*pX);
            *pXX++ =  MULT_BP2(pTrig[1],*pX);
            pTrig += 2;
        }
        //prvFFT4DCT( XX, LOGPP, FFT_FORWARD );  
        prvFFT32( XX );
        for( k = 0, pXX = &XX[0], pF = &F[j];  k<PPH;  k++, pXX += 2, pF += iQ2 )
        {
            *pF = SQUARE( pXX[0] ) + SQUARE( pXX[1] );
            LPC_FT_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,2*iQ*k+j,XX[2*k],XX[2*k+1],F[2*iQ*k+j]);
        }
        for( k = 0, pF = &F[iSize-j];  k<PPH;  k++, pXX += 2, pF -= iQ2 )
        {
            *pF = SQUARE( pXX[0] ) + SQUARE( pXX[1] );
            LPC_FT_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,iSize-(2*iQ*k+j),XX[PP+2*k],XX[PP+2*k+1],F[iSize-(2*iQ*k+j)]);
        }
        pTrig += (iStride-1)*20;
    }
    memset( XX, 0, sizeof(LpSpecType)*PP2*2 );      // all the imaginary parts must be zeroed
    for( k = 0, pX = &X[0], pXX = &XX[0]; 
    k<LL; 
    k++, pXX += 2 )
    {
        *pXX = *pX++;
    }
    prvFFT4DCT( XX, LOGPP+1, FFT_FORWARD );
    F[0] = SQUARE( XX[0] );         // XX[1] == 0
    for( k=1, pXX = &XX[2], pF = &F[iQ]; 
    k<PP; 
    k++, pXX += 2, pF += iQ )
    {
        *pF = SQUARE( pXX[0] ) + SQUARE( pXX[1] );
        LPC_FT_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,iQ*k,XX[2*k],XX[2*k+1],F[iQ*k]);
    }
    
    // the remainder should be identical to the other versions of this routine
    
    rgwtLpcSpec  = INTEGER_OR_INT_FLOAT( ppcinfo->m_rgiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    iLoopLimit   = pau->m_cSubband;
    
    for (iFreq = 0, iF = 0; iFreq < iLoopLimit; iFreq++, iF += iStride)     
    {
        LPC_COMPARE_DEBUG( pau, iFreq, iFreq, iF, F, rgLpcCoef );
        LPC_SPEC_PRINT(pau->m_iFrameNumber,pau->m_iCurrSubFrame,iFreq,F[iFreq],InverseQuadRoot(F[iFreq]));
        
        rgwtLpcSpec [iFreq] = wtTemp = InverseQuadRoot(F[iFreq]);
        
        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
        
        INTEGER_ONLY( assert( wtTemp>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,wtTemp);                                                            
        MONITOR_RANGE(gMR_fltLPC_F3,F[iFreq]);
    }
    
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


#endif  // defined(PLATFORM_LPC_LITDFT) && !defined(V4V5_COMPARE_MODE)


#if defined(PLATFORM_LPC_FOLDED) && !defined(V4V5_COMPARE_MODE)

// ************************* Reverse Engineered ARM's LPC2Spec.s **************************

#pragma COMPILER_MESSAGE(__FILE__ "(316) : Warning - building PLATFORM_LPC_FOLDED LPC spectrum - undebugged!!!")

// ************************************************************************************
//
// ARM's LPC2Spec.s translated into c
//
// Features no large stack array.
//
// Unlike ARM's asm code, there is no bit-reversal needed so indexing is easier and less 
// obscure dependencies on having a free shift in every instruction.
//
// ************************************************************************************

#ifndef CMPX_MULT_BP2
    INLINE void LpComplexTwiddle( LpSpecType* pa, LpSpecType* pb, LpSpecType x, LpSpecType y, BP2Type cr, BP2Type ci )
    {
        *pa = MULT_BP2(x,cr) - MULT_BP2(y,ci);
        *pb = MULT_BP2(x,ci) + MULT_BP2(y,cr);
    }
#   define CMPX_MULT_BP2(pa,pb,x,y,cr,ci) LpComplexTwiddle(pa,pb,x,y,cr+ci,ci)
#endif

// helper function - compute 4 WeightFactors from 4 pairs of input data and one set of 6 LPC constants
// called three times:
//   prvPowerCalcZero( 0x10, 0x1C, iSizeBy4, pWF, 6*510, Tmp );
//   prvPowerCalcZero( 0x08, 0x0C, iSizeBy4, pWF,     0, Tmp );
//   prvPowerCalcZero( 0x18, 0x14, iSizeBy4, pWF, 6*512, Tmp );

static void prvPowerCalcZero( int i, int j, int iOff, LpSpecType* pWF, int idxLpc, LpSpecType* pTmp )
{
    LpSpecType i1, i2, i3, i4, i5, i6, x, y, t1x, i5b, r3;
    const BP2Type* pLpc4 = lpc_cnst4+idxLpc;

    // {i3,i4} = {x,y}++ * {Lpc[3],Lpc[2]} 
    x = *pTmp++;  y = *pTmp++;                                      // T0 T1
    CMPX_MULT_BP2( &i4, &i3, x, y, pLpc4[3], pLpc4[2] );            // {t0,t1} = {T0,T1} * {W3,W2}

    // {i6,i5} = {x,y}++ * {Lpc[5],Lpc[4]}
    x = *pTmp++;  y = *pTmp++;                                      // T2 t3
    CMPX_MULT_BP2( &i6, &i5, x, y, pLpc4[5], pLpc4[4] );            // {t2,t3} = {T2,T3} * {W5,W4}

    // {i2,i1} = {x,y}++ * {Lpc[1],Lpc[0]}
    x = *pTmp++;  y = *pTmp++;                                      // T4 T5
    CMPX_MULT_BP2( &i2, &i1, x, y, pLpc4[1], pLpc4[0] );            // {t4,t5} = {T4,T5} * {W1,W0}

    // trivial Complex Multiply
    x = *pTmp++;  y = *pTmp++;                                      // T6 T7 
    x = x - y;                                                      // T6 - T7
    y = x + MUL2(y);     // original x + y                          // T6 + T7

    i5 += i3 + i1;                                                  // t3 + t1 + t5

    i1 = i5 - MUL2(i1 + i3) - i4 + x;                               // t3 - t1 - t5 - t0 + T6 - T7

    i5 += y;                                                        // t3 + t1 + t5 + T6 + T7
    i3 -= y;                                                        // t1 - T6 - T7

    t1x = i3 + MUL2(y);                                             // t1 + T6 + T7
    i5b = MUL2(t1x) - i5;                                           // t1 + T6 + T7 - t3 - t5

    r3 = i2 - i6 - i3;                                              // t4 - t2 - t1 + T6 + T7
    i3 = i6 - i3 - i2;                                              // t2 - t1 + T6 + T7 - t4

    t1x = i4 + i6 + i2 + x;                                         // t0 + t2 + t4 + T6 - T7

    i6 = i6 + i2 - i4 - x;                                          // t2 + t4 - t0 - T6 + T7;

    i4 = i1 + MUL2(i4) - MUL2(x);                                   // t3 - t5 + t0 - T6 + T7

    i5 = DIV2(i5);  t1x = DIV2(t1x);
    pWF[i] = InverseQuadRootOfSumSquares( i5, t1x );                // f( t3 + t1 + t5 + T6 + T7, 
                                                                    //    t0 + t2 + t4 + T6 - T7)
    
    i1 = DIV2(i1);  r3 = DIV2(r3);
    pWF[i^iOff] = InverseQuadRootOfSumSquares( i1, r3 );            // f( t3 - t1 - t5 - t0 + T6 - T7, 
                                                                    //    t4 - t2 - t1 + T6 + T7)

    i3 = DIV2(i3);  i4 = DIV4(i4);
    pWF[j] = InverseQuadRootOfSumSquares( i3, i4 );                 // f( t2 - t1 + T6 + T7 - t4, 
                                                                    //    t3 - t5 + t0 - T6 + T7)

    i5b = DIV2(i5b);  i6 = DIV2(i6);
    pWF[j^iOff] = InverseQuadRootOfSumSquares( i5b, i6 )  ;         // f( t1 + T6 + T7 - t3 - t5, 
                                                                    //    t2 - t0 + t4 - T6 + T7 )
}

// ********************************************************************************************
// This is the inner loop of the Lpc Spectrum calculation
// m_cSubband     2048    1024     512      256       128
// # times called  127      63      31       15         7
// 
static void prvLpcCalc4( const Int idx, const Int iSize, 
                         const BP2Type* pBP2Lpc4, const LpSpecType* pTmp, LpSpecType *pWF )
{
    LpSpecType i1, i2, i3, i4, i5, i6, x, y, I, i3b, i2b, i5b, t0x;
    Int jdx, kdx;
    Int iSizeBy2 = iSize>>1;
    Int iSizeBy4 = iSize>>2;
    Int iSizeBy8 = iSize>>3;

#define LPC_BREAK_IDX 8
#if defined(_DEBUG) && defined(LPC_BREAK_IDX)
        if ( idx == LPC_BREAK_IDX )
            DEBUG_BREAK();
#endif

    for( jdx =0;  jdx < 4; jdx++, pBP2Lpc4 += 6 ) {

        // {i3,i4} = ({x,y}++ * {Lpc4[3],Lpc4[2]} )*4
        x = *pTmp++;  y = *pTmp++;                                  // T0 T1
        CMPX_MULT_BP2( &i4, &i3, x, y, pBP2Lpc4[3], pBP2Lpc4[2] );  // {t0,t1} = {T0,T1} * {W3,W2}

        // {i6,i5} = {x,y}++ * {Lpc4[5],Lpc4[4]}
        x = *pTmp++;  y = *pTmp++;                                  // T2 t3
        CMPX_MULT_BP2( &i6, &i5, x, y, pBP2Lpc4[5], pBP2Lpc4[4] );  // {t2,t3} = {T2,T3} * {W5,W4}

        // {i2,i1} = {x,y}++ * {Lpc4[1],Lpc4[0]}
        x = *pTmp++;  y = *pTmp++;                                  // T4 T5
        CMPX_MULT_BP2( &i2, &i1, x, y, pBP2Lpc4[1], pBP2Lpc4[0] );  // {t4,t5} = {T4,T5} * {W1,W0}

        x = *pTmp++;  y = *pTmp++;                                  // T6 T7 

        t0x = (x-y);                                                // T6 - T7
        y = x + y + LP_SPEC_FROM_FLOAT(2.0f);                            // T6 + T7 + 2

        I  = -i4 + i5 - i1 + t0x;                                   // -t0 + t3 - t5 + T6 - T7
        i5 =  i3 + i5 + i1 + y;                                     // t1 + t3 + t5 + T6 + T7 + 2
        i3b =  i3 + y;                                              // t1 + T6 + T7 + 2
        i3 -= y;                                                    // t1 - T6 - T7 - 2

        i5b = MUL2(i3b) - i5;                                       // t1 + T6 + T7 + 2 - t3 - t5
        i3b = i4 + i6 + i2 + t0x;                                   // t0 + t2 + t4 + T6 - T7

        switch (jdx) {
        case 0:  kdx = idx; break;
        case 1:  kdx = iSizeBy8 - idx; break;
        case 2:  kdx = iSizeBy8 + idx; break;
        default: kdx = iSizeBy4 - idx;
        }
        // example:  iSize = 0x100 = 256.
        //            idx==1         | idx==2          | idx==3
        //       jdx:  0   1   2   3 |   0   1   2   3 |  0
        //    kdx      1  1f  21  3f |   2  1e  22  2e |  3 ...
        // 80+kdx     81  9f  a1  bf |  82  9e  a2  be | 83 ...
        // 80-kdx     7f  61  5f  41 |  7e  62  5e  42 | 7e ...
        //100-kdx     ff  e1  df  c1 |  fe  e2  de  c2 | fd ...

        i5 = DIV2(i5);  i3b = DIV2(i3b);
        pWF[kdx] = InverseQuadRootOfSumSquares( i5, i3b );          // (t1 + t3 + t5 + T6 + T7 + 2, t0 + t2 + t4 + T6 - T7)

        i2b = i6 - i2;                                              // t2 - t4
        i6  = MUL4(i6 + i2) - i4 - t0x;                             // t2 + t4 - t0 - T6 - T7
        i4  = MUL2(i4) + I - MUL2(t0x);                             // t0 + t3 - t5 - T6 + T7
        y   = i2b + i3;                                             // t2 - t4 + t1 - T6 - T7 - 2

        I = DIV2(I);  y = DIV2(I);
        pWF[iSizeBy2+kdx] = InverseQuadRootOfSumSquares( I, y );    // (-t0 + t3 - t5 + T6 - T7, t2 - t4 + t1 - T6 - T7 - 2)

        y = i2b - i3;                                               // t2 - t4 - t1 + T6 + T7 + 2

        i4 = DIV2(I);  y = DIV2(y);
        pWF[iSizeBy2-kdx] = InverseQuadRootOfSumSquares( i4, y );   // (t0 + t3 - t5 - T6 + T7, t2 - t4 + t1 - T6 - T7 - 2)

        i5b = DIV2(i5b);  i6 = DIV2(i6);
        pWF[iSize-kdx] = InverseQuadRootOfSumSquares( i5b, i6 );    // (t1 + T6 + T7 + 2 - t3 - t5, t2 + t4 - t0 - T6 - T7)
    }
}


WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo) {
    int iStride, iLoops, idx, i;
    int iStridem1x24, iMainLpc4Offset, iMainLpc4Stride, iMainLpc4Delta;
    LpSpecType Tmp[32];
    const BP2Type* pbp2Lpc3;
    const BP2Type* pBP2Lpc4;
    LpSpecType *pWF = (LpSpecType *)pau->m_rguiWeightFactor;  
    LpType LPC[LPCORDER];
    LpSpecType jv0, jv1, jv2, jv3, jv4, jv5, jv6, jv7, jv8, jv9, jv10, jv11, jv12;
#if 1
    LpSpecType iv0, iv1, iv2, iv3, iv4, iv5, iv6, iv7, iv8, iv9;
    LpSpecType k0, k1, k2, k3, k4, k5, k6, k8, k9, k10, kx;
#else
    // these register sets do not get used at the same time, so save a little stack space
#   define iv0 jv0
#   define iv1 jv1
#   define iv2 jv2
#   define iv3 jv3
#   define iv4 jv4
#   define iv5 jv5
#   define iv6 jv6
#   define iv7 jv7
#   define iv8 jv8
#   define iv9 jv9
#   define t0  jv10
#   define k0 jv0
#   define k1 jv1
#   define k2 jv2
#   define k3 jv3
#   define k4 jv4
#   define k5 jv5
#   define k6 jv6
#   define k8 jv8
#   define k9 jv9
#   define k10 jv10
#   define kx jv11
#endif
    LpSpecType wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    int iFreq, iF;

#ifdef REPLICATE_V4_LPC
    const Int iSize = pau->m_cFrameSampleHalf;
#else
    const Int iSize = (pau->m_fV5Lpc) ? pau->m_cSubband : pau->m_cFrameSampleHalf;
#endif
    const Int iSizeBy2 = iSize>>1;
    const Int iSizeBy4 = iSize>>2;
    const Int iSizeBy16 = iSize>>4;        

    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);

    // use this to break at a particular frame
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    assert( 10 == LPCORDER );

    for (i = 0; i < LPCORDER; i++) 
        LPC[i] = LP_SPEC_FROM_LP(rgLpcCoef[i]); 

    // m_cSubband   2048    1024     512      256       128
    // iStride         1       2       4        8        16
    // iLoopCntr     127     126     124      120
    // Loops         127      63      31       15         7
    iStride  = MAX_LP_SPEC_SIZE/iSize;

    // step constant array in main loop by stride * 4 inner_loops * (6 == 1 row per loop)
    iStridem1x24 = (iStride-1)*(4*6);    
    iLoops   = (iSize>>4) - 1;
    
    iMainLpc4Offset = 0;
    iMainLpc4Stride = iStride<<4;
    iMainLpc4Delta = 3;

    // Handle four sets of four points in each main loop
    // idx          iSize/2+idx     iSize/2-idx     iSize-idx
    // iSize/8-idx  5*iSize/8-idx   3*iSize/8+idx   7*iSize/8+idx
    // iSize/8+idx  5*iSize/8+idx   3*iSize/8-idx   7*iSize/8-idx
    // iSize/4-idx  3*iSize/4-idx   iSize/4+idx     3*iSize/4+idx

    for( idx = 1, pBP2Lpc4 = lpc_cnst4 + iStride*4*6 - 3*6; idx <= iLoops; idx++, pBP2Lpc4 += iStridem1x24 ) {

        // offsets into the Lpc constants array (numbers in hex)
        //
        //  512:  3D 7D BD FD 13D 17D 1BD 1FE 1BE 17E 13E FE BE 7E 3E 0 3F 7F BF FF 13F 17F 1BF 200 1B0 180 140 100 B0 80 40
        //  256:     7D    FD     17D     1FE     17E     FE    7E    0    7F    FF     17F     200     180     100    80
        //  128:           FD             1FE             FE          0          FF             200             100
        // 
        // show last two rows as an expression to see the component behavior
        //  256:  80-3, 100-3, 180-3, 200-2, 180-2, 100-2, 80-2, 0, 80-1, 100-1, 180-1, 200, 180, 100, 80
        //  128:        100-3,        200-2,        100-2,       0,       100-1,        200,      100

        iMainLpc4Offset += iMainLpc4Stride;
        if ( iMainLpc4Offset & 0x1FF ) {
            pbp2Lpc3 = lpc_cnst4 + (iMainLpc4Offset-iMainLpc4Delta)*6;
        } else {
            // iMainLpc4Offset is 0 or 512.  time to turn around
            iMainLpc4Stride = -iMainLpc4Stride;
            iMainLpc4Delta--;
            assert(iMainLpc4Delta>=0);
            pbp2Lpc3 = lpc_cnst4 + (iMainLpc4Offset>0 ? iMainLpc4Offset-iMainLpc4Delta*6 : 0);
        }

        // Comments below refer to LPC[i] as Li and BP2Lpc3[j] as wj

        iv0 = pbp2Lpc3[2];  iv1 = pbp2Lpc3[3];  iv2 = pbp2Lpc3[4];  iv3 = pbp2Lpc3[5];  // w2 w3 w4 w5
        iv1 += iv0;                             // W3 = w2 + w3
        iv2 = -iv2;                             // W4 = -w4
        iv3 = iv2 - iv3;                        // W5 = -w4 -w5

        iv8 = MULT_BP2(iv0,LPC[7]);             // w2*L7
        iv9 = MULT_BP2(iv1,LPC[7]);             // W3*L7
        iv0 = MULT_BP2(iv3,LPC[3]);             // W5*L3
        iv1 = MULT_BP2(iv2,LPC[3]);             // W4*L3
        iv2 = MULT_BP2(iv2,LPC[6]);             // W4*L6
        iv6 = MULT_BP2(iv3,LPC[6]);             // W5*L6

	    // Tmp[4:7]   = { W5*L6 - L2,  W4*L6 - L2, W5*L3 - W3*L7, W4*L3 - w2*L7 }
        // Tmp[12:15] = { W5*L6 - L2, -W4*L6 - L2, w2*L7 + W5*L3, W3*L7 - W4*L3 }
        // Tmp[20:23] = {  W4*L6 - L2, -W5*L6 - L2, W3*L7 + W4*L3, w2*L7 - W5*L3 }
        // Tmp[28:31] = { -W4*L6 - L2, -W5*L6 - L2, -w2*L7 - W4*L3, -W3*L7 - W5*L3 }

        Tmp[4] =   Tmp[12] = iv6 - LPC[2];      // W5*L6 - L2
        Tmp[5] =   Tmp[20] = iv2 - LPC[2];      // W4*L6 - L2
        Tmp[6] =  iv0 - iv9;                    // W5*L3 - W3*L7
        Tmp[7] =  iv1 - iv8;                    // W4*L3 - w2*L7
        Tmp[14] = iv8 + iv0;                    // w2*L7 + W5*L3
        Tmp[15] = iv9 - iv1;                    // W3*L7 - W4*L3

        Tmp[28] =   Tmp[13] = -LPC[2] - iv2;    // -W4*L6 - L2
        Tmp[29] =   Tmp[21] = -LPC[2] - iv6;    // -W5*L6 - L2
        Tmp[22] = iv9 + iv1;                    //  W3*L7 + W4*L3
        Tmp[23] = iv8 - iv0;                    //  w2*L7 - W5*L3
        Tmp[30] = -iv8 - iv1;                   // -w2*L7 - W4*L3
        Tmp[31] = -iv9 - iv0;                   // -W3*L7 - W5*L3

        // -----------------------------------------------------------

        // reload same lpc constants
        jv0 = pbp2Lpc3[2];  jv1 = pbp2Lpc3[3];  jv2 = pbp2Lpc3[4];  jv3 = pbp2Lpc3[5];  // w2 w3 w4 w5
        jv1 += jv0;                             // W3 = w2+w3
        jv3 += jv2;                             // W5 = w4+w5

        jv5  = MULT_BP2(jv1,LPC[9]);            // W3*L9
        jv11 = MULT_BP2(jv3,LPC[5]);            // W5*L5
        jv12 = MULT_BP2(jv0,LPC[9]);            // w2*L9
        jv10 = MULT_BP2(jv2,LPC[5]);            // w4*L5

        jv8  = MULT_BP2(jv1,LPC[8]);            // W3*L8
        jv1  = MULT_BP2(jv3,LPC[4]);            // W5*L4
        jv7  = MULT_BP2(jv0,LPC[8]);            // w2*L8
        jv2  = MULT_BP2(jv2,LPC[4]);            // w4*L4

        jv6  = jv7 - LPC[0] - jv1;              // w2*L8 - L0 - W5*L4
        jv9  = jv8 - LPC[0] + jv2;              // W3*L8 - L0 + w4*L4
        jv7 += LPC[0];                          // w2*L8 + L0
        jv8 += LPC[0];                          // W3*L8 + L0

        jv0   = LPC[1] - jv5;                   // L1 - W3*L9

	    // Tmp[8:11] = { w2*L9 - L1 - W5*L5, w4*L5 - L1 + W3*L9, w2*L8 - L0 - W5*L4, W3*L8 - L0 + w4*L4 }
        Tmp[8]  = jv12 - LPC[1] - jv11;         // w2*L9 - L1 - W5*L5
        Tmp[9]  = jv10 - jv0;                   // W3*L9 - L1 + w4*L5
        Tmp[10] = jv6;                          // w2*L8 - L0 - W5*L4
        Tmp[11] = jv9;                          // W3*L8 - L0 + w4*L4

        // Tmp[16:19] = {-w4*L5 - L1 + W3*L9, w2*L9 - L1 + W5*L5, W3*L8 - L0 - w4*L4, w2*L8 - L0 + W5*L4 }
        Tmp[16] = -jv10 - jv0;                  // W3*L9 - L1 - w4*L5
        Tmp[17] = jv12 - LPC[1] + jv11;         // w2*L9 - L1 + W5*L5
        Tmp[18] = jv9 - MUL2(jv2);              // W3*L8 - L0 - w4*L4
        Tmp[19] = jv6 + MUL2(jv1);              // w2*L8 - L0 + W5*L4

        // Tmp[0:3] = {-L1 - W3*L9 - W5*L5, -w2*L9 - L1 - w4*L5, -W3*L8 - L0 - W5*L4, -w2*L8 - L0 - w4*L4}
        Tmp[0]  = -jv5  - LPC[1] - jv11;        //-W3*L9 - L1  -W5*L5
        Tmp[1]  = -jv12 - LPC[1] - jv10;        //-w2*L9 - L1 - w4*L5
        Tmp[2]  = -( jv8 + jv1 );               //-W3*L8 - L0 - W5*L4
        Tmp[3]  = -( jv7 + jv2 );               //-w2*L8 - L0 - w4*L4

        // Tmp[24:27] = {-w2*L9 - L1 + w4*L5, -L1 - W3*L9 + W5*L5, -w2*L8 - L0 + w4*L4, -W3*L8 - L0 + W5*L4}
        Tmp[24] = -jv12 - LPC[1] + jv10;        //-w2*L9 - L1 + w4*L5
        Tmp[25] = -jv5  - LPC[1] + jv11;        //-W3*L9 - L1 + W5*L5
        Tmp[26] = -jv7  + jv2;                  //-w2*L8 - L0 + w4*L4
        Tmp[27] = -jv8  + jv1;                  //-W3*L8 - L0 + W5*L4

        // loop_fourvalues
        prvLpcCalc4( idx, iSize, pBP2Lpc4, Tmp, pWF );
    }

    // Final Batch of 16 points
    Tmp[1] = Tmp[16] = k1 = LPC[9] - LPC[1];        // L9 - L1
    Tmp[7] = Tmp[22] = kx = LPC[7] + LP_SPEC_FROM_FLOAT(1.0); // 1 + L7
    Tmp[8] = k1 - LPC[5];                           // L9 - L1 - L5
    Tmp[9] = k1 + LPC[5];                           // L9 - L1 + L5

    Tmp[14] = kx - LPC[3];                          // 1 + L7 - L3
    Tmp[15] = kx + LPC[3];                          // 1 + L7 + L3

    Tmp[3] = Tmp[18] = k8 = -LPC[0] + LPC[8];       // -L0 + L8

    Tmp[10] = k8 - LPC[4];                          // -L0 + L8 - L4
    Tmp[11] = k8 + LPC[4];                          // -L0 + L8 + L4

    kx = MULT_BP2( SQRT2, LPC[5] );                 // 1.414 * L5
    k9  = LPC[1] + LPC[9];
    Tmp[17] = -k9 + kx;                             // -L9 - L1 + 1.414*L5
    Tmp[0] = -k9 -kx;                               // -L1 - L9 - 1.414*L5
    k3 = k9 - LPC[5];                               //  L9 + L1 - L5 

    k10 = MULT_BP2( LPC[3], SQRT2 );                // 1.414*L3

    k1  = LP_SPEC_FROM_FLOAT(1.0) - LPC[7];              // 1 - L7
    Tmp[6] = k1 - k10;                              // 1 - L7 - 1.414*L3
    Tmp[23] = k10 + k1;                             // 1 - L7 + 1.414*L3

    k8 = LPC[0] + LPC[8];                           // L0 + L8
    kx =  MULT_BP2( LPC[4], SQRT2 );                // 1.414*L4
    Tmp[19] = -k8 + kx;                             // -L8 - L0 + 1.414*L4
    Tmp[2]  = -k8 - kx;                             // -L8 - L0 - 1.414*L4

    k1 -= LPC[3];                                   // 1 - L7 - L3
    k5 = k1 + k9 + LPC[5];                          // 1 - L7 - L3 + L9 + L1 + L5

    kx = MULT_BP2( SQRT2, LPC[6] );                 //  1.414*L6
    Tmp[21] = kx - LPC[2];                          //  1.414*L6 - L2
    Tmp[4]  = -kx - LPC[2];                         // -1.414*L6 - L2
    Tmp[5]  = Tmp[20] = -LPC[2];                    // -L2

    Tmp[12] = kx = -LPC[2] - LPC[6];                // -L2 - L6
    Tmp[13] = k6 = -LPC[2] + LPC[6];                // -L2 + L6
    k4 = LPC[4] - k8 - k6;                          // L4 - L8 - L0 + L2 - L6
    k0 = LPC[4] + k8 + kx;                          // L4 + L8 + L0 - L2 - L6

    kx = MULT_BP2( SQRT2_2, k4 );                   // 0.707*(L4 - L8 - L0 + L2 - L6)
    k1 = k1 + MUL2(LPC[3]);                         // 1 - L7 + L3
    k2 = k1 + kx;                                   // 1 - L7 + L3 + 0.707*(L4 - L8 - L0 + L2 - L6)
    k1 = k1 - kx;                                   // 1 - L7 + L3 - 0.707*(L4 - L8 - L0 + L2 - L6)

    k4 += MUL2(k6);                                 // L4 - L8 - L0 - L2 + L6
    k4 = MULT_BP2( SQRT2_2, k4 );                   // 0.707*(L4 - L8 - L0 - L2 + L6)

    k6 = k3 + k4;                                   // L9 + L1 - L5 + 0.707*(L4 - L8 - L0 - L2 + L6)
    k4 = k3 - k4;                                   // L9 + L1 - L5 - 0.707*(L4 - L8 - L0 - L2 + L6)

    kx = LP_SPEC_FROM_FLOAT(1.0);
    for( i=0; i<10; i++ ) 
        kx -= LPC[i];                               // 1 - L0 - L1 ... - L8 - L9

    // pWF[0] = invQuadRoot((1 - L7 - L3 - L9 - L1 - L5 - L8 - L0 - L4 - L2 - L6)^2)
    pWF[0] = InverseQuadRootOfSumSquares( kx, 0 );                          // wf[   0]

	// wf[ 64] = InvQuadRoot( ( 1 - L7 + L3 + 0.707 * (L4 - L8 - L0 + L2 - L6))^2 
    //                       +(L9 + L1 - L5 - 0.707 * (L4 - L8 - L0 - L2 + L6))^2 )
    pWF[iSizeBy4] = InverseQuadRootOfSumSquares( k2, k4 );                  // wf[0x40]

	// wf[128] = InvQuadRoot( ( 1 - L7 - L3 + L9 + L1 + L5)^2 
    //                       +(L4 + L8 + L0 - L2 - L6)^2 )
    pWF[iSizeBy2] = InverseQuadRootOfSumSquares( k5, k0 );                  // wf[0x80]

	// wf[192] = InvQuadRoot( ( 1 - L7 + L3 - 0.707 * (L4 - L8 - L0 + L2 - L6))^2
	//                       +(L9 + L1 - L5 + 0.707 * (L4 - L8 - L0 - L2 + L6))^2 )
    pWF[iSizeBy2 + iSizeBy4] = InverseQuadRootOfSumSquares( k1, k6 );       // wf[0xc0]
   

    prvPowerCalcZero(   iSizeBy16, 7*iSizeBy16, iSizeBy2, pWF, 6*510, Tmp );      // WF[10, 90, 70, f0]

    prvPowerCalcZero( 2*iSizeBy16, 6*iSizeBy16, iSizeBy2, pWF,     0, Tmp+8 );    // WF[20, a0, 60, e0]

    Tmp[8] = k10;                                   // 1 - L7 + 1.414*L3
    prvPowerCalcZero( 3*iSizeBy16, 5*iSizeBy16, iSizeBy2, pWF, 6*512, Tmp+16 );   // WF[30, b0, 50, d0]

    // now find maximum weight

    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    
    for (iFreq = 0, iF = 0; iFreq < iSize; iFreq++, iF += iStride) 
    {
        LPC_COMPARE_DEBUG( pau, iFreq, iFreq, iF, pWF, rgLpcCoef );

        if ( pWF[iFreq] > wtLpcSpecMax )
            wtLpcSpecMax = pWF[iFreq];
        
        INTEGER_ONLY( assert( pWF[iFreq]>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,pWF[iFreq]);                                
    }
    
    if (wtLpcSpecMax == WEIGHT_FROM_FLOAT(0.0F))
    {
        FUNCTION_PROFILE_STOP(&fp);
        return TraceResult(WMA_E_FAIL);
    }
    
#if defined(INTEGER_ENCODER)
    ppcinfo->m_iMaxWeight  = wtLpcSpecMax;
#endif
    ppcinfo->m_wtMaxWeight = wtLpcSpecMax;
    
    FUNCTION_PROFILE_STOP(&fp);
    return WMA_OK;
}


#endif  // defined(PLATFORM_LPC_FOLDED) && !defined(V4V5_COMPARE_MODE)



#if defined(PLATFORM_LPC_PRUNED_NONREDUNDANT) && !defined(V4V5_COMPARE_MODE)


//#pragma COMPILER_MESSAGE(__FILE__ "(961) : Warning - building PLATFORM_LPC_PRUNED_NONREDUNDANT LPC spectrum")

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
//
// The use of pointers instead of array indexes speeds up the SH4 and has no effect on the X86
// Question for Wei-ge: shouldn't F be malloced rather than demand 16kb of stack space?
//
// Cache usage: constants 14kb + F 16kb = 30kb (SH4 operand cache is 16kb so cache gets churned!)
//
// ************************************************************************************

WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    // put often used variables near the top for easier access in platforms like the SH3 and SH4
    LpSpecType original[LPCORDER];
    LpSpecType *pFa, *pFb, *pFc, *pFd;      
    Int i, iFStep, iFStep2, iFStep4, iFStep8, iFStep16;
    LpSpecType t1pO7, t1mO7, tO1pO9, tO1mO9, tO0pO8, tO0mO8, tO4pO6, tO4mO6;
    LpSpecType t1pO7pO3, t1pO7mO3, tO1pO5pO9, tO0pO2pO4pO6pO8;
    LpSpecType tS2x; 
    const BP2Type* pC;
    Int pCInc;
    LpSpecType tC1x, tC2x, tC3x, tC4x;
    LpSpecType C1, C2, C3, C4, C5, C6;
    LpSpecType T2, T4, T6, T7, T8, T9, TA, TB;
    LpSpecType D, E, G, I, J;
    
    LpSpecType* rgwtLpcSpec;
    WeightType wtLpcSpecMax, wtTemp;
    Int iFreq, iF, iLoopLimit;
    Int iShrink, iStride; 
    LpSpecType F[2*MAX_LP_SPEC_SIZE];
    // Notes for BUILD_INTEGER: InverseQuadRoot returns UInt, yet is stored in Int array, F, temporarily.
    // These values are then copied to WeightType, which is UInt. There should not be any numeric loss
    // between the two transitions.
    Int iSize;
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);
    
#ifdef REPLICATE_V4_LPC
    iSize = pau->m_cFrameSampleHalf;
#else
    if (pau->m_fV5Lpc)
        iSize = pau->m_cSubband;
    else 
        iSize = pau->m_cFrameSampleHalf;
#endif
    iShrink = LOG2(MAX_LP_SPEC_SIZE/iSize);     // for smaller transforms, shrink or expand indexing
    iStride = MAX_LP_SPEC_SIZE/iSize;           // for smaller transforms, stride past unused (lpc_compare only)
    
    
    
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    
    //for (i = 0; i < LPCORDER; i++) 
    //fprintf(stdout, "%.20lf\n", (double) FLOAT_FROM_LP(rgLpcCoef[i])); 
    
    for (i = 0; i < LPCORDER; i++) 
        original[i] = -LP_SPEC_FROM_LP(rgLpcCoef[i]); 
    
    DEBUG_ONLY( memset( F, 0, 2*MAX_LP_SPEC_SIZE*sizeof(LpSpecType) ) );
    
    iFStep   =  128>>iShrink;
    iFStep2  =  256>>iShrink;
    iFStep4  =  512>>iShrink;
    iFStep8  = 1024>>iShrink;
    iFStep16 = 2048>>iShrink;
    pFa = &F[iFStep];       // F[128]
    *pFa  = (t1pO7 = LP_SPEC_FROM_FLOAT(1) + original[7]) + (tS2x = MULT_BP2X(SQRT2,original[3]));
    pFa  += iFStep;         // F[256]  
    *pFa  = (t1mO7 = LP_SPEC_FROM_FLOAT(1) - original[7]) + original[3];
    pFa  += iFStep;         // F[384]  
    *pFa  = t1mO7;
    pFa  += iFStep2;        // F[640]
    *pFa  = t1pO7 - tS2x;
    pFa  += iFStep;         // F[768]
    *pFa  = t1mO7 - original[3];
    pFa  += iFStep;         // F[896]
    *pFa  = t1mO7;
    
    pFa  += iFStep2;        // F[1152] 
    *pFa  = (tO1pO9 = original[1] + original[9]) + (tS2x = MULT_BP2X(SQRT2,original[5]));
    pFa  += iFStep;         // F[1280] 
    *pFa  = (tO1mO9 = original[1] - original[9]) + original[5];
    pFa  += iFStep;         // F[1408]
    *pFa  = tO1mO9;
    pFa  += iFStep2;        // F[1664] 
    *pFa  = tO1pO9 - tS2x;
    pFa  += iFStep;         // F[1792] 
    *pFa  = tO1mO9 - original[5];
    pFa  += iFStep;         // F[1920] 
    *pFa  = tO1mO9;
    
    pFa  += iFStep2;        // F[2176] 
    *pFa  = (tO0pO8 = original[0] + original[8]) + (tS2x = MULT_BP2X(SQRT2,original[4]));
    pFa  += iFStep;         // F[2304] 
    *pFa  = (tO0mO8 = original[0] - original[8]) + original[4];
    pFa  += iFStep;         // F[2432] 
    *pFa  = tO0mO8;
    pFa  += iFStep2;        // F[2688]
    *pFa  = tO0pO8 - tS2x;
    pFa  += iFStep;         // F[2816] 
    *pFa  = tO0mO8 - original[4];
    pFa  += iFStep;         // F[2944] 
    *pFa  = tO0mO8;
    
    pFa  += iFStep2;        // F[3200] 
    *pFa  = original[2] + (tS2x = MULT_BP2X(SQRT2,original[6]));
    pFa  += iFStep;         // F[3328] 
    *pFa  = original[2] + original[6];
    pFa  += iFStep;         // F[3456] 
    *pFa  = original[2];
    pFa  += iFStep2;        // F[3712] 
    *pFa  = original[2] - tS2x;
    pFa  += iFStep;         // F[3840] 
    *pFa  = original[2] - original[6];
    pFa  += iFStep;         // F[3968]
    *pFa  = original[2];
    
    tO4pO6 = original[4] + original[6];
    tO4mO6 = original[4] - original[6];
    t1pO7pO3 = t1pO7 + original[3];
    t1pO7mO3 = t1pO7 - original[3];
    tO1pO5pO9 = tO1pO9 + original[5];
    tO0pO2pO4pO6pO8 = tO0pO8 + tO4pO6 + original[2];
    pFa   = &F[0];          // F[0]
    *pFa  = t1pO7pO3 + tO1pO5pO9 + tO0pO2pO4pO6pO8;
    pFa  += iFStep4;        // F[512]  
    *pFa  = t1pO7mO3 + (tS2x = MULT_BP2X(SQRT2_2, tO0pO8 - tO4mO6 - original[2]));
    pFa  += iFStep4;        // F[1024] 
    *pFa  = t1pO7pO3 - tO1pO5pO9;
    pFa  += iFStep4;        // F[1536] 
    *pFa  = t1pO7mO3 - tS2x;
    pFa  += iFStep4;        // F[2048] 
    *pFa  = t1pO7pO3 + tO1pO5pO9 - tO0pO2pO4pO6pO8;
    pFa  += iFStep4;        // F[2560] 
    *pFa  = -tO1pO9 + original[5] + (tS2x = MULT_BP2X(SQRT2_2,tO0pO8 - tO4pO6 + original[2]));
    pFa  += iFStep4;        // F[3072] 
    *pFa  =  tO0pO8 + tO4mO6 - original[2];
    pFa  += iFStep4;        // F[3584] 
    *pFa  =  tO1pO9 - original[5] + tS2x;
    
    // a few get squared.
    pFb   =  &F[0];      // F[0]
    *pFb  =  InverseQuadRootOfSumSquares(*pFb, 0);
    pFb  +=  iFStep4;   // b: F[512]  a: F[3584]
    *pFb  =  InverseQuadRootOfSumSquares(*pFb, *pFa);
    pFb  +=  iFStep4;    // b: F[1024]
    pFa  -=  iFStep4;    // a: F[3072]
    *pFb  =  InverseQuadRootOfSumSquares(*pFb, *pFa);
    pFb  +=  iFStep4;    // b: F[1536]
    pFa  -=  iFStep4;    // a: F[2560]
    *pFb  =  InverseQuadRootOfSumSquares(*pFb, *pFa);
    pFb  +=  iFStep4;    // b: F[2048]
    *pFb  =  InverseQuadRootOfSumSquares(*pFb, 0);
    
    //** up to here we have 46 adds and 14 mults
    
    
#if defined(_DEBUG) && defined(WMA_MONITOR)
    for(i=0; i<2*MAX_LP_SPEC_SIZE; i += 128)
        MONITOR_RANGE(gMR_fltLPC_F1,F[i]);
#endif
    
    pC = &lpc_cnst3[1*(4<<iShrink)];
    pCInc = (4<<iShrink)-3;  // 3 of them get done with ++
    for (i=1; i<iFStep; i++, pC += pCInc )
    {
        C1 = *pC++;  // cnst3[i*(4<<iShink)];
        C2 = *pC++;  // cnst3[i*(4<<iShinrk)+1];
        C3 = *pC++;  // cnst3[i*(4<<iShrink)+2];
        C4 = *pC;    // cnst3[i*(4<<iShrink)+3];
        assert( BP2_FROM_FLOAT(1) <= C1 && C1 <= BP2_FROM_FLOAT(1.5) );
        
        pFa   = &F[i];          // F[j]
        *pFa  = LP_SPEC_FROM_FLOAT(1) + (tC1x = MULT_BP2X(C1,original[7])) + (tC2x = MULT_BP2X(C2,original[3]));
        pFb   = &F[iFStep2-i];  // F[256-j]
        *pFb  = LP_SPEC_FROM_FLOAT(1) + (tC3x = MULT_BP2X(C3,original[7])) + tC2x;
        pFa  += iFStep2;        // F[256+j]  
        *pFa  = LP_SPEC_FROM_FLOAT(1) - tC1x - (tC4x = MULT_BP2X(C4,original[3]));
        pFb  += iFStep2;        // F[512-j]  
        *pFb  = LP_SPEC_FROM_FLOAT(1) - tC3x + tC4x;
        pFa  += iFStep2;        // F[512+j]  
        *pFa  = LP_SPEC_FROM_FLOAT(1) + tC1x - tC2x;
        pFb  += iFStep2;        // F[768-j]  
        *pFb  = LP_SPEC_FROM_FLOAT(1) + tC3x - tC2x;
        pFa  += iFStep2;        // F[768+j]  
        *pFa  = LP_SPEC_FROM_FLOAT(1) - tC1x + tC4x;
        pFb  += iFStep2;        // F[1024-j] 
        *pFb  = LP_SPEC_FROM_FLOAT(1) - tC3x - tC4x;
        
        pFa  += iFStep2;        // F[1024+j]
        *pFa  = original[1] + (tC1x = MULT_BP2X(C1,original[9])) + (tC2x = MULT_BP2X(C2,original[5]));
        pFb  += iFStep2;        // F[1280-j]  
        *pFb  = original[1] + (tC3x = MULT_BP2X(C3,original[9])) + tC2x;
        pFa  += iFStep2;        // F[1280+j]  
        *pFa  = original[1] - tC1x - (tC4x = MULT_BP2X(C4,original[5]));
        pFb  += iFStep2;        // F[1536-j]  
        *pFb  = original[1] - tC3x + tC4x;
        pFa  += iFStep2;        // F[1536+j]  
        *pFa  = original[1] + tC1x - tC2x;
        pFb  += iFStep2;        // F[1792-j]  
        *pFb  = original[1] + tC3x - tC2x;
        pFa  += iFStep2;        // F[1792+j]  
        *pFa  = original[1] - tC1x + tC4x;
        pFb  += iFStep2;        // F[2048-j]  
        *pFb  = original[1] - tC3x - tC4x;
        
        pFa  += iFStep2;        // F[2048+j]  
        *pFa  = original[0] + (tC1x = MULT_BP2X(C1,original[8])) + (tC2x = MULT_BP2X(C2,original[4]));
        pFb  += iFStep2;        // F[2304-j]  
        *pFb  = original[0] + (tC3x = MULT_BP2X(C3,original[8])) + tC2x;
        pFa  += iFStep2;        // F[2304+j]  
        *pFa  = original[0] - tC1x - (tC4x = MULT_BP2X(C4,original[4]));
        pFb  += iFStep2;        // F[2560-j]  
        *pFb  = original[0] - tC3x + tC4x;
        pFa  += iFStep2;        // F[2560+j]  
        *pFa  = original[0] + tC1x - tC2x;
        pFb  += iFStep2;        // F[2816-j]  
        *pFb  = original[0] + tC3x - tC2x;
        pFa  += iFStep2;        // F[2816+j]  
        *pFa  = original[0] - tC1x + tC4x;
        pFb  += iFStep2;        // F[3072-j]  
        *pFb  = original[0] - tC3x - tC4x;
        
        pFa  += iFStep2;        // F[3072+j] 
        pFb  += iFStep2;        // F[3328-j]
        *pFa  =  *pFb  = original[2] + (tC2x = MULT_BP2X(C2,original[6]));
        pFa  += iFStep2;        // F[3328+j] 
        *pFa  = original[2] - (tC4x = MULT_BP2X(C4,original[6]));
        pFb  += iFStep2;        // F[3584-j]
        *pFb  = original[2] + tC4x;
        pFa  += iFStep2;        // F[3584+j] 
        *pFa  = original[2] - tC2x;
        pFb  += iFStep2;        // F[3840-j]
        *pFb  = original[2] - tC2x;
        pFa  += iFStep2;        // F[3840+j] 
        *pFa  = original[2] + tC4x;
        pFb  += iFStep2;        // F[4096-j]
        *pFb  = original[2] - tC4x;
        
        //** this block uses 52 adds and 14 mults
    }
    //* This loop uses (128>>iShrink) * (52 adds and 14 mults)
    
#if defined(_DEBUG) && defined(WMA_MONITOR)
    for(i=0; i<2*MAX_LP_SPEC_SIZE; i += 1)
        MONITOR_RANGE(gMR_fltLPC_F2,F[i]);
#endif
    
    pC = &lpc_cnst4[1*(6<<iShrink)];
    pCInc = (6<<iShrink)-5;  // 5 of them get done with ++
    for (i=1; i<iFStep4; i++, pC += pCInc ){
        C1 = *pC++;   // cnst4[i*6];
        C2 = *pC++;   // cnst4[i*6+1];
        C3 = *pC++;   // cnst4[i*6+2];
        C4 = *pC++;   // cnst4[i*6+3];
        C5 = *pC++;   // cnst4[i*6+4];
        C6 = *pC;     // cnst4[i*6+5];
        assert( BP2_FROM_FLOAT(0) < C2 && C2 < BP2_FROM_FLOAT(1.5) );
        
        pFc = &F[iFStep8+i];        pFd = &F[iFStep16-i];       
        T7 = MULT_BP2X(C3,*pFc) + MULT_BP2X(C4,*pFd);           // F[1024+j]    F[2048-j]
        T4 = MULT_BP2X(C4,*pFc) - MULT_BP2X(C3,*pFd);           // F[1024+j]    F[2048-j]
        pFc += iFStep8;         pFd += iFStep8;
        T8 = MULT_BP2X(C5,*pFc) + MULT_BP2X(C6,*pFd);           // F[2048+j]    F[3072-j]
        pFc += iFStep8;         pFd += iFStep8;
        T9 = MULT_BP2X(C1,*pFc) + MULT_BP2X(C2,*pFd);           // F[3072+j]    F[4096-j]
        
        pFa = &F[i];            pFb = &F[iFStep8-i];
        TA = *pFa + *pFb;                           // F[j] + F[1024-j];
        TB = *pFa - *pFb;                           // F[j] - F[1024-j];
        
        D   = DIV2(+ T7  + T8 + T9 + TA);
        G   = DIV2(+ T4  + T8 - T9 - TB);
        I   = DIV2(- T4  + T8 - T9 + TB);
        J   = DIV2(+ T7  - T8 - T9 + TA);
        
        pFc -= iFStep8;         
        pFd -= iFStep8;
        T6 = MULT_BP2X(C6,*pFc) - MULT_BP2X(C5,*pFd);           // F[2048+j]    F[3072-j]
        pFc += iFStep8;         
        pFd += iFStep8;
        T2 = MULT_BP2X(C2,*pFc) - MULT_BP2X(C1,*pFd);           // F[3072+j]    F[4096-j]
        
        E     = DIV2(+ T4  + T6 + T2 + TB);
        *pFa  = InverseQuadRootOfSumSquares(D, E);           // F[j]
        
        E     = DIV2(- T7  + T6 - T2 + TA);
        *pFb  = InverseQuadRootOfSumSquares(E, G);           // F[1024-j]
        
        E     = DIV2(- T7  - T6 + T2 + TA);
        pFa  += iFStep8;
        *pFa  = InverseQuadRootOfSumSquares(E, I);               // F[1024+j]
        
        E     = DIV2(- T4  + T6 + T2 - TB);
        pFb  += iFStep8;
        *pFb  = InverseQuadRootOfSumSquares(E, J);               // F[2048-j]
        
        //INTEGER_ONLY( assert( F[i]>=0 && F[(1024>>iShrink)-i]>=0 && F[(1024>>iShrink)+i]>=0 && F[(2048>>iShrink)-i]>=0 ) );
        
        //** This block uses 36 adds and 20 mults plus either 4 more mults or 4 shifts
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
        LPC_COMPARE_DEBUG( pau, iFreq, iFreq, iF, F, rgLpcCoef );

        rgwtLpcSpec [iFreq] = wtTemp = F[iFreq];

#       if defined(_DEBUG) && 0
#           if defined(BUILD_INT_FLOAT)
                printf("%4d  %12.6f  0x%08x\n", iFreq, wtTemp, ((U32)((wtTemp)*(1<<21))) );
#           endif // BUILD_INT_FLOAT
#       endif // _DEBUG

        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
        
        INTEGER_ONLY( assert( wtTemp>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,wtTemp);                                
        MONITOR_RANGE(gMR_fltLPC_F3,F[iFreq]);
    }
    
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

#endif // defined(PLATFORM_LPC_PRUNED_NONREDUNDANT) && !defined(V4V5_COMPARE_MODE)

#if defined(PLATFORM_LPC_PRUNED_STRAIGHT) || defined(V4V5_COMPARE_MODE)

//#pragma COMPILER_MESSAGE(__FILE__ "(961) : Warning - building PLATFORM_LPC_PRUNED_STRAIGHT LPC spectrum")

// warning - does not quite match V4_INT_FLOAT - must be a bug somewhere below

// ************************************************************************************
//
// LPC to Spectrum using a Pruned FFT (with little duplicate expression reduction)
//
// currently this uses a stride length >= 1 to handle tarnsforms less than 2048.
// This can cause unnecessary cache activity since sizeof(F[.]) = 16K bytes.
// This could be improved with some loss of readability
//
// ************************************************************************************

WMARESULT prvLpcToSpectrum(CAudioObject* pau, const LpType* rgLpcCoef, PerChannelInfo* ppcinfo)
{
    LpSpecType original[LPCORDER];
    LpSpecType F[2*MAX_LP_SPEC_SIZE];      
    Int i,j;
    
    LpSpecType* rgwtLpcSpec;
    WeightType wtLpcSpecMax, wtTemp;
    Int iFreq, iF, iLoopLimit;
    
#ifdef REPLICATE_V4_LPC
    Int iSize = pau->m_cFrameSampleHalf;
#else
    Int iSize;
    if (pau->m_fV5Lpc)
        iSize = pau->m_cSubband;
    else 
        iSize = pau->m_cFrameSampleHalf;
#endif
    Int iShrink = LOG2(MAX_LP_SPEC_SIZE/iSize);     // for smaller transforms, shrink or expand indexing
    Int iStride = MAX_LP_SPEC_SIZE/iSize;               // for smaller transforms, stride past unused F[.]
    
    BP2Type* pC;
    Int pCInc;
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,LPC_TO_SPECTRUM_PROFILE);
    
    // DEBUG_ONLY( if (pau->m_iFrameNumber==32) { DEBUG_BREAK(); } );
    
    assert(pau->m_fltFlatenFactor == 0.5f); //So we can use quadRoot instead of pow(x,m_fltFlatenFactor/2);
    assert( iSize <= MAX_LP_SPEC_SIZE );
    
    for (i = 0; i < LPCORDER; i++) 
        original[i] = -LP_SPEC_FROM_LP(rgLpcCoef[i]); 
    
    DEBUG_ONLY( memset( F, 0, 2*MAX_LP_SPEC_SIZE*sizeof(LpSpecType) ) );
    
    // All these indexes into F[.] are 0 mod 128.
    F[128]  = LP_SPEC_FROM_FLOAT(1) + original[7] + MULT_BP2X(SQRT2,original[3]);
    F[256]  = LP_SPEC_FROM_FLOAT(1) - original[7] + original[3];
    F[384]  = F[896] = LP_SPEC_FROM_FLOAT(1) - original[7];
    F[640]  = LP_SPEC_FROM_FLOAT(1) + original[7] - MULT_BP2X(SQRT2,original[3]);
    F[768]  = LP_SPEC_FROM_FLOAT(1) - original[7] - original[3];
    
    F[1152] = original[1] + original[9] + MULT_BP2X(SQRT2,original[5]);
    F[1280] = original[1] - original[9] + original[5];
    F[1664] = original[1] + original[9] - MULT_BP2X(SQRT2,original[5]);
    F[1792] = original[1] - original[9] - original[5];
    F[1920] = F[1408]     = original[1] - original[9];
    
    F[2176] = original[0] + original[8] + MULT_BP2X(SQRT2,original[4]);
    F[2304] = original[0] - original[8] + original[4];
    F[2688] = original[0] + original[8] - MULT_BP2X(SQRT2,original[4]);
    F[2816] = original[0] - original[8] - original[4];
    F[2944] = F[2432] = original[0] - original[8];
    
    F[3200] = original[2] + MULT_BP2X(SQRT2,original[6]);
    F[3328] = original[2] + original[6];
    F[3712] = original[2] - MULT_BP2X(SQRT2,original[6]);
    F[3840] = original[2] - original[6];
    F[3968] = F[3456] = original[2];
    
    // All these indexes are 0 mod 512.
    F[0 ]   = LP_SPEC_FROM_FLOAT(1) + original[7] + original[3] + original[1] + original[9] + original[5] + original[0] + original[8] + original[4] + original[2] + original[6];
    F[512]  = LP_SPEC_FROM_FLOAT(1) + original[7] - original[3] + MULT_BP2X(SQRT2_2, original[0] + original[8] - original[4] - original[2] + original[6]);
    F[1024] = LP_SPEC_FROM_FLOAT(1) + original[7] + original[3] - original[1] - original[9] - original[5];
    F[1536] = LP_SPEC_FROM_FLOAT(1) + original[7] - original[3] + MULT_BP2X(SQRT2_2, original[2] - original[6] - original[0] - original[8] + original[4] );
    F[2048] = LP_SPEC_FROM_FLOAT(1) + original[7] + original[3] + original[1] + original[9] + original[5] - original[0] - original[8] - original[4] - original[2] - original[6];
    F[2560] = -original[1] - original[9] + original[5] + MULT_BP2X(SQRT2_2,original[2] - original[6] + original[0] + original[8] - original[4]);
    F[3072] =  original[0] + original[8] + original[4] - original[2] - original[6];
    F[3584] =  original[1] + original[9] - original[5] + MULT_BP2X(SQRT2_2,original[0] + original[8] - original[4] + original[2] - original[6]);
    
    // a few get squared.
    F[0 ]   =   SQUARE(F[0]);
    F[512]  =   SQUARE(F[512])  + SQUARE(F[3584]);
    F[1024] =   SQUARE(F[1024]) + SQUARE(F[3072]);
    F[1536] =   SQUARE(F[1536]) + SQUARE(F[2560]);
    F[2048] =   SQUARE(F[2048]);
    
#if defined(_DEBUG) && defined(WMA_MONITOR)
    for(i=0; i<2*SPEC_SIZE; i += 128)
        MONITOR_RANGE(gMR_fltLPC_F1,F[i]);
#endif
    
    iLoopLimit = 128>>iShrink;
    pC = &lpc_cnst3[1*(4<<iShrink)];
    pCInc = (4<<iShrink)-3;  // 3 of them get done with ++
    for (i=1,j=iStride; i<iLoopLimit; i++, j += iStride, pC += pCInc )
    {
        LpSpecType C1, C2, C3, C4;
        C1 = *pC++;  // cnst3[i*(4<<iShink)];
        C2 = *pC++;  // cnst3[i*(4<<iShinrk)+1];
        C3 = *pC++;  // cnst3[i*(4<<iShrink)+2];
        C4 = *pC;    // cnst3[i*(4<<iShrink)+3];
        assert( BP2_FROM_FLOAT(1) <= C1 && C1 <= BP2_FROM_FLOAT(1.5) );
        
        F[j ]     = LP_SPEC_FROM_FLOAT(1) + MULT_BP2X(C1,original[7]) + MULT_BP2X(C2,original[3]);
        F[256-j]  = LP_SPEC_FROM_FLOAT(1) + MULT_BP2X(C3,original[7]) + MULT_BP2X(C2,original[3]);
        F[256+j]  = LP_SPEC_FROM_FLOAT(1) - MULT_BP2X(C1,original[7]) - MULT_BP2X(C4,original[3]);
        F[512-j]  = LP_SPEC_FROM_FLOAT(1) - MULT_BP2X(C3,original[7]) + MULT_BP2X(C4,original[3]);
        F[512+j]  = LP_SPEC_FROM_FLOAT(1) + MULT_BP2X(C1,original[7]) - MULT_BP2X(C2,original[3]);
        F[768-j]  = LP_SPEC_FROM_FLOAT(1) + MULT_BP2X(C3,original[7]) - MULT_BP2X(C2,original[3]);
        F[768+j]  = LP_SPEC_FROM_FLOAT(1) - MULT_BP2X(C1,original[7]) + MULT_BP2X(C4,original[3]);
        F[1024-j] = LP_SPEC_FROM_FLOAT(1) - MULT_BP2X(C3,original[7]) - MULT_BP2X(C4,original[3]);
        
        F[1024+j]  = original[1] + MULT_BP2X(C1,original[9]) + MULT_BP2X(C2,original[5]);
        F[1280-j]  = original[1] + MULT_BP2X(C3,original[9]) + MULT_BP2X(C2,original[5]);
        F[1280+j]  = original[1] - MULT_BP2X(C1,original[9]) - MULT_BP2X(C4,original[5]);
        F[1536-j]  = original[1] - MULT_BP2X(C3,original[9]) + MULT_BP2X(C4,original[5]);
        F[1536+j]  = original[1] + MULT_BP2X(C1,original[9]) - MULT_BP2X(C2,original[5]);
        F[1792-j]  = original[1] + MULT_BP2X(C3,original[9]) - MULT_BP2X(C2,original[5]);
        F[1792+j]  = original[1] - MULT_BP2X(C1,original[9]) + MULT_BP2X(C4,original[5]);
        F[2048-j]  = original[1] - MULT_BP2X(C3,original[9]) - MULT_BP2X(C4,original[5]);
        
        F[2048+j]  = original[0] + MULT_BP2X(C1,original[8]) + MULT_BP2X(C2,original[4]);
        F[2304-j]  = original[0] + MULT_BP2X(C3,original[8]) + MULT_BP2X(C2,original[4]);
        F[2304+j]  = original[0] - MULT_BP2X(C1,original[8]) - MULT_BP2X(C4,original[4]);
        F[2560-j]  = original[0] - MULT_BP2X(C3,original[8]) + MULT_BP2X(C4,original[4]);
        F[2560+j]  = original[0] + MULT_BP2X(C1,original[8]) - MULT_BP2X(C2,original[4]);
        F[2816-j]  = original[0] + MULT_BP2X(C3,original[8]) - MULT_BP2X(C2,original[4]);
        F[2816+j]  = original[0] - MULT_BP2X(C1,original[8]) + MULT_BP2X(C4,original[4]);
        F[3072-j]  = original[0] - MULT_BP2X(C3,original[8]) - MULT_BP2X(C4,original[4]);
        
        F[3072+j] = F[3328-j] = original[2] + MULT_BP2X(C2,original[6]);
        F[3328+j] = F[4096-j] = original[2] - MULT_BP2X(C4,original[6]);
        F[3584-j] = F[3840+j] = original[2] + MULT_BP2X(C4,original[6]);
        F[3584+j] = F[3840-j] = original[2] - MULT_BP2X(C2,original[6]);
        
    }
    
#if defined(_DEBUG) && defined(WMA_MONITOR)
    for(j=0; j<2*SPEC_SIZE; j += iLoopLimit)
        MONITOR_RANGE(gMR_fltLPC_F2,F[j]);
#endif
    
    iLoopLimit = 512>>iShrink;
    pC = &lpc_cnst4[1*(6<<iShrink)];
    pCInc = (6<<iShrink)-5;  // 5 of them get done with ++
    for (i=1, j=iStride; i<iLoopLimit; i++, j += iStride, pC += pCInc ){
        LpSpecType C1, C2, C3, C4, C5, C6;
        LpSpecType T7, T8, T9, TA, TB;
        LpSpecType D, E, G, I, J;
        C1 = *pC++;   // cnst4[i*6];
        C2 = *pC++;   // cnst4[i*6+1];
        C3 = *pC++;   // cnst4[i*6+2];
        C4 = *pC++;   // cnst4[i*6+3];
        C5 = *pC++;   // cnst4[i*6+4];
        C6 = *pC;     // cnst4[i*6+5];
        assert( BP2_FROM_FLOAT(0) < C2 && C2 < BP2_FROM_FLOAT(1.5) );
        
        T7 = MULT_BP2X(C3,F[1024+j]) + MULT_BP2X(C4,F[2048-j]);
        C4 = MULT_BP2X(C4,F[1024+j]) - MULT_BP2X(C3,F[2048-j]);
        T8 = MULT_BP2X(C5,F[2048+j]) + MULT_BP2X(C6,F[3072-j]);
        T9 = MULT_BP2X(C1,F[3072+j]) + MULT_BP2X(C2,F[4096-j]);
        
        TA = F[j] + F[1024-j];
        TB = F[j] - F[1024-j];
        
        D   = DIV2(+ T7  + T8 + T9 + TA);
        G   = DIV2(+ C4  + T8 - T9 - TB);
        I   = DIV2(- C4  + T8 - T9 + TB);
        J   = DIV2(+ T7  - T8 - T9 + TA);
        
        C6 = MULT_BP2X(C6,F[2048+j]) - MULT_BP2X(C5,F[3072-j]); 
        C2 = MULT_BP2X(C2,F[3072+j]) - MULT_BP2X(C1,F[4096-j]);
        
        E         = DIV2(+ C4  + C6 + C2 + TB);
        F[j]      = SQUARE(D) + SQUARE(E);
        
        E         = DIV2(- T7  + C6 - C2 + TA);
        F[1024-j] = SQUARE(E) + SQUARE(G);
        
        E         = DIV2(- T7  - C6 + C2 + TA);
        F[1024+j] = SQUARE(E) + SQUARE(I);
        
        E         = DIV2(- C4  + C6 + C2 - TB);
        F[2048-j] = SQUARE(E) + SQUARE(J);
        
        INTEGER_ONLY( assert( F[j]>=0 && F[1024-j]>=0 && F[1024+j]>=0 && F[2048-j]>=0 ) );
    }
    
    // the remainder should be identical to the other versions of this routine
    
    rgwtLpcSpec  = INTEGER_OR_INT_FLOAT( ppcinfo->m_rgiWeightFactor, ppcinfo->m_rgfltWeightFactor ); 
    wtLpcSpecMax = WEIGHT_FROM_FLOAT(0.0F);
    iLoopLimit   = pau->m_cSubband;
    
    for (iFreq = 0, iF = 0; iFreq < iLoopLimit; iFreq++, iF += iStride) 
    {
        LPC_COMPARE_DEBUG( pau, iFreq, iF, iF, F, rgLpcCoef );
        
        rgwtLpcSpec [iFreq] = wtTemp = InverseQuadRoot(F[iF]);
        
        if ( wtTemp > wtLpcSpecMax )
            wtLpcSpecMax = wtTemp;
        
        INTEGER_ONLY( assert( wtTemp>= 0 ); );
        MONITOR_RANGE(gMR_rgfltWeightFactor,wtTemp);                                
        MONITOR_RANGE(gMR_fltLPC_F3,F[iF]);
    }
    
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

#endif  // defined(PLATFORM_LPC_PRUNED_STRAIGHT) || defined(V4V5_COMPARE_MODE)
