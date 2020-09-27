;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Copyright (C) 1998-1999, Nvidia Corporation
;
; File:         hwmacs.inc
;
; Purpose:      This file holds some macros that thehardware specific
;               code uses.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; A build option
DOIOTOFLUSHCACHE                TEXTEQU <1>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;IDEBUG;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;USE_ALPHA_CURSOR                TEXTEQU <0>
USE_ALPHA_CURSOR                TEXTEQU <1>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HW_WAIT                         TEXTEQU <0>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
JMP_COMMAND                     TEXTEQU <20000000H>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CHNOFF          MACRO   HwSegMember
        EXITM   <(OFFSET GDICHAN.&HwSegMember - OFFSET GDICHAN.StartOfChannelHdr)>
ENDM

CHNRUN          MACRO   HwSegMember,runlen
        EXITM   <(CHNOFF(HwSegMember) + ((runlen) SHL 18))>
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
WAIT_UNTIL_OTHER_CONTEXTS_IDLE  MACRO   sreg,hwcxreg,tempreg,mylabel
        LOCAL   skip,dskip

IF      (HW_WAIT NE 0)
        mov     tempreg,sreg:[hwcxreg].HWBOARDCX.dwGdiDriverChannelID
        cmp     tempreg,sreg:[hwcxreg].HWBOARDCX.dwCurrentChannelID
        jne     dskip
        push    hwcxreg
        call    WaitUntilGdiContextIdle
dskip:
ENDIF
        ; This macro should test a bit to see if any other contexts
        ; are active. If there are, then it should call the function
        ; WaitUntilOtherContextsIdle. The sreg:reg should point to
        ; the PDEVICE. tempreg will be trashed.
        mov     tempreg,sreg:[hwcxreg].HWBOARDCX.dwGdiDriverChannelID
        cmp     tempreg,sreg:[hwcxreg].HWBOARDCX.dwCurrentChannelID
        je      skip
        mov     sreg:[hwcxreg].HWBOARDCX.dwCurrentChannelID,tempreg
        push    hwcxreg
        call    WaitUntilOtherContextsIdle

        IFNB    <mylabel>
                jmp     mylabel
        ENDIF
skip:
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CHECKDMASPACE   MACRO   sreg,hwcxreg,pushbufferreg,bytes
        LOCAL   skip, skip2

IFDIFI  <pushbufferreg>,<edi>
        IFDIFI  <pushbufferreg>,<NONE>
                echo  pushbufferreg_ argument must either be edi or NONE in CHECKDMASPACE
                .err
        ENDIF
ENDIF
        sub     sreg:[hwcxreg].HWBOARDCX.dwBytesFree,bytes
        jge     skip
IFDIFI  <pushbufferreg>,<NONE>
        mov     sreg:[hwcxreg].HWBOARDCX.pCurrentPushBufferOffset,pushbufferreg
ENDIF
        push    hwcxreg
        pushd   bytes
        call    WaitForDmaBufferSpace

IFDIFI  <pushbufferreg>,<NONE>
        mov     pushbufferreg,sreg:[hwcxreg].HWBOARDCX.pCurrentPushBufferOffset
ENDIF


skip:
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SETDMASTATUS    MACRO   stataddr
        mov     stataddr,-1
ENDM

WAITDMASTATUS   MACRO   stataddr
        LOCAL   back

back:   cmp     stataddr,-1
        je      back
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
STARTTIME	MACRO	
IFDEF	DEBUG
	PUSHR	es,eax,edx
	mov	ax,_DATA
	mov	es,ax
	rdtsc
	mov	es:[qwStartTime],eax
	mov	es:[qwStartTime+4],edx
	POPR	es,eax,edx
ENDIF
ENDM

STOPTIME	MACRO	qwThisTime
IFDEF	DEBUG
	PUSHR	es,eax,edx
	mov	ax,_DATA
	mov	es,ax
	rdtsc
	sub	eax,es:[qwStartTime]
	sbb	edx,es:[qwStartTime+4]
	add	es:[qwThisTime],eax
	adc	es:[qwThisTime+4],edx
	POPR	es,eax,edx
ENDIF
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SETPUTPTR       MACRO   sreg,hwcxreg,pushbufferreg
        LOCAL   DoneFlush

        or      byte ptr sreg:[pushbufferreg - 40H],0
        mov     sreg:[hwcxreg].HWBOARDCX.pCurrentPushBufferOffset,pushbufferreg
        sub     pushbufferreg,sreg:[hwcxreg].HWBOARDCX.pPushBufferContextStartOffset

IF (DOIOTOFLUSHCACHE NE 0)
        push    OFFSET DoneFlush
        cmp     sreg:[hwcxreg].HWBOARDCX.dwFlushCacheWithIO,0
        jne     FlushChipsetCache
        add     sp,4
DoneFlush:
ENDIF
        mov     sreg:[hwcxreg].HWBOARDCX.dwDmaPutPtr,pushbufferreg
        mov     pushbufferreg,sreg:[hwcxreg].HWBOARDCX.pCurrentPushBufferOffset
ENDM


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
INTERNAL_BEGINACCESS    MACRO   sreg,tempreg
        LOCAL   skip
        mov     tempreg,HWSEGDATA(sreg,dwGetOffsetAtLastBeginAccess)
        cmp     tempreg,HWSEGDATA(sreg,dwCurrentPushBufferOffset)
        je      skip
        call    HW_InternalBeginAccess
skip:
ENDM


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CHECKDSTBASE    MACRO   CXSel,hwcxreg,DmaOff,DstBase,DstPitch
        LOCAL   skip

IFDIFI  <DmaOff>,<edi>
        echo The DmaOffargument to the CHECKDSTBASE macro is not edi
        echo The macro will need modification.
        .err
ENDIF
        cmp     DstBase,CXSel:[hwcxreg].HWBOARDCX.dwContextSurface2dDstOffset
        je      skip
        push    hwcxreg
        pushd   DstBase
        pushd   DstPitch
IFIDNI          <CXSel>,<ds>
        call    SetDstBaseAndPitchToDsEdi
ELSEIFIDN       <CXSel>,<es>
        call    SetDstBaseAndPitchToEsEdi
ELSE
        echo Unknown CXSel arg to macro CHECKDSTBASE
        .err
ENDIF
skip:
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NV10 requires 4K byte alignment. NV4/NV5 only require 2K but
; since 4K works for everyone, that is what we use.
HW_CURSOR_WIDTH         TEXTEQU  <CURSOR_WIDTH>
HW_CURSOR_HEIGHT        TEXTEQU  <CURSOR_HEIGHT>
HW_CURSOR_ALIGN         TEXTEQU  <4096>

NV_CURSOR_SET_PARAM     TEXTEQU <1>     ; Indicates that the rest of the
                                        ;   cursor image parameters are valid
                                        ;   and they should be programmed
                                        ;   into the hardware.
NV_CURSOR_SET_ENABLE    TEXTEQU <2>     ; Indicates that the cursor's enable
                                        ;   disable state should be changed.
NV_CURSOR_ENABLE        TEXTEQU <4>     ; Used in conjunction with
                                        ;   NV_CURSOR_SET_ENABLE.
NV_CURSOR_DISABLE       TEXTEQU <0>     ; Used in conjunction with
                                        ;   NV_CURSOR_SET_ENABLE.

; 16 bpp color cursor
; 32 bpp color cursor w/ alpha
HW_CURSOR_16_SIZE       TEXTEQU  <(HW_CURSOR_WIDTH * HW_CURSOR_HEIGHT * 2)>
HW_CURSOR_32_SIZE       TEXTEQU  <(HW_CURSOR_WIDTH * HW_CURSOR_HEIGHT * 4)>

IF      USE_ALPHA_CURSOR
HW_CURSOR_SIZE  TEXTEQU <((HW_CURSOR_32_SIZE + HW_CURSOR_ALIGN - 1) AND (NOT (HW_CURSOR_ALIGN - 1)))>
ELSE
HW_CURSOR_SIZE  TEXTEQU <((HW_CURSOR_16_SIZE + HW_CURSOR_ALIGN - 1) AND (NOT (HW_CURSOR_ALIGN - 1)))>
ENDIF   ; USE_ALPHA_CURSOR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
; hwaccess
EXTERNDEF       WaitUntilGdiContextIdle:NEAR

