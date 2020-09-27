/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    x86.c

Abstract:

    Intel x86-specific functions.

Author:

    Raymond Cheng (raych)       Jul 29, 1999

Revision History:


*************************************************************************/

#include "bldsetup.h"

#include "macros.h" // This will define WMA_TARGET_X86 (or not)
#ifdef _XBOX
#define WMA_TARGET_X86
#define _SCRUNCH_INLINE_ASM
#endif

#ifdef WMA_TARGET_X86

#ifndef UNDER_CE
#include <time.h>
#endif
#include <math.h>
#include <limits.h>
#include "MsAudio.h"
#include "x86opcodes.h"
#include "AutoProfile.h"


#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && !defined(_Embedded_x86)

#ifdef INTELFFT_FOR_CE
// For some reason, somebody has a #define __stdcall __cdecl somewhere.
// Get rid of this, because it's causing us linker errors.
#ifdef __stdcall
#undef __stdcall
#endif
#endif  // INTELFFT_FOR_CE

#include "nsp.h"
#include "nspfft.h"

// From userdll.c
Bool MyInitNSP(I32 fdwReason);
void auFreeIntelFFTHelper(I32 iMinFrameSizeLOG,
                          I32 iMaxFrameSizeLOG);
void auSetMinMaxFrameSize(const Int iMinFrameSizeLOG,
                          const Int iMaxFrameSizeLOG);
extern I32 g_iMinFrameSizeLOG;
extern I32 g_iMaxFrameSizeLOG;

// This function initializes for prvFFT4DCT_INTEL as well as prvInvFftComplexConjugate_INTEL

WMARESULT auInitIntelFFT(const CAudioObject *pau,
                         const INTELFFT_INITCODE iCode)
{
    const int c_iMinFrameSizeLOG = LOG2(pau->m_cFrameSampleHalf / pau->m_iMaxSubFrameDiv);
    const int c_iMaxFrameSizeLOG = LOG2(pau->m_cFrameSampleHalf);
    WMARESULT   wmaResult = WMA_OK;

    int     i;
    Bool    fResult;

    fResult = MyInitNSP(iCode);
    if (WMAB_FALSE == fResult)
    {
        wmaResult = WMA_E_NOTSUPPORTED;
        goto exit;
    }

    for (i = c_iMinFrameSizeLOG; i <= c_iMaxFrameSizeLOG; i++)
        nspcFft(NULL, i-1, NSP_Init); // Remember FFT size is frame size / 2, so subtract 1

    auSetMinMaxFrameSize(c_iMinFrameSizeLOG, c_iMaxFrameSizeLOG);

exit:
    return wmaResult;
}

WMARESULT auFreeIntelFFT(const CAudioObject *pau,
                         const INTELFFT_INITCODE iCode)
{
    int iMinFrameSizeLOG = 0;
    int iMaxFrameSizeLOG = 0;

    Bool fResult;

    // It is possible to be freed without having been initialized
    if (0 != pau->m_cFrameSampleHalf)
    {
        iMinFrameSizeLOG = LOG2(pau->m_cFrameSampleHalf / pau->m_iMaxSubFrameDiv);
        iMaxFrameSizeLOG = LOG2(pau->m_cFrameSampleHalf);
    }
    auFreeIntelFFTHelper(iMinFrameSizeLOG, iMaxFrameSizeLOG);

    fResult = MyInitNSP(iCode);
    assert(fResult);

    return WMA_OK;
}


void prvFFT4DCT_INTEL(CoefType data[], Int nLog2np, FftDirection fftDirection)
{
    assert (fftDirection == FFT_FORWARD || fftDirection == FFT_INVERSE);

    if (fftDirection == FFT_FORWARD)
      nspcFft((SCplx *)data, nLog2np, NSP_Forw);
    else
      nspcFft((SCplx *)data, nLog2np, NSP_Inv);
}


#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE)


#ifdef USE_SIN_COS_TABLES
//////////////////////////////////////////////////
//         MMX 32x32=32mbs's multiply           //
//////////////////////////////////////////////////

// input:  mm0 formated as defined by SHUFFLE_BITS
//         mm1 standerd 2s complement 32bit number 
//         mm7 = 0x0000FFFF0000FFFF
// output: mm1
#ifdef BEST_PRESITION
// temporary registers: mm2,mm4
#define MMX_MULT_BP2    _asm {                                     \
_asm                    movq        mm2,mm1                        \
_asm                    pand        mm2,mm7                        \
_asm                    pxor        mm1,mm2                        \
_asm                    psrlw       mm2,1                          \
_asm                    por         mm1,mm2                        \
_asm                    movq        mm2,mm0                        \
_asm                    pmaddwd     mm2,mm1                        \
_asm                    movq        mm4,mm1                        \
_asm                    pslld       mm4,16                         \
_asm                    pmaddwd     mm4,mm0                        \
_asm                    psrld       mm4,15                         \
_asm                    paddd       mm2,mm4                        \
_asm                    psrad       mm2,13                         \
_asm                    psrld       mm1,16                         \
_asm                    pmaddwd     mm1,mm0                        \
_asm                    pslld       mm1,2                          \
_asm                    paddd       mm1,mm2                        }
#else
// temporary registers: mm2
#define MMX_MULT_BP2     _asm {                                    \
_asm                     movq        mm2,mm1                       \
_asm                     pand        mm2,mm7                       \
_asm                     pxor        mm1,mm2                       \
_asm                     psrlw       mm2,1                         \
_asm                     por         mm1,mm2                       \
_asm                     movq        mm2,mm0                       \
_asm                     pmaddwd     mm2,mm1                       \
_asm                     psrad       mm2,13                        \
_asm                     psrld       mm1,16                        \
_asm                     pmaddwd     mm1,mm0                       \
_asm                     pslld       mm1,2                         \
_asm                     paddd       mm1,mm2                       }

#endif

///////////////////////////////////////////
//     Other macros for recomMono_MMX    //
///////////////////////////////////////////

#define LOAD_SIGN _asm  {                                          \
_asm                    movd        mm3,ebx                        \
_asm                    shr         ebx,14                         \
_asm                    movq        mm4,[g_rgiMMXSineTable+ebx*8]  \
_asm                    movd        ebx,mm3                        }

#define ADD_AND_SATURATE _asm {                                    \
_asm                    pslld       mm5,COEF_FRAC_BITS             \
_asm                    paddd       mm5,mm1                        \
_asm                    psrad       mm5,COEF_FRAC_BITS             \
_asm                    packssdw    mm5,mm5                        }

#define INIT_LOOP1_2 _asm {                                        \
_asm                    movzx       eax,word Ptr iLoopCount        \
_asm                    shr         eax,1                          \
_asm                    mov         edi,piDst                      \
_asm                    mov         esi,pSin                       \
_asm                    mov         ecx,piSrc                      \
_asm                    movq        mm7,x0000FFFF0000FFFF          \
_asm                    mov         edx,uiSignbitCount             \
_asm                    movzx       ebx,word Ptr uiSignbitData     }

#define SAVE_LOOP_STATE1_2 _asm {                                  \
_asm                mov         piSrc,ecx                          \
_asm                mov         piDst,edi                          \
_asm                mov         pSin,esi                           \
_asm                mov         uiSignbitCount,edx                 \
_asm                mov         uiSignbitData,bx                   }

#define GET_NEXT_SINES _asm {                                      \
_asm                    movd        mm3,eax                        \
_asm                    mov         eax,piSrcSign                  \
_asm                    movzx       ebx,word Ptr [eax]             \
_asm                    add         eax,2                          \
_asm                    mov         piSrcSign,eax                  \
_asm                    mov         edx,16                         \
_asm                    movd        eax,mm3                        }

#define LOAD_FLIP_SHIFT_SATURATE  _asm   {                         \
_asm                    movq        mm0,[eax-8]                    \
_asm                    movq        mm2,mm0                        \
_asm                    psrlq       mm0,32                         \
_asm                    psllq       mm2,32                         \
_asm                    por         mm0,mm2                        \
_asm                    psrad       mm0,COEF_FRAC_BITS             \
_asm                    packssdw    mm0,mm0                        }

