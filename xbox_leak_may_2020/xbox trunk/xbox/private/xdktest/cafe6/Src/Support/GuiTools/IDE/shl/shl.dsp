# Microsoft Developer Studio Project File - Name="shl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=shl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shl.mak" CFG="shl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shl - Win32 Release NoOptimizations" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cafe6/Src/Support/GuiTools/IDE/shl", SKLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shl - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /WX /GR /GX /Zi /O1 /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXPORT_SHELL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/shl.pdb" /debug /machine:I386 /out:"../../../../../bin/shl.dll" /implib:"../../../../../lib/shl.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "shl - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXPORT_SHELL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/shld.pdb" /debug /machine:I386 /out:"../../../../../bin/shld.dll" /implib:"../../../../../lib/shld.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "shl - Win32 Release NoOptimizations"

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
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXPORT_SHELL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXPORT_SHELL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/shl.pdb" /debug /machine:I386 /out:"../../../../../bin/shl.dll" /implib:"../../../../../lib/shl.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../../bin/shl.pdb" /debug /machine:I386 /out:"../../../../../bin/shl.dll" /implib:"../../../../../lib/shl.lib" /libpath:"..\..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "shl - Win32 Release"
# Name "shl - Win32 Debug"
# Name "shl - Win32 Release NoOptimizations"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sym\auto1.h
# End Source File
# Begin Source File

SOURCE=..\sym\cmdids.h
# End Source File
# Begin Source File

SOURCE=.\coconnec.h
# End Source File
# Begin Source File

SOURCE=.\coenv.h
# End Source File
# Begin Source File

SOURCE=.\cowrkspc.h
# End Source File
# Begin Source File

SOURCE=..\sym\dockids.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\evnthdlr.h
# End Source File
# Begin Source File

SOURCE=.\newfldlg.h
# End Source File
# Begin Source File

SOURCE=..\sym\qcqp.h
# End Source File
# Begin Source File

SOURCE=..\sym\reg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\shlxprt.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\ucommdlg.h
# End Source File
# Begin Source File

SOURCE=.\ucustdlg.h
# End Source File
# Begin Source File

SOURCE=.\udockwnd.h
# End Source File
# Begin Source File

SOURCE=.\uiconnec.h
# End Source File
# Begin Source File

SOURCE=.\uiconset.h
# End Source File
# Begin Source File

SOURCE=.\uioptdlg.h
# End Source File
# Begin Source File

SOURCE=.\uioutput.h
# End Source File
# Begin Source File

SOURCE=.\uitabdlg.h
# End Source File
# Begin Source File

SOURCE=.\uitipdlg.h
# End Source File
# Begin Source File

SOURCE=.\uiwbdlg.h
# End Source File
# Begin Source File

SOURCE=.\uiwbmsg.h
# End Source File
# Begin Source File

SOURCE=.\umainfrm.h
# End Source File
# Begin Source File

SOURCE=.\uopenwrk.h
# End Source File
# Begin Source File

SOURCE=.\upropwnd.h
# End Source File
# Begin Source File

SOURCE=.\utbardlg.h
# End Source File
# Begin Source File

SOURCE=.\utoolbar.h
# End Source File
# Begin Source File

SOURCE=.\uwbframe.h
# End Source File
# Begin Source File

SOURCE=..\sym\vshell.h
# End Source File
# Begin Source File

SOURCE=.\wb.h
# End Source File
# Begin Source File

SOURCE=.\wb_props.h
# End Source File
# Begin Source File

SOURCE=.\wbutil.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cmdarray.cpp
# End Source File
# Begin Source File

SOURCE=.\coconnec.cpp
# End Source File
# Begin Source File

SOURCE=.\coenv.cpp
# End Source File
# Begin Source File

SOURCE=.\cowrkspc.cpp
# End Source File
# Begin Source File

SOURCE=.\evnthdlr.cpp
# End Source File
# Begin Source File

SOURCE=.\newfldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\shl.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\ucommdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ucustdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\udockwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\uiconnec.cpp
# End Source File
# Begin Source File

SOURCE=.\uiconset.cpp
# End Source File
# Begin Source File

SOURCE=.\uioptdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uioutput.cpp
# End Source File
# Begin Source File

SOURCE=.\uitabdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uitipdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uiwbdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\uiwbmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\umainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\uopenwrk.cpp
# End Source File
# Begin Source File

SOURCE=.\upropwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\utbardlg.cpp
# End Source File
# Begin Source File

SOURCE=.\utoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\uwbframe.cpp
# End Source File
# Begin Source File

SOURCE=.\wbutil.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
