#include <xboxverp.h>

#pragma comment(linker, "/include:_XboxKrnlBuildNumber")

#pragma data_seg(".XBLD$A")
#ifdef XAPILIBP

#if DBG
#pragma comment(linker, "/include:_XapiBuildNumberPD")
__declspec(selectany) USHORT XapiBuildNumberPD[8] = { 'AX', 'IP', 'BL', 'DP',
    VER_PRODUCTVERSION | 0x8000
#else  // DBG
#pragma comment(linker, "/include:_XapiBuildNumberP")
__declspec(selectany) USHORT XapiBuildNumberP[8] = { 'AX', 'IP', 'BL', 'P',
    VER_PRODUCTVERSION
#endif // DBG

#else  // XAPILIBP

#if DBG
#pragma comment(linker, "/include:_XapiBuildNumberD")
__declspec(selectany) USHORT XapiBuildNumberD[8] = { 'AX', 'IP', 'IL', 'DB',
    VER_PRODUCTVERSION | 0x8000
#else  // DBG
#pragma comment(linker, "/include:_XapiBuildNumber")
__declspec(selectany) USHORT XapiBuildNumber[8] = { 'AX', 'IP', 'IL', 'B',
    VER_PRODUCTVERSION | 0x4000
#endif // DBG

#endif // XAPILIBP
};
#pragma comment(linker, "/SECTION:.XBLD,D")
#pragma data_seg()
