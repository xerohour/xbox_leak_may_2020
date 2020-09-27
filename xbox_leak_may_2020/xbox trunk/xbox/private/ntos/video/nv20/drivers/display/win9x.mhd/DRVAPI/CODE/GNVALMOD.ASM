;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnvalmod.asm
;
; Purpose:      This file holds the ValidateMode display driver entry point.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include valmode.inc
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
; Function:     GENERIC_ValidateMode
;
; Purpose:      This function is the ValidateMode driver entry
;               point. Make sure that the requested mode can be
;               supported. This will be based upon the amount of
;               memory, memory speed, output device (tv, monitor),
;               and maybe other things.
;
; Arguments:    ValidateMode display driver stack frame
;
; Returns:      eax     VALMODE_NO_UNKNOWN Can't be sure mode is supported
;                       VALMODE_YES        Mode definitely can be supported
;
; Preserve:     ds,esi,edi
;==============================================================================
PUBLIC  ValidateMode
ValidateMode    PROC    FAR
        ; Windows requires that ValidateMode be exported by name. I
        ; have the real routine GENERIC_ValidateMode equated to the
        ; same address.
        .errnz  $ - OFFSET GENERIC_ValidateMode
ValidateMode    ENDP

DECPROC GENERIC_ValidateMode, PASCAL, FRAME, FAR16
PARMD   lpValMode
LOCALD  dwCRTCCount
LOCALD  dwMemoryNeededForMode
LOCALV  sModeInfo,MODEINFO
LOCALV  sValModeCopy,DISPVALMODE
OPENPROC
        PUSHR   ds,esi,edi

        ; This may be the first call into the display driver besides
        ; the driver entry point which does not do anything. If we
        ; have not initted the driver, then just return VALMODE_YES.
        mov     eax,VALMODE_YES
        cmp     cs:[wNVTEXT16_Alias_In_NVTEXT16],0
        je      DoneIt

        ; Load the context selector and GNLOGDEVCX ptr
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ; if we are the primary, ...
        cmp     cs:[dwThisLogicalDeviceHandle_In_NVTEXT16],1
        jne     @F

        ; then don't bother trying to validate modes on a secondary
        ; right now -- just say yes.
        ; Kludge!! Peek up into GDI's stack frame and see if they
        ; want to validate on the primary or secondary
        mov     bx,ss:[bp]
        cmp     dword ptr ss:[bx + 14H],1
        je      @F
        
        ; If we fall through then we are validating a secondary mode on the
        ; primary driver. Check if this mode is overridden by the OEM.
        push    esi
        push    lpValMode
        call    GN_CheckForOEMOverrides
        or      ax,ax
        jne     FailMode
        
        ; Windows has a nasty habit of validating modes for the disabled
        ; secondary display on the primary driver. If we do not validate this
        ; mode, then Windows, in its infinite wisdom, will not let us change
        ; the mode on the primary. Since the secondary is disabled, we don't
        ; know what the actual display device is and therefore can not
        ; properly validate the mode. Therefore, we just say yes for all these
        ; modes. When the secondary display is actually enabled, the secondary
        ; driver will properly fall back to a mode that the display device can
        ; handle.
        jmp     Success
        
