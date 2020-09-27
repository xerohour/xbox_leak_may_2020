#include "bldsetup.h"

#include "xplatform.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "math.h"
#include "memory.h"

#ifdef IPP_COLORCONV
#include "..\arm\ippdefs.h"
#include "..\arm\ippIp.h"
#endif //IPP_COLORCONV

#if defined(_ARM_) || defined(__arm)
#   define PAGE_ARM_MINICACHE 0x800
#else
#   define PAGE_ARM_MINICACHE 0
#endif

// XBox surface requires 64-bit scanline alignment
#ifdef _XBOX
#define BITMAP_WIDTH_WMV(width, bitCount) \
    (I32_WMV)((I32_WMV)(((((I32_WMV)width) * ((I32_WMV)bitCount)) + 511L) & (I32_WMV)~511L) / 8L)
#else
#define BITMAP_WIDTH_WMV(width, bitCount) \
    (I32_WMV)((I32_WMV)(((((I32_WMV)width) * ((I32_WMV)bitCount)) + 31L) & (I32_WMV)~31L) / 8L)
#endif


#if defined (UNDER_CE) || defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)
#ifdef __cplusplus
extern "C" {
#endif
I32_WMV g_iVtoR_WMV [256];
I32_WMV g_iVtoG_WMV [256];
I32_WMV g_iUtoG_WMV [256];
I32_WMV g_iUtoB_WMV [256];
I32_WMV g_iYscale_WMV [256];
#ifdef __cplusplus
}
#endif
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
U8_WMV g_rgDitherMap_WMV [4] [4] [3] [256];

#define OFFSET 10          // First ten colours are used by Windows

const I32_WMV g_magic4x4 [4] [4] = {  
        0,  45,   9,  41,
        35,  16,  25,  19,
        38,   6,  48,   3,
        22,  29,  13,  32 
};

Void_WMV g_InitDstDitherMap_WMV ()
{
    I32_WMV x, y, z, t, ndiv51, nmod51;

    // Calculate the RED, GREEN and BLUE table entries

    for (x = 0; x < 4; x++) {
        for (y = 0; y < 4; y++) {
            for (z = 0; z < 256; z++) {
                t = g_magic4x4 [x] [y];
                ndiv51 = (z & 0xF8) / 51; nmod51 = (z & 0xF8) % 51;
                g_rgDitherMap_WMV [y] [x] [0] [z] = (ndiv51 + (nmod51 > t));  // r
                g_rgDitherMap_WMV [y] [x] [2] [z] = 36 * (ndiv51 + (nmod51 > t)) + OFFSET; // b
                ndiv51 = (z & 0xFC) / 51; nmod51 = (z & 0xFC) % 51;
                g_rgDitherMap_WMV [y] [x] [1] [z] = 6 * (ndiv51 + (nmod51 > t)); // g
            }
        }
    }
}
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
 
#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4)
const U32_WMV g_uiDithering444 [24] = {3359615270, 2933630948, 21414038, 1043013166, 2298472485, 1461799693, 1991657213, 28333636,
                                       949904317, 1286920827, 3729350426, 598052109, 1617798366, 3149964184, 2962788697, 1000534078, 
                                       357460706, 2831545023, 2553801342, 866463537, 1486665357, 2042543698, 2379626617, 774808343};
const U8_WMV g_uiClapTab4BitDec [64] = {0, 0, 0, 0, 0, 0, 0, 0, 
                                        0, 0, 0, 0, 0, 0, 0, 0, 
                                        0, 0, 0, 0, 0, 0, 0, 0, 
                                        0, 0, 0, 0, 0, 0, 0, 0, 
                                        0, 2, 4, 6, 8, 10, 12, 14, 
                                        16, 18, 20, 22, 24, 26, 28, 30, 
                                        30, 30, 30, 30, 30, 30, 30, 30, 
                                        30, 30, 30, 30, 30, 30, 30, 30};
Void_WMV g_EndDstTables_WMV(tWMVDecInternalMember *pWMVDec)
{
#ifdef _6BIT_COLORCONV_OPT_
    wmvfree(pWMVDec->p_yuv2rgb_6bit_table);
    wmvfree(pWMVDec->m_rgiClapTab6BitDec - 256);
#endif
    VirtualFree(pWMVDec->m_rgiClapTab4BitDec - 32, 0, MEM_RELEASE);
    VirtualFree(pWMVDec->m_rgiDithering, 0, MEM_RELEASE);
}
#endif

tWMVDecodeStatus g_InitDstTables_WMV (tWMVDecInternalMember *pWMVDec)
{

    I32_WMV i;

#ifdef _6BIT_COLORCONV_OPT_
    pWMVDec->p_yuv2rgb_6bit_table = (YUV2RGB_6BIT_TABLE * ) wmvalloc (sizeof(YUV2RGB_6BIT_TABLE));
    if(pWMVDec->p_yuv2rgb_6bit_table == NULL)
        return WMV_BadMemory;
    I32_WMV iClapTabCenter = 256; // for clapping table
    pWMVDec->m_rgiClapTab6BitDec = (U8_WMV *) wmvalloc(512) ;
    if(pWMVDec->m_rgiClapTab6BitDec == NULL)
        return WMV_BadMemory;
    pWMVDec->m_rgiClapTab6BitDec += 256;
    for (i = -iClapTabCenter; i < iClapTabCenter; i++)
            pWMVDec->m_rgiClapTab6BitDec [i] = (U8_WMV) ((i < 0) ? 0 : (i > 63) ? 63 : i);
#endif

#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4)
    pWMVDec->m_rgiClapTab4BitDec = (U8_WMV *) VirtualAlloc (0, 64, MEM_COMMIT, PAGE_READWRITE|PAGE_ARM_MINICACHE);
    if(pWMVDec->m_rgiClapTab4BitDec == NULL)
        return WMV_BadMemory;
    memcpy (pWMVDec->m_rgiClapTab4BitDec, g_uiClapTab4BitDec, 64);
    I32_WMV iClapTabCenter4 = 32;
    pWMVDec->m_rgiClapTab4BitDec += iClapTabCenter4;
    pWMVDec->m_rgiDithering = (U32_WMV *) VirtualAlloc (0, 96, MEM_COMMIT, PAGE_READWRITE|PAGE_ARM_MINICACHE);
    if(pWMVDec->m_rgiDithering == NULL)
        return WMV_BadMemory;
    memcpy (pWMVDec->m_rgiDithering, g_uiDithering444, 96);
    pWMVDec->m_iDitheringIndex = 3;
#endif

    for (i = 0; i < 256; i++) {
#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB8) || defined(OUTPUT_RGB16) || defined(OUTPUT_RGB24) || defined(OUTPUT_RGB32)
        I32_WMV shift_value=16;
        g_iVtoR_WMV [i] = ((I32_WMV) 104597L * (I32_WMV) (i - 128)) >> shift_value;
        g_iVtoG_WMV [i] = ((I32_WMV) 53279L * (I32_WMV) (i - 128)) >> shift_value;
        g_iUtoG_WMV [i] = ((I32_WMV) 25675L * (I32_WMV) (i - 128)) >> shift_value;
        g_iUtoB_WMV [i] = ((I32_WMV) 132201L * (I32_WMV) (i - 128)) >> shift_value;
        g_iYscale_WMV [i] = ((I32_WMV) 76309L * (I32_WMV) (i - 16)) >> shift_value;
#endif

#ifdef _6BIT_COLORCONV_OPT_
        shift_value=18;
        pWMVDec->p_yuv2rgb_6bit_table->g_iVtoR [i] = ((I32_WMV) 104597L * (I32_WMV) (i - 128)) >> shift_value;
        pWMVDec->p_yuv2rgb_6bit_table->g_iVtoG [i] = ((I32_WMV) 53279L * (I32_WMV) (i - 128)) >> shift_value;
        pWMVDec->p_yuv2rgb_6bit_table->g_iUtoG [i] = ((I32_WMV) 25675L * (I32_WMV) (i - 128)) >> shift_value;
        pWMVDec->p_yuv2rgb_6bit_table->g_iUtoB [i] = ((I32_WMV) 132201L * (I32_WMV) (i - 128)) >> shift_value;
        pWMVDec->p_yuv2rgb_6bit_table->g_iYscale [i] = ((I32_WMV) 76309L * (I32_WMV) (i - 16)) >> shift_value;

#endif

#ifdef _MAC
		I32_WMV a, b;
		a = g_iYscale_WMV [i] - 23;
		YTable32 [i] = ((a&0x1FE) << 16) | ((a&0x1FE) << 8) | (a&0x1FF);
	 	
		a = (g_iUtoB_WMV [i] > 255)? 255 : ((g_iUtoB_WMV [i] < -213)? -213 : g_iUtoB_WMV [i]);
        b = (0-g_iUtoG_WMV[i]);
		UTable32 [i] = ((b&0x1FE) << 8) | (a&0x1FF);

		a = (0-g_iVtoG_WMV [i]);
        b = (g_iVtoR [i]);
		VTable32 [i] = ((b&0x1FE) << 16) | ((a&0x1FE) << 8);
#endif
    };

    return WMV_Succeeded;
}


tWMVDecodeStatus WMVideoDecInitBMPInfo (tWMVDecInternalMember *pWMVDec)
{
    // setting the funtion pointers, etc.
    if (pWMVDec->m_uiFOURCCOutput == FOURCC_BI_RGB_WMV) {
        pWMVDec->m_bYUVDstBMP = FALSE_WMV;
        // 16-bit FOURCC_BI_RGB_WMV is, by definition, RGB 5-5-5
#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)
        if (pWMVDec->m_uiBitsPerPixelOutput == 16) {
            // See WMVideoDecUpdateDstMB16 for an explanation of the following variables
            pWMVDec->m_uiRedscale = 10 - (8 - 5);
            pWMVDec->m_uiGreenscale = 5 - (8 - 5);
            pWMVDec->m_uiRedmask = 0x7c00;
            pWMVDec->m_uiGreenmask = 0x3e0;
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB16;
        } else 
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)
        if (pWMVDec->m_uiBitsPerPixelOutput == 24) {
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB24;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB24;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB24;

        } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)
        if (pWMVDec->m_uiBitsPerPixelOutput == 32) {
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB32;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB32;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB32;

        } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
        // palettized
        if (pWMVDec->m_uiBitsPerPixelOutput == 8) {
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB8;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB8;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB8;
        } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
       
            return WMV_UnSupportedOutputPixelFormat;
    }
    else if (pWMVDec->m_uiFOURCCOutput == FOURCC_BI_BITFIELDS_WMV) { // must be 16 or 32-bit
        pWMVDec->m_bYUVDstBMP = FALSE_WMV;
#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB12) || defined (OUTPUT_GRAY4) 
        if (pWMVDec->m_uiBitsPerPixelOutput == 15) { // 5-5-5
            // We only support the Windows 95 "standard" bitmap types which
            // are RGB 5-5-5 and RGB 5-6-5.  Windows NT allows for any
            // combination of bitmasks just so long as they don't overlap.
            // We don't play that game here (unless ordered to do so)

            pWMVDec->m_uiRedscale = 10-(8-5);
            pWMVDec->m_uiGreenscale = 5-(8-5);
            pWMVDec->m_uiRedmask = 0x7c00;
            pWMVDec->m_uiGreenmask = 0x3e0;
#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB16;
#endif
        }
        else if (pWMVDec->m_uiBitsPerPixelOutput == 16) { // 5-6-5
                // See WMVideoDecUpdateDstMB16 for an explanation of the following variables
            pWMVDec->m_uiRedscale = 11 - (8 - 5);
            pWMVDec->m_uiGreenscale = 5 - (8 - 6);
            pWMVDec->m_uiRedmask = 0xf800;
            pWMVDec->m_uiGreenmask = 0x7e0;
#if defined (OUTPUT_ALL_FORMATS)
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB16;
#endif  //OUTPUT_RGB16
#if defined (OUTPUT_GRAY4)
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBGRAY4;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBGRAY4;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkGRAY4;
#endif  //OUTPUT_GRAY4
#if defined (OUTPUT_RGB16)
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB16;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB16;
#endif  //OUTPUT_RGB16
#if defined (OUTPUT_RGB12)
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB12;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB12;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB12;
#endif  //OUTPUT_RGB12

        } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)
        if (pWMVDec->m_uiBitsPerPixelOutput == 32) {
            pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBRGB32;
            pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkRGB32;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBRGB32;
        } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)
            return WMV_UnSupportedOutputPixelFormat;
    } else