#define INIT_LOOP_3 _asm {                                         \
_asm                    movzx       ecx,word Ptr iLoopCount        \
_asm                    shr         ecx,1                          \
_asm                    pxor        mm7,mm7                        \
_asm                    movq        mm6,xFFFF0000FFFF0000          \
_asm                    mov         edi,piDst                      \
_asm                    mov         eax,piCoef                     }


#ifdef BUILD_INTEGER

WMARESULT auReconstruct_MMX (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll){
    Int iChannel;
    WMARESULT hr;
    for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
    {
        hr = (auReconMono_MMX) (pau, 
                          pau->m_rgpcinfo + iChannel, 
                          piOutput + iChannel,  
                          (I16*)pcSampleGet,
                          fForceTransformAll);
    }
    return hr;
}

WMARESULT auReconMono_MMX ( CAudioObject* pau, 
                        PerChannelInfo* ppcinfo, 
                        I16* piOutput, 
                        I16* pcSampleGet,
                        Bool fForceTransformAll)
{
    //all starting from the begining of this frame
    //official starting of a subfrm (including leading zeros)
    //case 1 (Large -> Small):  starts later than what's been reconstructed w/o leading zeros
    //                          in this case dst should be skiped (this may mean memcpy from prev)
    //                          src and coef are fine
    //                          
    //case 2 (Small -> Large):  starts earler than what's been reconstructed but all leading zeros
    //                          in this case dst 

    int *pSin = pau->m_piSinForRecon;
    static U32 g_rgiMMXSineTable[8] = { 0,0,0,0xFFFF0000,0xFFFF0000,0,0xFFFF0000,0xFFFF0000};
    static const __int64 x0000FFFF0000FFFF = 0x0000FFFF0000FFFF;
    static const __int64 xFFFF0000FFFF0000 = 0xFFFF0000FFFF0000;

    CoefType *piCoef;   //floating vlaue = (float)*piCoef/(1<<5)  assuming TRANSFORM_FRACT_BITS = 5
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;

#ifndef PREVOUTPUT_16BITS
    I16    *piSrcSign;
    U16     uiSignbitData;
    U32     uiSignbitCount;
#else
    assert(0); //Not yet implemented
#   pragma COMPILER_MESSAGE("INVALID BUILD!! reconMono_MMX will not work with PREVOUTPUT_16BITS")
#endif  // PREVOUTPUT_16BITS
#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

    //persistent states
    i = ppcinfo->m_iCurrGetPCM_SubFrame;       //current position in a frame
    piCoef = (CoefType*)ppcinfo->m_rgiCoefReconCurr; 
    piSrc = ppcinfo->m_piPrevOutputCurr;

    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(ppcinfo->m_piPrevOutput, piSrc, ppcinfo->m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)

    ASSERT_SIGNBIT_POINTER(piSrcSign);

    if (i < cSubFrameSampleQuad) 
    {
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            if (cChan==1){
                I16 iLoopCount = (I16)(iSegEnd-i);
                assert((iLoopCount & 0xF) == 0); 
                (*pcSampleGet)+=iLoopCount;
                i+=iLoopCount;
                _asm{
                INIT_LOOP1_2
                pxor mm6,mm6

                Loop1m:
                    push        eax

                    mov         eax,piCoef
                    movq        mm0,[esi]
                    movq        mm1,[eax]

                    MMX_MULT_BP2

                    LOAD_SIGN

                    movd        mm5,[ecx]
                    punpcklwd   mm5,mm6     //for mono
                    por         mm5,mm4       

                    ADD_AND_SATURATE

                    movd        [edi],mm5


                    add         ecx,2*2
                    add         edi,2*2
                    add         esi,2*4
                    add         eax,2*4
                    mov         piCoef,eax

                    sub         edx,2
                    shl         ebx,2
                    and         ebx,0xFFFF

                    cmp         edx,0
                    jnz         SineRegisterNotEmpty1m
                        GET_NEXT_SINES
                    SineRegisterNotEmpty1m:


                pop         eax
                dec         eax
                jnz         Loop1m

                SAVE_LOOP_STATE1_2

                } 
            }else {
                I16 iLoopCount = (I16)(iSegEnd-i);
                assert((iLoopCount & 0xF) == 0); 
                (*pcSampleGet)+=iLoopCount;
                i+=iLoopCount;
                _asm{
                INIT_LOOP1_2
                movq        mm6,xFFFF0000FFFF0000     

                Loop1:
                    push        eax

                    mov         eax,piCoef
                    movq        mm0,[esi]
                    movq        mm1,[eax]

                    MMX_MULT_BP2

                    LOAD_SIGN

                    movq        mm5,[ecx]
                    pand        mm5,mm7       
                    por         mm5,mm4       

                    ADD_AND_SATURATE

                    pxor        mm4,mm4        //interleved write for sterio
                    punpcklwd   mm5,mm4
                    movq        mm4,[edi]
                    pand        mm4,mm6
                    por         mm5,mm4
                    movq        [edi],mm5


                    add         ecx,4*2
                    add         edi,4*2
                    add         esi,2*4
                    add         eax,2*4
                    mov         piCoef,eax

                    sub         edx,2
                    shl         ebx,2
                    and         ebx,0xFFFF

                    cmp         edx,0
                    jnz         SineRegisterNotEmpty1
                        GET_NEXT_SINES
                    SineRegisterNotEmpty1:


                pop         eax
                dec         eax
                jnz         Loop1

                SAVE_LOOP_STATE1_2

                } 
            }
        }
    }
    if (i < iCoefRecurQ2)  
    {
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            I16 iLoopCount = (I16)(iSegEnd-i);
            assert((iLoopCount&0x1) == 0);
            (*pcSampleGet)+=iLoopCount;
            i+=iLoopCount;
            if (cChan==1){
                _asm{
                INIT_LOOP1_2
                pxor             mm6,mm6

                Loop2:
                    push        eax

                    mov         eax,piCoef

                    movq        mm0,[esi]
                    movq        mm1,[eax-8]

                    movq        mm2,mm1   //Flip high and low 32 bits
                    psrlq       mm1,32
                    psllq       mm2,32
                    por         mm1,mm2

                    MMX_MULT_BP2

                    LOAD_SIGN

                    movd        mm5,[ecx]
                    punpcklwd   mm5,mm6     
                    por         mm5,mm4     

                    ADD_AND_SATURATE

                    movd        [edi],mm5


                    add         ecx,2*2
                    add         edi,2*2
                    add         esi,2*4

                    sub         eax,2*4
                    mov         piCoef,eax

                    sub         edx,2
                    shl         ebx,2
                    and         ebx,0xFFFF

                    cmp         edx,0
                    jnz         SineRegisterNotEmpty2
                        GET_NEXT_SINES
                    SineRegisterNotEmpty2:

                pop         eax
                dec         eax
                jnz         Loop2

                SAVE_LOOP_STATE1_2

                } 
            } else {
                _asm{
                INIT_LOOP1_2
                movq        mm6,xFFFF0000FFFF0000     

                Loop2m:
                    push        eax

                    mov         eax,piCoef

                    movq        mm0,[esi]
                    movq        mm1,[eax-8]

                    movq        mm2,mm1   //Flip high and low 32 bits
                    psrlq       mm1,32
                    psllq       mm2,32
                    por         mm1,mm2

                    MMX_MULT_BP2

                    LOAD_SIGN

                    movq        mm5,[ecx]
                    pand        mm5,mm7       //for sterio
                    por         mm5,mm4       //for sterio
                    //punpcklwd   mm5,mm6     //for mono

                    ADD_AND_SATURATE

                    pxor        mm4,mm4        //interleved write for sterio
                    punpcklwd   mm5,mm4
                    movq        mm4,[edi]
                    pand        mm4,mm6
                    por         mm5,mm4
                    movq        [edi],mm5


                    add         ecx,4*2
                    add         edi,4*2
                    add         esi,2*4

                    sub         eax,2*4
                    mov         piCoef,eax

                    sub         edx,2
                    shl         ebx,2
                    and         ebx,0xFFFF

                    cmp         edx,0
                    jnz         SineRegisterNotEmpty2m
                        GET_NEXT_SINES
                    SineRegisterNotEmpty2m:

                pop         eax
                dec         eax
                jnz         Loop2m

                SAVE_LOOP_STATE1_2

                } 
            }
        }
    }
    if (i < iCoefRecurQ3) 
    {
        I16 iLoopCount;
        iSegEnd = min (iCoefRecurQ3, iEnd);
        iLoopCount = (I16)( iSegEnd - i );
        assert((iLoopCount&1)==0); 
        (*pcSampleGet)+=iLoopCount;
        i+=iLoopCount;
        if (iLoopCount) {
            if (cChan==1){
                _asm{
                INIT_LOOP_3
                Loop3m:      
                    LOAD_FLIP_SHIFT_SATURATE
                    movd        [edi],mm0   //store 
                    add         edi,2*2
                    sub         eax,2*4
                dec         ecx
                jnz         Loop3m
                mov         piDst,edi
                mov         piCoef,eax
                }
            }else{
                _asm{
                INIT_LOOP_3
                
                Loop3:      
                    LOAD_FLIP_SHIFT_SATURATE
                    punpcklwd   mm0,mm7     //Merge with interleved data
                    movq        mm1,[edi]
                    pand        mm1,mm6
                    por         mm0,mm1
                    movq        [edi],mm0   //store 
                    add         edi,4*2
                    sub         eax,2*4
                dec         ecx
                jnz         Loop3
                mov         piDst,edi
                mov         piCoef,eax
                }
            }
        }
    }
    _asm emms

    SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
    ppcinfo->m_rgiCoefReconCurr = (Int*)piCoef;
    ppcinfo->m_iCurrGetPCM_SubFrame = (I16)i;
    ppcinfo->m_piPrevOutputCurr = piSrc;
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}
#undef MMX_MULT_BP2
#undef LOAD_SIGN
#undef ADD_AND_SATURATE
#undef INIT_LOOP1_2
#undef SAVE_LOOP_STATE1_2 
#undef GET_NEXT_SINES
#undef LOAD_FLIP_SHIFT_SATURATE
#undef INIT_LOOP_3 

