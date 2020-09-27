#ifndef _NVROMDAT_H_
#define _NVROMDAT_H_

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

/************************ NV ROM Defines and Structures ********************\
*                                                                           *
* Module: nvromdat.h                                                        *
*       NV RM/HAL ROM data table formats.                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//
// Describes format of data tables found in ROM.
//
// Currently two such tables:  engine descriptors and class descriptors.
//

#include <nvhal.h>

//
// Engine descriptor instruction opcodes (4 bits wide).
//
#define ENGDECL_INSTR                   0x0
#define ENGMALLOC_INSTR                 0x1
#define ENGINSTMALLOC_INSTR             0x2

//
// Engine declaration instruction
//
// There is one engine descriptor instruction for each engine in the
// chip:
//
//  31      27      25      21                                       0 
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 4 bits| 4 bits| 4 bits| 20 bits                                 |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Opcode  Tag     Hash     Obj inst size
//
// Opcode: ENGDECL_INSTR
// Tag:
//   Unique engine tag.  Used by class headers.
// Hash:
//   NV_RAMHT_ENGINE bits (if any) for objects managed by engine.
// Object instance size:
//   Default instmem size (in paragraphs) to alloc for objects managed
//   by engine.  This value can be increased for a given class using the
//   CLASSINSTMALLOC_INSTR (see below).
//

typedef U032 ENGINEDESCRIPTOR, *PENGINEDESCRIPTOR;

#define ENGDECL_OBJINSTSZ               19:0
#define ENGDECL_OBJRAMHT                23:20
#define ENGDECL_TAG                     27:24
#define ENGDECL_OPCODE                  31:28

#define MKENGDECL(tag,ht,osz) \
    (ENGDECL_INSTR << SF_SHIFT(ENGDECL_OPCODE) | \
     tag << SF_SHIFT(ENGDECL_TAG) | \
     ht << SF_SHIFT(ENGDECL_OBJRAMHT) | \
     osz << SF_SHIFT(ENGDECL_OBJINSTSZ))

// WARNING: these macros depend on <nvrm.h>
#define ENGDECL_FIELD(desc,field)       (((U032)desc >> SF_SHIFT(ENGDECL ## field)) & SF_MASK(ENGDECL ## field))

//
// Engine malloc instruction
//
// The engine malloc instruction is used to describe system memory
// requirements for the engine.
//
//  31      27      23      21      17                               0 
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 4 bits| 4 bits| 4 bits| 20 bits                                 |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Opcode  Tag     Type    Size
//
// Opcode: ENGMALLOC_OPCODE
// Tag: 
//   Unique engine tag.
// Type:
//   Describes purpose of memory, indicating when
//   memory should be allocated/freed.
// Size:
//   Amount of memory (in dwords).
//

#define ENGMALLOC_SIZE                  19:0
#define ENGMALLOC_TYPE                  23:20
#define ENGMALLOC_TAG                   27:24
#define ENGMALLOC_OPCODE                31:28

#define MKENGMALLOC(tag,type,sz) \
    (ENGMALLOC_INSTR << SF_SHIFT(ENGMALLOC_OPCODE) | \
     tag << SF_SHIFT(ENGMALLOC_TAG) | \
     type << SF_SHIFT(ENGMALLOC_TYPE) | \
     sz << SF_SHIFT(ENGMALLOC_SIZE))

// WARNING: these macros depend on <nvrm.h>
#define ENGMALLOC_FIELD(desc,field)    (((U032)desc >> SF_SHIFT(ENGMALLOC ## field)) & SF_MASK(ENGMALLOC ## field))

//
// Engine malloc types:
//   TYPE_PRIVATE
//     Buffer should be allocated/freed when resman starts up
//     and shuts down.  Used for chip-dependent private state.
//   TYPE_INIT
//     Buffer should be allocated/freed across call to INIT
//     engine (STATE_INIT).
//   TYPE_DESTROY
//     Buffer should be allocated/freed across call to DESTROY
//     engine (STATE_DESTROY).
//   TYPE_LOAD
//     Buffer should be allocated/freed across call to LOAD
//     engine (STATE_LOAD).
//
#define ENGMALLOC_TYPE_PRIVATE          0x0
#define ENGMALLOC_TYPE_INIT             0x1
#define ENGMALLOC_TYPE_DESTROY          0x2
#define ENGMALLOC_TYPE_LOAD             0x3

//
// Engine instmem alloc instruction
//
// The engine instmem alloc instruction is used to describe instance
// memory requirements for the engine.  Up to one megabyte of instance
// memory can be requested with this instruction (supporting up to a
// 4K alignment).
//
//  31      27      23              15                               0 
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 4 bits| 4 bits| 8 bits        | 16 bits                         |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    Opcode  Tag     Align            Size
//
// Opcode: ENGINSTMALLOC_OPCODE
// Tag: 
//   Unique engine tag.
// Align:
//   Alignment requirements of allocation (in paragraphs).
//   A value of 0 indicates the default alignment (1 paragraph).
// Size:
//   Amount of memory (in paragraphs).
//
#define ENGINSTMALLOC_SIZE              15:0
#define ENGINSTMALLOC_ALIGN             23:16
#define ENGINSTMALLOC_TAG               27:24
#define ENGINSTMALLOC_OPCODE            31:28

#define MKENGINSTMALLOC(tag,align,sz) \
    (ENGINSTMALLOC_INSTR << SF_SHIFT(ENGINSTMALLOC_OPCODE) | \
     tag << SF_SHIFT(ENGINSTMALLOC_TAG) | \
     align << SF_SHIFT(ENGINSTMALLOC_ALIGN) | \
     sz << SF_SHIFT(ENGINSTMALLOC_SIZE))

// WARNING: these macros depend on <nvrm.h>
#define ENGINSTMALLOC_FIELD(desc,field)    (((U032)desc >> SF_SHIFT(ENGINSTMALLOC ## field)) & SF_MASK(ENGINSTMALLOC ## field))

//
// Class descriptor instruction opcodes (4 bits wide).
//
#define CLASSDECL_INSTR                 0x0
#define CLASSINSTMALLOC_INSTR           0x1

//
// Class declaration instruction
//
//  31      27      21   21          15                               0 
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |4 bits |4 bits | 8 bits        | 16 bits                         |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Opcode  Tag     Lock            Class number
// 
// Opcode: CLASSDECL_INSTR
// Tag: engine tag
// Lock: allocation restriction
// Class number: class number
//

typedef U032 CLASSDESCRIPTOR, *PCLASSDESCRIPTOR;

#define CLASSDECL_CLASS                 15:0
#define CLASSDECL_LOCK                  23:16
#define CLASSDECL_TAG                   27:24
#define CLASSDECL_OPCODE                31:28

#define MKCLASSDECL(tag,lock,class) \
    (CLASSDECL_INSTR << SF_SHIFT(CLASSDECL_OPCODE) | \
     tag << SF_SHIFT(CLASSDECL_TAG) | \
     lock << SF_SHIFT(CLASSDECL_LOCK) | \
     class << SF_SHIFT(CLASSDECL_CLASS))

// WARNING: these macros depend on <nvrm.h>
#define CLASSDECL_FIELD(desc,field)     (((U032)desc >> SF_SHIFT(CLASSDECL ## field)) & SF_MASK(CLASSDECL ## field))

//
// No allocation restrictions for the class.
//
#define NO_LOCK                         0xff

//
// Class instance memory alloc instruction
//
//  31      27              19                                       0 
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 4 bits| 8 bits        | 20 bits                                 |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Opcode  Alignment       Size
//
// Opcode: CLASSINSTMALLOC_INSTR
// Alignment:
//   Instance memory alignment (in paragraphs).  A value of 0 indicates
//   the default alignment (1 paragraph).
// Size:
//   Additional instance space (in paragraphs) for objects of this class
//   to the amount specified in ENGDESC_OBJINSTSZ field.
//

#define CLASSINSTMALLOC_SIZE            19:0
#define CLASSINSTMALLOC_ALIGN           27:20
#define CLASSINSTMALLOC_OPCODE          31:28

#define MKCLASSINSTMALLOC(align,size) \
    (CLASSINSTMALLOC_INSTR << SF_SHIFT(CLASSINSTMALLOC_OPCODE) | \
     align << SF_SHIFT(CLASSINSTMALLOC_ALIGN) | \
     size << SF_SHIFT(CLASSINSTMALLOC_SIZE))

// WARNING: these macros depend on <nvrm.h>
#define CLASSINSTMALLOC_FIELD(desc,field)  (((U032)desc >> SF_SHIFT(CLASSINSTMALLOC ## field)) & SF_MASK(CLASSINSTMALLOC ## field))

//
// Graphics engine (per channel) allocation locks.
//
#define GR_LOCK_CONTEXT_BETA_SOLID          0x00
#define GR_LOCK_CONTEXT_BETA                0x01
#define GR_LOCK_CONTEXT_COLOR_KEY           0x02
#define GR_LOCK_CONTEXT_PATTERN             0x03
#define GR_LOCK_CONTEXT_CLIP                0x04
#define GR_LOCK_CONTEXT_ROP                 0x05
#define GR_LOCK_DVD                         0x06
#define GR_LOCK_CONTEXT_SURFACES_2D         0x07
#define GR_LOCK_CONTEXT_SURFACES_3D         0x08
#define GR_LOCK_CONTEXT_SURFACES_SWIZZLED   0x09
#define GR_LOCK_3D                          0x0a

//
// MediaPort engine (per device) allocation locks.
//
#define MP_LOCK_DECODER                     0x00    
#define MP_LOCK_DECOMPRESSOR                0x01

//
// Video engine (per device) allocation locks.
//

//
// HAL initialization data.
//
extern CLASSDESCRIPTOR NV04_ClassDescriptors[];
extern U032 NV04_NumClassDescs;
extern CLASSDESCRIPTOR NV05_ClassDescriptors[];
extern U032 NV05_NumClassDescs;
extern ENGINEDESCRIPTOR NV04_EngineDescriptors[];
extern U032 NV04_NumEngineDescs;

extern CLASSDESCRIPTOR NV10_ClassDescriptors[];
extern U032 NV10_NumClassDescs;
extern CLASSDESCRIPTOR NV15_ClassDescriptors[];
extern U032 NV15_NumClassDescs;
extern CLASSDESCRIPTOR NV11_ClassDescriptors[];
extern U032 NV11_NumClassDescs;
extern ENGINEDESCRIPTOR NV10_EngineDescriptors[];
extern U032 NV10_NumEngineDescs;

extern CLASSDESCRIPTOR NV20_ClassDescriptors[];
extern U032 NV20_NumClassDescs;
extern ENGINEDESCRIPTOR NV20_EngineDescriptors[];
extern U032 NV20_NumEngineDescs;

#endif // _NVROMDAT_H_

