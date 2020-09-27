;/*
;-----------------------------Module-Header-----------------------------;
; Module Name:  NVRMARCH.INC
;
;  Constants for interface to the Resource Manager
;
; Created: 6/12/98
; Author:  ARG
;
; Copyright (C) 1993,1998 NVidia Corporation. All Rights Reserved.
;
; History:
;-----------------------------------------------------------------------;

ifndef  NVRMARCH_INCLUDED
; This seems to break the stupid assembler in strange and mysterious
; ways.  It appears that this file is re-read on the second pass, or
; at least is reevaluated, which produces "phase errors" galore because
; the condition is now different.  This is not rocket science.
;NVRMARCH_INCLUDED       equ     1

comment ~  */

#ifndef NVRMARCH_INCLUDED
#define NVRMARCH_INCLUDED       1

/* ~


ORD_WEP                                 equ      1
ORD_NvSysDispatch                       equ      3
ORD_NvSysNotifyCallback                 equ      4
ORD_NvSysIpcCallback                    equ      5
ORD_NvSysClientExit                     equ      6
ORD_NVSysSetCursor                      equ      7
ORD_NvSysMoveCursor                     equ      8
ORD_NvSysGetScreenInfo                  equ      9
ORD_NvSysOpen                           equ      10
ORD_NvSysClose                          equ      11
ORD_NvSysGetRamInfo                     equ      12
ORD_NvSysSetCanvasRect                  equ      13
ORD_NvSysSetCanvasClip                  equ      14
ORD_NvSysInstallCallback                equ      15
ORD_NvSysAllocPages                     equ      16
ORD_NvSysFreePages                      equ      17
ORD_NvInstallCallback                   equ      22
ORD_NvSysGetBufferInfo                  equ      23
ORD_NvSysSetCanvas                      equ      24
ORD_NvConfigGet                         equ      25
ORD_NvConfigSet                         equ      26
ORD_NvConfigUpdate                      equ      27
ORD_NvConfigVersion                     equ      28
ORD_NvConfigVga                         equ      29
ORD_NvSysSetCanvasRegion                equ      30
ORD_NvSysSetCursorImage                 equ      35
ORD_NvSysMoveCursorMulti                equ      36
ORD_NvSysSetCursorImageMulti            equ      37
ORD_NvSysUpdateImageOffsetFormat        equ      38
ORD_DllEntryPoint                       equ      40
ORD_THK_ThunkData16                     equ      41
ORD_NvSysGetHardwarePointers            equ      50
ORD_NvDeviceBaseGet                     equ      51
ORD_NvDeviceLimitGet                    equ      52
ORD_NvDeviceSelectorGet                 equ      53
ORD_NvSysGetCurrentBufferFlatPointer    equ      54
ORD_NvSysSetColorCursor                 equ      70
ORD_NvSysShowHideCursor                 equ      71
ORD_NvSysGetCursorEmulatePosPointers    equ      72
ORD_NvSysFlushFifo                      equ      80
ORD_NvSysDisplayModeHook                equ      90
ORD_NvSysLockPages                      equ      100
ORD_NvSysUnlockPages                    equ      101
ORD_NvSysDriverConnect                  equ      102
ORD_NvSysDriverDisConnect               equ      103
ORD_NvSysClearScreen                    equ      200
ORD_NvSysRegisterDisplayDriver          equ      201
ORD_NvIoControl                         equ      299
ORD_Nv01Free                            equ      300
ORD_Nv01AllocRoot                       equ      301
ORD_Nv01AllocMemory                     equ      302
ORD_Nv01AllocContextDma                 equ      303
ORD_Nv01AllocChannelPio                 equ      304
ORD_Nv01AllocObject                     equ      305
ORD_Nv01AllocDevice                     equ      306
ORD_Nv03AllocChannelDma                 equ      307
ORD_Nv03DmaFlowControl                  equ      308
ORD_Nv01Interrupt                       equ      309
ORD_Nv01AllocEvent                      equ      310
ORD_NvI2CAccess                         equ      311
ORD_Nv04ConfigGetEx                     equ      312
ORD_Nv04ConfigSetEx                     equ      313
ORD_Nv04Alloc                           equ      314

; GDI and Video channel HANDLE definitions

NV_WIN_ROOT                                     equ             00002001h
NV_WIN_DEVICE                                   equ             00002002h
NV_WIN_CHANNEL                                  equ             00002003h

;
; GDI Context Dma Handles
;

NV_WIN_CONTEXT_DMA_TO_SYS_MEMORY                EQU             00001000h
NV_WIN_WB_CONTEXT_DMA_TO_VIDEO_MEMORY           EQU             00001002h
NV_WIN_WB_CONTEXT_DMA_TO_SYS_MEMORY             EQU             00001003h
NV_WIN_NOTIFIER_CONTEXT_DMA_TO_MEMORY           EQU             00001005h
NV_WIN_PUSHER_NOTIFIER_CONTEXT_DMA_TO_MEMORY    EQU             00001007h
NV_WIN_CONTEXT_DMA_IN_MEMORY                    EQU             00001010h
NV_WIN_CONTEXT_DMA_FROM_VIDEO_MEMORY            EQU             00001012h
NV_WIN_CONTEXT_DMA_FROM_MEMORY                  EQU             00001015h
NV_WIN_WB_CONTEXT_DMA_FROM_SYS_MEMORY           EQU             00001017h
NV_WIN_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT           EQU             00001020h
NV_WIN_WB_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT        EQU             00001022h

NV01_NULL_OBJECT                                equ             0

comment ~
*/