#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)
    if (pWMVDec->m_uiFOURCCOutput == FOURCC_UYVY_WMV) {
        pWMVDec->m_bYUVDstBMP = TRUE_WMV;
        pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBUYVY;
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBUYVY;
        pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkUYVY;
    } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)
    if (pWMVDec->m_uiFOURCCOutput == FOURCC_YVYU_WMV) {
        pWMVDec->m_bYUVDstBMP = TRUE_WMV;
        pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBYVYU;
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBYVYU;
        pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkYVYU;
    } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)
    if (pWMVDec->m_uiFOURCCOutput == FOURCC_YUY2_WMV) {
        pWMVDec->m_bYUVDstBMP = TRUE_WMV;
        pWMVDec->m_pWMVideoDecUpdateDstMB = WMVideoDecUpdateDstMBYUY2;
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB = WMVideoDecUpdateDstPartialMBYUY2;
        pWMVDec->m_pWMVideoDecUpdateDstBlk = WMVideoDecUpdateDstBlkYUY2;
    } else
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)
        return WMV_UnSupportedOutputPixelFormat;

    // set the BMP parameters
    pWMVDec->m_iWidthBMP = BITMAP_WIDTH_WMV (pWMVDec->m_iWidthSource, pWMVDec->m_uiBitsPerPixelOutput);
#ifdef _XBOX
        pWMVDec->m_iBMPPointerStart = 0;
#else
    if (pWMVDec->m_bYUVDstBMP == FALSE_WMV && pWMVDec->m_iHeightSource > 0) {
        pWMVDec->m_iWidthBMP = -pWMVDec->m_iWidthBMP;
        pWMVDec->m_iBMPPointerStart = (pWMVDec->m_iHeightSource - 1) * ((pWMVDec->m_iWidthSource * pWMVDec->m_uiBitsPerPixelOutput / 8 + 3) & ~3);
    }
    else 
        pWMVDec->m_iBMPPointerStart = 0;
#endif

    pWMVDec->m_iBMPMBIncrement = MB_SIZE * pWMVDec->m_uiBitsPerPixelOutput / 8;
    pWMVDec->m_iBMPBlkIncrement = BLOCK_SIZE * pWMVDec->m_uiBitsPerPixelOutput / 8;
    pWMVDec->m_iBMPMBHeightIncrement = pWMVDec->m_iWidthBMP * MB_SIZE;; 
    pWMVDec->m_iBMPBlkHeightIncrement = pWMVDec->m_iWidthBMP * BLOCK_SIZE;;


    return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecColorConvert (tWMVDecInternalMember *pWMVDec, U8_WMV* pucDecodedOutput)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    U32_WMV uiX, uiY;
    U32_WMV uiNumMBX = pWMVDec->m_uiNumMBX;
    U32_WMV uiNumMBY = pWMVDec->m_uiNumMBY;
    FUNCTION_PROFILE_DECL_START(fp,COLORCONVERTANDRENDER_PROFILE);

    U8_WMV* pucCurrY;
    U8_WMV* pucCurrU;
    U8_WMV* pucCurrV;
    if (pWMVDec->m_iPostProcessMode <= 0 || (pWMVDec->m_tFrmType == IVOP && pWMVDec->m_bXintra8)) {
        pucCurrY = pWMVDec->m_ppxliCurrQPlusExpY;
        pucCurrU = pWMVDec->m_ppxliCurrQPlusExpU;
        pucCurrV = pWMVDec->m_ppxliCurrQPlusExpV;
    }
    else {
        pucCurrY = pWMVDec->m_ppxliPostQPlusExpY;
        pucCurrU = pWMVDec->m_ppxliPostQPlusExpU;
        pucCurrV = pWMVDec->m_ppxliPostQPlusExpV;    
    }

    U8_WMV* pBmpPtr = pucDecodedOutput + pWMVDec->m_iBMPPointerStart;
//Bool_WMV* pbSkipMB = pWMVDec->m_pbSkipMB;
//Bool_WMV* pbSkipMB;
    for (uiY = 0; uiY < uiNumMBY; uiY++) {
        U8_WMV* pucCurrYMB = pucCurrY;
        U8_WMV* pucCurrUMB = pucCurrU;
        U8_WMV* pucCurrVMB = pucCurrV;
        U8_WMV* pBmpPtrMB = pBmpPtr;
        for (uiX = 0; uiX < uiNumMBX; uiX++) {
            if (1){//*pbSkipMB == FALSE_WMV)
                if (uiX != uiNumMBX - 1 && uiY != uiNumMBY - 1) {
                    pWMVDec->m_pWMVideoDecUpdateDstMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        pucCurrYMB, pucCurrUMB, pucCurrVMB,
                        pWMVDec->m_iWidthInternal, pWMVDec->m_iWidthInternalUV, 
                        pWMVDec->m_iWidthBMP);
                }
                else {
                    I32_WMV iValidWidth = 
                        (uiX != (uiNumMBX - 1)) ? MB_SIZE :
                        (MB_SIZE - (pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc));

                    I32_WMV iValidHeight = 
                        (uiY != (uiNumMBY - 1)) ? MB_SIZE :
                        (MB_SIZE - (abs(pWMVDec -> m_iHeightY) - abs(pWMVDec -> m_iFrmHeightSrc)));

                    pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        pucCurrYMB, pucCurrUMB, pucCurrVMB,
                        pWMVDec->m_iWidthInternal, pWMVDec->m_iWidthInternalUV, 
                        iValidWidth, iValidHeight);
                }
            }
            // pbSkipMB++;
            pucCurrYMB += MB_SIZE;
            pucCurrUMB += BLOCK_SIZE;
            pucCurrVMB += BLOCK_SIZE;
            pBmpPtrMB += pWMVDec->m_iBMPMBIncrement;
        }
        pucCurrY += pWMVDec->m_iWidthInternalTimesMB;
        pucCurrU += pWMVDec->m_iWidthInternalUVTimesBlk;
        pucCurrV += pWMVDec->m_iWidthInternalUVTimesBlk;
        pBmpPtr += pWMVDec->m_iBMPMBHeightIncrement;
    }

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

#define wDITH24(xy,r,g,b) \
    (rgDitherMap [xy       + r] + \
     rgDitherMap [xy + 256 + g] + \
     rgDitherMap [xy + 512 + b]  )

Void_WMV WMVideoDecUpdateDstPartialMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer
//
    U8_WMV  mb[MB_SIZE * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBRGB8 (
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy (pBits, pBitsMb, iRgbWidth);

        pBitsMb += MB_SIZE;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    U8_WMV rVal, gVal, bVal;
    U32_WMV iy;
    I32_WMV iYScale;

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U8_WMV * rgDitherMap = (U8_WMV *)(&(pWMVDec->m_rgDitherMap[0][0][0][0]));

    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        I32_WMV iyAnd3 = (iy * 2) & 3;
        I32_WMV     xy = 3072*iyAnd3;
        
        I32_WMV iVtoR0 = g_iVtoR_WMV [pucCurrV [0]];
        I32_WMV iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU [0]] + g_iVtoG_WMV [pucCurrV [0]];
        I32_WMV iUtoB0 = g_iUtoB_WMV [pucCurrU [0]];

        I32_WMV iVtoR1 = g_iVtoR_WMV [pucCurrV [1]];
        I32_WMV iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU [1]] + g_iVtoG_WMV[pucCurrV [1]];
        I32_WMV iUtoB1 = g_iUtoB_WMV [pucCurrU [1]];

        I32_WMV iVtoR2 = g_iVtoR_WMV [pucCurrV [2]];
        I32_WMV iUtoG2PlusVtoG2 = g_iUtoG_WMV [pucCurrU [2]] + g_iVtoG_WMV [pucCurrV [2]];
        I32_WMV iUtoB2 = g_iUtoB_WMV [pucCurrU [2]];

        I32_WMV iVtoR3 = g_iVtoR_WMV [pucCurrV [3]];
        I32_WMV iUtoG3PlusVtoG3 = g_iUtoG_WMV [pucCurrU [3]] + g_iVtoG_WMV [pucCurrV [3]];
        I32_WMV iUtoB3 = g_iUtoB_WMV [pucCurrU [3]];

        I32_WMV iVtoR4 = g_iVtoR_WMV [pucCurrV[4]];
        I32_WMV iUtoG4PlusVtoG4 = g_iUtoG_WMV [pucCurrU [4]] + g_iVtoG_WMV[pucCurrV [4]];
        I32_WMV iUtoB4 = g_iUtoB_WMV[pucCurrU [4]];

        I32_WMV iVtoR5 = g_iVtoR_WMV [pucCurrV [5]];
        I32_WMV iUtoG5PlusVtoG5 = g_iUtoG_WMV [pucCurrU [5]] + g_iVtoG_WMV [pucCurrV [5]];
        I32_WMV iUtoB5 = g_iUtoB_WMV[pucCurrU [5]];

        I32_WMV iVtoR6 = g_iVtoR_WMV [pucCurrV [6]];
        I32_WMV iUtoG6PlusVtoG6 = g_iUtoG_WMV [pucCurrU [6]] + g_iVtoG_WMV [pucCurrV [6]];
        I32_WMV iUtoB6 = g_iUtoB_WMV [pucCurrU [6]];

        I32_WMV iVtoR7 = g_iVtoR_WMV [pucCurrV [7]];
        I32_WMV iUtoG7PlusVtoG7 = g_iUtoG_WMV [pucCurrU [7]] + g_iVtoG_WMV [pucCurrV [7]];
        I32_WMV iUtoB7 = g_iUtoB_WMV [pucCurrU [7]];

        iYScale = g_iYscale_WMV [pucCurrY [0]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        pBits [0] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [4] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [8]];
        rVal = rgiClapTab [(iYScale + iVtoR4)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB4)]; // b
        pBits [8] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [12]];
        rVal = rgiClapTab [(iYScale + iVtoR6)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB6)]; // b
        pBits [12] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [1]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        xy += 768;
        pBits [1] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [5]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [5] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [9]];
        rVal = rgiClapTab [(iYScale + iVtoR4)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB4)]; // b
        pBits [9] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [13]];
        rVal = rgiClapTab [(iYScale + iVtoR6)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB6)]; // b
        pBits [13] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [2]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [2] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [6]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [6] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [10]];
        rVal = rgiClapTab [(iYScale + iVtoR5)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB5)]; // b
        pBits [10] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [14]];
        rVal = rgiClapTab [(iYScale + iVtoR7)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB7)]; // b
        pBits [14] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [3] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [7]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [7] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [11]];
        rVal = rgiClapTab [(iYScale + iVtoR5)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB5)]; // b
        pBits [11] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [15]];
        rVal = rgiClapTab [(iYScale + iVtoR7)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB7)]; // b
        pBits [15] = wDITH24 (xy, rVal, gVal, bVal);
		pucCurrY += iWidthY;
		pucCurrU += iWidthUV;
		pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iyAnd3 = (iy * 2 + 1) & 3;
        xy = 3072*iyAnd3;
        
        iYScale = g_iYscale_WMV [pucCurrY [0]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        pBits [0] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [4] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [8]];
        rVal = rgiClapTab [(iYScale + iVtoR4)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB4)]; // b
        pBits [8] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [12]];
        rVal = rgiClapTab [(iYScale + iVtoR6)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB6)]; // b
        pBits [12] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [1]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        xy += 768;
        pBits [1] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [5]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [5] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [9]];
        rVal = rgiClapTab [(iYScale + iVtoR4)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB4)]; // b
        pBits [9] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [13]];
        rVal = rgiClapTab [(iYScale + iVtoR6)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB6)]; // b
        pBits [13] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [2]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [2] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [6]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [6] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [10]];
        rVal = rgiClapTab [(iYScale + iVtoR5)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB5)]; // b
        pBits [10] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [14]];
        rVal = rgiClapTab [(iYScale + iVtoR7)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB7)]; // b
        pBits [14] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [3] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [7]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [7] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [11]];
        rVal = rgiClapTab [(iYScale + iVtoR5)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB5)]; // b
        pBits [11] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [15]];
        rVal = rgiClapTab [(iYScale + iVtoR7)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB7)]; // b
        pBits [15] = wDITH24 (xy, rVal, gVal, bVal);

        pucCurrY += iWidthY;

        pBits += iBitmapWidth;
    }
}