@@:
        ; Make sure pGNCrtcCX is not a null pointer.
        cmp     ds:[esi].GNLOGDEVCX.pGNCrtcCX,0
        je      Success

        ; Make a copy of the ValidateMode structure passed in by Windows
        ; so we can modify it if the mode is bogus.
        sub     ebx,ebx
        lfs     bx,lpValMode

        push    esi
        push    edi
        mov     cx,size DISPVALMODE
        sub     edi,edi
        mov     di,ss
        mov     es,di
        lea     di,sValModeCopy
        mov     esi,ebx
        rep     movs byte ptr es:[edi], fs:[esi]
        pop     edi
        pop     esi
        
        ; Load the copy into ES:[EBX] 
        mov     ax,ss
        shl     eax,10h
        lea     ax,sValModeCopy
        mov     lpValMode,eax
        sub     ebx,ebx
        les     bx,lpValMode
        
        ; Check if this mode is overridden by the OEM.
        push    esi
        push    lpValMode
        call    GN_CheckForOEMOverrides
        or      ax,ax
        jne     FailMode

        ; Check if this is a bogus multiple of 8 mode passed in because 
        ; Win98 in multimon mode wants resolutions to be a multiple of 8
        ; so 1600x900 gets changed to 1600x896 and 1400x1050 gets changed
        ; to 1400x1048. GN_CheckForMultipleOf8Mode will adjust to a mode
        ; we can handle
        mov     ax,ss
        shl     eax,10h
        lea     ax,es:[ebx].DISPVALMODE.dvmXRes
        push    eax
        lea     ax,es:[ebx].DISPVALMODE.dvmYRes
        push    eax
        call    GN_CheckForMultipleOf8Mode

        ; Check for bogus mode 604x402 that we sometimes get
        cmp     es:[ebx].DISPVALMODE.dvmXRes,604
        je      FailMode
        cmp     es:[ebx].DISPVALMODE.dvmYRes,402
        je      FailMode

        ; Get the memory needed for the mode
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        movzx   eax,es:[ebx].DISPVALMODE.dvmXRes
        push    eax
        movzx   eax,es:[ebx].DISPVALMODE.dvmYRes
        push    eax
        movzx   eax,es:[ebx].DISPVALMODE.dvmBpp
        push    eax
        call    HW_GetModeDimensions
        mov     dwMemoryNeededForMode,ebx

        ; Get the largest memory size that we could put this mode in.
        ; We suppose we can purge DX heap if we'll need an extra free space.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     eax,ds:[ebx].GNBOARDCX.dwMemorySizeForValidateMode

        ; Is it enough?
        cmp     eax,dwMemoryNeededForMode
        jb      FailMode

        ; For right now, if this is 480x360 or 960x720, and the device
        ; is a DFP, then disallow the mode. PLEASE NOTE: There is a
        ; corresponding check for these modes in the Modeset DLL.
        sub     ebx,ebx
        les     bx,lpValMode
        movzx   eax,es:[ebx].DISPVALMODE.dvmXRes
        movzx   edx,es:[ebx].DISPVALMODE.dvmYRes
        
Check480x360:
        cmp     eax,1E0H
        jne     Check960x720
        cmp     edx,168H
        jne     Check960x720
        jmp     CheckDisplayType
        
Check960x720:
        cmp     eax,3C0H
        jne     NotBadMode
        cmp     edx,2D0H
        jne     NotBadMode
        
CheckDisplayType:
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     ebx,[ecx].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[ebx].MODEINFO.MIDevData
        cmp     al,DEVTYPE_DFP
        je      FailMode

NotBadMode:
        ; Check for the registry key that says we allow all the modes.
        ; If set, then any desktop is OK and everyone will just pan and
        ; scan as needed.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetAllowMightyModesFlag,es,ax
        or      ax,ax
        jne     Success

MustValPhys:
        ; Always validate on the primary CRTC. We always allow the
        ; secondary to pan.

        ; Get everything into a MODEINFO structure
        sub     ebx,ebx
        les     bx,lpValMode
        movzx   eax,es:[ebx].DISPVALMODE.dvmXRes
        mov     sModeInfo.dwXRes,eax
        movzx   eax,es:[ebx].DISPVALMODE.dvmYRes
        mov     sModeInfo.dwYRes,eax
        movzx   eax,es:[ebx].DISPVALMODE.dvmBpp
        mov     sModeInfo.dwBpp,eax

        ; If this mode validation is occuring after a SetDisplayConfig
        ; escape, then the display device information in the GNCRTCCX
        ; may be stale. Check the dwDisplayDeviceChange flag, and if it
        ; is equal to 1, then just validate the mode. After all display
        ; devices are mapped during Enable, we will clear the flag and
        ; revalidate to make sure the mode can actually be set.
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        cmp     ds:[ecx].GNCRTCCX.dwDisplayDeviceChange,1
        je      Success

        ; We don't know what refresh rate they really want. Just
        ; choose a large value so that if we support any refresh
        ; rate at the resolution and pixel depth desired, then
        ; we will validate the mode.
        mov     sModeInfo.dwRefreshRate,1000
        lea     ebx,[ecx].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[ebx].MODEINFO.MIDevData
        mov     sModeInfo.MIDevData,eax

        ; Maybe we should re read the EDID
        push    ecx
        call    GN_CheckAndMaybeReadEdid

        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ecx
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        sub     ebx,ebx
        lfs     bx,ds:[ecx].GNCRTCCX.lpNvmodeDispData
        push    fs:[ebx].DISPDATA.dwMonitorInfType
        push    ds:[ecx].GNCRTCCX.lpEdidBuffer
        push    ds:[ecx].GNCRTCCX.dwEdidSize
        call    GN_ValidatePhysicalMode

        test    eax,MODE_RESOLUTION_ADJUSTED
        jnz     FailMode

