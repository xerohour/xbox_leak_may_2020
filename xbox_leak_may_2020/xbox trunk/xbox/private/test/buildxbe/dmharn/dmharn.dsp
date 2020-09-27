# Microsoft Developer Studio Project File - Name="dmharn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=dmharn - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dmharn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dmharn.mak" CFG="dmharn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dmharn - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "dmharn - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "dmharn - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f dmharn.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dmharn.exe"
# PROP BASE Bsc_Name "dmharn.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f dmharn.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "dmharn.exe"
# PROP Bsc_Name "dmharn.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dmharn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f dmharn.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dmharn.exe"
# PROP BASE Bsc_Name "dmharn.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "%_NTDRIVE%%_NTROOT%\public\tools\razzle go.bat"
# PROP Rebuild_Opt ""
# PROP Target_File "dmharn.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "dmharn - Win32 Release"
# Name "dmharn - Win32 Debug"

!IF  "$(CFG)" == "dmharn - Win32 Release"

!ELSEIF  "$(CFG)" == "dmharn - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\modules.cpp
# End Source File
# End Group
# Begin Group "BAT files"

# PROP Default_Filter "*.bat, *.cmd"
# Begin Source File

SOURCE=.\go.bat
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
