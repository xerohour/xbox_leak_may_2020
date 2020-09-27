# Microsoft Developer Studio Project File - Name="StyleDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=StyleDesigner - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "StyleDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StyleDesigner.mak" CFG="StyleDesigner - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StyleDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "StyleDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "StyleDesigner - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "StyleDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/Style Designer", JELAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StyleDesigner - Win32 Release"

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
# SUBTRACT LINK32 /incremental:yes
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\StyleDesigner.ocx
TargetName=StyleDesigner
InputPath=.\Release\StyleDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\StyleDesigner.ocx
TargetName=StyleDesigner
InputPath=.\Debug\StyleDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "StyleDesigner___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "StyleDesigner___Win32_BoundsChecker"
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib version.lib ConductorGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT BASE LINK32 /pdb:none /incremental:no
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\StyleDesigner.ocx
TargetName=StyleDesigner
InputPath=.\BoundsChecker\StyleDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "StyleDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "StyleDesigner___Win32_BBT"
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
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\StyleDesigner.ocx
TargetName=StyleDesigner
InputPath=.\BBT\StyleDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "StyleDesigner - Win32 Release"
# Name "StyleDesigner - Win32 Debug"
# Name "StyleDesigner - Win32 BoundsChecker"
# Name "StyleDesigner - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\AList.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogLinkExisting.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPatternDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\PatternCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\PatternDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatternLengthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Personality.cpp
# End Source File
# Begin Source File

SOURCE=.\RhythmDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Style.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleBands.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\StyleDesignerDLL.rc

!IF  "$(CFG)" == "StyleDesigner - Win32 Release"

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 Debug"

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BBT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StyleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleMotifs.cpp
# End Source File
# Begin Source File

SOURCE=.\StylePatterns.cpp
# End Source File
# Begin Source File

SOURCE=.\StylePersonalities.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleRef.cpp
# End Source File
# Begin Source File

SOURCE=.\styleriff.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.rc

!IF  "$(CFG)" == "StyleDesigner - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BoundsChecker"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "StyleDesigner - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TabMotifLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\TabMotifMotif.cpp
# End Source File
# Begin Source File

SOURCE=.\TabPatternPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\TabStyleInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabStyleStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\TabVarChoices.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSignatureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VarChoices.cpp
# End Source File
# Begin Source File

SOURCE=.\VarChoicesCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\VarChoicesDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;inl"
# Begin Source File

SOURCE=..\Shared\alist.h
# End Source File
# Begin Source File

SOURCE=.\DialogLinkExisting.h
# End Source File
# Begin Source File

SOURCE=..\Shared\guiddlg.h
# End Source File
# Begin Source File

SOURCE=..\Includes\ioDMStyle.h
# End Source File
# Begin Source File

SOURCE=.\MIDIFile.h
# End Source File
# Begin Source File

SOURCE=.\NewPatternDlg.h
# End Source File
# Begin Source File

SOURCE=.\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\PatternCtl.h
# End Source File
# Begin Source File

SOURCE=.\PatternDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatternLengthDlg.h
# End Source File
# Begin Source File

SOURCE=..\Includes\PChannelName.h
# End Source File
# Begin Source File

SOURCE=.\Personality.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RhythmDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\RiffStructs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedPattern.h
# End Source File
# Begin Source File

SOURCE=.\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Style.h
# End Source File
# Begin Source File

SOURCE=.\StyleBands.h
# End Source File
# Begin Source File

SOURCE=.\StyleComponent.h
# End Source File
# Begin Source File

SOURCE=.\StyleCtl.h
# End Source File
# Begin Source File

SOURCE=.\StyleDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\StyleDlg.h
# End Source File
# Begin Source File

SOURCE=.\StyleDocType.h
# End Source File
# Begin Source File

SOURCE=.\StyleMotifs.h
# End Source File
# Begin Source File

SOURCE=.\StylePatterns.h
# End Source File
# Begin Source File

SOURCE=.\StylePersonalities.h
# End Source File
# Begin Source File

SOURCE=.\StyleRef.h
# End Source File
# Begin Source File

SOURCE=.\styleriff.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TabBoundaryFlags.h
# End Source File
# Begin Source File

SOURCE=.\TabMotifLoop.h
# End Source File
# Begin Source File

SOURCE=.\TabMotifMotif.h
# End Source File
# Begin Source File

SOURCE=.\TabPatternPattern.h
# End Source File
# Begin Source File

SOURCE=.\TabStyleInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabStyleStyle.h
# End Source File
# Begin Source File

SOURCE=.\TabVarChoices.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Templates.h
# End Source File
# Begin Source File

SOURCE=.\TimeSignatureDlg.h
# End Source File
# Begin Source File

SOURCE=.\VarChoices.h
# End Source File
# Begin Source File

SOURCE=.\VarChoicesCtl.h
# End Source File
# Begin Source File

SOURCE=.\VarChoicesDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\aboutdll.ico
# End Source File
# Begin Source File

SOURCE=.\res\disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\gpb1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb1_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb2_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb3_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb4_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb6_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb8_alt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpb9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpba19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb21.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb23.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gpbb24.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\MotifCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\MotifNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\MotifNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\MotifToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PatternCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PatternNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PatternNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PatternToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Selected.bmp
# End Source File
# Begin Source File

SOURCE=.\res\splitv.cur
# End Source File
# Begin Source File

SOURCE=.\Res\StyleCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\StyleNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\StyleNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\StyleRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\StyleRefNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\StyleToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VarChoicesCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VarChoicesToolbar.bmp
# End Source File
# End Group
# End Target
# End Project
