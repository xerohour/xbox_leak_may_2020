 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

 /***************************************************************************\
|*                                 Typedefs                                  *|
 \***************************************************************************/

typedef unsigned char      NvV8;  /* "void": enumerated or multiple fields   */
typedef unsigned short     NvV16; /* "void": enumerated or multiple fields   */
typedef unsigned long      NvV32; /* "void": enumerated or multiple fields   */
typedef unsigned char      NvU8;  /* 0 to 255                                */
typedef unsigned short     NvU16; /* 0 to 65535                              */
typedef unsigned long      NvU32; /* 0 to 4294967295                         */
typedef signed char        NvS8;  /* -128 to 127                             */
typedef signed short       NvS16; /* -32768 to 32767                         */
typedef signed long        NvS32; /* -2147483648 to 2147483647               */
typedef float              NvF32; /* IEEE Single Precision (S1E8M23)         */
typedef double             NvF64; /* IEEE Double Precision (S1E11M52)        */


typedef unsigned char   U008;
typedef unsigned short  U016;
typedef unsigned long   U032;

typedef unsigned char   V008;
typedef unsigned short  V016;
typedef unsigned long   V032;

typedef unsigned long   BOOL;
typedef long            S032;



#define NULL 0

#define FALSE 0
#define TRUE  1


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
#define RM_ERR_BAD_OBJECT_HANDLE       	(0x00000003|NV_ERROR_2)
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


#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d) - DEVICE_BASE(d) + 1
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


// #include <nv_local.h>
