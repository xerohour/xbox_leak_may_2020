# Microsoft Developer Studio Project File - Name="dsremote" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=dsremote - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dsremote.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsremote.mak" CFG="dsremote - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsremote - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "dsremote - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "dsremote"
# PROP Scc_LocalPath "..\..\..\..\..\.."

!IF  "$(CFG)" == "dsremote - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f dsremote.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dsremote.exe"
# PROP BASE Bsc_Name "dsremote.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\xbox\public\tools\razzle.cmd build -Z"
# PROP Rebuild_Opt "-c"
# PROP Target_File "dsremote.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dsremote - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f dsremote.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dsremote.exe"
# PROP BASE Bsc_Name "dsremote.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\xbox\public\tools\razzle.cmd build -Z"
# PROP Rebuild_Opt "-c"
# PROP Target_File "dsremote.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "dsremote - Win32 Release"
# Name "dsremote - Win32 Debug"

!IF  "$(CFG)" == "dsremote - Win32 Release"

!ELSEIF  "$(CFG)" == "dsremote - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ifileio.cpp
# End Source File
# Begin Source File

SOURCE=.\iremote.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h;*.hpp"
# Begin Source File

SOURCE=.\dsremi.h
# End Source File
# Begin Source File

SOURCE=.\dsremote.h
# End Source File
# Begin Source File

SOURCE=.\ifileio.h
# End Source File
# Begin Source File

SOURCE=.\iremote.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources.inc
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\public\sdk\inc\winsockx.h
# End Source File
# End Target
# End Project
