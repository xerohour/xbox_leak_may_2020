#include <xboxverp.h>

#pragma data_seg(".XBLD$V")

#if DBG
unsigned short XboxKrnlBuildNumber[8] = { 'X' | ('B' << 8), 'O' | ('X' << 8), 'K' | ('R' << 8), 'N' | ('L' << 8),
                                           VER_PRODUCTVERSION | 0x8000 };
#else
unsigned short XboxKrnlBuildNumber[8] = { 'X' | ('B' << 8), 'O' | ('X' << 8), 'K' | ('R' << 8), 'N' | ('L' << 8),
                                           VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg()
