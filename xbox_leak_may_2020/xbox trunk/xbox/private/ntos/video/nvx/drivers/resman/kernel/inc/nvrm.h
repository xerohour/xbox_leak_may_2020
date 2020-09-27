#ifndef _NVRM_H_
#define _NVRM_H_

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

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: NVRM.H                                                            *
*       Resource Manager defines and structures used throughout the code.   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Version number of files built using this header file.
//
//  The format of this is 0x00010206, where
//   - 0x0001 is the chip architecture
//   -   0x02 is the software release
//   -   0x06 is the minor revision
//
//---------------------------------------------------------------------------

#define NVRM_VERSION    0x00030200

//---------------------------------------------------------------------------
//  define some inline assembly instructions

#ifdef __WATCOMC__

void my_sti();
#pragma aux my_sti = \
    "pushf" \
    "sti";

void my_cli();
#pragma aux my_cli =  \
    "popf";

#else   // if not Watcom, its the Microsoft compiler

#define my_sti \
    _asm    pushf   \
    _asm    sti
#define my_cli \
    __asm   popf

#endif  // _WATCOMC_

//---------------------------------------------------------------------------
//
//  NVidia data types.
//
//---------------------------------------------------------------------------

#include <nvtypes.h>

//
// Fixed point math types and macros.
//
typedef signed long     S016d016;
typedef unsigned long   U016d016;
typedef signed long     S008d024;
typedef unsigned long   U008d024;
typedef signed long     S012d020;
typedef unsigned long   U012d020;

#define FIX16_POINT             16
#define FIX16_SCALE             ((U032)1<<FIX16_POINT)
#define FIX16_INT_MASK          ((U032)~0<<FIX16_POINT)
#define FIX16_FRAC_MASK         (~FIX16_INT_MASK)
#define FIX16_INT(f)            ((f)>>FIX16_POINT)
#define FIX16_FRAC(f)           ((f)&FIX16_FRAC_MASK)
#define INT_FIX16(f)            ((f)<<FIX16_POINT)
#define FIX16(i)                (S016d016)((i)*FIX16_SCALE))
#define FIX16_MUL(f1,f2)        ((FIX16_INT(f1)*FIX16_INT(f2)<<FIX16_POINT)     \
                                +(FIX16_INT(f1)*FIX16_FRAC(f2))                 \
                                +(FIX16_FRAC(f1)*FIX16_INT(f2))                 \
                                +(FIX16_FRAC(f1)*FIX16_FRAC(f2)>>FIX16_POINT))
#define FIX16_DIV(f1,f2)        (((f1)<<(FIX16_POINT/2))/((f2)>>(FIX16_POINT/2)))
#define FIX16_DIVI(f1,f2)       ((f1)/((f2)>>(FIX16_POINT)))
#define FIX16_IDIV(f1,f2)       (((f1)<<(FIX16_POINT))/(f2))
#define FIX24_POINT             24
#define FIX24_SCALE             ((U032)1<<FIX24_POINT)
#define FIX24_INT_MASK          ((U032)~0<<FIX24_POINT)
#define FIX24_FRAC_MASK         (~FIX24_INT_MASK)
#define FIX24_TO_INT(f)         ((f)>>FIX24_POINT)
#define FIX24(i)                ((S008d024)((i)*FIX24_SCALE))
#define FIX24_MUL(f1,f2)        (((f1)>>(FIX24_POINT/2))*((f2)>>(FIX24_POINT/2)))
#define FIX24_DIV(f1,f2)        (((f1)<<(FIX24_POINT/2))/((f2)>>(FIX24_POINT/2)))
#define FIX24_DIVI(f1,f2)       ((f1)/((f2)>>(FIX24_POINT)))
#define FIX24_IDIV(f1,f2)       (((f1)<<(FIX24_POINT))/(f2))

//---------------------------------------------------------------------------
//
//  Common types.
//
//---------------------------------------------------------------------------

#ifndef VOID
#define VOID            void
#endif
#ifndef BOOL
#define BOOL            S032
#endif
#ifndef TRUE
#define TRUE            !0L
#endif
#ifndef FALSE
#define FALSE           0L
#endif
#ifndef NULL
#define NULL            0L
#endif
    
//---------------------------------------------------------------------------
//
//  Error codes.
//
//---------------------------------------------------------------------------

