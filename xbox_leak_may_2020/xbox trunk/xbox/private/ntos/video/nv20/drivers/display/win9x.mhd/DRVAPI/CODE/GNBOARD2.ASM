;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnboard2.asm
;
; Purpose:      This file holds run-time routines for GNBOARDCX.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include minivdd.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include modeext.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_BoardMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               dwMessage               which message
;               dwData1                 additional data that depends
;                                       upon the message type
;               dwData2                 additional data that depends
;                                       upon the message type
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_BoardMessage, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
OPENPROC
        PUSHR   esi
        mov     esi,pGNBoardCXOffset

        mov     eax,dwMessage
        cmp     eax,MSG_PREMODESET
        je      PreMS
        cmp     eax,MSG_POSTMODESET
        je      PostMS
        cmp     eax,MSG_DIRECTX_ENUM
        je      DXEnum
        cmp     eax,MSG_DIRECTXON
        je      DXOn
        cmp     eax,MSG_DIRECTXOFF
        je      DXOff
        cmp     eax,MSG_DOSON
        je      DosOn
        cmp     eax,MSG_DOSOFF
        je      DosOff
        cmp     eax,MSG_POWERON
        je      PowerOn
        cmp     eax,MSG_POWEROFF
        je      PowerOff
        cmp     eax,MSG_DISABLE
        je      DrvDis
        jmp     ChainIt

PreMS:
        ; Free up the memory used for offscreen allocations.
        ; This will appropriately flush all bitmaps to system
        ; memory as required.
        push    pGNBoardCXOffset
        pushd   0
        call    GN_ControlOffscreenAllocations
        jmp     ChainIt

PostMS:
        ; Alloc the memory used for offscreen allocations
        ; if DirectX is not running.
        cmp     ds:[esi].GNBOARDCX.dwDirectXActive,0
        jne     @F
        push    pGNBoardCXOffset
        pushd   1
        call    GN_ControlOffscreenAllocations
@@:
        ; We are no longer initting after the first modeset
        mov     ds:[esi].GNBOARDCX.dwInitting,0
        jmp     ChainIt

DXEnum:
        jmp     ChainIt

DXOn:
        ; DirectX is coming up.
        mov     ds:[esi].GNBOARDCX.dwDirectXActive,1

        ; Flush all offscreen bitmaps
        ; Free up the memory used for offscreen allocations.
        push    pGNBoardCXOffset
        pushd   0
        call    GN_ControlOffscreenAllocations
        jmp     ChainIt

DXOff:
        ; DirectX is going away.
        mov     ds:[esi].GNBOARDCX.dwDirectXActive,0

        ; Alloc the memory used for offscreen allocations
        push    pGNBoardCXOffset
        pushd   1
        call    GN_ControlOffscreenAllocations
        jmp     ChainIt

