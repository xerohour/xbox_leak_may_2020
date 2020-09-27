;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncrtc.asm
;
; Purpose:      This file holds init routines for GNCRTCCX.
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
include minivdd.inc
.listall

IF      (MAX_MODEOUT_SIZE LT (SIZE MODEOUT))
echo MAX_MODEOUT_SIZE less than size of MODEOUT structure
.err
ENDIF
IF      (MAX_DISPDATA_SIZE LT (SIZE DISPDATA))
echo MAX_DISPDATA_SIZE less than size of DISPDATA structure
.err
ENDIF
IF      (MAX_MODEINFO_SIZE LT SIZE MODEINFO)
echo MAX_MODEINFO_SIZE less than size of MODEINFO structure
.err
ENDIF

OPENSEG  _DATA, USE16, DATA
PUBLIC  szPowerUpSubkey
szPowerUpSubkey         DB      "PowerUpFlags",0
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_CreateCrtcContext
;
; Purpose:      This routine allocates memory for and initializes
;               an array of GNCRTCCX structure.
;
; Arguments:
;               ds                      Selector for the Context data
;               pGNBoardCXOffset        GNBOARDCX ptr
;               dwIsBootCrtc            TRUE if this is the CRTC we
;                                       are booting windows on. In a
;                                       Clone situation, this is only
;                                       the first of the clone monitors
;                                       that are the boot set.
;
; Returns:      eax     0       failure
;                       else    Success and eax is a GNCRTCCX ptr
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CreateCrtcContext, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   dwLogDeviceInstance
PARMD   dwPhysDeviceInstance
PARMD   dwIsBootCrtc
LOCALV  sDispInfo,DISPLAYINFO
OPENPROC
        PUSHR   esi,edi
        sub     esi,esi

        ; Allocate memory for the GNCRTCCX
        pushd   SIZE GNCRTCCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax

        ; We are initting.
        mov     ds:[esi].GNCRTCCX.dwInitting,1
        mov     eax,dwLogDeviceInstance
        mov     ds:[esi].GNCRTCCX.dwLogicalDeviceInstance,eax
        mov     eax,dwPhysDeviceInstance
        mov     ds:[esi].GNCRTCCX.dwPhysicalDeviceInstance,eax
        mov     eax,dwIsBootCrtc
        mov     ds:[esi].GNCRTCCX.dwBootCrtc,eax

        ; Save this
        mov     eax,pGNBoardCXOffset
        mov     ds:[esi].GNCRTCCX.pGNBoardCX,eax

        ; Crate the HW logical device next
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        call    HW_CreateCrtcContext
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].GNCRTCCX.pHWCrtcCX,eax

        ; Build a 16:16 ptr to the monitor path. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.szLocalMonitorBasePath
        add     ebx,eax
        mov     eax,1
        mov     ecx,100H
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNCRTCCX.lpLocalMonitorBasePath,eax

        ; Build a 16:16 ptr to the DISPDATA. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.sCrtcModeOut
        add     ebx,eax
        mov     eax,1
        mov     ecx,100H
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNCRTCCX.lpCrtcModeOut,eax

        ; Build a 16:16 ptr to the DISPDATA. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.sNvmodeDispData
        add     ebx,eax
        mov     eax,1
        mov     ecx,100H
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNCRTCCX.lpNvmodeDispData,eax
        lea     edi,[esi].GNCRTCCX.sNvmodeDispData

        push    ds:[esi].GNCRTCCX.pGNBoardCX
        call    GN_GetBoardRegistryPath
        mov     ds:[edi].DISPDATA.lpBoardRegistryData,eax
        mov     ds:[esi].GNCRTCCX.lpBoardRegistryData,eax

        ; The base monitor registry path
        mov     eax,ds:[esi].GNCRTCCX.lpLocalMonitorBasePath
        mov     ds:[edi].DISPDATA.lpMonitorRegistryData,eax

        ; Get the PCI device ID
        push    ds:[esi].GNCRTCCX.pGNBoardCX
        call    GN_GetPciDeviceID
        mov     ds:[edi].DISPDATA.dwDeviceID,eax

        ; We don't use this anymore since we validate the physical
        ; and virtual modes independently. I'd like to get rid of it,
        ; but for now we just set the amount of memory to a really
        ; big amount so that this will not be a limitation on the mode.
        mov     eax,10000000H
        mov     ds:[edi].DISPDATA.dwVideoMemoryInBytes,eax

        ; Get the maximum DFP scaling
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        call    HW_GetMaxDfpScaledXRes
        mov     ds:[edi].DISPDATA.dwMaxDfpScaledXResInPixels,eax

        ; Fill out address of GN_GetDisplayPitch so MODESET DLL can
        ; call back to display driver to get the pitch.
        mov     ax,SEG GN_Far16GetModeSize
        shl     eax,10h
        mov     ecx,OFFSET GN_Far16GetModeSize
        mov     ax,cx
        mov     ds:[edi].DISPDATA.lpfnGetModeSize,eax
        mov     ecx,OFFSET GN_Far16IsModeAcceptedByDriver
        mov     ax,cx
        mov     ds:[edi].DISPDATA.lpfnIsModeAcceptedByDriver,eax
        mov     ds:[edi].DISPDATA.dwContext1,ds
        mov     ds:[edi].DISPDATA.dwContext2,esi

        ; Fill the maximum resolution for TV
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        call    HW_GetMaxTVOutResolution
        mov     ds:[edi].DISPDATA.dwMaxTVOutResolution,eax
        mov     ds:[edi].DISPDATA.dwTVEncoderType,ebx

        ; Build a 16:16 ptr to the Edid Buffer. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.sEdidBuffer
        add     ebx,eax
        mov     eax,1
        mov     ecx,MAX_EDID_BUFFER_SIZE
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNCRTCCX.lpEdidBuffer,eax
        mov     ds:[esi].GNCRTCCX.dwEdidSize,0

        ; Build a 16:16 ptr to the sCrtcModeInfo.MIDevData
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        lea     ebx,[ebx].MODEINFO.MIDevData
        add     ebx,eax
        mov     eax,1
        mov     ecx,MAX_EDID_BUFFER_SIZE
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[edi].DISPDATA.lpDevData,eax

        ; Get the desired display device
        push    esi
        call    GN_GetDisplayDevice

        ; This forces an EDID read also for cases where we read more
        ; frequently than just at boot time.
        mov     ds:[esi].GNCRTCCX.dwTicksSinceLastEdidRead,0

        ; Ask the hardware how many bits per color gun
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        call    HW_BitsPerColorGun
        mov     ds:[esi].GNCRTCCX.dwBitsPerColorChannel,eax

        ; Ask the Hardware how many cursors it can cache
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        pushd   MAX_CACHEABLE_CURSORS
        call    HW_SizeCursorCache
        mov     ds:[esi].GNCRTCCX.dwMaxCacheableCursors,eax

        ; We force the first modeset after a GNCRTCCX creation
        ; to be a "display device change" modeset which means
        ; that the .inf status will be ignored -- the initial
        ; inf status that Windows gives us is unreliable.
        mov     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,1

        ; Now we can create it
        push    esi
        call    GN_CreateCursorCache
        or      eax,eax
        je      DoneIt

        ; Clear out the cursor cache.
        push    esi
        call    GN_ClearCursorCache

        ; Build a 16:16 ptr to the sCrtcModeInfo. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        add     ebx,eax
        mov     eax,1
        mov     ecx,SIZE MODEINFO
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNCRTCCX.lpCrtcModeInfo,eax

        ; Initialize the palette table
        ; The first ten entries are system colors
        mov     edi,OFFSET SystemColorTableFirst10
        sub     ebx,ebx