#define ORD_WEP                                  1
#define ORD_NvSysDispatch                        3
#define ORD_NvSysNotifyCallback                  4
#define ORD_NvSysIpcCallback                     5
#define ORD_NvSysClientExit                      6
#define ORD_NVSysSetCursor                       7
#define ORD_NvSysMoveCursor                      8
#define ORD_NvSysGetScreenInfo                   9
#define ORD_NvSysOpen                            10
#define ORD_NvSysClose                           11
#define ORD_NvSysGetRamInfo                      12
#define ORD_NvSysSetCanvasRect                   13
#define ORD_NvSysSetCanvasClip                   14
#define ORD_NvSysInstallCallback                 15
#define ORD_NvSysAllocPages                      16
#define ORD_NvSysFreePages                       17
#define ORD_NvInstallCallback                    22
#define ORD_NvSysGetBufferInfo                   23
#define ORD_NvSysSetCanvas                       24
#define ORD_NvConfigGet                          25
#define ORD_NvConfigSet                          26
#define ORD_NvConfigUpdate                       27
#define ORD_NvConfigVersion                      28
#define ORD_NvConfigVga                          29
#define ORD_NvSysSetCanvasRegion                 30
#define ORD_NvSysSetCursorImage                  35
#define ORD_DllEntryPoint                        40
#define ORD_THK_ThunkData16                      41
#define ORD_NvSysGetHardwarePointers             50
#define ORD_NvDeviceBaseGet                      51
#define ORD_NvDeviceLimitGet                     52
#define ORD_NvDeviceSelectorGet                  53
#define ORD_NvSysGetCurrentBufferFlatPointe      54
#define ORD_NvSysSetColorCursor                  70
#define ORD_NvSysShowHideCursor                  71
#define ORD_NvSysGetCursorEmulatePosPointer      72
#define ORD_NvSysFlushFifo                       80
#define ORD_NvSysDisplayModeHook                 90
#define ORD_NvSysLockPages                       100
#define ORD_NvSysUnlockPages                     101
#define ORD_NvSysDriverConnect                   102
#define ORD_NvSysDriverDisConnect                103
#define ORD_NvSysClearScreen                     200
#define ORD_NvIoControl                          299
#define ORD_Nv01Free                             300
#define ORD_Nv01AllocRoot                        301
#define ORD_Nv01AllocMemory                      302
#define ORD_Nv01AllocContextDma                  303
#define ORD_Nv01AllocChannelPio                  304
#define ORD_Nv01AllocObject                      305
#define ORD_Nv01AllocDevice                      306
#define ORD_Nv03AllocChannelDma                  307
#define ORD_Nv03DmaFlowControl                   308
#define ORD_Nv01Interrupt                        309
#define ORD_Nv01AllocEvent                       310
#define ORD_NvI2CAccess                          311


// GDI and Video channel HANDLE definitions

#define NV_WIN_ROOT                              0x00002001
#define NV_WIN_DEVICE                            0x00002002
#define NV_WIN_CHANNEL                           0x00002003

/* ~


;  funct NV01_ALLOC_ROOT parameters
ALLOC_ROOT_PARAM    STRUC           ;                   */ typedef struct {                 /*
    ALLOC_ROOT_hObjectNew   dd  ?   ; client handle ret */ unsigned long    hObjectNew;     /*
    ALLOC_ROOT_hClass       dd  ?   ;                   */ unsigned long    hClass;         /*
    ALLOC_ROOT_status       dd  ?   ;                   */ unsigned long    status;         /*
ALLOC_ROOT_PARAM    ENDS            ;                   */ } Alloc_Root_Param;              /*

; Neat-o! MASM 5.20 is only good for 31 characters on identifiers!
; Equates definitions must be unique before this point:
;                              |

ALLOC_ROOT_STATUS_SUCCESS                       equ     00000000h
ALLOC_ROOT_STATUS_ERROR_OPERATING_SYSTEM        equ     00000001h
ALLOC_ROOT_STATUS_ERROR_BAD_PARENT_OBJECT       equ     00000002h
ALLOC_ROOT_STATUS_ERROR_BAD_NEW_OBJECT          equ     00000003h
ALLOC_ROOT_STATUS_ERROR_BAD_CLASS               equ     00000004h
ALLOC_ROOT_STATUS_ERROR_INSUFFICIENT_RESOURCES  equ     00000005h

ALLOC_ROOT_CLASS_ROOT                           equ     00000000h

Nv01AllocRoot_MAC macro buffer,hClass
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hClass
        mov     [bx].ALLOC_ROOT_hClass,edx
        mov     [bx].ALLOC_ROOT_status,ALLOC_ROOT_STATUS_SUCCESS
        cCall   Nv01AllocRoot
        pop     edx
endm

comment ~  */

#define ALLOC_ROOT_STATUS_SUCCESS                       0x00000000
#define ALLOC_ROOT_STATUS_ERROR_OPERATING_SYSTEM        0x00000001
#define ALLOC_ROOT_STATUS_ERROR_BAD_PARENT_OBJECT       0x00000002
#define ALLOC_ROOT_STATUS_ERROR_BAD_NEW_OBJECT          0x00000003
#define ALLOC_ROOT_STATUS_ERROR_BAD_CLASS               0x00000004
#define ALLOC_ROOT_STATUS_ERROR_INSUFFICIENT_RESOURCES  0x00000005

#define ALLOC_ROOT_CLASS_ROOT                           0x00000000

/* ~


NVOS21_STATUS_SUCCESS                           equ     00000000
NVOS21_STATUS_ERROR_OPERATING_SYSTEM            equ     00000001
NVOS21_STATUS_ERROR_BAD_ROOT                    equ     00000002
NVOS21_STATUS_ERROR_BAD_OBJECT_PARENT           equ     00000003
NVOS21_STATUS_ERROR_BAD_OBJECT_NEW              equ     00000004
NVOS21_STATUS_ERROR_BAD_CLASS                   equ     00000005
NVOS21_STATUS_ERROR_INSUFFICIENT_RESOURCES      equ     00000006

ALLOC_PARAM      STRUC
  ALLOC_hRoot                   dd      ?
  ALLOC_hObjectParent           dd      ?
  ALLOC_hObjectNew              dd      ?
  ALLOC_hClass                  dd      ?
  ALLOC_pAllocParms             dd      ?
  ALLOC_Null                    dd      0       
  ALLOC_status                  dd      ?
ALLOC_PARAM      ENDS

ALLOC_DEV_PARAM    STRUC            ;                   */ typedef struct {                 /*
    ALLOC_DEV_hObjectParent dd  ?   ; client handle     */ unsigned long    hObjectParent;  /*
    ALLOC_DEV_hObjectNew    dd  ?   ;                   */ unsigned long    hObjectNew;     /*
    ALLOC_DEV_hClass        dd  ?   ;                   */ unsigned long    hClass;         /*
    ALLOC_DEV_szName        dd  ?   ;                   */ unsigned long    szName;         /*
    ALLOC_DEV_status        dd  ?   ;                   */ unsigned long    status;         /*
ALLOC_DEV_PARAM    ENDS             ;                   */ } Alloc_Dev_Param;               /*

ALLOC_DEV_STATUS_SUCCESS                        equ     00000000h
ALLOC_DEV_STATUS_ERR_OPERATING_SYSTEM           equ     00000001h
ALLOC_DEV_STATUS_ERR_BAD_PARENT_OBJECT          equ     00000002h
ALLOC_DEV_STATUS_ERR_BAD_NEW_OBJECT             equ     00000003h
ALLOC_DEV_STATUS_ERR_BAD_CLASS                  equ     00000004h
ALLOC_DEV_STATUS_ERR_INSUFFICIENT_RESOURCES     equ     00000005h

Nv01AllocDevice_MAC macro buffer,hObjectParent,hObjectNew,hClass,szName
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hObjectParent
        mov     [bx].ALLOC_DEV_hObjectParent, edx
        mov     edx, hObjectNew
        mov     [bx].ALLOC_DEV_hObjectNew, edx
        mov     edx, hClass
        mov     [bx].ALLOC_DEV_hClass, edx

        lea     ax, szName              ; Flatten the device name
        mov     dx, ds

        DPMI_Flatten

        shl     edx, 16
        mov     dx, ax

        lea     bx, buffer
        mov     [bx].ALLOC_DEV_szName, edx
        mov     [bx].ALLOC_DEV_status,ALLOC_DEV_STATUS_SUCCESS
        cCall   Nv01AllocDevice
        pop     edx
endm

; THE C-EQUIVALENTS OF THESE DEFINITIONS ARE FOUND IN NV32.H
; KEEP THEM IN SYNC!
NV01_DEVICE_0                                   equ     00000080h
NV01_DEVICE_1                                   equ     00000081h
NV01_DEVICE_2                                   equ     00000082h
NV01_DEVICE_3                                   equ     00000083h
NV01_DEVICE_4                                   equ     00000084h
NV01_DEVICE_5                                   equ     00000085h
NV01_DEVICE_6                                   equ     00000086h
NV01_DEVICE_7                                   equ     00000087h
NV01_CONTEXT_DMA                                equ     00000002h
NV01_CONTEXT_DMA_FROM_MEMORY                    equ     00000002h
NV01_CONTEXT_DMA_TO_MEMORY                      equ     00000003h
NV01_CONTEXT_DMA_IN_MEMORY                      equ     0000003Dh
NV01_CONTEXT_ERROR_TO_MEMORY                    equ     0000003Eh
NV03_CHANNEL_PIO                                equ     0000006Ah
NV03_CHANNEL_DMA                                equ     0000006Bh
NV04_CHANNEL_DMA                                equ     0000006Ch
NV01_MEMORY_SYSTEM                              equ     0000003Eh
NV01_MEMORY_LOCAL_PRIVILEGED                    equ     0000003Fh
NV01_MEMORY_LOCAL_USER                          equ     00000040h

comment ~  */

#define ALLOC_DEV_STATUS_SUCCESS                        0x00000000
#define ALLOC_DEV_STATUS_ERR_OPERATING_SYSTEM           0x00000001
#define ALLOC_DEV_STATUS_ERR_BAD_OBJECT_PARENT          0x00000002
#define ALLOC_DEV_STATUS_ERR_BAD_OBJECT_NEW             0x00000003
#define ALLOC_DEV_STATUS_ERR_BAD_CLASS                  0x00000004
#define ALLOC_DEV_STATUS_ERR_INSUFFICIENT_RESOURCES     0x00000005

/* ~

ALLOC_MEMORY_PARAM     STRUC           ;               */ typedef struct {                 /*
    ALLOC_MEMORY_hObjectClient dd  ?   ; client handle */ unsigned long    hObjectClient;  /*
    ALLOC_MEMORY_hObjectParent dd  ?   ;               */ unsigned long    hObjectParent;  /*
    ALLOC_MEMORY_hObjectNew    dd  ?   ;               */ unsigned long    hObjectNew;     /*
    ALLOC_MEMORY_hClass        dd  ?   ;               */ unsigned long    hClass;         /*
    ALLOC_MEMORY_flags         dd  ?   ;               */ unsigned long    flags;          /*
    ALLOC_MEMORY_pBase         dd  ?   ;               */ unsigned long    pBase;          /*
                               dd  ?   ;               */ unsigned long    pBase_high;     /*
    ALLOC_MEMORY_pLimit        dd  ?   ;               */ unsigned long    pLimit;         /*
                               dd  ?   ;               */ unsigned long    pLimit_high;    /*
    ALLOC_MEMORY_status        dd  ?   ;               */ unsigned long    status;         /*
ALLOC_MEMORY_PARAM     ENDS            ;               */ } Alloc_Memory_Param;            /*

ALLOC_MEMORY_FLAGS_CONTIGUOUS_PHYSICALITY               equ 00000000h
ALLOC_MEMORY_FLAGS_NONCONTIGUOUS_PHYSICALITY            equ 00000010h

ALLOC_MEMORY_FLAGS_LOCATION_PCI                         equ 00000000h
ALLOC_MEMORY_FLAGS_LOCATION_AGP                         equ 00000100h

ALLOC_MEMORY_FLAGS_UNCACHED                             equ 00000000h
ALLOC_MEMORY_FLAGS_CACHED                               equ 00001000h
ALLOC_MEMORY_FLAGS_WRITE_COMBINE                        equ 00002000h
ALLOC_MEMORY_FLAGS_WRITE_THROUGH                        equ 00003000h
ALLOC_MEMORY_FLAGS_WRITE_PROTECT                        equ 00004000h
ALLOC_MEMORY_FLAGS_WRITE_BACK                           equ 00005000h

ALLOC_MEMORY_STATUS_SUCCESS                             equ 00000000h
ALLOC_MEMORY_STATUS_ERR_OPERATING_SYSTEM                equ 00000001h
ALLOC_MEMORY_STATUS_ERR_BAD_PARENT_OBJECT               equ 00000002h
ALLOC_MEMORY_STATUS_ERR_BAD_NEW_OBJECT                  equ 00000003h
ALLOC_MEMORY_STATUS_ERR_BAD_CLASS                       equ 00000004h
ALLOC_MEMORY_STATUS_ERR_BAD_FLAGS                       equ 00000005h
ALLOC_MEMORY_STATUS_ERR_INSUFFICIENT_RESOURCES          equ 00000006h

Nv01AllocMemory_MAC macro buffer,hObjectClient,hObjectParent,hObjectNew,hClass,flags,limit
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hObjectClient
        mov     [bx].ALLOC_MEMORY_hObjectClient,edx
        mov     edx, hObjectParent
        mov     [bx].ALLOC_MEMORY_hObjectParent,edx
        mov     edx, hObjectNew
        mov     [bx].ALLOC_MEMORY_hObjectNew,edx
        mov     edx, hClass
        mov     [bx].ALLOC_MEMORY_hClass,edx
        mov     edx, flags
        mov     [bx].ALLOC_MEMORY_flags,edx
        mov     dword ptr [bx].ALLOC_MEMORY_pBase, 0
        mov     dword ptr [bx].ALLOC_MEMORY_pBase+4, 0
        mov     edx, limit
        mov     [bx].ALLOC_MEMORY_pLimit,edx
        mov     dword ptr [bx].ALLOC_MEMORY_pLimit+4, 0
        mov     [bx].ALLOC_MEMORY_status,ALLOC_MEMORY_STATUS_SUCCESS
        cCall   Nv01AllocMemory
        pop     edx
endm

comment ~  */

#define ALLOC_MEMORY_FLAGS_CONTIGUOUS_PHYSICALITY               0x00000000
#define ALLOC_MEMORY_FLAGS_NONCONTIGUOUS_PHYSICALITY            0x00000010

#define ALLOC_MEMORY_FLAGS_LOCATION_PCI                         0x00000000
#define ALLOC_MEMORY_FLAGS_LOCATION_AGP                         0x00000100

#define ALLOC_MEMORY_FLAGS_UNCACHED                             0x00000000
#define ALLOC_MEMORY_FLAGS_CACHED                               0x00001000
#define ALLOC_MEMORY_FLAGS_WRITE_COMBINE                        0x00002000
#define ALLOC_MEMORY_FLAGS_WRITE_THROUGH                        0x00003000
#define ALLOC_MEMORY_FLAGS_WRITE_PROTECT                        0x00004000
#define ALLOC_MEMORY_FLAGS_WRITE_BACK                           0x00005000

#define ALLOC_MEMORY_STATUS_SUCCESS                             0x00000000
#define ALLOC_MEMORY_STATUS_ERR_OPERATING_SYSTEM                0x00000001
#define ALLOC_MEMORY_STATUS_ERR_BAD_PARENT_OBJECT               0x00000002
#define ALLOC_MEMORY_STATUS_ERR_BAD_NEW_OBJECT                  0x00000003
#define ALLOC_MEMORY_STATUS_ERR_BAD_CLASS                       0x00000004
#define ALLOC_MEMORY_STATUS_ERR_BAD_FLAGS                       0x00000005
#define ALLOC_MEMORY_STATUS_ERR_INSUFFICIENT_RESOURCES          0x00000006

