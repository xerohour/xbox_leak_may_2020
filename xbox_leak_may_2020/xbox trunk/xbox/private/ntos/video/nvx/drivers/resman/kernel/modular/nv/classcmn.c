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

/**************************** Common Class Routines ************************\
*                                                                           *
* Module: classcmn.c                                                        *
*   This module implements the common class routines referenced in the      *
*   classTable in class.c                                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <gr.h>   
#include <dma.h>   
#include <modular.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
#include "dac.h"

/*
 * class04D common routines
 */
RM_STATUS Commonclass04DNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DNotify(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DStopTransferVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DStopTransferVbi(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DStopTransferImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DStopTransferImage(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DSetContextDmaNotifies(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetContextDmaImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DSetContextDmaImage(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetContextDmaVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04DSetContextDmaVbi(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetImageConfig
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DSetImageConfig(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetImageStartLine
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DSetImageStartLine(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass04DSetVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DSetVbi(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04DSetImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DSetImage(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04DGetVbiOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DGetVbiOffsetNotify(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04DGetImageOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
  return(class04DGetImageOffsetNotify(pDev, Object, Method, Offset, Data));
}  

/*
 * class04E common routines
 */
RM_STATUS Commonclass04ENotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04ENotify(pDev, Object, Method, Offset, Data));
}  
  
RM_STATUS Commonclass04EStopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04EStopTransfer(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04ESetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04ESetContextDmaNotifies(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04ESetContextDmaData
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04ESetContextDmaData(pDev, Object, Method, Offset, Data));
} 
 
RM_STATUS Commonclass04ESetContextDmaImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04ESetContextDmaImage(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04EReadData
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04EReadData(pDev, Object, Method, Offset, Data));
}
  
RM_STATUS Commonclass04EWriteImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04EWriteImage(pDev, Object, Method, Offset, Data));
}

/*
 * class04F common routines
 */
RM_STATUS Commonclass04FSetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FSetContextDmaNotifies(pDev, Object, Method, Offset, Data));
}  
  
RM_STATUS Commonclass04FNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FNotify(pDev, Object, Method, Offset, Data));
}
  
RM_STATUS Commonclass04FStopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FStopTransfer(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04FWrite
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FWrite(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04FRead
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FRead(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass04FSetInterruptNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class04FSetInterruptNotify(pDev, Object, Method, Offset, Data));
}  

/*
 * class63 common routines
 */
RM_STATUS Commonclass63SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63SetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}  
  
RM_STATUS Commonclass63SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63SetNotify(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass63StopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63StopTransfer(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass63SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63SetVideoOutput(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass63SetImageCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63SetImageCtxDma(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass63ImageScan
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class63ImageScan(pDev, Object, Method, Offset, Data));
}
  
RM_STATUS Commonclass63GetOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    return(class63GetOffsetNotify(pDev, Object, Method, Offset, Data));
}    

/*
 * class64 common routines
 */
RM_STATUS Commonclass64SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetNotify(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetVideoOutput(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetVideoInput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetVideoInput(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetDeltaDuDx
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetDeltaDuDx(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetDeltaDvDy
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetDeltaDvDy(pDev, Object, Method, Offset, Data));
}  

RM_STATUS Commonclass64SetPoint
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class64SetPoint(pDev, Object, Method, Offset, Data));
}  

/*
 * class65 common routines
 */
RM_STATUS Commonclass65SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}
  
RM_STATUS Commonclass65SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetNotify(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetVideoOutput(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetVideoInput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetVideoInput(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetColorFormat
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetColorFormat(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetColorKey
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetColorKey(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetPoint
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetPoint(pDev, Object, Method, Offset, Data));
}

RM_STATUS Commonclass65SetSize
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
  return(class65SetSize(pDev, Object, Method, Offset, Data));
}

