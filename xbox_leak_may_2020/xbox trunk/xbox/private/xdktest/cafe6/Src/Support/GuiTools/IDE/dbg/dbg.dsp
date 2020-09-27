# Microsoft Developer Studio Project File - Name="dbg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbg.mak" CFG="dbg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbg - Win32 Release NoOptimizations" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cafe6/Src/Support/GuiTools/IDE/dbg", PKLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbg - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O1 /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_DEBUGGER" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/dbg.pdb" /debug /machine:I386 /out:"../../../../../bin/dbg.dll" /implib:"../../../../../lib/dbg.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dbg - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_DEBUGGER" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/dbgd.pdb" /debug /machine:I386 /out:"../../../../../bin/dbgd.dll" /implib:"../../../../../lib/dbgd.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dbg - Win32 Release NoOptimizations"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PseudoDebug"
# PROP Intermediate_Dir "PseudoDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_DEBUGGER" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "EXPORT_DEBUGGER" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/dbg.pdb" /debug /machine:I386 /out:"../../../../../bin/dbg.dll" /implib:"../../../../../lib/dbg.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/dbg.pdb" /debug /machine:I386 /out:"../../../../../bin/dbg.dll" /implib:"../../../../../lib/dbg.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "dbg - Win32 Release"
# Name "dbg - Win32 Debug"
# Name "dbg - Win32 Release NoOptimizations"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\coauto.cpp
# End Source File
# Begin Source File

SOURCE=.\cobp.cpp
# End Source File
# Begin Source File

SOURCE=.\cobrowse.cpp
# End Source File
# Begin Source File

SOURCE=.\cocp.cpp
# End Source File
# Begin Source File

SOURCE=.\codam.cpp
# End Source File
# Begin Source File

SOURCE=.\codebug.cpp
# End Source File
# Begin Source File

SOURCE=.\coee.cpp
# End Source File
# Begin Source File

SOURCE=.\coexcpt.cpp
# End Source File
# Begin Source File

SOURCE=.\colocals.cpp
# End Source File
# Begin Source File

SOURCE=.\comem.cpp
# End Source File
# Begin Source File

SOURCE=.\coreg.cpp
# End Source File
# Begin Source File

SOURCE=.\costack.cpp
# End Source File
# Begin Source File

SOURCE=.\cothread.cpp
# End Source File
# Begin Source File

SOURCE=.\cowatch.cpp
# End Source File
# Begin Source File

SOURCE=.\dbg.cpp
# End Source File
# Begin Source File

SOURCE=.\parse.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\uiattach.cpp
# End Source File
# Begin Source File

SOURCE=.\uibp.cpp
# End Source File
# Begin Source File

SOURCE=.\uibrowse.cpp
# End Source File
# Begin Source File

SOURCE=.\uicp.cpp
# End Source File
# Begin Source File

SOURCE=.\uidam.cpp
# End Source File
# Begin Source File

SOURCE=.\uidebug.cpp
# End Source File
# Begin Source File

SOURCE=.\uieewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\uiexcpt.cpp
# End Source File
# Begin Source File

SOURCE=.\uilocals.cpp
# End Source File
# Begin Source File

SOURCE=.\uimem.cpp
# End Source File
# Begin Source File

SOURCE=.\uiqw.cpp
# End Source File
# Begin Source File

SOURCE=.\uireg.cpp
# End Source File
# Begin Source File

SOURCE=.\uistack.cpp
# End Source File
# Begin Source File

SOURCE=.\uithread.cpp
# End Source File
# Begin Source File

SOURCE=.\uivar.cpp
# End Source File
# Begin Source File

SOURCE=.\uiwatch.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\coauto.h
# End Source File
# Begin Source File

SOURCE=.\cobp.h
# End Source File
# Begin Source File

SOURCE=.\cobrowse.h
# End Source File
# Begin Source File

SOURCE=.\cocp.h
# End Source File
# Begin Source File

SOURCE=.\codam.h
# End Source File
# Begin Source File

SOURCE=.\codebug.h
# End Source File
# Begin Source File

SOURCE=.\coee.h
# End Source File
# Begin Source File

SOURCE=.\coexcpt.h
# End Source File
# Begin Source File

SOURCE=.\colocals.h
# End Source File
# Begin Source File

SOURCE=.\comem.h
# End Source File
# Begin Source File

SOURCE=.\coreg.h
# End Source File
# Begin Source File

SOURCE=.\costack.h
# End Source File
# Begin Source File

SOURCE=.\cothread.h
# End Source File
# Begin Source File

SOURCE=.\cowatch.h
# End Source File
# Begin Source File

SOURCE=.\dbgxprt.h
# End Source File
# Begin Source File

SOURCE=.\parse.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\uiattach.h
# End Source File
# Begin Source File

SOURCE=.\uibp.h
# End Source File
# Begin Source File

SOURCE=.\uibrowse.h
# End Source File
# Begin Source File

SOURCE=.\uicp.h
# End Source File
# Begin Source File

SOURCE=.\uidam.h
# End Source File
# Begin Source File

SOURCE=.\uidebug.h
# End Source File
# Begin Source File

SOURCE=.\uieewnd.h
# End Source File
# Begin Source File

SOURCE=.\uiexcpt.h
# End Source File
# Begin Source File

SOURCE=.\uilocals.h
# End Source File
# Begin Source File

SOURCE=.\uimem.h
# End Source File
# Begin Source File

SOURCE=.\uiqw.h
# End Source File
# Begin Source File

SOURCE=.\uireg.h
# End Source File
# Begin Source File

SOURCE=.\uistack.h
# End Source File
# Begin Source File

SOURCE=.\uithread.h
# End Source File
# Begin Source File

SOURCE=.\uivar.h
# End Source File
# Begin Source File

SOURCE=.\uiwatch.h
# End Source File
# End Group
# End Target
# End Project
