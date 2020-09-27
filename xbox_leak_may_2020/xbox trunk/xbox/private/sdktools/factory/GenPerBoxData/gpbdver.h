#include <xboxverp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_GPBDBuildNumberD")
__declspec(selectany) unsigned short GPBDBuildNumberD[8] = { 'G' | ('P' << 8), 'B' | ('D' << 8), 'A' | ('T' << 8), 'A' | ('D' << 8),
                                        VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_GPBDBuildNumber")
__declspec(selectany) unsigned short GPBDBuildNumber[8] = { 'G' | ('P' << 8), 'B' | ('D' << 8), 'A' | ('T' << 8), 'A',
                                        VER_PRODUCTVERSION };
#endif

#pragma data_seg()

#ifdef __cplusplus
}
#endif
