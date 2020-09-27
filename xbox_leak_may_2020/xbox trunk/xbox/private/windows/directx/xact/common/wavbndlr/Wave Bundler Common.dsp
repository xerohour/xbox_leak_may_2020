# Microsoft Developer Studio Project File - Name="Wave Bundler Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Wave Bundler Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wave Bundler Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wave Bundler Common.mak" CFG="Wave Bundler Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wave Bundler Common - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Wave Bundler Common - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Wave Bundler Common"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Wave Bundler Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f "Wave Bundler Common.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Wave Bundler Common.exe"
# PROP BASE Bsc_Name "Wave Bundler Common.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "Wave Bundler Common.mak""
# PROP Rebuild_Opt "-c"
# PROP Target_File "xactwbc.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Wave Bundler Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f "Wave Bundler Common.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Wave Bundler Common.exe"
# PROP BASE Bsc_Name "Wave Bundler Common.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\xbox\public\tools\razzle.cmd build -Z"
# PROP Rebuild_Opt "-c"
# PROP Target_File "xactwbcd.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Wave Bundler Common - Win32 Release"
# Name "Wave Bundler Common - Win32 Debug"

!IF  "$(CFG)" == "Wave Bundler Common - Win32 Release"

!ELSEIF  "$(CFG)" == "Wave Bundler Common - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\filter.cpp
# End Source File
# Begin Source File

SOURCE=.\format2.cpp
# End Source File
# Begin Source File

SOURCE=.\loader.cpp
# End Source File
# Begin Source File

SOURCE=.\reader.cpp
# End Source File
# Begin Source File

SOURCE=.\writer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\filter.h
# End Source File
# Begin Source File

SOURCE=.\loader.h
# End Source File
# Begin Source File

SOURCE=.\wavbndli.h
# End Source File
# Begin Source File

SOURCE=.\writer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