Void_WMV WMVideoDecUpdateDstBlkRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    U8_WMV rVal, gVal, bVal;
    U32_WMV iy;
    I32_WMV iYScale;

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U8_WMV * rgDitherMap = (U8_WMV *)(&(pWMVDec->m_rgDitherMap[0][0][0][0]));

    for (iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
        I32_WMV iyAnd3 = (iy * 2) & 3;
        I32_WMV xy = 3072 * iyAnd3;
        
        I32_WMV iVtoR0 = g_iVtoR_WMV [pucCurrV [0]];
        I32_WMV iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU [0]] + g_iVtoG_WMV [pucCurrV [0]];
        I32_WMV iUtoB0 = g_iUtoB_WMV [pucCurrU [0]];

        I32_WMV iVtoR1 = g_iVtoR_WMV [pucCurrV [1]];
        I32_WMV iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU [1]] + g_iVtoG_WMV[pucCurrV [1]];
        I32_WMV iUtoB1 = g_iUtoB_WMV [pucCurrU [1]];

        I32_WMV iVtoR2 = g_iVtoR_WMV [pucCurrV [2]];
        I32_WMV iUtoG2PlusVtoG2 = g_iUtoG_WMV [pucCurrU [2]] + g_iVtoG_WMV [pucCurrV [2]];
        I32_WMV iUtoB2 = g_iUtoB_WMV [pucCurrU [2]];

        I32_WMV iVtoR3 = g_iVtoR_WMV [pucCurrV [3]];
        I32_WMV iUtoG3PlusVtoG3 = g_iUtoG_WMV [pucCurrU [3]] + g_iVtoG_WMV [pucCurrV [3]];
        I32_WMV iUtoB3 = g_iUtoB_WMV [pucCurrU [3]];

        iYScale = g_iYscale_WMV [pucCurrY [0]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        pBits [0] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [4] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [1]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        xy += 768;
        pBits [1] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [5]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [5] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [2]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [2] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [6]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [6] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [3] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [7]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [7] = wDITH24 (xy, rVal, gVal, bVal);

		pucCurrY += iWidthY;
		pucCurrU += iWidthUV;
		pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iyAnd3 = (iy * 2 + 1) & 3;
        xy = 3072*iyAnd3;
        
        iYScale = g_iYscale_WMV [pucCurrY [0]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        pBits [0] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [4] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [1]];
        rVal = rgiClapTab [(iYScale + iVtoR0)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB0)]; // b
        xy += 768;
        pBits [1] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [5]];
        rVal = rgiClapTab [(iYScale + iVtoR2)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB2)]; // b
        pBits [5] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [2]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [2] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [6]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [6] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        rVal = rgiClapTab [(iYScale + iVtoR1)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB1)]; // b
        xy += 768;
        pBits [3] = wDITH24 (xy, rVal, gVal, bVal);

        iYScale = g_iYscale_WMV [pucCurrY [7]];
        rVal = rgiClapTab [(iYScale + iVtoR3)]; // r
        gVal = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        bVal = rgiClapTab [(iYScale + iUtoB3)]; // b
        pBits [7] = wDITH24 (xy, rVal, gVal, bVal);

        pucCurrY += iWidthY;

        pBits += iBitmapWidth;
    }
}
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#define UpdateDstRGB15(i, iVtoR0, iUtoG0PlusVtoG0, iUtoB0) \
    iYScale = g_iYscale_WMV[pucCurrY[2*i+1]];             \
    rVal = rgiClapTab [(iYScale + iVtoR0)];             \
    gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)];    \
    bVal = rgiClapTab [(iYScale + iUtoB0)];             \
    rVal <<= 16; gVal <<= 16; bVal <<= 16;              \
    iYScale = g_iYscale_WMV[pucCurrY[2*i]];               \
    rVal += rgiClapTab [(iYScale + iVtoR0)];            \
    gVal += rgiClapTab [(iYScale - iUtoG0PlusVtoG0)];   \
    bVal += rgiClapTab [(iYScale + iUtoB0)];            \
    pdwBits[i] = ((rVal << 7) & 0x7c007c00) |           \
        ((gVal << 2) &  0x3e003e0) |                    \
        ((bVal >> 3) &   0x1f001f) ;

#define UpdateDstRGB16(i, iVtoR0, iUtoG0PlusVtoG0, iUtoB0) \
    iYScale = g_iYscale_WMV[pucCurrY[2*i+1]];             \
    rVal = rgiClapTab [(iYScale + iVtoR0)];             \
    gVal = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)];    \
    bVal = rgiClapTab [(iYScale + iUtoB0)];             \
    rVal <<= 16; gVal <<= 16; bVal <<= 16;              \
    iYScale = g_iYscale_WMV[pucCurrY[2*i]];               \
    rVal += rgiClapTab [(iYScale + iVtoR0)];            \
    gVal += rgiClapTab [(iYScale - iUtoG0PlusVtoG0)];   \
    bVal += rgiClapTab [(iYScale + iUtoB0)];            \
    pdwBits[i] = ((rVal << 8) & 0xf800f800) |           \
        ((gVal << 3) &  0x7e007e0) |                    \
        ((bVal >> 3) &   0x1f001f) ;

