        title  "Thunks for DVDLIB"
;++
;
; Copyright (c) Microsoft Corporation. All rights reserved.
;
; Module Name:
;
;    thunks.asm
;
; Abstract:
;
;    This module contains thunks for DVD library used by Xbox Dashboard
;
;--

        .686p

DECLARE_IMPORT  macro Func,N
    ifb <N>
        public  Func
        Func    db 0E9h, 0, 0, 0, 0, 0CCh
    else
        public  Func&@&N
        Func&@&N db 0E9h, 0, 0, 0, 0, 0CCh
    endif
endm

DECLARE_EXPORT  macro Func,N
        public  pfn_&Func
    ifb <N>
        extrn   Func:near
        pfn_&Func dd offset Func
    else
        extrn   Func&@&N:near
        pfn_&Func dd offset Func&@&N
    endif
endm

DECLARE_DVD_API macro Func,N
    ifdef XDCS
        DECLARE_EXPORT Func,N
    else
        DECLARE_IMPORT Func,N
    endif
endm

DECLARE_DASH_API macro Func,N
    ifdef XDCS
        DECLARE_IMPORT Func,N
    else
        DECLARE_EXPORT Func,N
    endif
endm

DVDTHUNK segment dword public 'CODE'

        assume  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        public  _DvdAPIsBegin, _DvdAPIsEnd
        public  _DashAPIsBegin, _DashAPIsEnd

_DvdAPIsBegin   label   near

        DECLARE_DVD_API     _DDP_AbortAsyncRequest,4
        DECLARE_DVD_API     _DDP_AdvanceFrame,4
        DECLARE_DVD_API     _DDP_AdvanceFrameBy,8
        DECLARE_DVD_API     _DDP_AngleChange,8
        DECLARE_DVD_API     _DDP_AudioStreamChange,8
        DECLARE_DVD_API     _DDP_BackwardScan,8
        DECLARE_DVD_API     _DDP_ButtonActivate,4
        DECLARE_DVD_API     _DDP_ButtonSelectAndActivate,8
        DECLARE_DVD_API     _DDP_ButtonSelectAndActivateAt,12
        DECLARE_DVD_API     _DDP_ButtonSelectAt,12
        DECLARE_DVD_API     _DDP_CheckDrive,8
        DECLARE_DVD_API     _DDP_CheckDriveWithPath,8
        DECLARE_DVD_API     _DDP_CheckMenuAvail,8
        DECLARE_DVD_API     _DDP_ClearBreakpoint,8
        DECLARE_DVD_API     _DDP_ClosePlayer,4
        DECLARE_DVD_API     _DDP_Defrost,12
        DECLARE_DVD_API     _DDP_DefrostExt,16
        DECLARE_DVD_API     _DDP_DetachDrive,16
        DECLARE_DVD_API     _DDP_EjectDiskInDrive,4
        DECLARE_DVD_API     _DDP_ExtendedPlay,20
        DECLARE_DVD_API     _DDP_ForwardScan,8
        DECLARE_DVD_API     _DDP_Freeze,12
        DECLARE_DVD_API     _DDP_GetAudioStreamAttributes,12
        DECLARE_DVD_API     _DDP_GetAvailStreams,12
        DECLARE_DVD_API     _DDP_GetAVFileInfo,12
        DECLARE_DVD_API     _DDP_GetCurrentAngle,4
        DECLARE_DVD_API     _DDP_GetCurrentAudioStream,4
        DECLARE_DVD_API     _DDP_GetCurrentButtonState,16
        DECLARE_DVD_API     _DDP_GetCurrentDisplayMode,4
        DECLARE_DVD_API     _DDP_GetCurrentDuration,8
        DECLARE_DVD_API     _DDP_GetCurrentLocation,8
        DECLARE_DVD_API     _DDP_GetCurrentSubPictureStream,4
        DECLARE_DVD_API     _DDP_GetDiskInfoAsync,16
        DECLARE_DVD_API     _DDP_GetDiskRegionSettings,24
        DECLARE_DVD_API     _DDP_GetDiskType,4
        DECLARE_DVD_API     _DDP_GetDriveStatus,12
        DECLARE_DVD_API     _DDP_GetExtendedPlayerState,8
        DECLARE_DVD_API     _DDP_GetForbiddenUserOperations,4
        DECLARE_DVD_API     _DDP_GetFrozenInfo,12
        DECLARE_DVD_API     _DDP_GetNumberOfAngles,8
        DECLARE_DVD_API     _DDP_GetNumberOfPartOfTitle,8
        DECLARE_DVD_API     _DDP_GetNumberOfTitles,4
        DECLARE_DVD_API     _DDP_GetPlayerMode,4
        DECLARE_DVD_API     _DDP_GetRegionSettings,20
        DECLARE_DVD_API     _DDP_GetSubPictureStreamAttributes,12
        DECLARE_DVD_API     _DDP_GetTitleDuration,12
        DECLARE_DVD_API     _DDP_GetVolumeName,8
        DECLARE_DVD_API     _DDP_GoUp,4
        DECLARE_DVD_API     _DDP_InitialLanguageSelect,20
        DECLARE_DVD_API     _DDP_InquireCurrentBitRate,8
        DECLARE_DVD_API     _DDP_InstallEventHandler,16
        DECLARE_DVD_API     _DDP_IsButtonAt,12
        DECLARE_DVD_API     _DDP_IsCurrentSubPictureEnabled,4
        DECLARE_DVD_API     _DDP_LeftButtonSelect,4
        DECLARE_DVD_API     _DDP_LoadDiskInDrive,8
        DECLARE_DVD_API     _DDP_LowerButtonSelect,4
        DECLARE_DVD_API     _DDP_MenuCall,8
        DECLARE_DVD_API     _DDP_MenuLanguageSelect,8
        DECLARE_DVD_API     _DDP_NextPGSearch,4
        DECLARE_DVD_API     _DDP_OpenPlayer,12
        DECLARE_DVD_API     _DDP_OpenPlayerAsync,16
        DECLARE_DVD_API     _DDP_OpenPlayerExtended,20
        DECLARE_DVD_API     _DDP_OpenPlayerWithPath,12
        DECLARE_DVD_API     _DDP_ParentalCountrySelect,8
        DECLARE_DVD_API     _DDP_ParentalLevelSelect,8
        DECLARE_DVD_API     _DDP_PauseOff,4
        DECLARE_DVD_API     _DDP_PauseOn,4
        DECLARE_DVD_API     _DDP_PbcOff,4
        DECLARE_DVD_API     _DDP_PbcOn,4
        DECLARE_DVD_API     _DDP_PlayFile,12
        DECLARE_DVD_API     _DDP_PrevPGSearch,4
        DECLARE_DVD_API     _DDP_PTTPlay,12
        DECLARE_DVD_API     _DDP_PTTPlayForced,12
        DECLARE_DVD_API     _DDP_PTTSearch,8
        DECLARE_DVD_API     _DDP_PTTSearchForced,8
        DECLARE_DVD_API     _DDP_RemoveEventHandler,8
        DECLARE_DVD_API     _DDP_Resume,4
        DECLARE_DVD_API     _DDP_ReversePlayback,4
        DECLARE_DVD_API     _DDP_RightButtonSelect,4
        DECLARE_DVD_API     _DDP_SetAudioStreamSelectionPreferences,8
        DECLARE_DVD_API     _DDP_SetBreakpoint,20
        DECLARE_DVD_API     _DDP_SetBreakpointExt,24
        DECLARE_DVD_API     _DDP_SetDisplayMode,8
        DECLARE_DVD_API     _DDP_SetPlaybackSpeed,8
        DECLARE_DVD_API     _DDP_SetSystemRegion,12
        DECLARE_DVD_API     _DDP_StartPresentation,4
        DECLARE_DVD_API     _DDP_StartPresentationExt,8
        DECLARE_DVD_API     _DDP_StillOff,4
        DECLARE_DVD_API     _DDP_Stop,4
        DECLARE_DVD_API     _DDP_StopScan,4
        DECLARE_DVD_API     _DDP_SubPictureStreamChange,12
        DECLARE_DVD_API     _DDP_TimePlay,12
        DECLARE_DVD_API     _DDP_TimePlayForced,12
        DECLARE_DVD_API     _DDP_TimeSearch,8
        DECLARE_DVD_API     _DDP_TimeSearchForced,8
        DECLARE_DVD_API     _DDP_TitlePlay,8
        DECLARE_DVD_API     _DDP_TitlePlayForced,8
        DECLARE_DVD_API     _DDP_TopPGSearch,4
        DECLARE_DVD_API     _DDP_TrickPlay,4
        DECLARE_DVD_API     _DDP_UpperButtonSelect,4
        DECLARE_DVD_API     _VDR_ActivateUnits,4
        DECLARE_DVD_API     _VDR_AvailUnits,4
        DECLARE_DVD_API     _VDR_CheckRegionCodeValid,8
        DECLARE_DVD_API     _VDR_CloseDriver,4
        DECLARE_DVD_API     _VDR_CloseUnits,4
        DECLARE_DVD_API     _VDR_CompleteMPEGCommand,8
        DECLARE_DVD_API     _VDR_CompleteMPEGData,4
        DECLARE_DVD_API     _VDR_CompleteMPEGDataSplit,8
        DECLARE_DVD_API     _VDR_CompleteMPEGRefillMessage,4
        DECLARE_DVD_API     _VDR_ConfigureUnits,8
        DECLARE_DVD_API     _VDR_CurrentMPEGLocation,4
        DECLARE_DVD_API     _VDR_CurrentMPEGState,4
        DECLARE_DVD_API     _VDR_CurrentMPEGTransferLocation,4
        DECLARE_DVD_API     _VDR_DoAuthenticationCommand,16
        DECLARE_DVD_API     _VDR_DoMPEGCommand,12
        DECLARE_DVD_API     _VDR_EnablePIP,8
        DECLARE_DVD_API     _VDR_GrabFrame,24
        DECLARE_DVD_API     _VDR_InstallMessagePort,8
        DECLARE_DVD_API     _VDR_InstallMPEGWinHooks,20
        DECLARE_DVD_API     _VDR_LockUnits,4
        DECLARE_DVD_API     _VDR_MPEGCommandPending,8
        DECLARE_DVD_API     _VDR_OpenDriver,12
        DECLARE_DVD_API     _VDR_OpenSubUnits,12
        DECLARE_DVD_API     _VDR_OpenUnits,12
        DECLARE_DVD_API     _VDR_PassivateUnits,4
        DECLARE_DVD_API     _VDR_ReconfigureDriver,4
        DECLARE_DVD_API     _VDR_RemoveMessagePort,4
        DECLARE_DVD_API     _VDR_RemoveMPEGWinHooks,4
        DECLARE_DVD_API     _VDR_RestartMPEGDataSplit,8
        DECLARE_DVD_API     _VDR_SendMPEGCommand,16
        DECLARE_DVD_API     _VDR_SendMPEGData,12
        DECLARE_DVD_API     _VDR_SendMPEGDataMultiple,12
        DECLARE_DVD_API     _VDR_SendMPEGDataSplit,20
        DECLARE_DVD_API     _VDR_UnlockUnits,4
        DECLARE_DVD_API     _VDR_UpdatePIP,4


_DvdAPIsEnd     label   near

_DashAPIsBegin  label   near

        DECLARE_DASH_API    _Ac97CreateMediaObject,16
        DECLARE_DASH_API    _CloseHandle,4
        DECLARE_DASH_API    _CreateEventA,16
        DECLARE_DASH_API    _CreateFileA,28
        DECLARE_DASH_API    _CreateMutexA,12
        DECLARE_DASH_API    _CreateSemaphoreA,16
        DECLARE_DASH_API    _CreateThread,24
        DECLARE_DASH_API    _D3DDevice_BlockUntilVerticalBlank,0
        DECLARE_DASH_API    _D3DDevice_CreateImageSurface,16
        DECLARE_DASH_API    _D3DDevice_EnableCC,4
        DECLARE_DASH_API    _D3DDevice_EnableOverlay,4
        DECLARE_DASH_API    _D3DDevice_GetCCStatus,8
        DECLARE_DASH_API    _D3DDevice_GetDisplayFieldStatus,4
        DECLARE_DASH_API    _D3DDevice_GetRasterStatus,4
        DECLARE_DASH_API    _D3DDevice_SendCC,12
        DECLARE_DASH_API    _D3DDevice_UpdateOverlay,20
        DECLARE_DASH_API    _D3DResource_IsBusy,4
        DECLARE_DASH_API    _D3DResource_Release,4
        DECLARE_DASH_API    _D3DSurface_GetDesc,8
        DECLARE_DASH_API    _DeviceIoControl,32
        DECLARE_DASH_API    _GetCurrentThreadId,0
        DECLARE_DASH_API    _GetLastError,0
        DECLARE_DASH_API    _GetProcessHeap,0
        DECLARE_DASH_API    _GetThreadPriority,4
        DECLARE_DASH_API    _GetTickCount,0
        DECLARE_DASH_API    _HeapFree,12
        DECLARE_DASH_API    _OutputDebugStringA,4
        DECLARE_DASH_API    _QueryPerformanceCounter,4
        DECLARE_DASH_API    _QueryPerformanceFrequency,4
        DECLARE_DASH_API    _ReadFile,20
        DECLARE_DASH_API    _ReleaseMutex,4
        DECLARE_DASH_API    _ReleaseSemaphore,12
        DECLARE_DASH_API    _ResetEvent,4
        DECLARE_DASH_API    _ResumeThread,4
        DECLARE_DASH_API    _SetEvent,4
        DECLARE_DASH_API    _SetFilePointer,16
        DECLARE_DASH_API    _SetThreadPriority,8
        DECLARE_DASH_API    _Sleep,4
        DECLARE_DASH_API    _VirtualAlloc,16
        DECLARE_DASH_API    _VirtualFree,12
        DECLARE_DASH_API    _WaitForMultipleObjects,16
        DECLARE_DASH_API    _WaitForSingleObject,8
        DECLARE_DASH_API    _WriteFile,20
        DECLARE_DASH_API    _wsprintfA
        DECLARE_DASH_API    _wvsprintfA,12
        DECLARE_DASH_API    _XGetAudioFlags,0
        DECLARE_DASH_API    _XPhysicalAlloc,16
        DECLARE_DASH_API    _XPhysicalFree,4

        DECLARE_DASH_API    ___CxxFrameHandler
        DECLARE_DASH_API    __chkstk
        DECLARE_DASH_API    __CIacos
        DECLARE_DASH_API    __CIexp
        DECLARE_DASH_API    __CIpow
        DECLARE_DASH_API    __CxxThrowException,8
        DECLARE_DASH_API    __EH_prolog
        DECLARE_DASH_API    __ftol2
        DECLARE_DASH_API    __purecall
        DECLARE_DASH_API    _atexit
        DECLARE_DASH_API    _ceil
        DECLARE_DASH_API    _floor
        DECLARE_DASH_API    _free
        DECLARE_DASH_API    _malloc
        DECLARE_DASH_API    _memmove
        DECLARE_DASH_API    _sprintf
        DECLARE_DASH_API    _strncpy

        DECLARE_DASH_API    ??0exception@@QAE@ABV0@@Z           ; "public: __thiscall exception::exception(class exception const &)"
        DECLARE_DASH_API    ??0exception@@QAE@XZ                ; "public: __thiscall exception::exception(void)"
        DECLARE_DASH_API    ??1exception@@UAE@XZ                ; "public: virtual __thiscall exception::~exception(void)"
        DECLARE_DASH_API    ??3@YAXPAX@Z                        ; "void __cdecl operator delete(void *)"
        DECLARE_DASH_API    ??_L@YGXPAXIHP6EX0@Z1@Z             ; "void __stdcall `eh vector constructor iterator'(void *,unsigned int,int,void (__thiscall*)(void *),void (__thiscall*)(void *))"
        DECLARE_DASH_API    ??_M@YGXPAXIHP6EX0@Z@Z              ; "void __stdcall `eh vector destructor iterator'(void *,unsigned int,int,void (__thiscall*)(void *))"
        DECLARE_DASH_API    ??_U@YAPAXI@Z                       ; "void * __cdecl operator new[](unsigned int)"
        DECLARE_DASH_API    ??_V@YAXPAX@Z                       ; "void __cdecl operator delete[](void *)"
        DECLARE_DASH_API    ?_Xlen@_String_base@std@@QBEXXZ     ; "public: void __thiscall std::_String_base::_Xlen(void)const "
        DECLARE_DASH_API    ?_Xran@_String_base@std@@QBEXXZ     ; "public: void __thiscall std::_String_base::_Xran(void)const "

;
; BUGBUG: need to figure this out for RTTI
;

        DECLARE_DASH_API    ??_7type_info@@6B@                  ; "const type_info::`vftable'" ()

if DBG
        DECLARE_DASH_API    __CrtMemCheckpoint
        DECLARE_DASH_API    __CrtMemDumpAllObjectsSince
        DECLARE_DASH_API    __RTC_Initialize
endif

_DashAPIsEnd    label   near

DVDTHUNK ends

        end
