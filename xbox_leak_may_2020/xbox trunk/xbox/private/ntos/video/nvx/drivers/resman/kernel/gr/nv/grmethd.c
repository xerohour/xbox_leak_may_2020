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

/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: GRMETHD.C                                                         *
*   The graphics engine is managed in this module.  All priviledged state   *
*   relating to the canvas and datapath is managed here.  Context switching *
*   is also contained here.                                                 *
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
#include <fifo.h>
#include <dma.h>
#include <dac.h>
#include <gr.h>
#include <modular.h>   
#include <os.h>
#include <nv32.h>
#include "nvhw.h"


// extern PGRAPHICSCHANNEL grTable;


//---------------------------------------------------------------------------
//
//  Graphics video patchcord methods
//
//---------------------------------------------------------------------------

RM_STATUS mthdSetVideoInput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    return (RM_OK);

#ifdef KJK
    RM_STATUS       status;
    U032            InputIndex;
    U032            i;
    PVIDEOPATCHCORD *VideoInput;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoPatchcord;
    
    InputIndex = (Offset - Method->Low) / 4;

    /*
    KJK This is probably useless, since we wouldn't
        have been called if the method itself was
        out of range for this object.

    switch (Object->Class->Type)
    {
        case NV_VIDEO_SWITCH:
            if (InputIndex >= 2)
                return (RM_ERR_INVALID_PATCH);
            break;
        default:
            if (InputIndex >= MAX_GRPATCH_INPUT)
                return (RM_ERR_INVALID_PATCH);
            break;
    }
    */

    switch (Object->Class->Type)
    {
        case NV_VIDEO_SINK:
            VideoInput  = &(((PVIDEOSINKOBJECT)Object)->VideoInput[InputIndex]);
            VideoOutput = NULL;
            break;
        case NV_VIDEO_COLORMAP:
            VideoInput  = &(((PVIDEOCOLORMAPOBJECT)Object)->VideoInput);
            VideoOutput =   ((PVIDEOCOLORMAPOBJECT)Object)->VideoOutput;
            break;
        case NV_VIDEO_SCALER:
            VideoInput  = &(((PVIDEOSCALEROBJECT)Object)->VideoInput);
            VideoOutput =   ((PVIDEOSCALEROBJECT)Object)->VideoOutput;
            break;
        /*            
        case NV_VIDEO_CAPTURE_TO_MEMORY:
            VideoInput  = &(((PVIDEOTOMEMOBJECT)Object)->VideoInput);
            VideoOutput = NULL;
            break;
        */            
        default:
            VideoInput  = &(((PVIDEOOBJECT)Object)->VideoInput[InputIndex]);
            VideoOutput =   ((PVIDEOOBJECT)Object)->VideoOutput;
            break;
    }

    //
    // Disconnect old patchcord.  Break apart all patch information.
    //
    if (*VideoInput)
    {
        DBG_VAL_PTR(*VideoInput);
        for (i = 0; (*VideoInput)->Destination[i] != (PVIDEOOBJECT)Object; i++);
        (*VideoInput)->FanOut--;
        (*VideoInput)->Destination[i] = NULL;
        *VideoInput                   = NULL;
    }

    if (Data)
    {
        //
        // Validate patchcord and connect.
        //
        status = fifoSearchObject(pDev, Data, Object->ChID, (POBJECT *)&VideoPatchcord);
        if (status)
            return (status);
        if (VideoPatchcord->Base.Class->Type != NV_PATCHCORD_VIDEO)
            return (RM_ERR_OBJECT_TYPE_MISMATCH);
        if (VideoPatchcord->FanOut == MAX_GRPATCH_FANOUT)
            return (RM_ERR_MAX_PATCH_FANOUT);
        for (i = 0; (i < MAX_GRPATCH_FANOUT) && (VideoPatchcord->Destination[i] != (PVIDEOOBJECT)Object); i++);
        if (i == MAX_GRPATCH_FANOUT)
        {
            //
            // Insert this connection into the first available slot.
            //
            for (i = 0; VideoPatchcord->Destination[i]; i++);
            VideoPatchcord->FanOut++;
            VideoPatchcord->Destination[i] = (PVIDEOOBJECT)Object;
            *VideoInput                    = VideoPatchcord;
        }
    }

    return (RM_OK);
#endif // KJK    
}

RM_STATUS mthdSetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    return (RM_OK);

