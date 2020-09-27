# Microsoft Developer Studio Project File - Name="SequenceStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SequenceStripMgr - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SequenceStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SequenceStripMgr.mak" CFG="SequenceStripMgr - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SequenceStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SequenceStripMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SequenceStripMgr - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SequenceStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/SequenceStripMgr", KYEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SequenceStripMgr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
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
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\SequenceStripMgr.dll
TargetName=SequenceStripMgr
InputPath=.\Debug\SequenceStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\libs\release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\SequenceStripMgr.dll
TargetName=SequenceStripMgr
InputPath=.\Release\SequenceStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SequenceStripMgr___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "SequenceStripMgr___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\debug\JazzCommond.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\SequenceStripMgr.dll
TargetName=SequenceStripMgr
InputPath=.\BoundsChecker\SequenceStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SequenceStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "SequenceStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\libs\release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\libs\release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\SequenceStripMgr.dll
TargetName=SequenceStripMgr
InputPath=.\BBT\SequenceStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "SequenceStripMgr - Win32 Debug"
# Name "SequenceStripMgr - Win32 Release"
# Name "SequenceStripMgr - Win32 BoundsChecker"
# Name "SequenceStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CurveIO.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveStrip.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogNewCCTrack.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogVelocity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExtraLength.cpp
# End Source File
# Begin Source File

SOURCE=.\EventItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc

!IF  "$(CFG)" == "SequenceStripMgr - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 BoundsChecker"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SequenceStripMgr - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\MidiFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\midifileIO2.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPartDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NotePropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\NoteTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\PropCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\PropItem.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveReset.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageNote.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageSeqTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSequence.cpp
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

SOURCE=.\SequenceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceStripMgr.def
# End Source File
# Begin Source File

SOURCE=.\SequenceStripMgr.rc
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

SOURCE=.\Tracker.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CurveIO.h
# End Source File
# Begin Source File

SOURCE=.\CurvePropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\CurveStrip.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogNewCCTrack.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DialogVelocity.h
# End Source File
# Begin Source File

SOURCE=.\DlgExtraLength.h
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

SOURCE=..\Shared\LockoutNotification.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MidiFileIO.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MusicTimeConverter.h
# End Source File
# Begin Source File

SOURCE=.\NewPartDlg.h
# End Source File
# Begin Source File

SOURCE=.\NotePropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\NoteTracker.h
# End Source File
# Begin Source File

SOURCE=.\Pre388_dmusicf.h
# End Source File
# Begin Source File

SOURCE=.\PropCurve.h
# End Source File
# Begin Source File

SOURCE=.\PropItem.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurveReset.h
# End Source File
# Begin Source File

SOURCE=.\PropPageNote.h
# End Source File
# Begin Source File

SOURCE=.\PropPageSeqTrack.h
# End Source File
# Begin Source File

SOURCE=.\PropSequence.h
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

SOURCE=.\SequenceMgr.h
# End Source File
# Begin Source File

SOURCE=.\SequenceScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\SequenceStripMgrApp.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPianoRoll.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Tracker.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\endedit.cur
# End Source File
# Begin Source File

SOURCE=.\Res\gpb1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb1_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb2_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb3_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb4_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb6.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb6_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb7.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb8.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb8_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpb9.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba10.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba11.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba12.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba13.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba14.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba15.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba16.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba17.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba18.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpba19.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpbb20.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpbb21.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpbb22.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpbb23.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gpbb24.bmp
# End Source File
# Begin Source File

SOURCE=.\move.cur
# End Source File
# Begin Source File

SOURCE=.\Res\newband.bmp
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

SOURCE=.\SequenceMgr.rgs
# End Source File
# End Target
# End Project
# Section SequenceStripMgr : {CDD09F86-E73C-11D0-89AB-00A0C9054129}
# 	2:21:DefaultSinkHeaderFile:keysocx.h
# 	2:16:DefaultSinkClass:CKeysOCX
# End Section
# Section SequenceStripMgr : {CDD09F84-E73C-11D0-89AB-00A0C9054129}
# 	2:5:Class:CKeysOCX
# 	2:10:HeaderFile:keysocx.h
# 	2:8:ImplFile:keysocx.cpp
# End Section
