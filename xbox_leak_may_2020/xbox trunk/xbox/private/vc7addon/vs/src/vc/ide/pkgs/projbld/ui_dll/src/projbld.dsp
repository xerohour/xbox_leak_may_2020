# Microsoft Developer Studio Project File - Name="projbld" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 61000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=projbld - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "projbld.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "projbld.mak" CFG="projbld - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "projbld - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "projbld - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "projbld - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "projbld - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "projbld - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "projbld - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "projbld - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP ComPlus 0
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /RTC1 /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MTd
# ADD CPP /RTC1
# ADD CPP /W3
# ADD CPP /Gm
# ADD CPP /ZI
# ADD CPP /Od
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib" /I "..\..\projutil"
# ADD CPP /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL"
# ADD CPP /FR
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "_DEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /I "$(IDE)\..\common\idl\vsee"
# ADD MTL /D "_DEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
# ADD RSC /i "$(VSBUILT)\debug\bin\i386"
# ADD RSC /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 $(VSBUILT)\debug\lib\i386\vsguids.lib $(VSBUILT)\debug\lib\i386\vseeguids.lib $(VSBUILT)\debug\lib\i386\vcpbguids.lib $(VSBUILT)\debug\lib\i386\vcutillibd.lib $(VSBUILT)\debug\lib\i386\vcprojutillibd.lib $(VSROOT)\src\vc\ide\lib\i386\imaged.lib msvcrtd.lib msvcirtd.lib kernel32.lib gdi32.lib ole32.lib uuid.lib version.lib advapi32.lib user32.lib shell32.lib oleaut32.lib comdlg32.lib comctl32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /debug
# ADD LINK32 /machine:I386
# ADD LINK32 /nodefaultlib
# ADD LINK32 /def:".\projbld.def"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP ComPlus 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /RTC1 /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MTd
# ADD CPP /RTC1
# ADD CPP /W3
# ADD CPP /Gm
# ADD CPP /ZI
# ADD CPP /Od
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib"
# ADD CPP /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE"
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "_DEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "_DEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
# ADD RSC /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /opt:nowin98
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /debug
# ADD LINK32 /machine:I386
# ADD LINK32 /opt:nowin98

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP ComPlus 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MT
# ADD CPP /W3
# ADD CPP /O1
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib"
# ADD CPP /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT"
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "NDEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "NDEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409
# ADD RSC /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /machine:I386

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP ComPlus 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MT
# ADD CPP /W3
# ADD CPP /O1
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib"
# ADD CPP /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT"
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "NDEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "NDEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409
# ADD RSC /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /machine:I386

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP ComPlus 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MT
# ADD CPP /W3
# ADD CPP /O1
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib"
# ADD CPP /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT"
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "NDEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "NDEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409
# ADD RSC /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /opt:nowin98
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /machine:I386
# ADD LINK32 /opt:nowin98

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE ComPlus 0
# PROP BASE Target_Dir ""
# PROP BASE Debug_Exe ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP ComPlus 0
# PROP Target_Dir ""
# PROP Debug_Exe ""
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo
# ADD CPP /MT
# ADD CPP /W3
# ADD CPP /O1
# ADD CPP /I "$(IDE)\pkgs\include" /I "$(IDE)\include" /I "$(IDE)\..\..\common\idl\vsee" /I "$(VSBUILT)\debug\inc" /I "$(IDE)\..\..\common\inc" /I "$(IDE)\hierlib"
# ADD CPP /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT"
# ADD CPP /Yu"stdafx.h"
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /D "NDEBUG" /tlb ".\projbld.tlb" /h "projbld.h" /iid "projbld_i.c" /Oicf /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "NDEBUG"
# ADD MTL /tlb ".\projbld.tlb"
# ADD MTL /h "projbld.h"
# ADD MTL /iid "projbld_i.c"
# ADD MTL /Oicf
# ADD MTL /win32
# SUBTRACT MTL /nologo
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409
# ADD RSC /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /opt:nowin98
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /machine:I386
# ADD LINK32 /opt:nowin98

!ENDIF 

# Begin Target

# Name "projbld - Win32 Debug"
# Name "projbld - Win32 Unicode Debug"
# Name "projbld - Win32 Release MinSize"
# Name "projbld - Win32 Release MinDependency"
# Name "projbld - Win32 Unicode Release MinSize"
# Name "projbld - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bldpkg.cpp
# End Source File
# Begin Source File

SOURCE=.\buildinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\gencfg.cpp
# End Source File
# Begin Source File

SOURCE=.\gencfg.h
# End Source File
# Begin Source File