@@:     mov     eax,cs:[edi]
        add     edi,4
        mov     ds:[esi][ebx * 4].GNCRTCCX.dwPaletteTable,eax
        inc     ebx
        cmp     ebx,10
        jc      @B

        ; The middle 236 are just set to a default value
        mov     edi,OFFSET SystemColorTableDefault
        mov     eax,cs:[edi]
@@:     mov     ds:[esi][ebx * 4].GNCRTCCX.dwPaletteTable,eax
        inc     ebx
        cmp     ebx,246
        jc      @B

        ; The last ten entries are system colors also
        mov     edi,OFFSET SystemColorTableLast10
@@:     mov     eax,cs:[edi]
        add     edi,4
        mov     ds:[esi][ebx * 4].GNCRTCCX.dwPaletteTable,eax
        inc     ebx
        cmp     ebx,256
        jc      @B

        ; Create an identity gamma table.
        mov     ebx,0FFH
        mov     eax,0FFFFFFH
@@:     mov     ds:[esi][ebx * 4].GNCRTCCX.dwGammaTable,eax
        sub     eax,10101H
        dec     ebx
        jge     @B

        ; Return Success
        mov     eax,esi

DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the GNBOARDCX was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; GN_DestroyBoardContext can clean up partial inits
        push    esi
        call    GN_DestroyCrtcContext
        sub     eax,eax

