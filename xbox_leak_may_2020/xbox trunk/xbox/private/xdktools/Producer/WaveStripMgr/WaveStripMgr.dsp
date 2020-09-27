# Microsoft Developer Studio Project File - Name="WaveStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WaveStripMgr - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WaveStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WaveStripMgr.mak" CFG="WaveStripMgr - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WaveStripMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WaveStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WaveStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WaveStripMgr - Win32 Xbox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WaveStripMgr - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/WaveStripMgr", EFJBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WaveStripMgr - Win32 Release"

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
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering DLL...
OutDir=.\Release
TargetPath=.\Release\WaveStripMgr.dll
InputPath=.\Release\WaveStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering DLL...
OutDir=.\Debug
TargetPath=.\Debug\WaveStripMgr.dll
InputPath=.\Debug\WaveStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WaveStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "WaveStripMgr___Win32_BBT"
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
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering DLL...
OutDir=.\BBT
TargetPath=.\BBT\WaveStripMgr.dll
InputPath=.\BBT\WaveStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Xbox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WaveStripMgr___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "WaveStripMgr___Win32_Xbox_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering DLL...
OutDir=.\Xbox_Release
TargetPath=.\Xbox_Release\WaveStripMgr.dll
InputPath=.\Xbox_Release\WaveStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WaveStripMgr___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "WaveStripMgr___Win32_Xbox_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "DMP_XBOX" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering DLL...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\WaveStripMgr.dll
InputPath=.\Xbox_Debug\WaveStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ENDIF 

# Begin Target

# Name "WaveStripMgr - Win32 Release"
# Name "WaveStripMgr - Win32 Debug"
# Name "WaveStripMgr - Win32 BBT"
# Name "WaveStripMgr - Win32 Xbox Release"
# Name "WaveStripMgr - Win32 Xbox Debug"
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

!IF  "$(CFG)" == "WaveStripMgr - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveStripMgr - Win32 Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayerScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\myslider.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPartDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PropTrackItem.cpp
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

SOURCE=.\TabFileRef.cpp
# End Source File
# Begin Source File

SOURCE=.\TabLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\TabPerformance.cpp
# End Source File
# Begin Source File

SOURCE=.\TabVariations.cpp
# End Source File
# Begin Source File

SOURCE=.\TabWavePart.cpp
# End Source File
# Begin Source File

SOURCE=.\TabWaveTrack.cpp
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

SOURCE=.\WaveStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveStripLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveStripMgrDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveStripMgrDLL.def
# End Source File
# Begin Source File

SOURCE=.\WaveStripMgrDLL.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=.\LayerScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\Shared\myslider.h
# End Source File
# Begin Source File

SOURCE=.\NewPartDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropPageMgr_Item.h
# End Source File
# Begin Source File

SOURCE=.\PropTrackItem.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabFileRef.h
# End Source File
# Begin Source File

SOURCE=.\TabLoop.h
# End Source File
# Begin Source File

SOURCE=.\TabPerformance.h
# End Source File
# Begin Source File

SOURCE=.\TabVariations.h
# End Source File
# Begin Source File

SOURCE=.\TabWavePart.h
# End Source File
# Begin Source File

SOURCE=.\TabWaveTrack.h
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

SOURCE=.\WaveStripLayer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Lock16.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Locked.bmp
# End Source File
# Begin Source File

SOURCE=.\TrackMgr.rgs
# End Source File
# Begin Source File

SOURCE=.\Res\Unlocked.bmp
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
