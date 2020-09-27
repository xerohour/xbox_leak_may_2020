#ifndef _NVRM_H_
#define _NVRM_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: NVRM.H                                                            *
*       Resource Manager defines and structures used throughout the code.   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    11/22/93 - wrote it.                    *
*       David Schmenk (dschmenk)    07/06/94 - rewrote it.                  *
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

#ifdef NTRM
#include <nvtypes.h>
#endif

#ifndef NVTYPES_INCLUDED
typedef unsigned char   U008;
typedef unsigned short  U016;
typedef unsigned long   U032;
typedef double          U064;

typedef unsigned char   V008;
typedef unsigned short  V016;
typedef unsigned long   V032;
typedef double          V064;

typedef signed char     S008;
typedef signed short    S016;
typedef signed long     S032;
typedef double          S064;
#endif
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
#define TRUE            ~0
#endif
#ifndef FALSE
#define FALSE           0
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
#define INST_WR32(i,o,d)        MEM_WR32(DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o),(d))
#define INST_RD32(i,o)          MEM_RD32(DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o))
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
// Predefined GART offset in physical memory
//
#define AGP_APERTURE_SIZE               0x2000000       // 32 MB
#define AGP_APERTURE_PHYS_BASE          0xC0000000      // 3 gig
//
// Define the various NV chip ID's
//
#define NV3_REV_A_00                    0x00
#define NV3_REV_B_00                    0x10
#define NV3_REV_C_00                    0x20
//
// SEGA Game Port "DAC Hack" Gate Array ID's.
//
#define DGP_SGP                         1
#define DGP_ASGP                        2
//
// Define CODEC BIOS ID's
//
#define CODEC_AD1848                    0x00000000
#define CODEC_AD1845                    0x00000001
#define CODEC_AD1846                    0x00000002
#define CODEC_CS4248                    0x00000003
#define CODEC_CS4231A                   0x00000004
#define CODEC_ESS1688                   0x00000005
#define CODEC_ESS1788                   0x00000006
#define CODEC_ESS1888                   0x00000007
#define CODEC_ESS1868                   0x00000008
#define CODEC_ADREDCORAL                0x00000009
#define CODEC_OPTI                      0x0000000A
#define CODEC_YAC516                    0x0000000B
#define CODEC_CHRONTEL                  0x0000000C
#define CODEC_YAC701                    0x0000000D
#define CODEC_ID_MASK                   0x00003FFF
#define CODEC_NONE                      0xFFFFFFFF
//
// Define CODEC BIOS Frequency ID's.
//
#define CODEC_FREQ_11KHz                0x00000000
#define CODEC_FREQ_22KHz                0x00004000
#define CODEC_FREQ_44KHz                0x00008000
#define CODEC_FREQ_MASK                 0x0000C000
//
// Define the ever changing CODEC Interface Protocol values.
//
#define CODEC_IP_AD                     0x30
#define CODEC_IP_YAMAHA                 0x40
#define CODEC_IP_ESS                    0xC8
//
// CODEC values.
//
#define CODEC_22KHz                     0x56220000
#define CODEC_44KHz                     0xAC440000 
#define CODEC_48KHz                     0xBB800000
#define NUM_CODEC_INPUTS                4
#define CODEC_INPUT_LINE                0  
#define CODEC_INPUT_MIC                 1  
#define CODEC_INPUT_AUX                 2
#define CODEC_INPUT_AUX1                2
#define CODEC_INPUT_MIX                 3
#define CODEC_INPUT_AUX2                3
#define CODEC_PASSTHRU                  4
#define CODEC_INPUT_GAIN_0DB            0
#define CODEC_INPUT_GAIN_20DB           20
#define CODEC_OUTPUT_ATTEN_0DB          0
#define CODEC_OUTPUT_ATTEN_20DB         20
//
// Framebuffer values.
//
#define BUFFER_DRAM                     0
#define BUFFER_SGRAM                    1
#define BUFFER_DRAM_EDO                 0
#define BUFFER_DRAM_FPM                 1
#define BUFFER_HSYNC_NEGATIVE           0
#define BUFFER_HSYNC_POSITIVE           1
#define BUFFER_VSYNC_NEGATIVE           0
#define BUFFER_VSYNC_POSITIVE           1
#define BUFFER_CSYNC_DISABLED           0
#define BUFFER_CSYNC_ENABLED            1
//
// Dedicated privileged device ram size in bytes.
//
#define NV_PRAM_DEVICE_SIZE_12_KBYTES   0x3000  // 12KB
#define NV_PRAM_DEVICE_SIZE_20_KBYTES  0x5000   // 20KB
#define NV_PRAM_DEVICE_SIZE_36_KBYTES  0x9000   // 36KB             
#define NV_PRAM_DEVICE_SIZE_68_KBYTES  0x11000  // 68KB
//
// NV3's priviliged size, minus audio
//
#define NV_PRAM_DEVICE_SIZE_5_5_KBYTES   0x1600 // 5.5KB
#define NV_PRAM_DEVICE_SIZE_9_5_KBYTES   0x2600 // 9.5KB
#define NV_PRAM_DEVICE_SIZE_17_5_KBYTES  0x4600 // 17.5KB             
#define NV_PRAM_DEVICE_SIZE_33_5_KBYTES  0x8600 // 33.5KB
//
// Offscreen Instance Memory size.
//
#define NV_PRAM_MIN_SIZE_INSTANCE_MEM  0x1000   // 4KB
//
// New states for the SaveToEEPROMEnable flag in nvinfo.framebuffer
//
#define NV_CFG_DISABLE_NON_VOLATILE_STORAGE     0
#define NV_CFG_ENABLE_NON_VOLATILE_STORAGE      0x042
//
// DAC values.
//
#define DAC_CURSOR_DISABLED             0
#define DAC_CURSOR_THREE_COLOR          1
#define DAC_CURSOR_TWO_COLOR_XOR        2
#define DAC_CURSOR_TWO_COLOR            3
#define DAC_CURSOR_FULL_COLOR_XOR       4
#define DAC_CURSOR_FULL_COLOR           5

#define DAC_CURSOR_CACHE_ENTRY          0x80000000
#define DAC_CURSOR_CACHE_DISPLAY        0x40000000

#define MAX_CACHED_CURSORS              5

// For attempting to do Frame locking...

//#define FRAME_LOCK

#ifdef FRAME_LOCK
#define MAXROCKS    (256)
#define LOCKPARAM1  (18)
#define LOCKPARAM2  (4)
#define LOCKLOCATION (50)
#endif //FRAME_LOCK


#define NV3WDMHACK


#ifdef NV3WDMHACK

#define NVWDMHACKDELAY  (45)

#endif // NV3WDMHACK

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

// Use the following for monitor types. The ones above are obsolete names.
#define MONITOR_TYPE_VGA                0
#define MONITOR_TYPE_NTSC               2
#define MONITOR_TYPE_PAL                3
#define MONITOR_TYPE_FLAT_PANEL         4

// Use the following for display types. The ones above are obsolete names.
#define DISPLAY_TYPE_MONITOR            0
#define DISPLAY_TYPE_TV                 1
#define DISPLAY_TYPE_DUALSURFACE        2
#define DISPLAY_TYPE_FLAT_PANEL         3


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

#ifdef RM_MUT
#define NUM_FIFOS                       4
#else
#define NUM_FIFOS                       32
#endif

//---------------------------------------------------------------------------
//
//  Data structures.
//
//---------------------------------------------------------------------------

#include "all_incs.h"

// JJV-MM This is messy ... but it beats including nvhw.h
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
typedef struct _def_hw_info
{
    struct _def_chip_info
    {
        U032 Size;
        char ID[32];
        U032 SavePmc;
        U032 SaveIntrEn0;
        U032 IntrEn0;
        U032 Implementation;
        U032 Revision;
        U032 Architecture;
        U032 Manufacturer;
        U032 Bus;
        U032 NVAGPEnabled;
        U032 CrystalFreq;
        U032 ChipToken[2];
        U032 ServiceCount;
        U032 ServiceTimeLo;
        U032 ServiceTimeHi;
        U032 ExceptionTimeLo;
        U032 ExceptionTimeHi;
        U032 osStartInterrupt;
    } Chip;
    struct _def_mapping_info
    {
        U032 Size;
        char ID[32];
        U032 PhysAddr;
        U032 PhysFbAddr;
        U032 IntLine;
        U032 IntPin;
        U032 AGPPhysStart;
        U032 AGPLinearStart;
        U032 AGPLimit;
        U032 UpdateFlags;
        U032 FinishFlags;
        //PHWREG nvAddr;                   //***
        //PHWREG romAddr;                  //***
        //PHWREG fbAddr;                   //***
        //PHWREG agpAddr;                  //***
        U032 nvPhys;                       //***
        U032 fbPhys;                       //***
        U032 nvIRQ;                        //***
        U032 dllIpcOffset;                 //***
        U032 dllIpcSelector;               //***
        U032 dllNotifyOffset;              //***
        U032 dllNotifySelector;            //***
                                           //***
        U032 ChipID;                       //***
        //U032 *ApertureTranslationTable;  //***
        U032 hpicIRQ;                      //***
        U032 hDev;                         //***
        U032 osDeviceHandle;               // JJV - This may be the same as the one above ???
    } Mapping;
    struct _def_pram_info
    {
        U032 Size;
        char ID[32];
        U032 ConfigReg;
        U032 HashDepth;
        U032 RunOutMask;
        U032 FreeInstSize;
        U032 FreeInstBase;
        U032 FreeInstMax;
        U032 CurrentSize;
        U032 AvailableSize[4];
        U032 PrivBase;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 HashTableAddr;
        U032 FifoRunoutAddr;
        U032 FifoContextAddr;
        U032 AudioScratchAddr;
        U032 AuthScratchAddr;
        U032 VideoDmaInstance;
    } Pram;        
    struct _def_dma_info
    {
    
        //PDMAOBJECT DmaList;              //***
        U032       dmaPushPutOffset;       //***
        U032       dmaPushGetOffset;       //***
        U032       dmaPushGetOffsetRead;   //***
        U032       dmaPushJumpOffset;      //***
        U032       dmaPushJumpOffsetSet;   //***
        //PDMAOBJECT VidDmaObject;         //***
    } Dma;        
    struct _def_fifo_info
    {
        U032 Size;
        char ID[32];
        U032 ServiceCount;
        U032 AllocateCount;
        U032 CacheDepth;
        U032 LieDepth;
        U032 RetryCount;
        U032 Count;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 DmaFetchAmount;       // Burst amount the DMA pusher should use
                                   // when reading into the FIFO
        U032 DmaWaterMark;         // Fifo level which will kick of the next
                                   // DMA burst
        U032 CurrentChID;
        //PFIFO   fifoTable;               //***
        U032    fifoAllocateCount;         //***
        //POBJECT *hashTable;              //***
    } Fifo;
    struct _def_master_info
    {
        U032 Size;
        char ID[32];
        U032 UpdateFlags;
        U032 FinishFlags;
    } Master;
    struct _def_framebuffer_info
    {
        U032 Size;
        char ID[32];
        U032 ConfigReg;
        U032 ConfigPageHeight;
        U032 RamSizeMb;
        U032 RamSize;
        U032 RamType;
        U032 RamTech;
        U032 DpmLevel;
        U032 Depth;
        U032 Resolution;
        U032 RefreshRate;
        U032 HorizFrontPorch;
        U032 HorizSyncWidth;
        U032 HorizBackPorch;
        U032 HorizDisplayWidth;
        U032 VertFrontPorch;
        U032 VertSyncWidth;
        U032 VertBackPorch;
        U032 VertDisplayWidth;
        U032 HSyncPolarity;
        U032 VSyncPolarity;
        U032 CSync;
        U032 Count;
        U032 ServiceCount;
        U032 ActiveCount;
        U032 ActiveMask;
        U032 Start[4];
        U032 Limit[4];
        U032 Current;
        U032 FlipUsageCount;
        U032 FlipTo;
        U032 FlipFrom;
        U032 VBlankToggle;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 MonitorType;
        U032 FilterEnable;
        U032 SaveToEEPROMEnable;
        U032 Underscan_x;
        U032 Underscan_y;
        U032 Scale_x;               
        U032 Scale_y;
        U032 Instance;
        U032 DmaInstance;
        //U008 *fbInstBitmap;              //***
        U032 fbSave[4];                    //***
    } Framebuffer;
    struct _def_graphics_info
    {
        U032 Size;
        char ID[32];
        U032 Debug0;
        U032 Debug1;
        U032 Debug2;
        U032 Debug3;
        U032 CurrentChID;
        U032 Count;
        U032 CanvasCount;
        U032 Enabled;
        U032 ServiceCount;
        U032 ComplexClipCount;
        U032 UpdateFlags;
        U032 FinishFlags;
        //PGRAPHICSCHANNEL  grTable;         //***
        //PCANVAS           grSysCanvas;     //***
        //PNODE             grCanvasTree;    //***
        //PVBLANKNOTIFY     grVBlankList[2]; //***
        U032              grSave[36];        //***
    } Graphics;
    struct _def_video_info
    {
        U032 Size;
        char ID[32];
        U032 CurrentChID;
        U032 ColorKeyEnabled;
        U032 ScaleFactor;   // 12.20
        U032 Enabled;
        U032 VideoStart;
        U032 VideoSize;
        U032 VideoScale;
        U032 VideoColorKey;
        U032 UpdateFlags;
        U032 FinishFlags;
#ifdef FRAME_LOCK
        // the rock under which we store information to try to genlock things
        U032 dwStartIt;
        S032 dwPhaseError;
        S032 dwlocError;
        U032 dwRockIndex;
        S032 dwRockStorage[MAXROCKS];
        U032 dwRockStorage2[MAXROCKS];
        U032 dwLastIt;
        
        S032 oldRaw;
        S032 oldLow;
        S032 oldReject;
        S032 oldOutput;
#endif // FRAME_LOCK

#ifdef  NV3WDMHACK
        // there are four sets of these, two each for Image and VBI
        U032                pVidToMemDelayParam0;
        U032                pVidToMemDelayParam1;
        U032                pVidToMemDelayParam2;
        U032                pVidToMemDelayParam3;
        U032                dwDelayCounter0;        // zero indicates not running, 1 starts running, N completes
        U032                dwDelayCounter1;        
        U032                dwDelayCounter2;        
        U032                dwDelayCounter3;    
        U032                dwHasInited;    
#endif //NV3WDMHACK
    } Video;
    struct _def_mediaport_info
    {
        U032 Size;
        char ID[32];
        U032 CurrentChID;
        U032 Enabled;
        U032 MPCPresent;
        //MEDIAPORTCONTEXT  mpContext[NUM_FIFOS];  //***
    } MediaPort;
    struct _def_dac_info
    {
        U032 Size;
        char ID[32];
        U032 InputWidth;
        U032 PClkVClkRatio;
        U032 VClk;
        U032 MClk;
        U032 PClk;
        U032 VPllM;
        U032 VPllN;
        U032 VPllO;
        U032 VPllP;
        U032 MPllM;
        U032 MPllN;
        U032 MPllO;
        U032 MPllP;
        U032 DpmLevel;
        S032 CursorPosX;
        S032 CursorPosY;
        U032 TVCursorMin;
        U032 TVCursorMax;
        U032 CursorWidth;
        U032 CursorHeight;
        U008 CursorImagePlane[2][32 * sizeof(U032)];
        U032 CursorType;
        U032 CursorColor1;
        U032 CursorColor2;
        U032 CursorColor3;
        U016 CursorColorImage[32][32];
#ifdef SW_CURSOR
        U008 CursorColorImage[32][32 * sizeof(U032)];
        S032 CursorSaveUnderPosX;
        S032 CursorSaveUnderPosY;
        U032 CursorSaveUnderWidth;
        U032 CursorSaveUnderHeight;
        U032 CursorSaveUnder[32][32];
        U032 CursorExclude;
        U032 CursorEmulation;
#endif // SW_CURSOR
        U032 Palette[272];
        U032 Gamma[272];
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 CursorImageInstance;
        U016 CursorImageCache[32][32][MAX_CACHED_CURSORS]; // 32x32 words
        U032 CursorImageCacheInstance[MAX_CACHED_CURSORS];
        U032 CursorAddress[MAX_CACHED_CURSORS];
        // JJV - Cached cursor sum values.
        U032 CursorMaskCRC[MAX_CACHED_CURSORS];
        // JJV - Saved cursor for 8bpp.
        U008 SavedCursor[(32/8)*32 + 32*32];
        U032 CursorCachedExtra;
        U032 CursorCached;
        U032 CursorCacheCount;
        U032 CursorCacheEntry;
        U008 flickertoggle;                //***
        U008 dummy0;                       //Pads for the previous byte ***
        U008 dummy1;                       //Pads for the previous byte ***
        U008 dummy2;                       //Pads for the previous byte ***
        U032 filterCountdown;              //***
#ifdef NV3WDMHACK
        U032 dwImageCtxCount;
#endif
    } Dac;
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
    struct _def_timer_info
    {
        U032 Size;
        char ID[32];
        U032 Denominator;
        U032 Numerator;
        U032 ServiceCount;
        U032 UpdateFlags;
        U032 FinishFlags;
        //PTIMERCALLBACK    tmrCallbackList;     //***
        //PTIMERCALLBACK    tmrCallbackFreeList; //***
        //PTIMERCALLBACK    tmrCallbackTable;    //***
        U032              tmrSave[2];          //***
    } Timer;
    struct _def_vga_info
    {
        U032 Size;
        char ID[32];
        U032 Enabled;
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
    struct _def_null_info
    {
        U032 Size;
        char ID[32];
    } Null;
    struct _def_obsolete_info
    {
      //U032  (*displayModeHook)(U032);   
      //SURFACEATTRIBUTES OverlaySurfaceAttribs[20];
      //SURFACEATTRIBUTES ImageSurfaceAttribs[20];
      //PVBLANKNOTIFY     class63VBlankList[2];
      U032              class63VBlankCount[2];
      
      U032              dwOvMaxBuffer;
      //PVIDEOFROMMEMOBJECT class63ActiveVideo[1];
      U032              dwSchedule_1_ToSurfaceEven;
      U032              dwSchedule_1_ToSurfaceOdd; 
      U032              dwSchedule_0_ToSurfaceEven;
      U032              dwSchedule_0_ToSurfaceOdd;
      //POBJECT           pHardwareObject;
      U032              global_dwVideoFormat;
      U032              dwDigitiserType;
      U032              dwImageCtxCount;
      U032              dwVbiCtxCount;
      U032              FLIP_OVERLAY_SURFACE_0;
      U032              FLIP_OVERLAY_SURFACE_1;
      U032              dwMasterSkipPattern;
      U032              dwTempSkipPattern;
      U032              dwMasterSkipBits;
      U032              dwIndexSkipBits;
      U032              dwImage0Skipped;
      U032              dwImage1Skipped;
      U032              dwOldLength0;
      U032              dwOldLength1;
      U032              dwXres; 
      U032              dwYres; 
      U032              dwIntMasterIndex; // used for interleave mode only
      U032              dwIntBuff0Index;
      U032              dwIntBuff1Index;
      U032              dwMaxBuffers;     // this will be changed for multiple buffer
      U032              dwVbiBufferNumber;
      U032              dwImageBufferNumber;
      //PPARALLELBUSOBJECT pNotifyObject;
      U008              iniFlag;
      U008              dummy3;
      U008              dummy4;
      U008              dummy5;
     #ifdef DEBUG
      U032              dwActiveObjects;
      U032              dwCreatedObjects;
      U032              dwDormantObjects;
      U032              dwVmiInterrupts;
      U032              dwImageInterrupts;
      U032              dwVbiInterrupts;
     #endif   
    }  Obsolete; 
} HWINFO, * PHWINFO;

typedef struct _def_rmInstance_info  *PNvDB, NvDB;

// This is the Master Structure for all of the system globals.
struct _def_rmInstance_info
{
  HWINFO              DBnvInfo;
  U032                *DBApertureTranslationTable;  //***
  POBJECT             *DBhashTable;                 //***
  U008                *DBfbInstBitmap;              //***
  PHWREG              DBnvAddr;                     //***
  PHWREG              DBromAddr;                    //***
  PHWREG              DBfbAddr;                     //***
  PHWREG              DBagpAddr;                    //***
  PHWREG              DBimAddr;                     //***
                                                    
  PFIFO               DBfifoTable;                  //***
  PDMAOBJECT          DBDmaList;                    //***
  PDMAOBJECT          DBVidDmaObject;               //***
  PGRAPHICSCHANNEL    DBgrTable;                    //***
  PCANVAS             DBgrSysCanvas;                //***
  PNODE               DBgrCanvasTree;               //***
  PVBLANKNOTIFY       DBgrVBlankList[2];            //***
  PTIMERCALLBACK      DBtmrCallbackList;            //***
  PTIMERCALLBACK      DBtmrCallbackFreeList;        //***
  PTIMERCALLBACK      DBtmrCallbackTable;           //***
  PVBLANKNOTIFY       DBclass63VBlankList[2];
  PVIDEOFROMMEMOBJECT DBclass63ActiveVideo[1];
  POBJECT             DBpHardwareObject;
  PPARALLELBUSOBJECT  DBpNotifyObject;
  U032                (*DBdisplayModeHook)(U032);   
  MEDIAPORTCONTEXT    DBmpContext[NUM_FIFOS];       //***
  SURFACEATTRIBUTES   DBOverlaySurfaceAttribs[20];
  SURFACEATTRIBUTES   DBImageSurfaceAttribs[20];
} ;

#ifndef JV
#define MAX_INSTANCE 8
#define TEST_DEVICE_NUM 0
extern  int   DeviceInstance;
extern  PNvDB NvDBPtr_Table[MAX_INSTANCE];
#endif // JV

// Multiple instance macros.
// This is on the Ugly side at the moment (Should be cleaned up soon)

//#define nvAddr                      NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.nvAddr
//#define romAddr                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.romAddr                  
//#define fbAddr                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.fbAddr                   
//#define agpAddr                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.agpAddr                  

#define nvPhys                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.nvPhys                   
#define fbPhys                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.fbPhys                   
#define nvIRQ                     NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.nvIRQ                    
#define dllIpcOffset              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.dllIpcOffset             
#define dllIpcSelector            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.dllIpcSelector           
#define dllNotifyOffset           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.dllNotifyOffset          
#define dllNotifySelector         NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.dllNotifySelector        

//#define nvInfoSelector            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.nvInfoSelector           
//#define channelInfoSelector       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.channelInfoSelector      
//#define clientInfoSelector        NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.clientInfoSelector       

#define ChipID                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.ChipID                   
//#define ApertureTranslationTable  NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.ApertureTranslationTable
#define hpicIRQ                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.hpicIRQ                  
#define hDev                      NvDBPtr_Table[DeviceInstance]->DBnvInfo.Mapping.hDev                  

#define osStartInterrupt          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Chip.osStartInterrupt

//#define fifoTable                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.Fifo.fifoTable
#define fifoAllocateCount         NvDBPtr_Table[DeviceInstance]->DBnvInfo.Fifo.fifoAllocateCount
//#define hashTable                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.Fifo.hashTable
                                  
//#define mpContext                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.MediaPort.mpContext
                                  
//#define grTable                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Graphics.grTable     
//#define grSysCanvas               NvDBPtr_Table[DeviceInstance]->DBnvInfo.Graphics.grSysCanvas 
//#define grCanvasTree              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Graphics.grCanvasTree
//#define grVBlankList              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Graphics.grVBlankList
#define grSave                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Graphics.grSave      

//#define fbInstBitmap              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Framebuffer.fbInstBitmap                    
#define fbSave                    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Framebuffer.fbSave   

//#define tmrCallbackList           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Timer.tmrCallbackList    
//#define tmrCallbackFreeList       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Timer.tmrCallbackFreeList
//#define tmrCallbackTable          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Timer.tmrCallbackTable   
#define tmrSave                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Timer.tmrSave            
                                  
//#define DmaList                   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.DmaList
#define dmaPushPutOffset          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.dmaPushPutOffset
#define dmaPushGetOffset          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.dmaPushGetOffset
#define dmaPushGetOffsetRead      NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.dmaPushGetOffsetRead
#define dmaPushJumpOffset         NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.dmaPushJumpOffset
#define dmaPushJumpOffsetSet      NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.dmaPushJumpOffsetSet        
//#define VidDmaObject              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dma.VidDmaObject                    
                                  
#define flickertoggle             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dac.flickertoggle  
#define filterCountdown           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dac.filterCountdown
#ifdef NV3WDMHACK
#define dwImageCtxCount           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Dac.dwImageCtxCount
#endif
                                  
//#define displayModeHook             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.displayModeHook
//#define OverlaySurfaceAttribs        NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.OverlaySurfaceAttribs      
//#define ImageSurfaceAttribs          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.ImageSurfaceAttribs        

//#define class63VBlankList            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.class63VBlankList          

#define class63VBlankCount           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.class63VBlankCount         
#define dwOvMaxBuffer                NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwOvMaxBuffer

//#define class63ActiveVideo           NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.class63ActiveVideo         

#define dwSchedule_1_ToSurfaceEven   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwSchedule_1_ToSurfaceEven 
#define dwSchedule_1_ToSurfaceOdd    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwSchedule_1_ToSurfaceOdd  
#define dwSchedule_0_ToSurfaceEven   NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwSchedule_0_ToSurfaceEven 
#define dwSchedule_0_ToSurfaceOdd    NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwSchedule_0_ToSurfaceOdd  
     #ifdef DEBUG                                                                   
#define dwActiveObjects              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwActiveObjects            
#define dwCreatedObjects             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwCreatedObjects           
#define dwDormantObjects             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwDormantObjects           
#define dwVmiInterrupts              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwVmiInterrupts            
#define dwImageInterrupts            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwImageInterrupts          
#define dwVbiInterrupts              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwVbiInterrupts            
     #endif                                                                         
//#define pHardwareObject              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.pHardwareObject            

#define global_dwVideoFormat         NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.global_dwVideoFormat
#define dwDigitiserType              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwDigitiserType            
#define dwImageCtxCount              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwImageCtxCount            
#define dwVbiCtxCount                NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwVbiCtxCount              
#define FLIP_OVERLAY_SURFACE_0       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.FLIP_OVERLAY_SURFACE_0     
#define FLIP_OVERLAY_SURFACE_1       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.FLIP_OVERLAY_SURFACE_1     
#define dwMasterSkipPattern          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwMasterSkipPattern        
#define dwTempSkipPattern            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwTempSkipPattern          
#define dwMasterSkipBits             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwMasterSkipBits           
#define dwIndexSkipBits              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwIndexSkipBits            
#define dwImage0Skipped              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwImage0Skipped            
#define dwImage1Skipped              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwImage1Skipped            
#define dwOldLength0                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwOldLength0               
#define dwOldLength1                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwOldLength1               
#define dwXres                       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwXres                     
#define dwYres                       NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwYres                     
#define dwIntMasterIndex             NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwIntMasterIndex           
#define dwIntBuff0Index              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwIntBuff0Index            
#define dwIntBuff1Index              NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwIntBuff1Index            
#define dwMaxBuffers                 NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwMaxBuffers               
#define dwVbiBufferNumber            NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwVbiBufferNumber          
#define dwImageBufferNumber          NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.dwImageBufferNumber        

//#define pNotifyObject                NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.pNotifyObject              
#define iniFlag                      NvDBPtr_Table[DeviceInstance]->DBnvInfo.Obsolete.iniFlag                    

#define OverlaySurfaceAttribs        NvDBPtr_Table[DeviceInstance]->DBOverlaySurfaceAttribs
#define ImageSurfaceAttribs          NvDBPtr_Table[DeviceInstance]->DBImageSurfaceAttribs

#define mpContext                    NvDBPtr_Table[DeviceInstance]->DBmpContext

#define displayModeHook              NvDBPtr_Table[DeviceInstance]->DBdisplayModeHook
#define fifoTable                    NvDBPtr_Table[DeviceInstance]->DBfifoTable
#define DmaList                      NvDBPtr_Table[DeviceInstance]->DBDmaList
#define VidDmaObject                 NvDBPtr_Table[DeviceInstance]->DBVidDmaObject
#define grTable                      NvDBPtr_Table[DeviceInstance]->DBgrTable
#define grSysCanvas                  NvDBPtr_Table[DeviceInstance]->DBgrSysCanvas
#define grCanvasTree                 NvDBPtr_Table[DeviceInstance]->DBgrCanvasTree
#define grVBlankList                 NvDBPtr_Table[DeviceInstance]->DBgrVBlankList
#define tmrCallbackList              NvDBPtr_Table[DeviceInstance]->DBtmrCallbackList
#define tmrCallbackFreeList          NvDBPtr_Table[DeviceInstance]->DBtmrCallbackFreeList
#define tmrCallbackTable             NvDBPtr_Table[DeviceInstance]->DBtmrCallbackTable
#define class63VBlankList            NvDBPtr_Table[DeviceInstance]->DBclass63VBlankList
#define class63ActiveVideo           NvDBPtr_Table[DeviceInstance]->DBclass63ActiveVideo
#define pHardwareObject              NvDBPtr_Table[DeviceInstance]->DBpHardwareObject
#define pNotifyObject                NvDBPtr_Table[DeviceInstance]->DBpNotifyObject

#define nvAddr                       NvDBPtr_Table[DeviceInstance]->DBnvAddr 
#define romAddr                      NvDBPtr_Table[DeviceInstance]->DBromAddr
#define fbAddr                       NvDBPtr_Table[DeviceInstance]->DBfbAddr 
#define agpAddr                      NvDBPtr_Table[DeviceInstance]->DBagpAddr
#define imAddr                       NvDBPtr_Table[DeviceInstance]->DBimAddr

#define ApertureTranslationTable     NvDBPtr_Table[DeviceInstance]->DBApertureTranslationTable
#define hashTable                    NvDBPtr_Table[DeviceInstance]->DBhashTable
#define fbInstBitmap                 NvDBPtr_Table[DeviceInstance]->DBfbInstBitmap

#define nvInfo                       NvDBPtr_Table[DeviceInstance]->DBnvInfo 


#endif // _NVRM_H_
