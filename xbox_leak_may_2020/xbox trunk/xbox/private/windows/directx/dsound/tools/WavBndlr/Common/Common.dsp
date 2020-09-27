# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Common"
# PROP Scc_LocalPath "."
MTL=midl.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Common___Win32_Release"
# PROP BASE Intermediate_Dir "Common___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Common___Win32_Release"
# PROP Intermediate_Dir "Common___Win32_Release"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Common___Win32_Debug0"
# PROP BASE Intermediate_Dir "Common___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Common___Win32_Debug0"
# PROP Intermediate_Dir "Common___Win32_Debug0"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c;cpp;rc"
# Begin Source File

SOURCE=.\filter.cpp
# End Source File
# Begin Source File

SOURCE=.\format2.cpp
# End Source File
# Begin Source File

SOURCE=.\reader.cpp
# End Source File
# Begin Source File

SOURCE=.\wbcommon.cpp
# End Source File
# Begin Source File

SOURCE=.\writer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp"
# Begin Source File

SOURCE=.\filter.h
# End Source File
# Begin Source File

SOURCE=.\reader.h
# End Source File
# Begin Source File

SOURCE=.\wavbndli.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\genx\directx\dsound\inc\wavbndlr.h
# End Source File
# Begin Source File

SOURCE=.\writer.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\test\8bit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\adpcm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\compress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\loop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\mixed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\pcm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\test\pcmaiff.h
# End Source File
# End Target
# End Project
