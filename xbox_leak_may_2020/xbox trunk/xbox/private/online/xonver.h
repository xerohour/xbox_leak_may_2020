// ---------------------------------------------------------------------------------------
// xonver.h
//
// Include file for stamping source files with the appropriate version structure
// ---------------------------------------------------------------------------------------

#ifdef  __XONVER_H__
#error "xonver.h should not be included more than once"
#endif
#define __XONVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboxverp.h>

#ifdef XONLINE_FEATURE_XBOX
#pragma data_seg(push)
#pragma data_seg(".XBLD$V")
#endif

#if defined(XONLINE_BUILD_LIBX) && DBG==0
    #define VERXON()    VERGEN(XOnlineBuildNumber,  'X','O','N','L','I','N','E',0)
#elif defined(XONLINE_BUILD_LIBX) 
    #define VERXON()    VERGEN(XOnlineBuildNumberD, 'X','O','N','L','I','N','E','D')
#elif defined(XONLINE_BUILD_LIBS) && DBG==0
    #define VERXON()    VERGEN(XOnlineBuildNumberS, 'X','O','N','L','I','N','E','S')
#elif defined(XONLINE_BUILD_LIBS)
    #define VERXON()    VERGEN(XOnlineBuildNumberSD,'X','O','N','L','I','N','S','D')
#elif defined(XONLINE_BUILD_LIBW) && DBG==0
    #define VERXON()    VERGEN(XOnlineBuildNumberW, 'X','O','N','L','I','N','E','W')
#elif defined(XONLINE_BUILD_LIBW)
    #define VERXON()    VERGEN(XOnlineBuildNumberWD,'X','O','N','L','I','N','W','D')
#else
    #error "Don't know how to make VERXON"
#endif

#if DBG
#define VERDBG  0x8000
#else
#define VERDBG  0x0000
#endif

#if DBG==0 && defined(XONLINE_BUILD_LIBS)
#define VERSEC  0x0000
#else
#define VERSEC  0x0000
#endif

#define VERGEN(var,a1,a2,a3,a4,a5,a6,a7,a8) comment(linker, "/include:_" # var)
#pragma VERXON()
#undef  VERGEN
#define VERGEN(var,a1,a2,a3,a4,a5,a6,a7,a8) var[8] = { (a1) | ((a2) << 8), (a3) | ((a4) << 8), (a5) | ((a6) << 8), (a7) | ((a8) << 8), VER_PRODUCTVERSION | VERDBG | VERSEC }
__declspec(selectany) unsigned short VERXON();
#undef  VERGEN
#undef  VERDBG
#undef  VERSEC
#undef  VERXON

#ifdef XONLINE_FEATURE_XBOX
#pragma data_seg(pop)
#endif

#ifdef __cplusplus
};
#endif

