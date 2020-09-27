/************************************************************************
*                                                                       *
*   mswteEnum.h -- Constants definitions for Web Telephony Engine.      *
*                                                                       *
*   Copyright (c) 1991-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef MSWTEENUM_H_INCLUDED
#define MSWTEENUM_H_INCLUDED


typedef enum _ConstrainsType
{
    wteConsNone = 0,
    wteConsTerminator = 1,
    wteConsLegalDigits = 2,
    wteConsBoundaries = 4,
    wteConsAny = 8,
} ConstrainsType;

typedef enum _TelephonyObjectEndReasonType
{
    wteEndReasonScopeEnd = 0,
    wteEndReasonCutThroughChanged = 1,
    wteEndReasonIntrinsicControl = 2,
    wteEndReasonTable = 3,
    wteEndReasonIncompatibleMenuItemType = 4,
    wteEndReasonForm = 5,
    wteEndReasonPageEnd = 6,
    wteEndReasonSelectionTimeout = 7,
    wteEndReasonSelectionError = 8,
    wteEndReasonMax = 2147483647L
} TelephonyObjectEndReasonType;

typedef enum _DispidApplicationEventType
{
    wteDispidApplicationEventOnOffering =            1,
    wteDispidApplicationEventOnConnected =           2,
    wteDispidApplicationEventOnDisconnected =        3,
    wteDispidApplicationEventOnDigit =               4,
    wteDispidApplicationEventOnDigitsUsed =          5,
    wteDispidApplicationEventOnBeforeNavigate =      6,
    wteDispidApplicationEventOnNavigateComplete =    7,
    wteDispidApplicationEventOnDocumentComplete =    8,
    wteDispidApplicationEventOnTagBegin =            9,
    wteDispidApplicationEventOnTagEnd =              10,
    wteDispidApplicationEventOnSessionValue =        11,
    wteDispidApplicationEventOnStepEnd =             12,
    wteDispidApplicationEventOnSessionEnd =          13
} DispidApplicationEventType;

typedef enum _PrivateWTEControlsType
{
    wteServicePauseSoft = 128,
    wteServiceStopSoft = 129,
    wteServiceFlushLog = 130,
    wteServiceRefreshStorage = 131,

    wteServiceMax = 255
} PrivateWTEControlsType;
    
typedef enum _LogFileDirectoryType
{
    wteLogFullPath,
    wteLogRelativePath
} LogFileDirectoryType;

typedef enum  _LogType
{
    wteLogToFile,
    wteLogToSql
}  LogType;

typedef enum  _LogPeriodType
{
    wteLogInvalidPeriod = -1,
    wteLogNoPeriod = 0,
    wteLogDaily,
    wteLogWeekly,
    wteLogMonthly,
    wteLogYearly
} LogPeriodType;

typedef enum _LogEventType
{
    wteAuthoringError = 0x1,
    wteCallAvailable = 0x2,
    wteSessionConnection = 0x4,
    wteNavigate = 0x8,
    wteCallTransfer = 0x10,
    wteCallHangup = 0x20,
    wteSessionEnd = 0x40,
    wteCallerTimeout = 0x80,
    wteCallInfoChanged = 0x100,
    wteDtmfInputError = 0x200,
    wteCustomLog = 0x400,
    wteDtmfReceived = 0x800,
    wteSpeechInputData = 0x1000,
    wteSpeechInputFailed = 0x2000,
    wteRecordData = 0x4000,
    wteRecordFailed = 0x8000,
    wteAudioFailure = 0x10000
} LogEventType;

typedef enum _LogFieldType
{
    wteSessionID = 0x1,
    wteLogTime = 0x2,
    wteEventClass = 0x4,
    wteMachineName = 0x8,
    wteAddressGroup = 0x10,
    wteAddress = 0x20,
    wteCallerID = 0x40,
    wteCalledID = 0x80,
    wteReason = 0x100,
    wteApplicationName = 0x200,
    wteMedia = 0x400,
    wtePage = 0x800,
    wteDTMFValue = 0x1000,
    wteDestAddress = 0x2000,
    wteHangReason = 0x4000,
    wteTransferType = 0x8000,
    wteErrorType = 0x10000,
    wteInputRecordFile = 0x20000,
    wteCustom = 0x40000
} LogFieldType;

typedef enum _PCMWaveFormats
{
    wteDefaultWaveFormat = 0,

    wte8000Hz8BitMono = 1,

    wte8000Hz16BitMono = 2,

    wte8000Hz8BitStereo = 3,

    wte8000Hz16BitStereo = 4,

    wte11025Hz8BitMono = 5,

    wte11025Hz16BitMono = 6,

    wte11025Hz8BitStereo = 7,

    wte11025Hz16BitStereo = 8,

    wte12000Hz8BitMono = 9,

    wte12000Hz16BitMono = 10,

    wte12000Hz8BitStereo = 11,

    wte12000Hz16BitStereo = 12,

    wte16000Hz8BitMono = 13,

    wte16000Hz16BitMono = 14,

    wte16000Hz8BitStereo = 15,

    wte16000Hz16BitStereo = 16,

    wte22050Hz8BitMono = 17,

    wte22050Hz16BitMono = 18,

    wte22050Hz8BitStereo = 19,

    wte22050Hz16BitStereo = 20,

    wte24000Hz8BitMono = 21,

    wte24000Hz16BitMono = 22,

    wte24000Hz8BitStereo = 23,

    wte24000Hz16BitStereo = 24,

    wte32000Hz8BitMono = 25,

    wte32000Hz16BitMono = 26,

    wte32000Hz8BitStereo = 27,

    wte32000Hz16BitStereo = 28,

    wte44100Hz8BitMono = 29,

    wte44100Hz16BitMono = 30,

    wte44100Hz8BitStereo = 31,

    wte44100Hz16BitStereo = 32,

    wte48000Hz8BitMono = 33,

    wte48000Hz16BitMono = 34,

    wte48000Hz8BitStereo = 35,

    wte48000Hz16BitStereo = 36
} PCMWaveFormats;

#endif // ! MSWTEENUM_H_INCLUDED