ExitIt:

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_DestroyCrtcContext
;
; Purpose:      This routine destroys a GNCRTCCX.
;               You do not need to free its memory. Just free any
;               resources inside the structure.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset Offset to the crtc context data to destroy
;
; Returns:      eax     0       failure
;                       else    Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DestroyCrtcContext, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNCrtcCXOffset

        ; Turn off the cursor.
        push    pGNCrtcCXOffset
        pushd   0
        pushd   0
        call    GN_TurnCursorOffOrOn

        cmp     ds:[esi].GNCRTCCX.pCursorCache,0
        je      @F

        ; Now we can destroy the cursor cache.
        push    pGNCrtcCXOffset
        call    GN_DestroyCursorCache
@@:
        ; Free the 16:16 ptr to the lpCrtcModeInfo buffer
        mov     eax,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        or      eax,eax
        je      @F
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNCRTCCX.lpCrtcModeInfo,0
@@:


        ; Free the 16:16 ptr to the edid buffer
        mov     eax,ds:[esi].GNCRTCCX.lpEdidBuffer
        or      eax,eax
        je      @F
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNCRTCCX.lpEdidBuffer,0
@@:

        ; Free the 16:16 ptr to the lpNvmodeDispData buffer
        mov     eax,ds:[esi].GNCRTCCX.lpNvmodeDispData
        or      eax,eax
        je      DoneDispData

        push    eax
        CALLFAR16IND lpfnFreeMasterLists,es,bx

        ; Free the 16:16 ptr lpDevData in the NvModeDispData struct
        lea     ebx,[esi].GNCRTCCX.sNvmodeDispData
        mov     eax,ds:[ebx].DISPDATA.lpDevData
        or      eax,eax
        je      @F
        shr     eax,10H
        mov     ds:[ebx].DISPDATA.lpDevData,0
        call    GN_UnmapAndFreeSelector
@@:
        mov     eax,ds:[esi].GNCRTCCX.lpNvmodeDispData
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNCRTCCX.lpNvmodeDispData,0
DoneDispData:

        ; Free the 16:16 ptr to the lpNvmodeDispData buffer
        mov     eax,ds:[esi].GNCRTCCX.lpCrtcModeOut
        or      eax,eax
        je      DoneModeOut
        mov     eax,ds:[esi].GNCRTCCX.lpCrtcModeOut
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNCRTCCX.lpCrtcModeOut,0
DoneModeOut:

        ; Free the 16:16 ptr to the lpLocalMonitorBasePath buffer
        mov     eax,ds:[esi].GNCRTCCX.lpLocalMonitorBasePath
        or      eax,eax
        je      @F
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNCRTCCX.lpLocalMonitorBasePath,0
@@:
        cmp     ds:[esi].GNCRTCCX.pHWCrtcCX,0
        je      @F
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        call    HW_DestroyCrtcContext
        mov     ds:[esi].GNCRTCCX.pHWCrtcCX,0

@@:
        ; Free the memory for the object.
        push    esi
        call    HW_FreeFlatMemory

        ; Indicate that we destroyed something.
        mov     eax,1
DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetLargestEdidMode
;
; Purpose:      This function gets the largest EDI mode for the
;               current device type and tv format if they have
;               an EDID. If not, the mode passed in is unchanged.
;
; Arguments:
;               ds              CX Sel
;               pGNCrtcCXOffset GNCRTCCX ptr
;               lpModeInfo      MODEINFO ptr
;
; Returns:      eax     FALSE then there was no EDID mode
;                       TRUE lpModeInfo was overridden with the largest
;                               EDID mode.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetLargestEdidMode, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpModeInfo
OPENPROC
        mov     ecx,pGNCrtcCXOffset
        push    ds:[ecx].GNCRTCCX.lpEdidBuffer
        push    ds:[ecx].GNCRTCCX.dwEdidSize
        push    lpModeInfo
        CALLFAR16IND lpfnGetLargestEdidMode,es,ax
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetRefreshRateForDesktop
;
; Purpose:      This function gets the refresh rate for the desktop.
;               A refresh rate is really a CRTC thing, not a desktop
;               thing, but this is a Windows messiness. They have
;               a refresh rate affiliated with the desktop because
;               they don't understand about virtual desktops.
;
; Arguments:
;               ds              CX Sel
;               pGNCrtcCXOffset GNCRTCCX ptr
;               lpModeInfo      MODEINFO ptr
;
; Returns:      lpModeInfo->dwRefreshRate was filled in with some
;               refresh rate.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetRefreshRateForDesktop, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpModeInfo
OPENPROC
        mov     ebx,pGNCrtcCXOffset
        mov     gs,word ptr lpModeInfo+2
        movzx   eax,word ptr lpModeInfo
        push    ds:[ebx].GNCRTCCX.lpNvmodeDispData
        lea     ecx,[ebx].GNCRTCCX.sCrtcModeInfo
        mov     ecx,ds:[ecx].MODEINFO.MIDevData
        mov     gs:[eax].MODEINFO.MIDevData,ecx
        push    lpModeInfo
        push    ds:[ebx].GNCRTCCX.lpEdidBuffer
        push    ds:[ebx].GNCRTCCX.dwEdidSize
        CALLFAR16IND lpfnGetRefreshRateForDesktop,es,ax
