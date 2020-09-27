;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwboard.asm
;
; Purpose:      This file contains all the hardware specific BOARD
;               specific routines for init. The board specific run-
;               time routines are in hwboard2.asm.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include hwmacs.inc
include nvcm.inc
include nvreg.inc
include dxshare.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc
include escape.inc

OPENSEG  _DATA, USE16, DATA
szNvSysDll              DB      NV4_RM_16_BINARY,0
szModeSetDll            DB      NV4_MODESET_BINARY,0
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_CreateBoardContext
;
; Purpose:      This routine gets called from the generic code
;               the first time a PDEVICE is created on a given
;               board. This routine should create its BOARD
;               specific data and fill it in.
;
;               This routine will only be matched by a call to
;               HW_DestroyBoardContext when the last PDEVICE
;               initted on this board is going away.
;
; Arguments:
;               ds                      Context Selector
;
; Returns:      eax     0       failure
;                       else    This should be a ptr to what the HW
;                               wants as its HWBOARDCX ptr.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CreateBoardContext, PASCAL, FRAME, FAR32
PARMD   dwThisLogDeviceHandle
OPENPROC
        PUSHR   esi,edi
        sub     esi,esi

        pushd   SIZE HWBOARDCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax
        mov     ds:[esi].HWBOARDCX.pHWBoardCXReal,eax

        ; Set this to 1 right now.
        mov     ds:[esi].HWBOARDCX.dwInitting,1

        ; Get our device Number from the RM
        push    dwThisLogDeviceHandle
        call    NV_GetDeviceNumber
        or      eax,eax
        je      DoneIt

        ; Allocate the root node from the RM
        call    NV_AllocRoot
        or      eax,eax
        je      DoneIt

        ; Allocate our device slot from the RM
        call    NV_AllocDevice
        or      eax,eax
        je      DoneIt

        ; Get information about video memory.
        call    NV_GetMemoryInfo
        or      eax,eax
        je      DoneIt

        ; Get information on the chip base
        call    NV_GetDeviceInfo
        or      eax,eax
        je      DoneIt

        ; Get information on the chip version
        call    NV_GetDeviceVersion
        or      eax,eax
        je      DoneIt

        ; Get information of bus type
        call    NV_GetBusType
        or      eax,eax
        je      DoneIt

        ; We need to know how many CRTCs there are on this board.
        call    NV_GetNumCRTCs
        or      eax,eax
        je      DoneIt

        ; Allocate the push buffer
        call    NV_AllocPushBuffer
        or      eax,eax
        je      DoneIt

        ; OK, Here is the linear base of the real push buffer
        mov     ds:[esi].HWBOARDCX.pRealPushBuffer,ebx

        ; Allocate a context DMA for the PUSHBUFFER
        push    esi
        pushd   OBJ_DMA_PUSH_BUFFER
        push    ds:[esi].HWBOARDCX.pRealPushBuffer
        push    ds
        pushd   (DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY
        call    NV_AllocContextDma
        or      eax,eax
        je      DoneIt

        ; Allocate a channel for use with the DMA push buffer
        mov     eax,OBJ_DMA_PUSH_BUFFER
        call    NV_AllocChannelDma
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWBOARDCX.pDmaChannel,eax

        ; Allocate linear address space of the same size as the
        ; HWBOARDCX + DMA_PUSH_BUFFER_SIZE so we can remap pages.
        mov     eax,(SIZE HWBOARDCX + DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        call    NV_AllocLinearAddressSpace
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWBOARDCX.pAliasedHWBoardCX,eax

        ; Copy the PTEs for the HWBOARDCX ptr passed in to the
        ; aliased HWBOARDCX so the two ptrs reference the same
        ; physical memory.
        mov     eax,esi
        mov     ebx,ds:[esi].HWBOARDCX.pAliasedHWBoardCX
        mov     ecx,(SIZE HWBOARDCX) AND 0FFFFF000H
        call    NV_CopyPTEsForLinearRange
        or      eax,eax
        je      DoneIt

        ; Copy the PTEs for the Real DMA_PUSH_BUFFER memory allocated
        ; previously into the PTEs of our linear address space to the
        ; pages immediately after the ones mapping the HWBOARDCX. This
        ; way the HWBOARDCX and DMA_PUSH_BUFFER are linearly contiguous.
        mov     eax,ds:[esi].HWBOARDCX.pRealPushBuffer
        mov     ebx,ds:[esi].HWBOARDCX.pAliasedHWBoardCX
        add     ebx,(SIZE HWBOARDCX) AND 0FFFFF000H
        mov     ecx,(DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        call    NV_CopyPTEsForLinearRange
        or      eax,eax
        je      DoneIt

        ; Modify the mapping of the page in the middle of the
        ; HWBOARDCX structure that is supposed to point to the
        ; Nv04ControlDma structure.
        mov     eax,ds:[esi].HWBOARDCX.pDmaChannel
        mov     ebx,ds:[esi].HWBOARDCX.pAliasedHWBoardCX
        lea     ebx,[ebx].HWBOARDCX.bHWPageStart
        mov     ecx,1000H
        call    NV_CopyPTEsForLinearRange
        or      eax,eax
        je      DoneIt

        ; Now we have to switch over to the mapped HWBOARDCX ptr
        ; since it also has pages that point to the hardware and
        ; to the push buffer mapped after the end of the HWBOARDCX
        ; itself.
        mov     esi,ds:[esi].HWBOARDCX.pAliasedHWBoardCX

        ; Create all the DMA contexts we will need.
        call    NV_CreateDmaContexts
        or      eax,eax
        je      DoneIt

        ; Watch out for chipsets that can't get the cache coherency
        ; implemented correctly.
IF (DOIOTOFLUSHCACHE NE 0)
        pushd   NV_CFG_IO_NEEDED
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        mov     ds:[esi].HWBOARDCX.dwFlushCacheWithIO,eax
ELSE
        mov     ds:[esi].HWBOARDCX.dwFlushCacheWithIO,0
ENDIF

        ; Init the push buffer variables
        lea     edi,ds:[esi].HWBOARDCX.bScrapSpace
        mov     ds:[esi].HWBOARDCX.pPushBufferContextStartOffset,edi
        lea     edi,ds:[esi].HWBOARDCX.bDmaPushBuffer
        mov     ds:[esi].HWBOARDCX.pPushBufferStartOffset,edi
        mov     ds:[esi].HWBOARDCX.dwPushBufferLength,DMA_PUSH_BUFFER_SIZE - WRITE_COMBINE_PAD_SIZE - 10H
        mov     ds:[esi].HWBOARDCX.dwBytesFree,DMA_PUSH_BUFFER_SIZE - WRITE_COMBINE_PAD_SIZE - 08H

        ; The minimum and maximum pitces of a context surface 2D are
        ; dependent upon whether the board is nv04, 05, 10, etc.
        mov     eax,20H
        mov     ebx,8160
        cmp     ds:[esi].HWBOARDCX.dwDeviceVersion,NV_DEVICE_VERSION_10
        jl      @F
        mov     eax,40H
        mov     ebx,10000H
@@:     mov     ds:[esi].HWBOARDCX.dwMinContextSurface2dPitch,eax
        mov     ds:[esi].HWBOARDCX.dwMaxContextSurface2dPitch,ebx
        imul    eax,10001H
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch,eax

        ; Make sure that this gets reinitted properly in BeginAccess
        ; by setting it to an impossible push buffer offset now.
        mov     eax,-1
        mov     ds:[esi].HWBOARDCX.dwGetOffsetAtLastBeginAccess,eax

        ; The DMA pusher should be up and running. I'm going to put
        ; some NOPS in the push buffer to get it started. The first
        ; 20H bytes or so of the push buffer are not used because
        ; may need them as a place to access that we know is far
        ; enough away from the last CPU access to WRITE COMBINE
        ; memory so that a flush of the CPU WRITE COMBINE buffers
        ; is forced.
        push    ds
        pop     es
        mov     edi,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        sub     eax,eax
        mov     ecx,(WRITE_COMBINE_PAD_SIZE + 3) / 4
        rep     stos dword ptr es:[edi]
        SETPUTPTR  ds,esi,edi

        ; Now let's just wait until the push buffer fetches those
        ; NOP commands.
        mov     eax,edi
        sub     eax,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
@@:     cmp     ds:[esi].HWBOARDCX.dwDmaGetPtr,eax
        jne     @B

        ; Create all the obects we will need.
        call    NV_CreateObjects
        or      eax,eax
        je      DoneIt

        ; Let's figure out our channel mask
        call    NV_GetGdiChannelMask

        ; Now we have to allocate some memory that will NEVER
        ; go away. This will be for the LUT and for however
        ; many cursors are on the board.
        call    NV_AllocRequiredMemory
        or      eax,eax
        je      DoneIt

        ; Return the HWBOARDCX ptr that we really want to use
        mov     eax,esi

DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the HWBOARDCX was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; HW_DestroyBoardContext can clean up partial inits
        push    esi
        call    HW_DestroyBoardContext
        sub     eax,eax

ExitIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_DestroyBoardContext
;
; Purpose:      This routine gets called to destroy a HWBOARDCX.
;
; Arguments:
;               ds                      context selector
;               pHWBoardCXOffset        HWBOARDCX to destroy
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DestroyBoardContext, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
LOCALV  FreeParam,NV01FREE_PARAM
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWBoardCXOffset
        or      esi,esi
        je      DoneIt

        ; Destroy the objects
        call    NV_DestroyObjects

        ; Destroy the context DMAs
        call    NV_DestroyDmaContexts

        ; pAliasedHWBoardCX was allocated via the function
        ; NV_AllocLinearAddressSpace an must be freed by calling
        ; NV_FreeLinearAddressSpace.
        cmp     ds:[esi].HWBOARDCX.pAliasedHWBoardCX,0
        je      @F

        ; esi is currently using the aliased HWBoardCX.
        ; We are about to free this memory block, so we must
        ; switch esi back over to the real allocation offset
        ; which is a different linear range the first part
        ; of which maps to the ssame physical range -- the HWBOARDCX.
        mov     esi,ds:[esi].HWBOARDCX.pHWBoardCXReal

        mov     eax,ds:[esi].HWBOARDCX.pAliasedHWBoardCX
        mov     ebx,(SIZE HWBOARDCX + DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        call    NV_FreeLinearAddressSpace
        mov     ds:[esi].HWBOARDCX.pAliasedHWBoardCX,0
@@:
        ; Destroy the DMA channel
        mov     bx,ss
        shl     ebx,10H
        lea     bx,FreeParam
        push    ebx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     FreeParam.NV01FREE_hClient,ecx
        mov     FreeParam.NV01FREE_hObjectParent,NV_WIN_DEVICE
        mov     FreeParam.NV01FREE_hObjectOld,NV_WIN_CHANNEL
        mov     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        CALLFAR16IND lpfnArch_Nv01Free,es,ax

        ; Destroy the push buffer context DMA
        push    esi
        pushd   OBJ_DMA_PUSH_BUFFER
        call    NV_FreeContextDma

        ; If the real push buffer was allocated via SysAllocPages, we
        ; need to free the memory now. If it was allocated with the
        ; NV_AllocMemory function, then the memory will get freed when
        ; we free the RootHandle.
        cmp     esi,ds:[esi].HWBOARDCX.dwRealPushBufferPageHandle
        je      @F
        push    ds:[esi].HWBOARDCX.dwRealPushBufferPageHandle
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvSysFreePages,es,ax
        mov     ds:[esi].HWBOARDCX.dwRealPushBufferPageHandle,0
@@:

        ; Free the screen sel
        mov     eax,ds:[esi].HWBOARDCX.dwVideoMemorySel
        or      eax,eax
        je      @F
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].HWBOARDCX.dwVideoMemorySel,0
@@:
        ; Now we can free the root.
        call    NV_FreeRoot

        mov     eax,ds:[esi].HWBOARDCX.dwDeviceNum
        or      eax,eax
        jz      @F
        push    eax
        CALLFAR16IND lpfnNvSysDriverDisConnect,es,ax
@@:
        push    esi
        call    HW_FreeFlatMemory

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetDeviceNumber
;
; Purpose:      Determines the device number for the board controlled by
;               this particular driver since there may be multiple NVidia
;               cards in the same system.
;
; Arguments:    ds:esi          HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetDeviceNumber, PASCAL, FRAME, NEAR
PARMD   dwThisLogDeviceHandle
OPENPROC
        ; Call SysDriverConnect to get the device number.
        mov     edx,dwThisLogDeviceHandle
        sub     eax,eax
        cmp     edx,1
        je      @F
        and     edx, NOT 3H
@@:
        push    edx
        push    eax
        CALLFAR16IND lpfnNvSysDriverConnect,es,ax
        shl     edx,16
        mov     dx,ax
        mov     ds:[esi].HWBOARDCX.dwDeviceNum,edx
        mov     eax,edx
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocRoot
;
; Purpose:      Allocates the root node for this device.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocRoot, PASCAL, FRAME, NEAR
LOCALV  AllocRootParam,ALLOC_ROOT_PARAM
OPENPROC
        ; Get and save the handle to the root
        mov     ax,ss
        lea     bx,AllocRootParam
        push    ax
        push    bx
        mov     edx,ALLOC_ROOT_CLASS_ROOT
        mov     ecx,ALLOC_ROOT_STATUS_SUCCESS
        mov     AllocRootParam.ALLOC_ROOT_PARAM.ALLOC_ROOT_hClass,edx
        mov     AllocRootParam.ALLOC_ROOT_PARAM.ALLOC_ROOT_status,ecx
        CALLFAR16IND lpfnArch_Nv01AllocRoot,es,ax
        mov     eax,AllocRootParam.ALLOC_ROOT_PARAM.ALLOC_ROOT_hObjectNew
        mov     ds:[esi].HWBOARDCX.dwRootHandle,eax

        mov     ecx,ALLOC_ROOT_STATUS_SUCCESS
        cmp     ecx,AllocRootParam.ALLOC_ROOT_PARAM.ALLOC_ROOT_status
        sete    al
        movzx   eax,al
CLOSEPROC


;==============================================================================
;
; Function:     NV_FreeRoot
;
; Purpose:      Frees the root node for this device. This will cause
;               everything allocated under this root to be freed also.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeRoot, PASCAL, FRAME, NEAR
LOCALV  FreeParam,NV01FREE_PARAM
OPENPROC
        mov     bx,ss
        push    bx
        lea     bx,FreeParam
        push    bx
        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     FreeParam.NV01FREE_hClient,eax
        mov     FreeParam.NV01FREE_hObjectParent,eax
        mov     FreeParam.NV01FREE_hObjectOld,eax
        mov     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        CALLFAR16IND lpfnArch_Nv01Free,es,ax

        cmp     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        sete    al
        movzx   eax,al
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocDevice
;
; Purpose:      Allocates a device slot from the RM.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocDevice, PASCAL, FRAME, NEAR
LOCALV  AllocDevParam,ALLOC_DEV_PARAM
LOCALS  szDevName,10H
OPENPROC
        ; Call and allocate our "device"
        mov     ax,ss
        lea     bx,AllocDevParam
        push    ax
        push    bx
        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_hObjectParent,eax
        mov     eax,NV_WIN_DEVICE
        mov     AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_hObjectNew,eax
        mov     eax,ds:[esi].HWBOARDCX.dwDeviceNum
        add     eax,(NV01_DEVICE_0 - 1)
        mov     AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_hClass,eax

        push    ss
        call    GN_GetSelectorBase
        lea     edx,szDevName
        add     edx,eax
        mov     AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_szName,edx
        mov     ecx,ALLOC_DEV_STATUS_SUCCESS
        mov     AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_status,ecx
        CALLFAR16IND lpfnArch_Nv01AllocDevice,es,ax

        ; Now get the handle and save it
        mov     eax,AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_hObjectNew
        mov     ds:[esi].HWBOARDCX.dwhDevice,eax

        ; Set the return code.
        mov     ecx,ALLOC_DEV_STATUS_SUCCESS
        cmp     ecx,AllocDevParam.ALLOC_DEV_PARAM.ALLOC_DEV_status
        sete    al
        movzx   eax,al
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetMemoryInfo
;
; Purpose:      This routine gets the amount of onboard memory and
;               information about the memory capabilities.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetMemoryInfo, PASCAL, NOFRAME, NEAR
OPENPROC
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvSysGetRamInfo,es,ax
        movzx   eax,ax
        shl     edx,20
        mov     ds:[esi].HWBOARDCX.dwRamFlags,eax
        mov     ds:[esi].HWBOARDCX.dwVideoMemorySize,edx

        pushd   NV_DEV_BUFFER_0
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvDeviceBaseGet,es,ax
        shl     edx,10H
        mov     dx,ax
        mov     ds:[esi].HWBOARDCX.pVideoMemory,edx

        ; Alloc a selector to map to this linear address
        mov     eax,1
        mov     ebx,edx
        mov     ecx,ds:[esi].HWBOARDCX.dwVideoMemorySize
        call    GN_AllocSelectorAndMapIt
        mov     ds:[esi].HWBOARDCX.dwVideoMemorySel,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetDeviceInfo
;
; Purpose:      This routine gets the linear base address of the device
;               and maps a selector to this linear base.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetDeviceInfo, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Get the linear base address of the chip
        pushd   NV_DEV_BASE
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvDeviceBaseGet,es,ax
        shl     edx,10H
        mov     dx,ax
        mov     ds:[esi].HWBOARDCX.pChipRegs,edx
        mov     eax,edx
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetDeviceVersion
;
; Purpose:      This routine gets the device version from the RM.
;               Basically, are we running on NV4, NV5, or NV10.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetDeviceVersion, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Get the linear base address of the chip
        pushd   NV_CFG_ARCHITECTURE
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        push    eax

        pushd   NV_CFG_REVISION
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        pop     ebx

        ; Is it an NV20 or better?
        mov     ecx,NV_DEVICE_VERSION_20
        cmp     ebx,NV_20_ARCHITECTURE
        jge     Done

        ; Is it an NV10?
        mov     ecx,NV_DEVICE_VERSION_10
        cmp     ebx,NV_10_ARCHITECTURE
        je      Done

        ; If the architecture is < 5, then revision 0 is NV4, else NV5
        mov     ecx,NV_DEVICE_VERSION_4
        or      eax,eax
        je      Done
        mov     ecx,NV_DEVICE_VERSION_5
Done:
        mov     ds:[esi].HWBOARDCX.dwDeviceVersion,ecx

        ; We also want the revision
        pushd   NV_CFG_REVISION
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        mov     ds:[esi].HWBOARDCX.dwDeviceRevision,eax

        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetBusType
;
; Purpose:      This routine gets the bus type of the card -- PCI or AGP.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetBusType, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Get the linear base address of the chip
        pushd   NV_CFG_BUS_TYPE
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        movzx   eax,al
        mov     ds:[esi].HWBOARDCX.dwBusType,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetNumCRTCs
;
; Purpose:      This routine returns the number of CRTCs on the
;               board.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetNumCRTCs, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Get the linear base address of the chip
        pushd   NV_CFG_NUMBER_OF_HEADS
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        mov     ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard,eax
        mov     ds:[esi].HWBOARDCX.dwCRTCsAllocatedFlags,0
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocPushBuffer
;
; Purpose:      This routine allocates the push buffer. I made it a
;               separate routine because potentially I want to be able
;               to allocate the push buffer in PCI, AGP or video memory.
;               There may be some heuristic for determining which one.
;
;               Right now, the push buffer always goes in PCI memory
;               for a retail build, but for testing, I can stick it
;               in video memory and AGP as well.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       Failure
;                       non-0   Success in which case
;                               ebx     linear base of push buffer
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocPushBuffer, PASCAL, FRAME, NEAR
LOCALV  sMem,ALLOC_MEMORY_PARAM
OPENPROC
IFDEF   DEBUG
        cmp     cs:[bLocatePushBuffer],0
        je      PciBuffer
        cmp     cs:[bLocatePushBuffer],1
        je      VidBuffer
        cmp     cs:[bLocatePushBuffer],2
        je      AgpBuffer
        jmp     PciBuffer

PUBLIC  bLocatePushBuffer
bLocatePushBuffer       DB      0

AgpBuffer:
        mov     eax,(DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        mov     ebx,ALLOC_MEMORY_FLAGS_CONTIGUOUS_PHYSICALITY + ALLOC_MEMORY_FLAGS_LOCATION_AGP
        mov     ecx,OBJ_DMA_PUSH_MEMORY
        call    NV_AllocMemory
        or      eax,eax
        je      PciBuffer
        jmp     DoneBuffer

VidBuffer:
        push    esi
        pushd   NVIOCONTROL_HEAP_ALLOC_SIZE
        pushd   GDI2D_HEAP_OWNER
        pushd   NVIOCONTROL_TYPE_CURSOR
        pushd   (DMA_PUSH_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        call    NV_OffscreenHeapAlloc
        cmp     eax,-1
        je      PciBuffer

        add     eax,ds:[esi].HWBOARDCX.pVideoMemory
        mov	ebx,eax
        jmp     DoneBuffer
ENDIF

PciBuffer:
        ; Allocate the push buffer in PCI memory
        mov     eax,(DMA_PUSH_BUFFER_SIZE + 0FFFH) SHR 0CH
        call    NV_AllocPages
        mov     ds:[esi].HWBOARDCX.dwRealPushBufferPageHandle,eax

DoneBuffer:
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocChannelDma
;
; Purpose:      This function allocates a channel for an existing
;               context DMA object.
;
; Arguments:
;               ds:esi  HWBOARDCX
;               eax     Object ID
;
; Returns:      eax     linear base of a Nv04ControlDma structure
;                       0 if there was an error
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocChannelDma, PASCAL, FRAME, NEAR
LOCALV  AllocChannelDmaParam,ALLOC_CHL_DMA_PARAM
OPENPROC
        PUSHSREG ss
        lea     bx,AllocChannelDmaParam
        push    bx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hClient,ecx
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hObjectParent,NV_WIN_DEVICE
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hObjectNew,NV_WIN_CHANNEL
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hClass,NV04_CHANNEL_DMA
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hObjectError,NV01_NULL_OBJECT
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_hObjectBuffer,eax
        sub     edx,edx
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_offset,edx
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_pControl,edx
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_pControl+4,edx
        mov     AllocChannelDmaParam.ALLOC_CHL_DMA_status,edx
        CALLFAR16IND lpfnArch_Nv03AllocChannelDma,es,ax
        mov     eax,AllocChannelDmaParam.ALLOC_CHL_DMA_pControl
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocPages
;
; Purpose:      This routine allocates a number of pages of memory
;               typically to be used as a DMA buffer, but they can
;               be used for anything.
;
; Arguments:    ds:esi  HWBOARDCX
;               eax     number of pages to allocate
;
; Returns:      eax     PageHandle      (0 if failure)
;               ebx     LinearBase
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocPages, PASCAL, FRAME, NEAR
LOCALD  dwLinearBase
LOCALD  dwPageHandle
OPENPROC
        ; Call the RM to allocate some pages
        mov     cx,ss
        lea     bx,dwLinearBase
        push    cx
        push    bx
        lea     bx,dwPageHandle
        push    cx
        push    bx
        push    eax
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvSysAllocPages,es,ax

        ; Return the right things
        mov     eax,dwPageHandle
        mov     ebx,dwLinearBase
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocMemory
;
; Purpose:      This routine allocates different amounts and types
;               of memory. It is typically used to allocate a DMA
;               buffer, but can be used for anything. It also maps
;               a selector:0 to the base address of the allocation.
;
; Arguments:    ds:esi  HWBOARDCX
;               eax     number of bytes to allocate
;               ebx     allocation flags
;               ecx     Object ID
;
; Returns:      eax     0       Failure
;                       non-0   Success in which case
;                               ebx     LinearBase
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocMemory, PASCAL, FRAME, FAR32
LOCALV  nvAllocMemoryParam,ALLOC_MEMORY_PARAM
OPENPROC
        mov     dx,ss
        push    dx
        lea     dx,nvAllocMemoryParam
        push    dx
        mov     edx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     nvAllocMemoryParam.ALLOC_MEMORY_hObjectClient,edx
        mov     edx,ds:[esi].HWBOARDCX.dwhDevice
        mov     nvAllocMemoryParam.ALLOC_MEMORY_hObjectParent,edx
        sub     edx,edx
        mov     nvAllocMemoryParam.ALLOC_MEMORY_hObjectNew,ecx
        mov     nvAllocMemoryParam.ALLOC_MEMORY_hClass,NV1_MEMORY_SYSTEM
        mov     nvAllocMemoryParam.ALLOC_MEMORY_flags,ebx
        mov     nvAllocMemoryParam.ALLOC_MEMORY_pBase,edx
        mov     nvAllocMemoryParam.ALLOC_MEMORY_pBase+4,edx
        dec     eax
        mov     nvAllocMemoryParam.ALLOC_MEMORY_pLimit,eax
        mov     nvAllocMemoryParam.ALLOC_MEMORY_pLimit+4,edx
        mov     nvAllocMemoryParam.ALLOC_MEMORY_status,ALLOC_MEMORY_STATUS_SUCCESS
        CALLFAR16IND lpfnArch_Nv01AllocMemory,es,ax

        cmp     nvAllocMemoryParam.ALLOC_MEMORY_status,ALLOC_MEMORY_STATUS_SUCCESS
        sete    al
        movzx   eax,al
        mov     ebx,nvAllocMemoryParam.ALLOC_MEMORY_pBase
CLOSEPROC


;==============================================================================
;
; Function:     NV_FreeMemory
;
; Purpose:      This routine allocates different amounts and types
;               of memory. It is typically used to allocate a DMA
;               buffer, but can be used for anything. It also maps
;               a selector:0 to the base address of the allocation.
;
; Arguments:    ds:esi  HWBOARDCX
;               ecx     Object ID
;
; Returns:      eax     0       Failure
;                       non-0   Success in which case
;                               ebx     LinearBase
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeMemory, PASCAL, FRAME, FAR32
LOCALV  FreeParam,NV01FREE_PARAM
OPENPROC
        mov     bx,ss
        push    bx
        lea     bx,FreeParam
        push    bx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     FreeParam.NV01FREE_hClient,ecx
        mov     edx,ds:[esi].HWBOARDCX.dwhDevice
        mov     FreeParam.NV01FREE_hObjectParent,ecx
        mov     FreeParam.NV01FREE_hObjectOld,ecx
        mov     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        CALLFAR16IND lpfnArch_Nv01Free,es,ax

        cmp     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        sete    al
        movzx   eax,al
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocLinearAddressSpace
;
; Purpose:      This function can be used to allocate linear address
;               space via the minivdd. There is no physical memory to
;               back the address space.
;
; Arguments:    ds:esi  HWBOARDCX
;               eax     bytes of linear address space to allocate
;
; Returns:      eax     linear base of address space  (0 if error)
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocLinearAddressSpace, PASCAL, FRAME, NEAR
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegEDX,eax
        mov     rmRegs.RegECX,MVDD_ALLOC_LINEAR_ADDRESS_SPACE
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
        mov     eax,rmRegs.RegEAX
CLOSEPROC

;==============================================================================
;
; Function:     NV_FreeLinearAddressSpace
;
; Purpose:      This function can be used to free linear address
;               space via the minivdd.
;
; Arguments:    ds:esi  HWBOARDCX
;               eax     base of linear address space to free
;               ebx     length in bytes of linear address range to free
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeLinearAddressSpace, PASCAL, FRAME, NEAR
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegEDX,eax
        mov     rmRegs.RegECX,MVDD_FREE_LINEAR_ADDRESS_SPACE
        mov     rmRegs.RegEBX,ebx
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
CLOSEPROC

;==============================================================================
;
; Function:     NV_CopyPTEsForLinearRange
;
; Purpose:      This function can be used to copy the PTEs behind one
;               linear address range to the PTEs behind another linear
;               address range making both ranges address the same
;               physical memory.
;
; Arguments:    ds:esi  HWBOARDCX
;               eax     src linear range
;               ebx     dst linear range
;               ecx     length of linear range in bytes
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_CopyPTEsForLinearRange, PASCAL, FRAME, NEAR
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegEDX,eax
        mov     rmRegs.RegEBX,ebx
        mov     rmRegs.RegEDI,ecx
        mov     rmRegs.RegECX,MVDD_COPY_PTES_FOR_LINEAR_RANGE
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetGdiChannelMask
;
; Purpose:      This function determines which bit mask position our
;               DMA pusher holds in a chip register so we can tell
;               whether ours and other DMA contexts are active.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      HWBOARDCX.dwGdiPushBufferMask set correctly
;               HWBOARDCX.dwGdiPushBufferIndex set correctly
;               HWBOARDCX.dwGdiDriverChannelID set correctly
;
; Preserve:     ds,esi,edi
;==============================================================================
NV4_PFIFO_CACHE1_PUSH1          EQU     00003204H
DECPROC NV_GetGdiChannelMask, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   edi
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        mov     ecx,-1
        mov     ds:[esi].HWBOARDCX.dwGdiPushBufferIndex,1
        mov     ds:[esi].HWBOARDCX.dwGdiPushBufferMask,ecx
@@:
        ; This is what we think the PushBufferIndex is
        mov     ds:[esi].HWBOARDCX.dwGdiPushBufferIndex,ecx

        ; Trigger off some DMA activity.
        ; The easy thing is to write NOPS in the push buffer!
        CHECKDMASPACE ds,esi,edi,04H
        sub     eax,eax
        mov     ds:[edi + 00H],eax
        add     edi,4
        SETPUTPTR ds,esi,edi

        ; Read the ID of the current active DMA channel
        sub     ebx,ebx
        mov     eax,ds:[esi].HWBOARDCX.pChipRegs
        mov     ecx,ds:[eax + NV4_PFIFO_CACHE1_PUSH1]
        and     ecx,0FH

        ; If not what we thought, go back and test again.
        cmp     ecx,ds:[esi].HWBOARDCX.dwGdiPushBufferIndex
        jne     @B

DMAChannelQueryDone:
        mov     eax,1
        shl     eax,cl
        not     eax
        mov     ds:[esi].HWBOARDCX.dwGdiPushBufferMask,eax
        mov     ds:[esi].HWBOARDCX.dwGdiDriverChannelID,ecx

        ; Set up a ptr to the location of the current channel ID
        ; This ptr is used by all the clients with DMA pushers.
        lea     edx,[esi].HWBOARDCX.dwCurrentChannelID
        mov     ds:[esi].HWBOARDCX.pCurrentChannelID,edx

        POPR    edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocRequiredMemory
;
; Purpose:      This routine allocates memory for the IndexedImageLUT
;               and enough for cursors for all heads on the board.
;               The addresses are saved off so that other objects
;               can ask for them later.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       Failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocRequiredMemory, PASCAL, FRAME, NEAR
LOCALD  dwPitchInBytes
LOCALD  dwCRTCCount
OPENPROC
        PUSHR   edi

        ; The Indexed Image Lut needs to be 1K aligned on a 1K boundary
        ; so we have to alloc at least 2K - 1 to be certain of alignment.
        mov     eax,800H
        push    esi
        pushd   NVIOCONTROL_HEAP_ALLOC_SIZE
        pushd   GDI2D_BOARD_OWNER
        pushd   NVIOCONTROL_TYPE_CURSOR
        push    eax
        call    NV_OffscreenHeapAlloc
        cmp     eax,-1
        je      FailIt
        mov     ds:[esi].HWBOARDCX.dwIndexedLUTOffset,eax

        ; Set the cursor cache size
        mov     ds:[esi].HWBOARDCX.dwCursorCacheSize,24

        ; Now allocate a cursor cache for each/all heads.
        ; We are going to assume the largest cursor at 32x32 with
        ; alpha which is a 32bpp cursor. This takes 4K bytes. We
        ; must also align to a 4K boundary which means an extra 4K.
        mov     eax,ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard
        imul    eax,ds:[esi].HWBOARDCX.dwCursorCacheSize
        inc     eax
        imul    eax,1000H
        push    esi
        pushd   NVIOCONTROL_HEAP_ALLOC_SIZE
        pushd   GDI2D_BOARD_OWNER
        pushd   NVIOCONTROL_TYPE_CURSOR
        push    eax
        call    NV_OffscreenHeapAlloc
        cmp     eax,-1
        je      FailIt

        ; Align to a 4K boundary
        add     eax,0FFFH
        and     eax,NOT 0FFFH

        lea     edi,[esi].HWBOARDCX.adwCursorCacheVramOffset
        sub     ecx,ecx
        mov     edx,ds:[esi].HWBOARDCX.dwCursorCacheSize
        imul    edx,1000H
@@:     mov     ds:[edi + ecx * 4],eax
        add     eax,edx
        inc     ecx
        cmp     ecx,ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard
        jc      @B

        mov     eax,1
        jmp     DoneIt

FailIt:
        sub     eax,eax
DoneIt:
        POPR    edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetCursorCacheInfo
;
; Purpose:      This routine returns informatin about the cursor
;               cache for the given head.
;
; Arguments:    ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwPhysicalCRTCIndex     index of CRTC to get the cursor
;                                       cache info for
;
; Returns:      eax     offset of cursor cache in video mnemory
;               ebx     number of cache slots
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetCursorCacheInfo, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
PARMD   dwPhysicalCRTCIndex
OPENPROC
        mov     edx,pHWBoardCXOffset
        mov     ebx,ds:[edx].HWBOARDCX.dwCursorCacheSize
        mov     ecx,dwPhysicalCRTCIndex
        mov     eax,ds:[edx].HWBOARDCX.adwCursorCacheVramOffset[ecx * 4]
CLOSEPROC


;==============================================================================
;
; Function:     NV_BoardSetDXSharedData
;
; Purpose:      This routine sets the shared DX data structure fields
;               that have board specific information.
;
; Arguments:    ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               pDXShare                DISPDRVDIRECTXCOMMON ptr
;
; Returns:      eax     offset of cursor cache in video mnemory
;               ebx     number of cache slots
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_BoardSetDXSharedData, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   pDXShare
OPENPROC
        mov     ebx,pHWBoardCXOffset
        mov     edx,pDXShare
        mov     eax,ds:[ebx].HWBOARDCX.dwDeviceNum
        mov     ds:[edx].DISPDRVDIRECTXCOMMON.dwNVDeviceID,eax
        mov     eax,ds:[ebx].HWBOARDCX.dwDeviceVersion
        mov     ds:[edx].DISPDRVDIRECTXCOMMON.dwDeviceVersion,eax
        mov     eax,ds:[ebx].HWBOARDCX.dwRootHandle
        mov     ds:[edx].DISPDRVDIRECTXCOMMON.dwRootHandle,eax
        mov     eax,ds:[ebx].HWBOARDCX.pChipRegs
        mov     ds:[edx].DISPDRVDIRECTXCOMMON.dwDeviceLinearBase,eax
CLOSEPROC

;==============================================================================
;
; Function:     NV_SetInterlacedStereoMode
;
; Purpose:      This routine sets the interlaced stereo mode if needed
;               and lets resman know about it. If stereo type is interlaced 
;               then stereo buffers have to be adjacent in video memory. So 
;               resman must know about it in order to adjust the pitch
;               to accomodate the right eye.
;
; Arguments:    ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwInterlacedMode        1/0
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_SetInterlacedStereoMode, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwInterlacedMode
LOCALV  StereoParams,NV_CFG_STEREO_PARAMS
OPENPROC
        PUSHR   ds,esi,edi
        
        mov		eax,dwInterlacedMode
        lea     edi,StereoParams
        mov     esi,pHWBoardCXOffset
        mov     ss:[edi].NV_CFG_STEREO_PARAMS.Flags, STEREOCFG_EXIT_ADJACENT 
		mov     ds:[esi].HWBOARDCX.dwStereoConnectionType, eax
        test	eax, eax
        jz      @F
        mov     ss:[edi].NV_CFG_STEREO_PARAMS.Flags, STEREOCFG_ENTER_ADJACENT 
@@:
        pushd   NV_CFG_STEREO_CONFIG    
        mov     ax,ss
        push    ax
        lea     ax,StereoParams
        push    ax
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigSet,es,ax

        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

