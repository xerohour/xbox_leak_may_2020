# Microsoft Developer Studio Project File - Name="xtnshell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=xtnshell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xtnshell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xtnshell.mak" CFG="xtnshell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xtnshell - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "xtnshell - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "xtnshell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f xtnshell.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtnshell.exe"
# PROP BASE Bsc_Name "xtnshell.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\tnshell\dll\xbox"
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtnshell.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "xtnshell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xtnshell___Win32_Debug"
# PROP BASE Intermediate_Dir "xtnshell___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f xtnshell.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtnshell.exe"
# PROP BASE Bsc_Name "xtnshell.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xtnshell___Win32_Debug"
# PROP Intermediate_Dir "xtnshell___Win32_Debug"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\tnshell\dll\xbox"
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtnshell.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "xtnshell - Win32 Release"
# Name "xtnshell - Win32 Debug"

!IF  "$(CFG)" == "xtnshell - Win32 Release"

!ELSEIF  "$(CFG)" == "xtnshell - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dll\main.cpp
# End Source File
# Begin Source File

SOURCE=.\dll\prefs.cpp
# End Source File
# Begin Source File

SOURCE=.\dll\select.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll\main.h
# End Source File
# Begin Source File

SOURCE=.\dll\prefs.h
# End Source File
# Begin Source File

SOURCE=.\dll\resource.h
# End Source File
# Begin Source File

SOURCE=.\dll\select.h
# End Source File
# Begin Source File

SOURCE=.\dll\tnshell.h
# End Source File
# Begin Source File

SOURCE=.\dll\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\dll\tnshell.rc
# End Source File
# End Group
# End Target
# End Project
