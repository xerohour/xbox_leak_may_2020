#ifndef _VERSION_H_INCL
#include "version.h"                   /* SLM maintained version file */
#endif
#include "bld_version.h"               /* User maintained build version */

#if defined(_WIN32) || defined(WIN32)
#include <winver.h>
#else   /* !WIN32 */
#include <ver.h>
#endif  /* !WIN32 */

#if     (rmm < 10)
#define rmmpad "0"
#define rmmpad_W L"0"
#else
#define rmmpad
#define rmmpad_W
#endif

#if     (rbld == 0)
#define VERSION_STR1(a,b,c,d)       #a "." rmmpad #b "." ruppad #c
#define VERSION_STR1_W(a,b,c,d)     L#a L"." rmmpad_W L#b L"." ruppad_W L#c
#else
#define VERSION_STR1(a,b,c,d)       #a "." rmmpad #b "." ruppad #c "." #d
#define VERSION_STR1_W(a,b,c,d)     L#a L"." rmmpad_W L#b L"." ruppad_W L#c L"." L#d
#endif

#if     (rup < 10)
#define ruppad "000"
#define ruppad_W L"000"
#elif   (rup < 100)
#define ruppad "00"
#define ruppad_W L"00"
#elif   (rup < 1000)
#define ruppad "0"
#define ruppad_W L"0"
#else
#define ruppad
#define ruppad_W
#endif

#define VERSION_STR2(a,b,c,d)       VERSION_STR1(a,b,c,d)
#define VERSION_STR2_W(a,b,c,d)     VERSION_STR1_W(a,b,c,d)
#define VER_PRODUCTVERSION_STR      VERSION_STR2(rmj,rmm,rup,rbld)
#define VER_PRODUCTVERSION_STR_W    VERSION_STR2_W(rmj,rmm,rup,rbld)
#define VER_PRODUCTVERSION          rmj,rmm,rup,rbld

/*--------------------------------------------------------------*/
/* the following section defines values used in the version     */
/* data structure for all files, and which do not change.       */
/*--------------------------------------------------------------*/

#if defined(_SHIP)
#define VER_DEBUG                   0
#else
#define VER_DEBUG                   VS_FF_DEBUG
#endif

#if defined(_SHIP)
#define VER_PRIVATEBUILD            0
#else
#define VER_PRIVATEBUILD            VS_FF_PRIVATEBUILD
#endif

#if defined(_SHIP)
#define VER_PRERELEASE              0
#else
#define VER_PRERELEASE              VS_FF_PRERELEASE
#endif

#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#if defined(_WIN32) || defined(WIN32)
#define VER_FILEOS                  VOS__WINDOWS32
#else
#define VER_FILEOS                  VOS_DOS_WINDOWS16
#endif
#define VER_FILEFLAGS               (VER_PRIVATEBUILD|VER_PRERELEASE|VER_DEBUG)

#define VER_COMPANYNAME_STR         "Microsoft Corporation"
#define VER_COMPANYNAME_STR_W       L"Microsoft Corporation"
#define VER_PRODUCTNAME_STR         "Microsoft\256 Visual Studio .NET"
#define VER_PRODUCTNAME_STR_W       L"Microsoft\256 Visual Studio .NET"
