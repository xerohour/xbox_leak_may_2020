# Microsoft Developer Studio Project File - Name="qtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qtest - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qtest.mak" CFG="qtest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qtest - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qtest - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qtest - Win32 Debug"

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
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /X /I "..\common" /I "..\..\..\..\online\server\servhlp" /I "..\..\..\..\online\server\xqserver" /I "..\..\..\..\..\public\wsdk\inc" /I "\Program Files\Microsoft Visual Studio\VC98\ATL\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libcmtd.lib connection.lib ws2_32.lib kernel32.lib oleaut32.lib comsupp.lib ole32.lib user32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"..\lib\i386" /libpath:"..\..\..\..\..\public\wsdk\lib"
# SUBTRACT LINK32 /incremental:no
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\qtest.dll
InputPath=.\Debug\qtest.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "qtest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "qtest___Win32_Release"
# PROP BASE Intermediate_Dir "qtest___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /X /I "..\common" /I "..\..\..\..\online\server\servhlp" /I "..\..\..\..\online\server\xqserver" /I "..\..\..\..\..\public\wsdk\inc" /I "..\..\..\..\..\public\wsdk\inc\atl30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /X /I "..\common" /I "..\..\..\..\online\server\servhlp" /I "..\..\..\..\online\server\xqserver" /I "..\..\..\..\..\public\wsdk\inc" /I "\Program Files\Microsoft Visual Studio\VC98\ATL\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmtd.lib connection.lib threadpool.lib ws2_32.lib kernel32.lib oleaut32.lib comsupp.lib ole32.lib user32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"..\lib\i386" /libpath:"..\..\..\..\..\public\wsdk\lib"
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 libcmt.lib connection.lib ws2_32.lib kernel32.lib oleaut32.lib comsupp.lib ole32.lib user32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /libpath:"..\lib\i386" /libpath:"..\..\..\..\..\public\wsdk\lib"
# SUBTRACT LINK32 /incremental:no /debug
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Release\qtest.dll
InputPath=.\Release\qtest.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "qtest - Win32 Debug"
# Name "qtest - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\qtest.cpp
# End Source File
# Begin Source File

SOURCE=.\qtest.def
# End Source File
# Begin Source File

SOURCE=.\qtest.idl
# ADD MTL /tlb ".\qtest.tlb" /h "qtest.h" /iid "qtest_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\qtest.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\engine.rgs
# End Source File
# End Group
# Begin Group "Scripts"

# PROP Default_Filter "*.js"
# Begin Group "testcases"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scripts\connect.js
# End Source File
# Begin Source File

SOURCE=.\scripts\everything.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msgadd.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msgdeadxip.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msgdeadxrg.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msgdelete.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msgdeletematches.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msghello.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msglist.js
# End Source File
# Begin Source File

SOURCE=.\scripts\msguserinfo.js
# End Source File
# Begin Source File

SOURCE=.\scripts\simple.js
# End Source File
# End Group
# Begin Group "utilities"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scripts\common.js
# End Source File
# Begin Source File

SOURCE=.\scripts\config.js
# End Source File
# Begin Source File

SOURCE=.\scripts\conversions.js
# End Source File
# Begin Source File

SOURCE=.\scripts\debugprint.js
# End Source File
# Begin Source File

SOURCE=.\scripts\parseconfig.js
# End Source File
# Begin Source File

SOURCE=.\scripts\testcase.js
# End Source File
# End Group
# Begin Source File

SOURCE=.\scripts\settings.js
# End Source File
# End Group
# End Target
# End Project