DosOff:
PowerOn:
        ; Clear H/W busy for ALL logical devices.
        mov     ebx,ds:[esi].GNBOARDCX.pGNLogdevCX
        mov     ecx,ds:[ebx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        lea     ebx,ds:[esi].GNBOARDCX.pGNLogdevCX

@@:
        mov     eax,ds:[ebx]
        or      eax,eax
        je      NextLogdevOff

        ; Enable HW access to device
        mov     ds:[eax].GNLOGDEVCX.dwHardwareBusy,0

NextLogdevOff:
        add     ebx,4
        loop    @B
        jmp     ChainIt

DosOn:
PowerOff:
        ; Flush all offscreen bitmaps
        push    pGNBoardCXOffset
        call    GN_ClearOffscreenBitmapCache

        ; Set H/W busy for ALL logical devices.
        mov     ebx,ds:[esi].GNBOARDCX.pGNLogdevCX
        mov     ecx,ds:[ebx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        lea     ebx,ds:[esi].GNBOARDCX.pGNLogdevCX

@@:
        mov     eax,ds:[ebx]
        or      eax,eax
        je      NextLogdevOn

        ; Disable HW access to device
        mov     ds:[eax].GNLOGDEVCX.dwHardwareBusy,1

NextLogdevOn:
        add     ebx,4
        loop    @B
        jmp     ChainIt

DrvDis:
        ; Free up the memory used for offscreen allocations.
        ; ONLY if this is the primary driver. Any secondary
        ; disabling should not free the memory.
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        je      FlushOffscreen
        test    dwThisLogicalDeviceHandle_In_NVTEXT16,03H
        jnz     ChainIt

FlushOffscreen:
        push    pGNBoardCXOffset
        pushd   0
        call    GN_ControlOffscreenAllocations
        jmp     ChainIt

ChainIt:
        ; Call the hardware to inform it of the message
        push    ds:[esi].GNBOARDCX.pHWBoardCX
        push    dwMessage
        push    dwData1
        push    dwData2
        call    HW_BoardMessage
        jmp     DoneIt

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_IsDevDataUniqueOnBoard
;
; Purpose:      This routine runs through all the CRTCCXs on the board
;               and sees if anyone else is using the same device and number
;               as the CRTCCX passed in. For example, you can call
;               this routine if you want to know, "is someone else
;               using CRT4?" or "is someone else using DFP6?" or
;               "TV2", etc.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               pGNCrtcCXOffset         GNCRTCCX
;
; Returns:
;               eax                     TRUE no one else is using
;                                       the device and number in
;                                       the CRTCCX passed in
;                                       FALSE, someone is using it
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_IsDevDataUniqueOnBoard, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   pGNCrtcCXOffset
LOCALD  dwDesiredDevData
OPENPROC
        PUSHR   esi,edi
        push    pGNCrtcCXOffset
        call    GN_GetDevData
        mov     dwDesiredDevData,eax

        mov     eax,pGNBoardCXOffset

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev
        sub     edx,edx

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc

        ; Is this the CRTCCX that we are comparing against?
        cmp     esi,pGNCrtcCXOffset
        je      NextCrtc

        ; No, so see if this enumerated CRTCCX's DevData is
        ; the same as the DevData in the passed in CRTCCX
        PUSHR   eax,ebx,ecx,edx
        push    esi
        call    GN_GetDevData
        cmp     ax,word ptr dwDesiredDevData
        POPR    eax,ebx,ecx,edx
        je      NotUnique

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

Unique:
        mov     eax,1
        jmp     DoneIt

NotUnique:
        sub     eax,eax
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_FindFirstCloneDevice
;
; Purpose:      This routine runs through all the GNLOGDEVCXs on the
;               board looking to see if any of them have a clone device.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:
;               ebx                     GNLOGDEVCX ptr if one is found
;                                       that has a clone device.
;                                       Else 0
;               eax                     Which phys device number it is
;                                       if ebx is not 0.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_FindFirstCloneDevice, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
LOCALD  dwDesiredDevData
OPENPROC
        PUSHR   esi,edi
        mov     eax,pGNBoardCXOffset

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev

        ; Start with the first clone
        mov     edx,1

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc

        ; Ah! We found a clone device. Let's return it
        mov     ebx,ecx
        mov     eax,edx
        jmp     DoneIt

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        ; Didn't find one
        sub     ebx,ebx
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_SendDevicesRemapMsgToAllLogDevs
;
; Purpose:      This routine runs through all the GNLOGDEVCXs on the
;               board and sends a MSG_DEVICESREMAPPED to each one.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SendDevicesRemapMsgToAllLogDevs, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNBoardCXOffset

        ; Set up a loop to enumerate logical devices
        sub     edi,edi

LoopLogdev:
        mov     eax,ds:[esi].GNBOARDCX.pGNLogdevCX[edi * 4]
        or      eax,eax
        je      NextLogdev

        push    eax
        pushd   MSG_DEVICESREMAPPED
        push    eax
        push    ds:[eax].GNLOGDEVCX.Dibeng.delpPDevice
        call    GN_LogdevMessage

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     edi
        cmp     edi,ds:[esi].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_FindLogdevWaitingToClone
;
; Purpose:      This routine runs through all the GNLOGDEVCXs on the
;               board looking to see if any of them would like to
;               have a clone device. (Generally this is the result
;               of a clone device having been "Stolen" away from them
;               if someone switches to multi-mon on the fly.)
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:
;               eax                     GNLOGDEVCX ptr if one is found
;                                       that wants a clone device.
;                                       Else 0
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_FindLogdevWaitingToClone, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset

        ; Set up a loop to enumerate logical devices
        sub     ecx,ecx

LoopLogdev:
        mov     eax,ds:[ebx].GNBOARDCX.pGNLogdevCX[ecx * 4]
        or      eax,eax
        je      NextLogdev

        cmp     ds:[eax].GNLOGDEVCX.dwNumClonesAwaited,0
        jne     DoneIt

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ecx
        cmp     ecx,ds:[ebx].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        ; Didn't find one
        sub     eax,eax
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     GN_FindUnusedDevice
;
; Purpose:      This routine finds a device (and number -- like CRT6 or
;               DFP2), that is connected to the system and is not in use
;               by any other CRTCCX.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               dwUnusedIndex           0, 1, 2, 3, etc
;                                       This tells which unused device
;                                       to find. The zeroth, first
;                                       second, etc. They are always
;                                       ordered as CRTS, DFPS, TVS
; Returns:
;               eax                     DEVDATA describing device
;                                       0x0FFFFFFFF if couldn't find one
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_FindUnusedDevice, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   dwUnusedIndex
LOCALD  dwAllAttachedDevices
LOCALD  dwCurrentIndex
OPENPROC
        PUSHR   esi,edi

        ; First get a list of all the devices that exist and
        ; are attached.
        mov     eax,pGNBoardCXOffset
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        call    HW_GetAllAttachedDevices

        ; If there is no display device present, and this is the primary
        ; driver, then we need to drive the CRT even though it is not present.
        ; However, we only want to do this if this is not a mobile system
        ; since a mobile system is guaranteed to have an LCD, and its absence
        ; along with the absence of any other display device is extremely
        ; bad news.
        cmp     eax,0
        jne     DeviceMapCont
        
        ; Only force a device which is not present to be treated as present
        ; if we are the primary driver!
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        jne     DeviceMapCont

        ; This routine returns whether or not this is mobile behavior.
        mov     eax,pGNBoardCXOffset
        mov     ecx,dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX[0]
        push    ds:[ecx].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetBootDeviceSelection,es,ax
        cmp     ax,0
        jne     ReturnNoDevice
        mov     eax,1                   ; return CRT is attached
        jmp     DeviceMapCont
        
ReturnNoDevice:
        mov     eax,0
        
DeviceMapCont:
        mov     dwAllAttachedDevices,eax

        PUSHR   esi,edi
        mov     eax,pGNBoardCXOffset
        mov     dwNumDevices,0

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev
        sub     edx,edx

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc

        ; Get the dev data for this CRTCCX
        PUSHR   eax,ebx,ecx,edx
        push    esi
        call    GN_GetDevData

        ; watch out for the "no device" case
        cmp     eax,-1
        je      NoDevice

        ; Form a mask out of it
        mov     ecx,eax
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl

        ; Remove this bit from the list of attached devices
IFDEF DEBUG
        ; unless someone has hot-unplugged a device, the bit
        ; we are touching should currently be ON since it is
        ; the device that some CRTCCX is actively using
        test    eax,dwAllAttachedDevices
        jnz     @F
        int     3
@@:
ENDIF
        not     eax
        and     dwAllAttachedDevices,eax

NoDevice:
        POPR    eax,ebx,ecx,edx

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        ; If dwAllAttachedDevices if not 0, then there is an
        ; available device. If it is 0, then there isn't a device.
        mov     ecx,dwAllAttachedDevices
        or      ecx,ecx
        je      FailIt

        ; First look at CRTS
        mov     ebx,-1

        mov     eax,DEVTYPE_CRT + 0FF00H
@@:     inc     ah
        test    cl,1
        jz      Next1
        inc     ebx
        cmp     ebx,dwUnusedIndex
        je      DoneIt
Next1:  shr     cl,1
        cmp     ah,8
        jc      @B

        ; Nope. Try DFPS
        shr     ecx,8
        mov     eax,DEVTYPE_DFP + 0FF00H
@@:     inc     ah
        test    ch,1
        jz      Next2
        inc     ebx
        cmp     ebx,dwUnusedIndex
        je      DoneIt
Next2:  shr     ch,1
        cmp     ah,8
        jc      @B

        ; Lasthope is TVS
        mov     eax,DEVTYPE_TV + 0FF00H
@@:     inc     ah
        test    cl,1
        jz      Next3
        inc     ebx
        cmp     ebx,dwUnusedIndex
        je      DoneIt
Next3:  shr     cl,1
        cmp     ah,8
        jc      @B

FailIt:
        mov     eax,-1
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetAllDeviceData
;
; Purpose:      This routine loads up three arrays. One will hold all the
;               devices/numbers needed by all CRTCCXs, the other will
;               hold the ptrs to the GNCRTCCXs and the last will hold
;               the ptrs to the corresponding HWCRTCCXs which want to
;               display on the corresponding device and device number
;               in the first array.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               lpDevData               DEVDATA array ptr
;               lpHWCrtcCX              ULONG array ptr (for HW Crtc CXs)
;               Maximum array length
;
; Returns:
;               eax                     number of devices found
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetAllDeviceData, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   lpDevData
PARMD   lpGNCrtcCX
PARMD   lpHWCrtcCX
PARMD   dwMaxLen
LOCALD  lpDevData_Inc
LOCALD  lpGNCrtcCX_Inc
LOCALD  lpHWCrtcCX_Inc
LOCALD  dwNumDevices
OPENPROC
        PUSHR   esi,edi
        mov     eax,lpDevData
        mov     lpDevData_Inc,eax
        mov     eax,lpHWCrtcCX
        mov     lpHWCrtcCX_Inc,eax
        mov     eax,lpGNCrtcCX
        mov     lpGNCrtcCX_Inc,eax
        mov     eax,pGNBoardCXOffset
        mov     dwNumDevices,0

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev
        sub     edx,edx

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc

        ; Get the dev data for this CRTCCX
        PUSHR   eax,ebx,ecx,edx
        push    esi
        call    GN_GetDevData
        les     di,lpDevData_Inc
        mov     es:[di],eax
        add     lpDevData_Inc,4

        ; Put the corresponding HW Crtc CX ptr in the other array
        les     di,lpGNCrtcCX_Inc
        mov     es:[di],esi
        add     lpGNCrtcCX_Inc,4
        mov     eax,ds:[esi].GNCRTCCX.pHWCrtcCX
        les     di,lpHWCrtcCX_Inc
        mov     es:[di],eax
        add     lpHWCrtcCX_Inc,4
        POPR    eax,ebx,ecx,edx

        inc     dwNumDevices

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        ; return the number of devices
        mov     eax,dwNumDevices
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetDevDataMask
;
; Purpose:      This routine runs through all the CRTCCX in all
;               GNLOGDEVCXs on this board with the exception of
;               the CRTCCX passed in and forms a mask out of the
;               DEVDATAs.  The mask is as follows. The low byte
;               as a 1 in each bit where there is a CRTC in use
;               (i.e. bit 0 is set if some CRTCCX was using CRTC0, etc.)
;               Now, the next byte is the same thing for TVs and
;               the high byte is the same thing for DFPs.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
; Returns:
;               eax                     device mask described in purpose
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetDevDataMask, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     eax,pGNBoardCXOffset
        sub     edi,edi

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev
        sub     edx,edx

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc
        cmp     esi,pGNCrtcCXOffset
        je      NextCrtc

        ; Get the dev data for this CRTCCX and OR it into the mask
        PUSHR   eax,ebx,ecx,edx
        push    esi
        call    GN_GetDevData
        shl     al,3
        add     al,ah
        mov     cl,al
        mov     ebx,1
        shl     ebx,cl
        or      edi,ebx
        POPR    eax,ebx,ecx,edx

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

        ; return the mask
        mov     eax,edi
        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