//
// External codes.
//
#define NV_ERROR_1                      0x0100
#define NV_TYPE_CONFLICT                (0x01|NV_ERROR_1)
#define NV_OUT_OF_RANGE                 (0x02|NV_ERROR_1)
#define NV_NO_CONNECTION                (0x03|NV_ERROR_1)
#define NV_NO_SUCH_OBJECT               (0x04|NV_ERROR_1)
#define NV_NAME_IN_USE                  (0x05|NV_ERROR_1)
#define NV_OUT_OF_RESOURCES             (0x06|NV_ERROR_1)
#define NV_TRANSLATION_VIOLATION        (0x07|NV_ERROR_1)
#define NV_PROTECTION_VIOLATION         (0x08|NV_ERROR_1)
#define NV_BUFFER_BUSY                  (0x09|NV_ERROR_1)
#define NV_ILLEGAL_ACCESS               (0x0A|NV_ERROR_1)
#define NV_BAD_COLORMAP_FORMAT          (0x0B|NV_ERROR_1)
#define NV_BAD_COLOR_FORMAT             (0x0C|NV_ERROR_1)
#define NV_BAD_MONOCHROME_FORMAT        (0x0D|NV_ERROR_1)
#define NV_BAD_PATTERN_SHAPE            (0x0E|NV_ERROR_1)
#define NV_BAD_SUBDIVIDE                (0x0F|NV_ERROR_1)
#define NV_NO_CURRENT_POINT             (0x10|NV_ERROR_1)
#define NV_BAD_AUDIO_FORMAT             (0x11|NV_ERROR_1)
#define NV_NO_DMA_TRANSLATION           (0x12|NV_ERROR_1)
#define NV_INCOMPLETE_METHOD            (0x13|NV_ERROR_1)
#define NV_RESERVED_ADDRESS             (0x14|NV_ERROR_1)
#define NV_UNIMPLEMENTED_PATCH          (0x15|NV_ERROR_1)
#define NV_OS_NAME_ERROR                (0x16|NV_ERROR_1)
#define NV_INCOMPLETE_PATCH             (0x17|NV_ERROR_1)
#define NV_BUFFERGAP_ERROR              (0x18|NV_ERROR_1)
#define NV_FIFO_OVERFLOW                (0x19|NV_ERROR_1)
//
// Internal codes.
//
typedef U032 RM_STATUS;
#define NUM_RM_ERRORS                   22
#define RM_OK                           0x00000000
#define RM_ERROR                        0xFFFFFFFF
#define RM_WARN_NULL_OBJECT             (0x10000000|RM_OK)
#define RM_WARN_AUDIO_DISABLED          (0x20000000|RM_OK)
#define RM_WARN_GRAPHICS_DISABLED       (0x30000000|RM_OK)
#define RM_ERR_NO_FREE_MEM              (0x01000000|NV_OUT_OF_RESOURCES)
#define RM_ERR_MEM_NOT_FREED            (0x01010000|NV_OUT_OF_RESOURCES)
#define RM_ERR_PAGE_TABLE_NOT_AVAIL     (0x01020000|NV_OUT_OF_RESOURCES)
#define RM_ERR_NO_FREE_FIFOS            (0x02000000|NV_OUT_OF_RESOURCES)
#define RM_ERR_CANT_CREATE_CLASS_OBJS   (0x02010000|NV_OUT_OF_RESOURCES)
#define RM_ERR_BAD_OBJECT               (0x03000000|NV_NO_SUCH_OBJECT)
#define RM_ERR_INSERT_DUPLICATE_NAME    (0x03010000|NV_NAME_IN_USE)
#define RM_ERR_OBJECT_NOT_FOUND         (0x03020000|NV_NO_SUCH_OBJECT)
#define RM_ERR_CREATE_BAD_CLASS         (0x04000000|NV_NO_SUCH_OBJECT)
#define RM_ERR_DELETE_BAD_CLASS         (0x04010000|NV_NO_SUCH_OBJECT)
#define RM_ERR_FIFO_RUNOUT_OVERFLOW     (0x05000000|NV_FIFO_OVERFLOW)
#define RM_ERR_FIFO_BAD_ACCESS          (0x05010000|NV_RESERVED_ADDRESS)
#define RM_ERR_FIFO_OVERFLOW            (0x05020000|NV_FIFO_OVERFLOW)
#define RM_ERR_METHOD_ORDER             (0x06000000|NV_INCOMPLETE_METHOD)
#define RM_ERR_METHOD_COUNT             (0x06010000|NV_INCOMPLETE_METHOD)
#define RM_ERR_ILLEGAL_OBJECT           (0x06020000|NV_TYPE_CONFLICT)
#define RM_ERR_DMA_IN_USE               (0x07000000|NV_BUFFER_BUSY)
#define RM_ERR_BAD_DMA_SPECIFIER        (0x07010000|NV_TRANSLATION_VIOLATION)
#define RM_ERR_INVALID_XLATE            (0x07020000|NV_TRANSLATION_VIOLATION)
#define RM_ERR_INVALID_START_LENGTH     (0x07030000|NV_PROTECTION_VIOLATION)
#define RM_ERR_DMA_MEM_NOT_LOCKED       (0x07040000|NV_OUT_OF_RESOURCES)
#define RM_ERR_DMA_MEM_NOT_UNLOCKED     (0x07050000|NV_OUT_OF_RESOURCES)
#define RM_ERR_NOTIFY_IN_USE            (0x07060000|NV_BUFFER_BUSY)
#define RM_ERR_ILLEGAL_ADDRESS          (0x08000000|NV_ILLEGAL_ACCESS)
#define RM_ERR_BAD_ADDRESS              (0x08010000|NV_PROTECTION_VIOLATION)
#define RM_ERR_INVALID_COLOR_FORMAT     (0x09000000|NV_BAD_COLOR_FORMAT)
#define RM_ERR_INVALID_MONO_FORMAT      (0x09010000|NV_BAD_MONOCHROME_FORMAT)
#define RM_ERR_INVALID_AUDIO_FORMAT     (0x09020000|NV_BAD_AUDIO_FORMAT)
#define RM_ERR_INVALID_GAMEPORT_FORMAT  (0x09030000|NV_BAD_AUDIO_FORMAT)
#define RM_ERR_OBJECT_TYPE_MISMATCH     (0x0A000000|NV_TYPE_CONFLICT)
#define RM_ERR_NO_FREE_AUDIO_INSTANCES  (0x0B000000|NV_OUT_OF_RESOURCES)
#define RM_ERR_CODEC_INPUT_IN_USE       (0x0B010000|NV_OUT_OF_RESOURCES)
#define RM_ERR_AUDIO_DISABLED           (0x0B020000|NV_OUT_OF_RESOURCES)
#define RM_ERR_INCOMPLETE_PATCH         (0x0C000000|NV_INCOMPLETE_PATCH)
#define RM_ERR_INVALID_PATCH            (0x0C010000|NV_UNIMPLEMENTED_PATCH)
#define RM_ERR_PATCH_TOO_COMPLEX        (0x0C020000|NV_UNIMPLEMENTED_PATCH)
#define RM_ERR_MAX_PATCH_FANOUT         (0x0C030000|NV_UNIMPLEMENTED_PATCH)
#define RM_ERR_DEVICE_DISCONNECTED      (0x0C040000|NV_NO_CONNECTION)
#define RM_ERR_BUFFER_GAP               (0x0D000000|NV_BUFFER_GAP)
#define RM_ERR_INVALID_OS_NAME          (0x0E000000|NV_OS_NAME_ERROR)
#define RM_ERR_ILLEGAL_ACTION           (0x0E010000|NV_OS_NAME_ERROR)
// The DMA direction specified for the DMA object is not allowed in the
// specified address space.
#define RM_ERR_ILLEGAL_DIRECTION        (0x0E020000|NV_OS_NAME_ERROR)
#define RM_ERR_OUT_OF_TIMER_CALLBACKS   (0x0F000000|NV_OUT_OF_RESOURCES)

// Errors returned from the arch calls.
#define NV_ERROR_2                      0x0200
#define RM_ERR_OPERATING_SYSTEM         (0x00000001|NV_ERROR_2)
#define RM_ERR_BAD_OBJECT_PARENT        (0x00000002|NV_ERROR_2)
#define RM_ERR_BAD_OBJECT_HANDLE        (0x00000003|NV_ERROR_2)
#define RM_ERR_OBJECT_IN_USE            (0x00000004|NV_ERROR_2)
#define RM_ERR_OBJECT_HAS_CHILDERN      (0x00000005|NV_ERROR_2)
#define RM_ERR_BAD_CLASS                (0x00000006|NV_ERROR_2)
#define RM_ERR_INSUFFICIENT_RESOURCES   (0x00000007|NV_ERROR_2)
#define RM_ERR_BAD_FLAGS                (0x00000008|NV_ERROR_2)
#define RM_ERR_BAD_BASE                 (0x00000009|NV_ERROR_2)
#define RM_ERR_BAD_LIMIT                (0x0000000A|NV_ERROR_2)
#define RM_ERR_PROTECTION_FAULT         (0x0000000B|NV_ERROR_2)
#define RM_ERR_MULTIPLE_MEMORY_TYPES    (0x0000000C|NV_ERROR_2)
#define RM_ERR_BAD_OBJECT_ERROR         (0x0000000D|NV_ERROR_2)
#define RM_ERR_BAD_OBJECT_BUFFER        (0x0000000E|NV_ERROR_2)
#define RM_ERR_BAD_OFFSET               (0x0000000F|NV_ERROR_2)
#define RM_ERR_BAD_CLIENT               (0x00000010|NV_ERROR_2)