#else //not BUILD_INTEGER

#ifndef PREVOUTPUT_16BITS
#define COMBINE_SIGNBIT_FAST                                                   \
               iResult = (((uiSignbitData)>>15) & 0xFFFF0000) | (U16)*piSrc;   \
               uiSignbitData <<= 1;                                             
#else
#define COMBINE_SIGNBIT_FAST iResult = *piSrc; 
#endif            


#define RECORD_SIGNBIT_FAST                           \
    uiSignbitData <<= 1;                              \
    uiSignbitData |= ((iResult >> 31) & 0x00000001);  \
    uiSignbitCount += 1;                               


/////////////////////////////////////////////////////////////////////////////////////
// Save History Mono - X86 optimized version
// This function achieves its speedup by unrolling the loop so that the 1 out of 16
// brantch is removed and the same functionality is done with the loop structure.
// It also uses the sin tables instead of doing the recursion 
/////////////////////////////////////////////////////////////////////////////////////
#if !defined(PREVOUTPUT_16BITS) && !defined (BUILD_INTEGER)
WMARESULT auSaveHistoryMono (CAudioObject* pau, 
                            PerChannelInfo* ppcinfo, 
                            Bool fForceTransformAll)
{
    CoefType *piCoef;   //floating vlaue = (float)*piCoef/(1<<5)  assuming TRANSFORM_FRACT_BITS = 5
    
    Int i;      // , iEnd, iSegEnd;
    I16 *piDst; 
    I32 iResult;
    CoefType cfResult;

    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
	// HALF_TRANSFORM: the following member variables are not restored so continue to hold their halved values
    Int cSubFrameSample3Quad = 3 * pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ4 = pau->m_iCoefRecurQ4;
    BP2Type bp2Sin  = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownStart, pau->m_fltSinRampDownStart );
    BP2Type bp2Cos  = INTEGER_OR_INT_FLOAT( pau->m_iCosRampDownStart, pau->m_fltCosRampDownStart );
    BP2Type bp2Sin1 = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownPrior, pau->m_fltSinRampDownPrior );
    BP2Type bp2Cos1 = INTEGER_OR_INT_FLOAT( pau->m_iCosRampDownPrior, pau->m_fltCosRampDownPrior );
    BP2Type bp2Step = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownStep,  pau->m_fltSinRampDownStep  );
    Float *pfltSinTable = pau->m_piSinForSaveHistory;
    Int iLoopCnt;
    Int iLoopLen;
    Int iLoopBy16;
    Int iLoopBy1;

#ifndef PREVOUTPUT_16BITS
    I16    *piDstSign = ppcinfo->m_piPrevOutputSign;
    U32     uiSignbitData = 0;
    U32     uiSignbitCount = 0;
#endif  // PREVOUTPUT_16BITS

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,SAVE_HISTORY_MONO_PROFILE);
#endif

    //all starting from the begining of this frame
    //official starting of a subfrm (including leading zeros)
    //case 1 (Large -> Small):  starts later than what's been reconstructed w/o leading zeros
    //                          in this case dst should be skiped (this may mean memcpy from prev)
    //                          src and coef are fine
    //                          
    //case 2 (Small -> Large):  starts earler than what's been reconstructed but all leading zeros
    //                          in this case dst 

    //eventually we'll use piOutput directly
    piDst = ppcinfo->m_piPrevOutput;


    //persistent states
    i = ppcinfo->m_iCurrGetPCM_SubFrame;       //current position in a frame
    assert( (0<=i && i<=iCoefRecurQ4) || (i==CURRGETPCM_INVALID));
    piCoef = (CoefType*)ppcinfo->m_rgiCoefReconCurr; 
    ASSERT_SIGNBIT_POINTER(piDstSign);


#if defined(WMA_SHOW_FRAMES)  && defined(_DEBUG)
    prvWmaShowFrames(pau, "SavHist", " %4d 0x%08x 0x%08x %4d %4d", 
        i, piCoef, piDst, cSubFrameSample3Quad, iCoefRecurQ4);
#endif
    iLoopLen = cSubFrameSample3Quad - i;
    if ( iLoopLen<0 )
        iLoopLen = 0;
    iLoopBy1  = iLoopLen & 0xF;
    iLoopBy16 = iLoopLen >> 4;
    for (iLoopCnt = 0; iLoopCnt < iLoopBy16; iLoopCnt++,i+=16) {

        //1
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //2
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //3
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //4
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //5
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //6
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //7
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //8
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //9
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //10
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //11
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //12
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //13
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //14
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //15
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        //16
        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        *piDstSign = (I16) uiSignbitData;                                       
        piDstSign += 1;                                                      
        uiSignbitCount = 0;                                                     

    }

    for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopBy1++,i++)    {

        cfResult = INT_FROM_COEF( MULT_BP2(*--pfltSinTable,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );

        piDst += cChan;

        RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData,
            uiSignbitCount, 1);

    }

    iLoopLen = iCoefRecurQ4 - i;
    if ( iLoopLen < 0 )
        iLoopLen = 0;
    iLoopBy1  = iLoopLen & 0xF;
    iLoopBy16 = iLoopLen >> 4;
    for (iLoopCnt=0; iLoopCnt < iLoopBy16; iLoopCnt++,i++)    {

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );
        piDst += cChan;
        RECORD_SIGNBIT_FAST;

        *piDstSign = (I16) uiSignbitData;                                       
        piDstSign += 1;                                                      
        uiSignbitCount = 0;                                                     
    }
    for (iLoopCnt=0; iLoopCnt < iLoopBy1; iLoopCnt++,i++)    {

        cfResult = INT_FROM_COEF( MULT_BP2(-(*--pfltSinTable),*piCoef++) );
        
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );

        piDst += cChan;

        RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData,
            uiSignbitCount, 1);

    }

    ppcinfo->m_iCurrGetPCM_SubFrame = (I16)i;       //protect ourself from multiple calls

#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}
#endif // !defined(PREVOUTPUT_16BITS) && !defined (BUILD_INTEGER)

