# Microsoft Developer Studio Project File - Name="DMUSProd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DMUSProd - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DMUSProd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DMUSProd.mak" CFG="DMUSProd - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DMUSProd - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DMUSProd - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DMUSProd - Win32 BoundsChecker" (based on "Win32 (x86) Application")
!MESSAGE "DMUSProd - Win32 BBT" (based on "Win32 (x86) Application")
!MESSAGE "DMUSProd - Win32 Xbox Release" (based on "Win32 (x86) Application")
!MESSAGE "DMUSProd - Win32 Xbox Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/Framework", IELAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DMUSProd - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Creating symbol file
OutDir=.\Release
TargetName=DMUSProd
InputPath=.\Release\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Creating symbol file
OutDir=.\Debug
TargetName=DMUSProd
InputPath=.\Debug\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 BoundsChecker"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DMUSProd___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "DMUSProd___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib DMUSProdGUID.lib JazzCommond.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Creating symbol file
OutDir=.\BoundsChecker
TargetName=DMUSProd
InputPath=.\BoundsChecker\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 BBT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DMUSProd___Win32_BBT"
# PROP BASE Intermediate_Dir "DMUSProd___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release" /libpath:"..\..\BBTLibs" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Creating symbol file
OutDir=.\BBT
TargetName=DMUSProd
InputPath=.\BBT\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Xbox Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DMUSProd___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "DMUSProd___Win32_Xbox_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "DMP_XBOX"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "DMP_XBOX"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Creating symbol file
OutDir=.\Xbox_Release
TargetName=DMUSProd
InputPath=.\Xbox_Release\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Xbox Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DMUSProd___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "DMUSProd___Win32_Xbox_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "DMP_XBOX"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib DMUSProdGUIDPrivate.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Creating symbol file
OutDir=.\Xbox_Debug
TargetName=DMUSProd
InputPath=.\Xbox_Debug\DMUSProd.exe
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map"

# End Custom Build

!ENDIF 

# Begin Target

# Name "DMUSProd - Win32 Release"
# Name "DMUSProd - Win32 Debug"
# Name "DMUSProd - Win32 BoundsChecker"
# Name "DMUSProd - Win32 BBT"
# Name "DMUSProd - Win32 Xbox Release"
# Name "DMUSProd - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat"
# Begin Source File

SOURCE=.\Abhs.cpp
# End Source File
# Begin Source File

SOURCE=.\Bookmark.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CloseProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CntrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentCntrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentView.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectoryNode.cpp
# End Source File
# Begin Source File

SOURCE=.\EditLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\FileNewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileNode.cpp
# End Source File
# Begin Source File

SOURCE=.\FileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework.cpp
# End Source File
# Begin Source File

SOURCE=.\Jazz.rc

!IF  "$(CFG)" == "DMUSProd - Win32 Release"

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Debug"

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 BBT"

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Xbox Release"

!ELSEIF  "$(CFG)" == "DMUSProd - Win32 Xbox Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JazzApp.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzView.cpp
# End Source File
# Begin Source File

SOURCE=.\Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MemStream.cpp
# End Source File
# Begin Source File

SOURCE=.\NewBookmarkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NotifyList.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectPropTabFolders.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectPropTabGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertySheetCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RuntimeDupeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SavePrompt.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabFileDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\TabFileRuntime.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\urlmon.cpp
# End Source File
# Begin Source File

SOURCE=.\WhichProjectDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;inl"
# Begin Source File

SOURCE=.\Bookmark.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ClientToolBar.h
# End Source File
# Begin Source File

SOURCE=.\CloseProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\CntrItem.h
# End Source File
# Begin Source File

SOURCE=.\CommonDoc.h
# End Source File
# Begin Source File

SOURCE=.\ComponentCntrItem.h
# End Source File
# Begin Source File

SOURCE=.\ComponentDoc.h
# End Source File
# Begin Source File

SOURCE=.\ComponentView.h
# End Source File
# Begin Source File

SOURCE=.\DeleteFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\DeleteProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DirectoryNode.h
# End Source File
# Begin Source File

SOURCE=.\EditLabel.h
# End Source File
# Begin Source File

SOURCE=.\FileNewDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileNode.h
# End Source File
# Begin Source File

SOURCE=.\Framework.h
# End Source File
# Begin Source File

SOURCE=.\HelpBindHost.h
# End Source File
# Begin Source File

SOURCE=.\ioJazzDoc.h
# End Source File
# Begin Source File

SOURCE=.\JazzApp.h
# End Source File
# Begin Source File

SOURCE=.\jazzdoc.h
# End Source File
# Begin Source File

SOURCE=.\JazzDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\JazzToolBar.h
# End Source File
# Begin Source File

SOURCE=.\jazzview.h
# End Source File
# Begin Source File

SOURCE=.\Loader.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NewBookmarkDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\NotifyList.h
# End Source File
# Begin Source File

SOURCE=.\Project.h
# End Source File
# Begin Source File

SOURCE=.\ProjectPropTabFolders.h
# End Source File
# Begin Source File

SOURCE=.\ProjectPropTabGeneral.h
# End Source File
# Begin Source File

SOURCE=.\PropertyBar.h
# End Source File
# Begin Source File

SOURCE=.\PropertySheetCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\RuntimeDupeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SavePrompt.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabFileDesign.h
# End Source File
# Begin Source File

SOURCE=.\TabFileRuntime.h
# End Source File
# Begin Source File

SOURCE=.\TreeBar.h
# End Source File
# Begin Source File

SOURCE=.\WhichProjectDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\BookToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\FileNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\FileNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\folder.ico
# End Source File
# Begin Source File

SOURCE=.\res\FolderNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\FolderNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\FolderSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_jazz.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_proj.ico
# End Source File
# Begin Source File

SOURCE=.\res\Jazz.ico
# End Source File
# Begin Source File

SOURCE=.\res\Jazz.ico
# End Source File
# Begin Source File

SOURCE=.\res\Jazz.rc2
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\res\JazzDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\JazzDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\JazzFileNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\JazzFileNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\JazzToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectClosed.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProjectNodeClosed.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProjectNodeOpen.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProjectOpened.ico
# End Source File
# Begin Source File

SOURCE=.\res\Splith.cur
# End Source File
# Begin Source File

SOURCE=.\res\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\IDMUSProdComponent.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdDocType.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdEditor.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdMenu.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdNode.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdNodeDrop.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdNotifySink.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdPChannelName.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdPropPageManager.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdPropPageObject.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdReferenceNode.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdRIFFExt.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdSortNode.d
# End Source File
# Begin Source File

SOURCE=.\IDMUSProdToolBar.d
# End Source File
# End Target
# End Project
# Section DMUSProd : {A82AB343-BB35-11CF-8771-00A0C9039735}
# 	1:10:IDB_SPLASH:103
# 	2:21:SplashScreenInsertKey:4.0
# End Section