// Errors returned from HAL calls.
#define NV_ERROR_3                      (0x0400)
#define RM_ERR_VERSION_MISMATCH         (0x00000001|NV_ERROR_3)
#define RM_ERR_BAD_ARGUMENT             (0x00000002|NV_ERROR_3)
#define RM_ERR_INVALID_STATE            (0x00000003|NV_ERROR_3)

//
// RM_ASSERT macro
//
#ifdef DEBUG
#define RM_ASSERT(cond)                                                     \
    if ((cond) == 0x0)                                                      \
    {                                                                       \
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: assertion FAILED!\n");   \
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, __FILE__);                      \
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, ": line ");                     \
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)__LINE__);                  \
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");                          \
        DBG_BREAKPOINT();                                                   \
    }
#else
#define RM_ASSERT(cond)
#endif

//
// Power of 2 alignment.
//    (Will give unexpected results if 'a' is not a power of 2.)
//

#define NV_ALIGN_DOWN(v, gran) ((v) & ~((gran) - 1))
#define NV_ALIGN_UP(v, gran)   (((v) + ((gran) - 1)) & ~((gran)-1))

#define NV_ALIGN_PTR_DOWN(p, gran) ((void *) NV_ALIGN_DOWN(((NV_UINTPTR_T)p), (gran)))
#define NV_ALIGN_PTR_UP(p, gran)   ((void *) NV_ALIGN_UP(((NV_UINTPTR_T)p), (gran)))

#define NV_PAGE_ALIGN_DOWN(value) NV_ALIGN_DOWN((value), RM_PAGE_SIZE)
#define NV_PAGE_ALIGN_UP(value)   NV_ALIGN_UP((value), RM_PAGE_SIZE)

//
// NV Reference Manual register access definitions.
//
#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d)
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
//
// NV Reference Manual instance memory structure access definitions.
//
#define INST_WR32(i,o,d)        REG_WR32((pDev->Pram.HalInfo.PraminOffset+((i)<<4)+(o)),(d))
#define INST_RD32(i,o)          REG_RD32((pDev->Pram.HalInfo.PraminOffset+((i)<<4)+(o)))
#define SF_OFFSET(sf)           (((0?sf)/32)<<2)
#define SF_SHIFT(sf)            ((0?sf)&31)
#undef  SF_MASK
#define SF_MASK(sf)             (0xFFFFFFFF>>(31-(1?sf)+(0?sf)))
#define SF_DEF(s,f,c)           ((NV ## s ## f ## c)<<SF_SHIFT(NV ## s ## f))
#define SF_NUM(s,f,n)           (((n)&SF_MASK(NV ## s ## f))<<SF_SHIFT(NV ## s ## f))
#define SF_VAL(s,f,v)           (((v)>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))
#define RAM_WR_ISF_NUM(i,s,f,n) INST_WR32(i,SF_OFFSET(NV ## s ## f),SF_NUM(s,f,n))
#define RAM_WR_ISF_DEF(i,s,f,c) INST_WR32(i,SF_OFFSET(NV ## s ## f),SF_DEF(s,f,c))
#define FLD_WR_ISF_NUM(i,s,f,n) INST_WR32(i,SF_OFFSET(NV##s##f),(INST_RD32(i,SF_OFFSET(NV##s##f))&~(SF_MASK(NV##s##f)<<SF_SHIFT(NV##s##f)))|SF_NUM(s,f,n))
#define FLD_WR_ISF_DEF(i,s,f,c) INST_WR32(i,SF_OFFSET(NV##s##f),(INST_RD32(i,SF_OFFSET(NV##s##f))&~(SF_MASK(NV##s##f)<<SF_SHIFT(NV##s##f)))|SF_DEF(s,f,c))
#define RAM_RD_ISF(i,s,f)       (((INST_RD32(i,SF_OFFSET(NV ## s ## f)))>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))
//
// EEPROM access macros.
//
#define EEPROM_INDEX_RD08(ii, dd)   \
    {REG_WR32(NV_PEEPROM_CONTROL,DRF_NUM(_PEEPROM,_CONTROL,_ADDRESS,(ii))|DRF_DEF(_PEEPROM,_CONTROL,_COMMAND,_READ));\
    osDelay(1);\
    (dd)=REG_RD32(NV_PEEPROM_CONTROL)&0xFF;}