Void_WMV WMVideoDecUpdateDstPartialMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer
//
    U8_WMV  mb[MB_SIZE * 2 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBRGB16 (
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

#if !defined( OPT_DECOLORCONV_ARM) 
#ifndef IPP_COLORCONV
#ifndef _6BIT_COLORCONV_OPT_
Void_WMV WMVideoDecUpdateDstMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV                   rVal, gVal, bVal;
    U32_WMV           *pdwBits = (U32_WMV*) pBits;
    U32_WMV                  iy;
    I32_WMV                   iYScale;

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
//
// We need to do the following (not including the basic YUV to
// RGB 8-8-8 conversion):
//
// First, scale from RGB 8-8-8 to RGB 5-5-5 or RGB 5-6-5.  The 8-5
// below is the difference in the number of bits between the
// formats.
//
// pWMVDec->m_uiGreenscale is either 8-6 or 8-5 depending on 5-6-5 or 5-5-5
// respectively. pWMVDec->m_uiGreenmask is either 0x7e0 or 0x3e0.
//
// rVal = (rVal >> 8-5) & 0x1f;
// gVal = (gVal >> pWMVDec->m_uiGreenscale) & pWMVDec->m_uiGreenmask;
// bVal = (bVal >> 8-5) & 0x1f;
//
// Then we need to shift the r and g into their proper positions
// (b is shifted zero)
//
// m_redshift is either 10 or 11 depending on 5-5-5 or 5-6-5,
// respectively.
//
// rval <<= m_redshift;
// gval <<= 5;
//
// And finally, write the pixel
//
// *(WORD *)pBits = rVal | gVal | bVal;
//
// We can reduce this by combining the shifts and scales into a
// single shift.
//
// pWMVDec->m_uiRedscale = RGBT_555 ? 10-(8-5) : 11-(8-5)
// pWMVDec->m_uiGreenscale = RGBT_555 ? 5-(8-5) : 5-(8-6);
// bluescale = 0-(8-5); // actually must be right shifted.by 3
//
// Since after the shifts, we have non-significant bits overlapping
// the fields, we have to mask them off.
//
// pWMVDec->m_uiRedmask = RGBT_555 ? 0x7c00 : 0xf800;
// pWMVDec->m_uiGreenmask = RGBT_555 ? 0x3e0 : 0x7e0;
// bluemask = 0x1f;
//
// So we end up with
//
// *(WORD *)pBits = ((rVal << pWMVDec->m_uiRedscale) & pWMVDec->m_uiRedmask) |
//     ((gVal << pWMVDec->m_uiGreenscale) & pWMVDec->m_uiGreenmask) |
//     ((bVal >> bluescale) & bluemask);
//
// Calculation of all of the scale and mask values is moved to the
// init function.  bluescale and bluemask are constant 3 and 0x1f.
//
    if (pWMVDec->m_uiRedscale == 7) {
        for (iy = 0; iy < BLOCK_SIZE; iy++) {
            I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
            I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
            I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
            I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
            I32_WMV iVtoR4, iUtoG4PlusVtoG4, iUtoB4;
            I32_WMV iVtoR5, iUtoG5PlusVtoG5, iUtoB5;
            I32_WMV iVtoR6, iUtoG6PlusVtoG6, iUtoB6;
            I32_WMV iVtoR7, iUtoG7PlusVtoG7, iUtoB7;

            iVtoR0 = g_iVtoR_WMV[pucCurrV[0]];
            iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
            iUtoB0 = g_iUtoB_WMV[pucCurrU[0]];            
            UpdateDstRGB15 (0, iVtoR0, iUtoG0PlusVtoG0, iUtoB0)           

            iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
            iUtoG1PlusVtoG1 = g_iUtoG_WMV [pucCurrU[1]] + g_iVtoG_WMV [pucCurrV[1]];
            iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];
            UpdateDstRGB15 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           

            iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
            iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
            iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
            UpdateDstRGB15 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           

            iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
            iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
            iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
            UpdateDstRGB15 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           

            iVtoR4 = g_iVtoR_WMV[pucCurrV[4]];
            iUtoG4PlusVtoG4 = g_iUtoG_WMV[pucCurrU[4]] + g_iVtoG_WMV[pucCurrV[4]];
            iUtoB4 = g_iUtoB_WMV[pucCurrU[4]];
            UpdateDstRGB15 (4,iVtoR4,iUtoG4PlusVtoG4,iUtoB4)           

            iVtoR5 = g_iVtoR_WMV[pucCurrV[5]];
            iUtoG5PlusVtoG5 = g_iUtoG_WMV[pucCurrU[5]] + g_iVtoG_WMV[pucCurrV[5]];
            iUtoB5 = g_iUtoB_WMV[pucCurrU[5]];
            UpdateDstRGB15 (5,iVtoR5,iUtoG5PlusVtoG5,iUtoB5)           

            iVtoR6 = g_iVtoR_WMV[pucCurrV[6]];
            iUtoG6PlusVtoG6 = g_iUtoG_WMV[pucCurrU[6]] + g_iVtoG_WMV[pucCurrV[6]];
            iUtoB6 = g_iUtoB_WMV[pucCurrU[6]];
            UpdateDstRGB15 (6,iVtoR6,iUtoG6PlusVtoG6,iUtoB6)           

            iVtoR7 = g_iVtoR_WMV[pucCurrV[7]];
            iUtoG7PlusVtoG7 = g_iUtoG_WMV[pucCurrU[7]] + g_iVtoG_WMV[pucCurrV[7]];
            iUtoB7 = g_iUtoB_WMV[pucCurrU[7]];
            UpdateDstRGB15 (7,iVtoR7,iUtoG7PlusVtoG7,iUtoB7)           

            pucCurrY += iWidthY;
            pucCurrU += iWidthUV;
            pucCurrV += iWidthUV;

            pBits += iBitmapWidth;

            pdwBits = (U32_WMV *)pBits;

            UpdateDstRGB15 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           
            UpdateDstRGB15 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
            UpdateDstRGB15 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           
            UpdateDstRGB15 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           
            UpdateDstRGB15 (4,iVtoR4,iUtoG4PlusVtoG4,iUtoB4)           
            UpdateDstRGB15 (5,iVtoR5,iUtoG5PlusVtoG5,iUtoB5)           
            UpdateDstRGB15 (6,iVtoR6,iUtoG6PlusVtoG6,iUtoB6)           
            UpdateDstRGB15 (7,iVtoR7,iUtoG7PlusVtoG7,iUtoB7)           

            pucCurrY += iWidthY;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV *)pBits;
        }

    }
    else {
        for (iy = 0; iy < BLOCK_SIZE; iy++) {
            I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
            I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
            I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
            I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
            I32_WMV iVtoR4, iUtoG4PlusVtoG4, iUtoB4;
            I32_WMV iVtoR5, iUtoG5PlusVtoG5, iUtoB5;
            I32_WMV iVtoR6, iUtoG6PlusVtoG6, iUtoB6;
            I32_WMV iVtoR7, iUtoG7PlusVtoG7, iUtoB7;

            iVtoR0 = g_iVtoR_WMV[pucCurrV[0]];
            iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
            iUtoB0 = g_iUtoB_WMV[pucCurrU[0]];
            UpdateDstRGB16 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           

            iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
            iUtoG1PlusVtoG1 = g_iUtoG_WMV [pucCurrU[1]] + g_iVtoG_WMV [pucCurrV[1]];
            iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];            
            UpdateDstRGB16 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
                
            iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
            iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
            iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
            UpdateDstRGB16 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           

            iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
            iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
            iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
            UpdateDstRGB16 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           

            iVtoR4 = g_iVtoR_WMV[pucCurrV[4]];
            iUtoG4PlusVtoG4 = g_iUtoG_WMV[pucCurrU[4]] + g_iVtoG_WMV[pucCurrV[4]];
            iUtoB4 = g_iUtoB_WMV[pucCurrU[4]];
            UpdateDstRGB16 (4,iVtoR4,iUtoG4PlusVtoG4,iUtoB4)           

            iVtoR5 = g_iVtoR_WMV[pucCurrV[5]];
            iUtoG5PlusVtoG5 = g_iUtoG_WMV[pucCurrU[5]] + g_iVtoG_WMV[pucCurrV[5]];
            iUtoB5 = g_iUtoB_WMV[pucCurrU[5]];
            UpdateDstRGB16 (5,iVtoR5,iUtoG5PlusVtoG5,iUtoB5)           

            iVtoR6 = g_iVtoR_WMV[pucCurrV[6]];
            iUtoG6PlusVtoG6 = g_iUtoG_WMV[pucCurrU[6]] + g_iVtoG_WMV[pucCurrV[6]];
            iUtoB6 = g_iUtoB_WMV[pucCurrU[6]];
            UpdateDstRGB16 (6,iVtoR6,iUtoG6PlusVtoG6,iUtoB6)           
                    
            iVtoR7 = g_iVtoR_WMV[pucCurrV[7]];
            iUtoG7PlusVtoG7 = g_iUtoG_WMV[pucCurrU[7]] + g_iVtoG_WMV[pucCurrV[7]];
            iUtoB7 = g_iUtoB_WMV[pucCurrU[7]];
            UpdateDstRGB16 (7,iVtoR7,iUtoG7PlusVtoG7,iUtoB7)           

            pucCurrY += iWidthY;
            pucCurrU += iWidthUV;
            pucCurrV += iWidthUV;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV *)pBits;

            UpdateDstRGB16 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           
            UpdateDstRGB16 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
            UpdateDstRGB16 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           
            UpdateDstRGB16 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           
            UpdateDstRGB16 (4,iVtoR4,iUtoG4PlusVtoG4,iUtoB4)           
            UpdateDstRGB16 (5,iVtoR5,iUtoG5PlusVtoG5,iUtoB5)           
            UpdateDstRGB16 (6,iVtoR6,iUtoG6PlusVtoG6,iUtoB6)           
            UpdateDstRGB16 (7,iVtoR7,iUtoG7PlusVtoG7,iUtoB7)           
                
            pucCurrY += iWidthY;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV*) pBits;
        }
    }
}
#endif

Void_WMV WMVideoDecUpdateDstBlkRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV                   rVal, gVal, bVal;
    U32_WMV          *pdwBits = (U32_WMV*) pBits;
    U32_WMV                  iy;
    I32_WMV                   iYScale;

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    if (pWMVDec->m_uiRedscale == 7) {
        for (iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
            I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
            I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
            I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
            I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;

            iVtoR0 = g_iVtoR_WMV[pucCurrV[0]];
            iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
            iUtoB0 = g_iUtoB_WMV[pucCurrU[0]];
            UpdateDstRGB15 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           

            iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
            iUtoG1PlusVtoG1 = g_iUtoG_WMV [pucCurrU[1]] + g_iVtoG_WMV [pucCurrV[1]];
            iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];
            UpdateDstRGB15 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           

            iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
            iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
            iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
            UpdateDstRGB15 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           

            iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
            iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
            iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
            UpdateDstRGB15 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           

            pucCurrY += iWidthY;
            pucCurrU += iWidthUV;
            pucCurrV += iWidthUV;

            pBits += iBitmapWidth;

            pdwBits = (U32_WMV*) pBits;

            UpdateDstRGB15 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           
            UpdateDstRGB15 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
            UpdateDstRGB15 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           
            UpdateDstRGB15 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           

            pucCurrY += iWidthY;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV*) pBits;
        }

    }
    else {
        for (iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
            I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
            I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
            I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
            I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;

            iVtoR0 = g_iVtoR_WMV[pucCurrV[0]];
            iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
            iUtoB0 = g_iUtoB_WMV[pucCurrU[0]];
            UpdateDstRGB16 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           

            iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
            iUtoG1PlusVtoG1 = g_iUtoG_WMV [pucCurrU[1]] + g_iVtoG_WMV [pucCurrV[1]];
            iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];
            UpdateDstRGB16 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
                
            iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
            iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
            iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
            UpdateDstRGB16 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           

            iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
            iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
            iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
            UpdateDstRGB16 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           

            pucCurrY += iWidthY;
            pucCurrU += iWidthUV;
            pucCurrV += iWidthUV;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV*) pBits;

            UpdateDstRGB16 (0,iVtoR0,iUtoG0PlusVtoG0,iUtoB0)           
            UpdateDstRGB16 (1,iVtoR1,iUtoG1PlusVtoG1,iUtoB1)           
            UpdateDstRGB16 (2,iVtoR2,iUtoG2PlusVtoG2,iUtoB2)           
            UpdateDstRGB16 (3,iVtoR3,iUtoG3PlusVtoG3,iUtoB3)           
                
            pucCurrY += iWidthY;

            pBits += iBitmapWidth;
            pdwBits = (U32_WMV*) pBits;
        }
    }
}
#else // IPP_COLORCONV

Void_WMV WMVideoDecUpdateDstMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* ppxliCurrQYMB,
    const U8_WMV* ppxliCurrQUMB,
    const U8_WMV* ppxliCurrQVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
  
	const Ipp8u* const pSrc[3] = { (Ipp8u*) ppxliCurrQYMB,
								   (Ipp8u*) ppxliCurrQUMB,
								   (Ipp8u*) ppxliCurrQVMB};

	int srcStep[3];
	srcStep[0] = iWidthY;
	srcStep[1] = iWidthUV;
	srcStep[2] = iWidthUV;

	Ipp16u* pDst;
	pDst = (Ipp16u*) pBits;

	int dstStep = iBitmapWidth;

	IppiSize roiSize;
	roiSize.height = 16;
	roiSize.width  = 16;

	//ippiYUV420ToRGB565_8u16u_P3C3R(pSrc, srcStep, pDst, dstStep, roiSize);
    ippiYUV420ToBGR565_8u16u_P3C3R(pSrc, srcStep, pDst, dstStep, roiSize);
};

