//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       tdmperf.h
//
//--------------------------------------------------------------------------

//===========================================================================
// tdmperf.h
//
// History:
//  01/05/1998 - markburt - created
//  03/10/1998 - jimmo    - Merged tdmperfi.h and tdmperfb.h
//                          Added C-Style declarations where needed
//  06/09/1998 - jimmo    - Added tdmperfAddPortNoInitInvalTest
//  07/31/1998 - a-llucar - added GetResolvedTime tests
//===========================================================================

#ifndef _TDMPERF_H
#define _TDMPERF_H


//===========================================================================
// Unit test prototypes
//===========================================================================
#ifdef __cplusplus

// tests
DWORD tdmperfInitTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfPlaySegmentTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetSegmentStateTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfSetPrepareTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfSetBumperLengthTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfSendPMsgTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfMusicToReferenceTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfReferenceToMusicTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetGraphTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfSetNotificationHandleTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetNotificationPMsgTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfAddNotificationTypeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfRemovePortTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfAssignPChannelBlockTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfAssignPChannelTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfPChannelInfoTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfDownloadInstrumentTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfInvalidateTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetParamTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfSetParamTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetGlobalParamTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetLatencyTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetQueueTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfAdjustTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfCloseDownTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfGetResolvedTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfQueryInterfaceTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfRhythmToTimeTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);

// dx7 tests (perf2)
DWORD tdmperf2QueryInterfaceTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);

// dx8 tests
DWORD tdmperf8ClonePMsg(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8CreateAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8CreateStandardAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8GetDefaultAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8GetParamEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8InitAudio(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8PlaySegmentEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8SetDefaultAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
//PHOOPHOO
//DWORD tdmperf8SetParamHook(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperf8StopEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);


#endif // __cplusplus

//===========================================================================
// Invalid test prototypes
//===========================================================================
#ifdef __cplusplus
DWORD tdmperfiInitInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiPlaySegmentInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiStopInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetSegmentStateInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetPrepareTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetBumperLengthInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiSendPMsgInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiSendPMsgWithCOMToolTest(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiAllocPMsgInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiMusicToReferenceTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiReferenceToMusicTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiIsPlayingInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiFreePMsgInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiFreePMsgWithdwSize0(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetGraphInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetNotificationPMsgInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiSetGraphInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiAddPortInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiRemovePortInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiAssignPChannelBlockInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiAssignPChannelInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiPChannelInfoInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiDownloadInstrumentInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetParamInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiSetParamInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiSetGlobalParamInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetGlobalParamInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetLatencyTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetQueueTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);

DWORD tdmperfAddPortNoInitInvalTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmPerfiQIInvalidTest1( BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2 );

DWORD tdmperfiPlaySegmentInvalTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiPlaySegmentInvalidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiGetResolvedTimeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfiAddNotificationTypeInvalidTest1(BOOL fUseNULL, BOOL fParamNum, LPARAM dwParam1, LPARAM dwParam2);
//dx8
DWORD tdmperi8ClonePMsg(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8CreateAudioPath(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8CreateStandardAudioPath(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8GetDefaultAudioPath(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8InitAudio(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8PlaySegmentEx(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8SetDefaultAudioPath(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8GetParamEx(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8StopEx(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperi8AudioPathWithBandSends(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperfiInitAudioWithDSoundOne(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperfiInitAudioWithDSoundNoInit(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperfiInitAudioWithDSoundNoSetCoop(BOOL,BOOL,LPARAM, LPARAM);
DWORD tdmperfiInitAudioWithDSoundSetCoopNorm(BOOL,BOOL,LPARAM, LPARAM);

#endif // __cplusplus

//===========================================================================
// Valid test prototypes
//===========================================================================
#ifdef __cplusplus
DWORD tdmperfvAdjustTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAddPortValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAddPortValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvCloseDownValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvCloseDownValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAssignPChannelValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAssignPChannelValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAssignPChannelBlockValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAssignPChannelBlockValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAddNotificationTypeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetNotificationHandleValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAddNotificationTypeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetLatencyTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest4(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest5(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPlaySegmentValidTest6(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);

DWORD tdmperfvStopValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvStopValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvStopValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInitValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvMusicToReferenceTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvAllocPMsgValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetBumperLengthValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetBumperLengthValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvFreePMsgValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSendPMsgWithCOMToolTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvReferenceToMusicTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvRemovePortValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvDownloadInstrumentValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvFreePMsgValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvFreePMsgValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetSegmentStateValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInvalidateValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwFlags, LPARAM mtTime);
DWORD tdmperfvGetSegmentStateValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetParamValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetGlobalParamValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvQIValidTest1( BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2 );

DWORD tdmperfvSetPrepareTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetResolvedTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetResolvedTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetParamValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvMusicToReferenceTimeValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvRhythmToTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvMIDIToMusicValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvMIDIToMusicValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);

// dx8 tests
DWORD tdmperv8AddNotificationTypeValidTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8ClonePMsg(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8AllocPMsgChannelGroup(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8CloseDownTwiceTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8CreateAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8CreateStandardAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8GetDefaultAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8GetParamEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8InitAudio(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentTypeEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentExFrom(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentExRPN(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentExSynthLeak(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8SetDefaultAudioPath(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
//DWORD tdmperv8SetParamHook(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8StopEx(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlaySegmentNoPiano(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInitAudioPlayWave(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInitAudioPlayWaveLoop(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInitAudioWithDSound(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PChannelInfo(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperv8PlayWaveAsSegment(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);


// CAP tests
DWORD tdmperfvStopCAPTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetGraphValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSendPMsgValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetQueueTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetQueueTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvInitValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetLatencyTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetGlobalParamValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPChannelInfoValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvPChannelInfoValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetPrepareTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvIsPlayingValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetParamValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvRemoveNotificationTypeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvReferenceToMusicTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvMusicToReferenceTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvSetGraphValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmPerfvMultipleToolsValidTest(BOOL fIndexFromEnd, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetGlobalParamValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);
DWORD tdmperfvGetGlobalParamValidTest4(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2);

#endif // __cplusplus

#endif // _TDMPERF_H








