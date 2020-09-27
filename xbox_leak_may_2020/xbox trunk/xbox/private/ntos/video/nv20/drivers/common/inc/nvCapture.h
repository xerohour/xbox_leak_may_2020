 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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

//////////////////////////////////////////////////////////////////////////////
// schema history
// 1. original version
// 2. adds CAPTURE_OBJECT_FREE, ANNOTATION, CAPTURE_CONFIG settings
// 3. adds CAPTURE_SYNC3 and obsoletes CAPTURE_SYNC
// 4. adds CAPTURE_LOCK
// 5. adds CAPTURE_FLIP_CHAIN
//////////////////////////////////////////////////////////////////////////////

#ifndef _nvCapture_h
#define _nvCapture_h

#ifdef __cplusplus
extern "C"
{
#endif
#pragma pack(push,1)

//////////////////////////////////////////////////////////////////////////////
// constants
//
#define CAPTURE_JMP_COMMAND             0x20ffffff
#define CAPTURE_EXT_SIGNATURE           0x420352ec

#define CAPTURE_CTXDMA_TYPE_PCI         0x49435920
#define CAPTURE_CTXDMA_TYPE_AGP         0x50474120
#define CAPTURE_CTXDMA_TYPE_VID         0x44495620

#define CAPTURE_SURFACE_TYPE_NORMAL     0x4d524f4e  // 'NORM'  surface goes into normal space (tiled)
#define CAPTURE_SURFACE_TYPE_ZETA       0x4154455a  // 'ZETA'  surface goes into zeta space (tiled)
#define CAPTURE_SURFACE_TYPE_TEXTURE    0x54584554  // 'TEXT'  surface goes into texture space

#define CAPTURE_SURFACE_KIND_TARGET     0x54475254  // 'TRGT'  surface will be used as a render target
#define CAPTURE_SURFACE_KIND_ZETA       0x4154455a  // 'ZETA'  surface will be used as a z buffer
#define CAPTURE_SURFACE_KIND_TEXTURE    0x54584554  // 'TEXT'  surface will be used as a texture
#define CAPTURE_SURFACE_KIND_VERTEX     0x54524556  // 'VERT'  surface will be used as a vertex buffer
#define CAPTURE_SURFACE_KIND_PUSHER     0x48535550  // 'PUSH'  surface will be used as a push buffer
#define CAPTURE_SURFACE_KIND_UNKNOWN    0x4e4b4e55  // 'UNKN'  surface will be used as a push buffer
#define CAPTURE_SURFACE_KIND_COMMAND    0x4d4d4f43  // 'COMM'  surface will be used as a command buffer
#define CAPTURE_SURFACE_KIND_PALETTE    0x434c4150  // 'PALE'  surface will be used as a palette

#define CAPTURE_SURFACE_FORMAT_Y8                0
#define CAPTURE_SURFACE_FORMAT_AY8               1
#define CAPTURE_SURFACE_FORMAT_A1R5G5B5          2
#define CAPTURE_SURFACE_FORMAT_X1R5G5B5          3
#define CAPTURE_SURFACE_FORMAT_A4R4G4B4          4
#define CAPTURE_SURFACE_FORMAT_R5G6B5            5
#define CAPTURE_SURFACE_FORMAT_A8R8G8B8          6
#define CAPTURE_SURFACE_FORMAT_unknown1          7
#define CAPTURE_SURFACE_FORMAT_X8R8G8B8          8
#define CAPTURE_SURFACE_FORMAT_I8_A1R5G5B5       9
#define CAPTURE_SURFACE_FORMAT_I8_R5G6B5        10
#define CAPTURE_SURFACE_FORMAT_I8_A4R4G4B4      11
#define CAPTURE_SURFACE_FORMAT_I8_A8R8G8B8      12
#define CAPTURE_SURFACE_FORMAT_DXT1_A1R5G5B5    13
#define CAPTURE_SURFACE_FORMAT_DXT23_A8R8G8B8   14
#define CAPTURE_SURFACE_FORMAT_DXT45_A8R8G8B8   15
#define CAPTURE_SURFACE_FORMAT_IMAGE_A1R5G5B5   16
#define CAPTURE_SURFACE_FORMAT_IMAGE_R5G6B5     17
#define CAPTURE_SURFACE_FORMAT_IMAGE_A8R8G8B8   18
#define CAPTURE_SURFACE_FORMAT_IMAGE_Y8         19
#define CAPTURE_SURFACE_FORMAT_IMAGE_SY8        20
#define CAPTURE_SURFACE_FORMAT_IMAGE_X7SY9      21
#define CAPTURE_SURFACE_FORMAT_IMAGE_R8B8       22
#define CAPTURE_SURFACE_FORMAT_IMAGE_G8B8       23
#define CAPTURE_SURFACE_FORMAT_IMAGE_SG8SB8     24
#define CAPTURE_SURFACE_FORMAT_unknown2         25
#define CAPTURE_SURFACE_FORMAT_unknown3         26
#define CAPTURE_SURFACE_FORMAT_unknown4         27

#define CAPTURE_LOCK_TYPE_LOCK                   1
#define CAPTURE_LOCK_TYPE_UNLOCK                 2

#define CAPTURE_ANNOTATION_ALLOC4X               0

#define CAPTURE_SYNC_SPECIAL            0xffffffff
#define CAPTURE_SYNC_WAIT_ZEROCOMMANDS  0x00000001  // wait for command buffer to empty (put==get)
#define CAPTURE_SYNC_WAIT_CHIPIDLE      0x00000002  // wait fot call engines to go idle
#define CAPTURE_SYNC_WAIT_CHANNELS      0x00000003  // wait for other channels to complete

#define CAPTURE_SYNC3_TYPE_ZEROCOMMANDS 0x00000001  // wait for command buffer to empty (put==get)
#define CAPTURE_SYNC3_TYPE_CHIPIDLE     0x00000002  // wait fot call engines to go idle
#define CAPTURE_SYNC3_TYPE_CHANNELS     0x00000003  // wait for other channels to complete
#define CAPTURE_SYNC3_TYPE_NOTIFIER     0x00000004  // wait for a notifier
#define CAPTURE_SYNC3_TYPE_REFCOUNT     0x00000005  // wait for a refcount
#define CAPTURE_SYNC3_TYPE_SEMAPHORE    0x00000006  // wait for a semaphore

#define CAPTURE_SCHEMA_1                0x00000001
#define CAPTURE_SCHEMA_2                0x00000002
#define CAPTURE_SCHEMA_3                0x00000003
#define CAPTURE_SCHEMA_4                0x00000004
#define CAPTURE_SCHEMA_5                0x00000005

#define CAPTURE_CURRENT_SCHEMA          CAPTURE_SCHEMA_5

//
// INFORMATION
//
#define CAPTURE_XID_INFORMATION             0x4f464e49  // 'INFO'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_INFORMATION
    DWORD dwSchema;                 // file version (CAPTURE_CURRENT_SCHEMA)
    char  szInformation[256];       // creation info
} CAPTURE_INFORMATION;

