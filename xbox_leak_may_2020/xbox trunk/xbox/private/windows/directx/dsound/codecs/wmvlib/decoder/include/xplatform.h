#ifndef __XPLATFORM__
#define __XPLATFORM__

#ifdef __MACVIDEO__
#include "MacPort.h"
#define NO_WINDOWS
#endif

#ifdef _XBOX
#include <xtl.h>
#endif

//**************************************************************************************************
#ifdef _Embedded_x86
#   define NO_WINDOWS
#   undef _USE_INTEL_COMPILER
    typedef unsigned long       DWORD;
    typedef unsigned short      WORD;
    typedef long LONG;
    typedef unsigned char BYTE;
    typedef void *HANDLE; 
    typedef void *LPVOID; 
    typedef unsigned int UINT; // IW
#ifndef _XBOX
    typedef long HDC; // IW
    typedef BYTE BOOL; //IW
    typedef void VOID;
#endif
    // IW #define I32 long
    //#define Bool int
#   define NULL    0//((void *)0)
#   define NOERROR             0
#   define TRUE	1
#   define FALSE	0
#   define AVIIF_KEYFRAME  0x00000010L

#   undef WMAPI
#   define WMAPI
#   undef _stdcall
#   define _stdcall

#   define CONST const
#endif

#if defined(_Embedded_x86) || defined(_XBOX)
#   define BI_BITFIELDS     3L
#   define BI_RGB           0

#   ifndef _HRESULT_DEFINED
#       define _HRESULT_DEFINED
        typedef LONG HRESULT;
#   endif // !_HRESULT_DEFINED

#   define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#   define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#   define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#   define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#   define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#   define MoveMemory RtlMoveMemory
#   define CopyMemory RtlCopyMemory
#   define FillMemory RtlFillMemory
#   define ZeroMemory RtlZeroMemory

#   define min(a,b)    (((a) < (b)) ? (a) : (b))

#   ifndef _BITMAPINFOHEADER
#       define _BITMAPINFOHEADER
        typedef struct tagBITMAPINFOHEADER{
            DWORD      biSize;
            LONG       biWidth;
            LONG       biHeight;
            WORD       biPlanes;
            WORD       biBitCount;
            DWORD      biCompression;
            DWORD      biSizeImage;
            LONG       biXPelsPerMeter;
            LONG       biYPelsPerMeter;
            DWORD      biClrUsed;
            DWORD      biClrImportant;
        } BITMAPINFOHEADER, *LPBITMAPINFOHEADER;
#   endif

#   ifndef _BITMAPINFO
#       define _BITMAPINFO
        /*
        typedef struct tagRGBQUAD {
            BYTE    rgbBlue; 
            BYTE    rgbGreen; 
            BYTE    rgbRed; 
            BYTE    rgbReserved; 
        } RGBQUAD; 
        typedef struct tagBITMAPINFO {
            BITMAPINFOHEADER    bmiHeader;
            RGBQUAD             bmiColors[1];
        } BITMAPINFO;
        */
#   endif

    //IW check MAKEFOURCC
#ifndef _XBOX
#   define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
		((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
		((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif
#   ifndef mmioFOURCC
#       define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)
#   endif
#ifndef _XBOX
#   ifndef _WAVEFORMATEX_
#       define _WAVEFORMATEX_
        typedef struct tWAVEFORMATEX
        {
            WORD    wFormatTag;         /* format type */
            WORD    nChannels;          /* number of channels (i.e. mono, stereo...) */
            DWORD   nSamplesPerSec;     /* sample rate */
            DWORD   nAvgBytesPerSec;    /* for buffer estimation */
            WORD    nBlockAlign;        /* block size of data */
            WORD    wBitsPerSample;     /* number of bits per sample of mono data */
            WORD    cbSize;             /* the count in bytes of the size of */
        }   WAVEFORMATEX;
#   endif

#endif // xbox
#endif // EMbedded_x86 || xbox

//**************************************************************************************************
#if defined(__arm)
    // Embedded ARM
#   define _CE_WMV2_
#   define __huge
#   define NO_WINDOWS
    typedef void* LPVOID;
    typedef void* HANDLE;
    typedef long long LARGE_INTEGER;
    typedef char TCHAR;
    typedef char* LPCTSTR;
#   define _T(a) a
#   define TEXT(a) a
#   define _stprintf sprintf
#   define UINT_MAX  0xffffffffU
    typedef unsigned long DWORD;
#endif

//**************************************************************************************************
#if defined(UNDER_CE) && !defined(macintosh) // only works with little-endian for now, so no PowerPC
#   define _EMB_WMV2_
#   define __STREAMING_MODE_DECODE_
#   define _SLIM_C_                 // no multithread
#   define OUTPUT_ALL_FORMATS       // all color conversion
#endif



#ifdef _EMB_WMV2_                     //  conditional defines for CE devices
#   define COMBINE_LOOPFILTER_RENDERING
#   define DYNAMIC_EDGEPAD          // Verify: setting of motioncomp_arm.s -PD "DYNAMIC_EDGEPAD SETA 1", repeatpad_arm.s as -PD"EXPANDY_REFVOP SETA 8"
#   define DYNAMIC_EDGEPAD_0
#   define OPT_HUFFMAN_GET_WMV          // from wmvideo optimization - OK for all the platforms
#   define OPT_SMALL_CODE_SIZE      // without this, some loops get unrolled
    ////#    define OPT_TINY_CODE_SIZE       // trade-off for smaller code size with added complexity within loops.
#   if defined(OPT_TINY_CODE_SIZE) && !defined(OPT_SMALL_CODE_SIZE)
#       define OPT_SMALL_CODE_SIZE
#   endif




#   if defined(_ARM_) || defined(__arm) 
#       define _EMB_ASM_ARM_STRMHUFF_
#       define IPAQ_HACK
#       define _ARM_ASM_MOTIONCOMP_OPT_
#       define _ARM_ASM_LOOPFILTER_OPT_
#       define _ARM_ASM_BLKDEC_OPT_
#       define IDCTDEC_32BIT_INPUT      // To enable using ARM multiple load/stores
#       define OPT_IDCT_ARM
#       define OPT_DECOLORCONV_ARM
#       ifdef OPT_DECOLORCONV_ARM
#           define OUTPUT_RGB12
#       endif
//#       define IPP_COLORCONV
#       define OPT_ZEROMOTION_ARM
#       define OPT_REPEATPAD_ARM
#       define OPT_X8_ARM
#       if defined(DYNAMIC_EDGEPAD)
#           define _ARM_ASM_DYNAMICPAD_OPT_
#       endif
      // define STINGER in Stinger specific project files
#       if defined(STINGER)
#           define _WMVONLY_
#           ifdef OUTPUT_ALL_FORMATS
#               undef OUTPUT_ALL_FORMATS
#           endif
#           ifndef OUTPUT_RGB12
#               define OUTPUT_RGB12
#           endif
#           define STATIC_PAD_0
#       endif
      // ARM7 should define OPT_SLOW_MULTIPLY
#   endif
#   if defined(__arm)
//#       define CE_DECODEPMB_GLOBAL_FUNCTABLE    // otherwise C++ errors in vopdec_ce.cpp
#   endif

#   ifdef _SH3_
//#       define USE_SH3_ASM
#   endif


#	ifdef _SH4_
#       define _SH4_ASM_LOOPFILTER_OPT_
#       define OPT_DECOLORCONV_SH4
#	endif


#   ifdef _MIPS_
#       define _MIPS_ASM_MOTIONCOMP_OPT_
#       define _MIPS_ASM_LOOPFILTER_OPT_
#       define _6BIT_COLORCONV_OPT_
#       define _MIPS_ASM_IDCTDEC_OPT_
#       define _MIPS_ASM_X8_OPT_
#   endif
#endif //_EMB_WMV2_


//**************************************************************************************************
// memcpy of aligned U32_WMV with cbSiz = number of bytes to copy
// assumes pDst and pSrc are aligned, but casts them in case they are actually byte or short pointers.
#define ALIGNED32_MEMCPY(pDst,pSrc,cbSiz) {                 \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            for(int j=(cbSiz)>>2; j>0;j--)                  \
                *pD++ = *pS++;                              \
        }
#define ALIGNED32_MEMCPY16(pDst,pSrc) {                     \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            *pD++ = *pS++;                                  \
            *pD++ = *pS++;                                  \
            *pD++ = *pS++;                                  \
            *pD   = *pS;                                    \
        }
#define ALIGNED32_MEMCPY8(pDst,pSrc) {                      \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            *pD++ = *pS++;                                  \
            *pD   = *pS;                                    \
        }
// memset of aligned U32_WMV with cbSiz = number of bytes to copy
#define ALIGNED32_MEMSET_U32(pDst,u32C,cbSiz) {             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            for(int j=(cbSiz)>>2; j>0;j--)                  \
                *pD++ = u32C;                               \
        }
#define ALIGNED32_MEMSET_2U32(pDst,u32C) {                  \
            U32_WMV* pD = (U32_WMV*)(pDst);                         \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
// replicate a byte 4 times in an U32_WMV.
#ifndef OPT_SLOW_MULTIPLY
#   define ALIGNED32_REPLICATE_4U8(a) (((U8_WMV)(a)) * 0x01010101u)
#else
#   define ALIGNED32_REPLICATE_4U8(a) (((U8_WMV)(a)<<24) | ((U8_WMV)(a)<<16) | ((U8_WMV)(a)<<8) | (U8_WMV)(a))
#endif
// memset of aligned U32_WMV with a replicated U8_WMV constant
#define ALIGNED32_MEMSET_U8(pDst,u8C,cbSiz) {               \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            ALIGNED32_MEMSET_U32( (pDst), u32C, (cbSiz) );  \
        }
#define ALIGNED32_MEMSET_8U8(pDst,u8C) {                    \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#ifndef OPT_SMALL_CODE_SIZE
#   define ALIGNED32_MEMSET_20U8(pDst,u8C) {                \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#   define ALIGNED32_MEMSET_24U8(pDst,u8C) {                \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#   define ALIGNED32_MEMCLR_128U8(pDst) {                   \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
        }
#else // OPT_SMALL_CODE_SIZE
#   define ALIGNED32_MEMSET_20U8(pDst,u8C) ALIGNED32_MEMSET_U8((pDst),(u8C),20)
#   define ALIGNED32_MEMSET_24U8(pDst,u8C) ALIGNED32_MEMSET_U8((pDst),(u8C),24)
#   define ALIGNED32_MEMCLR_128U8(pDst)    ALIGNED32_MEMSET_U32((pDst),0,128)
#endif
// Average of 8 bytes
#define ALIGNED32_AVERAGE_8U8(avg,pSrc,Rnd) {                                                                           \
            const U32_WMV u32S0 = *((U32_WMV*)(pSrc)), u32S1 = *((U32_WMV*)(pSrc+4));                                               \
            avg = (u32S0 & 0x00ff00ff) + ((u32S0>>8) & 0x00ff00ff) + (u32S1 & 0x00ff00ff) + ((u32S1>>8) & 0x00ff00ff);  \
            avg = ((avg & 0x0000ffff) + ((avg>>16) & 0x0000ffff) + Rnd) >> 3;                                           \
        }
// Sum of 8 bytes (takes 13 ops + 2 reads)
#define ALIGNED32_SUM_8U8(sum,pSrc) {                                                                                   \
            const U32_WMV u32S0 = *((U32_WMV*)(pSrc)), u32S1 = *((U32_WMV*)(pSrc+4));                                               \
            sum = (u32S0 & 0x00ff00ff) + ((u32S0>>8) & 0x00ff00ff) + (u32S1 & 0x00ff00ff) + ((u32S1>>8) & 0x00ff00ff);  \
            sum = (sum & 0x0000ffff) + ((sum>>16) & 0x0000ffff);                                                        \
        }

//**************************************************************************************************
#ifdef PROFILE
#   include "AutoProfile.h"
#else
#   define FUNCTION_PROFILE(fp)
#   define FUNCTION_PROFILE_START(fp,id)
#   define FUNCTION_PROFILE_STOP(fp)
#   define FUNCTION_PROFILE_DECL_START(fp,id)
#   define FUNCTION_PROFILE_SWITCH(fp,id)
#endif

//**************************************************************************************************
#ifndef DEBUG_ONLY
#   if defined(DEBUG) || defined(_DEBUG)
#       define DEBUG_ONLY(a) a
#   else
#       define DEBUG_ONLY(a)
#   endif
#endif // DEBUG_ONLY
#ifndef DEBUG_CON_ONLY
#   if (defined(DEBUG) || defined(_DEBUG)) && !defined(UNDER_CE)
        // Some winCE platforms do not support printf.
#       define DEBUG_CON_ONLY(a) a
#   else
#       define DEBUG_CON_ONLY(a)
#   endif
#endif // DEBUG_CON_ONLY
#ifndef DEBUG_HEAP_ADD
#   if defined(WANT_HEAP_MEASURE)
#       define DEBUG_HEAP_ADD(c,a) { g_cbHeapSize[g_iHeapLastClass=c] += a;  if (g_cbHeapSize[c]>g_cbMaxHeapSize[c]) g_cbMaxHeapSize[c] = g_cbHeapSize[c]; }
#       define DEBUG_HEAP_SUB(c,a) { g_cbHeapSize[g_iHeapLastClass=c] -= a; }
#       ifdef __cplusplus
            extern "C"
#       else
            extern 
#       endif
        unsigned int g_cbHeapSize[6], g_cbMaxHeapSize[6], g_iHeapLastClass;
#       define DHEAP_STRUCT 0
#       define DHEAP_FRAMES 1
#       define DHEAP_HUFFMAN 2
#       define DHEAP_LOCALHUFF 3
#       define DHEAP_LOCAL_COUNT 4
#   else
#       define DEBUG_HEAP_ADD(c,a)
#       define DEBUG_HEAP_SUB(c,a)
#   endif
#endif // DEBUG_HEAP_MEASURE


#endif




