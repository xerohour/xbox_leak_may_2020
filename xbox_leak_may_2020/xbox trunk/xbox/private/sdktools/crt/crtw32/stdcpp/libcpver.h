#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <xboxverp.h>

#pragma data_seg(".XBLD$V")

#ifdef _MT
#if DBG
#pragma comment(linker, "/include:_LibcpBuildNumberMTD")
__declspec(selectany) unsigned short LibcpBuildNumberMTD[8] = { 'IL', 'CB', 'MP', 'DT',
    VER_PRODUCTVERSION | 0x8000
#else  // DBG
#pragma comment(linker, "/include:_LibcpBuildNumberMT")
__declspec(selectany) unsigned short LibcpBuildNumberMT[8] = { 'IL', 'CB', 'MP', 'T',
    VER_PRODUCTVERSION | 0x4000
#endif // DBG
#else  // _MT
#if DBG
#pragma comment(linker, "/include:_LibcpBuildNumberD")
__declspec(selectany) unsigned short LibcpBuildNumberD[8] = { 'IL', 'CB', 'DP', 0,
    VER_PRODUCTVERSION | 0x8000
#else  // DBG
#pragma comment(linker, "/include:_LibcpBuildNumber")
__declspec(selectany) unsigned short LibcpBuildNumber[8] = { 'IL', 'CB', 'P', 0,
    VER_PRODUCTVERSION | 0x4000
#endif // DBG
#endif // _MT
};

#pragma data_seg()

#ifdef __cplusplus
}
#endif // __cplusplus
