# Microsoft Developer Studio Project File - Name="MXSAgni" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MXSAgni - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MXSAgni.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MXSAgni.mak" CFG="MXSAgni - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MXSAgni - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MXSAgni - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MXSAgni - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MXSAgni - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /Zm150 /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 bmm.lib MNMath.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /libpath:"..\..\..\..\lib" /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MXSAgni - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib bmm.lib MNMath.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /pdbtype:sept /libpath:"..\..\..\..\lib"

!ELSEIF  "$(CFG)" == "MXSAgni - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Hybrid"
# PROP BASE Intermediate_Dir "Hybrid"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Hybrid"
# PROP Intermediate_Dir "Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /GX /Zi /Od /I "." /I ".\.." /I "..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\MXSAgni.dlx" /pdbtype:sept
# ADD LINK32 bmm.lib MNMath.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /pdbtype:sept /libpath:"..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MXSAgni - Win32 Release"
# Name "MXSAgni - Win32 Debug"
# Name "MXSAgni - Win32 Hybrid"
# Begin Source File

SOURCE=.\bmatpro.h
# End Source File
# Begin Source File

SOURCE=.\ClassCfg.h
# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtClass.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtClass.h
# End Source File
# Begin Source File

SOURCE=.\ExtFuncs.h
# End Source File
# Begin Source File

SOURCE=.\ExtKeys.h
# End Source File
# Begin Source File

SOURCE=.\MXSAgni.cpp
# End Source File
# Begin Source File

SOURCE=.\MXSAgni.def
# End Source File
# Begin Source File

SOURCE=.\MXSAgni.h
# End Source File
# Begin Source File

SOURCE=.\MXSAgni.rc
# End Source File
# Begin Source File

SOURCE=.\phyblpro.h
# End Source File
# Begin Source File

SOURCE=.\PHYEXP.H
# End Source File
# Begin Source File

SOURCE=.\PhyExprt.cpp
# End Source File
# Begin Source File

SOURCE=.\PhyExprt.h
# End Source File
# Begin Source File

SOURCE=.\phymcpro.h
# End Source File
# Begin Source File

SOURCE=.\phyrgpro.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