CLOSEPROC


;==============================================================================
;
; Function:     GN_DoOddballStuffAtBoot
;
; Purpose:      This function does some things that need to be done
;               at boot that are pretty random. Here is the list.
;               1) Clear PowerUp flags in the registry so that
;                       the control panel will know if the system
;                       has been rebooted since the the last time
;                       the control panel was invoked.
;
; Arguments:
;               ds      CX Sel
;               esi     GNCRTCCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DoOddballStuffAtBoot, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
OPENPROC
        mov     ebx,pGNCrtcCXOffset
        lfs     bx,ds:[ebx].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.lpBoardRegistryData
        mov     ax,_DATA
        push    ax
        mov     ax,OFFSET szPowerUpSubkey
        push    ax
        CALLFAR16IND lpfnDeleteLocalRegEntry,es,ax
CLOSEPROC


;==============================================================================
;
; Function:     GN_CheckAndMaybeReadEdid
;
; Purpose:      This function check to see if the EDID needs to be reread
;               and if so, then does the EDID read.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset GNCRTCCX ptr
;
; Returns:      eax     0       then there was a problem reading the EDID.
;                       non-0   then there was no problem
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CheckAndMaybeReadEdid, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
LOCALD  dwCurrentEdidSize
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; Read the registry to determine if the EDID should be reread.
        ; If AX is returned as 1, then the registry key ForceEdidRead
        ; exists and is enabled.
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetEdidRegistryKey,es,ax
        movzx   eax,ax
        or      eax,eax
        je      DoneRead

OKToReadEdid:
        ; Even Now we still won't read the EDID if we have read it
        ; very recently. This prevents long delays on monitors with
        ; no EDID which take a while to timeout.
        cmp     ds:[esi].GNCRTCCX.dwTicksSinceLastEdidRead,MIN_TICKS_BEFORE_REREADING_EDID
        jc      DoneRead

MustReRead:
        mov     ds:[esi].GNCRTCCX.dwEdidSize,0
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        cmp     ds:[ebx].MODEINFO.MIDevData.cType,DEVTYPE_TV
        je      DoneRead
        
        ; OK, NOW we can read the EDID
        mov     ds:[esi].GNCRTCCX.dwEdidSize,MAX_EDID_BUFFER_SIZE
        push    ds:[esi].GNCRTCCX.pHWCrtcCX

        ; JKTODO -- Need to be able to specify Device And Number
        ; For now, just device
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        push    ds:[ebx].MODEINFO.MIDevData
        lea     eax,[esi].GNCRTCCX.sEdidBuffer
        push    eax
        lea     eax,[esi].GNCRTCCX.dwEdidSize
        push    eax
        call    HW_ReadEdid

