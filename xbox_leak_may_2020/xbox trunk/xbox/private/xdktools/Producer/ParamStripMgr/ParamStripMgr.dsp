# Microsoft Developer Studio Project File - Name="ParamStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ParamStripMgr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ParamStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ParamStripMgr.mak" CFG="ParamStripMgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ParamStripMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParamStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParamStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/ParamStripMgr", HLJBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ParamStripMgr - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /libpath:"..\Libs\Release"
# Begin Custom Build - Registering DLL...
OutDir=.\Release
TargetPath=.\Release\ParamStripMgr.dll
InputPath=.\Release\ParamStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "ParamStripMgr - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Registering DLL...
OutDir=.\Debug
TargetPath=.\Debug\ParamStripMgr.dll
InputPath=.\Debug\ParamStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "ParamStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ParamStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "ParamStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering DLL...
OutDir=.\BBT
TargetPath=.\BBT\ParamStripMgr.dll
InputPath=.\BBT\ParamStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ENDIF 

# Begin Target

# Name "ParamStripMgr - Win32 Release"
# Name "ParamStripMgr - Win32 Debug"
# Name "ParamStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\BaseMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\BaseStrip.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DLSLoadSaveUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\GridsPerSecondDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc

!IF  "$(CFG)" == "ParamStripMgr - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ParamStripMgr - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ParamStripMgr - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\newparameterdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamStripMgrDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamStripMgrDLL.def
# End Source File
# Begin Source File

SOURCE=.\ParamStripMgrDLL.rc
# End Source File
# Begin Source File

SOURCE=.\PropCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SelectedRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Tracker.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackItem.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackObject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CurvePropPageMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DLSLoadSaveUtils.h
# End Source File
# Begin Source File

SOURCE=.\GridsPerSecondDlg.h
# End Source File
# Begin Source File

SOURCE=.\GroupBitsPPG.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPGresource.h
# End Source File
# Begin Source File

SOURCE=.\newparameterdialog.h
# End Source File
# Begin Source File

SOURCE=.\ParamStrip.h
# End Source File
# Begin Source File

SOURCE=.\ParamStripMgrApp.h
# End Source File
# Begin Source File

SOURCE=.\PropCurve.h
# End Source File
# Begin Source File

SOURCE=.\PropPageCurve.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabBoundary.h
# End Source File
# Begin Source File

SOURCE=.\TabObject.h
# End Source File
# Begin Source File

SOURCE=.\Tracker.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# Begin Source File

SOURCE=.\TrackItem.h
# End Source File
# Begin Source File

SOURCE=.\TrackMgr.h
# End Source File
# Begin Source File

SOURCE=.\TrackObject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\TrackMgr.rgs
# End Source File
# Begin Source File

SOURCE=.\Res\VarBtn_Down_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\VarBtn_Up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\VarBtnInactive_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\VarBtnInActive_Up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\VarGutter_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\VarGutter_Up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ZoomIn_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ZoomIn_Up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ZoomOut_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ZoomOut_Up.bmp
# End Source File
# End Group
# End Target
# End Project
