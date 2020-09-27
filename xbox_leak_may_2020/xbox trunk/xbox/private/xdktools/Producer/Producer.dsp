# Microsoft Developer Studio Project File - Name="Producer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Producer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Producer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Producer.mak" CFG="Producer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Producer - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Producer - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Producer"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Producer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Producer.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Producer.exe"
# PROP BASE Bsc_Name "Producer.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "E:\WINDOWS\system32\cmd.exe /k C:\nt\tools\razzle.cmd free  exec build -ze"
# PROP Rebuild_Opt ""
# PROP Target_File "DMUSProd.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Producer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Producer.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Producer.exe"
# PROP BASE Bsc_Name "Producer.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "E:\WINDOWS\system32\cmd.exe /k C:\nt\tools\razzle.cmd no_opt exec build -ze"
# PROP Rebuild_Opt ""
# PROP Target_File "DMUSProd.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Producer - Win32 Release"
# Name "Producer - Win32 Debug"

!IF  "$(CFG)" == "Producer - Win32 Release"

!ELSEIF  "$(CFG)" == "Producer - Win32 Debug"

!ENDIF 

# Begin Group "BandEditor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BandEditor\AddRemoveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\AddRemoveDlg.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\Band.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\Band.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandAboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandAboutBox.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandComponent.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandCtl.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandDlg.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandDocType.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditor.dsp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditorDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditorDLL.def
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditorDLL.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditorDLL.rc
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandEditorGuids.c
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandPpg.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandRef.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\BandRef.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\DlgDLS.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\DlgDLS.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\grid.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\IMABand.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\makefile
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelList.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelList.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelPropPage.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelPropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\PChannelPropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\resource.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\ResourceMaps.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\sources
# End Source File
# Begin Source File

SOURCE=.\BandEditor\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\TabBand.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\TabBand.h
# End Source File
# Begin Source File

