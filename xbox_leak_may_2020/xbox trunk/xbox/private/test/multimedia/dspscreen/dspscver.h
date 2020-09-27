#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_DSPScreenBuildNumberD")
__declspec(selectany) unsigned short DSPScreenBuildNumberD[8] = { 'D' | ('S' << 8), 'P' | ('S' << 8), 'C' | ('R' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_DSPScreenBuildNumber")
__declspec(selectany) unsigned short DSPScreenBuildNumber[8] = { 'D' | ('S' << 8), 'P' | ('S' << 8), 'C' | ('R' << 8), 0,
                                        VER_PRODUCTVERSION };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif
