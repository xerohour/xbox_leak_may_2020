# Microsoft Developer Studio Project File - Name="engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak" CFG="engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engine - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "engine - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f engine.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "engine.exe"
# PROP BASE Bsc_Name "engine.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "engine.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "xacteng.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "engine___Win32_Debug"
# PROP BASE Intermediate_Dir "engine___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f engine.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "engine.exe"
# PROP BASE Bsc_Name "engine.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "engine___Win32_Debug"
# PROP Intermediate_Dir "engine___Win32_Debug"
# PROP Cmd_Line "c:\xbox\private\developr\georgioc\razzle.cmd build -Z"
# PROP Rebuild_Opt "build -cz"
# PROP Target_File "xactengd.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"

!IF  "$(CFG)" == "engine - Win32 Release"

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\common.cpp
# End Source File
# Begin Source File

SOURCE=.\cue.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\sequencer.cpp
# End Source File
# Begin Source File

SOURCE=.\soundbank.cpp
# End Source File
# Begin Source File

SOURCE=.\soundsource.cpp
# End Source File
# Begin Source File

SOURCE=.\wavebank.cpp
# End Source File
# Begin Source File

SOURCE=.\xactapi.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\common.h
# End Source File
# Begin Source File

SOURCE=..\common\debug.h
# End Source File
# Begin Source File

SOURCE=..\common\drvhlp.h
# End Source File
# Begin Source File

SOURCE=..\common\macros.h
# End Source File
# Begin Source File

SOURCE=..\common\memmgr.h
# End Source File
# Begin Source File

SOURCE=..\common\ntlist.h
# End Source File
# Begin Source File

SOURCE=..\common\refcount.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\genx\directx\xact\inc\xact.x
# End Source File
# Begin Source File

SOURCE=..\inc\xacti.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\inc\xactp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sources
# End Source File
# End Group
# End Target
# End Project