; hwboard.asm
EXTERNDEF       NV_GetDeviceNumber:NEAR
EXTERNDEF       NV_AllocRoot:NEAR
EXTERNDEF       NV_FreeRoot:NEAR
EXTERNDEF       NV_AllocDevice:NEAR
EXTERNDEF       NV_GetMemoryInfo:NEAR
EXTERNDEF       NV_GetDeviceInfo:NEAR
EXTERNDEF       NV_GetDeviceVersion:NEAR
EXTERNDEF       NV_GetBusType:NEAR
EXTERNDEF       NV_GetNumCRTCs:NEAR
EXTERNDEF       NV_AllocPushBuffer:NEAR
EXTERNDEF       NV_AllocChannelDma:NEAR
EXTERNDEF       NV_AllocPages:NEAR
EXTERNDEF       NV_AllocMemory:FAR32
EXTERNDEF       NV_FreeMemory:FAR32
EXTERNDEF       NV_AllocLinearAddressSpace:NEAR
EXTERNDEF       NV_FreeLinearAddressSpace:NEAR
EXTERNDEF       NV_CopyPTEsForLinearRange:NEAR
EXTERNDEF       NV_GetGdiChannelMask:NEAR
EXTERNDEF       NV_AllocRequiredMemory:NEAR
EXTERNDEF       NV_GetCursorCacheInfo:NEAR
EXTERNDEF       NV_BoardSetDXSharedData:FAR32

; hwboard2.asm
EXTERNDEF       Enable_HiRes:FAR32
EXTERNDEF       NV_ConfigExCall:FAR32
EXTERNDEF       NV_ConfigExSetCall:FAR32
EXTERNDEF       NV_OffscreenHeapAlloc:FAR32
EXTERNDEF       NV_OffscreenHeapFree:FAR32
EXTERNDEF       NV_OffscreenHeapPurge:FAR32

; hwcrtc.asm
EXTERNDEF       NV_CrtcSetLogdev:FAR32
EXTERNDEF       HW_GetPhysicalCrtcIndex:FAR32
EXTERNDEF       NV_GetEncoderInfo:NEAR
EXTERNDEF       CreateVideoLutCursorDac:NEAR
EXTERNDEF       DestroyVideoLutCursorDac:NEAR
EXTERNDEF       NV_AllocHeadRelatedInfo:NEAR
EXTERNDEF       NV_FreeHeadRelatedInfo:NEAR

; hwcrtc2.asm
EXTERNDEF       NV_SetColorMap:FAR32

; Functions in hwinit.asm
EXTERNDEF       NvSysTable:WORD
EXTERNDEF       hNvSysDll:WORD
EXTERNDEF       wHWCSAlias:WORD
EXTERNDEF       lpfnNvSysMoveCursor:DWORD
EXTERNDEF       lpfnNvSysUpdateImageOffsetFormat:DWORD
EXTERNDEF       lpfnNvSysGetRamInfo:DWORD
EXTERNDEF       lpfnNvSysAllocPages:DWORD
EXTERNDEF       lpfnNvSysFreePages:DWORD
EXTERNDEF       lpfnNvConfigGet:DWORD
EXTERNDEF       lpfnNvConfigSet:DWORD
EXTERNDEF       lpfnNvConfigUpdate:DWORD
EXTERNDEF       lpfnNvConfigVga:DWORD
EXTERNDEF       lpfnNvSysSetCursorImage:DWORD
EXTERNDEF       lpfnNvSysGetHardwarePointers:DWORD
EXTERNDEF       lpfnNvDeviceBaseGet:DWORD
EXTERNDEF       lpfnNvDeviceLimitGet:DWORD
EXTERNDEF       lpfnNvSysDriverConnect:DWORD
EXTERNDEF       lpfnNvSysDriverDisConnect:DWORD
EXTERNDEF       lpfnNvSysRegisterDisplayDriver:DWORD
EXTERNDEF       lpfnNvIoControl:DWORD
EXTERNDEF       lpfnArch_Nv01Free:DWORD
EXTERNDEF       lpfnArch_Nv01AllocRoot:DWORD
EXTERNDEF       lpfnArch_Nv01AllocMemory:DWORD
EXTERNDEF       lpfnArch_Nv01AllocContextDma:DWORD
EXTERNDEF       lpfnArch_Nv01AllocChannelPio:DWORD
EXTERNDEF       lpfnArch_Nv01AllocObject:DWORD
EXTERNDEF       lpfnArch_Nv01AllocDevice:DWORD
EXTERNDEF       lpfnArch_Nv03AllocChannelDma:DWORD
EXTERNDEF       lpfnArch_Nv03DmaFlowControl:DWORD
EXTERNDEF       lpfnArch_Nv01Interrupt:DWORD
EXTERNDEF       lpfnArch_Nv01AllocEvent:DWORD
EXTERNDEF       lpfnArch_NvI2CAccess:DWORD
EXTERNDEF       lpfnArch_Nv04ConfigGetEx:DWORD
EXTERNDEF       lpfnArch_Nv04ConfigSetEx:DWORD
EXTERNDEF       lpfnArch_Nv04Alloc:DWORD