//This function is here to satisfy the function pointers to be compatible with the 
//reconStereo's
WMARESULT auReconstruct_X86 (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll){
    Int iChannel;
    WMARESULT hr;
    for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
    {
        hr = (auReconMono_X86) (pau, 
                          pau->m_rgpcinfo + iChannel, 
                          piOutput + iChannel,  
                          (I16*)pcSampleGet,
                          fForceTransformAll);
        if (hr != WMA_OK){
            pau->aupfnReconstruct = auReconstruct;
            return auReconstruct(pau,piOutput,pcSampleGet,fForceTransformAll); 
        }
    }
    return hr;
}

/////////////////////////////////////////////////////////////////////////////////////
// Recon Mono - X86 optimized version
// This function achieves its speedup by unrolling the loop so that the 1 out of 16
// brantch is removed and the same functionality is done with the loop structure.
// It also uses the sin tables instead of doing the recursion 
/////////////////////////////////////////////////////////////////////////////////////

WMARESULT auReconMono_X86 ( CAudioObject* pau, 
                        PerChannelInfo* ppcinfo, 
                        I16* piOutput, 
                        I16* pcSampleGet,
                        Bool fForceTransformAll)
{
    //all starting from the begining of this frame
    //official starting of a subfrm (including leading zeros)
    //case 1 (Large -> Small):  starts later than what's been reconstructed w/o leading zeros
    //                          in this case dst should be skiped (this may mean memcpy from prev)
    //                          src and coef are fine
    //                          
    //case 2 (Small -> Large):  starts earler than what's been reconstructed but all leading zeros
    //                          in this case dst 

    CoefType *piCoef;   //floating vlaue = (float)*piCoef/(1<<5)  assuming TRANSFORM_FRACT_BITS = 5
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    I32 iResult;
    CoefType cfResult;
    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;
    BP2Type bp2Sin  = *((BP2Type*)(&ppcinfo->m_iSin));
    BP2Type bp2Cos  = *((BP2Type*)(&ppcinfo->m_iCos));
    BP2Type bp2Sin1 = *((BP2Type*)(&ppcinfo->m_iSin1));
    BP2Type bp2Cos1 = *((BP2Type*)(&ppcinfo->m_iCos1));
    BP2Type bp2Step = *((BP2Type*)(&ppcinfo->m_iStep));
    float *piSin = pau->m_piSinForRecon;

#ifndef PREVOUTPUT_16BITS
    U16    *piSrcSign;
    I32     uiSignbitData;
    U32     uiSignbitCount;
#endif  // PREVOUTPUT_16BITS
#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

    //persistent states
    i = ppcinfo->m_iCurrGetPCM_SubFrame;       //current position in a frame
    piCoef = (CoefType*)ppcinfo->m_rgiCoefReconCurr; 
    piSrc = ppcinfo->m_piPrevOutputCurr;

    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(ppcinfo->m_piPrevOutput, piSrc, ppcinfo->m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)
    ASSERT_SIGNBIT_POINTER(piSrcSign);

    if (i < cSubFrameSampleQuad) 
    {
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);
            for (iLoopCnt = 0; iLoopCnt < iLoopBy16; iLoopCnt++)    {
                uiSignbitData<<=16;

                COMBINE_SIGNBIT_FAST
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                uiSignbitData = *piSrcSign++;                                              
            }
            uiSignbitCount = 16;                                                     
            if (iLoopBy1) return !WMA_OK;

/*            for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++, (*pcSampleGet)++)    {

                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);

                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );

                piSrc += cChan;
                piDst += cChan;
            }   */

        }
    }
    if (i < iCoefRecurQ2)  
    {
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);
            for (iLoopCnt = 0;iLoopCnt < iLoopBy16;iLoopCnt++)    {
                uiSignbitData<<=16;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                COMBINE_SIGNBIT_FAST                                                         
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc += cChan;
                piDst += cChan;

                uiSignbitData = *piSrcSign++;                                              
            }
            uiSignbitCount = 16;                                                     
            if (iLoopBy1) return !WMA_OK;
/*            for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++,(*pcSampleGet)++)    {

                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);

                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );

                piSrc += cChan;
                piDst += cChan;
            }  */
        }
    }
    if (i < iCoefRecurQ3) 
    {
        iSegEnd = min (iCoefRecurQ3, iEnd);
        for (; i < iSegEnd; i++,(*pcSampleGet)++)    {
            cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            piDst += cChan;
        }
    }

    SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
    if (pau->m_cChannel < 2 || ((pau->m_cChannel == 2) && (ppcinfo == &(pau->m_rgpcinfo[1]))))
        pau->m_piSinForRecon = piSin;

    ppcinfo->m_rgiCoefReconCurr = (Int*)piCoef;
    ppcinfo->m_iCurrGetPCM_SubFrame = (I16)i;
    ppcinfo->m_piPrevOutputCurr = piSrc;
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}


#if 0
//auReconStereo_X86 has been removed from build as the saturation code does not work with out the
//extra checking in ROUNDF_TO_S below which would cause it to be slow. I left it in the codebase
//as it serves as a good model for other reconStereo's for other platforms.

INLINE I16 FASTCALL ROUNDF_TO_S(float f) {
    I16 intval;
    if (f > 32767) f = 32767;
    if (f < -32767) f= -32767;
    _asm {
        fld         f
        fistp       word ptr [intval]
    }
    return intval;
}

WMARESULT auReconStereo_X86 (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll)
{
    Int overflow = 0;
    Float *piCoef2; 
    Float *piCoef;  
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    I32 iResult;
    Float cfResult;

    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;
    float *piSin = pau->m_piSinForRecon;

    U16    *piSrcSign;
    I32     uiSignbitData;
    U32     uiSignbitCount;
    U16    *piSrcSign2;
    I32     uiSignbitData2;
    U32     uiSignbitCount2;

#ifdef PREVOUTPUT_16BITS
    assert(0);
#endif// PREVOUTPUT_16BITS

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

    //persistent states
    i = pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame;       //current position in a frame
    assert(i==pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame);
    piCoef = (float*)pau->m_rgpcinfo[0].m_rgiCoefReconCurr; 
    piCoef2 = (float*)(pau->m_rgpcinfo[1].m_rgiCoefReconCurr); 

    piSrc = pau->m_rgpcinfo[0].m_piPrevOutputCurr;
    assert((piSrc+1) == pau->m_rgpcinfo[1].m_piPrevOutputCurr);
    
    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[0].m_piPrevOutput,(piSrc+0), pau->m_rgpcinfo[0].m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[1].m_piPrevOutput,(piSrc+1), pau->m_rgpcinfo[1].m_piPrevOutputSign, piSrcSign2, 
        uiSignbitData2, uiSignbitCount2, cChan)

    ASSERT_SIGNBIT_POINTER(piSrcSign);

    if (i < cSubFrameSampleQuad) 
    {
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);

            for (iLoopCnt = 0; iLoopCnt < iLoopBy16; iLoopCnt++)    {
                uiSignbitData<<=16;
                uiSignbitData2<<=16;

#               define RECONSTRUCT2                                                 \
                iResult = (((uiSignbitData)>>15) & 0xFFFF0000) | (U16)*piSrc;       \
                uiSignbitData <<= 1;                                                \
                cfResult = *piSin * *piCoef + iResult;                              \
                if ((*piDst = ROUNDF_TO_S(cfResult)) == 0x8000) overflow=1;         \
                iResult = (((uiSignbitData2)>>15) & 0xFFFF0000) | (U16)piSrc[1];    \
                uiSignbitData2 <<= 1;                                               \
                cfResult = *piSin * *piCoef2 + iResult;                             \
                if ((piDst[1] = ROUNDF_TO_S(cfResult)) == 0x8000) overflow=1;       \
                piSin++;                                                            \
                piCoef++;                                                           \
                piCoef2++;                                                          \
                piSrc += cChan;                                                     \
                piDst += cChan;     

                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2

#               undef RECONSTRUCT2          

                uiSignbitData = *piSrcSign++;                                              
                uiSignbitData2 = *piSrcSign2++;                                              
            }
            uiSignbitCount = 16;                                                     
        }
    }

    if (i < iCoefRecurQ2)  
    {
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            U16 *piDstCopy = piDst;
            Int iLoopBy16,iLoopBy1,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            for (iLoopCnt = 0;iLoopCnt < iLoopBy16;iLoopCnt++, i+=16,(*pcSampleGet)+=16)    {
                uiSignbitData<<=16;
                uiSignbitData2<<=16;

#               define RECONSTRUCT2                                                   \
                piCoef--;                                                             \
                piCoef2--;                                                            \
                iResult = (((uiSignbitData)>>15) & 0xFFFF0000) | (U16)*piSrc;         \
                uiSignbitData <<= 1;                                                  \
                cfResult = *piSin * *piCoef + iResult;                                \
                if ((*piDst = ROUNDF_TO_S(cfResult))==0x8000) overflow=1;             \
                iResult = (((uiSignbitData2)>>15) & 0xFFFF0000) | (U16)piSrc[1];      \
                uiSignbitData2 <<= 1;                                                 \
                cfResult = *piSin * *piCoef2 + iResult;                               \
                if ((piDst[1] = ROUNDF_TO_S(cfResult))==0x8000) overflow=1;           \
                piSin++;                                                              \
                piSrc += cChan;                                                       \
                piDst += cChan;     

                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
                RECONSTRUCT2
#               undef RECONSTRUCT2          

                uiSignbitData = *piSrcSign++;                                              
                uiSignbitData2 = *piSrcSign2++;                                              
            }
            uiSignbitCount = 16;                                                     
            uiSignbitCount2 = 16;                                                     
        }
    }
    if (i < iCoefRecurQ3) 
    {
        iSegEnd = min (iCoefRecurQ3, iEnd);
        for (; i < iSegEnd; i++,(*pcSampleGet)++)    {
            cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            cfResult = INT_FROM_COEF( *--piCoef2 );
            ROUND_SATURATE_STORE( (piDst+1), cfResult, SHRT_MIN, SHRT_MAX, iResult );
            piDst += cChan;
        }
    }

    if (overflow){
        Int iChannel;
        for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
        {
            //hr = 
            auReconMono_X86 (pau, 
                              pau->m_rgpcinfo + iChannel, 
                              piOutput + iChannel,  
                              (I16*)pcSampleGet,
                              fForceTransformAll);
        }
    } else {
        SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
        pau->m_rgpcinfo[0].m_rgiCoefReconCurr = (Int*)piCoef;
        pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame = (I16)i;
        pau->m_rgpcinfo[0].m_piPrevOutputCurr = piSrc;


        SAVE_SIGNBIT_STATE(piSrcSign2,uiSignbitData2);
        pau->m_rgpcinfo[1].m_rgiCoefReconCurr = (Int*)piCoef2;
        pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame = (I16)i;
        pau->m_rgpcinfo[1].m_piPrevOutputCurr = piSrc+1;

        pau->m_piSinForRecon = piSin;
    }         
    return WMA_OK;
}
#endif //if 0

