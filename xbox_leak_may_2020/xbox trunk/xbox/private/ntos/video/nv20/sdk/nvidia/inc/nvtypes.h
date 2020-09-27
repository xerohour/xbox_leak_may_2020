 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/


 /***************************************************************************\
|*                                                                           *|
|*                         NV Architecture Interface                         *|
|*                                                                           *|
|*  <nvtypes.h> defines common widths used to access hardware in of NVIDIA's *|
|*  Unified Media Architecture (TM).                                         *|
|*                                                                           *|
 \***************************************************************************/


#ifndef NVTYPES_INCLUDED
#define NVTYPES_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************\
|*    IA64 common defines                                                   *|
\****************************************************************************/

#if defined(H2INC) || ((defined(WIN32) || defined(_WIN32)) && !defined(_WIN32_WINNT)) /* win9x */ || defined(__DJGPP__)
#undef NVRM_64
#else                                             /* all other platforms */
#define NVRM_64 1
#  if defined(_WIN64) || defined(_LINUX64)
#    define NVRM_TRUE64 1
#  endif
#endif

#if defined(linux) || defined(MACOS) || defined(macintosh) || defined(__APPLE_CC__)
#define __int64 long long int
#endif

 /***************************************************************************\
|*                                 Typedefs                                  *|
 \***************************************************************************/

typedef unsigned char      NvV8;  /* "void": enumerated or multiple fields   */
typedef unsigned short     NvV16; /* "void": enumerated or multiple fields   */
typedef unsigned char      NvU8;  /* 0 to 255                                */
typedef unsigned short     NvU16; /* 0 to 65535                              */
typedef signed char        NvS8;  /* -128 to 127                             */
typedef signed short       NvS16; /* -32768 to 32767                         */
typedef float              NvF32; /* IEEE Single Precision (S1E8M23)         */
typedef double             NvF64; /* IEEE Double Precision (S1E11M52)        */

/* mainly for 64-bit linux, where long is 64 bits
 * and win9x, where int is 16 bit.
 */
#if defined(linux)
typedef unsigned int       NvV32; /* "void": enumerated or multiple fields   */
typedef unsigned int       NvU32; /* 0 to 4294967295                         */
typedef signed int         NvS32; /* -2147483648 to 2147483647               */
#else
typedef unsigned long      NvV32; /* "void": enumerated or multiple fields   */
typedef unsigned long      NvU32; /* 0 to 4294967295                         */
typedef signed long        NvS32; /* -2147483648 to 2147483647               */
#endif

 /***************************************************************************\
|*                                                                           *|
|*  64 bit type definitions for use in interface structures.                 *|
|*                                                                           *|
 \***************************************************************************/

#if defined(NVRM_TRUE64)

typedef unsigned __int64   NvV64; /* "void": enumerated or multiple fields   */
typedef unsigned __int64   NvU64; /* 0 to 18446744073709551615               */
typedef __int64            NvS64; /* 2^-63 to 2^63-1                         */
typedef void*              NvP64; /* 64 bit void pointer                     */

typedef NvU64 NvU64_VALUE_T;
typedef NvP64 NvP64_VALUE_T;

#define NvU64_VALUE(n)    (n)
#define NvP64_VALUE(n)    (n)
#define NvP64_SELECTOR(n) (0)

#elif defined(NVRM_64)

typedef unsigned __int64   NvV64; /* "void": enumerated or multiple fields   */
typedef unsigned __int64   NvU64; /* 0 to 18446744073709551615               */
typedef __int64            NvS64; /* 2^-63 to 2^63-1                         */
typedef unsigned __int64   NvP64; /* 64 bit void pointer                     */

typedef NvU64 NvU64_VALUE_T;
typedef NvP64 NvP64_VALUE_T;

#define NvU64_VALUE(n)    (n)
#define NvP64_VALUE(n)    (n)
#define NvP64_SELECTOR(n) (0)

#else

typedef struct {
 NvU32 low;
 NvU32 high;
}                          NvV64; /* "void": enumerated or multiple fields   */
typedef struct {
 NvU32 low;
 NvU32 high;
}                          NvU64; /* 0 to 18446744073709551615               */
typedef struct {
 NvU32 low;
 NvS32 high;
}                          NvS64; /* 2^-63 to 2^63-1                         */
typedef struct {
 NvU32 offset;
 NvU16 selector;
 NvU16 reserved;
}                          NvP64; /* 64 bit void pointer                     */

typedef NvU32 NvP64_VALUE_T;
typedef NvU32 NvU64_VALUE_T;

#define NvU64_VALUE(n)    ((n).low)
#define NvP64_VALUE(n)    ((n).offset)
#define NvP64_SELECTOR(n) ((n).selector)

#endif

 /***************************************************************************\
|*                                                                           *|
|*  64 bit integer definitions for high precision arithmetic.                *|
|*  Ideally, NvU64/NvS64 would be a flat integer on all platforms            *|
|*  but __int64 is not recognized by 16 bit compilers.                       *|
|*                                                                           *|
 \***************************************************************************/

#if (defined(_WIN32) && !defined(H2INC)) || defined(NVRM_64)
typedef unsigned __int64   NvUI64; /* 0 to 18446744073709551615              */
typedef __int64            NvSI64; /* 2^-63 to 2^63-1                        */
#endif

/* obsolete stuff */
typedef NvV8  V008;
typedef NvV16 V016;
typedef NvV32 V032;
typedef NvV64 V064;
typedef NvU8  U008;
typedef NvU16 U016;
typedef NvU32 U032;
typedef NvU64 U064;
typedef NvS8  S008;
typedef NvS16 S016;
typedef NvS32 S032;
typedef NvS64 S064;
typedef NvP64 P064;
typedef NvF32 F032;
typedef NvF64 F064;
#if defined(MACOS) || defined(macintosh) || defined(__APPLE_CC__)
// more obsolete stuff
// need to provide these on macos9 and macosX
typedef unsigned long  ULONG;
typedef unsigned long  DWORD;
typedef unsigned char *PUCHAR;
typedef void           VOID;
typedef void          *PVOID;
typedef void          *HANDLE;
#endif

/* polymorphic 64-bit macros */
#if defined(NVRM_TRUE64)
typedef NvP64 VOID_PTR;
typedef NvU64 NV_UINTPTR_T;
#else
typedef NvU32 VOID_PTR;
typedef NvU32 NV_UINTPTR_T;
#endif

#ifdef __cplusplus
};
#endif
#endif /* NVTYPES_INCLUDED */