//
// DevInfo update/finish flags.
//
#define UPDATE_HWINFO_AUDIO_VOLUME             BIT(0)
#define FINISH_HWINFO_AUDIO_VOLUME             BIT(0)
#define UPDATE_HWINFO_AUDIO_NEAR_MARK          BIT(1)
#define FINISH_HWINFO_AUDIO_NEAR_MARK          BIT(1)
#define UPDATE_HWINFO_AUDIO_BLOCK_LENGTH       BIT(2)
#define FINISH_HWINFO_AUDIO_BLOCK_LENGTH       BIT(2)
#define UPDATE_HWINFO_DAC_CURSOR_POS           BIT(0)
#define FINISH_HWINFO_DAC_CURSOR_POS           BIT(0)
#define UPDATE_HWINFO_DAC_CURSOR_IMAGE         BIT(1)
#define FINISH_HWINFO_DAC_CURSOR_IMAGE         BIT(1)
#define UPDATE_HWINFO_DAC_CURSOR_COLOR         BIT(2)
#define FINISH_HWINFO_DAC_CURSOR_COLOR         BIT(2)
#define UPDATE_HWINFO_DAC_COLORMAP             BIT(3)
#define FINISH_HWINFO_DAC_COLORMAP             BIT(3)
#define UPDATE_HWINFO_DAC_CURSOR_ENABLE        BIT(4)
#define FINISH_HWINFO_DAC_CURSOR_ENABLE        BIT(4)
#define UPDATE_HWINFO_DAC_CURSOR_DISABLE       BIT(5)
#define FINISH_HWINFO_DAC_CURSOR_DISABLE       BIT(5)
#define UPDATE_HWINFO_DAC_CURSOR_TYPE          BIT(6)
#define FINISH_HWINFO_DAC_CURSOR_TYPE          BIT(6)
#define UPDATE_HWINFO_DAC_CURSOR_IMAGE_DELAYED BIT(7)
#define FINISH_HWINFO_DAC_CURSOR_IMAGE_DELAYED BIT(7)
#define UPDATE_HWINFO_DAC_DPML                 BIT(8)
#define FINISH_HWINFO_DAC_DPML                 BIT(8)
#define UPDATE_HWINFO_DAC_APLL                 BIT(9)
#define FINISH_HWINFO_DAC_APLL                 BIT(9)
#define UPDATE_HWINFO_DAC_VPLL                 BIT(10)
#define FINISH_HWINFO_DAC_VPLL                 BIT(10)
#define UPDATE_HWINFO_DAC_MPLL                 BIT(11)
#define FINISH_HWINFO_DAC_MPLL                 BIT(11)
#define UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW     BIT(12)
#define FINISH_HWINFO_DAC_CURSOR_IMAGE_NEW     BIT(12)
#define UPDATE_HWINFO_DAC_NVPLL                BIT(13)
#define FINISH_HWINFO_DAC_NVPLL                BIT(13)
#define UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT  BIT(14)
#define FINISH_HWINFO_DAC_IMAGE_OFFSET_FORMAT  BIT(14)
#define UPDATE_HWINFO_CODEC_FREQUENCY          BIT(0)
#define FINISH_HWINFO_CODEC_FREQUENCY          BIT(0)
#define UPDATE_HWINFO_CODEC_SOURCE             BIT(1)
#define FINISH_HWINFO_CODEC_SOURCE             BIT(1)
#define UPDATE_HWINFO_CODEC_GAIN               BIT(2)
#define FINISH_HWINFO_CODEC_GAIN               BIT(2)
#define UPDATE_HWINFO_CODEC_PIN_CONTROL        BIT(3)
#define FINISH_HWINFO_CODEC_PIN_CONTROL        BIT(3)
#define UPDATE_HWINFO_BUFFER_FLIP              BIT(0)
#define FINISH_HWINFO_BUFFER_FLIP              BIT(0)
#define UPDATE_HWINFO_BUFFER_PARAMS            BIT(8)
#define FINISH_HWINFO_BUFFER_PARAMS            BIT(8)
#define UPDATE_HWINFO_DISPLAY_PARAMS           BIT(9)
#define FINISH_HWINFO_DISPLAY_PARAMS           BIT(9)
#define UPDATE_HWINFO_REFRESH_PARAMS           BIT(10)
#define FINISH_HWINFO_REFRESH_PARAMS           BIT(10)
#define UPDATE_HWINFO_GAMEPORT                 BIT(0)
#define FINISH_HWINFO_GAMEPORT                 BIT(0)
#define UPDATE_HWINFO_GAMEPORT_START_COUNT     BIT(1)
#define FINISH_HWINFO_GAMEPORT_START_COUNT     BIT(1)
#define UPDATE_HWINFO_GAMEPORT_GET_POSITION    BIT(2)
#define FINISH_HWINFO_GAMEPORT_GET_POSITION    BIT(2)
#define UPDATE_HWINFO_FIFO_LIE                 BIT(0)
#define FINISH_HWINFO_FIFO_LIE                 BIT(0)
#define FINISH_HWINFO_VGA_FIXUP                BIT(0)
#define UPDATE_HWINFO_DGP_3D_ENABLE            BIT(0)
#define FINISH_HWINFO_DGP_3D_ENABLE            BIT(0)
#define UPDATE_HWINFO_VIDEO_ENABLE             BIT(0)
#define FINISH_HWINFO_VIDEO_ENABLE             BIT(0)
#define UPDATE_HWINFO_VIDEO_START              BIT(1)
#define FINISH_HWINFO_VIDEO_START              BIT(1)
#define UPDATE_HWINFO_VIDEO_SIZE               BIT(2)
#define FINISH_HWINFO_VIDEO_SIZE               BIT(2)
#define UPDATE_HWINFO_VIDEO_SCALE              BIT(3)
#define FINISH_HWINFO_VIDEO_SCALE              BIT(3)
#define UPDATE_HWINFO_VIDEO_COLORKEY           BIT(4)
#define FINISH_HWINFO_VIDEO_COLORKEY           BIT(4)
//
// Notification otherInfo defines.
//
#define NOTIFY_INFO16_BUFFER_CHAIN_GAP  1
#define NOTIFY_INFO16_BUFFER_YANK       2
#define NOTIFY_INFO16_BUFFER_LOOP       3

//---------------------------------------------------------------------------
//
// 32 bit debug marker values.
//
//---------------------------------------------------------------------------

// Solaris machines are byte reversed from Intel machines
#ifdef SOLARIS
#define NV_MARKER1 (U032)(('N' << 24) | ('V' << 16) | ('R' << 8) | 'M')
#define NV_MARKER2 (U032)(('M' << 24) | ('R' << 16) | ('V' << 8) | 'N')
#else
#define NV_MARKER1 (U032)(('M' << 24) | ('R' << 16) | ('V' << 8) | 'N')
#define NV_MARKER2 (U032)(('N' << 24) | ('V' << 16) | ('R' << 8) | 'M')
#endif

//---------------------------------------------------------------------------
//
// DevInfo state values.
//
//---------------------------------------------------------------------------

//
// BUS type.
//
#define BUS_PCI                         1
#define BUS_VL                          2
#define BUS_NIRV                        3
#define BUS_AGP                         4
//
// AGP defines
//
#define AGP_APERTURE_SIZE               0x2000000       // 32 MB
#define MAP_WRITECOMBINE                0x2
//
// Define the various NV chip revision ID's
//
#define NV4_REV_A_00                    0x00
#define NV4_REV_B_00                    0x10
//
// Framebuffer values.
//
#define BUFFER_DRAM                     0
#define BUFFER_SGRAM                    1
#define BUFFER_SDRAM                    2
#define BUFFER_DDRAM                    3
#define BUFFER_CRUSH_UMA                4

#define BUFFER_DRAM_EDO                 0
#define BUFFER_DRAM_FPM                 1

#define BUFFER_HSYNC_NEGATIVE           1
#define BUFFER_HSYNC_POSITIVE           0
#define BUFFER_VSYNC_NEGATIVE           1
#define BUFFER_VSYNC_POSITIVE           0
#define BUFFER_CSYNC_DISABLED           0
#define BUFFER_CSYNC_ENABLED            1
// Offscreen Instance Memory size.
//
#define NV_PRAM_MIN_SIZE_INSTANCE_MEM  0x1000   // 4KB

