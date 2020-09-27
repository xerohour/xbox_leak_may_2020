# Microsoft Developer Studio Project File - Name="AudioPathDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AudioPathDesigner - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AudioPathDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AudioPathDesigner.mak" CFG="AudioPathDesigner - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AudioPathDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AudioPathDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AudioPathDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AudioPathDesigner - Win32 Xbox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AudioPathDesigner - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Jazz/AudioPathDesigner", YFJBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AudioPathDesigner - Win32 Release"

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
# ADD CPP /nologo /MD /W4 /GX /Zi /O1 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\AudioPathDesigner.ocx
TargetName=AudioPathDesigner
InputPath=.\Release\AudioPathDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Debug"

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
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D _WIN32_IE=0x0400 /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\AudioPathDesigner.ocx
TargetName=AudioPathDesigner
InputPath=.\Debug\AudioPathDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AudioPathDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "AudioPathDesigner___Win32_BBT"
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
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O1 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O1 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\AudioPathDesigner.ocx
TargetName=AudioPathDesigner
InputPath=.\BBT\AudioPathDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Xbox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AudioPathDesigner___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "AudioPathDesigner___Win32_Xbox_Release"
# PROP BASE Target_Ext "ocx"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Ext "ocx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O1 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O1 /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Release
TargetPath=.\Xbox_Release\AudioPathDesigner.ocx
TargetName=AudioPathDesigner
InputPath=.\Xbox_Release\AudioPathDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AudioPathDesigner___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "AudioPathDesigner___Win32_Xbox_Debug"
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
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D _WIN32_IE=0x0400 /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /I "..\Shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D _WIN32_IE=0x0400 /D "DMP_XBOX" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib htmlhelp.lib version.lib DMUSProdGUID.lib msdmo.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\AudioPathDesigner.ocx
TargetName=AudioPathDesigner
InputPath=.\Xbox_Debug\AudioPathDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AudioPathDesigner - Win32 Release"
# Name "AudioPathDesigner - Win32 Debug"
# Name "AudioPathDesigner - Win32 BBT"
# Name "AudioPathDesigner - Win32 Xbox Release"
# Name "AudioPathDesigner - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioPath.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathDesignerDLL.def
# End Source File
# Begin Source File

SOURCE=.\AudioPathDesignerDLL.rc
# End Source File
# Begin Source File

SOURCE=.\AudioPathDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPathRef.cpp
# End Source File
# Begin Source File

SOURCE=.\BufferPPGMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg3DParam.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgAddPChannel.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgAddPChannel.rc

!IF  "$(CFG)" == "AudioPathDesigner - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 BBT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioPathDesigner - Win32 Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgAddRemoveBuses.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEditSynth.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInsertEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNewAudiopath.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSendProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectListCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectPPGMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MixGroupPPGMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiTree.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\OlePropPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\OlePropSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabAudioPathAudioPath.cpp
# End Source File
# Begin Source File

SOURCE=.\TabAudioPathInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\TabEffectInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabMixGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeDroptarget.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioPath.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathComponent.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathCtl.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathDlg.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathDocType.h
# End Source File
# Begin Source File

SOURCE=.\AudioPathRef.h
# End Source File
# Begin Source File

SOURCE=.\BufferPPGMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\comhelp.h
# End Source File
# Begin Source File

SOURCE=.\Dlg3DParam.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgAddPChannel.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DlgAddPChannelresource.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddRemoveBuses.h
# End Source File
# Begin Source File

SOURCE=.\DlgEditSynth.h
# End Source File
# Begin Source File

SOURCE=.\DlgInsertEffect.h
# End Source File
# Begin Source File

SOURCE=.\DlgNewAudiopath.h
# End Source File
# Begin Source File

SOURCE=.\DlgSendProperties.h
# End Source File
# Begin Source File

SOURCE=.\EffectInfo.h
# End Source File
# Begin Source File

SOURCE=.\EffectListCtl.h
# End Source File
# Begin Source File

SOURCE=.\EffectListDlg.h
# End Source File
# Begin Source File

SOURCE=.\EffectPPGMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\guiddlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemInfo.h
# End Source File
# Begin Source File

SOURCE=..\Shared\LockoutNotification.h
# End Source File
# Begin Source File

SOURCE=.\MixGroupPPGMgr.h
# End Source File
# Begin Source File

SOURCE=.\MultiTree.h
# End Source File
# Begin Source File

SOURCE=..\Shared\OlePropPage.h
# End Source File
# Begin Source File

SOURCE=..\Shared\OlePropSheet.h
# End Source File
# Begin Source File

SOURCE=.\PPGItemBase.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabAudioPathAudioPath.h
# End Source File
# Begin Source File

SOURCE=.\TabAudioPathInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabBuffer.h
# End Source File
# Begin Source File

SOURCE=.\TabEffectInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabMixGroup.h
# End Source File
# Begin Source File

SOURCE=.\TreeDroptarget.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\AboutDLL.ico
# End Source File
# Begin Source File

SOURCE=.\Res\AudioPathCtl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\AudioPathNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\AudioPathNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\AudioPathRefNode.ico
# End Source File
# Begin Source File

SOURCE=.\Res\AudioPathRefNodeSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Splith.cur
# End Source File
# End Group
# End Target
# End Project