#ifdef KJK
    RM_STATUS        status;
    PVIDEOPATCHCORD *VideoOutput;
    PVIDEOPATCHCORD  VideoPatchcord;
    
    switch (Object->Class->Type)
    {
        /*
        case NV_IMAGE_TO_VIDEO:
            VideoOutput = &(((PIMAGEVIDEOOBJECT)Object)->VideoOutput);
            break;
        */            
        case NV_VIDEO_FROM_MEMORY:
            VideoOutput = &(((PVIDEOFROMMEMOBJECT)Object)->VideoOutput);
            break;
        /*            
        case NV_VIDEO_SOURCE:
            VideoOutput = &(((PVIDEOSOURCEOBJECT)Object)->VideoOutput);
            break;
        */            
        default:
            VideoOutput = &(((PVIDEOOBJECT)Object)->VideoOutput);
            break;
    }
    //
    // Disconnect old patchcord.  Break apart all patch information.
    //
    if (*VideoOutput)
    {
        DBG_VAL_PTR(*VideoOutput);
        grInvalidateVideoPatchcord(*VideoOutput);
        (*VideoOutput)->Source = NULL;
        *VideoOutput           = NULL;
    }
    if (Data)
    {
        //
        // Validate patchcord and connect.
        //
        status = fifoSearchObject(pDev, Data, Object->ChID, (POBJECT *)&VideoPatchcord);
        if (status)
            return (status);
        if (VideoPatchcord->Base.Class->Type != NV_PATCHCORD_VIDEO)
            return (RM_ERR_OBJECT_TYPE_MISMATCH);
        //
        // Unplug patchcord if already in use.
        //    
        if (VideoPatchcord->Source)
        {
            DBG_VAL_PTR(VideoPatchcord->Source);
            grInvalidateVideoPatchcord(VideoPatchcord);
            switch (VideoPatchcord->Source->Base.Class->Type)
            {
                /*
                case NV_IMAGE_TO_VIDEO:
                    ((PIMAGEVIDEOOBJECT)VideoPatchcord->Source)->VideoOutput = NULL;
                    break;
                */                    
                case NV_VIDEO_FROM_MEMORY:
                    ((PVIDEOFROMMEMOBJECT)VideoPatchcord->Source)->VideoOutput = NULL;
                    break;
                /*                    
                case NV_VIDEO_SOURCE:
                    ((PVIDEOSOURCEOBJECT)VideoPatchcord->Source)->VideoOutput = NULL;
                    break;
                */                    
                default:
                    VideoPatchcord->Source->VideoOutput = NULL;
                    break;
            }
        }
        //
        // Insert this connection into the first available slot.
        //
        VideoPatchcord->Source = (PVIDEOOBJECT)Object;
        *VideoOutput = VideoPatchcord;
    }
    return (RM_OK);
#endif // KJK
}

//---------------------------------------------------------------------------
//
//  Graphics class methods that map directly to hardware.
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//
//  Colormap methods.
//
//---------------------------------------------------------------------------

RM_STATUS mthdSetColormapCtxDma
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    RM_STATUS            status;
    PVIDEOCOLORMAPOBJECT ColormapObject;
    
    ColormapObject = (PVIDEOCOLORMAPOBJECT)Object;
    //
    // Check for a previous translation.  Free its resource before overwriting.
    //
    if (Data == NV1_NULL_OBJECT)
    {
        ColormapObject->Xlate = NULL;
        return (RM_OK);
    }
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(ColormapObject->Xlate));
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }
    return (RM_OK);
}
RM_STATUS mthdSetColormapFormat
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    ((PVIDEOCOLORMAPOBJECT)Object)->ColorFormat = Data;
    return (RM_OK);
}
RM_STATUS mthdSetColormapStart
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    ((PVIDEOCOLORMAPOBJECT)Object)->Start = Data;
    return (RM_OK);
}
RM_STATUS mthdSetColormapLength
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    ((PVIDEOCOLORMAPOBJECT)Object)->Length = Data;
    return (RM_OK);
}
RM_STATUS mthdColormapDirtyStart
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    ((PVIDEOCOLORMAPOBJECT)Object)->DirtyStart = Data + ((PVIDEOCOLORMAPOBJECT)Object)->Start;
    return (RM_OK);
}
RM_STATUS mthdColormapDirtyLength
(
    PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    RM_STATUS       status;
    PVIDEOCOLORMAPOBJECT ColormapObject;
    U032           *ColormapBuffer, *ColormapBufferBase;
    U032            StartIndex;
    U032            StopIndex;
    
    ColormapObject = (PVIDEOCOLORMAPOBJECT)Object;
    //
    // Validate length.
    //
    if ((ColormapObject->DirtyStart + Data) > (ColormapObject->Start + ColormapObject->Length))
        return (RM_ERR_INVALID_START_LENGTH);
    ColormapObject->DirtyLength = Data;
    //
    // Validate enumeration parameters.
    //
    if (!ColormapObject->Xlate)
        return (RM_ERR_INVALID_XLATE);
	/* KJK
    status = dmaAttach(ColormapObject->Xlate);
    if (status)
        return (status);
	*/
    status = dmaValidateXlate(ColormapObject->Xlate,
                              ColormapObject->Start,
                              ColormapObject->Length);
    if (status)
        return (status);
	/* KJK
    status = dmaBeginXfer(ColormapObject->Xlate, NULL);
    if (status)
        return (status);
	*/
    //
    // Get buffer translation.
    //
    status = dmaGetMappedAddress(pDev, ColormapObject->Xlate,
                                 ColormapObject->DirtyStart,
                                 ColormapObject->DirtyLength,
                                 (VOID **)&ColormapBuffer);
    if (status)
        return (status);
        
    // save original ptr value    
	ColormapBufferBase = ColormapBuffer;
    
    //
    // Load new colormap into palette copy.
    //
    if (pDev->Dac.UpdateFlags & UPDATE_HWINFO_DAC_COLORMAP)
        pDev->Dac.UpdateFlags &= ~UPDATE_HWINFO_DAC_COLORMAP;
    StartIndex = (ColormapObject->DirtyStart - ColormapObject->Start) >> 2;
    StopIndex  = StartIndex + ((ColormapObject->DirtyLength) >> 2);
    if (StopIndex > 255)
        StopIndex = 255;
    while (StartIndex <= StopIndex)
    {
        ColormapObject->ColorMap[StartIndex] = *ColormapBuffer;
        if (pDev->Dac.HalInfo.Depth == 8)
            pDev->Dac.Palette[StartIndex++]     = *ColormapBuffer++;
        else    
            pDev->Dac.Gamma[StartIndex++]       = *ColormapBuffer++;
    }
    pDev->colormapObjectToNotify = ColormapObject;
    pDev->Dac.UpdateFlags |= UPDATE_HWINFO_DAC_COLORMAP;
    //KJK dmaEndXfer(ColormapObject->Xlate, NULL);
    //dmaDetach(ColormapObject->Xlate);    
    return (RM_OK);
}