//
// DAC values.
//
#define DAC_CURSOR_DISABLED             0
#define DAC_CURSOR_THREE_COLOR          1
#define DAC_CURSOR_TWO_COLOR_XOR        2
#define DAC_CURSOR_TWO_COLOR            3
#define DAC_CURSOR_FULL_COLOR_XOR       4
#define DAC_CURSOR_FULL_COLOR           5
#define DAC_CURSOR_TYPE_NEW             6

#define DAC_CURSOR_CACHE_ENTRY          0x80000000
#define DAC_CURSOR_CACHE_DISPLAY        0x40000000

#define MAX_CACHED_CURSORS              5


//
// Video modes.
//
// (keep these around until all rm's os sections are updated)
//
#define NUM_VIDEO_DEPTHS                3
#define VIDEO_DEPTH_8BPP                8
#define VIDEO_DEPTH_15BPP               15
#define VIDEO_DEPTH_30BPP               30
#define NUM_RESOLUTIONS                 16
#define RESOLUTION_576X432              0
#define RESOLUTION_640X400              1
#define RESOLUTION_640X480              2
#define RESOLUTION_800X600              3
#define RESOLUTION_1024X768             4
#define RESOLUTION_1152X864             5
#define RESOLUTION_1280X1024            6
#define RESOLUTION_1600X1200            7
#define RESOLUTION_320X200              8
#define RESOLUTION_320X240              9
#define RESOLUTION_512X384              10
#define RESOLUTION_640X240              11
#define RESOLUTION_400X300              12
#define RESOLUTION_480X360              13
#define RESOLUTION_960X720              14
#define RESOLUTION_320X400              15
//
// Monitor types
//
#define NV_MONITOR_VGA                  0
#define NV_MONITOR_NTSC                 2
#define NV_MONITOR_PAL                  3

//
// Display Types
//
#define MONITOR                         0
#define TV                              1
#define DUALSURFACE                     2
#define MONITOR_FLAT_PANEL              3

//Queue depth for mobile hotkey events. Should be comfortably more than needed.
#define NV_HOTKEY_EVENT_QUEUE_SIZE      16

// 
// Customer coding (bitmasks used to enable/disable functionality)
//
#define CUSTOMER_GENERIC                0x00000000
#define CUSTOMER_CANOPUS                0x00000001
#define CUSTOMER_CANOPUS_TOSHIBA        0x00000002
#define CUSTOMER_CREATIVE               0x00000004
#define CUSTOMER_ASUS                   0x00000008       

//
// Gamport Info Analog Data Array values.
//
#define NV_GPORT_X_AXIS                 0
#define NV_GPORT_Y_AXIS                 1
#define NV_GPORT_Z_AXIS                 2
#define NV_GPORT_PEDALS                 3
#define NV_GPORT_PEDALS_1               3
#define NV_GPORT_PEDALS_2               4
#define NV_GPORT_THROTTLE               5
#define NV_GPORT_THROTTLE_1             5
#define NV_GPORT_THROTTLE_2             6
//
// Gamport source values.
//
#define GPORT_INPUT_NULL                0x0000
#define GPORT_INPUT_POT                 0x0100
#define GPORT_INPUT_JOYSTICK            0x0200
#define GPORT_INPUT_PEDALS              0x0300
#define GPORT_INPUT_THROTTLE            0x0400
#define GPORT_INPUT_PAD                 0x0500
#define GPORT_INPUT_YOKE                0x0600
#define GPORT_INPUT_POTA                0x0700
#define GPORT_INPUT_GUN                 0x0800
#define GPORT_INPUT_INVALID             0xFF00
#define GPORT_INPUT_SOURCE_MASK         0xFF00
//
// Gamport source port values.
//
#define GPORT_INPUT_PORT_PLAYER1        0
#define GPORT_INPUT_PORT_PLAYER2        1
#define GPORT_INPUT_PORT_PLAYER3        2
#define GPORT_INPUT_PORT_PLAYER4        3
#define GPORT_INPUT_PORT_PLAYER5        4
#define GPORT_INPUT_PORT_PLAYER6        5
#define GPORT_INPUT_PORT_PLAYER7        6
#define GPORT_INPUT_PORT_PLAYER8        7
#define GPORT_INPUT_PORT_PLAYER9        8
#define GPORT_INPUT_PORT_PLAYER10       9
#define GPORT_INPUT_PORT_PLAYER11       10
#define GPORT_INPUT_PORT_PLAYER12       11
#define GPORT_INPUT_PORT_POT            12
#define GPORT_INPUT_PORT_MAX            12
#define GPORT_INPUT_PORT_MASK           0x00FF
//
// Gameport source channel values. 
//
#define GPORT_INPUT_BUTTONS             BIT(0)
#define GPORT_INPUT_X_AXIS              BIT(1)
#define GPORT_INPUT_Y_AXIS              BIT(2)
#define GPORT_INPUT_Z_AXIS              BIT(3)
#define GPORT_INPUT_X_POS               BIT(4)
#define GPORT_INPUT_Y_POS               BIT(5)

//---------------------------------------------------------------------------
//
//  Max number of FIFO channels used.
//
//---------------------------------------------------------------------------

// currently only initClientInfo() uses MAX_FIFOS instead of NUM_FIFOS, since
// NUM_FIFOS may be different per device (e.g. nv4 vs. nv10). Also, the client
// database alloc occurs before an nvInfo has been allocated.
#define MAX_FIFOS                       32

//---------------------------------------------------------------------------
//
//  Data structures.
//
//---------------------------------------------------------------------------
//
// Device register access array.
//
typedef union _def_HwReg
{
    volatile V008 Reg008[1];
    volatile V016 Reg016[1];
    volatile V032 Reg032[1];
} HWREG, * PHWREG;

//
// A hardware fifo hash table entry.
//
typedef struct _def_HashTableEntry
{
    V032 ht_ObjectHandle;
    V032 ht_Context;
} HASH_TABLE_ENTRY;
         
//
// Device state and configuration information.
//
typedef struct _def_hw_info *PHWINFO;

#ifndef RM_HEADERS_MINIMAL

//
// setting RM_HEADERS_MINIMAL causes rest of file to be excluded so that
//    we don't pull in all the other include files.   Used by
//    unix version for os-interface files (so we can minimize
//    shipped include files)
//

#include "all_incs.h"

#include <nvhal.h>
#include <nvromdat.h>
#include <nvRmStereo.h>


