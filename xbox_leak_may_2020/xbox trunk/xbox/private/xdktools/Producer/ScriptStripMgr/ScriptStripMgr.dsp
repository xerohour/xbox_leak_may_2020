# Microsoft Developer Studio Project File - Name="ScriptStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ScriptStripMgr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScriptStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScriptStripMgr.mak" CFG="ScriptStripMgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScriptStripMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ScriptStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ScriptStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/ScriptStripMgr", YWIBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ScriptStripMgr - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering DLL...
OutDir=.\Release
TargetPath=.\Release\ScriptStripMgr.dll
InputPath=.\Release\ScriptStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "ScriptStripMgr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /GZ /c
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
# Begin Custom Build - Registering DLL...
OutDir=.\Debug
TargetPath=.\Debug\ScriptStripMgr.dll
InputPath=.\Debug\ScriptStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ELSEIF  "$(CFG)" == "ScriptStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ScriptStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "ScriptStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering DLL...
OutDir=.\BBT
TargetPath=.\BBT\ScriptStripMgr.dll
InputPath=.\BBT\ScriptStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" > $(OutDir)\regsvr.tmp

# End Custom Build

!ENDIF 

# Begin Target

# Name "ScriptStripMgr - Win32 Release"
# Name "ScriptStripMgr - Win32 Debug"
# Name "ScriptStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\BaseMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\BaseStrip.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc

!IF  "$(CFG)" == "ScriptStripMgr - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ScriptStripMgr - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ScriptStripMgr - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptStripMgrDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptStripMgrDLL.def
# End Source File
# Begin Source File

SOURCE=.\ScriptStripMgrDLL.rc
# End Source File
# Begin Source File

SOURCE=..\Shared\SelectedRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabRoutine.cpp
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
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Shared\BaseMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPGresource.h
# End Source File
# Begin Source File

SOURCE=.\PropPageMgr_Item.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabRoutine.h
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ScriptMgr.rgs
# End Source File
# End Group
# End Target
# End Project
