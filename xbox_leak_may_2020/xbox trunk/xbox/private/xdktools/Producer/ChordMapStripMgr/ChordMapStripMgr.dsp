# Microsoft Developer Studio Project File - Name="ChordMapStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ChordMapStripMgr - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ChordMapStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ChordMapStripMgr.mak" CFG="ChordMapStripMgr - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChordMapStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ChordMapStripMgr - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ChordMapStripMgr - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ChordMapStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/ChordMapStripMgr", UFYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ChordMapStripMgr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\ChordMapStripMgr.dll
TargetName=ChordMapStripMgr
InputPath=.\Debug\ChordMapStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ChordMapStripMgr - Win32 Release MinSize"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
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
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\libs\release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\ChordMapStripMgr.dll
TargetName=ChordMapStripMgr
InputPath=.\ReleaseMinSize\ChordMapStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ChordMapStripMgr - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ChordMapStripMgr___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "ChordMapStripMgr___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes\\" /D "_DEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\debug\JazzCommond.lib ..\libs\debug\ConductorGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept /libpath:"..\libs\debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\ChordMapStripMgr.dll
TargetName=ChordMapStripMgr
InputPath=.\BoundsChecker\ChordMapStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ChordMapStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ChordMapStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "ChordMapStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "CHORDMAP" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\libs\release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\libs\release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\ChordMapStripMgr.dll
TargetName=ChordMapStripMgr
InputPath=.\BBT\ChordMapStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ChordMapStripMgr - Win32 Debug"
# Name "ChordMapStripMgr - Win32 Release MinSize"
# Name "ChordMapStripMgr - Win32 BoundsChecker"
# Name "ChordMapStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CConnectionPropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Chord.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordInversionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordIO.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordMapMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordMapStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordMapStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordMapStripMgr.def
# End Source File
# Begin Source File

SOURCE=.\ChordMapStripMgr.rc
# End Source File
# Begin Source File

SOURCE=.\ChordScalePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ContextMenuHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\DWList.cpp
# End Source File
# Begin Source File

SOURCE=.\IllegalChord.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\InvertVector.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\PropChord.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Chord.h
# End Source File
# Begin Source File

SOURCE=.\ChordDatabase.h
# End Source File
# Begin Source File

SOURCE=.\ChordInversionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ChordIO.h
# End Source File
# Begin Source File

SOURCE=.\ChordMapMgr.h
# End Source File
# Begin Source File

SOURCE=.\chordmapriff.h
# End Source File
# Begin Source File

SOURCE=.\ChordScalePropPage.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionPropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\ContextMenuHandler.h
# End Source File
# Begin Source File

SOURCE=.\DWList.h
# End Source File
# Begin Source File

SOURCE=.\FileIO.h
# End Source File
# Begin Source File

SOURCE=.\IllegalChord.h
# End Source File
# Begin Source File

SOURCE=..\shared\InvertVector.h
# End Source File
# Begin Source File

SOURCE=..\shared\Keyboard.h
# End Source File
# Begin Source File

SOURCE=.\PropChord.h
# End Source File
# Begin Source File

SOURCE=.\proppagemgr.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\chord_mo.cur
# End Source File
# Begin Source File

SOURCE=.\Res\chord_mo.cur
# End Source File
# Begin Source File

SOURCE=.\ChordMove.cur
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\gosign.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\gosign.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\invertd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\invertd.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\invertf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\invertf.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\invertu.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\invertu.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\midiplug.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\midiplugd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\midiplugf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\midiplugu.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\playd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\playf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\playu.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\selgosign.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\selstopsign.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\shiftld.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftld.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\shiftlf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftlf.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\shiftlu.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftlu.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\shiftrd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftrd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftrf.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\shiftru.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftru.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\stopsign.bmp
# End Source File
# Begin Source File

SOURCE=.\stopsign.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChordMapMgr.rgs
# End Source File
# End Target
# End Project
# Section ChordMapStripMgr : {CDD09F86-E73C-11D0-89AB-00A0C9054129}
# 	2:21:DefaultSinkHeaderFile:keysocx1.h
# 	2:16:DefaultSinkClass:CKeysOCX1
# End Section
# Section ChordMapStripMgr : {CDD09F84-E73C-11D0-89AB-00A0C9054129}
# 	2:5:Class:CKeysOCX1
# 	2:10:HeaderFile:keysocx1.h
# 	2:8:ImplFile:keysocx1.cpp
# End Section
