# Microsoft Developer Studio Project File - Name="XboxAddin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XboxAddin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XboxAddin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XboxAddin.mak" CFG="XboxAddin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XboxAddin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XboxAddin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "XboxAddin - Win32 Release No_Opt" (based on "Win32 (x86) Application")
!MESSAGE "XboxAddin - Win32 BBT" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/XboxAddin", OVLBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XboxAddin - Win32 Release"

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
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\Includes" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 jazzcommon.lib XboxDbg.Lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Release/XboxAddin.ocx" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\XboxAddin.ocx
InputPath=.\Release\XboxAddin.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 jazzcommond.lib XboxDbg.Lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/XboxAddin.ocx" /libpath:"..\Libs\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\XboxAddin.ocx
InputPath=.\Debug\XboxAddin.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Release No_Opt"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XboxAddin___Win32_Release_No_Opt"
# PROP BASE Intermediate_Dir "XboxAddin___Win32_Release_No_Opt"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_No_Opt"
# PROP Intermediate_Dir "Release_No_Opt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\Jazz\Includes" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /Od /I "..\Includes" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 jazzcommon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Release/XboxAddin.ocx" /libpath:"..\Jazz\Libs\Release"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 jazzcommon.lib XboxDbg.Lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Release/XboxAddin.ocx" /libpath:"..\Libs\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release_No_Opt
TargetPath=.\Release\XboxAddin.ocx
InputPath=.\Release\XboxAddin.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 BBT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XboxAddin___Win32_BBT"
# PROP BASE Intermediate_Dir "XboxAddin___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\Includes" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\Includes" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 jazzcommon.lib XboxDbg.Lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Release/XboxAddin.ocx" /libpath:"..\Libs\Release"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 jazzcommon.lib XboxDbg.Lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Release/XboxAddin.ocx" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\BBT" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\Release\XboxAddin.ocx
InputPath=.\Release\XboxAddin.ocx
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "XboxAddin - Win32 Release"
# Name "XboxAddin - Win32 Debug"
# Name "XboxAddin - Win32 Release No_Opt"
# Name "XboxAddin - Win32 BBT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Audiopath.cpp
# End Source File
# Begin Source File

SOURCE=.\AudiopathListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Droptarget.cpp
# End Source File
# Begin Source File

SOURCE=.\FileItem.cpp
# End Source File
# Begin Source File

SOURCE=.\FileListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GetNodesErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\LeftPaneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherFile.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherFilesListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PrimaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RightPaneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptsDlg.cpp

!IF  "$(CFG)" == "XboxAddin - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Release No_Opt"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecondaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TargetDirectoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TargetXboxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxAddin.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxAddin.def
# End Source File
# Begin Source File

SOURCE=.\XboxAddin.rc
# End Source File
# Begin Source File

SOURCE=.\XboxAddinComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxAddinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxSynthConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxSynthMenu.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Audiopath.h
# End Source File
# Begin Source File

SOURCE=.\AudiopathListBox.h
# End Source File
# Begin Source File

SOURCE=.\Droptarget.h
# End Source File
# Begin Source File

SOURCE=.\FileItem.h
# End Source File
# Begin Source File

SOURCE=.\FileListBox.h
# End Source File
# Begin Source File

SOURCE=.\GetNodesErrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\HSplitter.h
# End Source File
# Begin Source File

SOURCE=.\LeftPaneDlg.h
# End Source File
# Begin Source File

SOURCE=.\OtherFile.h
# End Source File
# Begin Source File

SOURCE=.\OtherFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\OtherFilesListBox.h
# End Source File
# Begin Source File

SOURCE=.\PrimaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RightPaneDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptsDlg.h

!IF  "$(CFG)" == "XboxAddin - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 Release No_Opt"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XboxAddin - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecondaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\Segment.h
# End Source File
# Begin Source File

SOURCE=.\SegmentDlg.h
# End Source File
# Begin Source File

SOURCE=.\SegmentListBox.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TargetDirectoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\TargetXboxDlg.h
# End Source File
# Begin Source File

SOURCE=.\VSplitter.h
# End Source File
# Begin Source File

SOURCE=.\XboxAddin.h
# End Source File
# Begin Source File

SOURCE=.\XboxAddinComponent.h
# End Source File
# Begin Source File

SOURCE=.\XboxAddinDlg.h
# End Source File
# Begin Source File

SOURCE=.\XboxSynthConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\XboxSynthMenu.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Collection.ico
# End Source File
# Begin Source File

SOURCE=.\res\ContainerNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\PersonalityNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\play.ico
# End Source File
# Begin Source File

SOURCE=.\res\ScriptNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\SDK.Ico
# End Source File
# Begin Source File

SOURCE=.\res\Stop.ico
# End Source File
# Begin Source File

SOURCE=.\res\StyleNode.ico
# End Source File
# Begin Source File

SOURCE=.\res\transition.ico
# End Source File
# Begin Source File

SOURCE=.\res\Wave.ico
# End Source File
# Begin Source File

SOURCE=.\res\XboxAddin.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