////////////////////////////////////////////////////////////////////////////////////////
// Recon Stereo MMX
// This function improves on reconMono_X86 by doing both channels at once and using MMX
// to do the calculations 4 at a time for the integer part and doing the floating point part
// in a seperate loop. 
// If a non divisable by 16 frame is encounterd the function turns itself off and 
// calls the safer function 
////////////////////////////////////////////////////////////////////////////////////////

WMARESULT auReconStereo_MMX (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll)
{
    static const __int64 x0000FFFF0000FFFF = 0x0000FFFF0000FFFF;
    static const __int64 x8000800080008000 = 0x8000800080008000;
    Int tempBuf[2048];
    Int *tBuf;

    Float *piCoef2; 
    Float *piCoef;  
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    I32 iResult;
    Float cfResult;

    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;
    float *piSin = pau->m_piSinForRecon;

    U16    *piSrcSign;
    I32     uiSignbitData;
    U32     uiSignbitCount;
    U16    *piSrcSign2;
    I32     uiSignbitData2;
    U32     uiSignbitCount2;

#ifdef PREVOUTPUT_16BITS
    assert(0);
#endif// PREVOUTPUT_16BITS

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

    //persistent states
    i = pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame;       //current position in a frame
    assert(i==pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame);
    piCoef = (float*)pau->m_rgpcinfo[0].m_rgiCoefReconCurr; 
    piCoef2 = (float*)(pau->m_rgpcinfo[1].m_rgiCoefReconCurr); 

    piSrc = pau->m_rgpcinfo[0].m_piPrevOutputCurr;
    assert((piSrc+1) == pau->m_rgpcinfo[1].m_piPrevOutputCurr);
    
    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[0].m_piPrevOutput,(piSrc+0), pau->m_rgpcinfo[0].m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[1].m_piPrevOutput,(piSrc+1), pau->m_rgpcinfo[1].m_piPrevOutputSign, piSrcSign2, 
        uiSignbitData2, uiSignbitCount2, cChan)

    ASSERT_SIGNBIT_POINTER(piSrcSign);

    if (i < cSubFrameSampleQuad) 
    {
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1;//,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);
            assert(iLoopBy1==0);
            tBuf = tempBuf;

            _asm{
            
            mov         esi,piSin
            mov         ecx,piCoef
            mov         edx,piCoef2
            lea         ebx,tempBuf
            mov         eax,iLoopBy16
            shl         eax,4
            Loop1F:

                fld         dword Ptr [esi]         //Load Sin value
                add         esi,4                   //piSin++
                fld         dword Ptr [ecx]         //Load Coefficent from channel 1
                fmul        st(0),st(1)             //Multiply it with Sin value
                add         ecx,4                   //piCoef++
                fld         dword Ptr [edx]         //Load Coefficent from channel 2
                fmulp       st(2),st(0)             //Multiply it with Sin value
                fistp       dword Ptr [ebx]         //Store as 32bit int
                add         edx,4                   //piCoef2++
                fistp       dword Ptr [ebx+4]       //Store as 32bit int
                add         ebx,8                   //bump temp buffer ptr

            dec         eax
            jnz         Loop1F
            mov         piSin,esi
            mov         piCoef,ecx
            mov         piCoef2,edx
   

            mov         ebx,piSrcSign
            mov         ecx,piSrcSign2
            mov         esi,piSrc
            mov         edx,tBuf 
            mov         edi,piDst

            movd        mm0,uiSignbitData    
            movd        mm1,uiSignbitData2

            mov         eax,iLoopBy16
            Loop1M:    
                    
                punpcklwd   mm0,mm1
                    
#               define RECONSTRUCT4(x) _asm {                                            \
_asm            movq        mm1,mm0                                                      \
_asm            psraw       mm1,15             /*sine extend sine bit data*/             \
_asm            movd        mm2,[esi+(x*8)]    /*load 2 16bit data elements*/            \
_asm            punpcklwd   mm2,mm1            /*merge with sign bit for 2 32bit values*/\
_asm            movq        mm1,[edx+(x*16)]   /*get 2 precalculated coef * sin */       \
_asm            paddd       mm1,mm2            /*add the 2 vectors*/                     \
_asm            pslld       mm0,1              /*shift sine bit data for next time*/     \
_asm            movq        mm3,mm0            /*repeat process for 2 more elements*/    \
_asm            psraw       mm3,15                                                       \
_asm            movd        mm4,[esi+(x*8)+4]                                            \
_asm            punpcklwd   mm4,mm3                                                      \
_asm            movq        mm3,[edx+(x*16)+8]                                           \
_asm            paddd       mm3,mm4                                                      \
_asm            packssdw    mm1,mm3           /*merge and saturate 2 sets of 2*/         \
_asm            movq        [edi+(x*8)],mm1   /*store 4 reconstructed samples*/          \
_asm            pslld       mm0,1             /*shift sine bit data for next time*/      }

                RECONSTRUCT4(0)
                RECONSTRUCT4(1)
                RECONSTRUCT4(2)
                RECONSTRUCT4(3)
                RECONSTRUCT4(4)
                RECONSTRUCT4(5)
                RECONSTRUCT4(6)
                RECONSTRUCT4(7)
                    
                add         esi,32*2
                add         edx,32*4
                add         edi,32*2

                movd        mm0,[ebx]
                movd        mm1,[ecx]

                add         ebx,2
                add         ecx,2
                

            dec         eax
            jnz         Loop1M

            mov         piSrc,esi
            mov         tBuf,edx
            mov         piDst,edi

            movd        uiSignbitData,mm0
            movd        uiSignbitData2,mm1
            mov         piSrcSign,ebx
            mov         piSrcSign2,ecx

            emms
            }
            uiSignbitCount  = 16;
            uiSignbitCount2 = 16;
            if (iLoopBy1) {
                pau->aupfnReconstruct = auReconstruct;
                return auReconstruct(pau,piOutput,pcSampleGet,fForceTransformAll); 
            }
/*            for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++, (*pcSampleGet)++)    {
                //Channel 1
                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc++;
                piDst++;

                //Channel 2
                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData2,
                    uiSignbitCount2);
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*piCoef2++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc++;
                piDst++;
            }  */
        }
    }

    if (i < iCoefRecurQ2)  
    {
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1;//,iLoopCnt;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            assert(iLoopBy1==0);
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);
            tBuf = tempBuf;
            _asm{
            mov         esi,piSin
            mov         ecx,piCoef
            mov         edx,piCoef2
            mov         ebx,tBuf
            mov         eax,iLoopBy16
            shl         eax,4
            Loop2F:

                fld         dword Ptr [esi]         //Load Sin value
                add         esi,4                   //piSin++
                fld         dword Ptr [ecx-4]       //Load Coefficent from channel 1
                fmul        st(0),st(1)             //Multiply it with Sin value
                sub         ecx,4                   //piCoef--
                fld         dword Ptr [edx-4]       //Load Coefficent from channel 2
                fmulp       st(2),st(0)             //Multiply it with Sin value
                fistp       dword Ptr [ebx]         //Store as 32bit int
                sub         edx,4                   //piCoef--
                fistp       dword Ptr [ebx+4]       //Store as 32bit int
                add         ebx,8                   //bump temp buffer ptr

            dec         eax
            jnz         Loop2F
            mov         piSin,esi
            mov         piCoef,ecx
            mov         piCoef2,edx
                
            mov         ebx,piSrcSign
            mov         ecx,piSrcSign2
            mov         esi,piSrc
            mov         edx,tBuf 
            mov         edi,piDst

            movd        mm0,uiSignbitData    
            movd        mm1,uiSignbitData2

            mov         eax,iLoopBy16
            Loop2M:    
                    
                punpcklwd   mm0,mm1
                    
                RECONSTRUCT4(0)
                RECONSTRUCT4(1)
                RECONSTRUCT4(2)
                RECONSTRUCT4(3)
                RECONSTRUCT4(4)
                RECONSTRUCT4(5)
                RECONSTRUCT4(6)
                RECONSTRUCT4(7)
                    
                add         esi,32*2
                add         edx,32*4
                add         edi,32*2

                movd        mm0,[ebx]
                movd        mm1,[ecx]

                add         ebx,2
                add         ecx,2
                

            dec         eax
            jnz         Loop2M

            mov         piSrc,esi
            mov         tBuf,edx
            mov         piDst,edi

            movd        uiSignbitData,mm0
            movd        uiSignbitData2,mm1
            mov         piSrcSign,ebx
            mov         piSrcSign2,ecx

            emms
#           undef RECONSTRUCT4
            }
            uiSignbitCount  = 16;
            uiSignbitCount2 = 16;
            if (iLoopBy1) {
                pau->aupfnReconstruct = auReconstruct;
                return auReconstruct(pau,piOutput,pcSampleGet,fForceTransformAll); 
            }

/*            for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++, (*pcSampleGet)++)    {
                //Channel 1
                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc++;
                piDst++;

                //Channel 2
                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData2,
                    uiSignbitCount2);
                cfResult = INT_FROM_COEF( MULT_BP2(*piSin++,*--piCoef2) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
                piSrc++;
                piDst++;
            }   */
        }
    }
    if (i < iCoefRecurQ3) 
    {
        iSegEnd = min (iCoefRecurQ3, iEnd);

        for (; i < iSegEnd; i++,(*pcSampleGet)++)    {
            cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            cfResult = INT_FROM_COEF( *--piCoef2 );
            ROUND_SATURATE_STORE( (piDst+1), cfResult, SHRT_MIN, SHRT_MAX, iResult );
            piDst += cChan;
        }
    }

    SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
    pau->m_rgpcinfo[0].m_rgiCoefReconCurr = (Int*)piCoef;
    pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame = (I16)i;
    pau->m_rgpcinfo[0].m_piPrevOutputCurr = piSrc;


    SAVE_SIGNBIT_STATE(piSrcSign2,uiSignbitData2);
    pau->m_rgpcinfo[1].m_rgiCoefReconCurr = (Int*)piCoef2;
    pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame = (I16)i;//= (I16)i;
    pau->m_rgpcinfo[1].m_piPrevOutputCurr = piSrc+1;

    pau->m_piSinForRecon = piSin;

#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Recon Stereo SIMD FP
// This function improves on auReconStereo_MMX by doing the floating point using intels
// vector floating point instructions and removing the floating point loop and intermixing
// it with the int/MMX loop. It is otherwize similar to auReconStereo_MMX
//////////////////////////////////////////////////////////////////////////////////////////////