SOURCE=.\guids.cpp
# End Source File
# Begin Source File

SOURCE=.\localeinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\output.cpp
# End Source File
# Begin Source File

SOURCE=.\prjnode.cpp
# End Source File
# Begin Source File

SOURCE=.\projbld.cpp
# End Source File
# Begin Source File

SOURCE=.\projbld.def

!IF  "$(CFG)" == "projbld - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\projbld.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tasklist.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\vcarchy.cpp
# End Source File
# Begin Source File

SOURCE=.\vcbsnode.cpp
# End Source File
# Begin Source File

SOURCE=.\vcfgrp.cpp
# End Source File
# Begin Source File

SOURCE=.\vcfile.cpp
# End Source File
# Begin Source File

SOURCE=.\vcnode.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bsc.ico
# End Source File
# Begin Source File

SOURCE=.\buildpackage.rgs
# End Source File
# Begin Source File

SOURCE=.\res\c.ico
# End Source File
# Begin Source File

SOURCE=.\res\cpp.ico
# End Source File
# Begin Source File

SOURCE=.\res\h.ico
# End Source File
# Begin Source File

SOURCE=res\idr_appl.ico
# End Source File
# Begin Source File

SOURCE=res\idr_aslm.ico
# End Source File
# Begin Source File

SOURCE=res\idr_cons.ico
# End Source File
# Begin Source File

SOURCE=res\idr_dll.ico
# End Source File
# Begin Source File

SOURCE=res\idr_exte.ico
# End Source File
# Begin Source File

SOURCE=res\idr_libr.ico
# End Source File
# Begin Source File

SOURCE=res\idr_quic.ico
# End Source File
# Begin Source File

SOURCE=res\idr_stan.ico
# End Source File
# Begin Source File

SOURCE=res\imagelis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\projicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\rc.ico
# End Source File
# Begin Source File

SOURCE=.\res\solution.ico
# End Source File
# Begin Source File

SOURCE=.\res\text.ico
# End Source File
# Begin Source File

SOURCE=res\utilityp.ico
# End Source File
# Begin Source File

SOURCE=.\res\vague.ico
# End Source File
# End Group
# Begin Group "Build Events"

# PROP Default_Filter "bev"
# Begin Time Custom Build Step

SOURCE="..\engine\Post-Build"

!IF  "$(CFG)" == "projbld - Win32 Debug"

# PROP Custom_Build_Time "Post Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\Debug\projbld.dll
InputPath="..\engine\Post-Build"

	regsvr32 /s /c "$(TargetPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Debug"

# PROP BASE Custom_Build_Time "Post Build"
# PROP BASE Build_Tool "Custom Build"
# PROP Custom_Build_Time "Post Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\DebugU\projbld.dll
InputPath="..\engine\Post-Build"

	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinSize"

# PROP BASE Build_Tool "Custom Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\ReleaseMinSize\projbld.dll
InputPath="..\engine\Post-Build"

	regsvr32 /s /c "$(TargetPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "projbld - Win32 Release MinDependency"

# PROP BASE Build_Tool "Custom Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\ReleaseMinDependency\projbld.dll
InputPath="..\engine\Post-Build"

	regsvr32 /s /c "$(TargetPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinSize"

# PROP BASE Build_Tool "Custom Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\ReleaseUMinSize\projbld.dll
InputPath="..\engine\Post-Build"

	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "projbld - Win32 Unicode Release MinDependency"

# PROP BASE Build_Tool "Custom Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build - Performing registration
TargetPath=.\ReleaseUMinDependency\projbld.dll
InputPath="..\engine\Post-Build"

	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# End Time Custom Build Step
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bldguid_.h
# End Source File
# Begin Source File

SOURCE=.\bldpkg.h
# End Source File
# Begin Source File

SOURCE=.\buildinterface.h
# End Source File
# Begin Source File

SOURCE=.\dlink.h
# End Source File
# Begin Source File

SOURCE=.\exefordbg.h
# End Source File
# Begin Source File

SOURCE=.\filetime.h
# End Source File
# Begin Source File

SOURCE=.\global.inl
# End Source File
# Begin Source File

SOURCE=.\localeinfo.h
# End Source File
# Begin Source File

SOURCE=.\output.h
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\prjnode.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\rsrc.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tasklist.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\vcarchy.h
# End Source File
# Begin Source File

SOURCE=.\vcbsnode.h
# End Source File
# Begin Source File

SOURCE=.\vcfgrp.h
# End Source File
# Begin Source File

SOURCE=.\vcfile.h
# End Source File
# Begin Source File

SOURCE=.\VCNode.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
