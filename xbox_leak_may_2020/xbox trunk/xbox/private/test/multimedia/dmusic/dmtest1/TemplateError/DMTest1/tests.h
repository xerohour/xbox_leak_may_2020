#pragma once

HRESULT SampleTest                              (void);
HRESULT AudioPath_Activate                      (CtIDirectMusicPerformance8* ptPerf8);
HRESULT AudioPath_GetObjectInPath               (CtIDirectMusicPerformance8* ptPerf8);
HRESULT AudioPath_SetVolume                     (CtIDirectMusicPerformance8* ptPerf8);

HRESULT Performance8_AddNotificationType        (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg                  (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_CloseDown                  (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_CreateStandardAudioPath    (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_SendPMsg                   (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_FreePMsg                   (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetGlobalParam             (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetNotificationPMsg        (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetSegmentState            (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetTime                    (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_IsPlaying                  (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_InitAudio                  (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_PlaySegmentEx              (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_Release                    (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_RemoveNotificationType     (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_SetGlobalParam             (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_StopEx                     (CtIDirectMusicPerformance8* ptPerf8);

HRESULT Segment8_Download                       (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Segment8_SetRepeats                     (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Segment8_Unload                         (CtIDirectMusicPerformance8* ptPerf8);

HRESULT SegmentState8_GetObjectInPath           (CtIDirectMusicPerformance8* ptPerf8);


HRESULT Script_Init                             (CtIDirectMusicPerformance8* ptPerf8);                                                              
HRESULT Script_CallRoutine                      (CtIDirectMusicPerformance8* ptPerf8);                                                     
HRESULT Script_SetVariableNumber                (CtIDirectMusicPerformance8* ptPerf8);                                                 
HRESULT Script_GetVariableNumber                (CtIDirectMusicPerformance8* ptPerf8);                                               
HRESULT Script_SetVariableObject                (CtIDirectMusicPerformance8* ptPerf8);                                                        
HRESULT Script_GetVariableObject                (CtIDirectMusicPerformance8* ptPerf8);

HRESULT Loader_CollectGarbage                   (CtIDirectMusicPerformance8* ptPerf8);


HRESULT AudioPath_SetVolume_Invalid(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_IsPlaying_Invalid(CtIDirectMusicPerformance8* ptPerf8);

