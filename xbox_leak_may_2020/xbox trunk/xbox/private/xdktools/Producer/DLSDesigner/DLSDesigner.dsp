# Microsoft Developer Studio Project File - Name="DLSDesigner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DLSDesigner - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DLSDesigner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DLSDesigner.mak" CFG="DLSDesigner - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLSDesigner - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLSDesigner - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLSDesigner - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLSDesigner - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLSDesigner - Win32 Xbox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLSDesigner - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/DLSDesigner", OUWAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLSDesigner - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O2 /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "NDEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 JazzCommon.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\Release\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "_DEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JazzCommond.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\Debug\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DLSDesigner___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "DLSDesigner___Win32_BoundsChecker"
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "_DEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "_DEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib msacm32.lib DMUSProdGUID.lib ConductorGuid.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\BoundsChecker\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLSDesigner___Win32_BBT"
# PROP BASE Intermediate_Dir "DLSDesigner___Win32_BBT"
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
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "NDEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O2 /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "NDEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib ConductorGuid.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release" /libpath:"..\..\BBTLibs" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\BBT\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLSDesigner___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "DLSDesigner___Win32_Xbox_Release"
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
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /Zi /O2 /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "NDEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O2 /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "NDEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommon.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# ADD LINK32 JazzCommon.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"LIBC" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Release
TargetPath=.\Xbox_Release\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\Xbox_Release\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DLSDesigner___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "DLSDesigner___Win32_Xbox_Debug"
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
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "_DEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "." /I "..\Shared" /I "..\regionkeyboard" /I "..\includes" /I ".\ADSREnvelope" /D "_DEBUG" /D "DLS_UPDATE_SYNTH" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "MSAUDIO" /D "DMP_XBOX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 JazzCommond.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# ADD LINK32 JazzCommond.lib shlwapi.lib htmlhelp.lib msacm32.lib DMUSProdGUID.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\DLSDesigner.ocx
TargetName=DLSDesigner
InputPath=.\Xbox_Debug\DLSDesigner.ocx
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if NOT %BUILD_MACHINE%x==YESx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DLSDesigner - Win32 Release"
# Name "DLSDesigner - Win32 Debug"
# Name "DLSDesigner - Win32 BoundsChecker"
# Name "DLSDesigner - Win32 BBT"
# Name "DLSDesigner - Win32 Xbox Release"
# Name "DLSDesigner - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdpcmLoopWarnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\adsrenvelope.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\AList.cpp
# End Source File
# Begin Source File

SOURCE=.\Articulation.cpp
# End Source File
# Begin Source File

SOURCE=.\ArticulationList.cpp
# End Source File
# Begin Source File

SOURCE=.\ArticulationListPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ArticulationListPropPageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Collection.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionExtraPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionInstruments.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionRef.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectionWaves.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionalChunk.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionConfigEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\conditionconfigtoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner.def
# End Source File
# Begin Source File

SOURCE=.\DLSDesigner.rc

!IF  "$(CFG)" == "DLSDesigner - Win32 Release"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Debug"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BoundsChecker"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BBT"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Release"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DLSDesignerDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSDocType.cpp

!IF  "$(CFG)" == "DLSDesigner - Win32 Release"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Debug"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BoundsChecker"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 BBT"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Release"

!ELSEIF  "$(CFG)" == "DLSDesigner - Win32 Xbox Debug"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DLSEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DLSLoadSaveUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\DLSStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GuidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Guids.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Info.cpp
# End Source File
# Begin Source File

SOURCE=.\InsertSilenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Instrument.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentFVEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentRegions.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyBoardMap.cpp
# End Source File
# Begin Source File

SOURCE=.\LFODialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MonoWave.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\myslider.cpp
# End Source File
# Begin Source File

SOURCE=.\NameUnknownQueryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\newwavedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchConflictDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PitchDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Region.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionExtraPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\regionkeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionPropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ResampleDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StereoWave.cpp
# End Source File
# Begin Source File

SOURCE=.\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Unknownchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\UserPatchConflictDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VibratoLFODialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VolDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveCompressionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveCompressionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveDataManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveDelta.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveDocType.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveInfoPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveNode.cpp
# End Source File
# Begin Source File

