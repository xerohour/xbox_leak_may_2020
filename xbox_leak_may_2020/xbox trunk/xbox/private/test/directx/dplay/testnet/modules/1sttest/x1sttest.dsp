# Microsoft Developer Studio Project File - Name="x1sttest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=x1sttest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "x1sttest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "x1sttest.mak" CFG="x1sttest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "x1sttest - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "x1sttest - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "x1sttest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f x1sttest.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "x1sttest.exe"
# PROP BASE Bsc_Name "x1sttest.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\modules\1sttest"
# PROP Rebuild_Opt "/a"
# PROP Target_File "x1sttest.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "x1sttest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "x1sttest___Win32_Debug"
# PROP BASE Intermediate_Dir "x1sttest___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f x1sttest.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "x1sttest.exe"
# PROP BASE Bsc_Name "x1sttest.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x1sttest___Win32_Debug"
# PROP Intermediate_Dir "x1sttest___Win32_Debug"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\modules\1sttest"
# PROP Rebuild_Opt "/a"
# PROP Target_File "x1sttest.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "x1sttest - Win32 Release"
# Name "x1sttest - Win32 Debug"

!IF  "$(CFG)" == "x1sttest - Win32 Release"

!ELSEIF  "$(CFG)" == "x1sttest - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\1sttest.cpp
# End Source File
# Begin Source File

SOURCE=.\t_main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\1sttest.h
# End Source File
# Begin Source File

SOURCE=.\t_main.h
# End Source File
# Begin Source File

SOURCE=.\t_version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\1sttest.rc
# End Source File
# End Group
# End Target
# End Project