DoneRead:
        mov     eax,ds:[esi].GNCRTCCX.dwEdidSize
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_Far16GetModeSize
;
; Purpose:      This function returns the amount of memory taken up
;               by a given mode and the pitch for a gievn mode.
;               This function is called from 16bit C code. It must
;               lie within 64K of the start of a segment. We put it
;               in a 16Bit segment just to make things easier.
;
; Arguments:
;               dwContextData1  This context data was supplied by the
;                               display driver to the modeset dll.
;                               It ought to be the Context Selector.
;
;               dwContextData2  This context data was supplied by the
;                               display driver to the modeset dll.
;                               It ought to be the GNCRTCCX ptr.
;
;               dwXRes          X Resolution of mode to get size for
;               dwYRes          Y Resolution of mode to get size for
;               dwBpp           Color depth of mode to get size for
;               lpPitch         place to return pitch
;               lpSize          place to return total bytes needed for mode
;
; Returns:      TRUE
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_Far16GetModeSize, PASCAL, FRAME, FAR16
PARMD   dwContextData1
PARMD   dwContextData2
PARMD   dwXRes
PARMD   dwYRes
PARMD   dwBpp
PARMD   lpPitch
PARMD   lpSize
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,dwContextData1
        mov     ebx,dwContextData2
        mov     ebx,ds:[ebx].GNCRTCCX.pGNBoardCX

        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwXRes
        push    dwYRes
        push    dwBpp
        call    HW_GetModeDimensions

        lfs     si,lpPitch
        mov     fs:[si],eax
        lfs     si,lpSize
        mov     fs:[si],ebx

        mov     ax,1
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_Far16IsModeAcceptedByDriver
;
; Purpose:      This function gets called from the modeset dll
;               to allow the driver to veto or accept a mode.
;               Technically, you may also modify the output
;               timings, but this is discouraged and should
;               only be done in extreme cases.
;
; Arguments:
;               dwContextData1  This context data was supplied by the
;                               display driver to the modeset dll.
;                               It ought to be the Context Selector.
;
;               dwContextData2  This context data was supplied by the
;                               display driver to the modeset dll.
;                               It ought to be the GNCRTCCX ptr.
;
;               lpDispData      LPDISPDATA ptr
;               lpModeOut       LPMODEOUT ptr
;               lpEdidBuffer    LPCHAR ptr to an edid buffer
;               dwEdidSize      ULONG size in bytes of edid (0 if no edid)
;
; Returns:      TRUE
;
; Preserve:     ds,esi,edi
;==============================================================================
CALL_LINKREG    MACRO   label1,linkreg
        LOCAL   quickret
        mov     linkreg,OFFSET quickret
        jmp     label1
quickret:
ENDM

DECPROC GN_Far16IsModeAcceptedByDriver, PASCAL, FRAME, FAR16
PARMD   dwContextData1
PARMD   dwContextData2
PARMD   lpDispData
PARMD   lpModeOut
PARMD   lpEdidBuffer
PARMD   dwEdidSize
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aGNCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALS  aHWCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpGNCrtcCX
LOCALD  lpHWCrtcCX
LOCALD  dwDeviceCount
LOCALD  pTheHWCrtcCX
OPENPROC
        PUSHR   ds,esi,edi

        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        mov     ds,dwContextData1
        mov     esi,dwContextData2

        ; Just in case the HWCRTCCX has been locked by somebody
        ; else, we have to look a couple places for it.
        mov     eax,ds:[esi].GNCRTCCX.pHWCrtcCX
        or      eax,eax
        jne     @F
        mov     eax,ds:[esi].GNCRTCCX.pHWCrtcCXWhenLocked
@@:     mov     pTheHWCrtcCX,eax

        ; The paramter list we have to provide is the following:
        ; pHWCrtcCXOffset
        ; dwTotalWidth0
        ; dwVisibleWidth0
        ; dwTotalHeight0
        ; dwVisibleHeight0
        ; dwPixelDepth0
        ; dwPixelClock0
        ; dwRefreshRate0
        ; dwTotalWidth1
        ; dwVisibleWidth1
        ; dwTotalHeight1
        ; dwVisibleHeight1
        ; dwPixelDepth1
        ; dwPixelClock1
        ; dwRefreshRate1
        ;
        ; The 0 on the end means physical head 0 and the 1 means
        ; physical head 1. If you are only concerned with the mode
        ; on one of the two heads (i.e. the other head is not active)
        ; set dwTotalWidth for that head to 0.

        ; push the HWCRTCCX
        push    pTheHWCrtcCX

        push    ds:[esi].GNCRTCCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData
        mov     dwDeviceCount,eax

        ; See if the device we are testing is on head 0
        push    pTheHWCrtcCX
        call    HW_GetPhysicalCrtcIndex
        or      eax,eax
        je      UseNewParamsFirst

        ; If it is on head -1, we don't know!! It may not have
        ; been mapped yet. Unh! We will assume that it is the
        ; right one in this case, though.
        cmp     eax,-1
        je      UseNewParamsFirst

        ; We are on a non-0 legal head. Are there two devices?
        cmp     dwDeviceCount,1
        je      NullForFirst

