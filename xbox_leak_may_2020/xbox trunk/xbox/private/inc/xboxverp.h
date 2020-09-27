/****************************************************************************
 *                                                                          *
 *      XboxVerP.H      -- Version information for internal builds          *
 *                                                                          *
 *      This file is only modified by the official builder to update the    *
 *      VERSION, VER_PRODUCTVERSION, VER_PRODUCTVERSION_STR and             *
 *      VER_PRODUCTBETA_STR values.                                         *
 *                                                                          *
 ****************************************************************************/

/*--------------------------------------------------------------*/
/* the following values should be modified by the official      */
/* builder for each build                                       */
/*                                                              */
/* the VER_PRODUCTBUILD lines must contain the product          */
/* comments (Win9x or NT) and end with the build#<CR><LF>       */
/*                                                              */
/* the VER_PRODUCTBETA_STR lines must  contain the product      */
/* comments (Win9x or NT) and end with "some string"<CR><LF>    */
/*--------------------------------------------------------------*/

#if _MSC_VER > 1000 && !defined(SKIP_XBOXVERP_PRAGMA)
#pragma once
#endif

#define VER_PRODUCTBUILD_QFE        1
#define VER_PRODUCTBUILD            4400        // Must be greater than Windows 2000 gold
#define VER_PRODUCTBETA_STR         ""
#define VER_PRODUCTVERSION_STRING   "1.00"      // Not sure this will work, might have problems being less than NT5
#define VER_PRODUCTVERSION          1,00,VER_PRODUCTBUILD,VER_PRODUCTBUILD_QFE
#define VER_PRODUCTVERSION_W        (0x0100)
#define VER_PRODUCTVERSION_DW       (0x01000000 | VER_PRODUCTBUILD)

#define VER_WEB_YEAR_MONTH          02apr       // Format: YYmmm (Used for website URLs on xds.xbox.com for the current relnotes)

/* Define the _XTL_VER constant that will end up in xtl.h */
#if 0
#define _XTL_VER                   4400        // xtl
#endif

/* Define the minimum library version that we will approve (3911 == August Final). */
#define MINIMUM_APPROVED_XTL_VER    3911

/*--------------------------------------------------------------*/
/* this value is used by third party drivers build with the DDK */
/* and internally, to avoid version number conflicts.           */
/*--------------------------------------------------------------*/
#define VER_DDK_PRODUCTVERSION       5,01       // Leave this 5 so we don't not install over NT drivers
#define VER_DDK_PRODUCTVERSION_STR  "5.01"

#if     (VER_PRODUCTBUILD < 10)
#define VER_BPAD "000"
#elif   (VER_PRODUCTBUILD < 100)
#define VER_BPAD "00"
#elif   (VER_PRODUCTBUILD < 1000)
#define VER_BPAD "0"
#else
#define VER_BPAD
#endif

#define VER_PRODUCTVERSION_STR2(x,y) VER_PRODUCTVERSION_STRING "." VER_BPAD#x "." #y
#define VER_PRODUCTVERSION_STR1(x,y) VER_PRODUCTVERSION_STR2(x, y)
#define VER_PRODUCTVERSION_STR       VER_PRODUCTVERSION_STR1(VER_PRODUCTBUILD, VER_PRODUCTBUILD_QFE)

/*--------------------------------------------------------------*/
/* the following section defines values used in the version     */
/* data structure for all files, and which do not change.       */
/*--------------------------------------------------------------*/

/* default is nodebug */
#if DBG
#define VER_DEBUG                   VS_FF_DEBUG
#else
#define VER_DEBUG                   0
#endif

/* default is prerelease */
#if BETA
#define VER_PRERELEASE              VS_FF_PRERELEASE
#else
#define VER_PRERELEASE              0
#endif

#if OFFICIAL_BUILD
#define VER_PRIVATE                 0
#else
#define VER_PRIVATE                 VS_FF_PRIVATEBUILD
#endif

#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEOS                  VOS_NT_WINDOWS32
#define VER_FILEFLAGS               (VER_PRERELEASE|VER_DEBUG|VER_PRIVATE)

// @@BEGIN_DDKSPLIT
#if 0
// @@END_DDKSPLIT
#define VER_COMPANYNAME_STR         "Windows (R) Xbox DDK provider"
#define VER_PRODUCTNAME_STR         "Windows (R) Xbox DDK driver"
#define VER_LEGALTRADEMARKS_STR     \
"Windows (R) is a registered trademark of Microsoft Corporation."
// @@BEGIN_DDKSPLIT
#else

#define VER_COMPANYNAME_STR         "Microsoft Corporation"
#define VER_PRODUCTNAME_STR         "Microsoft(R) Xbox(TM)"
#define VER_LEGALTRADEMARKS_STR     \
"Microsoft(R) is a registered trademark of Microsoft Corporation. Xbox(TM) is a trademark of Microsoft Corporation."
#endif
// @@END_DDKSPLIT
