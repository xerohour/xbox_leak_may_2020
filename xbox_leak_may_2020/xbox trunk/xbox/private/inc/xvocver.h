#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_XVoiceBuildNumberD")
__declspec(selectany) unsigned short XVoiceBuildNumberD[8] = { 'X' | ('V' << 8), 'O' | ('I' << 8), 'C' | ('E' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_XVoiceBuildNumber")
__declspec(selectany) unsigned short XVoiceBuildNumber[8] = { 'X' | ('V' << 8), 'O' | ('I' << 8), 'C' | ('E' << 8), 0,
                                        VER_PRODUCTVERSION | 0x0000 };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif

