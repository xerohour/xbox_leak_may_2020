

#ifndef __X86MACROS_H_
#define __X86MACROS_H_

#ifdef WMA_TARGET_X86

#pragma MSVC_DISABLE_WARNING(4035)

// ======================================================
// Typedefs
// ======================================================
#define FASTCALL    __fastcall


// ======================================================
// Macros
// ======================================================
#define MULT_HI_DWORD_DOWN(a,b) MultiplyHighDwordDown(a,b)
#define MULT_HI_DWORD(a,b)      MultiplyHighDword(a,b)
#define MULT_HI_UDWORD(a,b)     MultiplyHighUDword(a,b)

#pragma warning(disable: 4035)

#ifndef WMA_ANSI_COMPATIBILITY_MODE
// The following two functions, ROUNDF and ROUNDD, use the "round to nearest
// (even)" rounding mode in the Intel processor. This means -0.7 goes to -1,
// 1.3 goes to 1, 11.5 goes to 12 and 12.5 goes to 12. This unfortunately
// is a pain to do in software, so ANSI just rounds up in the case of 0.5
// (-0.7 to -1, 1.3 to 1, 11.5 to 12 and 12.5 to 13). Don't use these rounding
// macros if we want to compare x86 output with ANSI.
//-------------------------------------------------------

#ifdef PLATFORM_SPECIFIC_ROUND
INLINE Int FASTCALL ROUNDF(float f) {
    __int64 intval;
    _asm {
        fld         f
        fistp       dword ptr [intval]
        mov         eax,dword ptr [intval]
    }
}

//-------------------------------------------------------
INLINE Int FASTCALL ROUNDD(double f) {
    _asm {
        fld         f
        fistp       dword ptr [f]
        mov         eax,dword ptr [f]
    }
}
#endif // PLATFORM_SPECIFIC_ROUND
#endif // WMA_ANSI_COMPATIBILITY_MODE


//-------------------------------------------------------
INLINE I32 FASTCALL MultiplyHighDwordDown(I32 reg1, I32 reg2){
__asm{  mov     eax,reg1
        mov     edx,reg2
        imul    edx
        mov     eax,edx
        shl     eax,2
     }
}

//-------------------------------------------------------
INLINE I32 FASTCALL MultiplyHighDword(I32 reg1, I32 reg2){
__asm{  mov     eax,reg1
        mov     edx,reg2
        imul    edx
        mov     eax,edx
     }
}

//-------------------------------------------------------
INLINE I32 FASTCALL MultiplyHighUDword(U32 reg1, U32 reg2){
__asm{  mov     eax,reg1
        mov     edx,reg2
        mul     edx
        mov     eax,edx
     }
}


//-------------------------------------------------------
INLINE U32 FASTCALL ByteSwap(U32 i){
    __asm
    {
        mov     eax, i
        bswap   eax
    }
}

#pragma warning(default: 4035)

#ifdef PLATFORM_SPECIFIC_BITCOPY
//-------------------------------------------------------
//assuming dwsrc is zeroed out
INLINE void FASTCALL bitCpy (const U8* pbSrc, Int iBitStartSrc,
                                 Int cBits, U8* pbDst)
{
    const U32* pdwSrc = (U32 *) pbSrc;
    const U32* pdwSrcEnd;
    U32* pdwDst = (U32 *) pbDst;
    Int iShiftDown;
    U32 dw2, dw1;


    assert (pdwSrc != NULL && pdwDst != NULL);
    assert (iBitStartSrc < BITS_PER_DWORD && iBitStartSrc >= 0);
    assert (cBits >= 0);

    pdwSrcEnd = pdwSrc + (iBitStartSrc > 0) + 
                            (((cBits - iBitStartSrc) + 31) & ~31) / 32; //open
    iShiftDown = (BITS_PER_DWORD - iBitStartSrc);
    dw1 = DEFAULTENDIAN_TO_BIGENDIAN(*pdwSrc);
    while (pdwSrc < pdwSrcEnd) {
        pdwSrc++;
        dw2 = DEFAULTENDIAN_TO_BIGENDIAN(*pdwSrc);
        *pdwDst = BIGENDIAN_TO_DEFAULTENDIAN((dw1 << iBitStartSrc) | (dw2 >> iShiftDown));
        dw1 = dw2;
        pdwDst++;
    }
}
#endif  // PLATFORM_SPECIFIC_BITCOPY

#pragma MSVC_RESTORE_WARNING(4035)
//wchen: mmx detection has 25db mismatch with C version. Off for now.
#undef PLATFORM_SPECIFIC_FNPTR
#define PLATFORM_SPECIFIC_FNPTR                                                     \
    if (g_SupportMMX()){                                                            \
        if (g_SupportCMOV()) {                                                      \
            pauenc->m_pfnGetLowMedHiPowerStereo_48 = g_GetLowMedHiPower_48_MMX;     \
            pauenc->m_pfnGetLowMedHiPowerStereo_44 = g_GetLowMedHiPower_44_MMX;     \
            pauenc->m_pfnGetLowMedHiPowerStereo_32 = g_GetLowMedHiPower_32_MMX;     \
            pauenc->m_pfnGetLowMedHiPowerStereo_22 = g_GetLowMedHiPower_22_MMX;     \
        }                                                                           \
    }
#endif // WMA_TARGET_X86
#endif // __X86MACROS_H_
