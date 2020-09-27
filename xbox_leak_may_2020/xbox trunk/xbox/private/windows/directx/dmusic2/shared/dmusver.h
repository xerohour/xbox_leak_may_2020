#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_DMusicBuildNumberD")
__declspec(selectany) unsigned short DMusicBuildNumberD[8] = { 'D' | ('M' << 8), 'U' | ('S' << 8), 'I' | ('C' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x8000 };
#elif PROFILE
#pragma comment(linker, "/include:_DMusicBuildNumberI")
__declspec(selectany) unsigned short DMusicBuildNumberI[8] = { 'D' | ('M' << 8), 'U' | ('S' << 8), 'I' | ('C' << 8), 'I',
                                        VER_PRODUCTVERSION };
#elif LTCG
#pragma comment(linker, "/include:_DMusicBuildNumberL")
__declspec(selectany) unsigned short DMusicBuildNumberL[8] = { 'D' | ('M' << 8), 'U' | ('S' << 8), 'I' | ('C' << 8), 'L' | ('T' << 8),
                                        VER_PRODUCTVERSION | 0x4000 };
#else
#pragma comment(linker, "/include:_DMusicBuildNumber")
__declspec(selectany) unsigned short DMusicBuildNumber[8] = { 'D' | ('M' << 8), 'U' | ('S' << 8), 'I' | ('C' << 8), 0,
                                        VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg()
#ifdef __cplusplus
}
#endif // __cplusplus