WMARESULT auReconStereo_SIMDFP (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll)
{
    Int overflow = 0;
    Float *piCoef2; 
    Float *piCoef;  
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    I32 iResult;

    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;
    float *piSin = pau->m_piSinForRecon;

    U16    *piSrcSign;
    I32     uiSignbitData;
    U32     uiSignbitCount;
    U16    *piSrcSign2;
    I32     uiSignbitData2;
    U32     uiSignbitCount2;

#ifdef PREVOUTPUT_16BITS
    assert(0);
#endif// PREVOUTPUT_16BITS

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

    //persistent states
    i = pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame;       //current position in a frame
    assert(i==pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame);
    piCoef = (float*)pau->m_rgpcinfo[0].m_rgiCoefReconCurr; 
    piCoef2 = (float*)(pau->m_rgpcinfo[1].m_rgiCoefReconCurr); 

    piSrc = pau->m_rgpcinfo[0].m_piPrevOutputCurr;
    assert((piSrc+1) == pau->m_rgpcinfo[1].m_piPrevOutputCurr);
    
    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[0].m_piPrevOutput,(piSrc+0), pau->m_rgpcinfo[0].m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)

    CALC_SIGN_OFFSETS(pau->m_rgpcinfo[1].m_piPrevOutput,(piSrc+1), pau->m_rgpcinfo[1].m_piPrevOutputSign, piSrcSign2, 
        uiSignbitData2, uiSignbitCount2, cChan)

    ASSERT_SIGNBIT_POINTER(piSrcSign);

    if (i < cSubFrameSampleQuad) 
    {
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            Int iLoopBy16,iLoopBy1;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);

            _asm {
            mov         ebx,piSin
            mov         ecx,piCoef
            mov         edx,piCoef2
            mov         esi,piSrc
            mov         edi,piDst
            
            movd        mm0,uiSignbitData    
            movd        mm1,uiSignbitData2

            mov         eax,iLoopBy16
            Loop1:

                punpcklwd   mm0,mm1
                                           //Note:The indentation of these comments tries
                                           //     to show logical paths since the instructions 
                                           //     have been scheduled out of logical paths

#define RECONSTRUCT8(x) _asm {                                                                                           \
_asm            movups      xmm3_ebx          /*Load 4 Sin values from table*/                                           \
_asm            movq        mm5,mm0               /*copy sign bit data*/                                                 \
_asm            add         ebx,4*4       /*Bump ptr*/                                                                   \
_asm            movaps      xmm0_xmm3         /*copy Sin values*/                                                        \
_asm            psraw       mm5,15                /*sine extend sine bit data*/                                          \
_asm            shufps      (xmm0_xmm3,0x50)  /*broadcast 1st and 2nd Sin values (a=a,b=a,c=b,d=b) */                    \
_asm            movd        mm2,[esi+(x*16)]      /*load 2 16bit data elements from piSrc*/                              \
_asm            movups      xmm4_ecx          /*Load Coefficents for channel 1*/                                         \
_asm            add         ecx,4*4       /*Bump ptr*/                                                                   \
_asm            punpcklwd   mm2,mm5               /*interleave src with sign bit for 2 32bit values*/                    \
_asm            movaps      xmm1_xmm4         /*make a copy of coefficents for channel 1*/                               \
_asm            pslld       mm0,1                 /*shift sine bit data for next 2*/                                     \
_asm            movups      xmm2_edx          /*Load Coefficents for channel 2*/                                         \
_asm            add         edx,4*4       /*Bump ptr*/                                                                   \
_asm            movq        mm6,mm0               /*copy sign bit data*/                                                 \
_asm            pslld       mm0,1                 /*shift sine bit data for next 2*/                                     \
_asm            unpcklps    xmm1_xmm2         /*Interleve first 2 Coefficents of channel 1 & 2*/                         \
_asm            psraw       mm6,15                /*sine extend sine bit data*/                                          \
_asm            mulps       xmm0_xmm1         /*Multiply coefficents with Sin values*/                                   \
_asm            movd        mm4,[esi+(x*16)+4]    /*load next 2 16bit data elements from piSrc*/                         \
_asm            cvtps2pi    mm1_xmm0          /*convert 2 lower multiply results to rounded integer values*/             \
_asm            punpcklwd   mm4,mm6               /*interleave src with sign bit for 2 32bit values*/                    \
_asm            shufps      (xmm0_xmm0,0xE)   /*move high elements in to low (a=c,b=d,c=a,d=a)*/                         \
_asm            movq        mm7,mm0                   /*copy sign bit data*/                                             \
_asm            pslld       mm0,1                     /*shift sine bit data for next 2*/                                 \
_asm            cvtps2pi    mm3_xmm0          /*convert 2 multiplyed results to rounded integer values*/                 \
_asm            psraw       mm7,15                    /*sine extend sine bit data*/                                      \
_asm            movaps      xmm0_xmm3                      /*copy Sin values*/                                           \
_asm            paddd       mm1,mm2           /*add the 2 vectors*/                                                      \
_asm            paddd       mm3,mm4           /*add the 2 other vectors*/                                                \
_asm            shufps      (xmm0_xmm3,0xFA)               /*broadcast 3rd and 4th Sin Values (a=c,b=c,c=d,d=d)*/        \
_asm            packssdw    mm1,mm3           /*merge and saturate vectors*/                                             \
_asm            movq        mm5,mm0                   /*copy sign bit data*/                                             \
_asm            movaps      xmm1_xmm4                      /*make a copy of coefficents for channel 1*/                  \
_asm            movq        [edi+(x*16)],mm1  /*store 4 reconstructed samples*/                                          \
_asm            unpckhps    xmm1_xmm2                      /*Interleve last 2 Coefficents of channel 1 & 2*/             \
_asm            movd        mm2,[esi+(x*16)+8]        /*load next 2 16bit data elements from piSrc*/                     \
_asm            pslld       mm0,1                     /*shift sine bit data for next 2*/                                 \
_asm            mulps       xmm0_xmm1                      /*Multiply coefficents with Sin values*/                      \
_asm            movd        mm4,[esi+(x*16)+12]       /*load next 2 16bit data elements from piSrc*/                     \
_asm            psraw       mm5,15                    /*sine extend sine bit data*/                                      \
_asm            cvtps2pi    mm1_xmm0                       /*convert 2 lower multiply results to rounded integer values*/\
_asm            punpcklwd   mm2,mm7                   /*merge with sign bit for 2 32bit values*/                         \
_asm            shufps      (xmm0_xmm0,0xE)                /*move high elements in to low (a=c,b=d,c=a,d=a)*/            \
_asm            punpcklwd   mm4,mm5                   /*interleave src with sign bit for 2 32bit values*/                \
_asm            paddd       mm1,mm2                        /*add the 2 vectors*/                                         \
_asm            cvtps2pi    mm3_xmm0                       /*convert 2 multiply results to rounded integer values*/      \
_asm            paddd       mm3,mm4                        /*add the 2 other vectors*/                                   \
_asm            packssdw    mm1,mm3                        /*merge and saturate 2 sets of 2*/                            \
_asm            movq        [edi+(x*16)+8],mm1             /*store 4 reconstructed samples*/                             }

                RECONSTRUCT8(0)
                RECONSTRUCT8(1)
                RECONSTRUCT8(2)
                RECONSTRUCT8(3)
                
                add         esi,2*32
                add         edi,2*32


                mov         iResult,ebx

                mov         ebx,piSrcSign  //uiSignbitData = *piSrcSign++;
                movd        mm0,[ebx]
                pslld       mm0,16
                psrld       mm0,16
                add         ebx,2
                mov         piSrcSign,ebx

                mov         ebx,piSrcSign2 //uiSignbitData2 = *piSrcSign2++;
                movd        mm1,[ebx]
                pslld       mm1,16
                psrld       mm1,16
                add         ebx,2
                mov         piSrcSign2,ebx

                mov         ebx,iResult
 
            dec         eax
            jnz         Loop1

            mov         piSrc,esi
            mov         piDst,edi
            mov         piSin,ebx
            mov         piCoef,ecx
            mov         piCoef2,edx
            movd        uiSignbitData,mm0
            movd        uiSignbitData2,mm1

            emms

