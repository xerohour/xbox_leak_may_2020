# Microsoft Developer Studio Project File - Name="Conductor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Conductor - Win32 Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Conductor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Conductor.mak" CFG="Conductor - Win32 Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Conductor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Conductor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Conductor - Win32 BoundsChecker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Conductor - Win32 BBT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Conductor - Win32 Xbox Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Conductor - Win32 Xbox Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/Conductor", DYRAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Conductor - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "main\obj\i386"
# PROP Intermediate_Dir "main\obj\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\main\obj\i386
TargetPath=.\main\obj\i386\Conductor.dll
TargetName=Conductor
InputPath=.\main\obj\i386\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Conductor - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Conducto"
# PROP BASE Intermediate_Dir "Conducto"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "..\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\Libs\IMusic25.lib atl.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\Conductor.dll
TargetName=Conductor
InputPath=.\Release\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Conductor - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Conductor___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "Conductor___Win32_BoundsChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fr /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 atl.lib winmm.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /pdbtype:sept
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BoundsChecker
TargetPath=.\BoundsChecker\Conductor.dll
TargetName=Conductor
InputPath=.\BoundsChecker\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Conductor - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Conductor___Win32_BBT"
# PROP BASE Intermediate_Dir "Conductor___Win32_BBT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 atl.lib winmm.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc"
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\..\BBTLibs" /libpath:"..\Libs\Release" /debugtype:cv,fixup /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\BBT
TargetPath=.\BBT\Conductor.dll
TargetName=Conductor
InputPath=.\BBT\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Conductor - Win32 Xbox Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Conductor___Win32_Xbox_Release"
# PROP BASE Intermediate_Dir "Conductor___Win32_Xbox_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "DMP_XBOX" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\Libs\Release"
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\Libs\Release"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Release
TargetPath=.\Xbox_Release\Conductor.dll
TargetName=Conductor
InputPath=.\Xbox_Release\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Conductor - Win32 Xbox Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Conductor___Win32_Xbox_Debug"
# PROP BASE Intermediate_Dir "Conductor___Win32_Xbox_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "DirectKS" /I "PhoneyDS" /I "Detours" /I "..\Includes" /I "..\Shared" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "DMP_XBOX" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# ADD LINK32 msdmo.lib atl.lib winmm.lib DMUSProdGUID.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCD" /libpath:"..\Libs\Debug"
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Xbox_Debug
TargetPath=.\Xbox_Debug\Conductor.dll
TargetName=Conductor
InputPath=.\Xbox_Debug\Conductor.dll
SOURCE="$(InputPath)"

"$(OutDir)\$(TargetName).sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not %BUILD_MACHINE%xx == YESxx regsvr32 /s /c "$(TargetPath)" 
	mapsym -o "$(OutDir)\$(TargetName).sym" "$(OutDir)\$(TargetName).map" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Conductor - Win32 Debug"
# Name "Conductor - Win32 Release"
# Name "Conductor - Win32 BoundsChecker"
# Name "Conductor - Win32 BBT"
# Name "Conductor - Win32 Xbox Release"
# Name "Conductor - Win32 Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main\cconduct.cpp
# End Source File
# Begin Source File

SOURCE=.\main\cconduct.h
# End Source File
# Begin Source File

SOURCE=.\main\conduct.cpp
# End Source File
# Begin Source File

SOURCE=.\main\conduct.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\main\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\main\Debug.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgAltTempo.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgAltTempo.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgEchoAdvanced.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgEchoAdvanced.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgEditPChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgEditPChannel.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgMetronome.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgMetronome.h
# End Source File
# Begin Source File

SOURCE=.\main\dlgmidi.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgMIDI.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgMIDIExport.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgMIDIExport.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgSecondaryStart.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgSecondaryStart.h
# End Source File
# Begin Source File

SOURCE=.\main\DlgTransition.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DlgTransition.h
# End Source File
# Begin Source File

