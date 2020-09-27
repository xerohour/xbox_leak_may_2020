#include <xboxverp.h>

//
// Force the linker to include library version info in the XBLD section
// if we are building d3d8$(D).lib - not d3d8ntpr$(D).lib, which has
// STARTUPANIMATION defined
//

#ifndef STARTUPANIMATION
extern "C"
{
#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_D3D8BuildNumberD")
__declspec(selectany) unsigned short D3D8BuildNumberD[8] = { 'D' | ('3' << 8), 'D' | ('8' << 8), 'D', 0,
                                                             VER_PRODUCTVERSION | 0x8000 };
#elif PROFILE
#pragma comment(linker, "/include:_D3D8BuildNumberP")
__declspec(selectany) unsigned short D3D8BuildNumberP[8] = { 'D' | ('3' << 8), 'D' | ('8' << 8), 'I', 0,
                                                             VER_PRODUCTVERSION };
#elif LTCG
#pragma comment(linker, "/include:_D3D8BuildNumberL")
__declspec(selectany) unsigned short D3D8BuildNumberL[8] = { 'D' | ('3' << 8), 'D' | ('8' << 8), 'L' | ('T' << 8), 'C' | ('G' << 8),
                                                             VER_PRODUCTVERSION | 0x4000 };
#else
#pragma comment(linker, "/include:_D3D8BuildNumber")
__declspec(selectany) unsigned short D3D8BuildNumber[8] = { 'D' | ('3' << 8), 'D' | ('8' << 8), 0, 0,
                                                            VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg()
}
#endif // ! STARTUPANIMATION

