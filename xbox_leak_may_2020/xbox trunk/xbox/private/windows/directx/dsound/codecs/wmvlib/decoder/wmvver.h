#ifndef __WMVVER_H__
#define __WMVVER_H__

#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_WMVDecBuildNumberD")
__declspec(selectany) unsigned short WMVDecBuildNumberD[8] = { 'MW', 'DV', 'CE', 'D',
    VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_WMVDecBuildNumber")
__declspec(selectany) unsigned short WMVDecBuildNumber[8] = { 'MW', 'DV', 'CE', 0,
    VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif

#endif // __WMVVER_H__
