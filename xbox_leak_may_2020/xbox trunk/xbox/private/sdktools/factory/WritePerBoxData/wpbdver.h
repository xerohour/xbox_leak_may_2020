#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(push)
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_WPBDBuildNumberD")
__declspec(selectany) unsigned short WPBDBuildNumberD[8] = { 'W' | ('P' << 8), 'B' | ('D' << 8), 'A' | ('T' << 8), 'A' | ('D' << 8),
                                        VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_WPBDBuildNumber")
__declspec(selectany) unsigned short WPBDBuildNumber[8] = { 'W' | ('P' << 8), 'B' | ('D' << 8), 'A' | ('T' << 8), 'A',
                                        VER_PRODUCTVERSION };
#endif

#pragma data_seg(pop)

#ifdef __cplusplus
}
#endif