Success:
        mov     eax,VALMODE_YES
        jmp     DoneIt

FailMode:
        mov     eax,VALMODE_NO_UNKNOWN

DoneIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ValidatePhysicalMode
;
; Purpose:      This function is a more specific version of the
;               Validate Mode call. The mode passed in is validated
;               and adjusted downwards to something that can be set
;               if it is out of range.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX
;               lpModeInfoToValidate    MODEINFO ptr
;               dwTypeMonitorInf
;               lpEdidBuffer            Edid Buffer ptr
;               dwEdidBufferSize        size of lpEdidBuffer. This
;                                       will be 0 if there is no EDID
;
; Returns:      eax                     A combination of the bitflags
;                                       defined in modeext.h like
;                                       MODE_RESOLUTION_ADJUSTED
;                                       MODE_REFRESHRATE_ADJUSTED
;                                       etc..
;
;               Note that if eax indicates that the lpModeInfoToValidate
;               could not be supported, then lpModeInfoToValidate will
;               have been modified on return to reflect the
;               next largest possible mode that can.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ValidatePhysicalMode, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpModeInfoToValidate
PARMD   dwTypeMonitorInf
PARMD   lpLocalEdidBuffer
PARMD   dwEdidBufferSize
LOCALD  dwSaveMonitorInfType
LOCALV  sDispData,DISPDATA
LOCALV  sMO,MODEOUT
LOCALV  sSettableCrtcModeInfo,MODEINFO
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; Build a correct DISPDATA structure
        sub     ebx,ebx
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        mov     eax,fs:[ebx].DISPDATA.dwMonitorInfType
        mov     dwSaveMonitorInfType,eax
        mov     eax,dwTypeMonitorInf
        mov     fs:[ebx].DISPDATA.dwMonitorInfType,eax

        ; Modify refresh rate list in registry.
        mov     ebx,lpModeInfoToValidate
        lea     bx,[bx].MODEINFO.MIDevData
        push    ebx
        push    ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    lpLocalEdidBuffer
        push    dwEdidBufferSize
        call    FFP16 PTR lpfnResetRefreshRates

        ; Now we are ready to validate!
        push    ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    lpLocalEdidBuffer
        push    dwEdidBufferSize
        push    lpModeInfoToValidate
        mov     ax,ss
        shl     eax,10H
        lea     ax,sMO
        push    eax
        lea     ax,sSettableCrtcModeInfo
        push    eax
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Return the mode that can be set
        sub     edi,edi
        les     di,lpModeInfoToValidate
        push    ss
        pop     fs
        lea     esi,sSettableCrtcModeInfo
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]
        mov     esi,pGNCrtcCXOffset

        ; Restore the NvmodeDispData
        sub     ebx,ebx
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        mov     ecx,dwSaveMonitorInfType
        mov     fs:[ebx].DISPDATA.dwMonitorInfType,ecx

        POPR    esi,edi
CLOSEPROC

;==============================================================================
;
; Function:     GN_GetFreeBlockSizeForMode
;
; Purpose:      This function is called to get the FreeBlock size
;               that we can fit a mode into. This whole determination
;               is actually very complicated because it depends upon
;               many things. Like whether we are running just one
;               logical device on this board, or more. And, if more
;               than one, whether DirectX is running on the device
;               that we are NOT trying to get a FreeBlock size for.
;               Argh! Just follow the comments along in the code.
;               That is where the details are.
;
; Arguments:
;               ds                      COntext Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      eax     The largest free block that would be available
;                       to set the mode in.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetFreeBlockSizeForMode, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALD  dwMyOffscreenFlags
LOCALD  dwJustOnePrimaryCanMove
LOCALD  pOtherGNLogdevCXOffset
LOCALD  dwValidationFreeBlockSize
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX

        ; Assume only one primary surface can move and that there is
        ; not even a second primary surface sharing video memory.
        mov     dwJustOnePrimaryCanMove,1
        mov     pOtherGNLogdevCXOffset,0

        ; Is there another primary surface on this board?
        cmp     ds:[edi].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard,1
        je      GotPrimaryData

        ; OK, there is more than 1 primary (that means 2 right now),
        ; but can the other primary be moved? If DX is running on it,
        ; then it cannot. Otherwise it can.
        mov     eax,ds:[edi].GNBOARDCX.pGNLogdevCX[0]
        cmp     eax,esi
        jne     @F
        mov     eax,ds:[edi].GNBOARDCX.pGNLogdevCX[4]
