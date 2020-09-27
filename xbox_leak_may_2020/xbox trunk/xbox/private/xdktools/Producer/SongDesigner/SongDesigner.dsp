# Microsoft Developer Studio Project File - Name="SongDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SongDesigner - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SongDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SongDesigner.mak" CFG="SongDesigner - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SongDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SongDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/SongDesigner", IVJBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SongDesigner - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\SongDesigner.ocx
TargetName=SongDesigner
InputPath=.\Release\SongDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SongDesigner - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\SongDesigner.ocx
TargetName=SongDesigner
InputPath=.\Debug\SongDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "SongDesigner - Win32 Release"
# Name "SongDesigner - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DlgAddTracks.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetLength.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DupeFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Folder.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderSegments.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderToolGraphs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Song.cpp
# End Source File
# Begin Source File

SOURCE=.\SongComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\SongCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\SongDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\SongDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\SongDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\SongDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SongDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\SongRef.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TabInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSegmentDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSegmentRuntime.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSong.cpp
# End Source File
# Begin Source File

SOURCE=.\TabTransition.cpp
# End Source File
# Begin Source File

SOURCE=.\TabVirtualSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\TabVirtualSegmentLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\Track.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackPropPageObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Transition.cpp
# End Source File
# Begin Source File

SOURCE=.\TransitionPropPageObject.cpp
# End Source File
# Begin Source File

SOURCE=.\VirtualSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\VirtualSegmentPropPageObject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DlgAddTracks.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetLength.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DupeFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\Folder.h
# End Source File
# Begin Source File

SOURCE=.\FolderSegments.h
# End Source File
# Begin Source File

SOURCE=.\FolderToolGraphs.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\Song.h
# End Source File
# Begin Source File

SOURCE=.\SongComponent.h
# End Source File
# Begin Source File

SOURCE=.\SongCtl.h
# End Source File
# Begin Source File

SOURCE=.\SongDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\SongDlg.h
# End Source File
# Begin Source File

SOURCE=.\SongDocType.h
# End Source File
# Begin Source File

SOURCE=.\SongRef.h
# End Source File
# Begin Source File

SOURCE=.\SourceSegment.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.h
# End Source File
# Begin Source File

SOURCE=.\TabInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabSegmentDesign.h
# End Source File
# Begin Source File

SOURCE=.\TabSegmentRuntime.h
# End Source File
# Begin Source File

SOURCE=.\TabSong.h
# End Source File
# Begin Source File

SOURCE=.\TabTransition.h
# End Source File
# Begin Source File

SOURCE=.\TabVirtualSegment.h
# End Source File
# Begin Source File

SOURCE=.\TabVirtualSegmentLoop.h
# End Source File
# Begin Source File

SOURCE=.\Track.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# Begin Source File

SOURCE=.\TrackPropPageObject.h
# End Source File
# Begin Source File

SOURCE=.\Transition.h
# End Source File
# Begin Source File

SOURCE=.\TransitionPropPageObject.h
# End Source File
# Begin Source File

SOURCE=.\VirtualSegment.h
# End Source File
# Begin Source File

SOURCE=.\VirtualSegmentPropPageObject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\AboutDLL.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\SongCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\SongNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\SongNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\SongRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\SongRefNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Transition.bmp
# End Source File
# End Group
# End Target
# End Project