#           undef RECONSTRUCT8
            }
            uiSignbitCount = 16;                                                     
            uiSignbitCount2 = 16;                                                     
            if (iLoopBy1) {
                pau->aupfnReconstruct = auReconstruct;
                return auReconstruct(pau,piOutput,pcSampleGet,fForceTransformAll); 
            }
        }
    }

    if (i < iCoefRecurQ2)  
    {
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            U16 *piDstCopy = piDst;
            Int iLoopBy16,iLoopBy1;
            iLoopBy16 = (iSegEnd - i) >> 4;
            iLoopBy1  = (iSegEnd - i) & 0xF;
            (*pcSampleGet) += (I16)(iLoopBy16<<4);
            i += (iLoopBy16<<4);

            _asm {
            mov         ebx,piSin
            mov         ecx,piCoef
            mov         edx,piCoef2
            mov         esi,piSrc
            mov         edi,piDst
            
            movd        mm0,uiSignbitData    
            movd        mm1,uiSignbitData2

            mov         eax,iLoopBy16
            Loop2:

                punpcklwd   mm0,mm1

#define RECONSTRUCT8(x) _asm {                                                                                           \
_asm            movups      xmm3_ebx          /*Load 4 Sin values from table*/                                           \
_asm            movq        mm5,mm0               /*copy sign bit data*/                                                 \
_asm            add         ebx,4*4       /*Bump ptr*/                                                                   \
_asm            movaps      xmm0_xmm3         /*copy Sin values*/                                                        \
_asm            psraw       mm5,15                /*sine extend sine bit data*/                                          \
_asm            shufps      (xmm0_xmm3,0x50)  /*broadcast 1st and 2nd Sin values (a=a,b=a,c=b,d=b) */                    \
_asm            movd        mm2,[esi+(x*16)]      /*load 2 16bit data elements from piSrc*/                              \
_asm            sub         ecx,4*4       /*Bump ptr*/                                                                   \
_asm            movups      xmm4_ecx          /*Load Coefficents for channel 1*/                                         \
_asm            shufps      (xmm4_xmm4,0x1B)                                                                             \
_asm            punpcklwd   mm2,mm5               /*interleave src with sign bit for 2 32bit values*/                    \
_asm            movaps      xmm1_xmm4         /*make a copy of coefficents for channel 1*/                               \
_asm            pslld       mm0,1                 /*shift sine bit data for next 2*/                                     \
_asm            sub         edx,4*4       /*Bump ptr*/                                                                   \
_asm            movups      xmm2_edx          /*Load Coefficents for channel 2*/                                         \
_asm            shufps      (xmm2_xmm2,0x1B)                                                                             \
_asm            movq        mm6,mm0               /*copy sign bit data*/                                                 \
_asm            pslld       mm0,1                 /*shift sine bit data for next 2*/                                     \
_asm            unpcklps    xmm1_xmm2         /*Interleve first 2 Coefficents of channel 1 & 2*/                         \
_asm            psraw       mm6,15                /*sine extend sine bit data*/                                          \
_asm            mulps       xmm0_xmm1         /*Multiply coefficents with Sin values*/                                   \
_asm            movd        mm4,[esi+(x*16)+4]    /*load next 2 16bit data elements from piSrc*/                         \
_asm            cvtps2pi    mm1_xmm0          /*convert 2 lower multiply results to rounded integer values*/             \
_asm            punpcklwd   mm4,mm6               /*interleave src with sign bit for 2 32bit values*/                    \
_asm            shufps      (xmm0_xmm0,0xE)   /*move high elements in to low (a=c,b=d,c=a,d=a)*/                         \
_asm            movq        mm7,mm0                   /*copy sign bit data*/                                             \
_asm            pslld       mm0,1                     /*shift sine bit data for next 2*/                                 \
_asm            cvtps2pi    mm3_xmm0          /*convert 2 multiplyed results to rounded integer values*/                 \
_asm            psraw       mm7,15                    /*sine extend sine bit data*/                                      \
_asm            movaps      xmm0_xmm3                      /*copy Sin values*/                                           \
_asm            paddd       mm1,mm2           /*add the 2 vectors*/                                                      \
_asm            paddd       mm3,mm4           /*add the 2 other vectors*/                                                \
_asm            shufps      (xmm0_xmm3,0xFA)               /*broadcast 3rd and 4th Sin Values (a=c,b=c,c=d,d=d)*/        \
_asm            packssdw    mm1,mm3           /*merge and saturate vectors*/                                             \
_asm            movq        mm5,mm0                   /*copy sign bit data*/                                             \
_asm            movaps      xmm1_xmm4                      /*make a copy of coefficents for channel 1*/                  \
_asm            movq        [edi+(x*16)],mm1  /*store 4 reconstructed samples*/                                          \
_asm            unpckhps    xmm1_xmm2                      /*Interleve last 2 Coefficents of channel 1 & 2*/             \
_asm            movd        mm2,[esi+(x*16)+8]        /*load next 2 16bit data elements from piSrc*/                     \
_asm            pslld       mm0,1                     /*shift sine bit data for next 2*/                                 \
_asm            mulps       xmm0_xmm1                      /*Multiply coefficents with Sin values*/                      \
_asm            movd        mm4,[esi+(x*16)+12]       /*load next 2 16bit data elements from piSrc*/                     \
_asm            psraw       mm5,15                    /*sine extend sine bit data*/                                      \
_asm            cvtps2pi    mm1_xmm0                       /*convert 2 lower multiply results to rounded integer values*/\
_asm            punpcklwd   mm2,mm7                   /*merge with sign bit for 2 32bit values*/                         \
_asm            shufps      (xmm0_xmm0,0xE)                /*move high elements in to low (a=c,b=d,c=a,d=a)*/            \
_asm            punpcklwd   mm4,mm5                   /*interleave src with sign bit for 2 32bit values*/                \
_asm            paddd       mm1,mm2                        /*add the 2 vectors*/                                         \
_asm            cvtps2pi    mm3_xmm0                       /*convert 2 multiply results to rounded integer values*/      \
_asm            paddd       mm3,mm4                        /*add the 2 other vectors*/                                   \
_asm            packssdw    mm1,mm3                        /*merge and saturate 2 sets of 2*/                            \
_asm            movq        [edi+(x*16)+8],mm1             /*store 4 reconstructed samples*/                             }

                RECONSTRUCT8(0)
                RECONSTRUCT8(1)
                RECONSTRUCT8(2)
                RECONSTRUCT8(3)
                
                add         esi,2*32
                add         edi,2*32

                mov         iResult,ebx

                mov         ebx,piSrcSign   //uiSignbitData = *piSrcSign++;
                movd        mm0,[ebx]
                pslld       mm0,16
                psrld       mm0,16
                add         ebx,2
                mov         piSrcSign,ebx

                mov         ebx,piSrcSign2  //uiSignbitData2 = *piSrcSign2++;
                movd        mm1,[ebx]
                pslld       mm1,16
                psrld       mm1,16
                add         ebx,2
                mov         piSrcSign2,ebx

                mov         ebx,iResult
 
            dec         eax
            jnz         Loop2

            mov         piSrc,esi
            mov         piDst,edi
            mov         piSin,ebx
            mov         piCoef,ecx
            mov         piCoef2,edx
            movd        uiSignbitData,mm0
            movd        uiSignbitData2,mm1

            emms

#           undef RECONSTRUCT8

            }
            uiSignbitCount = 16;                                                     
            uiSignbitCount2 = 16;                                                     
            if (iLoopBy1) {
                pau->aupfnReconstruct = auReconstruct;
                return auReconstruct(pau,piOutput,pcSampleGet,fForceTransformAll); 
            }
        }
    }
    if (i < iCoefRecurQ3) 
    {
        Int iLoopBy4,iLoopBy1,iLoopCnt;
        iSegEnd = min (iCoefRecurQ3, iEnd);
        iLoopBy4 = (iSegEnd - i) >> 2;
        iLoopBy1 = (iSegEnd - i) & 0x3;
        (*pcSampleGet) += (I16)(iLoopBy4<<2);
        i += (iLoopBy4<<2);
        if (iLoopBy4 != 0)
        {
            _asm {

            mov         eax,iLoopBy4
            mov         ecx,piCoef
            mov         edx,piCoef2
            mov         edi,piDst
            Loop3:    
                sub         ecx,4*4                 //Bump ptr                                                              
                movups      xmm4_ecx                //Load Coefficents for channel 1
                sub         edx,4*4                 //Bump ptr                                          
                movups      xmm2_edx                //Load Coefficents for channel 2                                        
                cvtps2pi    mm0_xmm4                //convert 2 lower multiply results to rounded integer values            
                cvtps2pi    mm1_xmm2                //convert 2 lower multiply results to rounded integer values           
                shufps      (xmm4_xmm4,0xE)         //move high elements in to low (a=c,b=d,c=a,d=a)              
                shufps      (xmm2_xmm2,0xE)         //move high elements in to low (a=c,b=d,c=a,d=a)                        
                cvtps2pi    mm2_xmm4                //convert 2 lower multiply results to rounded integer values           
                cvtps2pi    mm3_xmm2                //convert 2 lower multiply results to rounded integer values            
                packssdw    mm2,mm3                 //Convert to 16bit and saturate
                pshufw      mm2_mm2 emit 0x8D     //Shuffle (a=b,b=d,c=a,b=c)
                packssdw    mm0,mm1                 //Convert to 16bit and saturate
                pshufw      mm0_mm0 emit 0x8D     //Shuffle (a=b,b=d,c=a,b=c)
                movq        [edi],mm2               //Store 4 16bit words
                movq        [edi+8],mm0             //Store 4 16bit words
                add         edi,2*8                 //Bump Ptr
            dec         eax
            jnz         Loop3
            mov         piCoef,ecx
            mov         piCoef2,edx
            mov         piDst,edi

            emms

            }
        }
        //if (iLoopBy1) _asm emms
        for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++,(*pcSampleGet)++)    {
            Float cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            cfResult = INT_FROM_COEF( *--piCoef2 );
            ROUND_SATURATE_STORE( (piDst+1), cfResult, SHRT_MIN, SHRT_MAX, iResult );
            piDst += cChan;
        }
    }
    //_asm emms

    SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
    pau->m_rgpcinfo[0].m_rgiCoefReconCurr = (Int*)piCoef;
    pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame = (I16)i;
    pau->m_rgpcinfo[0].m_piPrevOutputCurr = piSrc;


    SAVE_SIGNBIT_STATE(piSrcSign2,uiSignbitData2);
    pau->m_rgpcinfo[1].m_rgiCoefReconCurr = (Int*)piCoef2;
    pau->m_rgpcinfo[1].m_iCurrGetPCM_SubFrame = (I16)i;
    pau->m_rgpcinfo[1].m_piPrevOutputCurr = piSrc+1;

    pau->m_piSinForRecon = piSin;

#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}

#endif //BUILD_INTEGER
#endif //USE_SIN_COS_TABLES

#endif // defined(WMA_TARGET_X86)
