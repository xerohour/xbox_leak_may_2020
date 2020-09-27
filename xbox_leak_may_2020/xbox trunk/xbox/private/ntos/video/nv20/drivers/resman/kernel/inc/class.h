#ifndef _CLASS_H_
#define _CLASS_H_
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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
/****************************** Object Manager *****************************\
*                                                                           *
* Module: CLASS.H                                                           *
*       User Object management.                                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Includes.
//
//---------------------------------------------------------------------------

#ifndef _BTREE_H_
#include <btree.h>
#endif // _BTREE_H_

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

#define RESERVED_NAMES                  4096
#define MAX_OS_NAME_LENGTH              80

//---------------------------------------------------------------------------
//
//  Types and structures.
//
//---------------------------------------------------------------------------

//
// Forward declarations of structures.
//
typedef struct _def_method METHOD, *PMETHOD;
typedef struct _def_object OBJECT, *POBJECT;
typedef struct _def_class  CLASS,  *PCLASS;

//
// Methods.
//
typedef RM_STATUS (*METHODPROC)(PHWINFO, POBJECT, PMETHOD, U032, V032);
struct _def_method
{
    METHODPROC Proc;
    U032       Low;
    U032       High;
};

//
// Objects.
//
typedef struct _def_class_object        CLASSOBJECT, *PCLASSOBJECT;
typedef struct _def_dma_object          DMAOBJECT,   *PDMAOBJECT;
typedef struct _def_patch_object        PATCHOBJECT, *PPATCHOBJECT;
typedef struct _def_context_time_object TIMEOBJECT,  *PTIMEOBJECT;
typedef struct _def_event_notification  EVENTNOTIFICATION, *PEVENTNOTIFICATION;
typedef struct _def_engine_info         ENGINEINFO, *PENGINEINFO;
  
struct _def_object
{
    U032                Name;
    U032                ChID;
    PCLASS              Class;
    U032                Subchannel;
    NODE                Node;
    POBJECT             Next;
    PDMAOBJECT          NotifyXlate;
    U032                NotifyAction;
    U032                NotifyTrigger;
    PEVENTNOTIFICATION  NotifyEvent;
    PCLASS              ThisClass;   //KJK I don't like this!!
};
//
// Patch structure.
//
struct _def_patch_object
{
    OBJECT  Base;
    POBJECT Destination;
    POBJECT Source;
};
//
// NV_CLASS structure and create/destroy function prototypes.
//
typedef RM_STATUS (*CREATEOBJECTPROC)(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
typedef RM_STATUS (*DELETEOBJECTPROC)(PHWINFO, POBJECT);

struct _def_class_create
{
    U032                Class;
    CREATEOBJECTPROC    Function;
};

//
// Classes.
//
typedef struct _def_class_methods
{
    U032                Type;
    PMETHOD             Method;
    U032                MethodMax;
} CLASSMETHODS, *PCLASSMETHODS;

struct _def_class
{
    U032                Type;
    PMETHOD             Method;
    U032                MethodMax;
    //U032                StateSize;
    PCLASSDESCRIPTOR    ClassDecl;
    PCLASSDESCRIPTOR    ClassInstMalloc;
    PENGINEDESCRIPTOR   EngineDecl;
    PENGINEINFO         EngineInfo;
};

struct _def_class_object
{
    OBJECT     Base;
};

//===========================================================================
//
//                                Class Defines.
//
//===========================================================================

#define NV_CONTEXT_DMA_TO_MEM               3
#define NV_NULL_OBJECT                      0

//---------------------------------------------------------------------------
//
//  Method prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS mthdNoOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);

RM_STATUS mthdValidate(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdCreateLegacy(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdCreate(PHWINFO, POBJECT, PMETHOD, U032, V032, VOID*);
RM_STATUS mthdDestroy(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNameCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNameStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNameLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetEnumerateCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetEnumerateStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetEnumerateLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdEnumerate(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetDmaSpecifier(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAlarmTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAlarmTimeRelative(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAlarmNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAlarmNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdTmrStopTransfer(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdGetChipToken(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetOrdinal(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSwitchNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSwitchNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetGamePortOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetGamePortInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetGamePortFormat(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetStartTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetStartCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSampleRate(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetNumberChannels(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferNotifyCount(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferPosNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferPosNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferInCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferOutCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferInStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferOutStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferMemNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferMemLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferMemNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVideoInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVideoOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetCtxOrdinal(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetColormapCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetColormapFormat(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetColormapStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetColormapLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdColormapDirtyStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdColormapDirtyLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetImageOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetImageInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBetaInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBetaOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetRopInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetRopOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetColorFormat(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetMonochromeFormat(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetGraphicsCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAudioInput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAudioFormat(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSampleRateAdjust(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetStopLoopingCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetPortamento(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVibratoDepth(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVibratoFrequency(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVibratoAttackCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetVibratoAttack(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetDiscardPriority(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferLoopStart(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetBufferLoopLength(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAudioOutput(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetOutputGain(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetInputGain(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAttack(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetDecay(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSustain(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetRelease(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetAttackCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetReleaseCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetSustainLevel(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetTremoloDepth(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetTremoloAttackCtxTime(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetTremoloAttack(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdSetTremoloFrequency(PHWINFO, POBJECT, PMETHOD, U032, V032);

//
// Method aliases to common method procedure.
//
RM_STATUS mthdStaticImage(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdRender(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdRenderBeta(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdRender3d(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdImageToMem(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdDmaRender(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdImageBlit(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdGrSetNotify(PHWINFO, POBJECT, PMETHOD, U032, V032);
RM_STATUS mthdGrSetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, V032);
#define mthdSetBeta         mthdStaticImage
#define mthdSetRop          mthdStaticImage
#define mthdSetColor        mthdStaticImage
#define mthdSetPatternShape mthdStaticImage
#define mthdSetColor0       mthdStaticImage
#define mthdSetColor1       mthdStaticImage
#define mthdSetPattern      mthdStaticImage
#define mthdSetRectangle    mthdStaticImage
#define mthdColor           mthdRender
#define mthdPoint           mthdRender
#define mthdPoint32         mthdRender
#define mthdColorPoint      mthdRender
#define mthdLine            mthdRender
#define mthdLine32          mthdRender
#define mthdPolyLine        mthdRender
#define mthdPolyLine32      mthdRender
#define mthdColorPolyLine   mthdRender
#define mthdLin             mthdRender
#define mthdLin32           mthdRender
#define mthdPolyLin         mthdRender
#define mthdPolyLin32       mthdRender
#define mthdColorPolyLin    mthdRender
#define mthdTriangle        mthdRender
#define mthdTriangle32      mthdRender
#define mthdTrimesh         mthdRender
#define mthdTrimesh32       mthdRender
#define mthdColorTriangle   mthdRender
#define mthdColorTrimesh    mthdRender
#define mthdRectangle       mthdRender
#define mthdControlPointIn  mthdRender
#define mthdControlPointOut mthdRender
#define mthdControlPointOutFP mthdRender
#define mthdSize            mthdRender
#define mthdImagePitch      mthdRender
#define mthdImageStart      mthdRender
#define mthdSizeIn          mthdRender
#define mthdColor0          mthdRender
#define mthdColor1          mthdRender
#define mthdMonochrome      mthdRender
#define mthdSubdivideIn     mthdRender
#define mthdColorBeta       mthdRenderBeta
#define mthdControlBetaOut  mthdRenderBeta
#define mthdSubdivideInBeta mthdRenderBeta
#define mthdFilterBeta      mthdRenderBeta
#define mthdControlPointOutBeta mthdRenderBeta
#define mthdControlPointOutFPBeta mthdRenderBeta
#define mthdBlitPointIn     mthdImageBlit
#define mthdBlitPointOut    mthdImageBlit
#define mthdBlitSize        mthdImageBlit
#define mthdImageToMemPoint mthdImageToMem
#define mthdImageToMemSize  mthdImageToMem
#define mthdImageToMemPitch mthdImageToMem
#define mthdImageToMemStart mthdImageToMem
#define mthdGraphicsFromMemPoint mthdDmaRender
#define mthdGraphicsFromMemSize  mthdDmaRender
#define mthdGraphicsFromMemPitch mthdDmaRender
#define mthdGraphicsFromMemStart mthdDmaRender
#define mthdGraphicsFromMemSize  mthdDmaRender

//---------------------------------------------------------------------------
//
//  Class object prototypes.
//
//---------------------------------------------------------------------------
RM_STATUS classCreateWellKnownObjects(PHWINFO, U032);
RM_STATUS classDeleteWellKnownObjects(VOID*, POBJECT);
RM_STATUS classInitCommonObject(POBJECT, PCLASSOBJECT, U032);
RM_STATUS classCreate(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS classDestroy(VOID*, POBJECT);
RM_STATUS nopCreate(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS nopDestroy(VOID*, POBJECT);
RM_STATUS classSoftwareMethod(PHWINFO, POBJECT, U032, V032);
RM_STATUS classDirectSoftwareMethod(PHWINFO, POBJECT, U032, V032);

// JJV - Moved this structure from class66.c and class63.c
typedef struct _def_surface_attributes
{
  
  U032 StartAddress;
  U032 Type;		// tells you whether this is an interleaved surface or other
  U032 Pitch;
  U032 Length;
  U032 FittedSize;  // not used for overlay
  
} SURFACEATTRIBUTES, * PSURFACEATTRIBUTES ;

// JJV - Moved these defines from class66.c
// defined digitisers

#define PHILIPS_DEVICE_110 		0
#define PHILIPS_DEVICE_111A		1
#define ITT_VPX3225D_A1			0x20
#define ITT_VPX3225D_C3			0x23
#define SAMSUNG_DEVICE_127 		0xF0

#define ST_NTSC			0x0
#define ST_PAL			0x10
#define ST_UNDECODED	0xFFFFFFFF

#define IS_FLIP_FOR_OVERLAY_SURFACE_0 			0x10
#define IS_FLIP_FOR_OVERLAY_SURFACE_1 			0x20

#define NV_VCTM_IMAGE_FORMAT_NEXT_FIELD_EVEN	0x0
#define NV_VCTM_IMAGE_FORMAT_NEXT_FIELD_ODD		0x80000000
#define NV_VCTM_IMAGE_FORMAT_COLOR_LE_Y8V8Y8U8	0x00020001

#define NV_VCTM_VBI_FORMAT_NEXT_FIELD_EVEN		0x0
#define NV_VCTM_VBI_FORMAT_NEXT_FIELD_ODD		0x80000000
#define NV_VCTM_VBI_FORMAT_COLOR_LE_Y8_P4		0x01010000

//---------------------------------------------------------------------------
//
//  Engine structures and prototypes.
//
//---------------------------------------------------------------------------

//
// Engine info structure.
//
typedef RM_STATUS (*ENGINECREATEOBJECT)(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
typedef RM_STATUS (*ENGINEDELETEOBJECT)(VOID*, POBJECT);

struct _def_engine_info
{
    U032 Tag;                           // cross-ref with rom data tables
    PMETHOD Methods;                    // default methods
    U032 MethodMax;                     // default method max
    ENGINECREATEOBJECT CreateObject;
    ENGINEDELETEOBJECT DestroyObject;
};

typedef struct _def_engine_db
{
    PENGINEINFO         pEngineInfos;
    PENGINEDESCRIPTOR   pEngineDescriptors;
    U032                numEngineDescriptors;
} ENGINEDB, *PENGINEDB;

// kernel/hal/haleng.c
RM_STATUS engineGetInfo(PENGINEDB, U032, PENGINEINFO *, PENGINEDESCRIPTOR *);
U032 engineGetBufferSize(PENGINEDB, U032, U032);

//---------------------------------------------------------------------------
//
//  Class database structure.
//
//  This state is pulled from the HAL tables.
//
//---------------------------------------------------------------------------

typedef struct _def_class_db
{
    PCLASS              pClasses;
    U032                numClasses;
    PCLASSDESCRIPTOR    pClassDescriptors;
    U032                numClassDescriptors;
} CLASSDB, *PCLASSDB;

RM_STATUS classBuildDB(PENGINEDB, PCLASSDB);
RM_STATUS classDestroyDB(PCLASSDB);
RM_STATUS classGetClass(PCLASSDB, U032, PCLASS *);

#endif // _CLASS_H_