SOURCE=.\main\DMOInfoProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\main\DMOInfoProxy.h
# End Source File
# Begin Source File

SOURCE=.\main\dmusics.h
# End Source File
# Begin Source File

SOURCE=.\main\EchoMIDIInThru.cpp
# End Source File
# Begin Source File

SOURCE=.\main\MIDISave.cpp
# End Source File
# Begin Source File

SOURCE=.\main\MIDISave.h
# End Source File
# Begin Source File

SOURCE=.\main\notify.cpp
# End Source File
# Begin Source File

SOURCE=.\main\OptionsToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\OptionsToolbar.h
# End Source File
# Begin Source File

SOURCE=.\main\OutputTool.cpp
# End Source File
# Begin Source File

SOURCE=.\main\OutputTool.h
# End Source File
# Begin Source File

SOURCE=.\main\resource.h
# End Source File
# Begin Source File

SOURCE=.\main\SecondaryToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\SecondaryToolbar.h
# End Source File
# Begin Source File

SOURCE=.\main\StatusToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\StatusToolbar.h
# End Source File
# Begin Source File

SOURCE=.\main\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\main\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\main\SynthStatusToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\SynthStatusToolbar.h
# End Source File
# Begin Source File

SOURCE=.\main\toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\Toolbar.h
# End Source File
# Begin Source File

SOURCE=.\main\trentry.cpp
# End Source File
# Begin Source File

SOURCE=.\main\TREntry.h
# End Source File
# Begin Source File

SOURCE=.\main\WaveRecordToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\main\WaveRecordToolbar.h
# End Source File
# End Group
# Begin Group "detours"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Detours\detours.cpp
# End Source File
# Begin Source File

SOURCE=.\Detours\detours.h
# End Source File
# Begin Source File

SOURCE=.\Detours\disasm.cpp
# End Source File
# Begin Source File

SOURCE=.\Detours\disasm.h
# End Source File
# Begin Source File

SOURCE=.\Detours\dtrsint.h
# End Source File
# End Group
# Begin Group "PhoneyDS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PhoneyDS\audiosink.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\audiosink.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\cclock.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\decibels.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\decibels.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dmstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dmstrm.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dsbufcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dsbufcfg.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dslink.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\dslink.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\effects.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\phoneyds.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\phoneyds.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\plclock.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\plclock.h
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\PhoneyDS\StdAfx.h
# End Source File
# End Group
# Begin Group "DirectKS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirectKS\directks.h
# End Source File
# Begin Source File

SOURCE=.\DirectKS\ilog.h
# End Source File
# Begin Source File

SOURCE=.\DirectKS\ksconn.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\ksfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\ksirptgt.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\kslib.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\kslibp.h
# End Source File
# Begin Source File

SOURCE=.\DirectKS\ksnode.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\kspin.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\list.h
# End Source File
# Begin Source File

SOURCE=.\DirectKS\pcmaudf.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\pcmaudp.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectKS\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\DirectKS\sysaud.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\Shared\AList.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\NodeRefChunk.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Shared\alist.h
# End Source File
# Begin Source File

SOURCE=..\Shared\NodeRefChunk.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RiffStrm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Exp_Wave.bmp
# End Source File
# Begin Source File

SOURCE=.\start_do.bmp
# End Source File
# Begin Source File

SOURCE=.\start_do_g.bmp
# End Source File
# Begin Source File

SOURCE=.\StartUp.bmp
# End Source File
# Begin Source File

SOURCE=.\StartUpgrey.bmp
# End Source File
# Begin Source File

SOURCE=.\status.bmp
# End Source File
# Begin Source File

SOURCE=.\synth1.bmp
# End Source File
# Begin Source File

SOURCE=.\synth2.bmp
# End Source File
# Begin Source File

SOURCE=.\transpor.bmp
# End Source File
# End Group
# End Target
# End Project