TwoDevicesNewIsNotHead0:
        lfs     bx,lpGNCrtcCX
        mov     ecx,fs:[bx]
        mov     edx,fs:[bx + 4]
        cmp     ecx,esi
        jne     @F
        mov     ecx,edx
@@:
        push    ds
        pop     fs
        lea     ecx,[ecx].GNCRTCCX.sCrtcModeOut
        CALL_LINKREG  PushParams,edx

        sub     ecx,ecx
        lfs     cx,lpModeOut
        CALL_LINKREG  PushParams,edx
        jmp     DoTheCall

NullForFirst:
        ; Nulls for head 0, and head 1 must be the "new" one
        CALL_LINKREG    PushNulls,edx

        sub     ecx,ecx
        lfs     cx,lpModeOut
        CALL_LINKREG    PushParams,edx
        jmp     DoTheCall

UseNewParamsFirst:
        sub     ecx,ecx
        lfs     cx,lpModeOut
        CALL_LINKREG    PushParams,edx

        ; OK, now, are we even concerned with two displays?
        cmp     dwDeviceCount,1
        je      NullForSecond

        ; Find the paramters for the device that isn't "new"
        lfs     bx,lpGNCrtcCX
        mov     ecx,fs:[bx]
        mov     edx,fs:[bx + 4]
        cmp     ecx,esi
        je      GotSecondCrtcCX
        mov     edx,ecx
        jmp     GotSecondCrtcCX

NullForSecond:
        CALL_LINKREG    PushNulls,edx
        jmp     DoTheCall

GotSecondCrtcCX:
        ; edx = GNCRTCCX
        lea     ecx,[edx].GNCRTCCX.sCrtcModeOut
        push    ds
        pop     fs
        CALL_LINKREG    PushParams,edx
        jmp     DoTheCall

DoTheCall:
        call    HW_IsBandwidthOK
        jmp     Done

PushParams:
        movzx   eax,fs:[ecx].MODEOUT.wHTotal
        push    eax
        movzx   eax,fs:[ecx].MODEOUT.wXRes
        push    eax
        movzx   eax,fs:[ecx].MODEOUT.wVTotal
        push    eax
        movzx   eax,fs:[ecx].MODEOUT.wYRes
        push    eax
        mov     eax,fs:[ecx].MODEOUT.dwFormat
        shr     eax,16
        and     eax,3
        inc     eax
        cmp     eax,3
        jne     @F
        mov     eax,2
@@:     shl     eax,3
        push    eax
        push    fs:[ecx].MODEOUT.dwPixelClock
        movzx   eax,fs:[ecx].MODEOUT.wRefreshRate
        push    eax
        jmp     edx

PushNulls:
        sub     eax,eax
        push    eax
        push    eax
        push    eax
        push    eax
        push    eax
        push    eax
        push    eax
        jmp     edx

Done:
        POPR    ds,esi,edi
CLOSEPROC


ALIGN 4
SystemColorTableFirst10 LABEL   BYTE
;       blue,   green,  red,    flags
DB      000H,   000H,   000H,   000H
DB      000H,   000H,   080H,   000H
DB      000H,   080H,   000H,   000H
DB      000H,   080H,   080H,   000H
DB      080H,   000H,   000H,   000H
DB      080H,   000H,   080H,   000H
DB      080H,   080H,   000H,   000H
DB      0C0H,   0C0H,   0C0H,   MAPTOWHITE
DB      0C0H,   0DCH,   0C0H,   NONSTATIC+MAPTOWHITE
DB      0F0H,   0CAH,   0A6H,   NONSTATIC+MAPTOWHITE

SystemColorTableDefault LABEL   BYTE
;       blue,   green,  red,    flags
DB      000H,   000H,   000H,   NONSTATIC

SystemColorTableLast10  LABEL   BYTE
;       blue,   green,  red,    flags
DB      0F0H,   0FBH,   0FFH,   NONSTATIC+MAPTOWHITE
DB      0A4H,   0A0H,   0A0H,   NONSTATIC+MAPTOWHITE
DB      080H,   080H,   080H,   MAPTOWHITE
DB      000H,   000H,   0FFH,   000H
DB      000H,   0FFH,   000H,   MAPTOWHITE
DB      000H,   0FFH,   0FFH,   MAPTOWHITE
DB      0FFH,   000H,   000H,   000H
DB      0FFH,   000H,   0FFH,   000H
DB      0FFH,   0FFH,   000H,   MAPTOWHITE
DB      0FFH,   0FFH,   0FFH,   MAPTOWHITE

CLOSESEG _NVTEXT16

END

