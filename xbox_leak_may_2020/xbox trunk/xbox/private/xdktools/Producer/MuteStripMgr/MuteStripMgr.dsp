# Microsoft Developer Studio Project File - Name="MuteStripMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MuteStripMgr - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MuteStripMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MuteStripMgr.mak" CFG="MuteStripMgr - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MuteStripMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MuteStripMgr - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MuteStripMgr - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MuteStripMgr - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/MuteStripMgr", BXEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MuteStripMgr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\MuteStripMgr.dll
TargetName=MuteStripMgr
InputPath=.\Debug\MuteStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 Release MinSize"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "." /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\MuteStripMgr.dll
TargetName=MuteStripMgr
InputPath=.\ReleaseMinSize\MuteStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MuteStripMgr___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "MuteStripMgr___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\Includes" /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib ConductorGUID.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\MuteStripMgr.dll
TargetName=MuteStripMgr
InputPath=.\BoundsChecker\MuteStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MuteStripMgr___Win32_BBT"
# PROP BASE Intermediate_Dir "MuteStripMgr___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "." /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "." /I "..\Includes" /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\MuteStripMgr.dll
TargetName=MuteStripMgr
InputPath=.\BBT\MuteStripMgr.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MuteStripMgr - Win32 Debug"
# Name "MuteStripMgr - Win32 Release MinSize"
# Name "MuteStripMgr - Win32 BoundsChecker"
# Name "MuteStripMgr - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DlgAddPChannel.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.rc

!IF  "$(CFG)" == "MuteStripMgr - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 Release MinSize"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 BoundsChecker"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MuteStripMgr - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MuteItem.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteStripMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MuteStripMgr.def
# End Source File
# Begin Source File

SOURCE=.\MuteStripMgr.rc
# End Source File
# Begin Source File

SOURCE=.\RemapDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DlgAddPChannel.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPG.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GroupBitsPPGresource.h
# End Source File
# Begin Source File

SOURCE=.\MuteItem.h
# End Source File
# Begin Source File

SOURCE=.\MuteMgr.h
# End Source File
# Begin Source File

SOURCE=.\MuteScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\RemapDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackFlagsPPG.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\mutemgr1.bin
# End Source File
# Begin Source File

SOURCE=.\mutemgr2.bin
# End Source File
# End Group
# Begin Source File

SOURCE=.\MuteMgr.rgs
# End Source File
# End Target
# End Project
