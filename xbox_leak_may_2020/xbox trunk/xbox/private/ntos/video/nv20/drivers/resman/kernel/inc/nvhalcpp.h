 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************** HAL/RM Interfaces ******************************\
*                                                                           *
* Module: nvhalcpp.h                                                        *
*   This include file contains any macro redefinitions required within the  *
*   HAL code to allow sharing of cpp directives between the RM and the HAL. *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#undef nvAddr
#undef ChipBugs
#define ChipBugs  pHalHwInfo->pMcHalInfo->ChipBugs

#ifdef DEBUG

// XXX  FIFOLOG generates external references wrt the HAL.
// XXX  At some point we need to HAL-ify this if we care about
// XXX  loading debug modules.  
#ifdef PURE_DEBUG_HAL
#undef FIFOLOG
#define FIFOLOG(a,b,c)
#endif

#undef  DBG_PRINT_STRING
#define DBG_PRINT_STRING(d,s) (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmPrintString)((PHALHWINFO) pHalHwInfo->pDeviceId, d, s)  

#undef  DBG_PRINT_VALUE
#define DBG_PRINT_VALUE(d,v) (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmPrintValue)((PHALHWINFO) pHalHwInfo->pDeviceId, d, v) 

#undef  DBG_PRINT_STRING_VALUE
#define DBG_PRINT_STRING_VALUE(d,s,v) (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmPrintStringValue)((PHALHWINFO) pHalHwInfo->pDeviceId, d, s, v) 

#undef  DBG_PRINT_STRING_PTR
#define DBG_PRINT_STRING_PTR(d,s,v) (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmPrintStringPtr)((PHALHWINFO) pHalHwInfo->pDeviceId, d, s, (VOID*) v) 

#undef  DBG_PRINT_STRING_VAL64
#define DBG_PRINT_STRING_VAL64(d,s,v) (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmPrintStringVal64)((PHALHWINFO) pHalHwInfo->pDeviceId, d, s, v) 

#undef  DBG_BREAKPOINT
#define DBG_BREAKPOINT() (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmBreakPoint)((PHALHWINFO) pHalHwInfo->pDeviceId) 

#else // DEBUG

#undef  DBG_PRINT_STRING
#define DBG_PRINT_STRING(d,s) ((void)0)

#undef  DBG_PRINT_VALUE
#define DBG_PRINT_VALUE(d,v) ((void)0)

#undef  DBG_PRINT_STRING_VALUE
#define DBG_PRINT_STRING_VALUE(d,s,v) ((void)0)

#undef  DBG_PRINT_STRING_PTR
#define DBG_PRINT_STRING_PTR(d,s,v) ((void)0)

#undef  DBG_PRINT_STRING_VAL64
#define DBG_PRINT_STRING_VAL64(d,s,v) ((void)0)

#undef  DBG_BREAKPOINT
#define DBG_BREAKPOINT() ((void)0)

#endif // DEBUG

#undef REG_WR08
#undef REG_RD08
#undef REG_WR08_DIRECT
#undef REG_RD08_DIRECT
#define REG_WR08(o,v)   (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmRegWr08)((PHALHWINFO) pHalHwInfo->pDeviceId, (NvU32)(NV_UINTPTR_T)o, (U008) v)
#define REG_RD08(o)		(*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmRegRd08)((PHALHWINFO) pHalHwInfo->pDeviceId, (NvU32)(NV_UINTPTR_T)o)





#undef REG_WR32
#undef REG_RD32
#undef REG_WR32_DIRECT
#undef REG_RD32_DIRECT
#define REG_WR32(o,v)   (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmRegWr32)((PHALHWINFO) pHalHwInfo->pDeviceId, (NvU32)(NV_UINTPTR_T)o, v)
#define REG_RD32(o)		(*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmRegRd32)((PHALHWINFO) pHalHwInfo->pDeviceId, (NvU32)(NV_UINTPTR_T)o)


#undef osDelay
#define osDelay(m)      (*((PHALRMFUNCS_000)((PHALHWINFO) pHalHwInfo)->pHalRmFuncs)->_nvHalRmDelay)((PHALHWINFO) pHalHwInfo->pDeviceId, m)

#undef INST_RD32
#define INST_RD32(i,o)      REG_RD32((pHalHwInfo->pPramHalInfo->PraminOffset+((i)<<4)+(o)))

#undef INST_WR32
#define INST_WR32(i,o,d)    REG_WR32((pHalHwInfo->pPramHalInfo->PraminOffset+((i)<<4)+(o)),(d))