/* ~

ALLOC_CTX_DMA_PARAM    STRUC            ;               */ typedef struct {                 /*
    ALLOC_CTX_DMA_hObjectParent dd  ?   ; client handle */ unsigned long    hObjectParent;  /*
    ALLOC_CTX_DMA_hObjectNew    dd  ?   ;               */ unsigned long    hObjectNew;     /*
    ALLOC_CTX_DMA_hClass        dd  ?   ;               */ unsigned long    hClass;         /*
    ALLOC_CTX_DMA_flags         dd  ?   ;               */ unsigned long    flags;          /*
    ALLOC_CTX_DMA_pBase         dd  ?   ;               */ unsigned long    pBase;          /*
                                dd  ?   ;               */ unsigned long    pBase_high;     /*
    ALLOC_CTX_DMA_limit         dd  ?   ;               */ unsigned long    limit;          /*
                                dd  ?   ;               */ unsigned long    limit_high;     /*
    ALLOC_CTX_DMA_status        dd  ?   ;               */ unsigned long    status;         /*
ALLOC_CTX_DMA_PARAM    ENDS             ;               */ } Alloc_Ctx_DMA_Param;           /*

ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR                        equ 00000000h
ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY                    equ 00000001h
ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY                   equ 00000002h

ALLOC_CTX_DMA_FLAGS_CONTIGUOUS_PHYSICALITY              equ 00000000h
ALLOC_CTX_DMA_FLAGS_NONCONTIGUOUS_PHYSICALITY           equ 00000010h

ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS                       equ 00000000h
ALLOC_CTX_DMA_FLAGS_LOCKED_IN_TRANSIT                   equ 00000100h

ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED                  equ 00000000h
ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED                    equ 00001000h

ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED                       equ ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS + ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED

ALLOC_CTX_DMA_STATUS_SUCCESS                            equ 00000000h
ALLOC_CTX_DMA_STATUS_ERR_OPERATING_SYSTEM               equ 00000001h
ALLOC_CTX_DMA_STATUS_ERR_BAD_PARENT_OBJECT              equ 00000002h
ALLOC_CTX_DMA_STATUS_ERR_BAD_NEW_OBJECT                 equ 00000003h
ALLOC_CTX_DMA_STATUS_ERR_BAD_CLASS                      equ 00000004h
ALLOC_CTX_DMA_STATUS_ERR_BAD_FLAGS                      equ 00000005h
ALLOC_CTX_DMA_STATUS_ERR_BAD_BASE                       equ 00000006h
ALLOC_CTX_DMA_STATUS_ERR_BAD_LIMIT                      equ 00000007h
ALLOC_CTX_DMA_STATUS_ERR_PROTECTION_FAULT               equ 00000008h
ALLOC_CTX_DMA_STATUS_ERR_MULTIPLE_MEMORY_TYPES          equ 00000009h
ALLOC_CTX_DMA_STATUS_ERR_INSUFFICIENT_RESOURCES         equ 0000000ah

Nv01AllocContextDma_MAC macro buffer,hObjectParent,hObjectNew,hClass,flags,sel,pBase,limit
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hObjectParent
        mov     [bx].ALLOC_CTX_DMA_hObjectParent,edx
        mov     edx, hObjectNew
        mov     [bx].ALLOC_CTX_DMA_hObjectNew,edx
        mov     edx, hClass
        mov     [bx].ALLOC_CTX_DMA_hClass,edx
        mov     edx, flags
        mov     [bx].ALLOC_CTX_DMA_flags,edx
        mov     edx, pBase
        mov     [bx].ALLOC_CTX_DMA_pBase,edx
        mov     dx, sel
        mov     word ptr [bx].ALLOC_CTX_DMA_pBase+4, dx
        mov     word ptr [bx].ALLOC_CTX_DMA_pBase+6, 0
        mov     edx, limit
        mov     [bx].ALLOC_CTX_DMA_limit,edx
        mov     dword ptr [bx].ALLOC_CTX_DMA_limit+4, 0
        mov     [bx].ALLOC_CTX_DMA_status,ALLOC_CTX_DMA_STATUS_SUCCESS
        cCall   Nv01AllocContextDma
        pop     edx
endm

comment ~  */

#define ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR                    0x00000000
#define ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY                0x00000001
#define ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY               0x00000002

#define ALLOC_CTX_DMA_FLAGS_CONTIGUOUS_PHYSICALITY          0x00000000
#define ALLOC_CTX_DMA_FLAGS_NONCONTIGUOUS_PHYSICALITY       0x00000010

#define ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS                   0x00000000
#define ALLOC_CTX_DMA_FLAGS_LOCKED_IN_TRANSIT               0x00000100

#define ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED              0x00000000
#define ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED                0x00001000

#define ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED                   ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS + ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED

#define ALLOC_CTX_DMA_STATUS_SUCCESS                        0x00000000
#define ALLOC_CTX_DMA_STATUS_ERR_OPERATING_SYSTEM           0x00000001
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_PARENT_OBJECT          0x00000002
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_NEW_OBJECT             0x00000003
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_CLASS                  0x00000004
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_FLAGS                  0x00000005
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_BASE                   0x00000006
#define ALLOC_CTX_DMA_STATUS_ERR_BAD_LIMIT                  0x00000007
#define ALLOC_CTX_DMA_STATUS_ERR_PROTECTION_FAULT           0x00000008
#define ALLOC_CTX_DMA_STATUS_ERR_MULTIPLE_MEMORY_TYPES      0x00000009
#define ALLOC_CTX_DMA_STATUS_ERR_INSUFFICIENT_RESOURCES     0x0000000a

/* ~

ALLOC_CHL_PIO_PARAM    STRUC            ;               */ typedef struct {                 /*
    ALLOC_CHL_PIO_hClient       dd  ?   ;               */ unsigned long    hClient;        /*
    ALLOC_CHL_PIO_hObjectParent dd  ?   ;               */ unsigned long    hObjectParent;  /*
    ALLOC_CHL_PIO_hObjectNew    dd  ?   ;               */ unsigned long    hObjectNew;     /*
    ALLOC_CHL_PIO_hClass        dd  ?   ;               */ unsigned long    hClass;         /*
    ALLOC_CHL_PIO_hObjectError  dd  ?   ;               */ unsigned long    hObjectError;   /*
    ALLOC_CHL_PIO_pChannel      dd  ?   ;               */ unsigned long    pChannel;       /*
                                dd  ?   ;               */ unsigned long    pChannel_high;  /*
    ALLOC_CHL_PIO_flags         dd  ?   ;               */ unsigned long    flags;          /*
    ALLOC_CHL_PIO_status        dd  ?   ;               */ unsigned long    status;         /*
ALLOC_CHL_PIO_PARAM    ENDS             ;               */ } Alloc_Chl_PIO_Param;           /*

ALLOC_CHL_PIO_FLAGS_FIFO_IGNORE_RUNOUT                  equ 00000000h
ALLOC_CHL_PIO_FLAGS_FIFO_ERROR_RUNOUT                   equ 00000001h

ALLOC_CHL_PIO_STATUS_SUCCESS                            equ 00000000h
ALLOC_CHL_PIO_STATUS_ERR_OPERATING_SYSTEM               equ 00000001h
ALLOC_CHL_PIO_STATUS_ERR_BAD_PARENT_OBJECT              equ 00000002h
ALLOC_CHL_PIO_STATUS_ERR_BAD_NEW_OBJECT                 equ 00000003h
ALLOC_CHL_PIO_STATUS_ERR_BAD_CLASS                      equ 00000004h
ALLOC_CHL_PIO_STATUS_ERR_BAD_OBJECT_ERROR               equ 00000005h
ALLOC_CHL_PIO_STATUS_ERR_BAD_FLAGS                      equ 00000006h
ALLOC_CHL_PIO_STATUS_ERR_INSUFFICIENT_RESOURCES         equ 00000007h

Nv01AllocChannelPio_MAC macro buffer,hClient,hObjectParent,hObjectNew,hClass,hObjectError,flags
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hClient
        mov     [bx].ALLOC_CHL_PIO_hClient, edx
        mov     edx, hObjectParent
        mov     [bx].ALLOC_CHL_PIO_hObjectParent, edx
        mov     edx, hObjectNew
        mov     [bx].ALLOC_CHL_PIO_hObjectNew, edx
        mov     edx, hClass
        mov     [bx].ALLOC_CHL_PIO_hClass, edx
        mov     edx, hObjectError
        mov     [bx].ALLOC_CHL_PIO_hObjectError, edx
        mov     dword ptr [bx].ALLOC_CHL_PIO_pChannel, 0
        mov     dword ptr [bx].ALLOC_CHL_PIO_pChannel+4, 0
        mov     edx, flags
        mov     [bx].ALLOC_CHL_PIO_flags,edx
        mov     [bx].ALLOC_CHL_PIO_status,ALLOC_CHL_PIO_STATUS_SUCCESS
        cCall   Nv01AllocChannelPio
        pop     edx
endm

comment ~  */

#define ALLOC_CHL_PIO_FLAGS_FIFO_IGNORE_RUNOUT              0x00000000
#define ALLOC_CHL_PIO_FLAGS_FIFO_ERROR_RUNOUT               0x00000001

#define ALLOC_CHL_PIO_STATUS_SUCCESS                        0x00000000
#define ALLOC_CHL_PIO_STATUS_ERR_OPERATING_SYSTEM           0x00000001
#define ALLOC_CHL_PIO_STATUS_ERR_BAD_PARENT_OBJECT          0x00000002
#define ALLOC_CHL_PIO_STATUS_ERR_BAD_NEW_OBJECT             0x00000003
#define ALLOC_CHL_PIO_STATUS_ERR_BAD_CLASS                  0x00000004
#define ALLOC_CHL_PIO_STATUS_ERR_BAD_OBJECT_ERROR           0x00000005
#define ALLOC_CHL_PIO_STATUS_ERR_BAD_FLAGS                  0x00000006
#define ALLOC_CHL_PIO_STATUS_ERR_INSUFFICIENT_RESOURCES     0x00000007

/* ~
;  funct NV03_ALLOC_CHANNEL_DMA parameters
ALLOC_CHL_DMA_PARAM STRUC                           ;               */ typedef struct {                 /*
    ALLOC_CHL_DMA_hClient       dd  ?   ;               */ unsigned long    hClient;        /*
    ALLOC_CHL_DMA_hObjectParent dd  ?   ;               */ unsigned long    hObjectParent;  /*
    ALLOC_CHL_DMA_hObjectNew    dd  ?   ;               */ unsigned long    hObjectNew;     /*
    ALLOC_CHL_DMA_hClass        dd  ?   ;               */ unsigned long    hClass;         /*
    ALLOC_CHL_DMA_hObjectError  dd  ?   ;               */ unsigned long    hObjectError;   /*
    ALLOC_CHL_DMA_hObjectBuffer dd  ?   ;               */ unsigned long    hObjectBuffer;  /*
    ALLOC_CHL_DMA_offset        dd  ?   ;               */ unsigned long    offset;         /*
    ALLOC_CHL_DMA_pControl      dd  ?   ;               */ unsigned long    pControl;       /*
                                dd  ?   ;               */ unsigned long    pControl_high;  /*
    ALLOC_CHL_DMA_status        dd  ?   ;               */ unsigned long    status;         /*
ALLOC_CHL_DMA_PARAM ENDS                                ;               */ } Alloc_Chl_DMA_Param;           /*

ALLOC_CHL_DMA_STATUS_SUCCESS                         equ        00000000h
ALLOC_CHL_DMA_STATUS_ERR_OPERATING_SYSTEM            equ        00000001h
ALLOC_CHL_DMA_STATUS_ERR_BAD_PARENT_OBJECT           equ        00000002h
ALLOC_CHL_DMA_STATUS_ERR_BAD_NEW_OBJECT              equ        00000003h
ALLOC_CHL_DMA_STATUS_ERR_BAD_CLASS                   equ        00000004h
ALLOC_CHL_DMA_STATUS_ERR_BAD_ERROR_OBJECT            equ        00000005h
ALLOC_CHL_DMA_STATUS_ERR_BAD_BUFFRE_OBJECT           equ        00000006h
ALLOC_CHL_DMA_STATUS_ERR_BAD_OFFSET                  equ        00000007h
ALLOC_CHL_DMA_STATUS_ERR_INSUFFICIENT_RESOURCES      equ        00000008h

Nv03AllocChannelDma_MAC macro buffer,hClient,hObjectParent,hObjectNew,hClass,hObjectError,hObjectBuffer,offset
        push    edx
        push    ds
        lea     bx, buffer
        push    bx
        mov     edx, hClient
        mov     [bx].ALLOC_CHL_DMA_hClient, edx
        mov     edx, hObjectParent
        mov     [bx].ALLOC_CHL_DMA_hObjectParent, edx
        mov     edx, hObjectNew
        mov     [bx].ALLOC_CHL_DMA_hObjectNew, edx
        mov     edx, hClass
        mov     [bx].ALLOC_CHL_DMA_hClass, edx
        mov     edx, hObjectError
        mov     [bx].ALLOC_CHL_DMA_hObjectError, edx
        mov     edx, hObjectBuffer
        mov     [bx].ALLOC_CHL_DMA_hObjectBuffer, edx
        mov     edx, offset
        mov     [bx].ALLOC_CHL_DMA_offset, edx
        mov     dword ptr [bx].ALLOC_CHL_DMA_pControl, 0
        mov     dword ptr [bx].ALLOC_CHL_DMA_pControl+4, 0
        mov     [bx].ALLOC_CHL_DMA_status,0
        cCall   Nv03AllocChannelDma
        pop     edx
endm

comment ~  */

