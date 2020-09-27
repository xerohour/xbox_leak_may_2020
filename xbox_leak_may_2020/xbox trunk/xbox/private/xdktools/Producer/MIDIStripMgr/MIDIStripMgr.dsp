# Microsoft Developer Studio Project File - Name="MIDIStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MIDIStripMgr - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MIDIStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MIDIStripMgr.mak" CFG="MIDIStripMgr - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MIDIStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MIDIStripMgr - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MIDIStripMgr - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MIDIStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/MIDIStripMgr", UYZAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MIDIStripMgr - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../Includes" /I "." /I "../Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib winmm.lib DMUSProdGUIDPrivate.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\MIDIStripMgr.dll
TargetName=MIDIStripMgr
InputPath=.\Debug\MIDIStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../Includes" /I "." /I "../Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib winmm.lib DMUSProdGUIDPrivate.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\libs\release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\MIDIStripMgr.dll
TargetName=MIDIStripMgr
InputPath=.\ReleaseMinSize\MIDIStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MIDIStripMgr___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "MIDIStripMgr___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Includes" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../Includes" /I "." /I "../Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\debug\JazzCommond.lib winmm.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib winmm.lib DMUSProdGUIDPrivate.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\MIDIStripMgr.dll
TargetName=MIDIStripMgr
InputPath=.\BoundsChecker\MIDIStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MIDIStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "MIDIStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../Includes" /I "." /I "../Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../Includes" /I "." /I "../Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\release\JazzCommon.lib winmm.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC"
# ADD LINK32 JazzCommon.lib winmm.lib DMUSProdGUIDPrivate.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\libs\release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\MIDIStripMgr.dll
TargetName=MIDIStripMgr
InputPath=.\BBT\MIDIStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MIDIStripMgr - Win32 Debug"
# Name "MIDIStripMgr - Win32 Release MinSize"
# Name "MIDIStripMgr - Win32 BoundsChecker"
# Name "MIDIStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\AList.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveIO.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogLinkExisting.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogNewCCTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogNewPart.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogSelectPartRef.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogVelocity.cpp
# End Source File
# Begin Source File

SOURCE=.\EventItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Shared\MidiFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\midifileIO2.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIStripMgr.def
# End Source File
# Begin Source File

SOURCE=.\MIDIStripMgr.rc

!IF  "$(CFG)" == "MIDIStripMgr - Win32 Debug"

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "MIDIStripMgr - Win32 BBT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NoteTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\PartLengthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\PatternLengthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PianoRollScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PianoRollStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\PropCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\PropNote.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveReset.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveVar.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageNote.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageNoteVar.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPianoRoll.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSwitchPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\QuantizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RectList.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPattern.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPianoRoll.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPropPageCurve.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPropPageNote.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabPatternPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSignatureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tracker.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\VarSwitchStrip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Shared\alist.h
# End Source File
# Begin Source File

SOURCE=.\ChordTrack.h
# End Source File
# Begin Source File

SOURCE=.\CurveIO.h
# End Source File
# Begin Source File

SOURCE=.\CurveStrip.h
# End Source File
# Begin Source File

SOURCE=.\DialogLinkExisting.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogNewCCTrack.h
# End Source File
# Begin Source File

SOURCE=.\DialogNewPart.h
# End Source File
# Begin Source File

SOURCE=.\DialogSelectPartRef.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogVelocity.h
# End Source File
# Begin Source File

SOURCE=.\EventItem.h
# End Source File
# Begin Source File

SOURCE=.\GroupBitsPPG.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPGresource.h
# End Source File
# Begin Source File

SOURCE=..\Includes\ioDMStyle.h
# End Source File
# Begin Source File

SOURCE=..\Shared\LockoutNotification.h
# End Source File
# Begin Source File

SOURCE=.\MIDI.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MidiFileIO.h
# End Source File
# Begin Source File

SOURCE=.\MIDIMgr.h
# End Source File
# Begin Source File

SOURCE=.\MIDIStripMgrApp.h
# End Source File
# Begin Source File

SOURCE=.\NoteTracker.h
# End Source File
# Begin Source File

SOURCE=.\PartLengthDlg.h
# End Source File
# Begin Source File

SOURCE=.\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\PatternLengthDlg.h
# End Source File
# Begin Source File

SOURCE=.\PianoRollScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\PropCurve.h
# End Source File
# Begin Source File

SOURCE=.\propnote.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveReset.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveVar.h
# End Source File
# Begin Source File

SOURCE=.\PropPageMarker.h
# End Source File
# Begin Source File

SOURCE=.\PropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\PropPageNote.h
# End Source File
# Begin Source File

SOURCE=.\PropPageNoteVar.h
# End Source File
# Begin Source File

SOURCE=.\PropPianoRoll.h
# End Source File
# Begin Source File

SOURCE=.\PropSwitchPoint.h
# End Source File
# Begin Source File

SOURCE=.\QuantizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\RectList.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\SequenceIO.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPattern.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPianoRoll.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabPatternPattern.h
# End Source File
# Begin Source File

SOURCE=.\TimeSignatureDlg.h
# End Source File
# Begin Source File

SOURCE=.\Tracker.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# Begin Source File

SOURCE=.\VarSwitchStrip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\blank_bu.bmp
# End Source File
# Begin Source File

SOURCE=.\blank_in.bmp
# End Source File
# Begin Source File

SOURCE=.\res\disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\endedit.cur
# End Source File
# Begin Source File

SOURCE=.\res\gpb1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb1_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb2_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb3_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb4_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb6_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb8_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb21.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb23.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb24.bmp
# End Source File
# Begin Source File

SOURCE=.\gutter_b.bmp
# End Source File
# Begin Source File

SOURCE=.\gutter_p.bmp
# End Source File
# Begin Source File

SOURCE=.\Momdown.BMP
# End Source File
# Begin Source File

SOURCE=.\Momup.BMP
# End Source File
# Begin Source File

SOURCE=.\move.cur
# End Source File
# Begin Source File

SOURCE=.\Res\newband.bmp
# End Source File
# Begin Source File

SOURCE=.\pressed_.bmp
# End Source File
# Begin Source File

SOURCE=.\pressed_in.bmp
# End Source File
# Begin Source File

SOURCE=.\startedit.cur
# End Source File
# Begin Source File

SOURCE=.\zoomin_down.bmp
# End Source File
# Begin Source File

SOURCE=.\zoomin_up.bmp
# End Source File
# Begin Source File

SOURCE=.\zoomout_down.bmp
# End Source File
# Begin Source File

SOURCE=.\zoomout_up.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\MIDIMgr.rgs
# End Source File
# End Target
# End Project