// A software hash table entry
typedef struct _def_sw_hashTableEntry
{
    POBJECT Object;
    U032    ChID;
} SW_HASH_TABLE_ENTRY, *PSW_HASH_TABLE_ENTRY;

// System info structure
typedef struct _def_rm_info
{
    struct _def_processor_info
    {
        U032 Size;
        char ID[32];
        U032 Type;
        U032 Clock;
        PROCESSORHALINFO HalInfo;
    } Processor;

	struct _def_agp_info
	{
        struct _def_agp_nb_addr {
            U008   bus;
            U008   device;
            U008   func;
            U008   valid;
        } NBAddr;                   // bus, device, func of the NB
		U032 AGPCapPtr;             // offset of the AGP capptr in the NB
		U032 AGPPhysStart;
		VOID_PTR AGPLinearStart;
		U032 AGPHeapFree;
		VOID_PTR AGPGartBase;
		VOID_PTR AGPGartBitmap;
		U032 AGPLimit;
		U032 AGPFWEnable;
		U032 AGPFullSpeedFW;
		U032 AGPChipset;			// AGP chipset enum (NT4 AGP support)
		VOID_PTR AGPChipsetMapping; // mapping to AGP registers (NT4 AGP support)
	} AGP;

    struct _def_debugger_info
    {
        U032 Handle;                // handle of currently registered DE client
        POBJECT object;             // object data structure
    } Debugger;

} RMINFO, *PRMINFO;

