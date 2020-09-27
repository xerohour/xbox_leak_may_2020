# Microsoft Developer Studio Project File - Name="PersonalityDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PersonalityDesigner - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PersonalityDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PersonalityDesigner.mak" CFG="PersonalityDesigner - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PersonalityDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PersonalityDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PersonalityDesigner - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PersonalityDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/PersonalityDesigner", WCYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PersonalityDesigner - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I ".\OldCode" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib version.lib htmlhelp.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /out:"Release/ChordmapDesigner.ocx" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\ChordmapDesigner.ocx
TargetName=ChordmapDesigner
InputPath=.\Release\ChordmapDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 Debug"

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
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I ".\OldCode" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib Version.lib htmlhelp.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libcd" /out:"Debug/ChordMapDesigner.ocx" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\ChordMapDesigner.ocx
TargetName=ChordMapDesigner
InputPath=.\Debug\ChordMapDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PersonalityDesigner___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "PersonalityDesigner___Win32_BoundsChecker"
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
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I ".\OldCode" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\Includes" /I "..\Shared" /I ".\OldCode" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Res" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib Version.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib Version.lib htmlhelp.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libcd" /out:"BoundsChecker/ChordmapDesigner.ocx" /pdbtype:sept /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\ChordmapDesigner.ocx
TargetName=ChordmapDesigner
InputPath=.\BoundsChecker\ChordmapDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PersonalityDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "PersonalityDesigner___Win32_BBT"
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
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I ".\OldCode" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I ".\OldCode" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "Res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib version.lib htmlhelp.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /out:"Release/ChordmapDesigner.ocx" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib version.lib htmlhelp.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /out:"BBT/ChordmapDesigner.ocx" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\ChordmapDesigner.ocx
TargetName=ChordmapDesigner
InputPath=.\BBT\ChordmapDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "PersonalityDesigner - Win32 Release"
# Name "PersonalityDesigner - Win32 Debug"
# Name "PersonalityDesigner - Win32 BoundsChecker"
# Name "PersonalityDesigner - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;idl"
# Begin Source File

SOURCE=.\OldCode\Chord.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordInversionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordIO.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordScalePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChangeLength.cpp
# End Source File
# Begin Source File

SOURCE=.\OldCode\DWList.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
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

SOURCE=.\msflexgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\Personality.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner.def
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner.rc

!IF  "$(CFG)" == "PersonalityDesigner - Win32 Release"

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 Debug"

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "PersonalityDesigner - Win32 BBT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PersonalityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityPpg.cpp
# End Source File
# Begin Source File

SOURCE=.\PersonalityRef.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
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

SOURCE=.\rowcursor.cpp
# End Source File
# Begin Source File

SOURCE=.\SignPostDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabAudition.cpp
# End Source File
# Begin Source File

SOURCE=.\TabChordPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\TabPersonality.cpp
# End Source File
# Begin Source File

SOURCE=.\TabPersonalityInfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\OldCode\Chord.h
# End Source File
# Begin Source File

SOURCE=.\ChordBuilder.h
# End Source File
# Begin Source File

SOURCE=.\ChordDatabase.h
# End Source File
# Begin Source File

SOURCE=.\ChordDialog.h
# End Source File
# Begin Source File

SOURCE=.\ChordInversionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ChordIO.h
# End Source File
# Begin Source File

SOURCE=.\ChordScalePropPage.h
# End Source File
# Begin Source File

SOURCE=.\DlgChangeLength.h
# End Source File
# Begin Source File

SOURCE=.\OldCode\DWList.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=..\Shared\guiddlg.h
# End Source File
# Begin Source File

SOURCE=.\IllegalChord.h
# End Source File
# Begin Source File

SOURCE=..\JazzCommon\JazzUndoMan.h
# End Source File
# Begin Source File

SOURCE=.\msflexgrid.h
# End Source File
# Begin Source File

SOURCE=.\Personality.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityAbout.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityComponent.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityCtl.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDesigner.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDlg.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityDocType.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityPpg.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityRef.h
# End Source File
# Begin Source File

SOURCE=.\PersonalityRIFF.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PropChord.h
# End Source File
# Begin Source File

SOURCE=.\PropPageMgr.h
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

SOURCE=.\rowcursor.h
# End Source File
# Begin Source File

SOURCE=.\SignPostDialog.h
# End Source File
# Begin Source File

SOURCE=.\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabAudition.h
# End Source File
# Begin Source File

SOURCE=.\TabChordPalette.h
# End Source File
# Begin Source File

SOURCE=.\TabPersonality.h
# End Source File
# Begin Source File

SOURCE=.\TabPersonalityInfo.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Templates.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\shared\invertd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\invertf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\invertu.bmp
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

SOURCE=.\Res\perrefsel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\personal.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PersonalityCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PersonalityDesigner.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PersonalityNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PersonalityNodeSel.ico
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

SOURCE=..\shared\shiftld.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftlf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftlu.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftrd.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftrf.bmp
# End Source File
# Begin Source File

SOURCE=..\shared\shiftru.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Splith.cur
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ImportantLocalizationNote.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section PersonalityDesigner : {9F6AA700-D188-11CD-AD48-00AA003C9CB6}
# 	2:5:Class:CRowCursor
# 	2:10:HeaderFile:rowcursor.h
# 	2:8:ImplFile:rowcursor.cpp
# End Section
# Section PersonalityDesigner : {5F4DF280-531B-11CF-91F6-C2863C385E30}
# 	2:5:Class:CMSFlexGrid
# 	2:10:HeaderFile:msflexgrid.h
# 	2:8:ImplFile:msflexgrid.cpp
# End Section
# Section PersonalityDesigner : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section PersonalityDesigner : {6262D3A0-531B-11CF-91F6-C2863C385E30}
# 	2:21:DefaultSinkHeaderFile:msflexgrid.h
# 	2:16:DefaultSinkClass:CMSFlexGrid
# End Section
# Section PersonalityDesigner : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