Void_WMV WMVideoDecUpdateDstBlkRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* ppxliCurrQYMB,
    const U8_WMV* ppxliCurrQUMB,
    const U8_WMV* ppxliCurrQVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
  
	const Ipp8u* const pSrc[3] = { (Ipp8u*) ppxliCurrQYMB,
								   (Ipp8u*) ppxliCurrQUMB,
								   (Ipp8u*) ppxliCurrQVMB};

	int srcStep[3];
	srcStep[0] = iWidthY;
	srcStep[1] = iWidthUV;
	srcStep[2] = iWidthUV;

	Ipp16u* pDst;
	pDst = (Ipp16u*) pBits;

	int dstStep = iBitmapWidth;

	IppiSize roiSize;
	roiSize.height = 8;
	roiSize.width  = 8;

	//ippiYUV420ToRGB565_8u16u_P3C3R(pSrc, srcStep, pDst, dstStep, roiSize);
    ippiYUV420ToBGR565_8u16u_P3C3R(pSrc, srcStep, pDst, dstStep, roiSize);
};
#endif // IPP_COLORCONV
#endif //OPT_DECOLORCONV_ARM
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4)
#define UpdateGray4(iUV, iY0, iY1) \
    iYtoRGB = rgiClapTab [(pucCurrY[iY1] + (uiDither >> 28)) >> 4];\
    uiDither <<= 1;\
    rVal = rgiClapTab [(pucCurrY[iY0] + (uiDither >> 28)) >> 4];\
    uiDither <<= 1;\
    iYtoRGB = rVal + (iYtoRGB << 16);\
    ((U32_WMV*)pBits) [iUV] = (iYtoRGB << 11) | (iYtoRGB << 6) | iYtoRGB;\
    iYtoRGB = rgiClapTab [((pucCurrY + iWidthY) [iY1] + (uiDither >> 28)) >> 4];\
    uiDither <<= 1;\
    rVal = rgiClapTab [((pucCurrY + iWidthY) [iY0] + (uiDither >> 28)) >> 4];\
    uiDither <<= 1;\
    iYtoRGB = rVal + (iYtoRGB << 16);\
    ((U32_WMV*)(pBits + iBitmapWidth)) [iUV] = (iYtoRGB << 11) | (iYtoRGB << 6) | iYtoRGB;

Void_WMV WMVideoDecUpdateDstPartialMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV, 
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
)
{
//
// Temporary macroblock-sized buffer
//
    U8_WMV  mb[MB_SIZE * 2 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBGRAY4 (
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
    U32_WMV uiDither;
    I32_WMV rVal, iYtoRGB;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTab4BitDec;
    pWMVDec->m_iDitheringIndex -= 1;
    pWMVDec->m_iDitheringIndex = (pWMVDec->m_iDitheringIndex < 0)? 2 : pWMVDec->m_iDitheringIndex;
    const U32_WMV* piDithering = pWMVDec->m_rgiDithering + (pWMVDec->m_iDitheringIndex << 3);
    for (U32_WMV iy = 0; iy < BLOCK_SIZE; iy++) {
        uiDither = piDithering [iy];
        UpdateGray4(0, 0, 1);
        UpdateGray4(1, 2, 3);
        UpdateGray4(2, 4, 5);
        UpdateGray4(3, 6, 7);
        UpdateGray4(4, 8, 9);
        UpdateGray4(5, 10, 11);
        UpdateGray4(6, 12, 13);
        UpdateGray4(7, 14, 15);
        pucCurrY += (iWidthY << 1);
        pBits += (iBitmapWidth << 1);
    }
}

Void_WMV WMVideoDecUpdateDstBlkGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
    U32_WMV uiDither;
    I32_WMV rVal, iYtoRGB;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTab4BitDec;
    pWMVDec->m_iDitheringIndex -= 1;
    pWMVDec->m_iDitheringIndex = (pWMVDec->m_iDitheringIndex < 0)? 2 : pWMVDec->m_iDitheringIndex;
    const U32_WMV* piDithering = pWMVDec->m_rgiDithering + (pWMVDec->m_iDitheringIndex << 3);
    for (U32_WMV iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
        uiDither = piDithering [iy];
        UpdateGray4(0, 0, 1);
        UpdateGray4(1, 2, 3);
        UpdateGray4(2, 4, 5);
        UpdateGray4(3, 6, 7);
        pucCurrY += (iWidthY << 1);
        pBits += (iBitmapWidth << 1);
    }
}
#endif //OUTPUT_GRAY4

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB12)
#define UpdateRGB12(iUV, iY0, iY1) \
    gVal = pucCurrU[iUV] - 128;\
    iUVtoG = 100 * gVal;\
    rVal = pucCurrV[iUV] - 128;\
    iVtoR = 408 * rVal;\
    iUtoB = gVal << 9;\
    iUVtoG += 208 * rVal;\
    iYtoRGB = pucCurrY[iY1] + (uiDither >> 28);\
    uiDither <<= 1;\
    rVal = rgiClapTab [(((iYtoRGB << 8) + iVtoR)) >> 12];\
    gVal = rgiClapTab [(((iYtoRGB << 8) - iUVtoG)) >> 12];\
    bVal = rgiClapTab [(((iYtoRGB << 8) + iUtoB)) >> 12];\
    iYtoRGB = pucCurrY[iY0] + (uiDither >> 28);\
    uiDither <<= 1;\
    rVal = rgiClapTab [(((iYtoRGB << 8) + iVtoR)) >> 12] + (rVal << 16);\
    gVal = rgiClapTab [(((iYtoRGB << 8) - iUVtoG)) >> 12] + (gVal << 16);\
    bVal = rgiClapTab [(((iYtoRGB << 8) + iUtoB)) >> 12] + (bVal << 16);\
    ((U32_WMV*)pBits) [iUV] = (rVal << 11) | (gVal << 6) | bVal;\
    iYtoRGB = (pucCurrY + iWidthY) [iY1] + (uiDither >> 28);\
    uiDither <<= 1;\
    rVal = rgiClapTab [(((iYtoRGB << 8) + iVtoR)) >> 12];\
    gVal = rgiClapTab [(((iYtoRGB << 8) - iUVtoG)) >> 12];\
    bVal = rgiClapTab [(((iYtoRGB << 8) + iUtoB)) >> 12];\
    iYtoRGB = (pucCurrY + iWidthY) [iY0] + (uiDither >> 28);\
    uiDither <<= 1;\
    rVal = rgiClapTab [(((iYtoRGB << 8) + iVtoR)) >> 12] + (rVal << 16);\
    gVal = rgiClapTab [(((iYtoRGB << 8) - iUVtoG)) >> 12] + (gVal << 16);\
    bVal = rgiClapTab [(((iYtoRGB << 8) + iUtoB)) >> 12] + (bVal << 16);\
    ((U32_WMV*)(pBits + iBitmapWidth)) [iUV] = (rVal << 11) | (gVal << 6) | bVal;

Void_WMV WMVideoDecUpdateDstPartialMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
)
{
//
// Temporary macroblock-sized buffer
//
    U8_WMV  mb[MB_SIZE * 2 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBRGB12 (
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

#if !defined( OPT_DECOLORCONV_ARM)
Void_WMV WMVideoDecUpdateDstMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
    U32_WMV uiDither;
    I32_WMV rVal, gVal, bVal;
    I32_WMV iYtoRGB, iVtoR, iUVtoG, iUtoB;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTab4BitDec;
    pWMVDec->m_iDitheringIndex -= 1;
    pWMVDec->m_iDitheringIndex = (pWMVDec->m_iDitheringIndex < 0)? 2 : pWMVDec->m_iDitheringIndex;
    const U32_WMV* piDithering = pWMVDec->m_rgiDithering + (pWMVDec->m_iDitheringIndex << 3);
    for (U32_WMV iy = 0; iy < BLOCK_SIZE; iy++) {
        uiDither = piDithering [iy];
        UpdateRGB12(0, 0, 1);
        UpdateRGB12(1, 2, 3);
        UpdateRGB12(2, 4, 5);
        UpdateRGB12(3, 6, 7);
        UpdateRGB12(4, 8, 9);
        UpdateRGB12(5, 10, 11);
        UpdateRGB12(6, 12, 13);
        UpdateRGB12(7, 14, 15);
        pucCurrY += (iWidthY << 1);
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
        pBits += (iBitmapWidth << 1);
    }
}

Void_WMV WMVideoDecUpdateDstBlkRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
)
{
    U32_WMV uiDither;
    I32_WMV rVal, gVal, bVal;
    I32_WMV iYtoRGB, iVtoR, iUVtoG, iUtoB;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTab4BitDec;
    pWMVDec->m_iDitheringIndex -= 1;
    pWMVDec->m_iDitheringIndex = (pWMVDec->m_iDitheringIndex < 0)? 2 : pWMVDec->m_iDitheringIndex;
    const U32_WMV* piDithering = pWMVDec->m_rgiDithering + (pWMVDec->m_iDitheringIndex << 3);
    for (U32_WMV iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
        uiDither = piDithering [iy];
        UpdateRGB12(0, 0, 1);
        UpdateRGB12(1, 2, 3);
        UpdateRGB12(2, 4, 5);
        UpdateRGB12(3, 6, 7);
        pucCurrY += (iWidthY << 1);
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
        pBits += (iBitmapWidth << 1);
    }
}
#endif //#if !defined( OPT_DECOLORCONV_ARM)
#endif //OUTPUT_RGB12

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)

Void_WMV WMVideoDecUpdateDstPartialMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer
//
    U8_WMV mb[MB_SIZE * 3 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBRGB24(
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 3);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 3);

        pBitsMb += MB_SIZE * 3;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

#ifndef OPT_DECOLORCONV_SH4
Void_WMV WMVideoDecUpdateDstMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U32_WMV iy;
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
        I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
        I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
        I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
        I32_WMV iVtoR4, iUtoG4PlusVtoG4, iUtoB4;
        I32_WMV iVtoR5, iUtoG5PlusVtoG5, iUtoB5;
        I32_WMV iVtoR6, iUtoG6PlusVtoG6, iUtoB6;
        I32_WMV iVtoR7, iUtoG7PlusVtoG7, iUtoB7;
        I32_WMV iYScale;

        iVtoR0 = g_iVtoR_WMV [pucCurrV [0]];
        iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU [0]] + g_iVtoG_WMV [pucCurrV [0]];
        iUtoB0 = g_iUtoB_WMV [pucCurrU [0]];

        iYScale = g_iYscale_WMV [pucCurrY [0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [5] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [4] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [3] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iVtoR1 = g_iVtoR_WMV [pucCurrV [1]];
        iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU [1]] + g_iVtoG_WMV[pucCurrV [1]];
        iUtoB1 = g_iUtoB_WMV [pucCurrU [1]];

        iYScale = g_iYscale_WMV[pucCurrY [2]];
        pBits [8] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [7] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [6] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        pBits [11] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [10] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [9]  = rgiClapTab [(iYScale + iUtoB1)]; // b

        iVtoR2 = g_iVtoR_WMV [pucCurrV [2]];
        iUtoG2PlusVtoG2 = g_iUtoG_WMV [pucCurrU [2]] + g_iVtoG_WMV [pucCurrV [2]];
        iUtoB2 = g_iUtoB_WMV [pucCurrU [2]];

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [17] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [16] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [15] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iVtoR3 = g_iVtoR_WMV [pucCurrV [3]];
        iUtoG3PlusVtoG3 = g_iUtoG_WMV [pucCurrU [3]] + g_iVtoG_WMV [pucCurrV [3]];
        iUtoB3 = g_iUtoB_WMV [pucCurrU [3]];
        
        iYScale = g_iYscale_WMV [pucCurrY [6]];
        pBits [20] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [19] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [18] = rgiClapTab [(iYScale + iUtoB3)]; // b
        
        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [23] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [22] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [21] = rgiClapTab [(iYScale + iUtoB3)]; // b
        
        iVtoR4 = g_iVtoR_WMV[pucCurrV[4]];
        iUtoG4PlusVtoG4 = g_iUtoG_WMV[pucCurrU[4]] + g_iVtoG_WMV[pucCurrV[4]];
        iUtoB4 = g_iUtoB_WMV[pucCurrU[4]];
        
        iYScale = g_iYscale_WMV[pucCurrY[8]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB4)]; // b
        
        iYScale = g_iYscale_WMV[pucCurrY[9]];
        pBits [29] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [28] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [27] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iVtoR5 = g_iVtoR_WMV[pucCurrV[5]];
        iUtoG5PlusVtoG5 = g_iUtoG_WMV[pucCurrU[5]] + g_iVtoG_WMV[pucCurrV[5]];
        iUtoB5 = g_iUtoB_WMV[pucCurrU[5]];

        iYScale = g_iYscale_WMV[pucCurrY[10]];
        pBits [32] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [31] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [30] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[11]];
        pBits [35] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [34] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [33] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iVtoR6 = g_iVtoR_WMV[pucCurrV[6]];
        iUtoG6PlusVtoG6 = g_iUtoG_WMV[pucCurrU[6]] + g_iVtoG_WMV[pucCurrV[6]];
        iUtoB6 = g_iUtoB_WMV[pucCurrU[6]];

        iYScale = g_iYscale_WMV[pucCurrY[12]];
        pBits [38] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [37] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [36] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[13]];
        pBits [41] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [40] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [39] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iVtoR7 = g_iVtoR_WMV[pucCurrV[7]];
        iUtoG7PlusVtoG7 = g_iUtoG_WMV[pucCurrU[7]] + g_iVtoG_WMV[pucCurrV[7]];
        iUtoB7 = g_iUtoB_WMV[pucCurrU[7]];

        iYScale = g_iYscale_WMV[pucCurrY[14]];
        pBits [44] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [43] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [42] = rgiClapTab [(iYScale + iUtoB7)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[15]];
        pBits [47] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [46] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [45] = rgiClapTab [(iYScale + iUtoB7)]; // b

        pucCurrY += iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [5] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [4] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [3] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [8] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [7] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [6] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [11] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [10] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [9]  = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [17] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [16] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [15] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [20] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [19] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [18] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [23] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [22] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [21] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[8]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[9]];
        pBits [29] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [28] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [27] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[10]];
        pBits [32] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [31] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [30] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[11]];
        pBits [35] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [34] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [33] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[12]];
        pBits [38] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [37] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [36] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[13]];
        pBits [41] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [40] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [39] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[14]];
        pBits [44] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [43] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [42] = rgiClapTab [(iYScale + iUtoB7)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[15]];
        pBits [47] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [46] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [45] = rgiClapTab [(iYScale + iUtoB7)]; // b

        pucCurrY += iWidthY;
        pBits += iBitmapWidth;
    }
}

Void_WMV WMVideoDecUpdateDstBlkRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV *pBits,
    const U8_WMV *pucCurrY, const U8_WMV *pucCurrU, const U8_WMV *pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U32_WMV iy;
    
    for (iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
        I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
        I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
        I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
        I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
        I32_WMV iYScale;

        iVtoR0 = g_iVtoR_WMV [pucCurrV [0]];
        iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU [0]] + g_iVtoG_WMV [pucCurrV [0]];
        iUtoB0 = g_iUtoB_WMV [pucCurrU [0]];

        iYScale = g_iYscale_WMV [pucCurrY [0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [5] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [4] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [3] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iVtoR1 = g_iVtoR_WMV [pucCurrV [1]];
        iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU [1]] + g_iVtoG_WMV[pucCurrV [1]];
        iUtoB1 = g_iUtoB_WMV [pucCurrU [1]];

        iYScale = g_iYscale_WMV[pucCurrY [2]];
        pBits [8] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [7] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [6] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV [pucCurrY [3]];
        pBits [11] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [10] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [9]  = rgiClapTab [(iYScale + iUtoB1)]; // b

        iVtoR2 = g_iVtoR_WMV [pucCurrV [2]];
        iUtoG2PlusVtoG2 = g_iUtoG_WMV [pucCurrU [2]] + g_iVtoG_WMV [pucCurrV [2]];
        iUtoB2 = g_iUtoB_WMV [pucCurrU [2]];

        iYScale = g_iYscale_WMV [pucCurrY [4]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [17] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [16] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [15] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iVtoR3 = g_iVtoR_WMV [pucCurrV [3]];
        iUtoG3PlusVtoG3 = g_iUtoG_WMV [pucCurrU [3]] + g_iVtoG_WMV [pucCurrV [3]];
        iUtoB3 = g_iUtoB_WMV [pucCurrU [3]];
        
        iYScale = g_iYscale_WMV [pucCurrY [6]];
        pBits [20] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [19] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [18] = rgiClapTab [(iYScale + iUtoB3)]; // b
        
        iYScale = g_iYscale_WMV[pucCurrY [7]];
        pBits [23] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [22] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [21] = rgiClapTab [(iYScale + iUtoB3)]; // b
        
        pucCurrY += iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [5] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [4] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [3] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [8] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [7] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [6] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [11] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [10] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [9]  = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [17] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [16] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [15] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [20] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [19] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [18] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [23] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [22] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [21] = rgiClapTab [(iYScale + iUtoB3)]; // b

        pucCurrY += iWidthY;
        pBits += iBitmapWidth;
    }
}
#endif	//define(OPT_DECOLORCONV_SH4)

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)

Void_WMV WMVideoDecUpdateDstPartialMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer
//
    U8_WMV  mb [MB_SIZE * 4 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBRGB32(
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 4);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 4);

        pBitsMb += MB_SIZE * 4;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U32_WMV iy;
    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
        I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
        I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
        I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
        I32_WMV iVtoR4, iUtoG4PlusVtoG4, iUtoB4;
        I32_WMV iVtoR5, iUtoG5PlusVtoG5, iUtoB5;
        I32_WMV iVtoR6, iUtoG6PlusVtoG6, iUtoB6;
        I32_WMV iVtoR7, iUtoG7PlusVtoG7, iUtoB7;
        I32_WMV iYScale;

        iVtoR0 = g_iVtoR_WMV [pucCurrV[0]];
        iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
        iUtoB0 = g_iUtoB_WMV [pucCurrU[0]];
        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [6] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [5] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [4] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
        iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU[1]] + g_iVtoG_WMV[pucCurrV[1]];
        iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];
        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [10] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [9] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [8] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
        iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
        iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [18] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [17] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [16] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [22] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [21] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [20] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
        iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
        iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [30] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [29] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [28] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iVtoR4 = g_iVtoR_WMV[pucCurrV[4]];
        iUtoG4PlusVtoG4 = g_iUtoG_WMV[pucCurrU[4]] + g_iVtoG_WMV[pucCurrV[4]];
        iUtoB4 = g_iUtoB_WMV[pucCurrU[4]];
        iYScale = g_iYscale_WMV[pucCurrY[8]];
        pBits [34] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [33] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [32] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[9]];
        pBits [38] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [37] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [36] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iVtoR5 = g_iVtoR_WMV[pucCurrV[5]];
        iUtoG5PlusVtoG5 = g_iUtoG_WMV[pucCurrU[5]] + g_iVtoG_WMV[pucCurrV[5]];
        iUtoB5 = g_iUtoB_WMV[pucCurrU[5]];
        iYScale = g_iYscale_WMV[pucCurrY[10]];
        pBits [42] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [41] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [40] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[11]];
        pBits [46] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [45] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [44] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iVtoR6 = g_iVtoR_WMV[pucCurrV[6]];
        iUtoG6PlusVtoG6 = g_iUtoG_WMV[pucCurrU[6]] + g_iVtoG_WMV[pucCurrV[6]];
        iUtoB6 = g_iUtoB_WMV[pucCurrU[6]];
        iYScale = g_iYscale_WMV[pucCurrY[12]];
        pBits [50] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [49] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [48] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[13]];
        pBits [54] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [53] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [52] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iVtoR7 = g_iVtoR_WMV[pucCurrV[7]];
        iUtoG7PlusVtoG7 = g_iUtoG_WMV[pucCurrU[7]] + g_iVtoG_WMV[pucCurrV[7]];
        iUtoB7 = g_iUtoB_WMV[pucCurrU[7]];
        iYScale = g_iYscale_WMV[pucCurrY[14]];
        pBits [58] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [57] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [56] = rgiClapTab [(iYScale + iUtoB7)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[15]];
        pBits [62] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [61] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [60] = rgiClapTab [(iYScale + iUtoB7)]; // b

        pucCurrY += iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [6] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [5] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [4] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [10] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [9] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [8] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [18] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [17] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [16] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [22] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [21] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [20] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [30] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [29] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [28] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[8]];
        pBits [34] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [33] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [32] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[9]];
        pBits [38] = rgiClapTab [(iYScale + iVtoR4)]; // r
        pBits [37] = rgiClapTab [(iYScale - iUtoG4PlusVtoG4)]; // g
        pBits [36] = rgiClapTab [(iYScale + iUtoB4)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[10]];
        pBits [42] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [41] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [40] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[11]];
        pBits [46] = rgiClapTab [(iYScale + iVtoR5)]; // r
        pBits [45] = rgiClapTab [(iYScale - iUtoG5PlusVtoG5)]; // g
        pBits [44] = rgiClapTab [(iYScale + iUtoB5)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[12]];
        pBits [50] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [49] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [48] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[13]];
        pBits [54] = rgiClapTab [(iYScale + iVtoR6)]; // r
        pBits [53] = rgiClapTab [(iYScale - iUtoG6PlusVtoG6)]; // g
        pBits [52] = rgiClapTab [(iYScale + iUtoB6)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[14]];
        pBits [58] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [57] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [56] = rgiClapTab [(iYScale + iUtoB7)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[15]];
        pBits [62] = rgiClapTab [(iYScale + iVtoR7)]; // r
        pBits [61] = rgiClapTab [(iYScale - iUtoG7PlusVtoG7)]; // g
        pBits [60] = rgiClapTab [(iYScale + iUtoB7)]; // b

        pucCurrY += iWidthY;

        pBits += iBitmapWidth;
    }
}
  
