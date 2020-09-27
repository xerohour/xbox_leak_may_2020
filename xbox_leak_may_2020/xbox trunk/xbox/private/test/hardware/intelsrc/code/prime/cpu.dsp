# Microsoft Developer Studio Project File - Name="cpu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cpu - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cpu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cpu.mak" CFG="cpu - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cpu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cpu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WinMTA Tests/CPU", GECAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cpu - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GM /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FAcs /Yu"stdafx.h" /FD /GM /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "cpu - Win32 Release"
# Name "cpu - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\cpu.cpp
# End Source File
# Begin Source File

SOURCE=.\cpu.rc
# End Source File
# Begin Source File

SOURCE=.\FFT.CPP
# End Source File
# Begin Source File

SOURCE=.\LUCAS64.ASM

!IF  "$(CFG)" == "cpu - Win32 Release"

USERDEP__LUCAS="common.mac"	"lucas1.mac"	"lucas2.mac"	"normal.mac"	"pfa.mac"	"unravel.mac"	
# Begin Custom Build
InputPath=.\LUCAS64.ASM

"lucas64.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe /nologo /I. /Zm /Zi /c  /coff /FR /Cx /FlNUL.lst /Ta lucas64.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

USERDEP__LUCAS="common.mac"	"lucas1.mac"	"lucas2.mac"	"normal.mac"	"pfa.mac"	"unravel.mac"	
# Begin Custom Build
InputPath=.\LUCAS64.ASM

"lucas64.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe /nologo /I. /Zm /Zi /c  /coff /FR /Cx /FlNUL.lst /Ta lucas64.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mmx.asm

!IF  "$(CFG)" == "cpu - Win32 Release"

# Begin Custom Build
InputPath=.\mmx.asm

"mmx.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Zi /W3 /coff /c mmx.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\xmm.asm

!IF  "$(CFG)" == "cpu - Win32 Release"

# Begin Custom Build
InputPath=.\xmm.asm

"xmm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /Zi /W3 /coff /c xmm.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

# Begin Custom Build
InputPath=.\xmm.asm

"xmm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe /nologo /I. /Zm /Zi /c  /coff /FR /Cx /FlNUL.lst /Ta xmm.asm

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd;mac"
# Begin Source File

SOURCE=.\COMMON.MAC
# End Source File
# Begin Source File

SOURCE=.\cpu.h
# End Source File
# Begin Source File

SOURCE=.\dmi16.h
# End Source File
# Begin Source File

SOURCE=.\LUCAS1.MAC
# End Source File
# Begin Source File

SOURCE=.\LUCAS2.MAC
# End Source File
# Begin Source File

SOURCE=.\NORMAL.MAC
# End Source File
# Begin Source File

SOURCE=.\PFA.MAC
# End Source File
# Begin Source File

SOURCE=.\Prime95.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UNRAVEL.MAC
# End Source File
# Begin Source File

SOURCE=.\wtlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cpu.ico
# End Source File
# Begin Source File

SOURCE=.\res\cpu.rc2
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf;hpj"
# Begin Source File

SOURCE=.\hlp\CPU.CNT
# End Source File
# Begin Source File

SOURCE=.\hlp\CPU.HPJ

!IF  "$(CFG)" == "cpu - Win32 Release"

USERDEP__CPU_H="hlp\cpu.rtf"	
# Begin Custom Build - Compiling help...
InputPath=.\hlp\CPU.HPJ

"hlp\cpu.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehelp.bat cpu

# End Custom Build

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

USERDEP__CPU_H="hlp\cpu.rtf"	
# Begin Custom Build - Compiling help...
InputPath=.\hlp\CPU.HPJ

"hlp\cpu.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehelp.bat cpu

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\cpu.rtf
# End Source File
# Begin Source File

SOURCE=.\makehelp.bat
# End Source File
# End Group
# Begin Source File

SOURCE=.\testlibc.lib

!IF  "$(CFG)" == "cpu - Win32 Release"

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\testlibd.lib

!IF  "$(CFG)" == "cpu - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cpu - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
