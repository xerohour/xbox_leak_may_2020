# Microsoft Developer Studio Project File - Name="DMCoreTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DMCoreTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DMCoreTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DMCoreTest.mak" CFG="DMCoreTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DMCoreTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DMCoreTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DMCoreTest - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DMCoreTest - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DMCoreTest - Win32 Release"
# Name "DMCoreTest - Win32 Debug"
# Begin Group "Kernel Files"

# PROP Default_Filter ""
# Begin Group "Kernel Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AllocatorMXF.cpp
# End Source File
# Begin Source File

SOURCE=.\BasicMXF.cpp
# End Source File
# Begin Source File

SOURCE=.\PackerMXF.cpp
# End Source File
# Begin Source File

SOURCE=.\SequencerMXF.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterMXF.cpp
# End Source File
# Begin Source File

SOURCE=.\UnpackerMXF.cpp
# End Source File
# End Group
# Begin Group "Kernel Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AllocatorMXF.h
# End Source File
# Begin Source File

SOURCE=.\BasicMXF.h
# End Source File
# Begin Source File

SOURCE=.\MXF.h
# End Source File
# Begin Source File

SOURCE=.\PackerMXF.h
# End Source File
# Begin Source File

SOURCE=.\SequencerMXF.h
# End Source File
# Begin Source File

SOURCE=.\SplitterMXF.h
# End Source File
# Begin Source File

SOURCE=.\UnpackerMXF.h
# End Source File
# End Group
# End Group
# Begin Group "Test Files"

# PROP Default_Filter ""
# Begin Group "Test Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DMCoreTest.cpp
# End Source File
# Begin Source File

SOURCE=.\DMCoreTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ksmiditest.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Test Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DMCoreTest.h
# End Source File
# Begin Source File

SOURCE=.\DMCoreTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Test Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\DMCoreTest.ico
# End Source File
# Begin Source File

SOURCE=.\DMCoreTest.rc

!IF  "$(CFG)" == "DMCoreTest - Win32 Release"

!ELSEIF  "$(CFG)" == "DMCoreTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\DMCoreTest.rc2
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