typedef struct _def_hw_info
{
    struct _def_chip_info
    {
        U032 Size;
        char ID[32];
        U032 IntrEn0;
        U032 Bus;
        U032 BiosRevision;
        U032 BiosOEMRevision;
        U032 BiosRMDword;
        U032 ReqAGPRate;
#ifdef RM_STATS
        U032 ServiceCount;
        U032 ServiceTimeLo;
        U032 ServiceTimeHi;
        U032 ExceptionTimeLo;
        U032 ExceptionTimeHi;
        U032 osStartInterrupt;
#endif
        MCHALINFO HalInfo;
    } Chip;
    struct _def_mapping_info
    {
        U032 Size;
        char ID[32];
        U032 PhysAddr;
        U032 PhysFbAddr;
        U032 IntLine;
        U032 IntPin;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 nvPhys;						//***
        U032 fbPhys;						//***
        U032 nvIRQ;							//***
        // NOTE: fbLength is the size of the memory window.
        // IT DOES NOT REFLECT THE AMOUNT OF MEMORY ACTUALLY ON THE BOARD.
        // Use Framebuffer.RamSize for that.
        U032 fbLength;
        U032 nvBusDeviceFunc;
        U032 ChipID;						//***
		U032 hpicIRQ;						//***  WIN9X ONLY
        U032 hDev;							//***  WIN9X ONLY
		U032 doIOToFlushCache;				// A non zero in this field indicates that an IO needs to be done to 
											// ensure all the caches are flushed. This is currently set if we
											// detect an ALI1541 chipset or a RegistryEntry exists. This info
											// is returned in rmConfigGet.
    } Mapping;
    struct _def_pram_info
    {
        U032 Size;
        char ID[32];
        U032 FreeInstSize;
        U032 FreeInstBase;
        U032 FreeInstMax;
        U032 PrivBase;						// throw this out!
        U032 UpdateFlags;
        U032 FinishFlags;
        PRAMHALINFO HalInfo;
    } Pram;        
    struct _def_fifo_info
    {
        U032 Size;
        char ID[32];
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 UpdateFlags;
        U032 FinishFlags;
        FIFOHALINFO HalInfo;
    } Fifo;
    struct _def_master_info
    {
        U032 Size;
        char ID[32];
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 Enable;
        U032 PciNv19;   // Not Master, but there is no Config
    } Master;
    struct _def_dma_info
    {
        U032 Size;
        char ID[32];
#ifdef LEGACY_ARCH    
        U032 DmaInMemEmptyInstance;
        U032 DmaToMemEmptyInstance;
        U032 DmaFromMemEmptyInstance;
#endif // LEGACY_ARCH
        DMAHALINFO HalInfo;
    } Dma;
    struct _def_framebuffer_info
    {
        U032 Size;
        char ID[32];
        // I think these can go too
        U032 DpmLevel;
        U032 Resolution;
        U032 RefreshRateType;
        U032 HorizFrontPorch;
        U032 HorizSyncWidth;
        U032 HorizBackPorch;
        U032 VertFrontPorch;
        U032 VertSyncWidth;
        U032 VertBackPorch;
        U032 HSyncPolarity;
        U032 VSyncPolarity;
        U032 CSync;
        U032 Count;
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 Current;
        U032 FlipUsageCount;
        U032 FlipTo;
        U032 FlipFrom;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 FilterEnable;
        U032 Underscan_x;
        U032 Underscan_y;
        U032 Scale_x;               
        U032 Scale_y;
        VOID_PTR HeapHandle;
        U032 CursorAdjust;
        U032 NoAdjustedPitch;
        U032 PrimarySurfaceAlloced;
        U032 ConfigPageHeight;  // for LEGACY_ARCH
        FBHALINFO HalInfo;
    } Framebuffer;
    struct _def_graphics_info
    {
        U032 Size;
        char ID[32];
        U032 Debug0;
        U032 Debug1;
        U032 Debug2;
        U032 Debug3;
        U032 Debug4;
        U032 CurrentChID;
        U032 Enabled;
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 Capabilities;
        VOID_PTR ClassLocks;					// ptr to lock table
        GRHALINFO HalInfo;
    } Graphics;
    struct _def_video_info
    {
        U032 Size;
        char ID[32];
        U032 OverlayAllowed;
        U032 CurrentChID;
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 dwHasInited;   
        U032 class63VBlankCount[2];
        U032 FlipOnHsync;
        U032 ClassLocks;
        VIDEOHALINFO HalInfo;
    } Video;
    struct _def_mediaport_info
    {
        U032 Size;
        char ID[32];
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 ImageBufferNumber;
        VOID_PTR ImageCallback;
        VOID_PTR CurrentDecoder;
        VOID_PTR CurrentDecompressor;
        VOID_PTR CurrentParallelBusObj;
        U032 ClassLocks;
        MPHALINFO HalInfo;
    } MediaPort;
    struct _def_dac_info
    {
        U032 Size;
        char ID[32];
        U032 DpmLevel;
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 DisplayChanged;
        U032 CurrentDisplay;
        U032 DisplayPitch;
        U032 EncoderType;
        U032 EncoderID;
        U032 TVCursorMin;
        U032 TVCursorMax;
#if 0
        U032 TVformat;					   // NTSC or Pal
#endif
        U032 TVStandard;                   // NTSC or PAL
        U032 TVoutput;                     // SVideo/Composite/Autoselect
        U032 DesktopState;                 // multimon/clone/single
#define NV_DAC_DESKTOP_STATE_DEFAULT       0x0
#define NV_DAC_DESKTOP_STATE_MULTI         0x1
        U032 CursorColor1;
        U032 CursorColor2;
        U032 CursorColor3;
        U016 CursorColorImage[32][32];
        U032 Palette[272];
        U032 Gamma[272];
        U032 UpdateFlags;
        U032 CursorImageInstance;
        U016 CursorImageCache[32][32][MAX_CACHED_CURSORS]; // 32x32 words
        U032 CursorImageCacheInstance[MAX_CACHED_CURSORS];
        U032 CursorAddress[MAX_CACHED_CURSORS];
        U032 CursorMaskCRC[MAX_CACHED_CURSORS];
        U008 SavedCursor[(32/8)*32 + 32*32];
        U032 CursorCached;
        U032 CursorCacheCount;
        U032 CursorCacheEntry;
        U008 flickertoggle;                //***
        U008 CursorBitDepth; 
        U008 I2cIniFlag;
        U008 dummy2;                       //Pads for the previous byte ***
        U032 filterCountdown;              //***
        U032 CursorCacheEnable;
        U032 CRTPortID;
        U032 CRT2PortID;
        U032 TVOutPortID;
        U032 DFPPortID;                    // I2C Port to use for digital flat panel as primary
        U032 DFP2PortID;                   // I2C Port to use for DFP 2 of 2
        U032 I2CPrimaryWriteIndex;         // CRTC Index of primary I2C write port
        U032 I2CPrimaryStatusIndex;        // CRTC Index of primary I2C status port
        U032 I2CSecondaryWriteIndex;       // CRTC Index of secondary I2C write port    
        U032 I2CSecondaryStatusIndex;      // CRTC Index of secondary I2C status port    
        U032 I2CTertiaryWriteIndex;        // CRTC Index of tertiary I2C write port    
        U032 I2CTertiaryStatusIndex;       // CRTC Index of tertiary I2C status port    
        U032 HorizontalVisible;
        U032 HorizontalBlankStart;
        U032 HorizontalRetraceStart;
        U032 HorizontalRetraceEnd;
        U032 HorizontalBlankEnd;
        U032 HorizontalTotal;
        U032 VerticalVisible;
        U032 VerticalBlankStart;
        U032 VerticalRetraceStart;
        U032 VerticalRetraceEnd;
        U032 VerticalBlankEnd;
        U032 VerticalTotal;
        U032 PixelDepth;
        U032 RefreshRate;
        U032 HorizontalSyncPolarity;
        U032 VerticalSyncPolarity;
        U032 DoubleScannedMode;
        U032 fpPixClk;
        U032 fpHActive;
        U032 fpHDispEnd;
        U032 fpHValidEnd;
        U032 fpHSyncStart;
        U032 fpHSyncEnd;
        U032 fpHValidStart;
        U032 fpHTotal;
        U032 fpVActive;
        U032 fpVDispEnd;
        U032 fpVValidEnd;
        U032 fpVSyncStart;
        U032 fpVSyncEnd;
        U032 fpVValidStart;
        U032 fpVTotal;
        U032 fpNative;
        U032 fpHMax;
        U032 fpVMax;
        U032 fpFlags;
        U032 fpRelativeBrightness;	//current setting of the FP brightness
        U032 fpPWMRegisterContents; //used by PM to shut off backlight and properly restore backlight
        U032 PClkVClkRatio; // for legacy
        U032 FminVco;
        U032 FmaxVco;
        U032 MultiSyncCounter;
        DACCRTCINFO CrtcInfo[MAX_CRTCS];
        DACHALINFO HalInfo;
        DACDCBDEVDESC DCBDesc[DCB_MAX_NUM_DEVDESCS];
        DACDCBI2CRECORD DCBI2c[DCB_MAX_NUM_I2C_RECORDS];
#define DAC_CRT_OFFSET_IN_MAP       0       // bits 0..7 map to CRT0..CRT7
#define DAC_TV_OFFSET_IN_MAP        8       // bits 8..15 map to TV0..TV7
#define DAC_DD_OFFSET_IN_MAP        16      // bits 16..23 map to DD0..DD7
        U032 DevicesBitMap;
        U032 BootDevicesBitMap;
        U032 DevicesNoCheck;
    } Dac;

#ifdef LEGACY_ARCH    
    struct _def_dgp_chip_info
    {
        U032 Size;
        char ID[32];
        U032 Chip;
        U032 Enable3D;
        U032 Scale;
        U032 HorizDelay;
        U032 HorizOffset;
        U032 HorizScale;
        U032 VertOffset;
        U032 VertScale;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Dgp;
#endif // LEGACY_ARCH

    struct _def_timer_info
    {
        U032 Size;
        char ID[32];
        U032 Denominator;
        U032 Numerator;
#ifdef RM_STATS
        U032 ServiceCount;
#endif
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 tmrSave[2];          //***
    } Timer;
    struct _def_vga_info
    {
        U032 Size;
        char ID[32];
        U032 Enabled;
        U032 DOSBox;
#ifdef NV_VGA
        U032 GlobalEnable;
        U032 RomVersion;
        U032 RomFixupCode;
        U032 UpdateFlags;
        U032 FinishFlags;
#endif // NV_VGA
    } Vga;
    struct _def_classes_info
    {
        U032 Size;
        char ID[32];
        U032 UpdateFlags;
        U032 FinishFlags;
    } Classes;
#ifdef RM_STATS
    struct _def_stat_info
    {
        U032 Size;
        char ID[32];
        U032 AudioExceptionCount;
        U032 FifoExceptionCount;
        U032 FramebufferExceptionCount;
        U032 GraphicsExceptionCount;
        U032 TimerExceptionCount;
        U032 TotalExceptionCount;
        U032 IntTimeLo;
        U032 IntTimeHi;
        U032 ExecTimeLo;
        U032 ExecTimeHi;
        U032 MemAlloced;
        U032 MemLocked;
        U032 UpdateLo;
        U032 UpdateHi;
    } Statistics;
#endif
    struct _def_power_info
    {
        U032 State;
        U032 strapInfo;
        U032 coeff_select;
        U032 nvpll;
        U032 mpll;
        U032 vpll;
        U032 PbusDebug1;
        U032 PbusDebug3;
        U032 MobileOperation;
        U032 PostNVCLK;
        U032 IdleNVCLK;
        U032 PostMCLK;
        U032 IdleMCLK;
        // Chip-dependent power data moved to MC section.
    } Power;
    struct _def_registry_info
    {
        U032 Size;
        char ID[32];
        char DBstrDevNodeDisplay[128];
        char DBstrDevNodeDisplayNumber[128];
    } Registry;

    struct _def_hotkey_info
    {
//        U032 reg4CShadow;
        BOOL enabled;
        U032 queueHead;
        U032 queueTail;
        struct _def_hotkey_queue
        {
            U032 event;
            U032 status;
        } Queue[NV_HOTKEY_EVENT_QUEUE_SIZE];
    } HotKeys;

    U016				  filler1;
    U008				  filler2;

    // NOTE: This filler above fixed a really strange problem. We don't know why it
    // fixed the problem. Without this filler, phantom drive letters would appear!
    // See bug # 19981209-132200. We also don't know how a misalignment here causes
    // the registry to go bad.

    U008                DeviceDisabled;
    PSW_HASH_TABLE_ENTRY DBhashTable;                 //***
    U008                *DBfbInstBitmap;              //***
    PHWREG              DBnvAddr;                     //***
    PHWREG              DBromAddr;                    //***
    PHWREG              DBbiosAddr;                   //***
    PHWREG              DBfbAddr;                     //***
    PHWREG              DBagpAddr;                    //***
    PHWREG              DBimAddr;                     //***                               
    PFIFO               DBfifoTable;                  //***
    PDMAOBJECT          DBDmaList;                    //***
    PDMAOBJECT          DBVidDmaObject;               //***
    PVBLANKNOTIFY       DBgrVBlankList[2];            //***
    PTIMERCALLBACK      DBtmrCallbackList;            //***
    PTIMERCALLBACK      DBtmrCallbackFreeList;        //***
    PTIMERCALLBACK      DBtmrCallbackTable;           //***
    PVBLANKNOTIFY       DBclass63VBlankList[2];
    POVERLAYSERVICEPROC pOverlayServiceHandler;
    POBJECT             DBpHardwareObject;
    U032                (*DBdisplayModeHook)(U032);
    SURFACEATTRIBUTES   DBOverlaySurfaceAttribs[20];
    SURFACEATTRIBUTES   DBImageSurfaceAttribs[20];
    U032                *fb_save;
    PVIDEOCOLORMAPOBJECT colormapObjectToNotify;

    ENGINEDB            engineDB;                   // HAL-derived engine info
    CLASSDB             classDB;                    // HAL-derived class info

#if 0
    PCLASS              pClasses;
    U032                numClasses;
    PENGINEINFO         pEngineInfos;

    PCLASSDESCRIPTOR    pClassDescriptors;
    U032                numClassDescriptors;
    PENGINEDESCRIPTOR   pEngineDescriptors;
    U032                numEngineDescriptors;
#endif

    HALHWINFO           halHwInfo;
    VOID_PTR            pHalEngineMem;
    PHALFUNCS           pHalFuncs;

    U008               *EDIDBuffer;
    U032                I2CAcquired;

    U032                DBdevInstance;
    U032                biosOffset;                   // has bios been relocated within rom?

    VOID_PTR			  pRmInfo;						// pointer to our system information (RMINFO NvRmInfo)
    VOID_PTR			  pOsHwInfo;					// pointer to our OS dependant information

    // Can't move this to WINNTHWINFO currently because of a hack in state.c
#ifdef NTRM
    VOID*				  DBmpDeviceExtension;			// miniport device extension
#endif

    // These will be moved to the MACOS9HWINFO
#ifdef MACOS
    BOOL                DBrmInitialized;              // per device init flag
    VOID*               DBmpDeviceExtension;          // os's device extension
#endif

#ifdef STEREO_SUPPORT
    //Stereo stuff
    LPNV_CFG_STEREO_PARAMS	pStereoParams;
#endif //STEREO_SUPPORT

} HWINFO;

// This macro will produce a divide by 0 error at compile time if the sizeof HWINFO
// struct is not a multiple of 4. However, it does not help if some sub struct of
// HWINFO is not aligned properly.
#define ASSERT_NVINFO_SIZE if (1/((sizeof(HWINFO) % 4) ? 0 : 1))

#ifndef JV
#ifndef LEGACY_ARCH
#define MAX_INSTANCE 8
#define TEST_DEVICE_NUM 0
#else
#define MAX_INSTANCE 1
#endif
extern  U032   ClientInstance;
extern  PHWINFO NvDBPtr_Table[MAX_INSTANCE];
extern	RMINFO	NvRmInfo;
#endif // JV

// nv10 needs 2 dwords, but nv4 only needs one, so we'll rename one of the
// fields to be the nv4 define
#define VideoScale                   VideoScaleX
#define Buff0VideoScale              Buff0VideoScaleX
#define Buff1VideoScale              Buff1VideoScaleX
#define nvAddr                       pDev->DBnvAddr 
#define romAddr                      pDev->DBromAddr
#define biosAddr                     pDev->DBbiosAddr
#define fbAddr                       pDev->DBfbAddr 
#define CurDacAdr(Head)              GETCRTCHALINFO(pDev,Head,RegOffset)
#define NUM_FIFOS                    pDev->Fifo.HalInfo.Count
#define devInstance                  pDev->DBdevInstance

// Things that were moved from nvarch.h
#define NV10_CHIP_ARCHITECTURE          0x10

// HW description
// TO DO: change this to DEVICE_EXTENT of instance memory so that we will ALWAYS map enuff memory
#define NV_DEVICE_SIZE                   0x740000		// 7M of regs + 256K of instance mem (first 64K is BIOS)
#define INSTANCE_MEMORY_BASE             (pDev->Mapping.PhysAddr + pDev->Pram.HalInfo.PraminOffset)
#define INSTANCE_MEMORY_PHYSICAL_BASE(d) ((d)->Mapping.PhysAddr + pDev->Pram.HalInfo.PraminOffset)
#define INSTANCE_MEMORY_LOGICAL_BASE(d)  ((U008*)(d)->DBnvAddr + pDev->Pram.HalInfo.PraminOffset)
#define INSTANCE_MEMORY_SIZE             0x40000        // 256K
#define ROM_SIZE                         0x8000
#define CHANNEL_LENGTH                   0x10000        // 64K
#define INT_A                            0x01
#define FIFO_USER_BASE(d)                (d->Fifo.HalInfo.UserBase)

#endif // RM_HEADERS_MINIMAL
#endif // _NVRM_H_
