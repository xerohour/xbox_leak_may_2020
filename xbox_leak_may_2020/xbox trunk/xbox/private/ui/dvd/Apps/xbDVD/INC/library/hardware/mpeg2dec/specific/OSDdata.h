// to import OSD-data

#include "library\common\prelude.h"

#define OSDpixels1 184
#define OSDlines1 94
#define OSDpixels2 160
#define OSDlines2 90

#if WDM_VERSION
extern DWORD OSDdata1[OSDpixels1 / 2 * OSDlines1];
extern DWORD OSDdata2[OSDpixels2 / 8 * OSDlines2];
#else
extern DWORD __based(__segname("_CODE")) OSDdata1[OSDpixels1 / 2 * OSDlines1];
extern DWORD __based(__segname("_CODE")) OSDdata2[OSDpixels2 / 8 * OSDlines2];
#endif

                                                           

