#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_DSoundBuildNumberD")
__declspec(selectany) unsigned short DSoundBuildNumberD[8] = { 'D' | ('S' << 8), 'O' | ('U' << 8), 'N' | ('D' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x8000 };
#elif LTCG
#pragma comment(linker, "/include:_DSoundBuildNumberL")
__declspec(selectany) unsigned short DSoundBuildNumberL[8] = { 'D' | ('S' << 8), 'O' | ('U' << 8), 'N' | ('D' << 8),  'L' | ('T' << 8),
                                        VER_PRODUCTVERSION | 0x4000 };
#else
#pragma comment(linker, "/include:_DSoundBuildNumber")
__declspec(selectany) unsigned short DSoundBuildNumber[8] = { 'D' | ('S' << 8), 'O' | ('U' << 8), 'N' | ('D' << 8), 0,
                                        VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif
