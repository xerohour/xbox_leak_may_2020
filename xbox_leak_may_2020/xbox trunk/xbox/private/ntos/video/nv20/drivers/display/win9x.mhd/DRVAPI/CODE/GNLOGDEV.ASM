;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnlogdev.asm
;
; Purpose:      This file holds the init and deinit of the
;               GNLOGDEVCX.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include minivdd.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc
include modeext.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_CreateLogdevContext
;
; Purpose:      This routine initializes a GNLOGDEV structure.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:      eax     0       failure
;                       else    Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CreateLogdevContext, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
LOCALD  dwVirtualDesktopState
LOCALD  dwCRTCIndex
LOCALV  sDispInfo,DISPLAYINFO
OPENPROC
        PUSHR   esi,edi
        sub     esi,esi

        ; Allocate memory for the GNLOGDEVCX
        pushd   SIZE GNLOGDEVCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax

        ; Crate the HW logical device next
        mov     eax,pGNBoardCXOffset
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        call    HW_CreateLogdevContext
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].GNLOGDEVCX.pHWLogdevCX,eax

        ; We are initting. This will be cleared after the first modeset.
        mov     ds:[esi].GNLOGDEVCX.dwInitting,1

        ; Fill in the logical board ptr
        mov     eax,pGNBoardCXOffset
        mov     ds:[esi].GNLOGDEVCX.pGNBoardCX,eax

        ; Save off the code segment values -- Ugh!
        mov     ds:[esi].GNLOGDEVCX.dw16BitCodeSegment,_NVTEXT16
        mov     ds:[esi].GNLOGDEVCX.dw32BitCodeSegment,_NVTEXT32

	; This gets initted to -1
        mov     ds:[esi].GNLOGDEVCX.dwCplData,-1

        ; We need a 16:16 ptr in the GNLOGDEVCX.Dibeng.deBitmapInfo
        ; field to the GNCRTCCX.bmi field.
        mov     eax,1
        lea     ebx,[esi].GNLOGDEVCX.bmi
        mov     ecx,SIZE DIBHEADER + 410H       ; 400H for palette table +
                                                ; some slack just in case
        ; Fill in the DIBENG header, Default to 640x480x8
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        sub     edx,edx
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitmapInfo,eax
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deType,'RP'
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deWidth,640
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deHeight,480
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deWidthBytes,640
        mov     ds:[esi].GNLOGDEVCX.Dibeng.dePlanes,1
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel,8
        mov     dword ptr ds:[esi].GNLOGDEVCX.Dibeng.bRealBpp,8
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan,640
        mov     ds:[esi].GNLOGDEVCX.Dibeng.delpPDevice,edx
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset,edx

        ; Get the frame buffer selector from the GNBOARDCX
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_GetFrameBufferSelector
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector,ax
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deFlags,MINIDRIVER + VRAM
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deVersion,400H
        mov     eax,OFFSET GENERIC_BeginAccess
        mov     ecx,OFFSET GENERIC_EndAccess
        mov     dx,SEG GENERIC_BeginAccess
        shl     edx,10H
        mov     dx,ax
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBeginAccess,edx
        mov     dx,cx
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deEndAccess,edx

        ; Fill in the bmi Header
        sub     edx,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biSize,SIZE DIBHEADER
        mov     ds:[esi].GNLOGDEVCX.bmi.biWidth,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biHeight,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biPlanes,1
        mov     ds:[esi].GNLOGDEVCX.bmi.biBitCount,dx
        mov     ds:[esi].GNLOGDEVCX.bmi.biCompression,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biSizeImage,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biXPelsPerMeter,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biYPelsPerMeter,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biClrUsed,edx
        mov     ds:[esi].GNLOGDEVCX.bmi.biClrImportant,edx

        ; Assume 0 for these right now
        mov     ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard,0
        mov     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,0

        ; Get the number of CRTCS
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_GetNumCRTCs
        mov     ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard,eax

        ; Do we have virtual desktops? The registry knows.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_GetBoardRegistryPath
        mov     ds:[esi].GNLOGDEVCX.lpBoardRegistryData,eax

        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0

        ; Are they using 1-5-5-5 or 5-6-5 mode for this logical device
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_Get16BppPixelFormat
        mov     ds:[esi].GNLOGDEVCX.dw16BppFormat,eax

        ; Are they bitpacked or byte packed fonts
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_GetTextFormat
        mov     ds:[esi].GNLOGDEVCX.dwFontFormat,eax

        ; Init the save screen bitmap stuff
        push    esi
        call    GN_ClearSaveScreenBitmapCache

        ; Init the macrovision stuff
        mov     ds:[esi].GNLOGDEVCX.dwCurrentMacrovisionKey,-1
        mov     ds:[esi].GNLOGDEVCX.dwNextMacrovisionKey,99999999H
        mov     ds:[esi].GNLOGDEVCX.dwMacrovisionMode,0
        mov     ds:[esi].GNLOGDEVCX.dwMacrovisionNavigatorID,0

        ; Hook int2F. This protects itself from hooking more than once
        ; and also will not hook unless we are the primary device.
        push    pGNLogdevCXOffset
        call    GN_HookInt2F

        mov     eax,esi
DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the GNLOGDEVCX. was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; GN_DestroyLogdevContext can clean up partial inits
        push    esi
        call    GN_DestroyLogdevContext
        sub     eax,eax

ExitIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_DestroyLogdevContext
;
; Purpose:      This routine destroys a GNLOGDEVCX structure.
;               You do not need to free its memory. Just free any
;               resources inside the structure.
;
; Arguments:
;               ds              Context Selector
;               pGNLogdevCXOffset       Offset to the pdev context data
;
; Returns:      eax     0       failure
;                       else    Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DestroyLogdevContext, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALD  pGNBoardCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset
        or      esi,esi
        je      DoneIt

        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     pGNBoardCXOffset,eax

        ; UnHook int2F. This routine will not unhook if we are not
        ; already hooked.
        push    pGNLogdevCXOffset
        call    GN_UnhookInt2F

        sub     edi,edi
@@:
        cmp     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        je      NextCrtc

        call    GN_VddIsDriverAttachedToDesktop
        or      eax,eax
        jnz     DoNotInitResolutionForClone

        ; Clear panning flag to prevent the next clone limitation
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        pushd   0
        CALLFAR16IND lpfnSetClonePanningFlag,es,ax

DoNotInitResolutionForClone:
        push    esi
        push    edi
        call    GN_LogdevDetachCrtc
NextCrtc:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      @B

        ; Free the primary surface memory.
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        call    HW_FreePrimarySurface
        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
@@:
        ; Free the selector we were using for the BitmapInfo
        mov     eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitmapInfo
        or      eax,eax
        je      Skip0
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitmapInfo,0
Skip0:
        cmp     ds:[esi].GNLOGDEVCX.pHWLogdevCX,0
        je      @F
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_DestroyLogdevContext
        mov     ds:[esi].GNLOGDEVCX.pHWLogdevCX,0
@@:
        ; Clear out a few fields
        push    esi
        call    HW_FreeFlatMemory

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetGdiInfoPtr
;
; Purpose:      This routine sets the GDIINFO ptr in the GNLOGDEVCX.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX
;               lpThisGdiInfo           16:16 ptr to GdiInfo table
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetGdiInfoPtr, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   lpThisGdiInfo
LOCALD  dwState
OPENPROC
        mov     eax,lpThisGdiInfo
        mov     edx,pGNLogdevCXOffset
        mov     ds:[edx].GNLOGDEVCX.lpGdiInfo,eax

        ; OK, it is kind of weird to put this code in this routine,
        ; but there is an order problem. We can do this at create
        ; time because we don't know the value of
        ; dwLogicalDeviceInstanceOnThisBoard at that time. But by
        ; the time we get called here, we do know it. So....
        push    ds:[edx].GNLOGDEVCX.lpBoardRegistryData
        push    ds:[edx].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        mov     ax,ss
        shl     eax,10H
        lea     ax,dwState
        push    eax
        CALLFAR16IND lpfnGetVirtualDesktop,es,ax
        mov     edx,pGNLogdevCXOffset
        mov     eax,dwState
        mov     ds:[edx].GNLOGDEVCX.dwVirtualDesktopEnabled,eax
CLOSEPROC


;==============================================================================
;
; Function:     GN_LogdevAttachCrtc
;
; Purpose:      This routine attaches a CRTC to a logical device
;               giving a viewable display to a logical desktop.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      eax     0       failure
;                       else    Success
;                               In this case, ebx is the physical device num
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_LogdevAttachCrtc, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALD  pGNCrtcCXOffset
LOCALD  dwPhysicalDeviceNum
LOCALD  dwDeviceCount
LOCALD  dwIsBootCrtc
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aGNCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALS  aHWCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpGNCrtcCX
LOCALD  lpHWCrtcCX
LOCALD  dwUnusedIndex
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        ; Find a free attachment point
        sub     eax,eax
        sub     ebx,ebx
@@:
        cmp     ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4],0
        je      FoundIt
        inc     ebx
        cmp     ebx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      @B
        jmp     DoneIt

FoundIt:
        mov     dwPhysicalDeviceNum,ebx

        ; If this is PhysicalDevice0 on the primary driver, then this
        ; is the boot crtc.
        sub     edx,edx
        or      ebx,ebx
        jne     @F
        mov     dx,SEG dwThisLogicalDeviceHandle
        mov     es,dx
        mov     edx,es:[dwThisLogicalDeviceHandle]
        cmp     edx,1
        sete    dl
        movzx   edx,dl
@@:     mov     dwIsBootCrtc,edx

        ; OK, this is a bit tricky. If this is dwPhysicalDeviceNum = 0,
        ; then we are here because a driver is loading. In that case,
        ; it is because we are booting, OR because we are running multimon
        ; and someone enabled another device on the Windows Settings page.
        ; We want to allow this driver to come up if at all possible.
        ; The one thing that might hinder us is if there are no more
        ; physical CRTCS available to assign to this driver as the primary.
        ; Let's see if this is the case currently.

        ; How many CRTCS are currently in use?
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      AnAvailCrtc

        ; All CRTCs are currently in use. However, if any of them
        ; is currently being used as a clone device, we will steal
        ; it away so that we can use it as the primary for this
        ; driver.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_FindFirstCloneDevice
        or      ebx,ebx
        je      DoneIt

        ; HA! We found a GNLOGDEVCX with a clone device
        ; Bump the number of clones awaited by 1 in the logical device
        ; that we are stealing the CRTCCX from. When we destroy the Logdev
        ; which stole the clone away, we will give it back to a logdev
        ; which is awaiting clones (in GN_DestroyLogdevContext).
        inc     ds:[ebx].GNLOGDEVCX.dwNumClonesAwaited

        ; Remove the clone
        push    ebx
        push    eax
        call    GN_LogdevDetachCrtc

        ; Now there should be an available CRTCCX

AnAvailCrtc:
        ; Create a GNCRTCCX
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        push    dwPhysicalDeviceNum
        push    dwIsBootCrtc
        call    GN_CreateCrtcContext
        or      eax,eax
        je      DoneIt
        mov     pGNCrtcCXOffset,eax

        ; This is UGLY and a bit of a hack right now. In multiomon
        ; this routine can be called with the context of the other
        ; driver. That is fine, but CreateCrtcContext stores the
        ; address of the function GN_Far16GetModeSize in the GNCRTCCX.
        ; The problem is that the code segment is incorrect because
        ; the code seg it stores is for this driver! Ugh! One driver
        ; is what we really want with all data just context. But for now..
        mov     ecx,ds:[esi].GNLOGDEVCX.dw16BitCodeSegment
        lea     edx,[eax].GNCRTCCX.sNvmodeDispData
        mov     word ptr ds:[edx].DISPDATA.lpfnGetModeSize+2,cx
        mov     word ptr ds:[edx].DISPDATA.lpfnIsModeAcceptedByDriver+2,cx

        ; This is tentative! We haven't yet determined if it is OK
        ; to attach the CRTC, so we have to clear it out if it fails.
        ; We need to do it though for some of the other tests to work.
        mov     ebx,dwPhysicalDeviceNum
        mov     ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4],eax

        ; Get the Device Data specifying the device the CRTC desires
        push    pGNCrtcCXOffset
        call    GN_GetDevData
        mov     dword ptr LocalDevData,eax

        ; Make sure that the device desired by this CRTC is not
        ; in use by any other active CRTCCX on the board.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    pGNCrtcCXOffset
        call    GN_IsDevDataUniqueOnBoard
        or      eax,eax
        mov     dwUnusedIndex,0
        jne     IsUnique

TryOther:
        ; Nope! The device is in use!
        ; Set the device used by the CRTCCX to nothing
        push    pGNCrtcCXOffset
        pushd   0FFFFFFFFH
        call    GN_SetDevData

        ; Find an unused device
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    dwUnusedIndex
        call    GN_FindUnusedDevice

        ; If there isn't an unused device, then abort the attach
        cmp     eax,-1
        je      FailIt
        mov     dword ptr LocalDevData,eax

        ; Find unused device can find a device type and a device
        ; number. However, in order to get a device format (like
        ; NTSCM, or PALA), we need to ask the ResMan what the
        ; last format was on that device. Right now, this only
        ; applies if the device type is a tv, but for consistency
        ; I always get the last display format. For non-tv's,
        ; this will simply set the format to 0.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        jne     GotDevData
        mov     eax,pGNCrtcCXOffset
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        call    HW_GetResManTvFormat
        shl     eax,10h
        and     dword ptr LocalDevData,0ff00ffffh
        or      dword ptr LocalDevData,eax
        jmp     GotDevData

IsUnique:
        ; OK, sure it's unique, but is the device connectable to some
        ; head? (any head - it doesn't matter right now which one.)
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    dword ptr LocalDevData
        call    HW_IsDeviceAttached
        or      eax,eax
        je      TryOther

        ; If this device doesn't work, then dwUnusedIndex will
        ; be incremented and we will start scanning at the zeroth
        ; unused device for others that might.
        mov     dwUnusedIndex,-1

GotDevData:
        ; OK, tentatively, we will allow this CRTCCX to be
        ; the device type / number found.
        push    pGNCrtcCXOffset
        push    dword ptr LocalDevData
        call    GN_SetDevData

        ; OK, we have a unique device (from the others in use)
        ; that is connected to the system. Now can we find an
        ; arrangment whereby this device can be used?
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData
        mov     dwDeviceCount,eax

        ; Ask the hardware if it is possible to have all the
        ; devices specified enabled simultaneously by assigning
        ; each to a different head.
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    lpLocalDevData
        push    dwDeviceCount
        call    HW_CanDevicesBeMapped
        or      eax,eax
        jne     GoodToGo

        ; If we only have one device, failing the mapping is fatal.
        cmp     dwDeviceCount,1
        je      FailIt

        ; Failing the mapping when there is more than one device
        ; may simply mean that the set of devices requested cannot
        ; all be used simultaneously. But there might be a different
        ; device which is available which could be used for the new
        ; CRTC we are trying to attach.
        inc     dwUnusedIndex
        jmp     TryOther

GoodToGo:
        ; OK, NOW we can read the EDID
        mov     edi,pGNCrtcCXOffset
        lea     ebx,[edi].GNCRTCCX.sCrtcModeInfo
        cmp     ds:[ebx].MODEINFO.MIDevData.cType,DEVTYPE_TV
        je      SkipEdidRead
        
        mov     ds:[edi].GNCRTCCX.dwEdidSize,MAX_EDID_BUFFER_SIZE
        push    ds:[edi].GNCRTCCX.pHWCrtcCX
        push    ds:[ebx].MODEINFO.MIDevData
        lea     eax,[edi].GNCRTCCX.sEdidBuffer
        push    eax
        lea     eax,[edi].GNCRTCCX.dwEdidSize
        push    eax
        call    HW_ReadEdid
        
SkipEdidRead:
        ; Write out the display device for the CRTCCX
        push    pGNCrtcCXOffset
        call    GN_GetDevData
        mov     dword ptr LocalDevData,eax
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        push    dwPhysicalDeviceNum
        push    lpLocalDevData
        CALLFAR16IND lpfnSetDisplayDevice,es,ax

        ; If this device is a TV, set the TV Format in the ResMan.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        jne     SkipTVFormat
        mov     eax,pGNCrtcCXOffset
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        call    HW_SetResManTvFormat
        
SkipTVFormat:
        ; Tell the HWLOGDEVCX about its new CRTCCX
        mov     eax,pGNCrtcCXOffset
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    dwPhysicalDeviceNum
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        call    HW_LogdevAttachCrtc

        ; One more CRTC attached to this logical device
        inc     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        mov     ebx,dwPhysicalDeviceNum
        mov     eax,1

        ; If this was a clone device that was added, then we can
        ; decrement WaitingOnAClone if it is > 0.
        or      ebx,ebx
        je      DoneIt
        sub     ds:[esi].GNLOGDEVCX.dwNumClonesAwaited,1
        jge     DoneIt
        mov     ds:[esi].GNLOGDEVCX.dwNumClonesAwaited,0
        jmp     DoneIt

FailIt:
        ; remove the CRTCCX from our array since it isn't
        ; going to work.
        sub     eax,eax
        mov     ecx,dwPhysicalDeviceNum
        mov     ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4],eax

        push    pGNCrtcCXOffset
        call    GN_DestroyCrtcContext
        sub     eax,eax

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_LogdevDetachCrtc
;
; Purpose:      This routine detaches a CRTC from a logical device
;               removing a viewable display from a logical desktop.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               dwPhysDevNum            0 based CRTCCX to detach
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_LogdevDetachCrtc, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   dwPhysDevNum
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aHWCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALD  lpHWCrtcCX
LOCALD  dwDeviceCount
LOCALD  dwDevicesChanged
LOCALD  dwDeviceMask
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ; Is there a valid GNCRTCCX in that logical slot?
        mov     edi,dwPhysDevNum
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        or      ecx,ecx
        je      DoneIt

        ; Some prep for other code
        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        ; Tell the HWLOGDEVCX to detach its CRTCCX
        ; We MUST be careful here. If DDRAW has locked
        ; our HWCRTCCX, we must use the cached copy to free
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        mov         eax,ds:[ecx].GNCRTCCX.pHWCrtcCX
        or          eax,eax
        jne         @F
        mov         eax,ds:[ecx].GNCRTCCX.pHWCrtcCXWhenLocked
        mov         ds:[ecx].GNCRTCCX.pHWCrtcCX,eax
        mov         ds:[ecx].GNCRTCCX.pHWCrtcCXWhenLocked,0
@@:
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    dwPhysDevNum
        push    eax
        call    HW_LogdevDetachCrtc

        ; What display devices will be in use AFTER we detach this
        ; CRTC. Well, GetDevDataMask will tell give us a mask of
        ; all the display devices EXCEPT the one is use by the
        ; CRTCCX passed in.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        call    GN_GetDevDataMask
        mov     dwDeviceMask,eax

        ; Signal the beginning of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    eax
        call    HW_BeginDisplayChange

        ; Destroy the CRTCCX
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        mov     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        call    GN_DestroyCrtcContext

        ; Signal the end of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDeviceMask
        call    HW_EndDisplayChange

        ; One less CRTC attached to this logical device
        dec     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        mov     eax,1

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_LogdevAddAClone
;
; Purpose:      This routine tries to adda clone display to the logical
;               device.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               dwPhysDevNum            0 based CRTCCX to detach
;
; Returns:
;               eax                     0       Failed
;                                       non-0   Success
;                                               In this case ebx holds
;                                               the physical device index
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_LogdevAddAClone, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALV  sDesktopModeInfo,MODEINFO
LOCALD  lpDesktopModeInfo
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ; OK, normal->clone means we attach a crtc
        push    esi
        call    GN_LogdevAttachCrtc

        ; If we couldn't attach, then stay in normal mode
        or      eax,eax
        je      DoneIt
        mov     edi,ebx

        ; Build up the desktop MODEINFO
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDesktopModeInfo
        mov     lpDesktopModeInfo,eax

        ; Set the MIDevData into sDesktopModeInfo.
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        call    GN_GetDevData
        mov     sDesktopModeInfo.MIDevData,eax
        
        ; Get the refresh rate from the registry.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        or      ax,ax
        jne     UseRefreshRate

        ; No mode in the registry, use 60Hz as default.
        mov     sDesktopModeInfo.dwRefreshRate,60

UseRefreshRate:
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     sDesktopModeInfo.dwXRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     sDesktopModeInfo.dwYRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        mov     sDesktopModeInfo.dwBpp,eax

        ; Only set the desktop mode into the registry of the primary
        ; display device.
        cmp     edi,0
        jne     SkipSetDesktop
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDesktopModeInfo
        push    eax
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetCloneDesktopSelection,es,ax
        cmp     ax,1
        mov     eax,0
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnSetDeviceDesktopMode,es,ax

SkipSetDesktop:
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDesktopModeInfo
        push    eax
        CALLFAR16IND lpfnSetDeviceDisplayMode,es,ax
        mov     sDesktopModeInfo.dwRefreshRate,60

        ; I know these are wrong. We haven't yet mapped devices.
        ; There is a partial chicken or egg problem here. It is
        ; not too big a deal because by the time we actually go
        ; to set the physical mode, these will be valid and we
        ; will back off if need be.
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        lea     edx,[edx].GNCRTCCX.sNvmodeDispData
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz8bpp,40000000H
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz16bpp,40000000H
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz32bpp,40000000H

        ; Get the CRTC mode (the desktop mode is just a suggestion)
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    lpDesktopModeInfo
        call    GN_GetCrtcMode

        ; There are a few things whose state we would like to grab
        ; from the primary CRTCCX and copy into this new clone device's
        ; CRTCCX.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[0 * 4]
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]

        ; First, The pan base and pitch
        mov     eax,ds:[ebx].GNCRTCCX.dwBytesPerPixelInCurrentMode
        mov     ds:[ecx].GNCRTCCX.dwBytesPerPixelInCurrentMode,eax
        mov     eax,ds:[ebx].GNCRTCCX.dwBytesPerLineInCurrentMode
        mov     ds:[ecx].GNCRTCCX.dwBytesPerLineInCurrentMode,eax

        ; One last thing. Right now the default palette / gamma table
        ; is in the new clone CRTCCX. This is probably not right. We
        ; should copy the palette and gamma table from the primary
        ; CRTCCX just as a starting point.
        PUSHR   esi,edi
        mov     ebx,esi
        mov     edx,edi
        mov     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        lea     edi,[edi].GNCRTCCX.dwPaletteTable
        mov     esi,[esi].GNLOGDEVCX.pGNCrtcCX
        lea     esi,[esi].GNCRTCCX.dwPaletteTable
        mov     ax,ds
        mov     es,ax
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]

        sub     esi,400H
        lea     esi,[esi].GNCRTCCX.dwGammaTable
        sub     edi,400H
        lea     edi,[edi].GNCRTCCX.dwGammaTable
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]
        POPR    esi,edi

        ; Copy the Cursor cache from the primary CRTCCX into the
        ; Clone CRTCCX
        PUSHR   esi,edi
        mov     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        mov     edi,ds:[edi].GNCRTCCX.pCursorCache
        mov     esi,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[esi].GNCRTCCX.pCursorCache
        mov     ecx,SIZE CURSORCACHE + (SIZE CURSORDESCEX * MAX_CACHEABLE_CURSORS)
        rep     movs byte ptr es:[edi], ds:[esi]
        POPR    esi,edi

        ; Success
        mov     eax,1
        mov     ebx,edi

DoneIt:
        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

