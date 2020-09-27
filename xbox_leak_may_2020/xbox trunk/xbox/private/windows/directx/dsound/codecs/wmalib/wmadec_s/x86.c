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

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include "../common/macros.h" // This will define WMA_TARGET_X86 (or not)
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
#include "..\x86\opcodes.h"
#include "macros.h"
#include "AutoProfile.h"
#include "..\..\..\dsound\dsndver.h"


#ifdef ENCODER
#include "lpfilter.h"
#include "lpfmacro.h"
#include "MSAudioEnc.h"

#if !defined(QUANTIZER_CLIP_LIMIT)
// i.e. used in v7 mode. In v8 mode, the value is 1.0F
#define QUANTIZER_CLIP_LIMIT 1.25F
#endif
#endif

#pragma warning(disable:4730)

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE)

#ifdef INTELFFT_FOR_CE
// For some reason, somebody has a #define __stdcall __cdecl somewhere.
// Get rid of this, because it's causing us linker errors.
#ifdef __stdcall
#undef __stdcall
#endif
#endif  // INTELFFT_FOR_CE

#if !defined(_Embedded_x86)

#include "..\x86\nsp.h"
#include "..\x86\nspfft.h"


// From userdll.c
Bool MyInitNSP(I32 fdwReason);

// This function initializes for prvFFT4DCT_INTEL as well as prvInvFftComplexConjugate_INTEL

WMARESULT auInitIntelFFT(const CAudioObject *pau,
                         const INTELFFT_INITCODE iCode)
{
    const int c_iMinFrameSizeLOG = LOG2(pau->m_cSubband / pau->m_iMaxSubFrameDiv);
    const int c_iMaxFrameSizeLOG = LOG2(pau->m_cSubband);
    WMARESULT   wmaResult = WMA_OK;

    int     i;
    Bool    fResult;

    fResult = MyInitNSP(iCode);
    if (WMAB_FALSE == fResult)
    {
        wmaResult = WMA_E_NOTSUPPORTED;
        goto exit;
    }

    for (i = c_iMinFrameSizeLOG; i < c_iMaxFrameSizeLOG; i++)
        nspcFft(NULL, i-1, NSP_Init); // Remember FFT size is frame size / 2, so subtract 1
    
#if defined (PEAQ_MASK) && defined (ENCODER)
    for (i = c_iMinFrameSizeLOG; i <= c_iMaxFrameSizeLOG; i++)
        nspsCcsFft(NULL, i+1, NSP_Init); // Remember FFT size is 2 * frame size, so add 1
#endif // PEAQ_MASK && ENCODER
    
exit:
    return wmaResult;
}



WMARESULT auFreeIntelFFT(CAudioObject *pau,
                         const INTELFFT_INITCODE iCode)
{
    Bool fResult;

    nspcFft(NULL, 0, NSP_Free);

    fResult = MyInitNSP(iCode);
    assert(fResult);

    return WMA_OK;
}


void prvFFT4DCT_INTEL(CoefType data[], Int nLog2np)
{
    nspcFft((SCplx *)data, nLog2np, NSP_Forw);
}


#if defined (PEAQ_MASK) && defined (ENCODER)
Void prvInvFftComplexConjugate_INTEL(Float rgfltData[], Int nLog2np)
{
    nspsCcsFft((float *)rgfltData, nLog2np, NSP_Inv);
}
#endif // PEAQ_MASK && ENCODER
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE)


#endif // if !defined(EMbedded_x86)


// ***************************************************************************
// CPU Detect MMX Functions
// ***************************************************************************

#ifdef _SCRUNCH_INLINE_ASM
Bool g_SupportMMX(void)
{
    static Bool g_bSupportMMX;
    static Int g_iSupportMMXCount = 0;

    Int cpuff;

#ifdef DISABLE_OPT
	return WMAB_FALSE;
#endif	// DISABLE_OPT

    g_iSupportMMXCount++;
    if (g_iSupportMMXCount != 1)
        return g_bSupportMMX;

    cpuff=0;
    _asm {
        // Push general purpose register and pop them out before finish.
        // Need to do push-pop general purpose register because 
        // compiler doesn't know the existence of CPUID which will affect
        // eax, ebx, ecx, edx.  
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd                  // Get original EFLAGS
        pop     eax
        mov     ecx,eax
        xor     eax,200000h     // Flip ID bit in EFLAGS
        push    eax             // Save new EFLAGS value on stack
        popfd                   // Replace current EFLAGS value
        pushfd                  // Get new EFLAGS
        pop     eax             // Store new EFLAGS in EAX
        xor     eax,ecx         // Can not toggle ID bit,
        // CPUID opcode is not available, So is MMX 
        jz      noSupport       // Processor=80486  

        xor     eax,eax         // Set up for CPUID instruction
        CPU_ID                  // Get and save vendor ID
        cmp     eax,1           // Make sure 1 is valid input 
        jl      noSupport       // If not, jump to end
        xor     eax,eax
        inc     eax
        CPU_ID                  // Get family/model/stepping/
        and     edx,0x00800000  //   features
        mov     cpuff,edx
noSupport:
        pop edx
        pop ecx
        pop ebx
        pop eax
    }
    g_bSupportMMX = (cpuff != 0);
    return g_bSupportMMX;
} // g_SupportMMX



Bool g_SupportKNI(void)
{
    static Int g_iSupportKNIValue = -1;

    Int cpuff;

#ifdef DISABLE_OPT
	return WMAB_FALSE;
#endif	// DISABLE_OPT

    if (g_iSupportKNIValue > -1) return (Bool)g_iSupportKNIValue;

    cpuff=0;
    _asm {
        // Push general purpose register and pop them out before finish.
        // Need to do push-pop general purpose register because 
        // compiler doesn't know the existence of CPUID which will affect
        // eax, ebx, ecx, edx.  
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd                  // Get original EFLAGS
        pop     eax
        mov     ecx,eax
        xor     eax,200000h     // Flip ID bit in EFLAGS
        push    eax             // Save new EFLAGS value on stack
        popfd                   // Replace current EFLAGS value
        pushfd                  // Get new EFLAGS
        pop     eax             // Store new EFLAGS in EAX
        xor     eax,ecx         // Can not toggle ID bit,
        // CPUID opcode is not available, So is MMX 
        jz      noSupport       // Processor=80486  

        xor     eax,eax         // Set up for CPUID instruction
        CPU_ID                  // Get and save vendor ID
        cmp     eax,1           // Make sure 1 is valid input 
        jl      noSupport       // If not, jump to end
		push	ebx
		push	edx
		push	ecx
		xor		ebx,0x756E6547
		xor		edx,0x49656E69
		xor		ecx,0x6C65746E
        xor     eax,eax
		or		ebx,edx
		or		ebx,ecx
		or		eax,ebx
		pop 	ecx
		pop 	edx
		pop 	ebx
		cmp		eax,0
		jz		INTEL
		push	ebx
		push	edx
		push	ecx
		xor		ebx,0x68747541
		xor		edx,0x69746E65
		xor		ecx,0x444D4163
        xor     eax,eax
		or		ebx,edx
		or		ebx,ecx
		or		eax,ebx
		pop 	ecx
		pop 	edx
		pop 	ebx
		cmp		eax,0
		jz		AMD
		jmp		noSupport
INTEL:
        xor     eax,eax
        inc     eax
        CPU_ID                  // Get family/model/stepping/
		// check bit 25 for KNI support
        and     edx,0x02000000  //   features
        mov     cpuff,edx
		jmp		noSupport
AMD:
        xor     eax,eax
        inc     eax
        CPU_ID                  // Get family/model/stepping/
		// check bit 25 for KNI support
        cmp     eax,0x0610  //   features
		jz		K7
        mov     cpuff,0
		jmp		noSupport
K7:
        mov     cpuff,1
noSupport:
        pop edx
        pop ecx
        pop ebx
        pop eax
    }
    g_iSupportKNIValue = (cpuff != 0);
    return (Bool)g_iSupportKNIValue;
} // g_SupportKNI



///////////////////////////////////////////////////////////////////////////////////////////
// Support SMPF FP - X86 vector floating point detection
//
// This function detects the presence of the vector floating point instructions
// as well as OS support it by attempting a harmless vector floating point instruction
// and catching the exception which is generated if the instructions are not supported
//
// A feture which is currently turned off attempts to detect if the emulation is enabled
// by timing the instruction. This is why the RDTSC instruction is used
//
///////////////////////////////////////////////////////////////////////////////////////////
Bool g_SupportSIMD_FP(void)
{
    static Int g_iSupportSIMD_FPValue = -1;

    I64 start;
    I64 end;
    Bool supported;

#ifdef DISABLE_OPT
	return WMAB_FALSE;
#endif	// DISABLE_OPT

    if (!g_SupportKNI()) return WMAB_FALSE;
    if (g_iSupportSIMD_FPValue > -1) return (Bool)g_iSupportSIMD_FPValue;

    supported = WMAB_TRUE;
    _try {
        _asm {
            lea     edi,start        //Check time stamp. Time operation to make sure emulator is not running
            RDTSC
            mov     [edi],eax
            mov     [edi+4],edx
            movaps  xmm1_xmm1
            lea     edi,end 
            RDTSC
            mov     [edi],eax
            mov     [edi+4],edx
        
        }
    } _except (1) {
        supported = WMAB_FALSE;
    }
//    if ((end-start) > 60) supported = WMAB_FALSE; //If it took a lot of clocks to execute the instruction then emulator is running
    g_iSupportSIMD_FPValue = (Int)supported;
    return supported;
} // g_SupportSIMD_FP


Bool g_SupportCMOV(void)
{
    static Bool g_bSupportCMOV = -1;
    Int cpuff;

#ifdef DISABLE_OPT
	return WMAB_FALSE;
#endif	// DISABLE_OPT

    if (g_bSupportCMOV != -1) return g_bSupportCMOV;

    cpuff=0;
    _asm {
        // Push general purpose register and pop them out before finish.
        // Need to do push-pop general purpose register because 
        // compiler doesn't know the existence of CPUID which will affect
        // eax, ebx, ecx, edx.  
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd                  // Get original EFLAGS
        pop     eax
        mov     ecx,eax
        xor     eax,200000h     // Flip ID bit in EFLAGS
        push    eax             // Save new EFLAGS value on stack
        popfd                   // Replace current EFLAGS value
        pushfd                  // Get new EFLAGS
        pop     eax             // Store new EFLAGS in EAX
        xor     eax,ecx         // Can not toggle ID bit,
        // CPUID opcode is not available, So does not support CMOV 
        jz      noSupport       // Processor=80486  

        mov     eax,1           // Set up for CPUID instruction	for version and feture information
        CPU_ID                  // Get features
        and     edx,0x00008000  // Mask out all but bit 15 for CMOV detection  
        mov     cpuff,edx
noSupport:
        pop edx
        pop ecx
        pop ebx
        pop eax
    }
    g_bSupportCMOV = (cpuff != 0);
    return g_bSupportCMOV;
} // g_SupportCMOV
#endif // _SCRUNCH_INLINE_ASM


#ifdef ENCODER
// ***************************************************************************
// Quantization X86
// ***************************************************************************

//////////////////////////////////////////////////////////////////////////////////////
// Quantize Spectrum Mono - X86 version
// This function takes advantage of the fact that a floating point compare can be 
// done in the integer unit to avoid exercizing the floating point unit in the most
// common case where a coefficent is either 0 or 1. 
// iZeroThresh is the hex represintation of the floating point number under which 
// all quantized coeffients are zero, iOneThresh is the same but for vales less than 1.
// Basicly the loop optimizes the most common case (0 or 1) at the expence of the less
// common case which needs to do the floating point divide. In this case it is done by
// multiplying with the recripical which is precalculated in fltInverseQuantStep
///////////////////////////////////////////////////////////////////////////////////////
WMARESULT prvQuantizeSpectrumMono_X86(CAudioObjectEncoder *pauenc, PerChannelInfo *pcinfo)	
{
    Float fltInverseQuantStep = 1.0f / pauenc->m_fltQuantStep;

    Float fltZeroThresh = pauenc->m_fltQuantStep / 2;
    Float fltOneThresh  = pauenc->m_fltQuantStep + fltZeroThresh;
    Int iZeroThresh = (*(Int*)&fltZeroThresh) & 0x7fffffff; 
    Int iOneThresh  = (*(Int*)&fltOneThresh)  & 0x7fffffff;
    Int iEscape = 0;
    Int anyNonZero = 0;
    
    assert(WMAB_FALSE); // Unmaintained code! Does not have saturation limits etc.
    _asm{
        mov         ecx, pcinfo               
        mov         esi, [ecx]pcinfo.m_rgfltCoef//Get the source and destination pointers
        mov         edi, [ecx]pcinfo.m_rgiCoefQ

        mov         edx,iZeroThresh             //Get the step size values
        mov         ebx,iOneThresh
        fld         fltInverseQuantStep             
        
        mov         ecx, [ecx]pcinfo.m_cSubbandActual//Get Loop Length
        dec         ecx

        mov         eax,[esi+ecx*4] 

QuantizationLoop:
        and         eax,0x7fffffff              //Check for zero
        cmp         eax,edx
        jge         NotZero
        mov         dword Ptr [edi+ecx*2-2],0   
        mov         eax,[esi+ecx*4-4] 
        dec         ecx
        jns         QuantizationLoop
        jmp         end
NotZero:
        mov         [anyNonZero],1
        cmp         eax,ebx                     //Check for one
        jge         FloatingPointCalc
        mov         eax,[esi+ecx*4]
        sar         eax,15
        or          eax,0x10000
        mov         [edi+ecx*2-2],eax
        mov         eax,[esi+ecx*4-4] 
        dec         ecx
        jns         QuantizationLoop
        jmp         end
FloatingPointCalc:
        fld         [esi+ecx*4]                 //Do full floating point calculateion
        fmul        st(0),st(1)
        fistp       word Ptr [edi+ecx*2]
        movsx       eax,word Ptr [edi+ecx*2]
        cdq
        xor         eax,edx
        sub         eax,edx
        mov         edx,iEscape
        cmp         eax,16
        _asm _emit 0x0F _asm _emit 0x4D _asm _emit 0xD0 //cmovg       eax,edx
        mov         iEscape,edx
        mov         edx,iZeroThresh             //Restore step size value
        mov         eax,[esi+ecx*4-4] 
        dec         ecx
        jns         QuantizationLoop

end:    fstp        st(0)
    }              
    pcinfo->m_rgiCoefQ[-1] = (I16)iEscape;
    pcinfo->m_fAnyNonZeroCoefQ = anyNonZero;
    return WMA_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Quantize (Run Length Code) spectrum
// This function preforms similar functionality to the prvQuantizeSpectrumMono_X86 and 
// does the same integer compare of zero and 1 (see above comment)
// It improves on the basic approach by coding the values as run length codes. This
// is faster as in the most common case (quantized to zero) there is no need to actually 
// store the value and therefore eleinates a lot of bus transctions.
// 
// WARNING!!! The values are stored as 16bit ones complement numbers that is
// 1 = 0x0001 and -1 = 0x8001
// The purpose of storing the numers in this fromat is that the function that uses these
// function starts by taking the absolute value and extracting the sign bit, so the value
// is stored in a format that is more like the format that it will be used in.
// The final output is in the format of (16 bit zero run count) (16 bit ones compement number)
//////////////////////////////////////////////////////////////////////////////////////////////
WMARESULT prvQuantizeRLCSpectrum_X86(CAudioObjectEncoder *pauenc, PerChannelInfo *pcinfo)	
{
    Float fltInverseQuantStep = 1.0f / pauenc->m_fltQuantStep;

    Float fltZeroThresh = pauenc->m_fltQuantStep / 2;
    Float fltOneThresh  = pauenc->m_fltQuantStep + fltZeroThresh;
    Int iZeroThresh = (*(Int*)&fltZeroThresh) & 0x7fffffff; 
    Int iOneThresh  = (*(Int*)&fltOneThresh)  & 0x7fffffff;
    Int iEscape = 0;
    I16 run = 0;
    Int overflow = (Int) (pauenc->pau->m_iMaxEscLevel * QUANTIZER_CLIP_LIMIT);
    Int iMaxEscLevel = pauenc->pau->m_iMaxEscLevel;

    _asm{
        xor         eax,eax
        mov         [esp-20],eax
        mov         eax,overflow
        mov         [esp-16],eax
        mov         eax,iMaxEscLevel
        mov         [esp-8],eax

        mov         ecx, pcinfo               
        mov         esi, [ecx]pcinfo.m_rgfltCoef		//Get the source and destination pointers
        mov         edi, [ecx]pcinfo.m_rgiCoefQRLC

        mov         edx,iZeroThresh             //Get the step size values
        mov         [esp-12],edx
        mov         ebx,iOneThresh
        fld         fltInverseQuantStep             
        
        mov         ecx, [ecx]pcinfo.m_cSubbandActual//Get Loop Length
        lea         esi,[esi+ecx*4]
        neg         ecx

        mov         eax,[esi+ecx*4] 
        mov         [esp-4],ebp
        xor         ebp,ebp

QuantizationLoop:
        and         eax,0x7fffffff              //Check for zero
        cmp         eax,edx
        jge         NotZero
        inc         ebp
        mov         eax,[esi+ecx*4+4] 
        inc         ecx
        jl          QuantizationLoop
        jmp         end
NotZero:
        mov         [esp-20],1 
        cmp         eax,ebx                     //Check for one
        jge         FloatingPointCalc
        mov         eax,[esi+ecx*4]
        and         eax,0x80000000;
        xor         eax,0x80000000;
        or          eax,0x10000
        or          eax,ebp
        mov         [edi],eax
        xor         ebp,ebp
        add         edi,4
        mov         eax,[esi+ecx*4+4]
        inc         ecx
        jl          QuantizationLoop
        jmp         end
FloatingPointCalc:
        fld         [esi+ecx*4]                //Do full floating point calculateion
        fmul        st(0),st(1)
        fistp       word Ptr [edi]
        movsx       eax,word Ptr [edi]         //eax = quantized value 
        mov         edx,eax                  
        and         edx,0x80000000             //Get sign bit in edx
        xor         edx,0x80000000             //and reverse it
        or          ebp,edx                    //save in ebp
        cdq
        xor         eax,edx                    //absolute value of eax
        sub         eax,edx
        mov         edx,[esp-16]
        cmp         eax,edx
        jg          overflow_end
        mov         edx,[esp-8]                //Get copy of edx=pau->m_iMaxEscLevel
        cmp         eax,edx                    //if (eax > edx) eax = edx;
        _asm _emit 0x0F _asm _emit 0x4D _asm _emit 0xC2 //cmovg       eax,edx
        shl         eax,16
        or          eax,ebp
        mov         [edi],eax
        add         edi,4

        mov         edx,[esp-12]             //Restore step size value
        mov         eax,[esi+ecx*4+4] 
        xor         ebp,ebp
        inc         ecx
        jl          QuantizationLoop
        jmp         end

overflow_end:
        mov         ebp,[esp-4]
        mov         ecx,pauenc
        mov         [ecx].m_fQuantOverflow,1


end:    fstp        st(0)
    
        mov         ax,bp
        mov         ebp,[esp-4]
        cmp         ax,0                                                       
        jz          done
        mov         [edi],ax
        add         edi,2
        mov         word Ptr [edi],0
        add         edi,2
done:
        mov         ecx, pcinfo               
        mov         eax, [ecx]pcinfo.m_rgiCoefQRLC
        sub         edi, eax
        shr         edi,1
        mov         [eax-2],di
        mov         eax,[esp-20]
        mov         [ecx]pcinfo.m_fAnyNonZeroCoefQ,eax
              
    }
    return WMA_OK;
}



WMARESULT prvQuantizeRLCSpectrumInvQ_X86(CAudioObjectEncoder *pauenc,
                                         PerChannelInfo *pcinfo)
{
    I16* rgiCoefQ       = pcinfo->m_rgiCoefQRLC;
    const Float* rgfltCoef    = pcinfo->m_rgfltCoef;

    Float* rgfltCoefRecon = pcinfo->m_rgfltCoefRecon;
    const Float* rgfltWeight    = pcinfo->m_rgfltWeightFactor;

    const Int cSubbandActual  = pcinfo->m_cSubbandActual;

    const Float fltQuantStep = pauenc->m_fltQuantStep;
    const Float fltInverseQuantStep = 1.0f / pauenc->m_fltQuantStep;

    const Float fltZeroThresh = pauenc->m_fltQuantStep / 2;
    const Float fltOneThresh  = pauenc->m_fltQuantStep + fltZeroThresh;
    const Int iZeroThresh = (*(Int*)&fltZeroThresh) & 0x7fffffff; 
    const Int iOneThresh  = (*(Int*)&fltOneThresh)  & 0x7fffffff;
    Int i;
    const CAudioObject *pau = pauenc->pau;

    const Int *rgiBarkIndexLastUpdate = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *
        LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);
    const Int cValidBarkBandLastUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf /
        pau->m_iSubFrameSizeWithUpdate)];
    const Int *piNextBarkIndex = rgiBarkIndexLastUpdate;

    const Int c_iMaskResampleFB = 12;
    const Int c_iRoundUp = (1 << c_iMaskResampleFB) - 1;
    const Int iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << 12) /                           
                          pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];
    const Int iIndexResampleRatio =
        (pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] << 12) /
        pau->m_iSubFrameSizeWithUpdate;
    
    const I32 c_iOverflow = (I32) (pauenc->pau->m_iMaxEscLevel * QUANTIZER_CLIP_LIMIT);
    const I32 c_iMaxEscLevel = pauenc->pau->m_iMaxEscLevel;
    I32 iTemp;
    I32 fOverflow = WMAB_FALSE;
    I32 fNonZeroCoef = WMAB_FALSE;
    Float *pfltCoefRecon = rgfltCoefRecon;
    I16 *piCoefQRLC = rgiCoefQ;
    const Float *pfltCoef = rgfltCoef;

#ifdef _DEBUG
    I32 rgiFPUEnviro[10] = {0};
#endif  // _DEBUG

    // Setting everything to zero here is faster than doing it in loop
    memset (rgfltCoefRecon, 0, sizeof (CoefType) * pauenc->pau->m_cSubband);
    i = 0;

// Outer loop register assignments
#define rpiNextBarkIndex    eax
#define rpfltWeight         eax
#define riResampledIdx      edx
#define riNextI             edx
#define riNextBoundary      ecx

#define MASKRESAMPLE_FB     12
#define ROUNDUP             0x00000FFF

// Inner loop register assignments
#define rCoef           eax
#define rTemp           eax
#define rpfltCoef       esi
#define riCountZero     ecx
#define riRun           ebx
#define riZeroThresh    edx
#define rTemp2          edx
#define rpiCoefQRLC     edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4
#define SIZEOF_FLOAT    4
    _asm
    {
        // Initialize FLOP stack
        fld     fltInverseQuantStep
        fld     fltQuantStep

        // Initialize persistent inner loop registers (untouched by outer loop)
        mov     rpfltCoef, pfltCoef
        xor     riRun, riRun                // run = 0
        mov     rpiCoefQRLC, piCoefQRLC

outerLoop:
        // Initialize outer loop registers
        mov     riResampledIdx, i
        mov     rpiNextBarkIndex, piNextBarkIndex
        imul    riResampledIdx, iMaskResampleRatio
        sar     riResampledIdx, MASKRESAMPLE_FB

        // Advance bark index until we find one that maps past current i
nextBarkIndex:
        mov     riNextBoundary, [rpiNextBarkIndex]
        add     rpiNextBarkIndex, SIZEOF_I32
        cmp     riResampledIdx, riNextBoundary
        jge     nextBarkIndex

        // Compute next boundary
        mov     riNextI, i
        imul    riNextBoundary, iIndexResampleRatio
        add     riNextBoundary, ROUNDUP
        sar     riNextBoundary, MASKRESAMPLE_FB
        cmp     riNextBoundary, cSubbandActual  // Clamp us to cSubbandActual
        cmovg   riNextBoundary, cSubbandActual

        // Compute inner loop iterations
        sub     rpiNextBarkIndex, SIZEOF_I32
        mov     piNextBarkIndex, rpiNextBarkIndex
        mov     rpfltWeight, rgfltWeight
        mov     i, riNextBoundary           // When we exit inner loop, i = iNextBoundary
        sub     riNextBoundary, riNextI     // Coincidence! riNextBoundary = riCountZero = ecx

        // Compute QuantStep times WeightFactor
        fld     dword ptr [rpfltWeight + SIZEOF_FLOAT * riNextI]    // Load weight factor
        fmul    st(0), st(1)                // QuantStep * WeightFactor
        fld     st(0)                       // Duplicate and
        fchs                                // make negative

        // ***** START OF INNER LOOP *****
        // Load non-persistent inner loop registers
        mov     riZeroThresh, iZeroThresh
        
checkZEROThreshold:
        mov     rCoef, dword ptr [rpfltCoef]// Load rgfltCoef[i] as integer
        and     rCoef, 0x7FFFFFFF       // Take absolute value
        cmp     rCoef, riZeroThresh     // Compare (iFltAbs < iZeroThresh)
        jge     checkONEThreshold
        add     rpfltCoef, SIZEOF_FLOAT // pfltCoef += 1
        inc     riRun                   // run += 1
        dec     riCountZero
        jg      checkZEROThreshold
        jmp     innerLoopEND

checkONEThreshold:
        cmp     rCoef, iOneThresh       // Compare (iFltAbs < iOneThresh)
        jge     moreThanONE
        mov     rCoef, dword ptr [rpfltCoef]// Load original rgfltCoef[i] for its sign
        add     rpfltCoef, SIZEOF_FLOAT // pfltCoef += 1
        not     rCoef                   // iSign = ((iFlt & 0x80000000) ^ 0x80000000)
        and     rCoef, 0x80000000       // We are writing to *piCoefQRLC one DWORD at a time

        // While sign is fresh in our flag register, do inverse quant
        fld     st(1)                   // Default is positive fltQuantStepXWeightFactor
        fcmove  st(0), st(1)            // Load negative fltQuantStepXWeightFactor if needed

        or      rCoef, 0x00010000       // (iSign | 1)
        or      rCoef, riRun            // Now record the run
        mov     dword ptr [rpiCoefQRLC], rCoef  // Write two I16's at a time to *piCoefQRLC
        add     rpiCoefQRLC, 2*SIZEOF_I16       // piCoefQRLC += 2

        // Finish inverse quant now that we have a register
        mov     rTemp, pfltCoefRecon
        lea     rTemp, [rTemp + SIZEOF_FLOAT * riRun + SIZEOF_FLOAT]    // Skip those 0's
        fstp    dword ptr [rTemp - SIZEOF_FLOAT]
        mov     pfltCoefRecon, rTemp

        xor     riRun, riRun            // run = 0
        mov     fNonZeroCoef, 1
        dec     riCountZero
        jg      checkZEROThreshold
        jmp     innerLoopEND

moreThanONE:
        fld     dword ptr [rpfltCoef]
        add     rpfltCoef, SIZEOF_FLOAT
        fmul    st(0), st(4)                // Full meal deal, rgfltCoef[i] * fltInverseQuantStep

#ifdef X86_C_EQUIVALENCE
        // Enable this to achieve bitwise identical output to C
        // Leave disabled for perf reasons
        fstp    dword ptr [iTemp]       // Convert to float so we are consistent with C
        fld     dword ptr [iTemp]
#endif

        fistp   dword ptr [iTemp]           // Round to integer
        mov     rCoef, dword ptr [iTemp]    // Load integer into register
        mov     rTemp2, rCoef               // Commandeer the iZeroThresh register
        not     rTemp2                      // Extract the sign bit
        and     rTemp2, 0x80000000

        // While sign is fresh in our flag register, prep inverse quant            
        fld     st(1)                   // Default is positive fltQuantStepXWeightFactor
        fcmove  st(0), st(1)            // Load negative fltQuantStepXWeightFactor if needed

        or      riRun, rTemp2           // Store sign bit in run register
        cdq                             // Take abs of integer (stored in eax)
        xor     eax, edx
        sub     eax, edx
        cmp     eax, c_iMaxEscLevel     // Clamp quantizer values to their maximums, and
        jg      moreThanONE_checkOverflow // also check for overflow

moreThanONE_invQ:
        mov     dword ptr [iTemp], eax
        fild    dword ptr [iTemp]
        fmulp   st(1), st(0)            // Do (clipTemp * fltQuantStepXWeightFactor)
        shl     eax, 16                 // Construct QRLC entry (entire DWORD at once)
        or      riRun, eax
        mov     dword ptr [edi], riRun  // Write two I16's at a time to *piCoefQRLC
        add     rpiCoefQRLC, 2*SIZEOF_I16   // piCoefQRLC += 2

        mov     riZeroThresh, iZeroThresh   // Restore iZeroThresh register
        and     riRun, 0x0000FFFF       // Restore run value so we can skip the 0's
        mov     rTemp, pfltCoefRecon
        lea     rTemp, [rTemp + SIZEOF_FLOAT * riRun + SIZEOF_FLOAT]
        fstp    dword ptr [rTemp - SIZEOF_FLOAT]
        mov     pfltCoefRecon, rTemp

        xor     riRun, riRun                // run = 0
        mov     fNonZeroCoef, 1
        dec     riCountZero
        jg      checkZEROThreshold
        jmp     innerLoopEND


moreThanONE_checkOverflow:
        cmp     rCoef, c_iOverflow
        cmovle  rCoef, c_iMaxEscLevel
        jle     moreThanONE_invQ

        // OVERFLOW!!
        mov     fOverflow, 1
        fstp    st(0)

        // Dump QuantStep * WeightFactor
        fstp    st(0)
        fstp    st(0)

        jmp     overflowExit


innerLoopEND:
        // Dump QuantStep * WeightFactor
        fstp    st(0)
        fstp    st(0)

        // ***** END OF INNER LOOP *****
        // Check for outer loop exit condition
        mov     rTemp, i
        cmp     rTemp, cSubbandActual
        jl      outerLoop

        // ***** END OF OUTER LOOP *****
        // If we have a non-zero run, write it out
        test    riRun, riRun
        je      overflowExit

        mov     [rpiCoefQRLC], riRun        // Write two I16's at a time to QRLC
        add     rpiCoefQRLC, 2*SIZEOF_I16

overflowExit:
        // Save position in QRLC
        mov     piCoefQRLC, rpiCoefQRLC

        // Dump floating point stack
        fstp    st(0)
        fstp    st(0)

#ifdef _DEBUG
        // Verify that FLOP stack is empty on exit
        fnstenv rgiFPUEnviro
        xor     eax, eax
        mov     eax, dword ptr [rgiFPUEnviro + 2*SIZEOF_I32]    // 3rd DWORD must be 0xFFFFFFFF
        not     eax
        test    eax, eax
        je      debugOUT

        int     3                // We need to clean up that FLOP stack!

debugOUT:
#endif  //_DEBUG
    }

    if (WMAB_FALSE == fOverflow)
    {
        pcinfo->m_rgiCoefQRLC[-1] = (piCoefQRLC - rgiCoefQ);
        pcinfo->m_fAnyNonZeroCoefQ = fNonZeroCoef;
    }
    else
    {
        pcinfo->m_rgiCoefQRLC[-1] = 0;
        pcinfo->m_fAnyNonZeroCoefQ = WMAB_TRUE;
        pauenc->m_fQuantOverflow = WMAB_TRUE;
    }

    return WMA_OK;
} // prvQuantizeRLCSpectrumInvQ_X86



WMARESULT prvQuantizeRLCSpectrumDIFFInvQ_X86(CAudioObjectEncoder *pauenc,
                                             PerChannelInfo *pcinfo)
{
    const Int       cSubbandActual  = pcinfo->m_cSubbandActual;
    const Float    *rgfltCoef = pcinfo->m_rgfltCoef;
    const Float     fltQuantStep = pauenc->m_fltQuantStep;
    const Float     fltInverseQuantStep = 1.0f / pauenc->m_fltQuantStep;
    const Float     fltZeroThresh = pauenc->m_fltQuantStep / 2;
    const Float     fltOneThresh  = pauenc->m_fltQuantStep + fltZeroThresh;
    const Int       iZeroThresh = (*(Int*)&fltZeroThresh) & 0x7fffffff; 
    const Int       iOneThresh  = (*(Int*)&fltOneThresh)  & 0x7fffffff;
    const I32       c_iOverflow = (I32) (pauenc->pau->m_iMaxEscLevel * QUANTIZER_CLIP_LIMIT);
    const I32       c_iMaxEscLevel = (I16) pauenc->pau->m_iMaxEscLevel;
    
    const I16      *rgiCoefQRLCIn = pcinfo->m_rgiCoefQRLC_DIFF;
    I16            *rgiCoefQRLCOut = pcinfo->m_rgiCoefQRLC;

    Float          *rgfltCoefRecon = pcinfo->m_rgfltCoefRecon;
    Float          *rgfltWeight = pcinfo->m_rgfltWeightFactor;
    const I16      *piSrcQ = rgiCoefQRLCIn;
    I16            *piDstQ = rgiCoefQRLCOut;
    Int             iQCount = rgiCoefQRLCIn[-1];
    Int             iCoefIdx = 0;

    Bool            fMergeZeroRuns = WMAB_FALSE;
    Bool            fNonZeroCoefs = WMAB_FALSE;
    Int             iZeroes;

    I32             iTemp = 0;
    I32             fOverflow = WMAB_FALSE;

#ifdef _DEBUG
    I32             rgiFPUEnviro[10] = {0};
#endif  // _DEBUG

    // Most coefficients end up going to zero, so it's faster to memset outside loop
    memset(rgfltCoefRecon, 0, sizeof(pcinfo->m_rgfltCoefRecon[0]) * cSubbandActual);

#define rCoef           eax
#define riCoefQRLC      eax
#define rTemp           eax
#define riZeroes        ebx
#define riLoopCtr       ecx
#define riCoefIdx       edx
#define rTemp2          edx
#define rpfltCoef       esi
#define rpiCoefQRLC     edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4
#define SIZEOF_FLOAT    4

    assert(iQCount > 0);
    _asm
    {
        // Initialize loop
        mov     riLoopCtr, iQCount
        sar     riLoopCtr, 1                // We do 2 QRLC entries at a time in this loop
        mov     rpfltCoef, rgfltCoef
        mov     rpiCoefQRLC, rgiCoefQRLCIn
        xor     riZeroes, riZeroes
        xor     riCoefIdx, riCoefIdx

        // Initialize FLOP stack
        fld     fltInverseQuantStep
        fld     fltQuantStep
        fld     st(0)                       // Duplicate and
        fchs                                // make negative

        jmp nextZeroRun

requantToZero:
        inc     riCoefIdx
        inc     riZeroes                            // This level is one more zero to add to the run
        dec     riLoopCtr
        mov     fMergeZeroRuns, 1                   // fMergeZeroRuns = 1
        jle     loopExit

nextZeroRun:
        mov     riCoefQRLC, dword ptr [rpiCoefQRLC] // Read in I32 at a time from QRLC
        add     rpiCoefQRLC, SIZEOF_I32
        and     riCoefQRLC, 0x0000FFFF              // Only keep the run, the rest is requantized
        add     riCoefIdx, riCoefQRLC               // Increment index
        add     riZeroes, riCoefQRLC                // iZeroes += cRunOfZeroes

        // If QRLC ends with run of zeroes, don't read from rgfltCoef (reads past end of array)
        cmp     riCoefIdx, cSubbandActual
        jl      requantize

        dec     riLoopCtr
        mov     fMergeZeroRuns, 1                   // fMergeZeroRuns = 1
        jmp     loopExit

requantize:
        // Now requantize the level
        mov     rCoef, dword ptr [rpfltCoef + SIZEOF_FLOAT * riCoefIdx]        // Load coefficient
        and     rCoef, 0x7FFFFFFF                   // Take absolute value
        cmp     rCoef, iZeroThresh                  // Does this go to zero?
        jl      requantToZero

        // If we reach here, this requantized to something other than zero
        cmp     rCoef, iOneThresh
        jge     moreThanONE
        mov     rTemp, dword ptr [rpfltCoef + SIZEOF_FLOAT * riCoefIdx] // Reload original to get its sign
        not     rTemp
        and     rTemp, 0x80000000           // Extract that (reversed) sign bit

        // While sign is fresh in our flag register, prep inverse quant            
        fld     st(1)                   // Default is positive fltQuantStep
        fcmove  st(0), st(1)            // Load negative fltQuantStep if needed

        or      rTemp, 0x00010000           // Set magnitude to 1
        or      riZeroes, rTemp             // Construct QLRC entry (two I16's at a time)

        mov     rTemp, piDstQ                       // Write out two I16's at once to QRLC
        mov     dword ptr [rTemp], riZeroes
        add     rTemp, SIZEOF_I32
        mov     piDstQ, rTemp

        // Now finish inverse quantization
        mov     rTemp, rgfltWeight
        fld     dword ptr [rTemp + SIZEOF_FLOAT * riCoefIdx]    // Load rgfltWeight[iCoefIdx]
        mov     rTemp, rgfltCoefRecon
        fmulp   st(1), st(0)                        // (+/-) fltQuantStep * rgfltWeight[iCoefIdx]
        fstp    dword ptr [rTemp + SIZEOF_FLOAT * riCoefIdx]    // Store to rgfltCoefRecon[iCoefIdx]

        mov     fMergeZeroRuns, 0
        inc     riCoefIdx
        mov     fNonZeroCoefs, 1
        xor     riZeroes, riZeroes                  // iZeroes = 0

        dec     riLoopCtr
        jg      nextZeroRun
        jmp     loopExit

moreThanONE:
        // Requantize the mother
        fld     dword ptr [rpfltCoef + SIZEOF_FLOAT * riCoefIdx]
        fmul    st(0), st(3)            // rgfltCoef[iCoefIdx] * fltInverseQuantStep

#ifdef X86_C_EQUIVALENCE
        // Enable this to achieve bitwise identical output to C
        // Leave disabled for perf reasons
        fstp    dword ptr [iTemp]       // Convert to float so we are consistent with C
        fld     dword ptr [iTemp]
#endif

        fistp   dword ptr [iTemp]       // Round to integer
        mov     rTemp, dword ptr [iTemp]// Load rounded integer to register
        mov     iCoefIdx, riCoefIdx
        mov     rTemp2, rTemp           // Commandeer the riCoefIdx register
        not     rTemp2                  // Extract the sign bit
        and     rTemp2, 0x80000000

        // While sign is fresh in our flag register, prep inverse quant            
        fld     st(1)                   // Default is positive fltQuantStep
        fcmove  st(0), st(1)            // Load negative fltQuantStep if needed

        or      riZeroes, rTemp2        // Dump sign bit into riZeroes

        cdq                             // Take abs of integer (stored in eax)
        xor     eax, edx
        sub     eax, edx
        cmp     eax, c_iMaxEscLevel     // Clamp quantizer values to their maximums, and
        jg      moreThanONE_checkOverflow // also check for overflow

moreThanONE_invQ:
        mov     riCoefIdx, iCoefIdx     // Restore riCoefIdx
        mov     dword ptr [iTemp], eax
        fild    dword ptr [iTemp]       // Load quantizer value into FLOP
        fmulp   st(1), st(0)            // iQuantized * fltQuantStep
        shl     eax, 16                 // Construct next two I16's for QRLC
        or      riZeroes, eax

        mov     rTemp, piDstQ                       // Write out two I16's at once to QRLC
        mov     dword ptr [rTemp], riZeroes
        add     rTemp, SIZEOF_I32
        mov     piDstQ, rTemp

        // Now finish inverse quantization
        mov     rTemp, rgfltWeight
        fld     dword ptr [rTemp + SIZEOF_FLOAT * riCoefIdx]    // Load rgfltWeight[iCoefIdx]
        mov     rTemp, rgfltCoefRecon
        fmulp   st(1), st(0)                        // (+/-) fltQuantStep * rgfltWeight[iCoefIdx]
        fstp    dword ptr [rTemp + SIZEOF_FLOAT * riCoefIdx]    // Store to rgfltCoefRecon[iCoefIdx]

        mov     fMergeZeroRuns, 0
        inc     riCoefIdx
        mov     fNonZeroCoefs, 1
        xor     riZeroes, riZeroes                  // iZeroes = 0

        dec     riLoopCtr
        jg      nextZeroRun
        jmp     loopExit

moreThanONE_checkOverflow:
        cmp     eax, c_iOverflow
        cmovle  eax, c_iMaxEscLevel
        jle     moreThanONE_invQ

        // OVERFLOW!!
        mov     fOverflow, 1
        fstp    st(0)               // Dump +/- fltQuantStep
        jmp     loopExit


loopExit:
        // Save variables used in post-processing
        mov     iZeroes, riZeroes
        mov     piSrcQ, rpiCoefQRLC

#ifdef _DEBUG
        // Just for the asserts
        shl     riLoopCtr, 1        // We initially divide by 2 because we write 2 QRLC's at once
        mov     iQCount, riLoopCtr
        mov     iCoefIdx, riCoefIdx
#endif  // _DEBUG

        // Dump FLOP stack
        fstp    st(0)
        fstp    st(0)
        fstp    st(0)

#ifdef _DEBUG
        // Verify that FLOP stack is empty on exit
        fnstenv rgiFPUEnviro
        xor     eax, eax
        mov     eax, dword ptr [rgiFPUEnviro + 2*SIZEOF_I32]    // 3rd DWORD must be 0xFFFFFFFF
        not     eax
        test    eax, eax
        je      debugOUT

        int     3                // We need to clean up that FLOP stack!

debugOUT:
#endif  //_DEBUG
    } // asm

    if (fOverflow)
    {
        pauenc->m_fQuantOverflow = WMAB_TRUE;
        return WMA_OK;
    }

    assert(0 == iQCount); // Otherwise we read bogus memory
    assert(iCoefIdx == pcinfo->m_cSubbandActual);
    assert(piSrcQ == rgiCoefQRLCIn + rgiCoefQRLCIn[-1]);

    if (fMergeZeroRuns)
    {
        // Write out zero level to finish the QRLC table
        *piDstQ = (I16) iZeroes;
        *(piDstQ + 1) = 0;
        piDstQ += 2;
    }

    pcinfo->m_fAnyNonZeroCoefQ = fNonZeroCoefs;
    rgiCoefQRLCOut[-1] = (piDstQ - rgiCoefQRLCOut);

    return WMA_OK;
} // prvQuantizeRLCSpectrumDIFFInvQ_X86


#endif

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
                    mov         [esp-12],eax

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


                mov         eax,[esp-12]
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
                    mov         [esp-12],eax

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


                mov         eax,[esp-12]
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
                    mov         [esp-12],eax

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

                mov         eax,[esp-12]
                dec         eax
                jnz         Loop2

                SAVE_LOOP_STATE1_2

                } 
            } else {
                _asm{
                INIT_LOOP1_2
                movq        mm6,xFFFF0000FFFF0000     

                Loop2m:
                    mov         [esp-12],eax

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

                mov         eax,[esp-12]
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
#if !defined(PREVOUTPUT_16BITS) && !defined (BUILD_INTEGER) && !defined (DISABLE_OPT)
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
#endif // !defined(PREVOUTPUT_16BITS) && !defined (BUILD_INTEGER) && !defined (DISABLE_OPT)

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

            }
        }
        if (iLoopBy1) _asm emms
        for (iLoopCnt = 0; iLoopCnt < iLoopBy1; iLoopCnt++, i++,(*pcSampleGet)++)    {
            Float cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            cfResult = INT_FROM_COEF( *--piCoef2 );
            ROUND_SATURATE_STORE( (piDst+1), cfResult, SHRT_MIN, SHRT_MAX, iResult );
            piDst += cChan;
        }
    }
    _asm emms

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

#ifdef ENCODER


//---------------------------------------------------------------------------
// Encoder Filter Verification
//---------------------------------------------------------------------------

#ifdef MMXVERIFY_ENABLE
void FourTapNUMERATOR_MMX_VERIFY
                      (const I16 *rgiCoefNum,
                       const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                       const I16 cChannel, const Int iSourceLength,
                       const I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                       const I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const Int c_minus1Mod3[] = {2, 0, 1};
    const Int ORDER = 3;

    I32 iSrcCurr = *piSrcCurr;
    int j;

    for (j = 0; j < iSourceLength; j++)
    {
        I32 iResult;

        assert (iSrcCurr >= 0 && iSrcCurr < 3);

        iResult = piSource[j*cChannel] * rgiCoefNum[0]
            + rgiSrc[iSrcCurr + 0] * rgiCoefNum[1]
            + rgiSrc[iSrcCurr + 1] * rgiCoefNum[2]
            + rgiSrc[iSrcCurr + 2] * rgiCoefNum[3];

        if (iResult != piFIROutAligned[j])
            DEBUG_BREAK();

        iSrcCurr = c_minus1Mod3[iSrcCurr];
        rgiSrc[iSrcCurr] = rgiSrc[iSrcCurr + ORDER] = piSource[j*cChannel];
    }

    *piSrcCurr = iSrcCurr;
}



void FiveTapNUMERATOR_MMX_VERIFY
                      (const I16 *rgiCoefNum,
                       const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                       const I16 cChannel, const Int iSourceLength,
                       const I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                       const I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const Int c_minus1Mod4[] = {3, 0, 1, 2};
    const Int ORDER = 4;

    I32 iSrcCurr = *piSrcCurr;
    int j;

    for (j = 0; j < iSourceLength; j++)
    {
        I32 iResult;

        assert (iSrcCurr >= 0 && iSrcCurr < ORDER);

        iResult = piSource[j*cChannel] * rgiCoefNum[0]
            + rgiSrc[iSrcCurr + 0] * rgiCoefNum[1]
            + rgiSrc[iSrcCurr + 1] * rgiCoefNum[2]
            + rgiSrc[iSrcCurr + 2] * rgiCoefNum[3]
            + rgiSrc[iSrcCurr + 3] * rgiCoefNum[4];

        if (iResult != piFIROutAligned[j])
            DEBUG_BREAK();

        iSrcCurr = c_minus1Mod4[iSrcCurr];
        rgiSrc[iSrcCurr] = rgiSrc[iSrcCurr + ORDER] = piSource[j*cChannel];
    }

    *piSrcCurr = iSrcCurr;
}



void EightTapNUMERATOR_MMX_VERIFY
                       (const I16 *rgiCoefNum,
                        const I16 *piSource,
                        I16* rgiSrc, I32 *piSrcCurr,
                        const I16 cChannel, const Int iSourceLength,
                        const I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                        const I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const Int c_minus1Mod6[] = {5, 0, 1, 2, 3, 4};
    const Int ORDER = 6;

    I32 iSrcCurr = *piSrcCurr;
    int j;

    for (j = 0; j < iSourceLength; j++)
    {
        I32 iResult;

        assert (iSrcCurr >= 0 && iSrcCurr < ORDER+1);

        iResult = piSource[j*cChannel] * rgiCoefNum[0]
            + rgiSrc[iSrcCurr + 0] * rgiCoefNum[1]
            + rgiSrc[iSrcCurr + 1] * rgiCoefNum[2]
            + rgiSrc[iSrcCurr + 2] * rgiCoefNum[3]
            + rgiSrc[iSrcCurr + 3] * rgiCoefNum[4]
            + rgiSrc[iSrcCurr + 4] * rgiCoefNum[5]
            + rgiSrc[iSrcCurr + 5] * rgiCoefNum[6]
            + rgiSrc[iSrcCurr + 6] * rgiCoefNum[7];

        if (iResult != piFIROutAligned[j])
            DEBUG_BREAK();

        iSrcCurr = c_minus1Mod6[iSrcCurr];
        rgiSrc[iSrcCurr] = rgiSrc[iSrcCurr + ORDER] = piSource[j*cChannel];
    }

    *piSrcCurr = iSrcCurr;
}



void FourTapDENOMINATOR_FLOP_VERIFY
                      (const Float *rgfltCoefDen,
                       const int position, const Float fltInvNumScale,
                       float *rgfltDst, I32 *piDstCurr,
                       const Int iSliceIncr, Float *pfltPower,
                       const Int iSourceLength, const I32 *piFIROutAligned)
{
    const Int c_minus1Mod3[] = {2, 0, 1};
    const Int ORDER = 3;
    const int c_iSliceMask = iSliceIncr - 1;

    Float fltSumSquared = 0;
    I32 iDstCurr = *piDstCurr;
    int j;

    for (j = 0; j < iSourceLength; )
    {
        Float fltDst;

        assert (rgfltCoefDen[0] == 1.0F);
        assert (iDstCurr >= 0 && iDstCurr < 3);

        fltDst = fltInvNumScale * piFIROutAligned[j]
            - rgfltCoefDen[1] * rgfltDst[iDstCurr + 0]
            - rgfltCoefDen[2] * rgfltDst[iDstCurr + 1]
            - rgfltCoefDen[3] * rgfltDst[iDstCurr + 2];

        iDstCurr = c_minus1Mod3[iDstCurr];
        rgfltDst[iDstCurr] = rgfltDst[iDstCurr + ORDER] = fltDst;

        fltSumSquared += fltDst * fltDst;

        j += 1;
        if ((j & c_iSliceMask) == 0)
        {
            if (fabs((pfltPower[position] - fltSumSquared)/fltSumSquared) >= 0.1)
                DEBUG_BREAK();

            pfltPower += 3;
            fltSumSquared = 0;
        } // if
    } // for

    *piDstCurr = iDstCurr;
}



void FiveTapDENOMINATOR_FLOP_VERIFY
                      (const Float *rgfltCoefDen,
                       const int position, const Float fltInvNumScale,
                       float *rgfltDst, I32 *piDstCurr,
                       const Int iSliceIncr, Float *pfltPower,
                       const Int iSourceLength, const I32 *piFIROutAligned)
{
    const Int c_minus1Mod4[] = {3, 0, 1, 2};
    const Int ORDER = 4;
    const int c_iSliceMask = iSliceIncr - 1;

    Float fltSumSquared = 0;
    I32 iDstCurr = *piDstCurr;
    int j;

    for (j = 0; j < iSourceLength; )
    {
        Float fltDst;

        assert (rgfltCoefDen[0] == 1.0F);
        assert (iDstCurr >= 0 && iDstCurr < ORDER);

        fltDst = fltInvNumScale * piFIROutAligned[j]
            - rgfltCoefDen[1] * rgfltDst[iDstCurr + 0]
            - rgfltCoefDen[2] * rgfltDst[iDstCurr + 1]
            - rgfltCoefDen[3] * rgfltDst[iDstCurr + 2]
            - rgfltCoefDen[4] * rgfltDst[iDstCurr + 3];

        iDstCurr = c_minus1Mod4[iDstCurr];
        rgfltDst[iDstCurr] = rgfltDst[iDstCurr + ORDER] = fltDst;

        fltSumSquared += fltDst * fltDst;

        j += 1;
        if ((j & c_iSliceMask) == 0)
        {
            if (fabs((pfltPower[position] - fltSumSquared)/fltSumSquared) >= 0.1)
                DEBUG_BREAK();

            pfltPower += 3;
            fltSumSquared = 0;
        } // if
    } // for

    *piDstCurr = iDstCurr;
}



void SevenTapDENOMINATOR_FLOP_VERIFY
                      (const Float *rgfltCoefDen,
                       const int position, const Float fltInvNumScale,
                       float *rgfltDst, I32 *piDstCurr,
                       const Int iSliceIncr, Float *pfltPower,
                       const Int iSourceLength, const I32 *piFIROutAligned)
{
    const Int c_minus1Mod6[] = {5, 0, 1, 2, 3, 4};
    const Int ORDER = 6;
    const int c_iSliceMask = iSliceIncr - 1;

    Float fltSumSquared = 0;
    I32 iDstCurr = *piDstCurr;
    int j;

    for (j = 0; j < iSourceLength; )
    {
        Float fltDst;

        assert (rgfltCoefDen[0] == 1.0F);
        assert (iDstCurr >= 0 && iDstCurr < ORDER+1);

        fltDst = fltInvNumScale * piFIROutAligned[j]
            - rgfltCoefDen[1] * rgfltDst[iDstCurr + 0]
            - rgfltCoefDen[2] * rgfltDst[iDstCurr + 1]
            - rgfltCoefDen[3] * rgfltDst[iDstCurr + 2]
            - rgfltCoefDen[4] * rgfltDst[iDstCurr + 3]
            - rgfltCoefDen[5] * rgfltDst[iDstCurr + 4]
            - rgfltCoefDen[6] * rgfltDst[iDstCurr + 5];

        iDstCurr = c_minus1Mod6[iDstCurr];
        rgfltDst[iDstCurr] = rgfltDst[iDstCurr + ORDER] = fltDst;

        fltSumSquared += fltDst * fltDst;

        j += 1;
        if ((j & c_iSliceMask) == 0)
        {
            if (fabs((pfltPower[position] - fltSumSquared)/fltSumSquared) >= 0.1)
                DEBUG_BREAK();

            pfltPower += 3;
            fltSumSquared = 0;
        } // if
    } // for

    *piDstCurr = iDstCurr;
}
#endif  // MMXVERIFY_ENABLE


//---------------------------------------------------------------------------
// Encoder Filter Helper Functions
//---------------------------------------------------------------------------

//***************************************************************************
// Function: NTapFIR_Unaligned_MMX
//
// Purpose:
//     This function performs FIR filter calculations on the given inputs,
// and when complete it updates the input history in a manner that the MMX
// numerator code can understand. Its purpose is to perform the math on the
// unaligned memory segment, thus allowing the MMX portion to operate only
// on aligned memory.
//
// Arguments
//  I16 *rgiCoefNum [in] - Numerator coefficients.
//  I16 *piSource [in] - Input data. piSource[0] must always be a valid input.
//     For mono sources, piSource[1] is next valid input. For stereo sources,
//     piSource[2] is next valid input (skip piSource[1]).
//  I32 iSourceLength [in] - Number of samples to filter.
//  I16 *rgiSrc [in/out] - Input data history buffer. Before returning, this
//     function updates this array to reflect the recently used inputs.
//     NOTE that this history is in MMX form, basically a quadword copy of the
//     last 4*N inputs (where N is an integer >= 1).
//  I32 *piFIROut [out] - The FIR filter output is stored here.
//  int cChannels [in] - number of channels in the input data stream (interleaved).
//  I32 iNumTaps [in] - number of taps in the FIR filter.
//***************************************************************************
void INLINE NTapFIR_Unaligned_MMX(const I16 *rgiCoefNum,
                                  const I16 *piSource, const I32 iSourceLength,
                                  I16 *rgiSrc, I32 *piFIROut,
                                  const int cChannels, const I32 iNumTaps)
{
    const int c_iTotalHistorySlots = 4*(((iNumTaps - 1) + 3)/4);

    int j;
    int iNewHistory;

    for (j = 0; j < iSourceLength; j++)
    {
        // Recall history is a quadword of last input vectors. Since we only need
        // (iNumTaps - 1) histories, calculate how many quadwords are required
        // and index to the end of that quadword.
        const int c_iHistoryStart = c_iTotalHistorySlots - 1;
        int i;
        int k;

        piFIROut[j] = 0;

        // Multiply current input samples by the coefficients
        for (i = 0; i <= j && i < iNumTaps; i++)
            piFIROut[j] += piSource[(j - i) * cChannels] * rgiCoefNum[i];

        // Next, multiply historic input samples by the coefficients (if required)
        for (k = i ; k < iNumTaps; k++)
            piFIROut[j] += rgiSrc[c_iHistoryStart - (k - i)] * rgiCoefNum[k];
    }

    // Now that we've calculated unaligned samples, update src history to reflect this
    // Recall number of history slots is always a multiple of 4, even if iNumTaps is not!
    // Right now we copy unused history slots, we can fix this later if we get bored.
    iNewHistory = min(c_iTotalHistorySlots, iSourceLength);

    if (iNewHistory < c_iTotalHistorySlots)
    {
        // Partial update of history: expunge expired samples by collapsing the array
        for (j = 0 ; j < c_iTotalHistorySlots - iNewHistory; j++)
            rgiSrc[j] = rgiSrc[j + iNewHistory];
    }

    // Read in the most recent samples, up to iNewHistory worth
    for (j = c_iTotalHistorySlots - iNewHistory; j < c_iTotalHistorySlots; j++)
        rgiSrc[j] = piSource[cChannels * (iSourceLength + j - c_iTotalHistorySlots)];

} // NTapFIR_Unaligned_MMX


//***************************************************************************
// Function: InitMMXFilterCoefs
//
// Purpose:
//     Writes coefficients to the given array to optimize MMX performance.
// To cut down on unaligned memory accesses, MMX coefficients are typically
// arranged to fit in one MMX register. For instance, given an input vector
// I = "5678" in an MMX register, plus history vector H = "1234", one would
// compute I * "a..." + H * ".dcb" to get a*5+b*4+c*3+d*2 = FIR output.
// Then, without changing input or history vectors, one uses a different
// MMX coefficient array to compute the next output (eg, "ba.." and "..cd").
//
// Arguments
//   I16 *piDst [out] - the function outputs the staggered MMX coefficients
//     to this given buffer. The minimal number of MMX registers are used,
//     ie, this function never returns an entire quadword of 0's.
//     THIS MEMORY SHOULD BE ALIGNED ON A 16-BYTE BOUNDARY (32 is even better).
//   U32 iDstSize [in] - size of buffer pointed to by *piDst.
//   I16 *piSrc [in] - FIR Numerator coefficients.
//   U32 iTapCount [in] - Number of taps in numberator.
//
// Perf Notes
//   Inlining this function actually slows down performance for some reason.
//***************************************************************************
void InitMMXFilterCoefs(I16 *piDst, const U32 iDstSize,
                        const I16 *piSrc, const U32 iTapCount)
{
    U32 i;

    const int c_rgiMod4Offset[] = {1, 0, 3, 2};
    const int c_iOffset0 = c_rgiMod4Offset[iTapCount % 4];
    const int c_iOffset1 = 4*((c_iOffset0 + iTapCount + 3) / 4) + (c_iOffset0 + 1)%4;
    const int c_iOffset2 = 4*((c_iOffset1 + iTapCount + 3) / 4) + (c_iOffset0 + 2)%4;
    const int c_iOffset3 = 4*((c_iOffset2 + iTapCount + 3) / 4) + (c_iOffset0 + 3)%4;

    // We know what multiple-of-4 SIMD coefs look like, so verify those
    assert(iTapCount % 4 != 0 || (U32)c_iOffset0 == 1);
    assert(iTapCount % 4 != 0 || (U32)c_iOffset1 == 1 * (iTapCount + 4) + 2);
    assert(iTapCount % 4 != 0 || (U32)c_iOffset2 == 2 * (iTapCount + 4) + 3);
    assert(iTapCount % 4 != 0 || (U32)c_iOffset3 == 3 * (iTapCount + 4));

    assert(iDstSize >= (iTapCount + 3) * 4 * sizeof(I16)); // Check dest size
    assert((((U32)piDst) & 0x0000001F) == 0); // Verify 32-byte alignment of dst

    memset(piDst, 0, iDstSize); // The cheater's way to fill in the zeroes
    for (i = 0; i < iTapCount; i++)
    {
        const I16 iCurrCoef = piSrc[(iTapCount - 1) - i];

        // Do first-position coefficient array (x432 1xxx)
        piDst[i + c_iOffset0] = iCurrCoef;

        // Next, second-position coefficient array (xx43 21xx)
        piDst[i + c_iOffset1] = iCurrCoef;

        // Next, third-position coefficient array (xxx4 321x)
        piDst[i + c_iOffset2] = iCurrCoef;

        // Finally, fourth-position (unstaggered) coefficient array (4321)
        piDst[i + c_iOffset3] = iCurrCoef;
    }
} // InitMMXFilterCoefs



//***************************************************************************
// MMX MONO NUMERATORS
//
// I used these functions to construct stereo versions of the MMX numerators.
// Unfortunately it appears that MMX mono is no faster than the C code, so
// I've decided not to include them. These filters will probably be deleted
// from the codebase soon after this (I just want to record for posterity).
//***************************************************************************
#ifdef MMXMONO
void INLINE FourTapNUMERATOR_MMX_MONO(const I16 *rgiCoefNum,
                                      const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                                      const Int iSourceLength,
                                      I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                                      I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 1;

    Int iSrcCurr = *piSrcCurr;

    I32    *piFIROut = piFIROutAligned;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStart = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4


    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 4);
    piSource += iNumUnalignedStart;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 4);

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

        movq        mm2, [piMMXCoefsASM + SIZEOF_I16*0] // Load 1-offset coefs (part 1)
        movq        mm3, [piMMXCoefsASM + SIZEOF_I16*4] // Load 1-offset coefs (part 2)
        movq        mm4, [piMMXCoefsASM + SIZEOF_I16*24]    // Load 4-offset coefs

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        movq        mm6, [piSrc]                        // Load in the input

        // ---INTERATION 0---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, mm2                            // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, mm3                            // MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*8] // MADD oldest history w/ 2-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*12]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, mm4                            // MADD current input vector
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 4*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Current input vector is now history

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 4);
} // FourTapNUMERATOR_MMX_MONO


void INLINE
FiveTapNUMERATOR_MMX_MONO
                      (const I16 *rgiCoefNum,
                       const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                       const Int iSourceLength,
                       I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                       I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 1;

    Int iSrcCurr = *piSrcCurr;

    I32    *piFIROut = piFIROutAligned;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStart = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4


    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 5);
    piSource += iNumUnalignedStart;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 5);

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

        movq        mm2, [piMMXCoefsASM + SIZEOF_I16*0] // Load 1-offset coefs (part 1)
        movq        mm3, [piMMXCoefsASM + SIZEOF_I16*4] // Load 1-offset coefs (part 2)

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        movq        mm6, [piSrc]                        // Load in the input

        // ---INTERATION 0---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, mm2                            // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, mm3                            // MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*8] // MADD oldest history w/ 2-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*12]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*24]// MADD oldest history w/ 4-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*28]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 4*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Current input vector is now history

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 5);
} // FiveTapNUMERATOR_MMX_MONO


void INLINE
EightTapNUMERATOR_MMX_MONO
                       (const I16 *rgiCoefNum,
                        const I16 *piSource,
                        I16* rgiSrc, I32 *piSrcCurr,
                        const Int iSourceLength,
                        I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                        I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 1;

    Int iSrcCurr = *piSrcCurr;
    I32    *piFIROut = piFIROutAligned;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStart = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4

    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 8);
    piSource += iNumUnalignedStart;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 8);

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        movq        mm6, [piSrcHistory + 4*SIZEOF_I16]  // Second-oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        movq        mm5, [piSrc]                        // Load in the input

        // ---INTERATION 0---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*0] // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*4] // MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*8] // MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*12]// MADD oldest history w/ 2-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*24]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*28]// MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*32]// MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm1, mm6                            // Copy second-oldest history
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*36]// MADD second-oldest history w/ 4-offset coefs
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*40]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 4*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Second-oldest history is now oldest
        movq        mm6, mm5                    // Current input vector is now 2nd oldest

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        movq        [piSrcHistory + 4*SIZEOF_I16], mm6  // Second-oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 8);
} // EightTapNUMERATOR_MMX_MONO


#endif  // MMXMONO



//***************************************************************************
// MMX STEREO NUMERATORS
//***************************************************************************
void INLINE
FourTapNUMERATOR_MMX_STEREO
                      (const I16 *rgiCoefNum,
                       const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                       const Int iSourceLength,
                       I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                       I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 2;

    Int iSrcCurr = *piSrcCurr;

    I32    *piFIROut = piFIROutAligned;
    I32     iShiftLeft;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedStartALL;          // Unaligned samples at start including other channel's
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStartALL = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iNumUnalignedStart = (iNumUnalignedStartALL + 1) / cChannel;
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4

    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 4);
    piSource += iNumUnalignedStartALL;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 4);
    if (iNumUnalignedStartALL & 1)
    {
        // ODD # samples from piSource to align point,
        // we use sizeof(I16) offset from align
        iShiftLeft = 0; // Desired input will get read into slots 1,3
    }
    else
    {
        // EVEN # samples from piSource to align point,
        // we use 0 offset from align
        iShiftLeft = 16; // Desired input will get read into slots 0,2
    }

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define iShiftLeftASM   eax
#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iShiftLeftASM, iShiftLeft
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

        movq        mm2, [piMMXCoefsASM + SIZEOF_I16*0] // Load 1-offset coefs (part 1)
        movq        mm3, [piMMXCoefsASM + SIZEOF_I16*4] // Load 1-offset coefs (part 2)

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        movq        mm5, [piSrc + 0*SIZEOF_I16]         // Load in the input
        movq        mm4, [piSrc + 4*SIZEOF_I16]

        // Throw away the samples we don't want to construct next input vector
        pxor        mm1, mm1                    // Zero it
        movd        mm1, iShiftLeftASM
        movq        mm6, mm5
        pslld       mm6, mm1                    // Shift left by 16 or 0
        psrad       mm6, 16                     // Shift right is always 16
        pslld       mm4, mm1                    // Shift left by 16 or 0
        psrad       mm4, 16                     // Shift right is always 16
        packssdw    mm6, mm4                    // Combine the results to get 4 inputs

        // ---INTERATION 0---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, mm2                            // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, mm3                            // MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*8] // MADD oldest history w/ 2-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*12]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*24]// MADD current input vector
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 8*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Current input vector is now history

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4 * cChannel;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Remember that piSource was made to point to 32-byte boundary, even if actual input sources
    // don't live on that boundary (eg, ABAB with first "A" on 32-byte boundary, our inputs are the "B"'s)
    // If we were reading the odd ones, we need to add one here
    if (iNumUnalignedStartALL & 1)
        piSource += 1;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 4);
} // FourTapNUMERATOR_MMX_STEREO


void INLINE
FiveTapNUMERATOR_MMX_STEREO
                      (const I16 *rgiCoefNum,
                       const I16 *piSource, I16* rgiSrc, I32 *piSrcCurr,
                       const Int iSourceLength,
                       I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                       I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 2;

    Int iSrcCurr = *piSrcCurr;

    I32    *piFIROut = piFIROutAligned;
    I32     iShiftLeft;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedStartALL;          // Unaligned samples at start including other channel's
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStartALL = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iNumUnalignedStart = (iNumUnalignedStartALL + 1) / cChannel;
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4

    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 5);
    piSource += iNumUnalignedStartALL;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 5);
    if (iNumUnalignedStartALL & 1)
    {
        // ODD # samples from piSource to align point,
        // we use sizeof(I16) offset from align
        iShiftLeft = 0; // Desired input will get read into slots 1,3
    }
    else
    {
        // EVEN # samples from piSource to align point,
        // we use 0 offset from align
        iShiftLeft = 16; // Desired input will get read into slots 0,2
    }

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define iShiftLeftASM   eax
#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iShiftLeftASM, iShiftLeft
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

        movq        mm2, [piMMXCoefsASM + SIZEOF_I16*0] // Load 1-offset coefs (part 1)
        movq        mm3, [piMMXCoefsASM + SIZEOF_I16*4] // Load 1-offset coefs (part 2)

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        movq        mm5, [piSrc + 0*SIZEOF_I16]         // Load in the input
        movq        mm4, [piSrc + 4*SIZEOF_I16]

        // Throw away the samples we don't want to construct next input vector
        pxor        mm1, mm1                    // Zero it
        movd        mm1, iShiftLeftASM
        movq        mm6, mm5
        pslld       mm6, mm1                    // Shift left by 16 or 0
        psrad       mm6, 16                     // Shift right is always 16
        pslld       mm4, mm1                    // Shift left by 16 or 0
        psrad       mm4, 16                     // Shift right is always 16
        packssdw    mm6, mm4                    // Combine the results to get 4 inputs

        // ---INTERATION 0---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, mm2                            // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, mm3                            // MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*8] // MADD oldest history w/ 2-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*12]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm1, mm7                            // Copy oldest history
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*24]// MADD oldest history w/ 4-offset coefs
        movq        mm0, mm6                            // Copy current input vector
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*28]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 8*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Current input vector is now history

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4 * cChannel;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Remember that piSource was made to point to 32-byte boundary, even if actual input sources
    // don't live on that boundary (eg, ABAB with first "A" on 32-byte boundary, our inputs are the "B"'s)
    // If we were reading the odd ones, we need to add one here
    if (iNumUnalignedStartALL & 1)
        piSource += 1;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 5);
} // FiveTapNUMERATOR_MMX_STEREO



void EightTapNUMERATOR_MMX_STEREO
                       (const I16 *rgiCoefNum,
                        const I16 *piSource,
                        I16* rgiSrc, I32 *piSrcCurr,
                        const Int iSourceLength,
                        I32 *piFIROutAligned, const I32 c_iAlignMultiple,
                        I16 *piMMXCoefs, const I32 iSizeOfMMXCoefs)
{
    const int cChannel = 2;

    Int iSrcCurr = *piSrcCurr;
    I32    *piFIROut = piFIROutAligned;
    I32     iShiftLeft;

    int     iNumUnalignedStart;             // Number of unaligned samples at start of block
    int     iNumUnalignedStartALL;          // Unaligned samples at start including other channel's
    int     iNumUnalignedEnd;               // Number of unaligned samples at end of block
    int     iAlignedBlocksOfFour;           // Number of aligned blocks of 4 samples

    // Determine number of aligned, unaligned samples
    assert(((c_iAlignMultiple - 1) & c_iAlignMultiple) == 0); // Pure power of 2
    iNumUnalignedStartALL = ((((U32) piSource + c_iAlignMultiple - 1) & ~(c_iAlignMultiple - 1)) -
        (U32) piSource) / sizeof(*piSource);
    iNumUnalignedStart = (iNumUnalignedStartALL + 1) / cChannel;
    iAlignedBlocksOfFour = (iSourceLength - iNumUnalignedStart) / 4; // DIV 4
    iNumUnalignedEnd = (iSourceLength - iNumUnalignedStart) & (4-1); // MOD 4

    // FIRST, calculate unaligned start samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedStart, rgiSrc,
        piFIROut, cChannel, 8);
    piSource += iNumUnalignedStartALL;
    assert(0 == (((U32)piSource) & (c_iAlignMultiple -1)));
    piFIROut += iNumUnalignedStart;

    // Set up staggered MMX coefficient arrays, then go MMX for aligned inputs
    // Align staggered MMX coefficient array on 32-byte boundary

    // Perf tests indicate that initializing MMX coefs EVERY TIME is slightly
    // faster than init once and much faster than hard-coded tables or
    // pre-initialized. Strange but true. Mem alignment is definitely a factor.

    InitMMXFilterCoefs(piMMXCoefs, iSizeOfMMXCoefs, rgiCoefNum, 8);
    if (iNumUnalignedStartALL & 1)
    {
        // ODD # samples from piSource to align point,
        // we use sizeof(I16) offset from align
        iShiftLeft = 0; // Desired input will get read into slots 1,3
    }
    else
    {
        // EVEN # samples from piSource to align point,
        // we use 0 offset from align
        iShiftLeft = 16; // Desired input will get read into slots 0,2
    }

    assert(0 == ((U32)piFIROut & 1)); // Verify we're on a DWORD boundary
    assert(0 == *piSrcCurr); // MMX history matches input order, it's NOT a doubled history

#define iShiftLeftASM   eax
#define piFIROutput     ebx
#define iLoopCtr        ecx
#define piSrcHistory    edx
#define piSrc           esi
#define piMMXCoefsASM   edi

#define SIZEOF_I16      2
#define SIZEOF_I32      4

    _asm
    {
        mov         iShiftLeftASM, iShiftLeft
        mov         iLoopCtr, iAlignedBlocksOfFour  // Load the index ctr
        mov         piSrcHistory, rgiSrc
        mov         piSrc, piSource
        movq        mm7, [piSrcHistory]                 // Oldest 4 samples (history): unaligned, but only done once
        movq        mm6, [piSrcHistory + 4*SIZEOF_I16]  // Second-oldest 4 samples (history): unaligned, but only done once
        mov         piFIROutput, piFIROut
        mov         piMMXCoefsASM, piMMXCoefs

MMXLOOP:
        // We used to pre-load input. DON'T DO THIS, last iteration can read past valid memory
        // Load in the input
        movq        mm4, [piSrc + 0*SIZEOF_I16]
        movq        mm3, [piSrc + 4*SIZEOF_I16]

        // Throw away the samples we don't want to construct next input vector
        pxor        mm1, mm1                    // Zero it
        movd        mm1, iShiftLeftASM
        movq        mm5, mm4
        pslld       mm5, mm1                    // Shift left by 16 or 0
        psrad       mm5, 16                     // Shift right is always 16
        pslld       mm3, mm1                    // Shift left by 16 or 0
        psrad       mm3, 16                     // Shift right is always 16
        packssdw    mm5, mm3                    // Combine the results to get 4 inputs

        // ---INTERATION 0---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*0] // MADD oldest history w/ 1-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*4] // MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*8] // MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*0], mm0   // Output results


        // ---INTERATION 1---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*12]// MADD oldest history w/ 2-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*16]// MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*20]// MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*1], mm0   // Output results


        // ---INTERATION 2---
        movq        mm2, mm7                            // Copy oldest history
        movq        mm1, mm6                            // Copy second-oldest history
        pmaddwd     mm2, [piMMXCoefsASM + SIZEOF_I16*24]// MADD oldest history w/ 3-offset coefs
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*28]// MADD second-oldest history
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*32]// MADD current input vector
        paddd       mm2, mm1                            // Sum mm1 + mm2 results
        paddd       mm0, mm2                            // Sum (mm1 + mm2) + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*2], mm0   // Output results


        // ---INTERATION 3---
        movq        mm1, mm6                            // Copy second-oldest history
        movq        mm0, mm5                            // Copy current input vector
        pmaddwd     mm1, [piMMXCoefsASM + SIZEOF_I16*36]// MADD second-oldest history w/ 4-offset coefs
        pmaddwd     mm0, [piMMXCoefsASM + SIZEOF_I16*40]// MADD current input vector
        paddd       mm0, mm1                            // Sum mm1 + mm0 results
        movq        mm1, mm0                            // Now sum horizontally: HI32(mm0) + LO32(mm0)
        psrlq       mm0, 32
        paddd       mm0, mm1

        movd        [piFIROutput + SIZEOF_I32*3], mm0   // Output results


        // Load next input, decrement ctrs, advance ptrs and loop around
        add         piSrc, 8*SIZEOF_I16
        add         piFIROutput, 4*SIZEOF_I32
        dec         iLoopCtr
        movq        mm7, mm6                    // Second-oldest history is now oldest
        movq        mm6, mm5                    // Current input vector is now 2nd oldest

        jnz         MMXLOOP

        // End of loop
        movq        [piSrcHistory], mm7                 // Oldest 4 samples (history): unaligned, but only done once
        movq        [piSrcHistory + 4*SIZEOF_I16], mm6  // Second-oldest 4 samples (history): unaligned, but only done once
        emms
    } // asm
    piSource += iAlignedBlocksOfFour * 4 * cChannel;
    piFIROut += iAlignedBlocksOfFour * 4;

    // Remember that piSource was made to point to 32-byte boundary, even if actual input sources
    // don't live on that boundary (eg, ABAB with first "A" on 32-byte boundary, our inputs are the "B"'s)
    // If we were reading the odd ones, we need to add one here
    if (iNumUnalignedStartALL & 1)
        piSource += 1;

    // Finish off the unaligned samples
    NTapFIR_Unaligned_MMX(rgiCoefNum, piSource, iNumUnalignedEnd, rgiSrc,
        piFIROut, cChannel, 8);
} // EightTapNUMERATOR_MMX_STEREO




//***************************************************************************
// FLOP DENOMINATORS
//***************************************************************************
void INLINE
FourTapDENOMINATOR_FLOP
                      (const Float *rgfltCoefDen,
                       const int position, const Float fltInvNumScale,
                       float *rgfltDst, I32 *piDstCurr,
                       const Int iSliceIncr, Float *pfltPower,
                       const Int iSourceLength, const I32 *piFIROutAligned)
{
    const Int c_minus1Mod3[] = {2, 0, 1};
    const int c_iSliceMask = iSliceIncr - 1;
    const int c_iASMLoopsPerSlice = iSliceIncr / 2;

    Int iDstCurr = *piDstCurr;
    Float fltSumSquared = 0;

    I32 iOuterLoops;

    // C setup for FLOP denominator
    assert(0 == iSliceIncr % 4); // ASM FLOP loop can only handle multiples of 4
    assert(0 == iSourceLength % iSliceIncr); // Won't do partial computations at the moment
    assert(iSourceLength >= iSliceIncr);
    iOuterLoops = iSourceLength / iSliceIncr;

#undef  ORDER
#define ORDER   3

#define iDstCurrPREV    eax
#define piFIROutput     ebx
#define iSliceLoopCtr   ecx
#define pfltPowerASM    ecx
#define pfltDstHistory  edx
#define iOuterLoopsASM  edx
#define iDstCurrASM     esi
#define iPositionASM    edi
#define pfltCoefDen     edi

// Need to do this because stupid assembler translates [edx+(esi+x)*4]
// to [edx+esi*4+x]. So this macro does the expansion manually.
#define HISTORYIDX(reg,offset)  ((reg)*SIZEOF_FLT + (offset)*SIZEOF_FLT)

#define SIZEOF_FLT      4

    // For ease of commenting, let
    //    A = fltInvNumScale * piFIROutAligned[j]
    //    B = rgfltCoefDen[1] * rgfltDst[iDstCurr + 0] = B1-1 * B1-2
    //    C = rgfltCoefDen[2] * rgfltDst[iDstCurr + 1] = C1-1 * C1-2
    //    D = rgfltCoefDen[3] * rgfltDst[iDstCurr + 2] = D1-1 * D1-2

    _asm
    {
        //-------------------------------------------------------------------
        // SETUP
        //-------------------------------------------------------------------
        mov     piFIROutput, piFIROutAligned
        mov     iSliceLoopCtr, c_iASMLoopsPerSlice
        mov     pfltDstHistory, rgfltDst
        mov     iDstCurrASM, iDstCurr
        mov     pfltCoefDen, rgfltCoefDen

        // NOTE: All NOP's have been omitted in the code below.

        //-------------------------------------------------------------------
        // PROLOG: Get us into steady-state so we can loop
        //-------------------------------------------------------------------

        // Clock 0: stack empty
        fild    dword ptr [piFIROutput]                         // Load A1-1

        // Clock 1: A1-1
        fld     fltInvNumScale                                  // Load A1-2

        // Clock 2: A1-2, A1-1
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C1-1
        
        // Clock 3: C1-1, A1-2, A1-1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C1-2
        fxch    st(2)

        // Clock 4: A1-2, C1-1, C1-2, A1-1
        fmulp   st(3), st(0)                                    // Compute A1
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D1-1

        // Clock 5: D1-1, C1-1, C1-2, A1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D1-2
        mov     iDstCurrASM, dword ptr [c_minus1Mod3 + iDstCurrASM*SIZEOF_I32] // Load next history slot

        // Clock 6: D1-2, D1-1, C1-1, C1-2, A1
        fild    dword ptr [piFIROutput + 1*SIZEOF_FLT]          // Load A2-1
        fxch    st(3)
        fmulp   st(4), st(0)                                    // Compute C1

        // Clock 7: D1-2, D1-1, A2-1, C1, A1
        fld     fltInvNumScale                                  // Load A2-2
        fxch    st(1)

        // Clock 8: D1-2, A2-2, D1-1, A2-1, C1, A1
        fmulp   st(2), st(0)                                    // Compute D1
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B1-1
        fxch    st(1)                                           // StackOrder: A2 in st(1) by clock 22

        // Clock 9: A2-2, B1-1, D1, A2-1, C1, A1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load B1-2 (we loaded new iDstCurrASM so use 1 instead of 0)
        fxch    st(4)

        // Clock 10: A2-1, A2-2, B1-1, D1, B1-2, C1, A1
        fmulp   st(1), st(0)                                    // Compute A2
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C2-1
        fxch    st(5)

        // Clock 11: C1, A2, B1-1, D1, B1-2, C2-1, A1
        fsubp   st(6), st(0)                                    // Compute A1-C1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C2-2
        fxch    st(2)

        // Clock 12: B1-1, A2, C2-2, D1, B1-2, C2-1, A1-C1
        fmulp   st(4), st(0)                                    // Compute B1
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D2-1
        mov     iDstCurrPREV, iDstCurrASM                       // Save this for later reference

        // Clock 13: D2-1, A2, C2-2, D1, B1, C2-1, A1-C1
        fxch    st(1)                                           // StackOrder: A2 in st(1) by clock 22
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D2-2
        fxch    st(4)

        // Clock 14: D1, A2, D2-1, C2-2, D2-2, B1, C2-1, A1-C1
        fsubp   st(7), st(0)                                    // Compute A1-C1-D1
        fld     fltSumSquared                                   // Load prevPwr

        // Clock 15: prevPwr, A2, D2-1, C2-2, D2-2, B1, C2-1, A1-C1-D1
        fxch    st(3)
        mov     iDstCurrASM, dword ptr [c_minus1Mod3 + iDstCurrASM*SIZEOF_I32] // Load next history slot

        // Clock 16: C2-2, A2, D2-1, prevPwr, D2-2, B1, C2-1, A1-C1-D1
        fmulp   st(6), st(0)                                    // Compute C2
        fldz                                                    // Load PWR0 = 0
        fxch    st(5)

        // Clock 17: B1, A2, D2-1, prevPwr, D2-2, PWR0, C2, A1-C1-D1
        fsubp   st(7), st(0)                                    // Compute A1-C1-D1-B1 = O1
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B2-1
        fxch    st(2)

        //-------------------------------------------------------------------
        // STEADY-STATE: Loop like the wind
        //-------------------------------------------------------------------

        // The FLOP stacks of clocks 41 and 18 MUST MATCH in order to to loop around
        // Comments marked as "StackOrder" indicate that a particular choice was made
        // in order to satisfy this requirement (or in order to avoid fxch's)

FLOPLOOP:
        // Clock 18: D2-1, A2, B2-1, prevPwr, D2-2, PWR0, C2, O1
        fmulp   st(4), st(0)                                    // Compute D2


        // Clock 19: A2, B2-1, prevPwr, D2, PWR0, C2, O1
        fild    dword ptr [piFIROutput + 2*SIZEOF_FLT]          // Load A3-1
        fxch    st(3)
        faddp   st(5), st(0)                                    // Compute prevPwr+PWR0=newPwr0

        // Clock 20: A2, B2-1, A3-1, D2, newPwr0, C2, O1
        fld     fltInvNumScale                                  // Load A3-2
        fxch    st(7)
        
        // Clock 21: O1, A2, B2-1, A3-1, D2, newPwr0, C2, A3-2
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, 0)] // Store O1 in proper slot
        fmul    st(2), st(0)                                    // Compute B2

        // Clock 22: O1, A2, B2, A3-1, D2, newPwr0, C2, A3-2
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, ORDER)] // Store O1 in proper slot
        fxch    st(1)
        fsubrp  st(6), st(0)                                    // Compute A2-C2

        // Clock 23: O1, B2, A3-1, D2, newPwr0, A2-C2, A3-2
        fmul    st(0), st(0)                                    // Compute PWR1
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C3-1
        fxch    st(5)

        // Clock 24: newPwr0, PWR1, B2, A3-1, D2, C3-1, A2-C2, A3-2
        fstp    fltSumSquared                                   // Store newPwr0
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C3-2
        fxch    st(3)

        // Clock 25: A3-1, PWR1, B2, C3-2, D2, C3-1, A2-C2, A3-2
        fmulp   st(7), st(0)                                    // Compute A3
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D3-1
        fxch    st(4)

        // Clock 26: D2, PWR1, B2, C3-2, D3-1, C3-1, A2-C2, A3
        fsubp   st(6), st(0)                                    // Compute A2-C2-D2
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D3-2
        fxch    st(3)

        // Clock 27: C3-2, PWR1, B2, D3-2, D3-1, C3-1, A2-C2-D2, A3
        fmulp   st(5), st(0)                                    // Compute C3
        fld     fltSumSquared                                   // Load prevPwr
        mov     iDstCurrPREV, iDstCurrASM                       // Save this for later reference

        // Clock 28: prevPwr, PWR1, B2, D3-2, D3-1, C3, A2-C2-D2, A3
        mov     iDstCurrASM, dword ptr [c_minus1Mod3 + iDstCurrASM*SIZEOF_I32] // Load next history slot
        fxch    st(3)

        // Clock 29: D3-2, PWR1, B2, prevPwr, D3-1, C3, A2-C2-D2, A3
        fmulp   st(4), st(0)                                    // Compute D3
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B3-1
        fxch    st(6)                                           // StackOrder: O2 in st(1) by clock 32

        // Clock 30: A2-C2-D2, PWR1, B2, prevPwr, D3, C3, B3-1, A3
        fsubrp  st(2), st(0)                                    // Compute A2-C2-D2-B2 = O2
        fxch    st(1)                                           // StackOrder: O2 in st(1) by clock 32

        // Clock 31: O2, PWR1, prevPwr, D3, C3, B3-1, A3
        fild    dword ptr [piFIROutput + 3*SIZEOF_FLT]          // Load A4-1
        fxch    st(2)
        faddp   st(3), st(0)                                    // Compute newPwr1

        // Clock 32: O2, A4-1, newPwr1, D3, C3, B3-1, A3
        fld     fltInvNumScale                                  // Load A4-2
        fxch    st(5)
        fsubp   st(7), st(0)                                    // Compute A3-C3

        // Clock 33: O2, A4-1, newPwr1, D3, A4-2, B3-1, A3-C3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, 0)] // Store O1 in proper slot
        fmul    st(5), st(0)                                    // Compute B3
        add     piFIROutput, 2*SIZEOF_FLT                       // Increment piFIROutput

        // Clock 34: O2, A4-1, newPwr1, D3, A4-2, B3, A3-C3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, ORDER)] // Store O1 in proper slot
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C4-1
        fxch    st(3)

        // Clock 35: newPwr1, O2, A4-1, C4-1, D3, A4-2, B3, A3-C3
        fstp    fltSumSquared                                   // Store newPwr1
        fmul    st(0), st(0)                                    // Compute PWR2
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C4-2

        // Clock 36: C4-2, PWR2, A4-1, C4-1, D3, A4-2, B3, A3-C3
        fxch    st(4)
        fsubp   st(7), st(0)                                    // Compute A3-C3-D3
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D4-1

        // Clock 37: D4-1, PWR2, A4-1, C4-1, C4-2, A4-2, B3, A3-C3-D3
        fxch    st(5)                                           // StackOrder: A4 in st(1)
        fmulp   st(2), st(0)                                    // Compute A4
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D4-2

        // Clock 38: D4-2, PWR2, A4, C4-1, C4-2, D4-1, B3, A3-C3-D3
        // Push the usual fxch out of this slot to slot 39, so we can do dec iSliceLoopCtr will in advance
        dec     iSliceLoopCtr                                   // Decrement loop counter: 2 outputs completed/iteration
        mov     iDstCurrPREV, iDstCurrASM                       // Save this for later reference
        mov     iDstCurrASM, dword ptr [c_minus1Mod3 + iDstCurrASM*SIZEOF_I32] // Load next history slot

        // Clock 39: C4-2, PWR2, A4, C4-1, D4-2, D4-1, B3, A3-C3-D3
        fxch    st(4)                                           // StackOrder: D4-2 in st(4)
        fmulp   st(3), st(0)                                    // Compute C4
        fld     fltSumSquared                                   // Load prevPwr

        // Clock 40: B3, PWR2, A4, C4, D4-2, D4-1, prevPwr, A3-C3-D3
        fxch    st(6)
        fsubp   st(7), st(0)                                    // Compute A3-C3-D3-B3 = O3
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B4-1

        // Clock 41-1: B4-1, PWR2, A4, C4, D4-2, D4-1, prevPwr, O3
        fxch    st(2)
        // Clock 41-2: A4, PWR2, B4-1, C4, D4-2, D4-1, prevPwr, O3
        fxch    st(1)
        // Clock 41-3: PWR2, A4, B4-1, C4, D4-2, D4-1, prevPwr, O3
        fxch    st(3)

        // Clock 42-1: C4, A4, B4-1, PWR2, D4-2, D4-1, prevPwr, O3
        fxch    st(6)
        // Clock 42-2: prevPwr, A4, B4-1, PWR2, D4-2, D4-1, C4, O3
        fxch    st(5)
        // Clock 42-3:   D4-1, A4, B4-1, PWR2,    D4-2, prevPwr, C4, O3
        // vs. Clock 18: D2-1, A2, B2-1, prevPwr, D2-2, PWR0,    C2, O1
        // The FLOP stacks of clocks 41 and 18 MUST MATCH in order to to loop around
        // Note PWR2/prevPwr will be added, so they can exchange slots with each other

        //-------------------------------------------------------------------
        // END OF LOOP: Do some housekeeping, don't worry about pipeline scheduling
        //-------------------------------------------------------------------

        // Clock EOL-1 (use different numbering scheme for end-of-loop)
        jnz     FLOPLOOP

        // If we've reached this point, we've outputed iSliceIncr number of samples
        // prevPwr + PWR2 is the value we wish to write to pfltPower[position]
        // After doing this, be sure to set PWR2 = 0 and prevPwr = 0 on FLOP stack
        // Calculate power, write it out, and clear the running sum (fltSumSquared)
        mov     iOuterLoopsASM, iOuterLoops              // Check for outer loop termination

        // Clock EOL-2:
        dec     iOuterLoopsASM
        mov     iOuterLoops, iOuterLoopsASM              // Write iOuterLoops back to local var
        jz      EPILOG                                   // Done with loops, go to epilog

        // Clock EOL-3
        mov     iPositionASM, position
        fxch    st(3)                                    // Load PWR2 to st(0)
        faddp   st(5), st(0)                             // Add prevPwr + PWR2 to get newPwr2

        // Clock EOL-4
        mov     pfltPowerASM, pfltPower
        fldz
        fxch    st(5)                                    // Replace prevPwr with 0

        // Clock EOL-5
        fstp    dword ptr [pfltPowerASM + iPositionASM*SIZEOF_FLT] // Store newPwr2 in pfltPower[position]
        fldz                                             // Replace PWR2 with 0
        lea     pfltPowerASM, [pfltPowerASM + 3*SIZEOF_FLT] // Increment pfltPower

        // Clock EOL-6
        mov     pfltPower, pfltPowerASM                  // Write pfltPower back to local var
        mov     pfltCoefDen, rgfltCoefDen                // Restore inner loop var
        cmp     iOuterLoopsASM, 1                        // Check if this is the last time around

        // Clock EOL-7
        mov     fltSumSquared, 0                         // Clear the running sum
        mov     iOuterLoopsASM, -1                       // Prep for CMOV
        mov     iSliceLoopCtr, 0                         // Zero this without disturbing flags

        // Clock EOL-8
        fxch    st(3)                                    // Restore original FLOP stack order
        cmove   iSliceLoopCtr, iOuterLoopsASM            // If last time, do 1 less iteration

        // Clock EOL-9
        add     iSliceLoopCtr, c_iASMLoopsPerSlice
        mov     pfltDstHistory, rgfltDst                 // Restore inner loop var
        jmp     FLOPLOOP

        //-------------------------------------------------------------------
        // EPILOG: Finish off the last operation, ignore pipeline crap
        //-------------------------------------------------------------------

EPILOG:

        // Clock X
        mov     pfltDstHistory, rgfltDst                 // Restore inner loop var

        // Clock E-1: D4-1, A4, B4-1, PWR2, D4-2, prevPwr, C4, O3
        fmulp   st(4), st(0)                                    // Compute D4
        fxch    st(2)

        // Clock E-2: PWR2, B4-1, A4, D4, prevPwr, C4, O3
        faddp   st(4), st(0)                                    // Compute newPwr2
        fxch    st(5)

        // Clock E-3: O3, A4, D4, newPwr2, C4, B4-1
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, 0)] // Store O3 in proper slot
        fmul    st(5), st(0)                                    // Compute B4

        // Clock E-4: O3, A4, D4, newPwr2, C4, B4
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrPREV, ORDER)] // Store O3 in proper slot
        fxch    st(1)
        fsubrp  st(4), st(0)                                    // Compute A4-C4

        // Clock E-5: O3, D4, newPwr2, A4-C4, B4
        fmul    st(0), st(0)                                    // Compute PWR3
        fxch    st(1)

        // Clock E-6: D4, PWR3, newPwr2, A4-C4, B4
        fsubp   st(3), st(0)                                    // Compute A4-C4-D4
        fxch    st(3)

        // Clock E-7: B4, newPwr2, A4-C4-D4, PWR3
        fsubp   st(2), st(0)                                    // Compute A4-C4-D4-B4 = O4

        // Clock E-8: newPwr2, O4, PWR3
        faddp   st(2), st(0)                                    // Compute newPwr2+PWR3=newPwr3

        // Clock E-9: O4, newPwr3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)] // Store O4 in proper slot

        // Clock E-10: O4, newPwr3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, ORDER)] // Store O4 in proper slot
        fmul    st(0), st(0)                                    // Compute PWR4

        // Clock E-11: PWR4, newPwr3
        faddp   st(1), st(0)                                    // Compute newPwr3+PWR4=newPwr4

        // Clock E-12: newPwr4
        mov     iPositionASM, position
        mov     pfltPowerASM, pfltPower
        fst     dword ptr [pfltPowerASM + iPositionASM*SIZEOF_FLT] // Store newPwr4 to pfltPower[position]
        fstp    fltSumSquared                                   // Store newPwr4

        //-------------------------------------------------------------------
        // SHUT DOWN
        //-------------------------------------------------------------------

        // Save current dst
        mov     iDstCurr, iDstCurrASM

    } // asm

#undef ORDER

    *piDstCurr = iDstCurr;
} // FourTapDENOMINATOR_FLOP



void INLINE
FiveTapDENOMINATOR_FLOP
                      (const Float *rgfltCoefDen,
                       const int position, const Float fltInvNumScale,
                       float *rgfltDst, I32 *piDstCurr,
                       const Int iSliceIncr, Float *pfltPower,
                       const Int iSourceLength, const I32 *piFIROutAligned)
{
    int j;
    Float fltSumSquared = 0;

    // Translate
    const I32 *output = piFIROutAligned;

    FIVE_TAP_IIR_FLOAT_DENOMINATOR_VARIABLES
    FIVE_TAP_IIR_FLOAT_DENOMINATOR(fltInvNumScale, position)
}



void INLINE
SevenTapDENOMINATOR_FLOP
                       (const Float *rgfltCoefDen, const int position,
                        const Float fltInvNumScale,
                        float *rgfltDst, I32 *piDstCurr,
                        const Int iSliceIncr, Float *pfltPower,
                        const Int iSourceLength, const I32 *piFIROutAligned)
{
    const Int c_minus1Mod6[] = {5, 0, 1, 2, 3, 4};
    const int c_iSliceMask = iSliceIncr - 1;
    const int c_iASMLoopsPerSlice = iSliceIncr / 2;

    Int iDstCurr = *piDstCurr;
    Float fltSumSquared = 0;
    I32 iOuterLoops;

    // C setup for FLOP denominator
    assert(0 == iSliceIncr % 4); // ASM FLOP loop can only handle multiples of 4
    assert(0 == iSourceLength % iSliceIncr); // Won't do partial computations at the moment
    assert(iSourceLength >= iSliceIncr);
    iOuterLoops = iSourceLength / iSliceIncr;

#undef  ORDER
#define ORDER   6

#undef iPositionASM

#define cMinus1Mod6     eax
#define iPositionASM    eax
#define piFIROutput     ebx
#define iSliceLoopCtr   ecx
#define pfltPowerASM    ecx
#define pfltDstHistory  edx
#define iOuterLoopsASM  edx
#define iDstCurrASM     esi
#define pfltCoefDen     edi

// Need to do this because stupid assembler translates [edx+(esi+x)*4]
// to [edx+esi*4+x]. So this macro does the expansion manually.
#define HISTORYIDX(reg,offset)  ((reg)*SIZEOF_FLT + (offset)*SIZEOF_FLT)

#define SIZEOF_FLT      4

    // For ease of commenting, let
    //    A = fltInvNumScale * piFIROutAligned[j]
    //    B = rgfltCoefDen[1] * rgfltDst[iDstCurr + 0] = B1-1 * B1-2
    //    C = rgfltCoefDen[2] * rgfltDst[iDstCurr + 1] = C1-1 * C1-2
    //    D = rgfltCoefDen[3] * rgfltDst[iDstCurr + 2] = D1-1 * D1-2
    //    E = rgfltCoefDen[4] * rgfltDst[iDstCurr + 3] = E1-1 * E1-2
    //    F = rgfltCoefDen[5] * rgfltDst[iDstCurr + 4] = F1-1 * F1-2
    //    G = rgfltCoefDen[6] * rgfltDst[iDstCurr + 5] = G1-1 * G1-2
    _asm
    {
        //-------------------------------------------------------------------
        // SETUP
        //-------------------------------------------------------------------
        lea     cMinus1Mod6, c_minus1Mod6
        mov     piFIROutput, piFIROutAligned
        mov     iSliceLoopCtr, c_iASMLoopsPerSlice
        mov     pfltDstHistory, rgfltDst
        mov     iDstCurrASM, iDstCurr
        mov     pfltCoefDen, rgfltCoefDen

        //-------------------------------------------------------------------
        // PROLOG: Get us into steady-state so we can loop
        //-------------------------------------------------------------------

        // Initially I chose to do fxch in the D0 (decoder 0) slot. Later I realized
        // that fst(p) is a 2 micro-op instruction, and so I moved fxch to the D1/D2
        // slot of the previous clock so fst(p) could be placed in D0 slot to avoid
        // decode penalties (4-1-1 decode pattern). I only did this for the looping portion,
        // and left the prolog and epilog alone. Hence the change of style at clock 18.

        // Clock 0: stack empty
        fild    dword ptr [piFIROutput]                         // Load A1-1
        //nop
        //nop

        // Clock 1: A1-1
        fld     fltInvNumScale                                  // Load A1-2
        //nop
        //nop

        // Clock 2: A1-2, A1-1
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C1-1
        //nop
        //nop

        // Clock 3: C1-1, A1-2, A1-1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C1-2
        //nop
        //nop

        // Clock 4: C1-2, C1-1, A1-2, A1-1
        fxch    st(2)
        fmulp   st(3), st(0)                                    // Compute A1
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D1-1

        // Clock 5: D1-1, C1-1, C1-2, A1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D1-2
        //nop
        //nop

        // Clock 6: D1-2, D1-1, C1-1, C1-2, A1
        fxch    st(2)
        fmulp   st(3), st(0)                                    // Compute C1
        fld     dword ptr [pfltCoefDen + 4*SIZEOF_FLT]          // Load E1-1

        // Clock 7: E1-1, D1-1, D1-2, C1, A1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 3)] // Load E1-2
        //nop
        //nop

        // Clock 8: E1-2, E1-1, D1-1, D1-2, C1, A1
        fxch    st(2)
        fmulp   st(3), st(0)                                    // Compute D1
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B1-1

        // Clock 9: B1-1, E1-1, E1-2, D1, C1, A1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)] // Load B1-2
        //nop
        //nop

        // Clock 10: B1-2, B1-1, E1-1, E1-2, D1, C1, A1
        fxch    st(2)
        fmulp   st(3), st(0)                                    // Compute E1
        //nop

        // Clock 11: B1-1, B1-2, E1, D1, C1, A1
        fxch    st(4)
        fsubp   st(5), st(0)                                    // Compute A1-C1
        fldz                                                    // Load PWR0 (=0)

        // Clock 12: PWR0(=0), B1-2, E1, D1, B1-1, A1-C1
        fxch    st(1)
        fmulp   st(4), st(0)                                    // Compute B1
        fld     dword ptr [pfltCoefDen + 5*SIZEOF_FLT]          // Load F1-1

        // Clock 13: F1-1, PWR0, E1, D1, B1, A1-C1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 4)] // Load F1-2
        //nop
        //nop

        // Clock 14: F1-2, F1-1, PWR0, E1, D1, B1, A1-C1
        fld     dword ptr [pfltCoefDen + 6*SIZEOF_FLT]          // Load G1-1
        add     piFIROutput, SIZEOF_I32                         // Increment piFIROutput
        //nop

        // Clock 15: G1-1, F1-2, F1-1, PWR0, E1, D1, B1, A1-C1
        fxch    st(5)
        fsubp   st(7), st(0)                                    // Compute A1-C1-D1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 5)] // Load G1-2

        // Clock 16: G1-2, F1-2, F1-1, PWR0, E1, G1-1, B1, A1-C1-D1
        fxch    st(1)
        fmulp   st(2), st(0)                                    // Compute F1
        fld     fltSumSquared                                   // Load prevPwr

        // Clock 17: prevPwr, G1-2, F1, PWR0, E1, G1-1, B1, A1-C1-D1
        mov     iDstCurrASM, dword ptr [cMinus1Mod6 + iDstCurrASM*SIZEOF_I32] // Load next history slot
        fxch    st(4)                                           // Move fxch from D0 to D1/D2 slot
        //nop

        //-------------------------------------------------------------------
        // STEADY-STATE: Loop like the wind
        //-------------------------------------------------------------------

        // The FLOP stacks of clocks 53 and 18 MUST MATCH in order to to loop around
        // Comments marked as "StackOrder" indicate that a particular choice was made
        // in order to satisfy this requirement

FLOPLOOP:
        // Clock 18: E1, G1-2, F1, PWR0, prevPwr, G1-1, B1, A1-C1-D1
        faddp   st(6), st(0)                                    // Compute E1+B1
        fild    dword ptr [piFIROutput]                         // Load A2-1
        fxch    st(1)

        // Clock 19: G1-2, A2-1, F1, PWR0, prevPwr, G1-1, E1+B1, A1-C1-D1
        fmulp   st(5), st(0)                                    // Compute G1
        fld     fltInvNumScale                                  // Load A2-2
        fxch    st(3)

        // Clock 20: PWR0, A2-1, F1, A2-2, prevPwr, G1, E1+B1, A1-C1-D1
        faddp   st(4), st(0)                                    // Compute newPwr0 = PWR0+prevPwr
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C2-1
        fxch    st(6)

        // Clock 21: E1+B1, A2-1, F1, A2-2, newPwr0, G1, C2-1, A1-C1-D1
        fsubp   st(7), st(0)                                    // Compute A1-C1-D1-(E1+B1)
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C2-2
        fxch    st(1)

        // Clock 22: A2-1, C2-2, F1, A2-2, newPwr0, G1, C2-1, A1-C1-D1-(E1+B1)
        fmulp   st(3), st(0)                                    // Compute A2
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D2-1
        fxch    st(4)

        // Clock 23: newPwr0, C2-2, F1, A2, D2-1, G1, C2-1, A1-C1-D1-(E1+B1)
        fstp    fltSumSquared                                   // Store newPwr0
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D2-2
        fxch    st(1)

        // Clock 24: C2-2, D2-2, F1, A2, D2-1, G1, C2-1, A1-C1-D1-(E1+B1)
        fmulp   st(6), st(0)                                    // Compute C2
        fld     dword ptr [pfltCoefDen + 4*SIZEOF_FLT]          // Load E2-1
        fxch    st(2)

        // Clock 25: F1, D2-2, E2-1, A2, D2-1, G1, C2, A1-C1-D1-(E1+B1)
        faddp   st(5), st(0)                                    // Compute F1+G1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 3)] // Load E2-2
        fxch    st(1)

        // Clock 26: D2-2, E2-2, E2-1, A2, D2-1, F1+G1, C2, A1-C1-D1-(E1+B1)
        fmulp   st(4), st(0)                                    // Compute D2
        fld     dword ptr [pfltCoefDen + 5*SIZEOF_FLT]          // Load F2-1
        //nop

        // Clock 27: F2-1, E2-2, E2-1, A2, D2, F1+G1, C2, A1-C1-D1-(E1+B1)
        add     piFIROutput, SIZEOF_I32                         // Increment piFIROutput
        fxch    st(1)
        //nop

        // Clock 28: E2-2, F2-1, E2-1, A2, D2, F1+G1, C2, A1-C1-D1-(E1+B1)
        fmulp   st(2), st(0)                                    // Compute E2
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 4)] // Load F2-2
        fxch    st(5)

        // Clock 29: F1+G1, F2-1, E2, A2, D2, F2-2, C2, A1-C1-D1-(E1+B1)
        fsubp   st(7), st(0)                                    // Compute O1 = A1-C1-D1-(E1+B1)-(F1+G1)
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B2-1
        fxch    st(3)                                           // StackOrder: B3 in st(6)

        // Clock 30: A2, F2-1, E2, B2-1, D2, F2-2, C2, O1
        fsubrp  st(6), st(0)                                    // Compute A2-C2
        fld     dword ptr [pfltCoefDen + 6*SIZEOF_FLT]          // Load G2-1
        fxch    st(1)

        // Clock 31: F2-1, G2-1, E2, B2-1, D2, F2-2, A2-C2, O1
        fmulp   st(5), st(0)                                    // Compute F2
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 5)] // Load G2-2
        fxch    st(7)

        // Clock 32: O1, G2-1, E2, B2-1, D2, F2, A2-C2, G2-2
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)] // Store O1 in proper slot
        //nop
        //nop

        // Clock 33: O1, G2-1, E2, B2-1, D2, F2, A2-C2, G2-2
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, ORDER)] // Store O1 in proper slot (aliased)
        fmul    st(3), st(0)                                    // Compute B2
        fxch    st(2)                                           // StackOrder: E3 in st(0)

        // Clock 34: E2, G2-1, O1, B2, D2, F2, A2-C2, G2-2
        faddp   st(4), st(0)                                    // Compute D2+E2
        fild    dword ptr [piFIROutput]                         // Load A3-1
        fxch    st(2)

        // Clock 35: O1, G2-1, A3-1, B2, D2+E2, F2, A2-C2, G2-2
        fmul    st(0), st(0)                                    // Compute PWR1=O1^2
        mov     iDstCurrASM, dword ptr [cMinus1Mod6 + iDstCurrASM*SIZEOF_I32] // Load next history slot
        fxch    st(5)                                           // StackOrder: B3 in st(6)

        // Clock 36: F2, G2-1, A3-1, B2, D2+E2, PWR1, A2-C2, G2-2
        fsubp   st(6), st(0)                                    // Compute A2-C2-F2
        fld     fltInvNumScale                                  // Load A3-2
        fxch    st(7)                                           // StackOrder: A3-C3-D3 in st(7)

        // Clock 37: G2-2, G2-1, A3-1, B2, D2+E2, PWR1, A2-C2-F2, A3-2
        fmulp   st(1), st(0)                                    // Compute G2
        fld     fltSumSquared                                   // Load prevPwr
        fxch    st(4)                                           // StackOrder: E3 in st(0) via prevPwr in st(4)

        // Clock 38: D2+E2, G2, A3-1, B2, prevPwr, PWR1, A2-C2-F2, A3-2
        faddp   st(3), st(0)                                    // Compute D2+E2+B2
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C3-1
        fxch    st(2)                                           // StackOrder: A3-C3-D3 in st(7)

        // Clock 39: A3-1, G2, C3-1, D2+E2+B2, prevPwr, PWR1, A2-C2-F2, A3-2
        fmulp   st(7), st(0)                                    // Compute A3
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C3-2
        fxch    st(5)                                           // StackOrder: E3 in st(0) via prevPwr in st(4)

        // Clock 40: PWR1, G2, C3-1, D2+E2+B2, prevPwr, C3-2, A2-C2-F2, A3
        faddp   st(4), st(0)                                    // Compute newPwr1 = PWR1+prevPwr
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D3-1
        fxch    st(3)                                           // StackOrder: B3 in st(6)

        // Clock 41: D2+E2+B2, G2, C3-1, D3-1, newPwr1, C3-2, A2-C2-F2, A3
        fsubp   st(6), st(0)                                    // Compute A2-C2-F2-(D2+E2+B2)
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D3-2
        fxch    st(5)                                           // StackOrder: F3 in st(2)

        // Clock 42: C3-2, G2, C3-1, D3-1, newPwr1, D3-2, A2-C2-F2-(D2+E2+B2), A3
        fmulp   st(2), st(0)                                    // Compute C3
        fld     dword ptr [pfltCoefDen + 4*SIZEOF_FLT]          // Load E3-1
        fxch    st(4)

        // Clock 43: newPwr1, G2, C3, D3-1, E3-1, D3-2, A2-C2-F2-(D2+E2+B2), A3
        fstp    fltSumSquared                                   // Store newPwr1
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 3)] // Load E3-2
        fxch    st(5)                                           // StackOrder: PWR2 in st(3)

        // Clock 44: D3-2, G2, C3, D3-1, E3-1, E3-2, A2-C2-F2-(D2+E2+B2), A3
        fmulp   st(3), st(0)                                    // Compute D3
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B3-1
        fxch    st(6)                                           // StackOrder: B3 in st(6)

        // Clock 45: A2-C2-F2-(D2+E2+B2), G2, C3, D3, E3-1, E3-2, B3-1, A3
        fsubrp  st(1), st(0)                                    // Compute O2 = A2-C2-F2-(D2+E2+B2)-G2
        fld     dword ptr [pfltCoefDen + 5*SIZEOF_FLT]          // Load F3-1
        fxch    st(5)                                           // StackOrder: E3 in st(0) via prevPwr in st(4)

        // Clock 46: E3-2, O2, C3, D3, E3-1, F3-1, B3-1, A3
        fmulp   st(4), st(0)                                    // Compute E3
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 4)] // Load F3-2
        fxch    st(2)                                           // StackOrder: A3-C3-D3 in st(7)

        // Clock 47: C3, O2, F3-2, D3, E3, F3-1, B3-1, A3
        fsubp   st(7), st(0)                                    // Compute A3-C3
        fld     dword ptr [pfltCoefDen + 6*SIZEOF_FLT]          // Load G3-1
        fxch    st(1)

        // Clock 48: O2, G3-1, F3-2, D3, E3, F3-1, B3-1, A3-C3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)] // Store O2 in proper slot
        fmul    st(6), st(0)                                    // Compute B3
        //nop

        // Clock 49: O2, G3-1, F3-2, D3, E3, F3-1, B3, A3-C3
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, ORDER)] // Store O2 in proper slot (aliased)
        add     piFIROutput, SIZEOF_I32                         // Increment piFIROutput
        //nop

        // Clock 50: O2, G3-1, F3-2, D3, E3, F3-1, B3, A3-C3
        fmul    st(0), st(0)                                    // Compute PWR2 = O2^2
        fxch    st(3)                                           // StackOrder: PWR2 in st(3), A3-C3-D3 in st(7)
        dec     iSliceLoopCtr                                   // Decrement loop counter: 2 outputs completed/iteration (3rd output started)

        // Clock 51: D3, G3-1, F3-2, PWR2, E3, F3-1, B3, A3-C3
        fsubp   st(7), st(0)                                    // Compute A3-C3-D3
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 5)] // Load G3-2
        fxch    st(5)                                           // StackOrder: F3 in st(2)

        // Clock 52: F3-1, G3-1, F3-2, PWR2, E3, G3-2, B3, A3-C3-D3
        fmulp   st(2), st(0)                                    // Compute F3
        fld     fltSumSquared                                   // Load prevPwr
        fxch    st(4)                                           // StackOrder: E3 in st(0)

        // Clock 53:     E3, G3-1, F3, PWR2, prevPwr, G3-2, B3, A3-C3-D3
        // vs. Clock 18: E1, G1-2, F1, PWR0, prevPwr, G1-1, B1, A1-C1-D1
        // The FLOP stacks of clocks 53 and 18 MUST MATCH in order to to loop around
        // NB: the reversal of G3-1, G3-2 vs. G1-2, G1-1 have no ill effect (whew!)

        //-------------------------------------------------------------------
        // END OF LOOP: Do some housekeeping, don't worry about pipeline scheduling
        //-------------------------------------------------------------------

        // Clock EOL-1 (use different numbering scheme for end-of-loop)
        mov     iDstCurrASM, dword ptr [cMinus1Mod6 + iDstCurrASM*SIZEOF_I32] // Load next history slot
        jnz     FLOPLOOP

        // If we've reached this point, we've outputed iSliceIncr number of samples
        // prevPwr + PWR2 is the value we wish to write to pfltPower[position]
        // After doing this, be sure to set PWR2 = 0 and prevPwr = 0 on FLOP stack
        // Calculate power, write it out, and clear the running sum (fltSumSquared)
        mov     iOuterLoopsASM, iOuterLoops              // Check for outer loop termination

        // Clock EOL-2:
        dec     iOuterLoopsASM
        mov     iOuterLoops, iOuterLoopsASM              // Write iOuterLoops back to local var
        jz      EPILOG                                   // Done with loops, go to epilog

        // Clock EOL-3
        mov     iPositionASM, position
        fxch    st(4)                                    // Load prevPwr to st(0)
        faddp   st(3), st(0)                             // Add prevPwr + PWR2 to get newPwr2

        // Clock EOL-4
        mov     pfltPowerASM, pfltPower
        fldz
        fxch    st(3)                                    // Replace PWR2 with 0        

        // Clock EOL-5
        fstp    dword ptr [pfltPowerASM + iPositionASM*SIZEOF_FLT] // Store newPwr2 in pfltPower[position]
        fldz                                             // Replace prevPwr with 0
        lea     pfltPowerASM, [pfltPowerASM + 3*SIZEOF_FLT] // Increment pfltPower

        // Clock EOL-6
        mov     pfltPower, pfltPowerASM                  // Write pfltPower back to local var
        lea     cMinus1Mod6, c_minus1Mod6                // Restore inner loop var
        cmp     iOuterLoopsASM, 1                        // Check if this is the last time around

        // Clock EOL-7
        mov     fltSumSquared, 0                         // Clear the running sum
        mov     iOuterLoopsASM, -1                       // Prep for CMOV
        mov     iSliceLoopCtr, 0                         // Zero this without disturbing flags

        // Clock EOL-8
        fxch    st(4)                                    // Restore original FLOP stack order
        cmove   iSliceLoopCtr, iOuterLoopsASM            // If last time, do 1 less iteration
        //nop

        // Clock EOL-9
        add     iSliceLoopCtr, c_iASMLoopsPerSlice
        mov     pfltDstHistory, rgfltDst                 // Restore inner loop var
        jmp     FLOPLOOP

        //-------------------------------------------------------------------
        // EPILOG: Finish off the last operation
        //-------------------------------------------------------------------

EPILOG:
        // Clock X
        mov     pfltDstHistory, rgfltDst                 // Restore inner loop var
        //nop
        //nop

        // Clock 53: E3, G3-1, F3, PWR2, prevPwr, G3-2, B3, A3-C3-D3
        faddp   st(6), st(0)                                    // Compute E3+B3
        fild    dword ptr [piFIROutput]                         // Load A4-1
        //nop

        // Clock 54: A4-1, G3-2, F3, PWR2, prevPwr, G3-1, E3+B3, A3-C3-D3
        fxch    st(1)
        fmulp   st(5), st(0)                                    // Compute G3
        fld     fltInvNumScale                                  // Load A4-2

        // Clock 55: A4-2, A4-1, F3, PWR2, prevPwr, G3, E3+B3, A3-C3-D3
        fxch    st(3)
        faddp   st(4), st(0)                                    // Compute newPwr2 = PWR2+prevPwr
        fld     dword ptr [pfltCoefDen + 2*SIZEOF_FLT]          // Load C4-1

        // Clock 56: C4-1, A4-1, F3, A4-2, newPwr2, G3, E3+B3, A3-C3-D3
        fxch    st(6)
        fsubp   st(7), st(0)                                    // Compute A3-C3-D3-(E3+B3)
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 1)] // Load C4-2

        // Clock 57: C4-2, A4-1, F3, A4-2, newPwr2, G3, C4-1, A3-C3-D3-(E3+B3)
        fxch    st(1)
        fmulp   st(3), st(0)                                    // Compute A4
        fld     dword ptr [pfltCoefDen + 3*SIZEOF_FLT]          // Load D4-1

        // Clock 58: D4-1, C4-2, F3, A4, newPwr2, G3, C4-1, A3-C3-D3-(E3+B3)
        fxch    st(4)

        // This will take another clock cycle. It's not worth it to fix this.
        fstp    fltSumSquared                                   // Store newPwr2
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 2)] // Load D4-2

        // Clock 59: D4-2, C4-2, F3, A4, D4-1, G3, C4-1, A3-C3-D3-(E3+B3)
        fxch    st(1)
        fmulp   st(6), st(0)                                    // Compute C4
        fld     dword ptr [pfltCoefDen + 4*SIZEOF_FLT]          // Load E4-1

        // Clock 60: E4-1, D4-2, F3, A4, D4-1, G3, C4, A3-C3-D3-(E3+B3)
        fxch    st(2)
        faddp   st(5), st(0)                                    // Compute F3+G3
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 3)] // Load E4-2

        // Clock 61: E4-2, D4-2, E4-1, A4, D4-1, F3+G3, C4, A3-C3-D3-(E3+B3)
        fxch    st(1)
        fmulp   st(4), st(0)                                    // Compute D4
        fld     dword ptr [pfltCoefDen + 5*SIZEOF_FLT]          // Load F4-1

        // Clock 62: F4-1, E4-2, E4-1, A4, D4, F3+G3, C4, A3-C3-D3-(E3+B3)
        //nop
        //nop
        //nop

        // Clock 63: F4-1, E4-2, E4-1, A4, D4, F3+G3, C4, A3-C3-D3-(E3+B3)
        fxch    st(1)
        fmulp   st(2), st(0)                                    // Compute E4
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 4)] // Load F4-2

        // Clock 64: F4-2, F4-1, E4, A4, D4, F3+G3, C4, A3-C3-D3-(E3+B3)
        fxch    st(5)
        fsubp   st(7), st(0)                                    // Compute O3 = A3-C3-D3-(E3+B3)-(F3+G3)
        fld     dword ptr [pfltCoefDen + 1*SIZEOF_FLT]          // Load B4-1

        // Clock 65: B4-1, F4-1, E4, A4, D4, F4-2, C4, O3
        fxch    st(6)
        fsubp   st(3), st(0)                                    // Compute A4-C4
        fld     dword ptr [pfltCoefDen + 6*SIZEOF_FLT]          // Load G4-1

        // Clock 66: G4-1, F4-1, E4, A4-C4, D4, F4-2, B4-1, O3
        fxch    st(1)
        fmulp   st(5), st(0)                                    // Compute F4
        fld     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 5)] // Load G4-2

        // Clock 67: G4-2, G4-1, E4, A4-C4, D4, F4, B4-1, O3
        fxch    st(7)

        // This will take another clock cycle. It's not worth it to fix this.
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)] // Store O3 in proper slot
        //nop

        // Clock 68: O3, G4-1, E4, A4-C4, D4, F4, B4-1, G4-2
        fmul    st(6), st(0)                                    // Compute B4

        // This will take another clock cycle. It's not worth it to fix this.
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, ORDER)] // Store O3 in proper slot (aliased)
        
        // At this point, ignore pipelining and dump the remaining instructions
        fmul    st(0), st(0)                                    // Compute PWR3

        // Stack: PWR3, G4-1, E4, A4-C4, D4, F4, B4, G4-2
        fxch    st(2)
        faddp   st(4), st(0)                                    // Compute D4+E4

        // Stack: G4-1, PWR3, A4-C4, D4+E4, F4, B4, G4-2
        fxch    st(4)
        fsubp   st(2), st(0)                                    // Compute A4-C4-F4

        // Stack: PWR3, A4-C4-F4, D4+E4, G4-1, B4, G4-2
        fld     fltSumSquared                                   // Load prevPwr

        // Stack: prevPwr, PWR3, A4-C4-F4, D4+E4, G4-1, B4, G4-2
        fxch    st(4)
        fmulp   st(6), st(0)                                    // Compute G4

        // Stack: PWR3, A4-C4-F4, D4+E4, prevPwr, B4, G4
        fxch    st(4)
        faddp   st(2), st(0)                                    // Compute D4+E4+B4

        // Stack: A4-C4-F4, D4+E4+B4, prevPwr, PWR3, G4
        fxch    st(2)
        faddp   st(3), st(0)                                    // Compute newPwr3 = PWR3+prevPwr

        // Stack: D4+E4+B4, A4-C4-F4, newPwr3, G4
        fsubp   st(1), st(0)                                    // Compute A4-C4-F4-(D4+E4+B4)

        // Stack: A4-C4-F4-(D4+E4+B4), newPwr3, G4
        fxch    st(1)
        fstp    fltSumSquared                                   // Store newPwr3

        // Stack: A4-C4-F4-(D4+E4+B4), G4
        fsubrp  st(1), st(0)                                    // Compute O4 = A4-C4-F4-(D4+E4+B4)-G4
        mov     iDstCurrASM, dword ptr [cMinus1Mod6 + iDstCurrASM*SIZEOF_I32] // Load next history slot

        // Stack: O4
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, 0)]; // Store O4 in proper slot
        fst     dword ptr [pfltDstHistory + HISTORYIDX(iDstCurrASM, ORDER)]; // Store O4 in proper slot (aliased)
        fmul    st(0), st(0)                                    // Compute PWR4 = O4^2

        // Stack: PWR4
        fld     fltSumSquared                                   // Load prevPwr

        // Stack: prevPwr, PWR4
        faddp   st(1), st(0)                                    // Compute newPwr4 = PWR4+prevPwr

        // Stack: newPwr4
        mov     iPositionASM, position
        mov     pfltPowerASM, pfltPower
        fst     dword ptr [pfltPowerASM + iPositionASM*SIZEOF_FLT] // Store newPwr4 to pfltPower[position]
        fstp    fltSumSquared                                   // Store newPwr4

        //-------------------------------------------------------------------
        // SHUT DOWN
        //-------------------------------------------------------------------

        // Save current dst
        mov     iDstCurr, iDstCurrASM
    }

#undef ORDER

    *piDstCurr = iDstCurr;
} // SevenTapDENOMINATOR_FLOP




//***************************************************************************
// FILTER WRAPPER FUNCTIONS
// These functions provide aligned memory for the MMX filters to operate with.
//***************************************************************************

Void INLINE FourFiveFourSTEREO(const Int cCoefLow, const Int cCoefMed, const Int cCoefHi,
                               FLOATORU32* pfltPower, const I16 *piSource,
                               const Int iSourceLength, const Int iSliceWidth,
                               const Int iSliceIncr,
                               const I16 cChannel, I16 *piTransientSrcLow,
                               Int *piTransientSrcLowCurr, FLOATORI32 *pfltTransientDstLow,
                               Int *piTransientDstLowCurr,
                               I16 *piTransientSrcMed,Int *piTransientSrcMedCurr,
                               FLOATORI32 *pfltTransientDstMed, Int *piTransientDstMedCurr,
                               I16 *piTransientSrcHi, Int *piTransientSrcHiCurr,
                               FLOATORI32 *pfltTransientDstHi, Int *piTransientDstHiCurr,
                               const I16 *rgiCoefNumLow, const Float fltInvScaleNumLow,
                               const Float *rgfltCoefDenLow,
                               const I16 *rgiCoefNumMed, const Float fltInvScaleNumMed,
                               const Float *rgfltCoefDenMed,
                               const I16 *rgiCoefNumHi, const Float fltInvScaleNumHi,
                               const Float *rgfltCoefDenHi)
{
    // Get us some local 32-byte aligned arrays
    const int c_iAlignMultiple = 32; // Used to compute mask: must be pure pwr of 2 like 16 or 32
    I32     rgiFIROut[4096+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment
    I16     rgiMMXCoefs[32+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment

    I32    *piFIROutAligned = (I32 *)(((U32)rgiFIROut + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    I16    *piMMXCoefs = (I16 *)(((U32)rgiMMXCoefs + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    const I32 iSizeOfMMXCoefs = (U32)rgiMMXCoefs + sizeof(rgiMMXCoefs) -
        (U32)piMMXCoefs;

#ifdef MMXVERIFY_ENABLE
#define MMXVERIFY(a)    (a)
#else   // MMXVERIFY
#define MMXVERIFY(a)
#endif  // MMXVERIFY

    // LOW Filter
    FourTapNUMERATOR_MMX_STEREO(rgiCoefNumLow, piSource, piTransientSrcLow,
        piTransientSrcLowCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumLow, piSource,
        piTransientSrcLow + 6, (I32 *)(piTransientSrcLow + 4),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenLow, 0, fltInvScaleNumLow, pfltTransientDstLow,
        piTransientDstLowCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenLow, 0, fltInvScaleNumLow,
        pfltTransientDstLow + 9, (I32 *)(pfltTransientDstLow + 8),
        iSliceIncr, pfltPower,
        iSourceLength, piFIROutAligned));


    // MED Filter
    FiveTapNUMERATOR_MMX_STEREO(rgiCoefNumMed, piSource, piTransientSrcMed,
        piTransientSrcMedCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FiveTapNUMERATOR_MMX_VERIFY(rgiCoefNumMed, piSource,
        piTransientSrcMed + 10, (I32 *)(piTransientSrcMed + 8),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FiveTapDENOMINATOR_FLOP(rgfltCoefDenMed, 1, fltInvScaleNumMed, pfltTransientDstMed,
        piTransientDstMedCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FiveTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenMed, 1, fltInvScaleNumMed,
        pfltTransientDstMed + 15, (I32 *)(pfltTransientDstMed + 14),
        iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));


    // HIGH Filter
    FourTapNUMERATOR_MMX_STEREO(rgiCoefNumHi, piSource, piTransientSrcHi,
        piTransientSrcHiCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumHi, piSource,
        piTransientSrcHi + 6, (I32 *)(piTransientSrcHi + 4),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenHi, 2, fltInvScaleNumHi, pfltTransientDstHi,
        piTransientDstHiCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenHi, 2, fltInvScaleNumHi,
        pfltTransientDstHi + 9, (I32 *)(pfltTransientDstHi + 8),
        iSliceIncr, pfltPower, iSourceLength, piFIROutAligned));
} // FourFiveFourSTEREO



#ifdef MMXMONO
Void INLINE FourFiveFourMONO(const Int cCoefLow, const Int cCoefMed, const Int cCoefHi,
                             FLOATORU32* pfltPower, const I16 *piSource,
                             const Int iSourceLength, const Int iSliceWidth,
                             const Int iSliceIncr,
                             const I16 cChannel, I16 *piTransientSrcLow,
                             Int *piTransientSrcLowCurr, FLOATORI32 *pfltTransientDstLow,
                             Int *piTransientDstLowCurr,
                             I16 *piTransientSrcMed,Int *piTransientSrcMedCurr,
                             FLOATORI32 *pfltTransientDstMed, Int *piTransientDstMedCurr,
                             I16 *piTransientSrcHi, Int *piTransientSrcHiCurr,
                             FLOATORI32 *pfltTransientDstHi, Int *piTransientDstHiCurr,
                             const I16 *rgiCoefNumLow, const Float fltInvScaleNumLow,
                             const Float *rgfltCoefDenLow,
                             const I16 *rgiCoefNumMed, const Float fltInvScaleNumMed,
                             const Float *rgfltCoefDenMed,
                             const I16 *rgiCoefNumHi, const Float fltInvScaleNumHi,
                             const Float *rgfltCoefDenHi)
{
    // Get us some local 32-byte aligned arrays
    const int c_iAlignMultiple = 32; // Used to compute mask: must be pure pwr of 2 like 16 or 32
    I32     rgiFIROut[4096+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment
    I16     rgiMMXCoefs[32+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment

    I32    *piFIROutAligned = (I32 *)(((U32)rgiFIROut + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    I16    *piMMXCoefs = (I16 *)(((U32)rgiMMXCoefs + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    const I32 iSizeOfMMXCoefs = (U32)rgiMMXCoefs + sizeof(rgiMMXCoefs) -
        (U32)piMMXCoefs;

#ifdef MMXVERIFY_ENABLE
#define MMXVERIFY(a)    (a)
    static I16 rgiSrcLowVERIFY[8] = {0};
    static Float rgfltDstLowVERIFY[8] = {0};
    static I32 iSrcLowCurrVERIFY = 0;
    static I32 iDstLowCurrVERIFY = 0;

    static I16 rgiSrcMedVERIFY[10] = {0};
    static Float rgfltDstMedVERIFY[10] = {0};
    static I32 iSrcMedCurrVERIFY = 0;
    static I32 iDstMedCurrVERIFY = 0;

    static I16 rgiSrcHiVERIFY[8] = {0};
    static Float rgfltDstHiVERIFY[8] = {0};
    static I32 iSrcHiCurrVERIFY = 0;
    static I32 iDstHiCurrVERIFY = 0;
#else   // MMXVERIFY
#define MMXVERIFY(a)
#endif  // MMXVERIFY

    // LOW Filter
    FourTapNUMERATOR_MMX_MONO(rgiCoefNumLow, piSource, piTransientSrcLow,
        piTransientSrcLowCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumLow, piSource,
        rgiSrcLowVERIFY, &iSrcLowCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenLow, 0, fltInvScaleNumLow, pfltTransientDstLow,
        piTransientDstLowCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenLow, 0, fltInvScaleNumLow,
        rgfltDstLowVERIFY, &iDstLowCurrVERIFY, iSliceIncr, pfltPower,
        iSourceLength, piFIROutAligned));


    // MED Filter
    FiveTapNUMERATOR_MMX_MONO(rgiCoefNumMed, piSource, piTransientSrcMed,
        piTransientSrcMedCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FiveTapNUMERATOR_MMX_VERIFY(rgiCoefNumMed, piSource,
        rgiSrcMedVERIFY, &iSrcMedCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FiveTapDENOMINATOR_FLOP(rgfltCoefDenMed, 1, fltInvScaleNumMed, pfltTransientDstMed,
        piTransientDstMedCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FiveTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenMed, 1, fltInvScaleNumMed,
        rgfltDstMedVERIFY, &iDstMedCurrVERIFY, iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));


    // HIGH Filter
    FourTapNUMERATOR_MMX_MONO(rgiCoefNumHi, piSource, piTransientSrcHi,
        piTransientSrcHiCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumHi, piSource,
        rgiSrcHiVERIFY, &iSrcHiCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenHi, 2, fltInvScaleNumHi, pfltTransientDstHi,
        piTransientDstHiCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenHi, 2, fltInvScaleNumHi,
        rgfltDstHiVERIFY, &iDstHiCurrVERIFY, iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));
} // FourFiveFourMONO
#endif  // MMXMONO



//***************************************************************************
// MONO ENCODE FILTER WRAPPERS
// These functions basically provide coefficients, and not much else.
// As noted above, MMX mono is no faster than C code, so these are likely to
// die sometime soon.
//***************************************************************************

#ifdef MMXMONO
Void g_GetLowMedHiPower_48MONO_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 4.1942460e-003F, -4.1410504e-003F,
        -4.1410504e-003F, 4.1942460e-003F, 3931353.900655440967641966055168);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F,-2.9414462e+000F,
        2.8870889e+000F,-9.4553636e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 1.2855937e-001F, -7.3982581e-002F,
        -1.0910086e-001F, -7.3982581e-002F, 1.2855937e-001F,
        127460.01996558293471170036792558);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F,-2.8771186e+000F,
        3.2317212e+000F,-1.7996087e+000F,4.4667200e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 4.1398413e-001F, -1.1541642e+000F,
        1.1541642e+000F, -4.1398413e-001F, 20896.620165554909424925763335516);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F,-1.2260380e+000F,
        8.4042799e-001F,-6.9830589e-002F};
    
    FourFiveFourMONO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);    
} // g_GetLowMedHiPower_48MONO_MMX

Void g_GetLowMedHiPower_44MONO_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 4.5562337e-003F, -4.4878074e-003F, -4.4878074e-003F,
        4.5562337e-003F, 3623269.6924894840137334842660726);
    const Float rgfltCoefDenLow [] = {1.0F, -2.936026F, 2.877028F, -0.94086515F};

    // Really a seven-tap but make 8-tap for MMX (#taps must be multiple of 4)
    EIGHTTAP_FILTER_COEFS(Med, Num, 7.2210559e-002F, -1.5162153e-001F, 8.6766934e-002F,
        -1.5332479e-016F, -8.6766934e-002F, 1.5162153e-001F, -7.2210559e-002F, 0.0F,
        105499.36592923926502391456937428);
    const Float rgfltCoefDenMed [] = {1.0F, -4.5286285F, 8.9140898F, -9.9460928F,
        6.6909911F, -2.5544758F, 4.2425609e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 3.8254407e-001F, -1.0501596F, 1.0501596F,
        -3.8254407e-001F, 22872.140765340019501728920781339);
    const Float rgfltCoefDenHi  [] = {1.0F,-1.0657992F,7.6597665e-001F,-3.3631573e-002F};

    // Get us some local 32-byte aligned arrays
    const int c_iAlignMultiple = 32; // Used to compute mask: must be pure pwr of 2 like 16 or 32
    I32     rgiFIROut[4096+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment
    I16     rgiMMXCoefs[44+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment

    I32    *piFIROutAligned = (I32 *)(((U32)rgiFIROut + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    I16    *piMMXCoefs = (I16 *)(((U32)rgiMMXCoefs + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    const I32 iSizeOfMMXCoefs = (U32)rgiMMXCoefs + sizeof(rgiMMXCoefs) -
        (U32)piMMXCoefs;

#ifdef MMXVERIFY_ENABLE
#define MMXVERIFY(a)    (a)
    static I16 rgiSrcLowVERIFY[8] = {0};
    static Float rgfltDstLowVERIFY[8] = {0};
    static I32 iSrcLowCurrVERIFY = 0;
    static I32 iDstLowCurrVERIFY = 0;

    static I16 rgiSrcMedVERIFY[16] = {0};
    static Float rgfltDstMedVERIFY[16] = {0};
    static I32 iSrcMedCurrVERIFY = 0;
    static I32 iDstMedCurrVERIFY = 0;

    static I16 rgiSrcHiVERIFY[8] = {0};
    static Float rgfltDstHiVERIFY[8] = {0};
    static I32 iSrcHiCurrVERIFY = 0;
    static I32 iDstHiCurrVERIFY = 0;

    static s_iCallNum = 0;
    s_iCallNum += 1;
#else   // MMXVERIFY
#define MMXVERIFY(a)
#endif  // MMXVERIFY

    // LOW Filter
    FourTapNUMERATOR_MMX_MONO(rgiCoefNumLow, piSource, piTransientSrcLow,
        piTransientSrcLowCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumLow, piSource,
        rgiSrcLowVERIFY, &iSrcLowCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenLow, 0, fltInvScaleNumLow, pfltTransientDstLow,
        piTransientDstLowCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenLow, 0, fltInvScaleNumLow,
        rgfltDstLowVERIFY, &iDstLowCurrVERIFY, iSliceIncr, pfltPower,
        iSourceLength, piFIROutAligned));


    // MED Filter
    EightTapNUMERATOR_MMX_MONO(rgiCoefNumMed, piSource, piTransientSrcMed,
        piTransientSrcMedCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(EightTapNUMERATOR_MMX_VERIFY(rgiCoefNumMed, piSource,
        rgiSrcMedVERIFY, &iSrcMedCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    SevenTapDENOMINATOR_FLOP(rgfltCoefDenMed, 1, fltInvScaleNumMed, pfltTransientDstMed,
        piTransientDstMedCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(SevenTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenMed, 1, fltInvScaleNumMed,
        rgfltDstMedVERIFY, &iDstMedCurrVERIFY, iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));


    // HIGH Filter
    FourTapNUMERATOR_MMX_MONO(rgiCoefNumHi, piSource, piTransientSrcHi,
        piTransientSrcHiCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumHi, piSource,
        rgiSrcHiVERIFY, &iSrcHiCurrVERIFY, cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenHi, 2, fltInvScaleNumHi, pfltTransientDstHi,
        piTransientDstHiCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenHi, 2, fltInvScaleNumHi,
        rgfltDstHiVERIFY, &iDstHiCurrVERIFY, iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));
} // g_GetLowMedHiPower_44MONO_MMX


Void g_GetLowMedHiPower_32MONO_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 6.2256543e-003F, -6.0485736e-003F,
        -6.0485736e-003F, 6.2256543e-003F, 2669740.2298729726810758170905857);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.9102951e+000F,
        2.8300775e+000F, -9.1942820e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 2.2369834e-001F, -5.1857903e-002F,
        -3.4345617e-001F, -5.1857903e-002F, 2.2369834e-001F,
        73262.12425502029826987051544664);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F, -2.2512264e+000F,
        1.9390413e+000F, -1.0269138e+000F, 3.4620074e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 2.6059998e-001F, -6.4498064e-001F,
        6.4498064e-001F, -2.6059998e-001F, 36185.624218922943313427573293876);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, -3.1030711e-001F,
        6.0270790e-001F, 1.0185376e-001F};

    FourFiveFourMONO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_32MONO_MMX


Void g_GetLowMedHiPower_22MONO_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 8.9270704e-003F, -8.3957364e-003F,
        -8.3957364e-003F, 8.9270704e-003F, 1891668.0416512672033032543056801);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.8661956e+000F,
        2.7524893e+000F, -8.8523103e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 3.7825275e-001F, -2.7009893e-002F,
        -7.0166945e-001F, -2.7009893e-002F, 3.7825275e-001F,
        43339.656242870633479044984783711);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F, -1.4097054e+000F,
        3.9462869e-001F, -2.9769521e-001F, 3.3857173e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 1.2646487e-001F, -2.1157565e-001F,
        2.1157565e-001F, -1.2646487e-001F, 96938.083089149356231018092590096);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, 9.2724155e-001F,
        8.8804049e-001F, 2.8471791e-001F};

    FourFiveFourMONO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_22MONO_MMX


Void g_GetLowMedHiPower_16MONO_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 1.2164886e-002F, -1.0804778e-002F,
        -1.0804778e-002F, 1.2164886e-002F, 1426620.7818825410218450995255553);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.8096674e+000F,
        2.6577447e+000F, -8.4535714e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 6.0241859e-001F, -5.6092363e-003F,
        -1.1912111e+000F, -5.6092363e-003F, 6.0241859e-001F,
        27225.067583280221753351021750463);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F,-4.3220212e-001F,-1.0839609e+000F,7.7620433e-002F,5.1464140e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 3.9054597e-002F, 3.3076983e-003F,
        -3.3076983e-003F, -3.9054597e-002F, 773541.65498344124752840283771319);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, 2.2733598e+000F,
        1.9348375e+000F, 5.8998385e-001F};

    FourFiveFourMONO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_16MONO_MMX

#endif  // MMXMONO



//***************************************************************************
// STEREO ENCODE FILTER WRAPPERS
// These functions basically provide coefficients, and not much else.
//***************************************************************************

Void g_GetLowMedHiPower_48_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 4.1942460e-003F, -4.1410504e-003F,
        -4.1410504e-003F, 4.1942460e-003F, 3931353.900655440967641966055168);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F,-2.9414462e+000F,
        2.8870889e+000F,-9.4553636e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 1.2855937e-001F, -7.3982581e-002F,
        -1.0910086e-001F, -7.3982581e-002F, 1.2855937e-001F,
        127460.01996558293471170036792558);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F,-2.8771186e+000F,
        3.2317212e+000F,-1.7996087e+000F,4.4667200e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 4.1398413e-001F, -1.1541642e+000F,
        1.1541642e+000F, -4.1398413e-001F, 20896.620165554909424925763335516);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F,-1.2260380e+000F,
        8.4042799e-001F,-6.9830589e-002F};

    FourFiveFourSTEREO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_48_MMX


Void g_GetLowMedHiPower_44_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 4.5562337e-003F, -4.4878074e-003F, -4.4878074e-003F,
        4.5562337e-003F, 3623269.6924894840137334842660726);
    const Float rgfltCoefDenLow [] = {1.0F, -2.936026F, 2.877028F, -0.94086515F};

    // Really a seven-tap but make 8-tap for MMX (#taps must be multiple of 4)
    EIGHTTAP_FILTER_COEFS(Med, Num, 7.2210559e-002F, -1.5162153e-001F, 8.6766934e-002F,
        -1.5332479e-016F, -8.6766934e-002F, 1.5162153e-001F, -7.2210559e-002F, 0.0F,
        105499.36592923926502391456937428);
    const Float rgfltCoefDenMed [] = {1.0F, -4.5286285F, 8.9140898F, -9.9460928F,
        6.6909911F, -2.5544758F, 4.2425609e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 3.8254407e-001F, -1.0501596F, 1.0501596F,
        -3.8254407e-001F, 22872.140765340019501728920781339);
    const Float rgfltCoefDenHi  [] = {1.0F,-1.0657992F,7.6597665e-001F,-3.3631573e-002F};

    // Get us some local 32-byte aligned arrays
    const int c_iAlignMultiple = 32; // Used to compute mask: must be pure pwr of 2 like 16 or 32
    I32     rgiFIROut[4096+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment
    I16     rgiMMXCoefs[44+16]; // +16 == +(32/sizeof(I16)), for 32-byte alignment

    I32    *piFIROutAligned = (I32 *)(((U32)rgiFIROut + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    I16    *piMMXCoefs = (I16 *)(((U32)rgiMMXCoefs + c_iAlignMultiple - 1) &
        ~(c_iAlignMultiple - 1));
    const I32 iSizeOfMMXCoefs = (U32)rgiMMXCoefs + sizeof(rgiMMXCoefs) -
        (U32)piMMXCoefs;

#ifdef MMXVERIFY_ENABLE
#define MMXVERIFY(a)    (a)
#else   // MMXVERIFY
#define MMXVERIFY(a)
#endif  // MMXVERIFY

    // LOW Filter
    FourTapNUMERATOR_MMX_STEREO(rgiCoefNumLow, piSource, piTransientSrcLow,
        piTransientSrcLowCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumLow, piSource,
        piTransientSrcLow + 6, (I32 *)(piTransientSrcLow + 4),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenLow, 0, fltInvScaleNumLow, pfltTransientDstLow,
        piTransientDstLowCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenLow, 0, fltInvScaleNumLow,
        pfltTransientDstLow + 9, (I32 *)(pfltTransientDstLow + 8),
        iSliceIncr, pfltPower,
        iSourceLength, piFIROutAligned));


    // MED Filter
    EightTapNUMERATOR_MMX_STEREO(rgiCoefNumMed, piSource, piTransientSrcMed,
        piTransientSrcMedCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(EightTapNUMERATOR_MMX_VERIFY(rgiCoefNumMed, piSource,
        piTransientSrcMed + 10, (I32 *)(piTransientSrcMed + 8),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    SevenTapDENOMINATOR_FLOP(rgfltCoefDenMed, 1, fltInvScaleNumMed, pfltTransientDstMed,
        piTransientDstMedCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(SevenTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenMed, 1, fltInvScaleNumMed,
        pfltTransientDstMed + 15, (I32 *)(pfltTransientDstMed + 14),
        iSliceIncr, pfltPower, iSourceLength,
        piFIROutAligned));


    // HIGH Filter
    FourTapNUMERATOR_MMX_STEREO(rgiCoefNumHi, piSource, piTransientSrcHi,
        piTransientSrcHiCurr, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs);
    MMXVERIFY(FourTapNUMERATOR_MMX_VERIFY(rgiCoefNumHi, piSource,
        piTransientSrcHi + 6, (I32 *)(piTransientSrcHi + 4),
        cChannel, iSourceLength, piFIROutAligned,
        c_iAlignMultiple, piMMXCoefs, iSizeOfMMXCoefs));
    FourTapDENOMINATOR_FLOP(rgfltCoefDenHi, 2, fltInvScaleNumHi, pfltTransientDstHi,
        piTransientDstHiCurr, iSliceIncr, pfltPower, iSourceLength, piFIROutAligned);
    MMXVERIFY(FourTapDENOMINATOR_FLOP_VERIFY(rgfltCoefDenHi, 2, fltInvScaleNumHi,
        pfltTransientDstHi + 9, (I32 *)(pfltTransientDstHi + 8),
        iSliceIncr, pfltPower, iSourceLength, piFIROutAligned));
}



Void g_GetLowMedHiPower_32_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 6.2256543e-003F, -6.0485736e-003F,
        -6.0485736e-003F, 6.2256543e-003F, 2669740.2298729726810758170905857);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.9102951e+000F,
        2.8300775e+000F, -9.1942820e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 2.2369834e-001F, -5.1857903e-002F,
        -3.4345617e-001F, -5.1857903e-002F, 2.2369834e-001F,
        73262.12425502029826987051544664);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F, -2.2512264e+000F,
        1.9390413e+000F, -1.0269138e+000F, 3.4620074e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 2.6059998e-001F, -6.4498064e-001F,
        6.4498064e-001F, -2.6059998e-001F, 36185.624218922943313427573293876);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, -3.1030711e-001F,
        6.0270790e-001F, 1.0185376e-001F};

    FourFiveFourSTEREO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_32_MMX



Void g_GetLowMedHiPower_22_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 8.9270704e-003F, -8.3957364e-003F,
        -8.3957364e-003F, 8.9270704e-003F, 1891668.0416512672033032543056801);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.8661956e+000F,
        2.7524893e+000F, -8.8523103e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 3.7825275e-001F, -2.7009893e-002F,
        -7.0166945e-001F, -2.7009893e-002F, 3.7825275e-001F,
        43339.656242870633479044984783711);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F, -1.4097054e+000F,
        3.9462869e-001F, -2.9769521e-001F, 3.3857173e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 1.2646487e-001F, -2.1157565e-001F,
        2.1157565e-001F, -1.2646487e-001F, 96938.083089149356231018092590096);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, 9.2724155e-001F,
        8.8804049e-001F, 2.8471791e-001F};

    FourFiveFourSTEREO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_22_MMX


Void g_GetLowMedHiPower_16_MMX(GET_LOW_MED_HI_ARGS)
{
    FOURTAP_FILTER_COEFS(Low, Num, 1.2164886e-002F, -1.0804778e-002F,
        -1.0804778e-002F, 1.2164886e-002F, 1426620.7818825410218450995255553);
    const Float rgfltCoefDenLow [] = {1.0000000e+000F, -2.8096674e+000F,
        2.6577447e+000F, -8.4535714e-001F};

    FIVETAP_FILTER_COEFS(Med, Num, 6.0241859e-001F, -5.6092363e-003F,
        -1.1912111e+000F, -5.6092363e-003F, 6.0241859e-001F,
        27225.067583280221753351021750463);
    const Float rgfltCoefDenMed [] = {1.0000000e+000F,-4.3220212e-001F,-1.0839609e+000F,7.7620433e-002F,5.1464140e-001F};

    FOURTAP_FILTER_COEFS(Hi, Num, 3.9054597e-002F, 3.3076983e-003F,
        -3.3076983e-003F, -3.9054597e-002F, 773541.65498344124752840283771319);
    const Float rgfltCoefDenHi  [] = {1.0000000e+000F, 2.2733598e+000F,
        1.9348375e+000F, 5.8998385e-001F};

    FourFiveFourSTEREO(cCoefLow, cCoefMed, cCoefHi, pfltPower, piSource,
        iSourceLength, iSliceWidth, iSliceIncr, cChannel, piTransientSrcLow,
        piTransientSrcLowCurr, pfltTransientDstLow, piTransientDstLowCurr,
        piTransientSrcMed, piTransientSrcMedCurr, pfltTransientDstMed,
        piTransientDstMedCurr, piTransientSrcHi, piTransientSrcHiCurr,
        pfltTransientDstHi, piTransientDstHiCurr,
        rgiCoefNumLow, fltInvScaleNumLow, rgfltCoefDenLow,
        rgiCoefNumMed, fltInvScaleNumMed, rgfltCoefDenMed,
        rgiCoefNumHi, fltInvScaleNumHi, rgfltCoefDenHi);
} // g_GetLowMedHiPower_16_MMX




//***************************************************************************
//***************************************************************************
#if defined (PEAQ_MASK)
Void prvComputeNoisePattern_SIMD(const CAudioObjectEncoder* pauenc,
                                 const Float* rgfltCoefOrig,
                                 const Float* rgfltCoefRecon,
                                 const Int iLenCoef,
                                 Float* rgfltNoisePattern,
                                 const Bool* rgfNoisyBark)
{
    CAudioObject* pau = pauenc->pau; 
    Int cFrameSampleHalf = pau->m_cFrameSampleHalf;
    Int cSamplingRate    = pau->m_iSamplingRate;
    Int cValidBarkBand   = pau->m_rgcValidBarkBand[0];   // For max subframe size

    const Int*   rgiBarkIndex       = pau->m_rgiBarkIndexOrig; // For max subframe size
    const Float* rgfltOuterEarXform = pauenc->m_rgfltOuterEarXform;

    Int     iBand=0;
    Float   fltScale = 1.0F / iLenCoef / cFrameSampleHalf / 0.625F / 0.625F; //wchen: this assumes !NO_LEVELING
    Int     iDownFactor = LOG2 (cFrameSampleHalf) - LOG2 (iLenCoef);
    
    const Float c_fltPowStep  = 1.0592537251772888788092803732781F;
    Float fltPower    = 1.1885022274370184377301224648922F;
    Float fltPowConst = (Float) pow(10.0, 0.3 - (cValidBarkBand-1)*0.025);

    const Bool* pfNoisyBark = rgfNoisyBark;

    Bool    fPrint = 0; //pau->m_iFrameNumber == 2;

    assert (iDownFactor >= 0); 
    memset(rgfltNoisePattern, 0, NUM_BARK_BAND * sizeof(rgfltNoisePattern[0]));

    if (iDownFactor > 0)
    {
        const I16 c_iDownFactorDivisor = (1 << iDownFactor);
        const I16 c_iDFBlockMASK = ~(c_iDownFactorDivisor - 1);

        Int iCoefFull = 0;      // Full-frame coef idx for output
        Int iCoefIn = 0;        // Small subframe coef idx for input
        Int iGoldenBark = 0;    // Current golden bark band we're working on

        // pfltXform is must be incremented specially and so is not indexed
        // with the others.
        const Float* pfltXform = pauenc->m_rgpfltXfromComboHead [iDownFactor - 1];
        Float  fltError;

        I16 iNextBand, iNextBandR;

        iNextBand = (I16) rgiBarkIndex[iGoldenBark + 1];
        iNextBandR = (iNextBand & c_iDFBlockMASK);
        fltError = rgfltCoefOrig[iCoefIn] - rgfltCoefRecon[iCoefIn];
        fltError *= fltError;
        
        while (iCoefFull < cFrameSampleHalf)
        {
            if (rgfNoisyBark[iGoldenBark])
            {
                // Advance iCoefIn but be careful: advancement of iCoefIn must
                // match non-noisy loop behaviour below

                // Pre-loop fractional case: iWidth = 1 to c_iDownFactorDivisor for 0th
                // iteration. Don't worry about it, it's all covered in next calculation.
                
                // Simulate loop. Don't increment iCoefIn for the post-fractional part
                // (remainder of downsample factor). We need to consider when iCoefFull
                // is not a multiple of c_iDownFactorDivisor, so we ROUND UP.
                assert(iCoefFull <= iNextBandR);
                iCoefIn += ((iNextBandR - iCoefFull + c_iDownFactorDivisor - 1) >> iDownFactor);
            }
            else
            {
                Int iWidth;

	            iWidth = (c_iDownFactorDivisor - iCoefFull + (iCoefFull & c_iDFBlockMASK));
	            while (iCoefFull < iNextBandR)
	            {
                    fltError = rgfltCoefOrig[iCoefIn] - rgfltCoefRecon[iCoefIn];
                    fltError *= fltError;
                    rgfltNoisePattern[iGoldenBark] += fltError * *pfltXform;
                    pfltXform++;
	                iCoefFull += iWidth;
                    iWidth = c_iDownFactorDivisor;
                    iCoefIn += 1;
	            }
                //take care of the fractional part
                rgfltNoisePattern[iGoldenBark] += fltError * *pfltXform;
                rgfltNoisePattern[iGoldenBark] *= fltScale;
                pfltXform++;
            }

            // The following pointers are advanced the same way for everyone for each
            // iteration of the loop.  iCoefIn also needs to be incremented, but in a
            // specifc way and is therefore done above.
            iCoefFull = iNextBand;
            iNextBand = (I16) rgiBarkIndex[iGoldenBark + 2]; // Avoid iGoldenBark dependency
            iGoldenBark += 1;
            iNextBandR = (iNextBand & c_iDFBlockMASK);
        }
    }
    else
    {
        Int     iCoef=0;
        Float *pfltNoisePattern = rgfltNoisePattern;

        assert(0 == iDownFactor); // We assume (iDownFactor < 0) is not possible
        iBand = cValidBarkBand; // Count down to zero


#define rTemp               eax
#define rpfltNoisePattern   eax
#define riRemainingCoefs    eax
#define rpiBarkIndex        ebx
#define riCountZero         ecx
#define rpfltOuterEar       edx
#define rpfltCoefOrig       esi
#define rpfltCoefRecon      edi

#define SIZEOF_I32      4
#define SIZEOF_FLOAT    4

        _asm
        {

            // Initialize outer loop registers and vars
            mov     rpiBarkIndex, rgiBarkIndex
            add     rpiBarkIndex, SIZEOF_I32
            
            // Initialize inner loop registers and vars
            mov     rpfltOuterEar, rgfltOuterEarXform
            mov     rpfltCoefOrig, rgfltCoefOrig
            mov     rpfltCoefRecon, rgfltCoefRecon
            mov     rTemp, dword ptr [fltScale]
            push    rTemp

nextBark:
            dec     dword ptr [iBand]                   // Check if it's time to exit outer loop
            jl      exitLoop
            mov     rTemp, dword ptr [rpiBarkIndex]     // Find next bark band boundary
            add     rpiBarkIndex, SIZEOF_I32
            mov     riCountZero, rTemp
            sub     riCountZero, dword ptr [iCoef]      // Calculate width of bark band
            mov     dword ptr [iCoef], rTemp            // Save new iCoef value

            // Is this band noisy? If so, go to next band
            mov     rTemp, dword ptr [pfNoisyBark]
            add     rTemp, SIZEOF_I32
            mov     dword ptr [pfNoisyBark], rTemp
            mov     rTemp, dword ptr [rTemp - SIZEOF_I32]
            test    rTemp, rTemp
            je      computeNoisePower

            // Advance inner loop pointers
            imul    riCountZero, SIZEOF_FLOAT
            add     rpfltCoefOrig, riCountZero
            add     rpfltCoefRecon, riCountZero
            add     rpfltOuterEar, riCountZero

            jmp     nextBark


computeNoisePower:
            // This band is not noisy. Compute noise pattern.
            xorps   xmm7_xmm7                           // Zero the accumulator
            sar     riCountZero, 2                      // Each SIMD register holds 4 coefs

            // Compute error power with outer ear transform
            // Calculate number of iterations we need from the Software Pipelined
            // loop, then calculate number of iterations needed to finish
            mov     riRemainingCoefs, riCountZero
            sub     riCountZero, 3                      // 2 SIMD coefs finish in epilog
            sar     riCountZero, 2                      // Divide by 4

            jle     computeRemainder

            // If we reach this point, we're going to do software pipelined loop
            // Compute how many iterations will remain when we exit
            neg     riRemainingCoefs    // LEA only does positive displacements
            lea     riRemainingCoefs, [riRemainingCoefs + 4*riCountZero + 3]
            neg     riRemainingCoefs

            // Software Pipelined Loop: as described on p. 5-25 of Intel's
            // Optimization Reference Manual, this loop achieves high throughput
            // by overlapping the execution of several iterations.

            // **************************************************************
            // PROLOG (Software Pipelined Loop)
            // **************************************************************

            // Some clocks are skipped because there's no instructions there. This
            // is because the decoder can sometimes decode faster than the execution
            // units can execute.

            // Clock 0
            movaps  xmm0_esiBASE                    // Load rgfltCoefOrig = O1
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 2
            movaps  xmm1_ediBASE                    // Load rgfltCoefRecon = R1
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 4
            movaps  xmm2_edxBASE                    // Load rgfltOuterEarXform = E1
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 5
            subps   xmm0_xmm1                       // D1 = O1 - R1

            // Clock 6
            movaps  xmm3_esiBASE                    // Load rgfltCoefOrig = O2
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 8
            movaps  xmm4_ediBASE                    // Load rgfltCoefRecon = R2
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 9
            mulps   xmm0_xmm2                       // T1 = D1 * E1

            // Clock 10
            movaps  xmm5_edxBASE                    // Load rgfltOuterEarXform = E2
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 11
            subps   xmm3_xmm4                       // D2 = O2 - R2
            
            // Clock 12
            movaps  xmm6_esiBASE                    // Load rgfltCoefOrig = O3
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // **************************************************************
            // STEADY STATE (Software Pipelined Loop)
            // **************************************************************

softPipeLOOP:
            // Clock 14
            movaps  xmm1_ediBASE                    // Load rgfltCoefRecon = R3
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 15
            mulps   xmm0_xmm0                       // A1 = T1^2

            // Clock 16
            movaps  xmm2_edxBASE                    // Load rgfltOuterEarXform = E3
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 17
            mulps   xmm3_xmm5                       // T2 = D2 * E2

            // Clock 18
            subps   xmm6_xmm1                       // D3 = O3 - R3

            // Clock 19
            movaps  xmm4_esiBASE                    // Load rgfltCoefOrig = O4
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 20
            addps   xmm7_xmm0                       // Accumulate noise power, A += A1

            // Clock 21
            movaps  xmm5_ediBASE                    // Load rgfltCoefRecon = R4
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 22
            mulps   xmm3_xmm3                       // A2 = T2^2

            // Clock 23
            movaps  xmm1_edxBASE                    // Load rgfltOuterEarXform = E4
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 24
            mulps   xmm6_xmm2                       // T3 = D3 * E3

            // Clock 25
            subps   xmm4_xmm5                       // D4 = O4 - R4

            // Clock 26
            movaps  xmm0_esiBASE                    // Load rgfltCoefOrig = O5
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 27
            addps   xmm7_xmm3                       // Accumulate noise power, A += A2

            // Clock 28
            movaps  xmm5_ediBASE                    // Load rgfltCoefRecon = R5
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 29
            mulps   xmm6_xmm6                       // A3 = T3^2

            // Clock 30
            movaps  xmm2_edxBASE                    // Load rgfltOuterEarXform = E5
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 31
            mulps   xmm4_xmm1                       // T4 = D4 * E4

            // Clock 32
            subps   xmm0_xmm5                       // D5 = O5 - R5

            // Clock 33
            movaps  xmm3_esiBASE                    // Load rgfltCoefOrig = O6
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 34
            addps   xmm7_xmm6                       // Accumulate noise power, A += A3

            // Clock 35
            movaps  xmm1_ediBASE                    // Load rgfltCoefRecon = R6
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 36
            mulps   xmm4_xmm4                       // A4 = T4^2

            // Clock 37
            movaps  xmm5_edxBASE                    // Load rgfltOuterEarXform = E6
            add     rpfltOuterEar, 4*SIZEOF_FLOAT

            // Clock 38
            mulps   xmm0_xmm2                       // T5 = D5 * E5

            // Clock 39
            subps   xmm3_xmm1                       // D6 = O6 - R6

            // Clock 40
            movaps  xmm6_esiBASE                    // Load rgfltCoefOrig = O7
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs
            dec     riCountZero                     // Decrement loop counter

            // Clock 41
            addps   xmm7_xmm4                       // Accumulate noise power, A += A4

            // End of steady-state: keep looping until done
            jg      softPipeLOOP

            // **************************************************************
            // EPILOG (Software Pipelined Loop)
            // **************************************************************

            // Clock 42
            movaps  xmm2_ediBASE                    // Load rgfltCoefRecon = R7
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs

            // Clock 43
            mulps   xmm0_xmm0                       // A5 = T5^2

            // Clock 44
            movaps  xmm1_edxBASE                    // Load rgfltOuterEarXform = E7
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            // Clock 45
            mulps   xmm3_xmm5                       // T6 = D6 * E6

            // Clock 46
            subps   xmm6_xmm2                       // D7 = O7 - R7

            // Clock 48
            addps   xmm7_xmm0                       // Accumulate noise power, A += A5

            // Clock 50
            mulps   xmm3_xmm3                       // A6 = T6^2

            // Clock 52
            mulps   xmm6_xmm1                       // T7 = D7 * E7

            // Clock 55
            addps   xmm7_xmm3                       // Accumulate noise power, A += A6

            // Clock 57
            mulps   xmm6_xmm6                       // A7 = T7^2

            // Clock 62
            addps   xmm7_xmm6                       // Accumulate noise power, A += A7


computeRemainder:
            // Finish the job, one SIMD register at a time
            mov     riCountZero, riRemainingCoefs
            test    riCountZero, riCountZero
            jle     horizontalSum                   // Sometimes the remainder is zero

remainderLOOP:
            movaps  xmm0_esiBASE                    // Load rgfltCoefOrig
            add     rpfltCoefOrig, 4*SIZEOF_FLOAT   // Advance ptrs
            movaps  xmm1_ediBASE                    // Load rgfltCoefRecon
            add     rpfltCoefRecon, 4*SIZEOF_FLOAT  // Advance ptrs
            movaps  xmm2_edxBASE                    // Load rgfltOuterEar
            add     rpfltOuterEar, 4*SIZEOF_FLOAT   // Advance ptrs

            subps   xmm0_xmm1                       // D = O - R
            dec     riCountZero
            mulps   xmm0_xmm2                       // T = D * E
            mulps   xmm0_xmm0                       // A = T^2
            addps   xmm7_xmm0                       // Accumulate noise power, Accum += A

            jg     remainderLOOP

horizontalSum:
            // OK, now do the horizontal summation to get the power sum
            // for this bark band
            movhlps xmm3_xmm7
            mov     rpfltNoisePattern, dword ptr [pfltNoisePattern]
            addps   xmm3_xmm7
            movssFROMMEM xmm4_espBASE               // Load fltScale to xmm4
            movaps  xmm7_xmm3
            shufps(xmm3_xmm3, 0xB1)                 // ABCD -> BADC
            addss   xmm3_xmm7                       // A+B+C+D in lowest cell
            mulss   xmm3_xmm4                       // Multiply by fltScale
            movssTOMEM eaxBASE_xmm3                 // Store result

            add     rpfltNoisePattern, SIZEOF_FLOAT
            mov     pfltNoisePattern, rpfltNoisePattern

            jmp     nextBark

exitLoop:
            pop     rTemp                           // Clean up fltScale
        }
    }

#if !defined NO_BARK_BW
    assert (cValidBarkBand >= 3);
    rgfltNoisePattern[0] *= fltPowConst;
    rgfltNoisePattern[1] *= fltPowConst;
    rgfltNoisePattern[2] *= fltPowConst;
    fltPowConst = (Float) pow(10.0, (1 - cValidBarkBand)*0.025);
    for (iBand = 3; iBand < cValidBarkBand; iBand++) {
        rgfltNoisePattern[iBand] *= fltPower * fltPowConst;
        fltPower *= c_fltPowStep;
    }
#endif

#if 0
    // Absorb the scale based on bark bandwidth (since while computing NMR, we are simply dividing with excitation
    for (iBand = 0; iBand < 3; iBand++) {
#if !defined NO_BARK_BW
        rgfltNoisePattern[iBand] *= (Float) pow(10.0, 0.3)/(Float) pow(10.0, (cValidBarkBand-1)*0.025);
#endif
    }
    for (iBand = 3; iBand < cValidBarkBand; iBand++) {
#if !defined NO_BARK_BW
        rgfltNoisePattern[iBand] *= (Float) pow(10.0, iBand * 0.025)/(Float) pow(10.0, (cValidBarkBand-1)*0.025);
#endif
    }
#endif

} // prvComputeNoisePattern_SIMD

#endif  // PEAQ_MASK

#endif  // ENCODER

#endif // defined(WMA_TARGET_X86)