//
// CTXDMA_CREATE
//
#define CAPTURE_XID_CTXDMA_CREATE           0x52435843  // 'CXCR'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_CTXDMA_CREATE
    DWORD dwHandle;                 // you figure it out, Tex
    DWORD dwType;                   // CAPTURE_CTXDMA_TYPE_xxx
    DWORD dwSize;                   // [bytes]
    DWORD dwBaseAddress;            // application base address
} CAPTURE_CTXDMA_CREATE;

//
// SURFACE_ALLOC
//
#define CAPTURE_XID_SURFACE_ALLOC           0x41465253  // 'SRFA'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_SURFACE_ALLOC
    DWORD dwCtxDMAHandle;           // you figure it out, Tex
    DWORD dwOffset;                 // offset in CTXDMA
    DWORD dwSize;                   // size of surface (in bytes)
    DWORD dwAlignment;              // alignment (if known, 0 - let interpreter infer from CTXDMA)
    DWORD dwType;                   // CAPTURE_SURFACE_TYPE_xxx
    DWORD dwKind;                   // usage hint - CAPTURE_SURFACE_KIND_xxx
    DWORD dwSurfaceType;            // CAPTURE_SURFACE_FORMAT_xxx
    DWORD dwIntendedPitch;          // pitch we think we will apply on this surface
    DWORD dwIntendedHeight;         // height we think we will apply on this surface
} CAPTURE_SURFACE_ALLOC;

//
// SURFACE_FREE
//
#define CAPTURE_XID_SURFACE_FREE            0x46465253  // 'SRFF'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_SURFACE_FREE
    DWORD dwCtxDMAHandle;           // you figure it out, Tex
    DWORD dwOffset;                 // offset in CTXDMA
} CAPTURE_SURFACE_FREE;

