# Microsoft Developer Studio Project File - Name="Morpher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Morpher - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Morpher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Morpher.mak" CFG="Morpher - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Morpher - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Morpher - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Morpher - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Morpher - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /libpath:"..\..\..\lib" /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Morpher - Win32 Debug"

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
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /pdbtype:sept /libpath:"..\..\..\lib"

!ELSEIF  "$(CFG)" == "Morpher - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Morpher_"
# PROP BASE Intermediate_Dir "Morpher_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Hybrid"
# PROP Intermediate_Dir "Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\include" /I "..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x00aa0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\maxsdk\plugin\Morpher.dlm" /pdbtype:sept
# ADD LINK32 maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /pdbtype:sept /libpath:"..\..\..\lib"

!ENDIF 

# Begin Target

# Name "Morpher - Win32 Release"
# Name "Morpher - Win32 Debug"
# Name "Morpher - Win32 Hybrid"
# Begin Group "Morph Plugin"

# PROP Default_Filter ""
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Group "Resource Files"

# PROP Default_Filter "*.bmp;*.ico;*.cur"
# Begin Source File

SOURCE=.\morph16.bmp
# End Source File
# Begin Source File

SOURCE=.\morph16m.bmp
# End Source File
# Begin Source File

SOURCE=.\morph32.bmp
# End Source File
# Begin Source File

SOURCE=.\morph32m.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\wM3.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\wM3.h
# End Source File
# Begin Source File

SOURCE=.\wM3_cache.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_channel.cpp
# End Source File
# Begin Source File

SOURCE=".\wM3_dlg-fns.cpp"
# End Source File
# Begin Source File

SOURCE=.\wM3_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_impexp.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_io.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_main.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_pick.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_subdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_undo.cpp
# End Source File
# End Group
# Begin Group "Morph Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wM3_core.cpp
# End Source File
# End Group
# Begin Group "Morph Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wM3_material.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_material_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\wM3_material_pick.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\wM3.def
# End Source File
# Begin Source File

SOURCE=.\wM3_devel.txt
# End Source File
# End Target
# End Project
