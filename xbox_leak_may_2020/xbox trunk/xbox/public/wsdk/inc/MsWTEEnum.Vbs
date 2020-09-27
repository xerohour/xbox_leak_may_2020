''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Copyright (c) 1991-1999, Microsoft Corp. All rights reserved.              '
'                                                                              '
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

const wteConsNone = 0
const wteConsTerminator = 1
const wteConsLegalDigits = 2
const wteConsBoundaries = 4
const wteConsAny = 8

const wteEndReasonScopeEnd = 0
const wteEndReasonCutThroughChanged = 1
const wteEndReasonIntrinsicControl = 2
const wteEndReasonTable = 3
const wteEndReasonIncompatibleMenuItemType = 4
const wteEndReasonForm = 5
const wteEndReasonPageEnd = 6
const wteEndReasonSelectionTimeout = 7
const wteEndReasonSelectionError = 8
const wteEndReasonMax = 2147483647

const wteDispidApplicationEventOnOffering = 1
const wteDispidApplicationEventOnConnected = 2
const wteDispidApplicationEventOnDisconnected = 3
const wteDispidApplicationEventOnDigit = 4
const wteDispidApplicationEventOnDigitsUsed = 5
const wteDispidApplicationEventOnBeforeNavigate = 6
const wteDispidApplicationEventOnNavigateComplete = 7
const wteDispidApplicationEventOnDocumentComplete = 8
const wteDispidApplicationEventOnTagBegin = 9
const wteDispidApplicationEventOnTagEnd = 10
const wteDispidApplicationEventOnSessionValue = 11
const wteDispidApplicationEventOnStepEnd = 12
const wteDispidApplicationEventOnSessionEnd = 13

const wteServicePauseSoft = 128
const wteServiceStopSoft = 129
const wteServiceFlushLog = 130
const wteServiceRefreshStorage = 131
const wteServiceMax = 255

const wteLogFullPath = 0
const wteLogRelativePath = 1

const wteLogToFile = 0
const wteLogToSql = 1

const wteLogInvalidPeriod = -1
const wteLogNoPeriod = 0
const wteLogDaily = 1
const wteLogWeekly = 2
const wteLogMonthly = 3
const wteLogYearly = 4

const wteAuthoringError = &H1&
const wteCallAvailable = &H2&
const wteSessionConnection = &H4&
const wteNavigate = &H8&
const wteCallTransfer = &H10&
const wteCallHangup = &H20&
const wteSessionEnd = &H40&
const wteCallerTimeout = &H80&
const wteCallInfoChanged = &H100&
const wteDtmfInputError = &H200&
const wteCustomLog = &H400&
const wteDtmfReceived = &H800&
const wteSpeechInputData = &H1000&
const wteSpeechInputFailed = &H2000&
const wteRecordData = &H4000&
const wteRecordFailed = &H8000&
const wteAudioFailure = &H10000&

const wteSessionID = &H1&
const wteLogTime = &H2&
const wteEventClass = &H4&
const wteMachineName = &H8&
const wteAddressGroup = &H10&
const wteAddress = &H20&
const wteCallerID = &H40&
const wteCalledID = &H80&
const wteReason = &H100&
const wteApplicationName = &H200&
const wteMedia = &H400&
const wtePage = &H800&
const wteDTMFValue = &H1000&
const wteDestAddress = &H2000&
const wteHangReason = &H4000&
const wteTransferType = &H8000&
const wteErrorType = &H10000&
const wteInputRecordFile = &H20000&
const wteCustom = &H40000&

const wteDefaultWaveFormat = 0
const wte8000Hz8BitMono = 1
const wte8000Hz16BitMono = 2
const wte8000Hz8BitStereo = 3
const wte8000Hz16BitStereo = 4
const wte11025Hz8BitMono = 5
const wte11025Hz16BitMono = 6
const wte11025Hz8BitStereo = 7
const wte11025Hz16BitStereo = 8
const wte12000Hz8BitMono = 9
const wte12000Hz16BitMono = 10
const wte12000Hz8BitStereo = 11
const wte12000Hz16BitStereo = 12
const wte16000Hz8BitMono = 13
const wte16000Hz16BitMono = 14
const wte16000Hz8BitStereo = 15
const wte16000Hz16BitStereo = 16
const wte22050Hz8BitMono = 17
const wte22050Hz16BitMono = 18
const wte22050Hz8BitStereo = 19
const wte22050Hz16BitStereo = 20
const wte24000Hz8BitMono = 21
const wte24000Hz16BitMono = 22
const wte24000Hz8BitStereo = 23
const wte24000Hz16BitStereo = 24
const wte32000Hz8BitMono = 25
const wte32000Hz16BitMono = 26
const wte32000Hz8BitStereo = 27
const wte32000Hz16BitStereo = 28
const wte44100Hz8BitMono = 29
const wte44100Hz16BitMono = 30
const wte44100Hz8BitStereo = 31
const wte44100Hz16BitStereo = 32
const wte48000Hz8BitMono = 33
const wte48000Hz16BitMono = 34
const wte48000Hz8BitStereo = 35
const wte48000Hz16BitStereo = 36