Void_WMV WMVideoDecUpdateDstBlkRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U32_WMV iy;
    for (iy = 0; iy < SUB_BLOCK_SIZE; iy++) {
        I32_WMV iVtoR0, iUtoG0PlusVtoG0, iUtoB0;
        I32_WMV iVtoR1, iUtoG1PlusVtoG1, iUtoB1;
        I32_WMV iVtoR2, iUtoG2PlusVtoG2, iUtoB2;
        I32_WMV iVtoR3, iUtoG3PlusVtoG3, iUtoB3;
        I32_WMV iYScale;

        iVtoR0 = g_iVtoR_WMV [pucCurrV[0]];
        iUtoG0PlusVtoG0 = g_iUtoG_WMV [pucCurrU[0]] + g_iVtoG_WMV [pucCurrV[0]];
        iUtoB0 = g_iUtoB_WMV [pucCurrU[0]];
        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [6] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [5] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [4] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iVtoR1 = g_iVtoR_WMV[pucCurrV[1]];
        iUtoG1PlusVtoG1 = g_iUtoG_WMV[pucCurrU[1]] + g_iVtoG_WMV[pucCurrV[1]];
        iUtoB1 = g_iUtoB_WMV[pucCurrU[1]];
        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [10] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [9] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [8] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iVtoR2 = g_iVtoR_WMV[pucCurrV[2]];
        iUtoG2PlusVtoG2 = g_iUtoG_WMV[pucCurrU[2]] + g_iVtoG_WMV[pucCurrV[2]];
        iUtoB2 = g_iUtoB_WMV[pucCurrU[2]];
        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [18] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [17] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [16] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [22] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [21] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [20] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iVtoR3 = g_iVtoR_WMV[pucCurrV[3]];
        iUtoG3PlusVtoG3 = g_iUtoG_WMV[pucCurrU[3]] + g_iVtoG_WMV[pucCurrV[3]];
        iUtoB3 = g_iUtoB_WMV[pucCurrU[3]];
        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [30] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [29] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [28] = rgiClapTab [(iYScale + iUtoB3)]; // b

        pucCurrY += iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;

        pBits += iBitmapWidth;

        iYScale = g_iYscale_WMV[pucCurrY[0]];
        pBits [2] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [1] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [0] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[1]];
        pBits [6] = rgiClapTab [(iYScale + iVtoR0)]; // r
        pBits [5] = rgiClapTab [(iYScale - iUtoG0PlusVtoG0)]; // g
        pBits [4] = rgiClapTab [(iYScale + iUtoB0)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[2]];
        pBits [10] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [9] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [8] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[3]];
        pBits [14] = rgiClapTab [(iYScale + iVtoR1)]; // r
        pBits [13] = rgiClapTab [(iYScale - iUtoG1PlusVtoG1)]; // g
        pBits [12] = rgiClapTab [(iYScale + iUtoB1)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[4]];
        pBits [18] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [17] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [16] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[5]];
        pBits [22] = rgiClapTab [(iYScale + iVtoR2)]; // r
        pBits [21] = rgiClapTab [(iYScale - iUtoG2PlusVtoG2)]; // g
        pBits [20] = rgiClapTab [(iYScale + iUtoB2)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[6]];
        pBits [26] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [25] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [24] = rgiClapTab [(iYScale + iUtoB3)]; // b

        iYScale = g_iYscale_WMV[pucCurrY[7]];
        pBits [30] = rgiClapTab [(iYScale + iVtoR3)]; // r
        pBits [29] = rgiClapTab [(iYScale - iUtoG3PlusVtoG3)]; // g
        pBits [28] = rgiClapTab [(iYScale + iUtoB3)]; // b

        pucCurrY += iWidthY;

        pBits += iBitmapWidth;
    }
}

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)

Void_WMV WMVideoDecUpdateDstPartialMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer.  YUY2 is packed as follows:
//
// Y0 U0 Y1 V0    Y2 U2 Y3 V2    Y4 U4 Y5 V4
//
// Pixels 0,1     Pixels 2,3     Pixels 4,5
//
// Width must be a multiple of two.  Two pixels is four bytes, so effectively, 
// a pixel is two bytes.  If in some wierd situation we're given a bitmap
// into which we have to write an odd number of pixels, we'll just leave the
// last pixel undone.
//
    U8_WMV  mb[MB_SIZE * 2 * MB_SIZE + 8];
    U8_WMV *pBitsMb = mb;
    I32_WMV i;

    iRgbWidth &= ~1;
//
// Now color-space convert the macroblock into the buffer.
//
    pBitsMb = (U8_WMV*) (((ULONG_PTR) pBitsMb + 7) & ~7);
    WMVideoDecUpdateDstMBYUY2 (
        pWMVDec,
        pBitsMb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);

//
// And, copy the valid pixels from the buffer into the bitmap.
//
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    #define MMX_YUY2_SHUFFLING
    #ifdef MMX_YUY2_SHUFFLING

    #define ASSERT_ALIGN8(x) assert(((ULONG_PTR) (x) & 7) == 0)
    ASSERT_ALIGN8(pBits);
    ASSERT_ALIGN8(pucCurrY);
    ASSERT_ALIGN8(pucCurrU);
    ASSERT_ALIGN8(pucCurrV);
    ASSERT_ALIGN8(iBitmapWidth);
    ASSERT_ALIGN8(iWidthY);
    ASSERT_ALIGN8(iWidthUV);

    __asm {
        mov         esi, pucCurrY
        mov         edi, pBits
        mov         ecx, pucCurrU
        mov         edx, pucCurrV
        mov         eax, BLOCK_SIZE

    L1: movq        mm2, [ecx]          ; 8 U values
        movq        mm3, [edx]          ; 8 V values
        pxor        mm4, mm4
        pxor        mm6, mm6
        pxor        mm0, mm0
        pxor        mm1, mm1
        punpcklbw   mm4, mm2
        punpckhbw   mm6, mm2
        movq        mm5, mm4
        movq        mm7, mm6
        punpckhwd   mm5, mm0            ; spread Us into 4 MMX registers
        punpckhwd   mm7, mm0            ;   ..u...u...u...u...u...u...u...u.
        punpcklwd   mm4, mm0
        punpcklwd   mm6, mm0

        pxor        mm2, mm2
        add         ecx, iWidthUV
        add         edx, iWidthUV

        punpcklbw   mm0, mm3            ; spread Vs and then OR them into Us 
        punpcklwd   mm1, mm0            ;   v.u.v.u.v.u.v.u.v.u.v.u.v.u.v.u.
        punpckhwd   mm2, mm0
        por         mm4, mm1
        por         mm5, mm2

        movq        mm0, [esi]          ; 16 Y values from the current line
        movq        mm1, [esi+8]

        pxor        mm2, mm2
        punpckhbw   mm2, mm3
        pxor        mm3, mm3
        punpcklwd   mm3, mm2
        por         mm6, mm3
        pxor        mm3, mm3
        punpckhwd   mm3, mm2
        por         mm7, mm3

        add         esi, iWidthY        ; spread Ys and output the final results
        pxor        mm2, mm2            ;   vyuyvyuyvyuyvyuyvyuyvyuyvyuyvyuy
        movq        mm3, mm0
        punpcklbw   mm0, mm2
        por         mm0, mm4
        movq        [edi], mm0          ; notice that we output 32 bytes (4 qwords)
        punpckhbw   mm3, mm2            ; without any intervening memory access
        por         mm3, mm5            ; to achieve maximum memory write perf
        movq        [edi+8], mm3
        movq        mm3, mm1
        punpcklbw   mm1, mm2
        por         mm1, mm6
        movq        [edi+16], mm1
        punpckhbw   mm3, mm2
        por         mm3, mm7
        movq        [edi+24], mm3

        movq        mm0, [esi]          ; 16 Y values from the next line
        movq        mm1, [esi+8]
        add         edi, iBitmapWidth
        pxor        mm2, mm2
        movq        mm3, mm0
        punpcklbw   mm0, mm2
        por         mm0, mm4
        movq        [edi], mm0
        punpckhbw   mm3, mm2
        por         mm3, mm5
        movq        [edi+8], mm3
        movq        mm3, mm1
        punpcklbw   mm1, mm2
        por         mm1, mm6
        movq        [edi+16], mm1
        punpckhbw   mm3, mm2
        por         mm3, mm7
        movq        [edi+24], mm3

        add         esi, iWidthY
        add         edi, iBitmapWidth
        dec         eax
        jnz         L1
        emms
    }

    #else // !MMX_YUY2_SHUFFLING

    I32_WMV i;
    for (i = 0; i < BLOCK_SIZE; i++) {        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   (((I32_WMV)pucCurrU [0])<<8) | (((I32_WMV)pucCurrV [0])<<24);
        (*(I32_WMV *)pBits) =               (ipxliUV | ((I32_WMV)pucCurrY [0]) | (((I32_WMV)pucCurrY [1])<<16));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [0]) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [1])<<8) | (((I32_WMV)pucCurrV [1])<<24);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | ((I32_WMV)pucCurrY [2]) | (((I32_WMV)pucCurrY [3])<<16));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [2]) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [2])<<8) | (((I32_WMV)pucCurrV [2])<<24);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | ((I32_WMV)pucCurrY [4]) | (((I32_WMV)pucCurrY [5])<<16));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [4]) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [3])<<8) | (((I32_WMV)pucCurrV [3])<<24);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | ((I32_WMV)pucCurrY [6]) | (((I32_WMV)pucCurrY [7])<<16));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [6]) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [4])<<8) | (((I32_WMV)pucCurrV [4])<<24);
        (*(I32_WMV *)(pBits+16)) =          (ipxliUV | ((I32_WMV)pucCurrY [8]) | (((I32_WMV)pucCurrY [9])<<16));
        (*(I32_WMV *)(pBitsNextLine+16)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [8]) | (((I32_WMV)ppxliCurrQYMBNextLine [9])<<16));
        
        ipxliUV =                       (((I32_WMV)pucCurrU [5])<<8) | (((I32_WMV)pucCurrV [5])<<24);
        (*(I32_WMV *)(pBits+20)) =          (ipxliUV | ((I32_WMV)pucCurrY [10]) | (((I32_WMV)pucCurrY [11])<<16));
        (*(I32_WMV *)(pBitsNextLine+20)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [10]) | (((I32_WMV)ppxliCurrQYMBNextLine [11])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [6])<<8) | (((I32_WMV)pucCurrV [6])<<24);
        (*(I32_WMV *)(pBits+24)) =          (ipxliUV | ((I32_WMV)pucCurrY [12]) | (((I32_WMV)pucCurrY [13])<<16));
        (*(I32_WMV *)(pBitsNextLine+24)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [12]) | (((I32_WMV)ppxliCurrQYMBNextLine [13])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [7])<<8) | (((I32_WMV)pucCurrV [7])<<24);
        (*(I32_WMV *)(pBits+28)) =          (ipxliUV | ((I32_WMV)pucCurrY [14]) | (((I32_WMV)pucCurrY [15])<<16));
        (*(I32_WMV *)(pBitsNextLine+28)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [14]) | (((I32_WMV)ppxliCurrQYMBNextLine [15])<<16));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }

    #endif // !MMX_YUY2_SHUFFLING
}

Void_WMV WMVideoDecUpdateDstBlkYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV i;
    for (i = 0; i < SUB_BLOCK_SIZE; i++) {
        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   (((I32_WMV)pucCurrU [0])<<8) | (((I32_WMV)pucCurrV [0])<<24);
        (*(I32_WMV *)pBits) =               (ipxliUV | ((I32_WMV)pucCurrY [0]) | (((I32_WMV)pucCurrY [1])<<16));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [0]) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [1])<<8) | (((I32_WMV)pucCurrV [1])<<24);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | ((I32_WMV)pucCurrY [2]) | (((I32_WMV)pucCurrY [3])<<16));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [2]) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [2])<<8) | (((I32_WMV)pucCurrV [2])<<24);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | ((I32_WMV)pucCurrY [4]) | (((I32_WMV)pucCurrY [5])<<16));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [4]) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<16));

        ipxliUV =                       (((I32_WMV)pucCurrU [3])<<8) | (((I32_WMV)pucCurrV [3])<<24);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | ((I32_WMV)pucCurrY [6]) | (((I32_WMV)pucCurrY [7])<<16));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [6]) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<16));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }
}

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

