# Microsoft Developer Studio Project File - Name="Edit_n_Go" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Edit_n_Go - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Editgo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Editgo.mak" CFG="Edit_n_Go - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Edit_n_Go - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Edit_n_Go - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Edit_n_Go - Win32 Release NoOptimizations" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cafe6/Tests/ProjBld/Edit-n-go/Src", BSPCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Edit_n_Go - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /ZI /O2 /I "..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X /u
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../bin/Editgo.pdb" /debug /machine:I386 /out:"../bin/Editgo.dll" /libpath:"..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Edit_n_Go - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../bin/editgod.pdb" /debug /machine:I386 /out:"../bin/editgod.dll" /libpath:"..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Edit_n_Go - Win32 Release NoOptimizations"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Edit_n_G"
# PROP BASE Intermediate_Dir "Edit_n_G"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PsuedoDebug"
# PROP Intermediate_Dir "PsuedoDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X /u
# ADD CPP /nologo /MD /W3 /WX /GR /GX /ZI /Od /I "..\..\..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X /u
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../bin/Editgo.dll" /libpath:"..\..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../bin/Editgo.dll" /libpath:"..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Edit_n_Go - Win32 Release"
# Name "Edit_n_Go - Win32 Debug"
# Name "Edit_n_Go - Win32 Release NoOptimizations"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugger.h
# End Source File
# Begin Source File

SOURCE=.\difcases.h
# End Source File
# Begin Source File

SOURCE=.\DllApi.h
# End Source File
# Begin Source File

SOURCE=.\mpdcases.h
# End Source File
# Begin Source File

SOURCE=.\percases.h
# End Source File
# Begin Source File

SOURCE=.\sniff.h
# End Source File
# Begin Source File

SOURCE=.\spycases.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\wizcases.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\difcases.cpp
# End Source File
# Begin Source File

SOURCE=.\mpdcases.cpp
# End Source File
# Begin Source File

SOURCE=.\percases.cpp
# End Source File
# Begin Source File

SOURCE=.\sniff.cpp
# End Source File
# Begin Source File

SOURCE=.\spycases.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\wizcases.cpp
# End Source File
# End Group
# Begin Group "guitools and ide headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\cobp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\cobrowse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\coclip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\cocompon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\coconnec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\cocp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\codam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\codebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\coee.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\coenv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\coexcpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\cofile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\src\cohtml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\comem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\connec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\coprject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\coreg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\coresscr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\src\cosource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\costack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\cothread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\cowrkspc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\dbgxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\dockids.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\edsxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\export.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\src\export.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\wrk\export.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\filetabl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\imewrap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\macrodlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\menuids.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\newfldlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\odbcdlgs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\prjxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\projprop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\props.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\qcqp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\shrdres.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\slob.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\testutil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\uaccedit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uappwiz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\ucommdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\ucustdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\ucwzdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\udialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\udlgedit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\udockwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\ueditor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\ufindinf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\src\ufindrep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uibp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uibrowse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uiconnec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uiconset.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uicp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uictrlwz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uidam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uidebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uieewnd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uiexcpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uiextwiz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uilocals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uimem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\uimgedit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\src\uinsctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uioptdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uioutput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uiqw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uireg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uistack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uitabdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uitarget.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uithread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uivar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\uiwatch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uiwbdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uiwbmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uiwizbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\wrk\uiwrkspc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\umainfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\umenedit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\umsgbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uogaldlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uprjfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uprjtdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\uprojwiz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\upropwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\uresbrws.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\uresedit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\urestabl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\ustredit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\usymdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\utbardlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\utoolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\prj\utstapwz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\res\uveredit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\uwbframe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\uwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\wb_props.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\shl\wbutil.h
# End Source File
# End Group
# Begin Group "Cafe/include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\include\guitarg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\guiv1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\guixprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\idetarg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\idexprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\ime32.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\mstwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\shlxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\subsuite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\support.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\t4ctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\target.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\targxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\test.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\testxcpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\toolset.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\toolxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\w32repl.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Bin\editgo.stf
# End Source File
# End Target
# End Project
