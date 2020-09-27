# Microsoft Developer Studio Project File - Name="prj" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=prj - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prj.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prj.mak" CFG="prj - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prj - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prj - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prj - Win32 Release NoOptimizations" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cafe6/Src/Support/GuiTools/IDE/prj", RKLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prj - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O1 /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_PROJECT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/prj.pdb" /debug /machine:I386 /out:"../../../../../bin/prj.dll" /implib:"../../../../../lib/prj.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "prj - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_PROJECT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/prjd.pdb" /debug /machine:I386 /out:"../../../../../bin/prjd.dll" /implib:"../../../../../lib/prjd.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>

!ELSEIF  "$(CFG)" == "prj - Win32 Release NoOptimizations"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PseudoDebug"
# PROP Intermediate_Dir "PseudoDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_PROJECT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_PROJECT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/prj.pdb" /debug /machine:I386 /out:"../../../../../bin/prj.dll" /implib:"../../../../../lib/prj.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/prj.pdb" /debug /machine:I386 /out:"../../../../../bin/prj.dll" /implib:"../../../../../lib/prj.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "prj - Win32 Release"
# Name "prj - Win32 Debug"
# Name "prj - Win32 Release NoOptimizations"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\sym\appwz.h
# End Source File
# Begin Source File

SOURCE=..\sym\awx.h
# End Source File
# Begin Source File

SOURCE=..\sym\clswiz.h
# End Source File
# Begin Source File

SOURCE=.\cocompon.h
# End Source File
# Begin Source File

SOURCE=.\coprject.h
# End Source File
# Begin Source File

SOURCE=..\sym\ctrlwz.h
# End Source File
# Begin Source File

SOURCE=.\odbcdlgs.h
# End Source File
# Begin Source File

SOURCE=..\sym\optncplr.h
# End Source File
# Begin Source File

SOURCE=..\sym\optnlink.h
# End Source File
# Begin Source File

SOURCE=..\sym\optnrc.h
# End Source File
# Begin Source File

SOURCE=.\prjxprt.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\uappwiz.h
# End Source File
# Begin Source File

SOURCE=.\ucwzdlg.h
# End Source File
# Begin Source File

SOURCE=.\uictrlwz.h
# End Source File
# Begin Source File

SOURCE=.\uidepdlg.h
# End Source File
# Begin Source File

SOURCE=.\uiextwiz.h
# End Source File
# Begin Source File

SOURCE=.\uitarget.h
# End Source File
# Begin Source File

SOURCE=.\uiwizbar.h
# End Source File
# Begin Source File

SOURCE=.\uogaldlg.h
# End Source File
# Begin Source File

SOURCE=.\uprjfile.h
# End Source File
# Begin Source File

SOURCE=.\uprjtdlg.h
# End Source File
# Begin Source File

SOURCE=.\uprojwiz.h
# End Source File
# Begin Source File

SOURCE=.\utstapwz.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat"
# Begin Source File

SOURCE=.\cocompon.cpp
# End Source File
# Begin Source File

SOURCE=.\coprject.cpp
# End Source File
# Begin Source File

SOURCE=.\odbcdlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\prj.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\uappwiz.cpp
# End Source File
# Begin Source File

SOURCE=.\ucwzdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uictrlwz.cpp
# End Source File
# Begin Source File

SOURCE=.\uidepdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uiextwiz.cpp
# End Source File
# Begin Source File

SOURCE=.\uitarget.cpp
# End Source File
# Begin Source File

SOURCE=.\uiwizbar.cpp
# End Source File
# Begin Source File

SOURCE=.\uogaldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uprjfile.cpp
# End Source File
# Begin Source File

SOURCE=.\uprjtdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uprojwiz.cpp
# End Source File
# Begin Source File

SOURCE=.\utstapwz.cpp
# End Source File
# End Group
# End Target
# End Project