SOURCE=.\WavePropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\WavePropPgMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveRefNode.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSourceFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveStream.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveTempFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveUndoManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdpcmLoopWarnDlg.h
# End Source File
# Begin Source File

SOURCE=.\adsrenvelope.h
# End Source File
# Begin Source File

SOURCE=..\Shared\alist.h
# End Source File
# Begin Source File

SOURCE=.\Articulation.h
# End Source File
# Begin Source File

SOURCE=.\ArticulationList.h
# End Source File
# Begin Source File

SOURCE=.\ArticulationListPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ArticulationListPropPageMgr.h
# End Source File
# Begin Source File

SOURCE=.\Collection.h
# End Source File
# Begin Source File

SOURCE=.\CollectionExtraPropPg.h
# End Source File
# Begin Source File

SOURCE=.\CollectionInstruments.h
# End Source File
# Begin Source File

SOURCE=.\CollectionPropPg.h
# End Source File
# Begin Source File

SOURCE=.\CollectionPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\CollectionRef.h
# End Source File
# Begin Source File

SOURCE=.\CollectionWaves.h
# End Source File
# Begin Source File

SOURCE=.\ConditionalChunk.h
# End Source File
# Begin Source File

SOURCE=.\conditionconfigeditor.h
# End Source File
# Begin Source File

SOURCE=.\conditionconfigtoolbar.h
# End Source File
# Begin Source File

SOURCE=.\ConditionEditor.h
# End Source File
# Begin Source File

SOURCE=..\Includes\dls1.h
# End Source File
# Begin Source File

SOURCE=..\Includes\dls2.h
# End Source File
# Begin Source File

SOURCE=.\DLSComponent.h
# End Source File
# Begin Source File

SOURCE=.\DlsDefsPlus.h
# End Source File
# Begin Source File

SOURCE=.\DLSDesignerDLL.h
# End Source File
# Begin Source File

SOURCE=.\DLSDocType.h
# End Source File
# Begin Source File

SOURCE=.\DLSEdit.h
# End Source File
# Begin Source File

SOURCE=..\Shared\DLSLoadSaveUtils.h
# End Source File
# Begin Source File

SOURCE=.\DLSStatic.h
# End Source File
# Begin Source File

SOURCE=..\Includes\dmusicc.h
# End Source File
# Begin Source File

SOURCE=..\Includes\dmusici.h
# End Source File
# Begin Source File

SOURCE=.\filterdialog.h
# End Source File
# Begin Source File

SOURCE=.\guiddlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Info.h
# End Source File
# Begin Source File

SOURCE=.\InsertSilenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Instrument.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentCtl.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentFVEditor.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentPropPg.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentRegions.h
# End Source File
# Begin Source File

SOURCE=.\KeyBoardMap.h
# End Source File
# Begin Source File

SOURCE=.\LFODialog.h
# End Source File
# Begin Source File

SOURCE=.\MonoWave.h
# End Source File
# Begin Source File

SOURCE=..\Shared\myslider.h
# End Source File
# Begin Source File

SOURCE=.\NameUnknownQueryDialog.h
# End Source File
# Begin Source File

SOURCE=.\newwavedialog.h
# End Source File
# Begin Source File

SOURCE=.\PatchConflictDlg.h
# End Source File
# Begin Source File

SOURCE=.\PitchDialog.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDialog.h
# End Source File
# Begin Source File

SOURCE=.\Region.h
# End Source File
# Begin Source File

SOURCE=.\RegionExtraPropPg.h
# End Source File
# Begin Source File

SOURCE=.\regionkeyboard.h
# End Source File
# Begin Source File

SOURCE=.\RegionPropPg.h
# End Source File
# Begin Source File

SOURCE=.\RegionPropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\ResampleDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StereoWave.h
# End Source File
# Begin Source File

SOURCE=.\UndoMan.h
# End Source File
# Begin Source File

SOURCE=..\Shared\UnknownChunk.h
# End Source File
# Begin Source File

SOURCE=.\UserPatchConflictDlg.h
# End Source File
# Begin Source File

