/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    bldsetup.h

Abstract:

    Compiler and linker customizations

--*/

#ifdef _XBOX
#ifndef _BLDSETUP_H
#define _BLDSETUP_H

//
// Put code and data into named sections so that
// games could potentially unload them when they're not needed anymore
//
#pragma code_seg("WMVDEC")
#pragma data_seg("WMVDECRW")
#pragma const_seg("WMVDECRD")

//
// Remap some internal function names. This is a temporary fix to
// resolve link-time conflicts between WMA and WMV decoders.
// Post-launch, we should change dsound to pick up the new WMA decoder code.
// Then we can remove the WMA decoder files here and just use what's in dsound.
//

#define WMARawDecClose          WMV_WMARawDecClose
#define WMARawDecReset          WMV_WMARawDecReset
#define WMARawDecStatus         WMV_WMARawDecStatus
#define WMARawDecGetPCM         WMV_WMARawDecGetPCM
#define SWAPWORD                WMV_SWAPWORD
#define SWAPDWORD               WMV_SWAPDWORD
#define SwapWstr                WMV_SwapWstr
#define auInitPcInfoDEC         WMV_auInitPcInfoDEC
#define prvOutputBufferSize     WMV_prvOutputBufferSize
#define prvFinalLoad            WMV_prvFinalLoad
#define g_cBitGet               WMV_g_cBitGet
#define getMask                 WMV_getMask
#define WMA_set_fNoMoreData     WMV_WMA_set_fNoMoreData
#define WMA_get_nHdrBits        WMV_WMA_get_nHdrBits

#define ibstrmBitsLeft          WMV_ibstrmBitsLeft
#define ibstrmSetGetMoreData    WMV_ibstrmSetGetMoreData
#define ibstrmSetUserData       WMV_ibstrmSetUserData
#define ibstrmSetOwner          WMV_ibstrmSetOwner
#define ibstrmGetPacketHeader   WMV_ibstrmGetPacketHeader
#define ibstrmSetPacketHeader   WMV_ibstrmSetPacketHeader
#define ibstrmFlush             WMV_ibstrmFlush
#define ibstrmInit              WMV_ibstrmInit
#define ibstrmReset             WMV_ibstrmReset
#define ibstrmGetMoreData       WMV_ibstrmGetMoreData
#define ibstrmResetPacket       WMV_ibstrmResetPacket
#define ibstrmPeekBitsNonStop   WMV_ibstrmPeekBitsNonStop
#define ibstrmPeekBits          WMV_ibstrmPeekBits
#define ibstrmLookForBits       WMV_ibstrmLookForBits
#define ibstrmFlushBits         WMV_ibstrmFlushBits
#define ibstrmGetBits           WMV_ibstrmGetBits
#define ibstrmAttach            WMV_ibstrmAttach

#define RandStateClear          WMV_RandStateClear
#define LOG2                    WMV_LOG2
#define ROUNDF                  WMV_ROUNDF
#define MyOutputDbgStr          WMV_MyOutputDbgStr
#define ffltAvgSquareWeights    WMV_ffltAvgSquareWeights
#define ffltSqrtRatio           WMV_ffltSqrtRatio
#define fltPerfTimerDecodeTime  WMV_fltPerfTimerDecodeTime
#define quickRand               WMV_quickRand
#define huffDecGet              WMV_huffDecGet
#define InverseQuadRootOfSumSquares WMV_InverseQuadRootOfSumSquares
#define InverseQuadRootI        WMV_InverseQuadRootI

#define TraceInfoHelper         WMV_TraceInfoHelper
#define PerfTimerNew            WMV_PerfTimerNew
#define PerfTimerStart          WMV_PerfTimerStart
#define PerfTimerStop           WMV_PerfTimerStop
#define PerfTimerStopElapsed    WMV_PerfTimerStopElapsed
#define PerfTimerReport         WMV_PerfTimerReport
#define PerfTimerFree           WMV_PerfTimerFree

#define g_rgiHuffDecTblMsk      WMV_g_rgiHuffDecTblMsk
#define g_rgiHuffDecTblNoisePower WMV_g_rgiHuffDecTblNoisePower
#define g_rgiHuffDecTbl16smOb   WMV_g_rgiHuffDecTbl16smOb
#define g_rgiHuffDecTbl44smOb   WMV_g_rgiHuffDecTbl44smOb
#define g_rgiHuffDecTbl16ssOb   WMV_g_rgiHuffDecTbl16ssOb
#define g_rgiHuffDecTbl44ssOb   WMV_g_rgiHuffDecTbl44ssOb
#define g_rgiHuffDecTbl44smQb   WMV_g_rgiHuffDecTbl44smQb
#define g_rgiHuffDecTbl44ssQb   WMV_g_rgiHuffDecTbl44ssQb
#define g_rgiBarkFreq           WMV_g_rgiBarkFreq
#define g_rgfiPwrToRMS          WMV_g_rgfiPwrToRMS
#define g_rgfltLsfReconLevel    WMV_g_rgfltLsfReconLevel
#define g_SinTableForReconstruction WMV_g_SinTableForReconstruction
#define rgSinCosTables          WMV_rgSinCosTables
#define rgiMaskMinusPower10     WMV_rgiMaskMinusPower10
#define rgiMaskPlusPower10      WMV_rgiMaskPlusPower10
#define rgDBPower10             WMV_rgDBPower10
#define rgfltDBPowerExponentScale WMV_rgfltDBPowerExponentScale

#define g_sct64                 WMV_g_sct64
#define g_sct128                WMV_g_sct128
#define g_sct256                WMV_g_sct256
#define g_sct512                WMV_g_sct512
#define g_sct1024               WMV_g_sct1024
#define g_sct2048               WMV_g_sct2048
#define gRun16smOb              WMV_gRun16smOb
#define gRun16ssOb              WMV_gRun16ssOb
#define gRun44smOb              WMV_gRun44smOb
#define gRun44ssOb              WMV_gRun44ssOb
#define gRun44smQb              WMV_gRun44smQb
#define gRun44ssQb              WMV_gRun44ssQb
#define gLevel16smOb            WMV_gLevel16smOb
#define gLevel16ssOb            WMV_gLevel16ssOb
#define gLevel44smOb            WMV_gLevel44smOb
#define gLevel44ssOb            WMV_gLevel44ssOb
#define gLevel44smQb            WMV_gLevel44smQb
#define gLevel44ssQb            WMV_gLevel44ssQb

#ifndef WMV_MEMORY_DEBUGGING

#define wmvalloc    malloc
#define wmvfree     free
#define wmvrealloc  realloc

#else // WMV_MEMORY_DEBUGGING

#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

void  WmvDebugMemInit();
void* WmvDebugAlloc(char* filename, int linenum, size_t size);
void  WmvDebugFree(void* ptr);
void* WmvDebugRealloc(char* filename, int linenum, void* ptr, size_t size);

#define wmvalloc(size) WmvDebugAlloc(__FILE__, __LINE__, size)
#define wmvfree WmvDebugFree
#define wmvrealloc(ptr, size) WmvDebugRealloc(__FILE__, __LINE__, ptr, size)

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
static __forceinline void* __cdecl operator new(size_t size) { return wmvalloc(size); }
static __forceinline void __cdecl operator delete(void* ptr) { wmvfree(ptr); }

static __forceinline void* __cdecl operator new[](size_t size) { return wmvalloc(size); }
static __forceinline void __cdecl operator delete(void* ptr ) { wmvfree(ptr); }
#endif

#endif // WMV_MEMORY_DEBUGGING

#endif // !_BLDSETUP_H
#endif // XBOX