@@:     mov     pOtherGNLogdevCXOffset,eax

        cmp     ds:[eax].GNLOGDEVCX.bDirectXActive,0
        jne     GotPrimaryData
        mov     dwJustOnePrimaryCanMove,0

GotPrimaryData:
        ; NOTE: The general fitting algorithm is too difficult to do in
        ; its full generality. Hence, we just implement the two specific
        ; cases that we know are the only ones that can happen right now.
        cmp     dwJustOnePrimaryCanMove,0
        jne     JustOnePrimaryCanMove


BothPrimariesCanMove:
        ; This scenario allows us the ultimate in flexibility. We
        ; will be able to host the highest resolutions on both
        ; logical devices in small video memory configurations if
        ; we have the flexibility to move both primaries.

        ; First, we need to remember if we currently have an offscreen
        ; heap for bitmaps
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     eax,ds:[edi].GNBOARDCX.dwOffscreenFlags
        mov     dwMyOffscreenFlags,eax

        ; Now purge this heap if it exists sweeping all bitmaps to
        ; system memory.
        push    edi
        pushd   0
        call    GN_ControlOffscreenAllocations

        ; Until we get the RM call we want, cross your fingers and
        ; use the largest two things of: 1) The size for one of the
        ; primaries. 2) The size for the other primary. 3) The size
        ; of the largest other free block.

        ; This is not quite accurate since the block may be large
        ; emough if it starts at its base address, but starting at
        ; its aligned address, it may not be large enough. For now
        ; that is all we have.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        mov     eax,-1
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        mov     eax,ds:[esi].DIBENGINEHDR.deBitsOffset
@@:
        push    eax

        mov     ebx,pOtherGNLogdevCXOffset
        mov     eax,-1
        cmp     ds:[ebx].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        mov     eax,ds:[ebx].DIBENGINEHDR.deBitsOffset
@@:
        push    eax
        call    HW_GetLargestOffscreenSpace
        or      ebx,ebx
        je      OneBlock

TwoBlocks:
        ; See if the other guy will fit in one of the two blocks
        mov     ecx,pOtherGNLogdevCXOffset
        mov     ecx,ds:[ecx].GNLOGDEVCX.dwMemorySizeForPrimarySurface

        ; Will other guy fit in the smaller block?
        sub     ebx,ecx
        jge     GotIt
        add     ebx,ecx

OneBlock:
        ; Other guy has to go in larger block (Note, this must work
        ; since the "free" blocks were gotten assuming that both
        ; primaries were freed.)
        mov     ecx,pOtherGNLogdevCXOffset
        mov     ecx,ds:[ecx].GNLOGDEVCX.dwMemorySizeForPrimarySurface
        sub     eax,ecx

GotIt:
        ; The larger of the two free pieces, after the other logical
        ; device's primary surface has been placed in one of them,
        ; should go into dwValidationFreeBlockSize
        cmp     eax,ebx
        jae     @F
        mov     eax,ebx
@@:
        mov     dwValidationFreeBlockSize,eax

        ; If we had an offscreen heap, then let us turn it back on.
        test    dwMyOffscreenFlags,OFFS_HEAP_UNAVAILABLE
        jnz     @F
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        pushd   1
        call    GN_ControlOffscreenAllocations
@@:
        jmp     GotValidationSize


JustOnePrimaryCanMove:
        ; There is only one primary onthis board OR the other primary
        ; cannot be moved for some reason (DX is running on it maybe.)

        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        mov     eax,-1
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        mov     eax,ds:[esi].DIBENGINEHDR.deBitsOffset
@@:
        push    eax
        mov     eax,ds:[ebx].GNBOARDCX.dwOffscreenBaseAddress
        cmp     ds:[ebx].GNBOARDCX.dwOffscreenLength,0
        jne     @F
        mov     eax,-1
@@:
        push    eax
        call    HW_GetLargestOffscreenSpace
        mov     dwValidationFreeBlockSize,eax

GotValidationSize:
        mov     eax,dwValidationFreeBlockSize
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CheckForMultipleOf8Mode
;
; Purpose:      The following is a HACK! Win98 in multimon mode wants
;               resolutions to be a multiple of 8 so 1600x900 gets
;               changed to 1600x896. When a mode is being changed on
;               the secondary, Windows revalidates the mode on the
;               primary. Since 1600x896 is not in our master mode list,
;               we reject it, and Windows aborts the mode set. Since
;               1600x900 is what was really set, all we need to do is
;               validate 1600x896 and Windows is happy. Please also note
;               that Microsoft has fixed this problem under WinME.
;               NOTE: This is also a problem with 1400x1050 which Windows
;               asks to validate as 1400x1048. Chnage these modes to the
;               real mode so we can validate.
;
; Arguments:
;               lpXRes                  X Res ptr
;               lpYRes                  Y Res ptr
;
; Returns:      Corrected Y Res for modes in question
;
; Preserve:     ds,es,fs,esi,edi
;==============================================================================
DECPROC GN_CheckForMultipleOf8Mode, PASCAL, FRAME, FAR16
PARMD   lpXRes
PARMD   lpYRes
OPENPROC
        PUSHR   es,fs,esi,edi

        sub     esi,esi
        les     si,lpXRes
        sub     edi,edi
        lfs     di,lpYRes
        cmp     word ptr es:[esi],1600
        jne     Check_1400x1048
        cmp     word ptr fs:[edi],896
        jne     Check_1400x1048
        mov     word ptr fs:[edi],900
        jmp     DoneIt

Check_1400x1048:
        cmp     word ptr es:[esi],1400
        jne     DoneIt
        cmp     word ptr fs:[edi],1048
        jne     DoneIt
        mov     word ptr fs:[edi],1050
        
DoneIt:

        POPR    es,fs,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CheckForOEMOverrides
;
; Purpose:      This routine checks for certain modes that certain OEMs
;               want blocked.
;
; Arguments:
;               ds                      COntext Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               lpModeInfoToValidate    MODEINFO ptr
;
; Returns:      AX = 0                  Continue Mode Validation
;               AX = 1                  Invalidate Mode
;
; Preserve:     ds,es,fs,ebx,esi,edi
;==============================================================================
DECPROC GN_CheckForOEMOverrides, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   lpLocalValMode
LOCALS  sEdidBuf,MAX_EDID_BUFFER_SIZE
LOCALD  dwEdidSze;
LOCALV  sModeInfo,MODEINFO
LOCALD  dwCheckedBothDisplayFlag
OPENPROC
        PUSHR   es,fs,ebx,esi,edi

        mov     esi,pGNLogdevCXOffset

        ; This routine returns TRUE if we want to exclude 1400x1050 when
        ; a 1400x1050 panel is not installed.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGet1400x1050Override,es,ax
        cmp     ax,1
        jne     CheckRestrictLargestMode
        
        ; Only allow 1400x1050 if we are on the SXGA+ panel (1400x1050 panel).
        ; First, check to see if the mode in question is 1400x1050. Then, read
        ; the EDID for DFP0 and check to see if the largest mode is 1400x1050.
        ; If so, PassMode, otherwise FailMode. 
        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,1400
        jne     CheckRestrictLargestMode
        cmp     es:[ebx].DISPVALMODE.dvmYRes,1050
        jne     CheckRestrictLargestMode

        ; Now read the EDID.
        mov     dwEdidSze,MAX_EDID_BUFFER_SIZE
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        mov     eax,10000h              ; Device mask for DFP0
        push    eax
        mov     ax,ss
        shl     eax,10h
        lea     ax,sEdidBuf
        push    eax
        lea     ax,dwEdidSze
        push    eax
        call    HW_ReadEdidForDevice

        ; Make sure we read the EDID. If EAX != 0, then FailMode.
        or      eax,eax
        je      FailMode
        
        cmp     dwEdidSze,0
        je      FailMode

        ; Now get the largest mode for this EDID which will be the native
        ; mode of DFP0.
        mov     ax,ss
        shl     eax,10h
        lea     ax,sEdidBuf
        push    eax
        push    dwEdidSze
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnGetLargestEdidMode,es,ax

        sub     ebx,ebx
        lea     bx,sModeInfo
        cmp     ss:[ebx].MODEINFO.dwXRes,1400
        jne     FailMode
        cmp     ss:[ebx].MODEINFO.dwYRes,1050
        jne     FailMode
        
