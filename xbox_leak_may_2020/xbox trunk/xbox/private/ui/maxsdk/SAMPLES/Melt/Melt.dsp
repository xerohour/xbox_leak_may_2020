# Microsoft Developer Studio Project File - Name="Melt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Melt - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Melt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Melt.mak" CFG="Melt - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Melt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Melt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Melt - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Melt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\maxsdk\plugin\melt.dlm" /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Melt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\melt.dlm" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Melt - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Melt___W"
# PROP BASE Intermediate_Dir "Melt___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"Melt.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\melt.dlm" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\melt.dlm" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Melt - Win32 Release"
# Name "Melt - Win32 Debug"
# Name "Melt - Win32 Hybrid"
# Begin Group "Library Files"

# PROP Default_Filter "*.LIB"
# Begin Source File

SOURCE=..\..\lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\MNMath.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\viewfile.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\MELT.CPP
# End Source File
# Begin Source File

SOURCE=.\MELT.DEF
# End Source File
# Begin Source File

SOURCE=.\Melt.rc
# End Source File
# End Target
# End Project
