# Microsoft Developer Studio Project File - Name="GUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=GUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GUI.mak" CFG="GUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GUI - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "GUI - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "GUI"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "GUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f GUI.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "GUI.exe"
# PROP BASE Bsc_Name "GUI.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\xbox\public\tools\razzle.cmd build -Z"
# PROP Rebuild_Opt "-c"
# PROP Target_File "Wbnd32.exe"
# PROP Bsc_Name "GUI.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "GUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GUI___Win32_Debug"
# PROP BASE Intermediate_Dir "GUI___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f GUI.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "GUI.exe"
# PROP BASE Bsc_Name "GUI.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GUI___Win32_Debug"
# PROP Intermediate_Dir "GUI___Win32_Debug"
# PROP Cmd_Line "d:\xbox\public\tools\razzle.cmd build -Z"
# PROP Rebuild_Opt "-c"
# PROP Target_File "Wbnd32.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "GUI - Win32 Release"
# Name "GUI - Win32 Debug"

!IF  "$(CFG)" == "GUI - Win32 Release"

!ELSEIF  "$(CFG)" == "GUI - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gui.rc
# End Source File
# Begin Source File

SOURCE=.\listview.cpp
# End Source File
# Begin Source File

SOURCE=.\mru.cpp
# End Source File
# Begin Source File

SOURCE=.\sbgui.cpp
# End Source File
# Begin Source File

SOURCE=.\version.rc
# End Source File
# Begin Source File

SOURCE=.\winmain.cpp
# End Source File
# Begin Source File

SOURCE=.\wndwrap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\listview.h
# End Source File
# Begin Source File

SOURCE=.\mru.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sbgui.h
# End Source File
# Begin Source File

SOURCE=.\wbndgui.h
# End Source File
# Begin Source File

SOURCE=.\winmain.h
# End Source File
# Begin Source File

SOURCE=.\wndwrap.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bank.ico
# End Source File
# Begin Source File

SOURCE=.\entry.ico
# End Source File
# Begin Source File

SOURCE=.\project.ico
# End Source File
# Begin Source File

SOURCE=.\toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
