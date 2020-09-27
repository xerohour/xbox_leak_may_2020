# Microsoft Developer Studio Project File - Name="core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=CORE - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core.mak" CFG="CORE - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "core - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/idetest/proj_bld/core", EFGAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "core - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f core.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "core.exe"
# PROP BASE Bsc_Name "core.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "echo Release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "core.exe"
# PROP Bsc_Name "core.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f core.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "core.exe"
# PROP BASE Bsc_Name "core.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "echo Debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "core.exe"
# PROP Bsc_Name "core.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "core - Win32 Release"
# Name "core - Win32 Debug"

!IF  "$(CFG)" == "core - Win32 Release"

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

!ENDIF 

# Begin Group "Common headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\..\include\test.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bin\core.stf
# End Source File
# End Target
# End Project
