# Microsoft Developer Studio Project File - Name="tiflib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tiflib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Tiflib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tiflib.mak" CFG="tiflib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tiflib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tiflib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tiflib - Win32 Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tiflib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fd"tiflib.pdb" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tiflib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\tiflib__"
# PROP BASE Intermediate_Dir ".\tiflib__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fd"tiflib.pdb" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tiflib - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Hybrid"
# PROP BASE Intermediate_Dir ".\Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /Z7 /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fd"tiflib.pdb" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "tiflib - Win32 Release"
# Name "tiflib - Win32 Debug"
# Name "tiflib - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\GeoTIFF_Read.cpp
# End Source File
# Begin Source File

SOURCE=.\TIF_CFAX.C
# End Source File
# Begin Source File

SOURCE=.\TIF_CLSE.C
# End Source File
# Begin Source File

SOURCE=.\TIF_COMP.C
# End Source File
# Begin Source File

SOURCE=.\TIF_CRLE.C
# End Source File
# Begin Source File

SOURCE=.\TIF_DIR.C
# End Source File
# Begin Source File

SOURCE=.\TIF_DUMP.C
# End Source File
# Begin Source File

SOURCE=.\TIF_FLSH.C
# End Source File
# Begin Source File

SOURCE=.\TIF_LZW.C
# End Source File
# Begin Source File

SOURCE=.\tif_misc.c
# End Source File
# Begin Source File

SOURCE=.\TIF_NEXT.C
# End Source File
# Begin Source File

SOURCE=.\TIF_OPEN.C
# End Source File
# Begin Source File

SOURCE=.\TIF_PACK.C
# End Source File
# Begin Source File

SOURCE=.\TIF_PIC.C
# End Source File
# Begin Source File

SOURCE=.\TIF_PRNT.C
# End Source File
# Begin Source File

SOURCE=.\TIF_READ.C
# End Source File
# Begin Source File

SOURCE=.\TIF_SGI.C
# End Source File
# Begin Source File

SOURCE=.\TIF_SWAB.C
# End Source File
# Begin Source File

SOURCE=.\TIF_THDR.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\compat.h
# End Source File
# Begin Source File

SOURCE=.\geokeys.h
# End Source File
# Begin Source File

SOURCE=.\geovalues.h
# End Source File
# Begin Source File

SOURCE=.\gtiffio.h
# End Source File
# Begin Source File

SOURCE=.\machdep.h
# End Source File
# Begin Source File

SOURCE=.\tif_misc.h
# End Source File
# Begin Source File

SOURCE=.\tif_port.h
# End Source File
# Begin Source File

SOURCE=.\tiffdefs.h
# End Source File
# Begin Source File

SOURCE=.\tiffio.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
