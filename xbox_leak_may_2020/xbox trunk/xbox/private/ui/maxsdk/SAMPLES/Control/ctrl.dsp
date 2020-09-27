# Microsoft Developer Studio Project File - Name="ctrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ctrl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Ctrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ctrl.mak" CFG="ctrl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ctrl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctrl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ctrl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Yu"ctrl.h" /Fd"ctrl.pdb" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\maxsdk\plugin\ctrl.dlc" /release

!ELSEIF  "$(CFG)" == "ctrl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"ctrl.pdb" /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"ctrl.h" /Fd"ctrl.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\ctrl.dlc"

!ELSEIF  "$(CFG)" == "ctrl - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ctrl___W"
# PROP BASE Intermediate_Dir ".\ctrl___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"ctrl.h" /Fd"ctrl.pdb" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"ctrl.h" /Fd"ctrl.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\ctrl.dlc"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\ctrl.dlc"

!ENDIF 

# Begin Target

# Name "ctrl - Win32 Release"
# Name "ctrl - Win32 Debug"
# Name "ctrl - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\attach.cpp
# End Source File
# Begin Source File

SOURCE=.\boolctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\control.rc
# End Source File
# Begin Source File

SOURCE=.\ctrl.cpp
# ADD CPP /Yc"ctrl.h"
# End Source File
# Begin Source File

SOURCE=.\ctrl.def
# End Source File
# Begin Source File

SOURCE=.\eulrctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\exprctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\followu.cpp
# End Source File
# Begin Source File

SOURCE=.\indepos.cpp
# End Source File
# Begin Source File

SOURCE=.\indescale.cpp
# End Source File
# Begin Source File

SOURCE=.\linkctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\loceulrc.cpp
# End Source File
# Begin Source File

SOURCE=.\lodctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\noizctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\pathctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\perlin.cpp
# ADD CPP /W1
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\surfctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\EXPR.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\maxutil.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CTRL.H
# End Source File
# Begin Source File

SOURCE=.\noise.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