SOURCE=.\vibratolfodialog.h
# End Source File
# Begin Source File

SOURCE=.\VolDialog.h
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# Begin Source File

SOURCE=.\WaveCompressionManager.h
# End Source File
# Begin Source File

SOURCE=.\WaveCompressionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\WaveCtl.h
# End Source File
# Begin Source File

SOURCE=.\WaveDataManager.h
# End Source File
# Begin Source File

SOURCE=.\WaveDelta.h
# End Source File
# Begin Source File

SOURCE=.\WaveDocType.h
# End Source File
# Begin Source File

SOURCE=.\WaveFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\WaveInfoPropPg.h
# End Source File
# Begin Source File

SOURCE=.\WaveNode.h
# End Source File
# Begin Source File

SOURCE=.\WavePropPg.h
# End Source File
# Begin Source File

SOURCE=.\WavePropPgMgr.h
# End Source File
# Begin Source File

SOURCE=.\WaveRefNode.h
# End Source File
# Begin Source File

SOURCE=.\WaveSourceFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\WaveStream.h
# End Source File
# Begin Source File

SOURCE=.\WaveTempFileHandler.h
# End Source File
# Begin Source File

SOURCE=..\Includes\WaveTimelineDraw.h
# End Source File
# Begin Source File

SOURCE=..\Includes\WaveTrackDownload.h
# End Source File
# Begin Source File

SOURCE=.\WaveUndoManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\3dwwe.cur
# End Source File
# Begin Source File

SOURCE=.\Res\artcgsel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\articg.ico
# End Source File
# Begin Source File

SOURCE=.\Res\articulation.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Articulation.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ArticulationSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Attack.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cfgtbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\collection.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Collection.ico
# End Source File
# Begin Source File

SOURCE=.\Res\CollectionRef.ico
# End Source File
# Begin Source File

SOURCE=.\Res\CollectionRefSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\CollectionSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Decay.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Decay2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\delay.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\DLSDesigner.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Folder.ico
# End Source File
# Begin Source File

SOURCE=.\Res\folder_g.ico
# End Source File
# Begin Source File

SOURCE=.\Res\FolderSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\foldgsel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\hold.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\instrument.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Instrument.ico
# End Source File
# Begin Source File

SOURCE=.\Res\InstrumentSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Jazz.ico
# End Source File
# Begin Source File

SOURCE=.\Res\region.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Region.ico
# End Source File
# Begin Source File

SOURCE=.\Res\region_g.ico
# End Source File
# Begin Source File

SOURCE=.\Res\RegionSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Release.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Release2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\rgn_gsel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\sizewe_b.cur
# End Source File
# Begin Source File

SOURCE=.\Res\Sustain.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\wave.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Wave.ico
# End Source File
# Begin Source File

SOURCE=.\Res\wave_ste.ico
# End Source File
# Begin Source File

SOURCE=.\Res\WaveRef.ico
# End Source File
# Begin Source File

SOURCE=.\Res\WaveRefSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\WaveSel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\wavestrf.ico
# End Source File
# Begin Source File

SOURCE=.\Res\wavestsl.ico
# End Source File
# Begin Source File

SOURCE=.\Res\wvstrfsl.ico
# End Source File
# Begin Source File

SOURCE=.\Res\zoomin.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoomindown.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoomout.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoomoutd.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section DLSDesigner : {36CD3185-EE61-11D0-876A-00AA00C08146}
# 	2:5:Class:CRegionKeyboard
# 	2:10:HeaderFile:regionkeyboard.h
# 	2:8:ImplFile:regionkeyboard.cpp
# End Section
# Section DLSDesigner : {71AE3626-A9BD-11D0-BCBA-00AA00C08146}
# 	2:16:DefaultSinkClass:CADSREnvelope
# End Section
# Section DLSDesigner : {36CD3187-EE61-11D0-876A-00AA00C08146}
# 	2:21:DefaultSinkHeaderFile:regionkeyboard.h
# 	2:16:DefaultSinkClass:CRegionKeyboard
# End Section
# Section DLSDesigner : {71AE3624-A9BD-11D0-BCBA-00AA00C08146}
# 	2:5:Class:CADSREnvelope
# End Section