Void_WMV WMVideoDecUpdateDstPartialMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer.  UYVY is packed as follows:
//
// U0 Y0 V0 Y1    U2 Y2 V2 Y3    U4 Y4 V4 Y5
//
// Pixels 0,1     Pixels 2,3     Pixels 4,5
//
// Width must be a multiple of two.  Two pixels is four bytes, so effectively, 
// a pixel is two bytes.  If in some wierd situation we're given a bitmap
// into which we have to write an odd number of pixels, we'll just leave the
// last pixel undone.
//

    U8_WMV mb[MB_SIZE * 2 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;

    iRgbWidth &= ~1;
//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBUYVY(
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV i;
    for (i = 0; i < BLOCK_SIZE; i++) {
        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   ((I32_WMV)pucCurrU [0]) | (((I32_WMV)pucCurrV [0])<<16);
        (*(I32_WMV *)pBits) =               (ipxliUV | (((I32_WMV)pucCurrY [0])<<8) | (((I32_WMV)pucCurrY [1])<<24));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [0])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [1]) | (((I32_WMV)pucCurrV [1])<<16);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | (((I32_WMV)pucCurrY [2])<<8) | (((I32_WMV)pucCurrY [3])<<24));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [2])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [2]) | (((I32_WMV)pucCurrV [2])<<16);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | (((I32_WMV)pucCurrY [4])<<8) | (((I32_WMV)pucCurrY [5])<<24));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [4])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [3]) | (((I32_WMV)pucCurrV [3])<<16);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | (((I32_WMV)pucCurrY [6])<<8) | (((I32_WMV)pucCurrY [7])<<24));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [6])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [4]) | (((I32_WMV)pucCurrV [4])<<16);
        (*(I32_WMV *)(pBits+16)) =          (ipxliUV | (((I32_WMV)pucCurrY [8])<<8) | (((I32_WMV)pucCurrY [9])<<24));
        (*(I32_WMV *)(pBitsNextLine+16)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [8])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [9])<<24));
        
        ipxliUV =                       ((I32_WMV)pucCurrU [5]) | (((I32_WMV)pucCurrV [5])<<16);
        (*(I32_WMV *)(pBits+20)) =          (ipxliUV | (((I32_WMV)pucCurrY [10])<<8) | (((I32_WMV)pucCurrY [11])<<24));
        (*(I32_WMV *)(pBitsNextLine+20)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [10])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [11])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [6]) | (((I32_WMV)pucCurrV [6])<<16);
        (*(I32_WMV *)(pBits+24)) =          (ipxliUV | (((I32_WMV)pucCurrY [12])<<8) | (((I32_WMV)pucCurrY [13])<<24));
        (*(I32_WMV *)(pBitsNextLine+24)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [12])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [13])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [7]) | (((I32_WMV)pucCurrV [7])<<16);
        (*(I32_WMV *)(pBits+28)) =          (ipxliUV | (((I32_WMV)pucCurrY [14])<<8) | (((I32_WMV)pucCurrY [15])<<24));
        (*(I32_WMV *)(pBitsNextLine+28)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [14])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [15])<<24));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }
}

Void_WMV WMVideoDecUpdateDstBlkUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV i;
    for (i = 0; i < SUB_BLOCK_SIZE; i++) {
        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   ((I32_WMV)pucCurrU [0]) | (((I32_WMV)pucCurrV [0])<<16);
        (*(I32_WMV *)pBits) =               (ipxliUV | (((I32_WMV)pucCurrY [0])<<8) | (((I32_WMV)pucCurrY [1])<<24));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [0])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [1]) | (((I32_WMV)pucCurrV [1])<<16);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | (((I32_WMV)pucCurrY [2])<<8) | (((I32_WMV)pucCurrY [3])<<24));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [2])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [2]) | (((I32_WMV)pucCurrV [2])<<16);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | (((I32_WMV)pucCurrY [4])<<8) | (((I32_WMV)pucCurrY [5])<<24));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [4])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<24));

        ipxliUV =                       ((I32_WMV)pucCurrU [3]) | (((I32_WMV)pucCurrV [3])<<16);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | (((I32_WMV)pucCurrY [6])<<8) | (((I32_WMV)pucCurrY [7])<<24));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | (((I32_WMV)ppxliCurrQYMBNextLine [6])<<8) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<24));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }
}

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)


#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)

Void_WMV WMVideoDecUpdateDstPartialMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
) {
//
// Temporary macroblock-sized buffer.  YVYU is packed as follows:
//
// Y0 V0 Y1 U0    Y2 V2 Y3 U2    Y4 V4 Y5 U4 
//
// Pixels 0,1     Pixels 2,3     Pixels 4,5
//
// Width must be a multiple of two.  Two pixels is four bytes, so effectively, 
// a pixel is two bytes.  If in some wierd situation we're given a bitmap
// into which we have to write an odd number of pixels, we'll just leave the
// last pixel undone.
//
    U8_WMV  mb[MB_SIZE * 2 * MB_SIZE];
    U8_WMV *pBitsMb;
    I32_WMV i;

    iRgbWidth &= ~1;

//
// Now color-space convert the macroblock into the buffer.
//
    WMVideoDecUpdateDstMBYVYU(
        pWMVDec,
        (U8_WMV *)mb,
        pucCurrY,
        pucCurrU,
        pucCurrV,
        iWidthY,
        iWidthUV,
        MB_SIZE * 2);
//
// And, copy the valid pixels from the buffer into the bitmap.
//
    pBitsMb = mb;
    for (i = 0; i < iRgbHeight; ++i) {
        memcpy(pBits, pBitsMb, iRgbWidth * 2);

        pBitsMb += MB_SIZE * 2;
        pBits += pWMVDec->m_iWidthBMP;
    }
}

Void_WMV WMVideoDecUpdateDstMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV i;
    for (i = 0; i < BLOCK_SIZE; i++) {
        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   ((I32_WMV)pucCurrU [0]<<24) | (((I32_WMV)pucCurrV [0])<<8);
        (*(I32_WMV *)pBits) =               (ipxliUV | ((I32_WMV)pucCurrY [0]) | (((I32_WMV)pucCurrY [1])<<16));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [0]) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [1]<<24) | (((I32_WMV)pucCurrV [1])<<8);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | ((I32_WMV)pucCurrY [2]) | (((I32_WMV)pucCurrY [3])<<16));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [2]) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [2]<<24) | (((I32_WMV)pucCurrV [2])<<8);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | ((I32_WMV)pucCurrY [4]) | (((I32_WMV)pucCurrY [5])<<16));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [4]) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [3]<<24) | (((I32_WMV)pucCurrV [3])<<8);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | ((I32_WMV)pucCurrY [6]) | (((I32_WMV)pucCurrY [7])<<16));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [6]) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [4]<<24) | (((I32_WMV)pucCurrV [4])<<8);
        (*(I32_WMV *)(pBits+16)) =          (ipxliUV | ((I32_WMV)pucCurrY [8]) | (((I32_WMV)pucCurrY [9])<<16));
        (*(I32_WMV *)(pBitsNextLine+16)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [8]) | (((I32_WMV)ppxliCurrQYMBNextLine [9])<<16));
        
        ipxliUV =                       ((I32_WMV)pucCurrU [5]<<24) | (((I32_WMV)pucCurrV [5])<<8);
        (*(I32_WMV *)(pBits+20)) =          (ipxliUV | ((I32_WMV)pucCurrY [10]) | (((I32_WMV)pucCurrY [11])<<16));
        (*(I32_WMV *)(pBitsNextLine+20)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [10]) | (((I32_WMV)ppxliCurrQYMBNextLine [11])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [6]<<24) | (((I32_WMV)pucCurrV [6])<<8);
        (*(I32_WMV *)(pBits+24)) =          (ipxliUV | ((I32_WMV)pucCurrY [12]) | (((I32_WMV)pucCurrY [13])<<16));
        (*(I32_WMV *)(pBitsNextLine+24)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [12]) | (((I32_WMV)ppxliCurrQYMBNextLine [13])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [7]<<24) | (((I32_WMV)pucCurrV [7])<<8);
        (*(I32_WMV *)(pBits+28)) =          (ipxliUV | ((I32_WMV)pucCurrY [14]) | (((I32_WMV)pucCurrY [15])<<16));
        (*(I32_WMV *)(pBitsNextLine+28)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [14]) | (((I32_WMV)ppxliCurrQYMBNextLine [15])<<16));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }
}

Void_WMV WMVideoDecUpdateDstBlkYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
) {
    I32_WMV i;
    for (i = 0; i < SUB_BLOCK_SIZE; i++) {
        
        U8_WMV *pBitsNextLine = pBits + iBitmapWidth;
        const U8_WMV *ppxliCurrQYMBNextLine = pucCurrY + iWidthY;

        I32_WMV ipxliUV =                   ((I32_WMV)pucCurrU [0]<<24) | (((I32_WMV)pucCurrV [0])<<8);
        (*(I32_WMV *)pBits) =               (ipxliUV | ((I32_WMV)pucCurrY [0]) | (((I32_WMV)pucCurrY [1])<<16));
        (*(I32_WMV *)pBitsNextLine) =       (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [0]) | (((I32_WMV)ppxliCurrQYMBNextLine [1])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [1]<<24) | (((I32_WMV)pucCurrV [1])<<8);
        (*(I32_WMV *)(pBits+4)) =           (ipxliUV | ((I32_WMV)pucCurrY [2]) | (((I32_WMV)pucCurrY [3])<<16));
        (*(I32_WMV *)(pBitsNextLine+4)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [2]) | (((I32_WMV)ppxliCurrQYMBNextLine [3])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [2]<<24) | (((I32_WMV)pucCurrV [2])<<8);
        (*(I32_WMV *)(pBits+8)) =           (ipxliUV | ((I32_WMV)pucCurrY [4]) | (((I32_WMV)pucCurrY [5])<<16));
        (*(I32_WMV *)(pBitsNextLine+8)) =   (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [4]) | (((I32_WMV)ppxliCurrQYMBNextLine [5])<<16));

        ipxliUV =                       ((I32_WMV)pucCurrU [3]<<24) | (((I32_WMV)pucCurrV [3])<<8);
        (*(I32_WMV *)(pBits+12)) =          (ipxliUV | ((I32_WMV)pucCurrY [6]) | (((I32_WMV)pucCurrY [7])<<16));
        (*(I32_WMV *)(pBitsNextLine+12)) =  (ipxliUV | ((I32_WMV)ppxliCurrQYMBNextLine [6]) | (((I32_WMV)ppxliCurrQYMBNextLine [7])<<16));

        pBits = pBitsNextLine + iBitmapWidth;
        pucCurrY = ppxliCurrQYMBNextLine + iWidthY;
        pucCurrU += iWidthUV;
        pucCurrV += iWidthUV;
    }
}

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)