#define ALLOC_CHL_DMA_STATUS_SUCCESS                        0x00000000
#define ALLOC_CHL_DMA_STATUS_ERR_OPERATING_SYSTEM           0x00000001
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_OBJECT_PARENT          0x00000002
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_OBJECT_NEW             0x00000003
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_CLASS                  0x00000004
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_OBJECT_ERROR           0x00000005
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_OBJECT_BUFFER          0x00000006
#define ALLOC_CHL_DMA_STATUS_ERR_BAD_OFFSET                 0x00000007
#define ALLOC_CHL_DMA_STATUS_ERR_INSUFFICIENT_RESOURCES     0x00000008

/* ~

NV01FREE_PARAM    STRUC                 ;               */ typedef struct {                 /*
    NV01FREE_hClient            dd  ?   ;               */ unsigned long    hClient;        /*
    NV01FREE_hObjectParent      dd  ?   ;               */ unsigned long    hObjectParent;  /*
    NV01FREE_hObjectOld         dd  ?   ;               */ unsigned long    hObjectOld;     /*
    NV01FREE_status             dd  ?   ;               */ unsigned long    status;         /*
NV01FREE_PARAM    ENDS                  ;               */ } NV01Free_Param;                /*

NV01FREE_STATUS_SUCCESS                                 equ 00000000h
NV01FREE_STATUS_ERR_OPERATING_SYSTEM                    equ 00000001h
NV01FREE_STATUS_ERR_BAD_PARENT_OBJECT                   equ 00000002h
NV01FREE_STATUS_ERR_BAD_OLD_OBJECT                      equ 00000003h
NV01FREE_STATUS_ERR_OBJECT_IN_USE                       equ 00000004h
NV01FREE_STATUS_ERR_OBJECT_HAS_CHILDERN                 equ 00000005h

Nv01Free_MAC macro buffer,hClient,hObjectParent,hObjectOld
          push  edx
          push  ds
          lea   bx, buffer
          push  bx
          mov   edx, hClient
          mov   [bx].NV01FREE_hClient, edx
          mov   edx, hObjectParent
          mov   [bx].NV01FREE_hObjectParent, edx
          mov   edx, hObjectOld
          mov   [bx].NV01FREE_hObjectOld, edx
          mov   [bx].NV01FREE_status, NV01FREE_STATUS_SUCCESS
          cCall Nv01Free
          pop   edx
endm

comment ~  */


#define NV01FREE_STATUS_SUCCESS                             0x00000000
#define NV01FREE_STATUS_ERR_OPERATING_SYSTEM                0x00000001
#define NV01FREE_STATUS_ERR_BAD_OBJECT_PARENT               0x00000002
#define NV01FREE_STATUS_ERR_BAD_OBJECT_OLD                  0x00000003
#define NV01FREE_STATUS_ERR_OBJECT_IN_USE                   0x00000004
#define NV01FREE_STATUS_ERR_OBJECT_HAS_CHILDERN             0x00000005

/* ~

ALLOC_OBJECT_PARAM        STRUC            ;               */ typedef struct {                 /*
    ALLOC_OBJECT_hClient           dd  ?   ;               */ unsigned long    hClient;        /*
    ALLOC_OBJECT_hObjectParent     dd  ?   ;               */ unsigned long    hObjectParent;  /*
    ALLOC_OBJECT_hObjectNew        dd  ?   ;               */ unsigned long    hObjectNew;     /*
    ALLOC_OBJECT_hClass            dd  ?   ;               */ unsigned long    hClass;         /*
    ALLOC_OBJECT_status            dd  ?   ;               */ unsigned long    status;         /*
ALLOC_OBJECT_PARAM        ENDS             ;               */ } AllocObject_Param;        /*

ALLOC_OBJ_STATUS_SUCCESS                                 equ 00000000h
ALLOC_OBJ_STATUS_ERR_OPERATING_SYSTEM                    equ 00000001h
ALLOC_OBJ_STATUS_ERR_BAD_PARENT_OBJECT                   equ 00000002h
ALLOC_OBJ_STATUS_ERR_BAD_NEW_OBJECT                      equ 00000003h
ALLOC_OBJ_STATUS_ERR_BAD_CLASS                           equ 00000004h
ALLOC_OBJ_STATUS_ERR_INSUFFICIENT_RESOURCES              equ 00000005h

Nv01AllocObject_MAC macro buffer,hClient,hObjectParent,hObjectNew,hClass
          push  edx
          push  ds
          lea   bx, buffer
          push  bx
          mov   edx, hClient
          mov   [bx].ALLOC_OBJECT_hClient, edx
          mov   edx, hObjectParent
          mov   [bx].ALLOC_OBJECT_hObjectParent, edx
          mov   edx, hObjectNew
          mov   [bx].ALLOC_OBJECT_hObjectNew, edx
          mov   edx, hClass
          mov   [bx].ALLOC_OBJECT_hClass, edx
          mov   [bx].ALLOC_OBJECT_status, ALLOC_OBJ_STATUS_SUCCESS
          cCall Nv01AllocObject
          pop   edx
endm

comment ~  */


#define ALLOC_OBJ_STATUS_SUCCESS                             0x00000000
#define ALLOC_OBJ_STATUS_ERR_OPERATING_SYSTEM                0x00000001
#define ALLOC_OBJ_STATUS_ERR_BAD_OBJECT_PARENT               0x00000002
#define ALLOC_OBJ_STATUS_ERR_BAD_OBJECT_OLD                  0x00000003
#define ALLOC_OBJ_STATUS_ERR_BAD_CLASS                       0x00000004
#define ALLOC_OBJ_STATUS_ERR_INSUFFICIENT_RESOURCES          0x00000005

/* ~

NVIOCONTROL_PARAM   STRUC               ;               */ typedef struct {                 /*
    NVIOCONTROL_hRoot           dd  ?   ;               */ unsigned long    hRoot;          /*
    NVIOCONTROL_hObjectParent   dd  ?   ;               */ unsigned long    hObjectParent;  /*
    NVIOCONTROL_function        dd  ?   ;               */ unsigned long    function;       /*
    NVIOCONTROL_owner           dd  ?   ;               */ unsigned long    owner;          /*
    NVIOCONTROL_type            dd  ?   ;               */ unsigned long    type;           /*
    NVIOCONTROL_depth           dd  ?   ;               */ unsigned long    depth;          /*
    NVIOCONTROL_width           dd  ?   ;               */ unsigned long    width;          /*
    NVIOCONTROL_height          dd  ?   ;               */ unsigned long    height;         /*
    NVIOCONTROL_pitch           dd  ?   ;               */ unsigned long    pitch;          /*
    NVIOCONTROL_offset          dd  ?   ;               */ unsigned long    offset;         /*
    NVIOCONTROL_size            dd  ?   ;               */ unsigned long    size;           /*
    NVIOCONTROL_address         dd  ?   ;               */ unsigned long    address;        /*
    NVIOCONTROL_limit           dd  ?   ;               */ unsigned long    limit;          /*
    NVIOCONTROL_total           dd  ?   ;               */ unsigned long    total;          /*
    NVIOCONTROL_free            dd  ?   ;               */ unsigned long    free;           /*
    NVIOCONTROL_status          dd  ?   ;               */ unsigned long    status;         /*
NVIOCONTROL_PARAM   ENDS                ;               */ } NVIOControl_Param;             /*

NVIOCONTROL_ARCH_HEAP                                   equ 0000000Bh

NVIOCONTROL_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT               equ 1
NVIOCONTROL_HEAP_ALLOC_SIZE                             equ 2
NVIOCONTROL_HEAP_FREE                                   equ 3
NVIOCONTROL_HEAP_PURGE                                  equ 4
NVIOCONTROL_HEAP_INFO                                   equ 5
NVIOCONTROL_HEAP_ALLOC_TILED_PITCH_HEIGHT               equ 6
NVIOCONTROL_HEAP_DESTROY                                equ 7
NVIOCONTROL_HEAP_INFO_FREE_BLOCKS                       equ 8
NVIOCONTROL_TYPE_IMAGE                                  equ 0
NVIOCONTROL_TYPE_DEPTH                                  equ 1
NVIOCONTROL_TYPE_TEXTURE                                equ 2
NVIOCONTROL_TYPE_VIDEO_OVERLAY                          equ 3
NVIOCONTROL_TYPE_FONT                                   equ 4
NVIOCONTROL_TYPE_CURSOR                                 equ 5
NVIOCONTROL_TYPE_DMA                                    equ 6
NVIOCONTROL_TYPE_INSTANCE                               equ 7
NVIOCONTROL_TYPE_PRIMARY                                equ 8
NVIOCONTROL_STATUS_SUCCESS                              equ 00000000h
NVIOCONTROL_STATUS_ERR_INSUFFICIENT_RESOURCES           equ 00000001h
NVIOCONTROL_STATUS_ERR_FUNCTION_INVALID                 equ 00000002h
NVIOCONTROL_STATUS_ERR_OWNER_INVALID                    equ 00000003h

comment ~  */

#define NVIOCONTROL_ARCH_HEAP                               (0x0000000B)

#define NVIOCONTROL_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT           1
#define NVIOCONTROL_HEAP_ALLOC_SIZE                         2
#define NVIOCONTROL_HEAP_FREE                               3
#define NVIOCONTROL_HEAP_PURGE                              4
#define NVIOCONTROL_HEAP_INFO                               5
#define NVIOCONTROL_HEAP_ALLOC_TILED_PITCH_HEIGHT           6
#define NVIOCONTROL_TYPE_IMAGE                              0
#define NVIOCONTROL_TYPE_DEPTH                              1
#define NVIOCONTROL_TYPE_TEXTURE                            2
#define NVIOCONTROL_TYPE_VIDEO_OVERLAY                      3
#define NVIOCONTROL_TYPE_FONT                               4
#define NVIOCONTROL_TYPE_CURSOR                             5
#define NVIOCONTROL_TYPE_DMA                                6
#define NVIOCONTROL_TYPE_INSTANCE                           7
#define NVIOCONTROL_TYPE_PRIMARY                            8
#define NVIOCONTROL_STATUS_SUCCESS                          (0x00000000)
#define NVIOCONTROL_STATUS_ERR_INSUFFICIENT_RESOURCES       (0x00000001)
#define NVIOCONTROL_STATUS_ERR_FUNCTION_INVALID             (0x00000002)
#define NVIOCONTROL_STATUS_ERR_OWNER_INVALID                (0x00000003)


/* ~

; This must be identical to what is NVRMWIN.H - 
; it would be nice if we could use this definition only.

_def_rmParams   STRUC                   ;       */      typedef struct _def_rmParams {      /*
    RegEAX              dd      ?       ;       */          unsigned long    RegEAX;        /*
    RegEBX              dd      ?       ;       */          unsigned long    RegEBX;        /*
    RegECX              dd      ?       ;       */          unsigned long    RegECX;        /*
    RegEDX              dd      ?       ;       */          unsigned long    RegEDX;        /*
    RegEDI              dd      ?       ;       */          unsigned long    RegEDI;        /*
    RegESI              dd      ?       ;       */          unsigned long    RegESI;        /*
    RegES               dd      ?       ;       */          unsigned long    RegES;         /*
    RegDS               dd      ?       ;       */          unsigned long    RegDS;         /*
    RegCS               dd      ?       ;       */          unsigned long    RegCS;         /*
_def_rmParams   ENDS                    ;       */      } RMPARAMS, *PRMPARAMS;             /*

RMPARAMS        TYPEDEF         _def_rmParams
PRMPARAMS       TYPEDEF         PTR _def_rmParams

comment ~  */

/* ~
comment ~  */

/* ~
comment ~  */

#endif  // NVRMARCH_INCLUDED
/* ~

endif   ; NVRMARCH_INCLUDED

;*/