RM_STATUS mthdColormapDirtyNotify
(
    PHWINFO   pDev,
    RM_STATUS status
)
{
    RM_STATUS rmStatus;
    PVIDEOCOLORMAPOBJECT pColormapObj;
    
    pColormapObj = pDev->colormapObjectToNotify;
    
    if (pColormapObj == NULL)
        return RM_OK;
        
    rmStatus = notifyFillNotifierArray(pDev, pColormapObj->CBase.NotifyXlate,
                                       0 /* info32 */, 0 /* info16 */, 
                                       status,
                                       NVFF9_NOTIFIERS_COLORMAP_DIRTY_NOTIFY);
    //
    // Do any OS specified action related to this notification.
    //
    if (pColormapObj->CBase.NotifyAction)
    {
        osNotifyEvent(pDev,
                      (POBJECT)pColormapObj, 
                      NVFF9_NOTIFIERS_COLORMAP_DIRTY_NOTIFY,
                      0 /* Method */, 
                      0 /* Data */, 
                      status, 
                      pColormapObj->CBase.NotifyAction);
    }
                                       
    pDev->colormapObjectToNotify = NULL;
    return rmStatus;
}

//---------------------------------------------------------------------------
//
//  Graphics notification on completion of VBlank.
//
//---------------------------------------------------------------------------

RM_STATUS grVideoSwitchComplete
(
	PHWINFO   pDev,
    POBJECT   Object,
    U032      Method,
    V032      Data,
    RM_STATUS Status
)
{
    notifyFillNotifier(pDev, ((PVIDEOSWITCHOBJECT)Object)->VBlankNotify[Data].Xlate, 0, 0, Status);
    //KJK dmaEndXfer(((PVIDEOSWITCHOBJECT)Object)->VBlankNotify[Data].Xlate, NULL);
    //
    // Do any OS specified action related to this notification.
    //
    if (((PVIDEOSWITCHOBJECT)Object)->VBlankNotify[Data].Action)
    {
        osNotifyEvent(pDev,
                      Object,
        			  0,
                      0x0400 + (Data << 2),
                      Object->Name,
                      Status,
                      ((PVIDEOSWITCHOBJECT)Object)->VBlankNotify[Data].Action);
    }
    ((PVIDEOSWITCHOBJECT)Object)->VBlankNotify[Data].Pending = FALSE;
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Default HW method handler for graphics engine classes.
//
//---------------------------------------------------------------------------

RM_STATUS grHWMethod
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: grHWMethod class ", Object->Class->Type);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:            chid ", Object->ChID);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:            handle ", Object->Name);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:            offset ", Offset);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:            data ", Data);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:            instance ", ((PCOMMONOBJECT)Object)->Instance);
    DBG_BREAKPOINT();

    //
    // THIS IS A HARDWARE METHOD...
    //
    // We'll only be here if we get an ILLEGAL_MTHD exception
    // from graphics.  We don't attempt any recovery, just drop
    // it on the floor and continue...
    //
    return (RM_OK);
}
