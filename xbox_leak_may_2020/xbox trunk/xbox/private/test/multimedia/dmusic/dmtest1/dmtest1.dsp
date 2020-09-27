# Microsoft Developer Studio Project File - Name="dmtest1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=dmtest1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dmtest1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dmtest1.mak" CFG="dmtest1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dmtest1 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "dmtest1 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "dmtest1 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f dmtest1.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dmtest1.exe"
# PROP BASE Bsc_Name "dmtest1.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "f:\xboxsd\public\tools\razzle.cmd go"
# PROP Rebuild_Opt ""
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dmtest1 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f dmtest1.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dmtest1.exe"
# PROP BASE Bsc_Name "dmtest1.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "%_NTDRIVE%%_NTROOT%\public\tools\razzle b.bat"
# PROP Rebuild_Opt ""
# PROP Target_File "dmtest1.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "dmtest1 - Win32 Release"
# Name "dmtest1 - Win32 Debug"

!IF  "$(CFG)" == "dmtest1 - Win32 Release"

!ELSEIF  "$(CFG)" == "dmtest1 - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ADPCM_Standard.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_Activate.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath1.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath_DSBTest.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath_DSBTestFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_SetPitch.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_SetVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_SetVolume_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\CICMusicX.cpp
# End Source File
# Begin Source File

SOURCE=.\DLS_Volume.cpp
# End Source File
# Begin Source File

SOURCE=.\DMTEST1.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_Buffer3d.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_ComCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_Definitions.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_DoWork.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_FileIOStress.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_MusicHeapTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_StatsMemoryUse.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_Stream.CPP
# End Source File
# Begin Source File

SOURCE=.\Help_Suite.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_SuiteList.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_TestCases.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_VerifyPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Help_WorstTimes.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_ClearCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_CollectGarbage.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_GetObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_LoadObjectFromFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_LoadObjectFromFile_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_LoadObjectFromFile_IStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_ReleaseObjectByUnknown.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_SetObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader_SetSearchDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\Logging.cpp
# End Source File
# Begin Source File

SOURCE=.\Perf_Template.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_AddNotificationType.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_AllocPMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_AllocPMsg_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_ClonePMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_CloseDown.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_CreateStandardAudioPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_FreePMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetGlobalParam.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetNotificationPMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetParamEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetSegmentState.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetSetDefaultAudioPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_GetTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_InitAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_InitAudio_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_IsPlaying.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_IsPlaying_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_MusicToReferenceTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_PlaySegmentEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_ReferenceToMusicTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_RemoveNotificationType.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_SendPMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_SetGlobalParam.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance8_StopEx.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_CallRoutine.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_CallRoutine_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_GetSetVariableString.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_GetVariableNumber.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_GetVariableNumber_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_GetVariableObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_GetVariableObject_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_Init.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_Init_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_SetVariableNumber.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_SetVariableNumber_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_SetVariableObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Script_SetVariableObject_Invalid.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_Compose.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_GetClockTimeLength.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_GetLength.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_GetStartPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetClockTimeLength.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetClockTimeLoopPoints.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetLength.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetRepeats.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetStartPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment8_SetWavePlaybackParams.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentState8_GetObjectInPath1.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentState8_GetSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentState8_GetStartTime.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentState8_SetPitch.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentState8_SetVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolEcho.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraph_GetTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraph_InsertTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraph_RemoveTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraph_StampPMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTrace.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath_DSBTestFunc.h
# End Source File
# Begin Source File

SOURCE=.\CICMusicX.h
# End Source File
# Begin Source File

SOURCE=.\FakeStuff.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\Help_Buffer3D.h
# End Source File
# Begin Source File

SOURCE=.\Help_ComCounter.h
# End Source File
# Begin Source File

SOURCE=.\Help_Definitions.h
# End Source File
# Begin Source File

SOURCE=.\Help_DoWork.h
# End Source File
# Begin Source File

SOURCE=.\Help_FileIOStress.h
# End Source File
# Begin Source File

SOURCE=.\Help_MusicHeap.h
# End Source File
# Begin Source File

SOURCE=.\Help_StatsMemoryUse.h
# End Source File
# Begin Source File

SOURCE=.\Help_Stream.H
# End Source File
# Begin Source File

SOURCE=.\Help_StreamWrapper.H
# End Source File
# Begin Source File

SOURCE=.\Help_Suite.h
# End Source File
# Begin Source File

SOURCE=.\Help_SuiteList.h
# End Source File
# Begin Source File

SOURCE=.\Help_TestCases.h
# End Source File
# Begin Source File

SOURCE=.\Help_VerifyPath.h
# End Source File
# Begin Source File

SOURCE=.\Help_WorstTimes.h
# End Source File
# Begin Source File

SOURCE=.\Helpers.h
# End Source File
# Begin Source File

SOURCE=.\loader8.h
# End Source File
# Begin Source File

SOURCE=.\Logging.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\tests.h
# End Source File
# Begin Source File

SOURCE=.\ToolEcho.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraph.h
# End Source File
# Begin Source File

SOURCE=.\ToolTest.h
# End Source File
# Begin Source File

SOURCE=.\ToolTrace.h
# End Source File
# End Group
# Begin Group "HPP Files"

# PROP Default_Filter ".hpp"
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath1.hpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_GetObjectInPath_Invalid.hpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_SetPitch.hpp
# End Source File
# Begin Source File

SOURCE=.\AudioPath_SetVolume.hpp
# End Source File
# End Group
# Begin Group "BAT files"

# PROP Default_Filter "*.bat,*.cmd"
# Begin Source File

SOURCE=.\bar.bat
# End Source File
# Begin Source File

SOURCE=.\bl.bat
# End Source File
# Begin Source File

SOURCE=.\BuildAndRun.bat
# End Source File
# Begin Source File

SOURCE=.\CopyMedia.bat
# End Source File
# Begin Source File

SOURCE=.\go.bat
# End Source File
# Begin Source File

SOURCE=.\param.cmd
# End Source File
# Begin Source File

SOURCE=.\prop\prop.cmd
# End Source File
# Begin Source File

SOURCE=.\run.bat
# End Source File
# End Group
# Begin Group "TESTINI.INI files"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\TestParams\AllBVT.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllBVTandValid.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllFatal.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllPerf.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllTestsListed.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllTestsTemp.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\AllValid.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_CC.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_Filter.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_ModLFO.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_Pitch.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_Regions.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_VibLFO.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\DLS_Volume.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessBVT.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessBVTLocal.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessFatal.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessPerf.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessValid.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessValidPassing.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\HarnessValidPassingDefault.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\StressBVT.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\StressValid.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\test.txt
# End Source File
# End Group
# Begin Group "Xternal Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\public\sdk\inc\dmerror.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\public\xdk\inc\dmusici.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\genx\directx\dmusic\inc\dmusici.x
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\public\xdk\inc\dsound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\genx\directx\dsound\inc\dsound.x
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\dxconio.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\macros.h
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\medialoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\public\xdk\inc\stdio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\xlog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\public\xdk\inc\xtl.h
# End Source File
# End Group
# Begin Group "LNT Files"

# PROP Default_Filter "*.lnt"
# Begin Source File

SOURCE=C:\LINT\DMTEST1.LNT
# End Source File
# Begin Source File

SOURCE=C:\LINT\DMusic.lnt
# End Source File
# Begin Source File

SOURCE=C:\LINT\MSG.TXT
# End Source File
# Begin Source File

SOURCE=C:\LINT\OPTIONS.LNT
# End Source File
# Begin Source File

SOURCE=C:\LINT\STD.LNT
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
