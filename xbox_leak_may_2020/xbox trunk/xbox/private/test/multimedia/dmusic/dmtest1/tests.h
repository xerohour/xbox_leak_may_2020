#pragma once



//BVT tests

    //DANHAFF
    HRESULT AudioPath_Activate_BVT(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_GetObjectInPath_BVT           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_SetVolume_BVT                 (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_SetPitch_BVT                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_AddNotificationType_BVT    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_AllocPMsg_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_ClonePMsg_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_CloseDown_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_CreateStandardAudioPath_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_SendPMsg_BVT               (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_FreePMsg_BVT               (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetParamEx_BVT             (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetGlobalParam_BVT         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetNotificationPMsg_BVT    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetSegmentState_BVT        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_GetTime_BVT_ReferenceTime  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwErrorMargin, DWORD dwUnused2);
    HRESULT Performance8_GetTime_BVT_MusicTime      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwErrorMargin, DWORD dwUnused2);
    HRESULT Performance8_GetTime_Valid_InOrder      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_GetTime_BVT                (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetSetDefaultAudioPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_IsPlaying_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_InitAudio_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_MusicToReferenceTime_BVT   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_ReferenceToMusicTime_BVT   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_BVT          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_Release_BVT                (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_RemoveNotificationType_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_SetGlobalParam_BVT         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_StopEx_BVT                 (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_RelativeVolume(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


    HRESULT Segment8_Download_BVT                   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_SetRepeats_BVT                 (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Unload_BVT                     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_GetLength_BVT                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_SetLength_BVT                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Compose_BVT                    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


    HRESULT SegmentState_GetObjectInPath_BVT       (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_GetSegment_BVT             (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_GetStartTime_BVT           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_SetVolume_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwUnused2);
    HRESULT SegmentState_SetPitch_BVT               (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT SegmentState_GetSegment_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD bFull, DWORD dwUnused2);


    HRESULT Script_InitBVT                     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                              
    HRESULT Script_CallRoutineBVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                     
    HRESULT Script_SetVariableNumberBVT        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                 
    HRESULT Script_GetVariableNumberBVT        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                               
    HRESULT Script_SetVariableObjectBVT        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                        
    HRESULT Script_GetVariableObjectBVT        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    
    
    HRESULT Loader_ClearCache_BVT              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_GetObject_BVT               (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_SetObject_BVT               (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_SetSearchDirectory_BVT      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_ReleaseObjectByUnknown_BVT  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_LoadObjectFromFile_BVT      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

// ToolGraph tests
	HRESULT ToolGraph_InsertTool_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_StampPMsg_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_RemoveTool_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_GetTool_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);




//Valid Tests
    //DANHAFF
    HRESULT SampleTest                              (void);


    //ADPCM
    HRESULT ADPCM_TestWaveSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwDirName, DWORD dwBaseName);
    HRESULT ADPCM_TestMusicSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwDirName, DWORD dwBaseName);


    //HRESULT AudioPath_Activate_Valid                (CtIDirectMusicPerformance8* ptPerf8);
    HRESULT AudioPath_Activate_Valid_Many(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwPathType, DWORD dwCount);
    HRESULT AudioPath_Activate_Valid_Sharing(CtIDirectMusicPerformance8 *ptPerf8, DWORD bShared, DWORD dwUnused2);
    HRESULT AudioPath_Activate_Valid_OnAndOff(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


    //HRESULT AudioPath_GetObjectInPath_Valid         (CtIDirectMusicPerformance8* ptPerf8);
    HRESULT AudioPath_GetObjectInPath_Valid_GetAll(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_GetObjectInPath_Valid_Traverse(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPathType, DWORD dwPathDefinition);
    HRESULT AudioPath_GetObjectInPath_Valid_SimplyGetBuffer(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


    //BUFFER Tests for GetObjectInPath.
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetPosition_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetAllParameters(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeAngles(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOutsideVolume(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetMinDistance_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetMode(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetPosition_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetPosition_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetVelocity(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_INFINITY_Bug(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetFrequency_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetFrequency_Range(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetVolume_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_GetObjectInPath_3DBuf_SetVolume_Range(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer);
    HRESULT AudioPath_SetVolume_Valid_Ramp          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime,    DWORD dwRange);
    HRESULT AudioPath_SetVolume_Valid_Ind           (CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwUnused2);

    HRESULT AudioPath_SetPitch_Valid_Ramp           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_SetPitch_Valid_Immediate      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT DLS_TestSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentName, DWORD dwUnused2);

    ////////////// PERFORMANCE8 ///////////////
    HRESULT Performance8_AddNotificationType_NoRestart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes);
    HRESULT Performance8_AddNotificationType_Restart  (CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes);

    HRESULT Performance8_AllocPMsg_Valid            (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_ClonePMsg_Valid            (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_CloseDown_WhilePlaying(CtIDirectMusicPerformance8* ptPerf8, DWORD dwScenario, DWORD dwUnused2);

    HRESULT Performance8_CloseDown_Valid            (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_CreateStandardAudioPath_AllCombos      (CtIDirectMusicPerformance8* ptPerf8, DWORD bAllCombos, DWORD dwUnused2);
    HRESULT Performance8_Play2AudioPaths                        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_CreateStandardAudioPath_Wait           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_CreateStandardAudioPath_LimitPChannels (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    
    HRESULT Performance8_PMsg_Valid_Tempos                      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PMsg_Valid_Transpose                   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PMsg_Valid_PitchBend(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PMsg_Valid_Transpose_ChangeSong(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PMsg_Crossfade(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);



    HRESULT Performance8_FreePMsg_Valid             (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetParamEx_Valid           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetGlobalParam_Valid       (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetNotificationPMsg_Valid  (CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetSegmentState_Valid      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetTime_Valid              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_GetSetDefaultAudioPath_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_IsPlaying_Valid            (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_InitAudio_Valid            (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_MusicToReferenceTime_Repeat     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_MusicToReferenceTime_TempoChange(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_PlaySegmentEx_Valid        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_PlaySegment_StartTime_Start(CtIDirectMusicPerformance8      *ptPerf8, DWORD mtRel, DWORD rtRel);
    HRESULT Performance8_PlaySegment_StartTime_Transition(CtIDirectMusicPerformance8 *ptPerf8, DWORD mtRel, DWORD rtRel);
    HRESULT Performance8_PlaySegment_TransitionType(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwFlags, DWORD dwUnused2);
    HRESULT Performance8_PlaySegment_TransPreAlign(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwFlags, DWORD dwUnused2);



    HRESULT Performance8_PlaySegmentEx_Valid_Play   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime);
    HRESULT Performance8_PlaySegmentEx_Valid_PlayBeforeDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime);
    HRESULT Performance8_PlaySegment_Latency        (CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegment_Latency_Interactive(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_Valid_DelayDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwDelay);
    HRESULT Performance8_PlaySegmentEx_Valid_SetFrequency(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime);
    HRESULT Performance8_PlaySegmentEx_QuickStartStop(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_QuickStartStopOneFile(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    
    //Allow you to set repeats.
    HRESULT Performance8_PlaySegmentEx_Segment_WaitForEnd      (CtIDirectMusicPerformance8 *ptPerf8, DWORD dwSegmentName, DWORD dwRepeats);
    HRESULT Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwSegmentName, DWORD dwRepeats);
    HRESULT Performance8_PlaySegmentEx_Multiple_NoRepeats(CtIDirectMusicPerformance *ptPerf8, DWORD szSegmentName, DWORD dwCount);
    HRESULT Performance8_PlaySegmentEx_Multiple_OneRepeat(CtIDirectMusicPerformance *ptPerf8, DWORD szSegmentName, DWORD dwCount);
    HRESULT Performance8_PlaySegmentEx_QuickStartStop_ADPCM(CtIDirectMusicPerformance *ptPerf8,        DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_QuickStartStop_WaveFiles(CtIDirectMusicPerformance *ptPerf8,    DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_QuickStartStop_WaveSegments(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_CompareWithDSound(CtIDirectMusicPerformance *ptPerf8,           DWORD dwPath,    DWORD dwWave);
    HRESULT Performance8_PlaySegmentEx_Timing(CtIDirectMusicPerformance *ptPerf8,                      DWORD dwSegment, DWORD dwExpectedTime);

    HRESULT Performance8_PlaySegmentEx_StartStopStreaming_PerfMem(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_ClockTime_Looping(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSuffix, DWORD dwType);

    HRESULT Performance8_PlaySegmentEx_Leak         (CtIDirectMusicPerformance *ptPerf8, DWORD dwSegment1, DWORD dwSegment2);
    HRESULT Performance8_PlaySegmentEx_AudioPath    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwUnused2);

    HRESULT Performance8_PlaySegmentEx_AudioPath_1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback);
    HRESULT Performance8_PlaySegmentEx_AudioPath_2(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback);
    HRESULT Performance8_PlaySegmentEx_AudioPath_4(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback);
    HRESULT Performance8_PlaySegmentEx_AudioPath_6(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback);
    HRESULT Performance8_PlaySegmentEx_MunchPerf(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_AntzLooping(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


    HRESULT Performance8_ReferenceToMusicTime_TempoChange (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_Release_Valid              (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_RemoveNotificationType_Valid(CtIDirectMusicPerformance8* ptPerf8,DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_SetGlobalParam_Valid       (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_StopEx_NoSegments(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_StopEx_1Segment_DLS  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1,   DWORD dwUnused2);
    HRESULT Performance8_StopEx_1Segment_Wave (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1,   DWORD dwUnused2);
    HRESULT Performance8_StopEx_2Segments_DLS (CtIDirectMusicPerformance8* ptPerf8, DWORD bSinglePath, DWORD bSingleSegment);
    HRESULT Performance8_StopEx_2Segments_Wave(CtIDirectMusicPerformance8* ptPerf8, DWORD bSinglePath, DWORD bSingleSegment);
    HRESULT Performance8_StopEx_TimePast  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_StopEx_TimeFuture(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_StopEx_NoDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    //////////////// SEGMENT ///////////////////////

    HRESULT Segment8_AddNotificationType_NoRestart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes);
    HRESULT Segment8_AddNotificationType_Restart  (CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes);

    HRESULT Segment8_Download_Test1_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
    HRESULT Segment8_Download_Test2_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
    HRESULT Segment8_Download_Test3_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
    HRESULT Segment8_Download_Test4_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
    HRESULT Segment8_Download_PreCache(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFile1, DWORD dwFile2);
    HRESULT Segment8_Download_PreCache_Multiple01(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Download_PreCache_Multiple02(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Unload_Test1_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Unload_Test2_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Unload_Test3_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_Unload_Test4_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
	HRESULT Segment8_SetRepeats_Test1_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
	HRESULT Segment8_SetRepeats_Test2_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
	HRESULT Segment8_SetRepeats_Test3_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
	HRESULT Segment8_SetRepeats_Test4_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
	HRESULT Segment8_SetRepeats_Test5_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
	HRESULT Segment8_SetRepeats_Test6_Valid(CtIDirectMusicPerformance8*, DWORD, DWORD);
    HRESULT Segment8_GetLength_Valid                (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_SetLength_Test1_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_SetLength_Range(CtIDirectMusicPerformance8* ptPerf8, DWORD dwMusicTimeLength, DWORD dwUnused2);
    HRESULT Segment8_Compose_Valid                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_GetObjectInPath_Valid     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_GetStartTime_Valid         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_GetSegment_Valid           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT SegmentState_SetVolume_Valid_Ramp       (CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime   , DWORD dwRange);
    HRESULT SegmentState_SetPitch_Valid_Ramp        (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Segment8_GetClockTimeLength_VerifySegmentLength(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentName, DWORD dwExpectedLength);
    HRESULT Segment8_GetClockTimeLength_GetSet(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentName, DWORD dwExpectedLength);
    
//    HRESULT Segment8_SetClockTimeLoopPoints_SetsClockTimeMode(CtIDirectMusicPerformance8* ptPerf8,  DWORD dwUnused1, DWORD dwUnused2);
//    HRESULT Segment8_SetClockTimeLoopPoints_KeepsLength(CtIDirectMusicPerformance8* ptPerf8,        DWORD dwUnused1, DWORD dwUnused2);
//    HRESULT Segment8_SetClockTimeLoopPoints_DLS     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
//    HRESULT Segment8_SetClockTimeLoopPoints_WaveSeg (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
//    HRESULT Segment8_SetClockTimeLoopPoints_Wave    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);

    //HRESULT Segment8_GetClockTimeLoopPoints_SetAndGet(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    
    HRESULT Segment8_SetStartPoint_DLS_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_DLS_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_DLS_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_DLS_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1,DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_DLS_End     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);

    HRESULT Segment8_SetStartPoint_WaveSeg_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_WaveSeg_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_WaveSeg_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_WaveSeg_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1,DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_WaveSeg_End     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);

    HRESULT Segment8_SetStartPoint_Wave_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_Wave_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_Wave_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_Wave_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1,DWORD dwLoopPoint2);
    HRESULT Segment8_SetStartPoint_Wave_End     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2);

    HRESULT Segment8_GetStartPoint_Generic(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegment, DWORD dwStartPointExpected);

    HRESULT Segment8_SetWavePlaybackParams_WAVEF_OFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegment, DWORD dwUnused);
    HRESULT Segment8_GetRepeats_Programmatic(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingOFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference);
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON (CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference);
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingOFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference);
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingON(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference);
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE(CtIDirectMusicPerformance8* ptPerf8, DWORD bNoInvalidate, DWORD dwUnused2);
    HRESULT Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS(CtIDirectMusicPerformance8* ptPerf8, DWORD bIgnoreLoops, DWORD dwUnused2);

    HRESULT Segment8_GetRepeats_Content(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRepeats, DWORD dwUnused2);





    ////////////// SCRIPT ///////////////
    HRESULT Script_Init_Valid                       (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                              
    HRESULT Script_CallRoutine_Valid                (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                     
    HRESULT Script_CallRoutine_Valid_TransitionTempo(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_SetVariableNumber_Valid          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                 
    HRESULT Script_GetVariableNumber_Valid          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                               
    HRESULT Script_SetVariableObject_Valid          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);                                                        
    HRESULT Script_GetVariableObject_Valid          (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_CallRoutine_Valid_TestScript           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected);
    HRESULT Script_CallRoutine_Valid_TestScript_Transition(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected);
    HRESULT Script_CallRoutine_Valid_TestScript_TransSimple(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected);
    HRESULT Script_CallRoutine_Valid_TestScript_SegBoundaries(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected);
    HRESULT Script_CallRoutine_Invalid_TestScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected);
    HRESULT Script_CallRoutine_Valid_PlayOneScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwScript, DWORD dwRoutine);
    HRESULT Script_SetVariableString_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_GetVariableString_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    ////////////// LOADER ///////////////
    HRESULT Loader_CollectGarbage_Valid             (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_ClearCache_Valid                 (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_GetObject_Valid                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_GetObject_IStream_Test           (CtIDirectMusicPerformance8* ptPerf8, DWORD dwFileName, DWORD dwUnused2);
    HRESULT Loader_GetObject_IStream_Streaming      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwFileName, DWORD dwUnused2);

    HRESULT Loader_SetObject_Valid                  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_SetSearchDirectory_Valid         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_ReleaseObjectByUnknown_Valid     (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_LoadObjectFromFile_Valid         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_LoadObjectFromFile_Valid_AbsolutePath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2);
    HRESULT Loader_LoadObjectFromFile_Valid_Bug7707(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader_LoadObjectFromFile_Container_ByFileName(CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwFileName);
    HRESULT Loader_LoadObjectFromFile_Container_ByName    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwName);
    HRESULT Loader_LoadObjectFromFile_Container_ByGUID    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwGUID);
    HRESULT Loader_LoadObjectFromFile_Container_ByFileName_Script(CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwFileName);
    HRESULT Loader_LoadObjectFromFile_Container_ByName_Script    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwName);
    HRESULT Loader_LoadObjectFromFile_Container_ByGUID_Script    (CtIDirectMusicPerformance8* ptPerf8, DWORD dwContainer, DWORD dwGUID);


    ////////////// TOOLGRAPH ///////////////
	HRESULT ToolGraph_InsertTool_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_StampPMsg_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_RemoveTool_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
	HRESULT ToolGraph_GetTool_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Sleep_Zero(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime);



//Invalid Tests
    //DANHAFF
    HRESULT AudioPath_GetObjectInPath_Invalid   (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT AudioPath_SetVolume_Invalid         (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader8_LoadObjectFromFile_Invalid  (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader8_LoadObjectFromFile_Invalid_GMRef(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2);
    HRESULT Loader8_LoadObjectFromFile_TooLongPath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Loader8_LoadObjectFromFile_Invalid_Content(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_AllocPMsg_Invalid      (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Performance8_IsPlaying_Invalid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Performance8_InitAudio_Fatal (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_CallRoutine_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_GetVariableNumber_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_GetVariableObject_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_Init_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_SetVariableObject_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);
    HRESULT Script_SetVariableNumber_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2);

    HRESULT Script_CallRoutine_Invalid_NoAudioPath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD dwUnused);
    HRESULT Fail_On_Purpose(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


//Performance (memory) tests
    HRESULT Loader_GetObject_PerfMem_DownloadWaveLeak(CtIDirectMusicPerformance8* ptPerf8, DWORD bStreaming, DWORD dwUnused2);
    HRESULT Loader_GetObject_PerfMem_Type_Full(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2);
    HRESULT Loader_GetObject_PerfMem_Type_Partial(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2);
    HRESULT Performance8_PlaySegmentEx_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD bFull);
    HRESULT Performance8_CreateStandardAudioPath_PerfMem_Bug10385(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTempoFactor, DWORD dwAudioPath);

    HRESULT Script_CallRoutine_PerfMem1_Partial( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptName, DWORD dwRoutineIndex);
    HRESULT Script_CallRoutine_PerfMem1_Full( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptName, DWORD dwRoutineIndex);
    HRESULT Script_CallRoutine_PerfMem_TestScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD bFull);
    HRESULT Performance8_CreateStandardAudioPath_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD bFull, DWORD dwUnused2);
