# Microsoft Developer Studio Project File - Name="SurgeEngine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SurgeEngine - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SurgeEngine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SurgeEngine.mak" CFG="SurgeEngine - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SurgeEngine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SurgeEngine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WebPlat/WIPS/Testing/Tools/Surge/SurgeEngine", SJTCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SurgeEngine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "C:\Program Files\Microsoft Platform SDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\lib\HttpClientd.lib Crypt32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\Microsoft Platform SDK\Lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\SurgeEngine.dll
InputPath=.\Debug\SurgeEngine.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\SurgeEngine.dll ..\bin\SurgeEngineD.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SurgeEngine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SurgeEngine___Win32_Release"
# PROP BASE Intermediate_Dir "SurgeEngine___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "C:\Program Files\Microsoft Platform SDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /ML /W3 /GX /O2 /I "C:\Program Files\Microsoft Platform SDK\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\lib\HttpClientd.lib Crypt32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\Microsoft Platform SDK\Lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\lib\HttpClient.lib Crypt32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\Microsoft Platform SDK\Lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Release\SurgeEngine.dll
InputPath=.\Release\SurgeEngine.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\SurgeEngine.dll ..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SurgeEngine - Win32 Debug"
# Name "SurgeEngine - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\AnsiBstr.cpp
# End Source File
# Begin Source File

SOURCE=.\browser.cpp
# End Source File
# Begin Source File

SOURCE=.\CSurgeEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\digest.cpp
# End Source File
# Begin Source File

SOURCE=.\fastprofdb.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ParserTools.cpp
# End Source File
# Begin Source File

SOURCE=.\Reporting.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stresstimer.cpp
# End Source File
# Begin Source File

SOURCE=.\SurgeEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\SurgeEngine.def
# End Source File
# Begin Source File

SOURCE=.\SurgeEngine.idl
# ADD MTL /tlb ".\SurgeEngine.tlb" /h "SurgeEngine.h" /iid "SurgeEngine_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\SurgeEngine.rc
# End Source File
# Begin Source File

SOURCE=..\common\Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\AnsiBstr.h
# End Source File
# Begin Source File

SOURCE=.\AnsiBstr.h
# End Source File
# Begin Source File

SOURCE=.\browser.h
# End Source File
# Begin Source File

SOURCE=.\CSurgeEngine.h
# End Source File
# Begin Source File

SOURCE=.\ParserTools.h
# End Source File
# Begin Source File

SOURCE=.\Reporting.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StressTimer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SurgeEngine.rgs
# End Source File
# End Group
# End Target
# End Project
