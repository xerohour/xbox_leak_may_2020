#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_TestSoundLibBuildNumberD")
__declspec(selectany) unsigned short TestSoundLibBuildNumberD[8] = { 'T' | ('E' << 8), 'S' | ('T' << 8), 'S' | ('N' << 8), 'D' | ('D' << 8),
                                        VER_PRODUCTVERSION | 0x8000 };
#elif LTCG
#pragma comment(linker, "/include:_TestSoundLibBuildNumberL")
__declspec(selectany) unsigned short TestSoundLibBuildNumberL[8] = { 'T' | ('E' << 8), 'S' | ('T' << 8), 'S' | ('N' << 8), 'D' | ('D' << 8), 'L',
                                        VER_PRODUCTVERSION | 0x4000 };
#else
#pragma comment(linker, "/include:_TestSoundLibBuildNumber")
__declspec(selectany) unsigned short TestSoundLibBuildNumber[8] = { 'T' | ('E' << 8), 'S' | ('T' << 8), 'S' | ('N' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif
