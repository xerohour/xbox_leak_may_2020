# Microsoft Developer Studio Project File - Name="build" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=build - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "build.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "build.mak" CFG="build - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "build - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "build - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/idetest/proj_bld/core/build", ICHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "build - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\bin/build.dll" /libpath:"..\..\..\..\lib"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\bin/buildd.dll" /pdbtype:sept /libpath:"..\..\..\..\lib"

!ENDIF 

# Begin Target

# Name "build - Win32 Release"
# Name "build - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\bldatl.cpp
# End Source File
# Begin Source File

SOURCE=.\bldmfc.cpp

!IF  "$(CFG)" == "build - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bldmisc.cpp

!IF  "$(CFG)" == "build - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bldwin32.cpp

!IF  "$(CFG)" == "build - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\buildsub.cpp

!IF  "$(CFG)" == "build - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\prjset.cpp

!IF  "$(CFG)" == "build - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "build - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\bldatl.h
# End Source File
# Begin Source File

SOURCE=.\bldmfc.h
# End Source File
# Begin Source File

SOURCE=.\bldmisc.h
# End Source File
# Begin Source File

SOURCE=.\bldwin32.h
# End Source File
# Begin Source File

SOURCE=.\buildsub.h
# End Source File
# Begin Source File

SOURCE=..\prjset.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
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
# Begin Group "Cafe headers"

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

SOURCE=..\..\..\..\Src\support\guitools\ide\dbg\colocals.h
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

SOURCE=..\..\..\..\Src\support\guitools\imewrap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\eds\macrodlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\menuids.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\mstwrap.h
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

SOURCE=..\..\..\..\include\platform.h
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

SOURCE=..\..\..\..\include\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\shlxprt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\shrdres.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Src\support\guitools\ide\sym\slob.h
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

SOURCE=..\..\..\..\Src\support\guitools\testutil.h
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

SOURCE=..\..\..\..\src\support\guitools\ide\res\udlgedit.h
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
# End Group
# Begin Source File

SOURCE=..\bin\build.stf
# End Source File
# End Target
# End Project
