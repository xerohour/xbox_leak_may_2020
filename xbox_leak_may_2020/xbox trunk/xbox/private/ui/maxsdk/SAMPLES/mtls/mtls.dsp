# Microsoft Developer Studio Project File - Name="mtl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mtl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mtls.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mtls.mak" CFG="mtl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mtl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mtl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mtl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mtl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Yu"mtlhdr.h" /Fd"mtl.pdb" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\maxsdk\plugin\mtl.dlt" /release

!ELSEIF  "$(CFG)" == "mtl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"mtl.pdb" /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"mtlhdr.h" /Fd"mtl.pdb" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mtl.dlt"
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "mtl - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\mtl___Wi"
# PROP BASE Intermediate_Dir ".\mtl___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\MaxSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"mtlhdr.h" /Fd"mtl.pdb" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"mtlhdr.h" /Fd"mtl.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\maxsdk\plugin\mtl.dlt"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mtl.dlt"

!ENDIF 

# Begin Target

# Name "mtl - Win32 Release"
# Name "mtl - Win32 Debug"
# Name "mtl - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\acubic.cpp
# End Source File
# Begin Source File

SOURCE=.\bmtex.cpp
# End Source File
# Begin Source File

SOURCE=.\checker.cpp
# End Source File
# Begin Source File

SOURCE=.\cmtl.cpp
# End Source File
# Begin Source File

SOURCE=.\composit.cpp
# End Source File
# Begin Source File

SOURCE=.\composite_material.cpp
# End Source File
# Begin Source File

SOURCE=.\dblsided.cpp
# End Source File
# Begin Source File

SOURCE=.\falloff.cpp
# End Source File
# Begin Source File

SOURCE=.\gradient.cpp
# End Source File
# Begin Source File

SOURCE=.\marble.cpp
# End Source File
# Begin Source File

SOURCE=.\mask.cpp
# End Source File
# Begin Source File

SOURCE=.\matte.cpp
# End Source File
# Begin Source File

SOURCE=.\mirror.cpp
# End Source File
# Begin Source File

SOURCE=.\mix.cpp
# End Source File
# Begin Source File

SOURCE=.\mixmat.cpp
# End Source File
# Begin Source File

SOURCE=.\mtl.def
# End Source File
# Begin Source File

SOURCE=.\mtl.rc
# End Source File
# Begin Source File

SOURCE=.\mtlmain.cpp
# ADD CPP /Yc"mtlhdr.h"
# End Source File
# Begin Source File

SOURCE=.\multi.cpp
# End Source File
# Begin Source File

SOURCE=.\noise.cpp
# End Source File
# Begin Source File

SOURCE=.\output.cpp
# End Source File
# Begin Source File

SOURCE=.\partage.cpp
# End Source File
# Begin Source File

SOURCE=.\partblur.cpp
# End Source File
# Begin Source File

SOURCE=.\plate.cpp
# End Source File
# Begin Source File

SOURCE=.\rgbmult.cpp
# End Source File
# Begin Source File

SOURCE=.\stdmtl.cpp
# End Source File
# Begin Source File

SOURCE=.\stdmtl2.cpp
# End Source File
# Begin Source File

SOURCE=.\stdShaders.cpp
# End Source File
# Begin Source File

SOURCE=.\TexMaps.cpp
# End Source File
# Begin Source File

SOURCE=.\tint.cpp
# End Source File
# Begin Source File

SOURCE=.\vertcol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Lib\BMM.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Maxscrpt.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\mtlhdr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\shaders.h
# End Source File
# Begin Source File

SOURCE=.\stdmtl.h
# End Source File
# Begin Source File

SOURCE=.\stdmtl2.h
# End Source File
# Begin Source File

SOURCE=.\TexMaps.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\dmtlbut.bmp
# End Source File
# Begin Source File

SOURCE=.\dmtlmsk.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\preset_d.bmp
# End Source File
# Begin Source File

SOURCE=.\preset_r.bmp
# End Source File
# End Group
# End Target
# End Project