; hwcur32.asm
IF  (USE_ALPHA_CURSOR NE 0)
EXTERNDEF       HW_CursorSet8888:NEAR
EXTERNDEF       SetCursor_1Bpp_8888:NEAR
EXTERNDEF       SetCursor_4Bpp_8888:NEAR
EXTERNDEF       SetCursor_8Bpp_8888:NEAR
EXTERNDEF       SetCursor_15Bpp_8888:NEAR
EXTERNDEF       SetCursor_16Bpp_8888:NEAR
EXTERNDEF       SetCursor_24Bpp_8888:NEAR
EXTERNDEF       SetCursor_32Bpp_8888:NEAR
EXTERNDEF       SetCursor_3XBpp_8888:NEAR
ENDIF

; hwcursor.asm
EXTERNDEF       SetCursor_1Bpp_1555:NEAR
EXTERNDEF       SetCursor_4Bpp_1555:NEAR
EXTERNDEF       SetCursor_8Bpp_1555:NEAR
EXTERNDEF       SetCursor_15Bpp_1555:NEAR
EXTERNDEF       SetCursor_16Bpp_1555:NEAR
EXTERNDEF       SetCursor_24Bpp_1555:NEAR
EXTERNDEF       SetCursor_32Bpp_1555:NEAR
EXTERNDEF       SetCursor_3XBpp_1555:NEAR

; in hwpatblt.asm
EXTERNDEF       bConvertPatDstToSrcDstRop:BYTE

; hwrect.asm
EXTERNDEF       bConvertRop2ToRop3:BYTE

; Functions in misc.asm
EXTERNDEF       dwContextSurface2DColorFormat:DWORD
EXTERNDEF       SetDstBaseAndPitchToDsEdi:NEAR
EXTERNDEF       SetDstBaseAndPitchToEsEdi:NEAR
EXTERNDEF       WaitUntilOtherContextsIdle:NEAR
EXTERNDEF       FlushChipsetCache:NEAR
EXTERNDEF       WaitForDmaBufferSpace:NEAR

; misc.asm
EXTERNDEF       SetDstBaseAndPitchToDsEdi:NEAR
EXTERNDEF       SetDstBaseAndPitchToEsEdi:NEAR
EXTERNDEF       WaitUntilOtherContextsIdle:NEAR
EXTERNDEF       FlushChipsetCache:NEAR
EXTERNDEF       WaitForDmaBufferSpace:NEAR

; objects.asm
EXTERNDEF       NV_CreateDmaContexts:FAR32
EXTERNDEF       NV_DestroyDmaContexts:FAR32
EXTERNDEF       NV_CreateObjects:FAR32
EXTERNDEF       NV_DestroyObjects:FAR32
EXTERNDEF       NV_AllocContextDma:FAR32
EXTERNDEF       NV_FreeContextDma:FAR32
EXTERNDEF       NV_AllocWinChannelObject:FAR32
EXTERNDEF       NV_FreeWinChannelObject:FAR32
EXTERNDEF       NV_RMAllocObject:FAR32
CLOSESEG _NVTEXT32


OPENSEG  _NVTEXT16, USE16, CODE
; hwcntrl.asm
EXTERNDEF       NV_GetFlatModeSwitchPtrCount:NEAR
EXTERNDEF       NV_GetFlatActiveChannelPtr:NEAR
EXTERNDEF       NV_GetEscapeID:NEAR
EXTERNDEF       NV_IsNvidiaBasedBoardAndDiver:NEAR
EXTERNDEF       NV_GetHardwarePointers:NEAR
EXTERNDEF       NV_GetFlatDirectXSharedData:NEAR
EXTERNDEF       NV_IncModeSitchCounter:NEAR
EXTERNDEF       NV_IntelDiags:NEAR

; hwidiag
EXTERNDEF       NV_IntelDiagnostics:FAR16

; hwlogdev2.asm
EXTERNDEF       NV_PostModeset:FAR16
EXTERNDEF       NV_CreateDrvObj:FAR16
EXTERNDEF       DirectCallToFlatland:FAR16
EXTERNDEF       NV_DirectXServiceNoFrame:FAR32
EXTERNDEF       NV_DirectXService:NEAR
CLOSESEG _NVTEXT16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

