# Microsoft Developer Studio Project File - Name="BandEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=BandEditor - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BandEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BandEditor.mak" CFG="BandEditor - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BandEditor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BandEditor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BandEditor - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BandEditor - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BandEditor - Win32 XBox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BandEditor - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/Band Editor", HISAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BandEditor - Win32 Release"

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
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /base:"0x8000000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\BandEditor.ocx
TargetName=BandEditor
InputPath=.\Release\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Release"	cd ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BandEditor - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\BandEditor.ocx
TargetName=BandEditor
InputPath=.\Debug\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Debug"	cd ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BandEditor - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BandEditor___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "BandEditor___Win32_BoundsChecker"
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\BandEditor.ocx
TargetName=BandEditor
InputPath=.\BoundsChecker\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Debug"	cd ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BandEditor - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BandEditor___Win32_BBT"
# PROP BASE Intermediate_Dir "BandEditor___Win32_BBT"
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
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /base:"0x8000000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /base:"0x8000000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\BandEditor.ocx
TargetName=BandEditor
InputPath=.\BBT\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Release"	cd ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BandEditor - Win32 XBox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BandEditor___Win32_XBox_Release"
# PROP BASE Intermediate_Dir "BandEditor___Win32_XBox_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XBox_Release"
# PROP Intermediate_Dir "XBox_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /base:"0x8000000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommon.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /base:"0x8000000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\XBox_Release
TargetPath=.\XBox_Release\BandEditor.ocx
TargetName=BandEditor
InputPath=.\XBox_Release\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Release"	cd ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "BandEditor - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BandEditor___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "BandEditor___Win32_Xbox_Debug"
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "DMP_XBOX" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 JazzCommond.lib htmlhelp.lib DMUSProdGuid.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\BandEditor.ocx
TargetName=BandEditor
InputPath=.\Xbox_Debug\BandEditor.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Compile grid control
PreLink_Cmds=cd Grid	MSDEV Grid.dsp /MAKE "Grid - Win32 Debug"	cd ..
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "BandEditor - Win32 Release"
# Name "BandEditor - Win32 Debug"
# Name "BandEditor - Win32 BoundsChecker"
# Name "BandEditor - Win32 BBT"
# Name "BandEditor - Win32 XBox Release"
# Name "BandEditor - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddRemoveDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\AList.cpp
# End Source File
# Begin Source File

SOURCE=.\Band.cpp
# End Source File
# Begin Source File

SOURCE=.\BandAboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\BandComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\BandCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\BandDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BandDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditorDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\BandEditorDLL.def
# End Source File
# Begin Source File

SOURCE=.\BandEditorDLL.rc

!IF  "$(CFG)" == "BandEditor - Win32 Release"

!ELSEIF  "$(CFG)" == "BandEditor - Win32 Debug"

!ELSEIF  "$(CFG)" == "BandEditor - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "BandEditor - Win32 BBT"

!ELSEIF  "$(CFG)" == "BandEditor - Win32 XBox Release"

!ELSEIF  "$(CFG)" == "BandEditor - Win32 Xbox Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BandPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\BandRef.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDLS.cpp
# End Source File
# Begin Source File

SOURCE=.\grid.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Info.cpp
# End Source File
# Begin Source File

SOURCE=.\PChannelList.cpp
# End Source File
# Begin Source File

SOURCE=.\PChannelPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PChannelPropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabBand.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Unknownchunk.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddRemoveDlg.h
# End Source File
# Begin Source File

SOURCE=.\Band.h
# End Source File
# Begin Source File

SOURCE=.\BandAboutBox.h
# End Source File
# Begin Source File

SOURCE=.\BandComponent.h
# End Source File
# Begin Source File

SOURCE=.\BandCtl.h
# End Source File
# Begin Source File

SOURCE=.\BandDlg.h
# End Source File
# Begin Source File

SOURCE=.\BandDocType.h
# End Source File
# Begin Source File

SOURCE=.\BandEditorDLL.h
# End Source File
# Begin Source File

SOURCE=.\BandPpg.h
# End Source File
# Begin Source File

SOURCE=.\BandRef.h
# End Source File
# Begin Source File

SOURCE=.\DlgDLS.h
# End Source File
# Begin Source File

SOURCE=.\grid.h
# End Source File
# Begin Source File

SOURCE=..\Shared\guiddlg.h
# End Source File
# Begin Source File

SOURCE=.\IMABand.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Info.h
# End Source File
# Begin Source File

SOURCE=.\PChannelList.h
# End Source File
# Begin Source File

SOURCE=.\PChannelPropPage.h
# End Source File
# Begin Source File

SOURCE=.\PChannelPropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceMaps.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabBand.h
# End Source File
# Begin Source File

SOURCE=.\UndoMan.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\BandCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BandCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BandDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandDocSel.ico
# End Source File
# Begin Source File

SOURCE=.\BandEditor.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandEditor.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\BandRefNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Folder.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\style_ed.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_bass.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_drums.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_gtr.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_kybd.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_sax.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\U_violn.bmp
# End Source File
# End Group
# End Target
# End Project
# Section BandEditor : {EAB0CD46-9459-11D0-8C10-00A0C92E1CAC}
# 	2:21:DefaultSinkHeaderFile:grid.h
# 	2:16:DefaultSinkClass:CGrid
# End Section
# Section BandEditor : {EAB0CD44-9459-11D0-8C10-00A0C92E1CAC}
# 	2:5:Class:CGrid
# 	2:10:HeaderFile:grid.h
# 	2:8:ImplFile:grid.cpp
# End Section