//
// MEMORY_WRITE
//
#define CAPTURE_XID_MEMORY_WRITE            0x574d454d  // 'MEMW'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_MEMORY_WRITE
    DWORD dwCtxDMAHandle;           // you figure it out, Tex
    DWORD dwOffset;                 // offset in CTXDMA
    DWORD dwSize;                   // number of bytes (multiple of 4)
    // [ data goes here ]
} CAPTURE_MEMORY_WRITE;

//
// SYNC - obsoleted in schema 3
//
#define CAPTURE_XID_SYNC                    0x434e5953  // 'SYNC'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_SYNC
    DWORD dwCtxDMAHandle;           //                  [ if special, undefined                              ]
    DWORD dwOffset;                 // offset in CTXDMA [ special case when dwOffset == CAPTURE_SYNC_SPECIAL ]
    DWORD dwValue;                  // value to match   [ if special, one of CAPTURE_SYNC_WAIT_xxx           ]
} CAPTURE_SYNC;

//
// SYNC3
//
#define CAPTURE_XID_SYNC3                   0x334e5953  // 'SYN3'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_SYNC3
    DWORD dwType;                   // CAPTURE_SYNC3_TYPE_xxx
    DWORD dwCtxDMAHandle;           // valid for type semaphore, else irrelevant
    DWORD dwOffset;                 // offset in CTXDMA, valid for type semaphore, else irrelevant
    DWORD dwValue;                  // value to match
} CAPTURE_SYNC3;

//
// OBJECT_CREATE
//
#define CAPTURE_XID_OBJECT_CREATE           0x5243424f  // 'OBCR'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_OBJECT_CREATE
    DWORD dwClassID;                // actual class this handle maps to
    DWORD dwHandle;                 // handle of the class
} CAPTURE_OBJECT_CREATE;

//
// OBJECT_FREE
//
#define CAPTURE_XID_OBJECT_FREE             0x5246424f  // 'OBFR'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_OBJECT_FREE
    DWORD dwHandle;                 // you figure it out, Tex
} CAPTURE_OBJECT_FREE;

//
// SCENE
//
#define CAPTURE_XID_SCENE                   0x454e4353  // 'SCNE'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_SCENE
} CAPTURE_SCENE;

//
// LOCK / UNLOCK
//
#define CAPTURE_XID_LOCK                    0x4b434f4c  // 'LOCK'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_LOCK
    DWORD dwType;                   // CAPTURE_LOCK_TYPE_xxx
    DWORD dwCtxDMAHandle;
    DWORD dwOffset;
} CAPTURE_LOCK;

//
// FLIP_CHAIN
//
#define CAPTURE_XID_FLIP_CHAIN              0x4e484346  // 'FCHN'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_FLIP_CHAIN
    DWORD dwCount;                  // number of surfaces in chain. addresses follow.
    // [ data here ]
} CAPTURE_FLIP_CHAIN;

//
// ANNOTATION
//
#define CAPTURE_XID_ANNOTATION              0x4f4e4e41  // 'ANNO'
typedef struct {
    DWORD dwJmpCommand;             // CAPTURE_JMP_COMMAND
    DWORD dwExtensionSignature;     // CAPTURE_EXT_SIGNATURE
    DWORD dwExtensionID;            // CAPTURE_XID_ANNOTATION
    DWORD dwValue;                  // CAPTURE_ANNOTATION_xxx
} CAPTURE_ANNOTATION;


//////////////////////////////////////////////////////////////////////////////
// procedures
//
int captureStartup   (char *szBasePath, char *szBaseFilename, DWORD dwBaseFilenum);
int captureShutdown  (void);
int captureSetParams (char *szBasePath, char *szBaseFilename, DWORD dwBaseFilenum);
int captureLog       (void *pBuffer, unsigned uCount);
int captureFileInc   (void);

int capturePlayStartup   (char *szBasePath, char *szBaseFilename, DWORD dwBaseFilenum);
int capturePlayShutdown  (void);
int capturePlaySetParams (char *szBasePath, char *szBaseFilename, DWORD dwBaseFilenum);
int capturePlay          (void *pRenderTarget, DWORD dwPitch, DWORD dwWidth, DWORD dwHeight);
int capturePlayFileInc   (void);

#pragma pack(pop)
#ifdef __cplusplus
}
#endif

#endif // _nvPM_h
