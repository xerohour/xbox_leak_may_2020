# Microsoft Developer Studio Project File - Name="SegmentDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SegmentDesigner - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SegmentDesignerDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SegmentDesignerDLL.mak" CFG="SegmentDesigner - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SegmentDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SegmentDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SegmentDesigner - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SegmentDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SegmentDesigner - Win32 Xbox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SegmentDesigner - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/SegmentDesigner", CXWAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SegmentDesigner - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /out:"Release/SegmentDesigner.ocx" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\Release\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"Debug/SegmentDesigner.ocx" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\Debug\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SegmentDesigner___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "SegmentDesigner___Win32_BoundsChecker"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "." /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"Debug/SegmentDesigner.ocx" /pdbtype:sept /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"BoundsChecker/SegmentDesigner.ocx" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\BoundsChecker\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SegmentDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "SegmentDesigner___Win32_BBT"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /out:"Release/SegmentDesigner.ocx" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /out:"BBT/SegmentDesigner.ocx" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\BBT\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SegmentDesigner___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "SegmentDesigner___Win32_Xbox_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /out:"Release/SegmentDesigner.ocx" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /out:"Xbox_Release/SegmentDesigner.ocx" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Release
TargetPath=.\Xbox_Release\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\Xbox_Release\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SegmentDesigner___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "SegmentDesigner___Win32_Xbox_Debug"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "DMP_XBOX" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"Debug/SegmentDesigner.ocx" /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"Xbox_Debug/SegmentDesigner.ocx" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\SegmentDesigner.ocx
TargetName=SegmentDesigner
InputPath=.\Xbox_Debug\SegmentDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "SegmentDesigner - Win32 Release"
# Name "SegmentDesigner - Win32 Debug"
# Name "SegmentDesigner - Win32 BoundsChecker"
# Name "SegmentDesigner - Win32 BBT"
# Name "SegmentDesigner - Win32 Xbox Release"
# Name "SegmentDesigner - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddTrackDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordList.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandList.cpp
# End Source File
# Begin Source File

SOURCE=.\FileStructs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc

!IF  "$(CFG)" == "SegmentDesigner - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BoundsChecker"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\LoopPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteList.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentAboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\SegmentDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\SegmentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentLength.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentPPGMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentRef.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentRiff.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentSeq.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StyleRef.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.rc

!IF  "$(CFG)" == "SegmentDesigner - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BoundsChecker"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SegmentDesigner - Win32 Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TemplateDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\Track.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\UnknownStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\UnknownStripMgr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddTrackDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\alist.h
# End Source File
# Begin Source File

SOURCE=.\ChordList.h
# End Source File
# Begin Source File

SOURCE=.\CommandList.h
# End Source File
# Begin Source File

SOURCE=.\FileStructs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPGresource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\guiddlg.h
# End Source File
# Begin Source File

SOURCE=.\InfoPPG.h
# End Source File
# Begin Source File

SOURCE=.\LoopPPG.h
# End Source File
# Begin Source File

SOURCE=.\MuteList.h
# End Source File
# Begin Source File

SOURCE=.\Pre388_dmusicf.h
# End Source File
# Begin Source File

SOURCE=.\ProxyStripMgr.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\RiffStructs.h
# End Source File
# Begin Source File

SOURCE=.\Segment.h
# End Source File
# Begin Source File

SOURCE=.\SegmentAboutBox.h
# End Source File
# Begin Source File

SOURCE=.\SegmentComponent.h
# End Source File
# Begin Source File

SOURCE=.\SegmentCtl.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDlg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDocType.h
# End Source File
# Begin Source File

SOURCE=..\Includes\SegmentGuids.h
# End Source File
# Begin Source File

SOURCE=.\SegmentLength.h
# End Source File
# Begin Source File

SOURCE=.\SegmentPpg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentPPGMgr.h
# End Source File
# Begin Source File

SOURCE=.\SegmentRef.h
# End Source File
# Begin Source File

SOURCE=.\SegmentRiff.h
# End Source File
# Begin Source File

SOURCE=.\SignPostIO.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StyleRef.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlagsResource.h
# End Source File
# Begin Source File

SOURCE=.\TemplateDocType.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Templates.h
# End Source File
# Begin Source File

SOURCE=.\Track.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# Begin Source File

SOURCE=.\UndoMan.h
# End Source File
# Begin Source File

SOURCE=.\UnknownStripMgr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\SegmentCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\SegmentDesignerDLL.ico
# End Source File
# Begin Source File

SOURCE=.\res\SegmentNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\SegmentNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\SegmentRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\SegmentRefNodeSel.ico
# End Source File
# End Group
# End Target
# End Project
