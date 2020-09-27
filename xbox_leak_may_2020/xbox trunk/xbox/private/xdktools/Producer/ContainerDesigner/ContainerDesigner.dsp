# Microsoft Developer Studio Project File - Name="ContainerDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ContainerDesigner - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ContainerDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ContainerDesigner.mak" CFG="ContainerDesigner - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ContainerDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ContainerDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ContainerDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ContainerDesigner - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\ContainerDesigner.ocx
TargetName=ContainerDesigner
InputPath=.\Release\ContainerDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ContainerDesigner - Win32 Debug"

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
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\ContainerDesigner.ocx
TargetName=ContainerDesigner
InputPath=.\Debug\ContainerDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ContainerDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ContainerDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "ContainerDesigner___Win32_BBT"
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\ContainerDesigner.ocx
TargetName=ContainerDesigner
InputPath=.\BBT\ContainerDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ContainerDesigner - Win32 Release"
# Name "ContainerDesigner - Win32 Debug"
# Name "ContainerDesigner - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Container.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\ContainerDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\ContainerDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\ContainerRef.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAddFiles.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DupeFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Folder.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\TabInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabObjectDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\TabObjectFlags.cpp
# End Source File
# Begin Source File

SOURCE=.\TabObjectRuntime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Container.h
# End Source File
# Begin Source File

SOURCE=.\ContainerComponent.h
# End Source File
# Begin Source File

SOURCE=.\ContainerCtl.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\ContainerDocType.h
# End Source File
# Begin Source File

SOURCE=.\ContainerPpg.h
# End Source File
# Begin Source File

SOURCE=.\ContainerRef.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddFiles.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DupeFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\Folder.h
# End Source File
# Begin Source File

SOURCE=.\Object.h
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

SOURCE=.\TabContainer.h
# End Source File
# Begin Source File

SOURCE=.\TabInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabObjectDesign.h
# End Source File
# Begin Source File

SOURCE=.\TabObjectFlags.h
# End Source File
# Begin Source File

SOURCE=.\TabObjectRuntime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\containe.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerDesigner.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ContainerRefNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\DisabledFolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\DisabledFolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNodeSel.ico
# End Source File
# End Group
# End Target
# End Project