CheckRestrictLargestMode:
        ; This OEM Override is for OEMs who, in clone mode, only want
        ; modes up to the greatest EDID mode for the lesser display device.
        ; This is due to the fact tha they do not want panning.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetRestrictLargestMode,es,ax
        cmp     ax,1
        jne     CheckMaxDfpMode

        ; If not in clone mode, pass mode to standard validation.
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      CheckMaxDfpMode
        
        ; We are on restricted desktop system and in clone mode. Get the
        ; largest EDID modes for each display device and make sure the
        ; requested mode is less than or equal to the smaller of the two.
        mov     dwCheckedBothDisplayFlag,0
        
        ; Make sure there is an EDID for the primary. If not, check display
        ; type and use safe mode.
        mov     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        cmp     ds:[edi].GNCRTCCX.dwEdidSize,0
        je      NoEdidForDevice
        
        ; Get the largest EDID mode of the primary display.
        push    edi
        mov     ax,ss
        shl     eax,10h
        lea     ax,sModeInfo
        push    eax
        call    GN_GetLargestEdidMode

        ; Now, compare to see if the requested mode is greater than the
        ; maximum mode this EDID can handle.
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     eax,ss:[ebx].MODEINFO.dwXRes

        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,ax
        jg      FailMode

CheckSecondary:
        mov     dwCheckedBothDisplayFlag,1
        
        ; Make sure there is an EDID for the secondary. If not, check display
        ; type and use safe mode.
        mov     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        cmp     ds:[edi].GNCRTCCX.dwEdidSize,0
        je      NoEdidForDevice

        ; Now get the largest EDID mode for the clone device.
        push    edi
        mov     ax,ss
        shl     eax,10h
        lea     ax,sModeInfo
        push    eax
        call    GN_GetLargestEdidMode

        ; Now, compare to see if the requested mode is greater than the
        ; maximum mode this EDID can handle.
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     eax,ss:[ebx].MODEINFO.dwXRes

        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,ax
        jg      FailMode
        
        jmp     CheckMaxDfpMode

NoEdidForDevice:
        lea     ebx,[edi].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[ebx].MODEINFO.MIDevData
        cmp     al,DEVTYPE_TV
        je      UseMaxTvResolution

        ; There is no EDID and the display device is not a TV. Use 800x600
        ; as a safe mode.
        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,800
        jg      FailMode
        
        cmp     dwCheckedBothDisplayFlag,1
        jne     CheckSecondary
        
        jmp     CheckMaxDfpMode

UseMaxTvResolution:
        sub     ebx,ebx
        lfs     bx,ds:[edi].GNCRTCCX.lpNvmodeDispData
        mov     eax,fs:[ebx].DISPDATA.dwMaxTVOutResolution
        shr     eax,10h
        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,ax
        jg      FailMode

        cmp     dwCheckedBothDisplayFlag,1
        jne     CheckSecondary
        
CheckMaxDfpMode:
        ; This OEM override is for OEMs who want to limit the maximum
        ; allowable mode for the DFP. First, check the display type to
        ; make sure this is a DFP.
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     ebx,[ecx].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[ebx].MODEINFO.MIDevData
        cmp     al,DEVTYPE_DFP
        jne     PassMode
        
        ; Now check if the registry key MaxDfpMode exists. If so, the max
        ; DFP mode will be returned in sModeInfo.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10h
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnGetMaxDfpMode,es,ax
        
        cmp     ax,1
        jne     PassMode

        ; Now, compare to see if the requested mode is greater than the
        ; maximum mode for the DFP in the registry. Please note that Windows
        ; considers a mode greater if the XRes is greater.
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     eax,ss:[ebx].MODEINFO.dwXRes

        sub     ebx,ebx
        les     bx,lpLocalValMode
        cmp     es:[ebx].DISPVALMODE.dvmXRes,ax
        jg      FailMode

PassMode:
        mov     eax,0
        jmp     DoneIt

FailMode:
        mov     eax,1

DoneIt:

        POPR    es,fs,ebx,esi,edi
CLOSEPROC


CLOSESEG _NVTEXT16

END