SOURCE=.\BandEditor\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditor\UndoMan.h
# End Source File
# End Group
# Begin Group "ContainerDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ContainerDesigner\Container.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\Container.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerComponent.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerCtl.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerDocType.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerEditorGuids.c
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerPpg.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerRef.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\ContainerRef.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\DlgAddFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\DlgAddFiles.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\Folder.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\Folder.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\Object.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabContainer.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabInfo.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectDesign.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectFlags.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectFlags.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectRuntime.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesigner\TabObjectRuntime.h
# End Source File
# End Group
# Begin Group "DLSDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLSDesigner\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\AdpcmLoopWarnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\AdpcmLoopWarnDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\adsrenvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\adsrenvelope.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Articulation.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Articulation.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationList.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationList.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationListPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationListPropPage.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationListPropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationListPropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ArticulationTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Collection.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Collection.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionExtraPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionExtraPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionInstruments.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionInstruments.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionRef.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionRef.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionWaves.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\CollectionWaves.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ConditionalChunk.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ConditionalChunk.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ConditionConfigEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\conditionconfigeditor.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\conditionconfigtoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\conditionconfigtoolbar.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ConditionEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ConditionEditor.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSComponent.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DlsDefsPlus.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDesigner.def
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDesigner.rc
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSDocType.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSEdit.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DLSStatic.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\DMSynthError.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\FilterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\filterdialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\GMStrings.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\guiddlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Guids.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InsertSilenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InsertSilenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Instrument.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Instrument.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentCtl.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentFVEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentFVEditor.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentRegions.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\InstrumentRegions.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\KeyBoardMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\KeyBoardMap.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\LFODialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\LFODialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\MonoWave.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\MonoWave.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\NameUnknownQueryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\NameUnknownQueryDialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\newwavedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\newwavedialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\PatchConflictDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\PatchConflictDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\PitchDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\PitchDialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ProgressDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ProgressDialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Region.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Region.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionExtraPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionExtraPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\regionkeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\regionkeyboard.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\RegionPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ResampleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ResampleDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ScriptEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\ScriptEdit.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\StereoWave.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\StereoWave.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\stream.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\UndoMan.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\UserPatchConflictDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\UserPatchConflictDlg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\VibratoLFODialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\vibratolfodialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\VolDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\VolDialog.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\Wave.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCompressionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCompressionManager.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCompressionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCompressionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveCtl.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDataManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDataManager.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDelta.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDelta.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveDocType.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveInfoPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveInfoPropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveNode.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveNode.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WavePropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WavePropPg.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WavePropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WavePropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveRefNode.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveRefNode.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveSourceFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveSourceFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveStream.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveStream.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveTempFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveTempFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveUndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner\WaveUndoManager.h
# End Source File
# End Group
# Begin Group "PersonalityDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PersonalityDesigner\Chord.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\Chord.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordBuilder.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordDatabase.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordDialog.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordInversionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordInversionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordIO.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordIO.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordScalePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ChordScalePropPage.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\DlgChangeLength.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\DlgChangeLength.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\DWList.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\DWList.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\font.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\font.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\IllegalChord.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\IllegalChord.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ImportantLocalizationNote.txt
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\keysocx.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\keysocx.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\keysocx1.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\keysocx1.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\msflexgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\msflexgrid.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\Personality.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\Personality.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityAbout.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityComponent.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityCtl.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDesigner.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDesigner.def
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDesigner.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDesigner.rc
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDlg.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityDocType.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityEditorGuids.c
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityPpg.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityRef.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityRef.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityRIFF.asp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PersonalityRIFF.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\picture.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropChord.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropChord.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropPageChord.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropPageChord.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\PropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\RiffStructs.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\rowcursor.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\rowcursor.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\SignPostDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\SignPostDialog.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabAudition.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabAudition.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabChordPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabChordPalette.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabPersonality.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabPersonality.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabPersonalityInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner\TabPersonalityInfo.h
# End Source File
# End Group
# Begin Group "ScriptDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ScriptDesigner\DebugDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\DebugDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\EditSource.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\EditSource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\EditValue.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\EditValue.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\LogTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\LogTool.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\Script.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\Script.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptComponent.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptCtl.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptDocType.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptEditorGuids.c
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptPpg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptRef.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\ScriptRef.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\TabScriptInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\TabScriptInfo.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\TabScriptScript.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDesigner\TabScriptScript.h
# End Source File
# End Group
# Begin Group "SegmentDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SegmentDesigner\AddTrackDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\AddTrackDlg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\ChordList.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\ChordList.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\CommandList.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\CommandList.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\FileStructs.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\FileStructs.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\InfoPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\InfoPPG.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\LoopPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\LoopPPG.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\MIDIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\MuteList.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\MuteList.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\Pre388_dmusicf.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\ProxyStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\ProxyStripMgr.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\RiffStructs.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\Segment.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\Segment.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentAboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentAboutBox.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentComponent.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentCtl.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.dsp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.ico
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDesignerGuids.c
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDlg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentDocType.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentLength.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentLength.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentPpg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentPPGMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentPPGMgr.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentRef.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentRef.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentRiff.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentRiff.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SegmentSeq.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\SignPostIO.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\StyleRef.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\StyleRef.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\TemplateDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\TemplateDocType.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\Track.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\Track.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\UndoMan.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\UnknownStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\UnknownStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesigner\UnknownStripMgr.h
# End Source File
# End Group
# Begin Group "StyleDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StyleDesigner\DialogLinkExisting.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\DialogLinkExisting.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\MIDIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\MIDIFile.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\NewPatternDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\NewPatternDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternCtl.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternLengthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\PatternLengthDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Personality.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Personality.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\RhythmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\RhythmDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\RiffStructs.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Style.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\Style.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleBands.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleBands.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleComponent.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleCtl.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleDocType.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleEditorGuids.c
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleMotifs.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleMotifs.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StylePatterns.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StylePatterns.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StylePersonalities.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StylePersonalities.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleRef.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\StyleRef.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\styleriff.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\styleriff.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabMotifLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabMotifLoop.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabMotifMotif.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabMotifMotif.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabPatternPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabPatternPattern.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabStyleInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabStyleInfo.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabStyleStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabStyleStyle.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabVarChoices.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TabVarChoices.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TimeSignatureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\TimeSignatureDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoices.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoices.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoicesCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoicesCtl.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoicesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesigner\VarChoicesDlg.h
# End Source File
# End Group
# Begin Group "ToolGraphDesigner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ToolGraphDesigner\Graph.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\Graph.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphComponent.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphCtl.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphDocType.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphRef.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\GraphRef.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\makefile
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\PChannelGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\PChannelGroup.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\resource.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\sources
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabGraph.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabInfo.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\TabTool.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\Tool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\Tool.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphDesigner.dsp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\ToolGraphDesigner\ToolGraphEditorGuids.c
# End Source File
# End Group
# Begin Group "WaveSaveDMO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WaveSaveDMO\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\debug.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\dsdmobse.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\dsdmobse.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\dump.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\dumpp.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\guid.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\makefile
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\oledll.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\oledll.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\resource.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\sources
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\testdmo.def
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\testdmo.h
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\testdmo.rc
# End Source File
# Begin Source File

SOURCE=.\WaveSaveDMO\WaveSaveDmo.dsp
# End Source File
# End Group
# End Target
# End Project
