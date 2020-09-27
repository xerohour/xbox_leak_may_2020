# Microsoft Developer Studio Project File - Name="xtncommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=xtncommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xtncommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xtncommon.mak" CFG="xtncommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xtncommon - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "xtncommon - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "xtncommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f xtncommon.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtncommon.exe"
# PROP BASE Bsc_Name "xtncommon.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "xtncommon.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtncommon.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "xtncommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xtncommon___Win32_Debug"
# PROP BASE Intermediate_Dir "xtncommon___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f xtncommon.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtncommon.exe"
# PROP BASE Bsc_Name "xtncommon.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xtncommon___Win32_Debug"
# PROP Intermediate_Dir "xtncommon___Win32_Debug"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\tncommon\xbox"
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtncommon.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "xtncommon - Win32 Release"
# Name "xtncommon - Win32 Debug"

!IF  "$(CFG)" == "xtncommon - Win32 Release"

!ELSEIF  "$(CFG)" == "xtncommon - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\checkver.c
# End Source File
# Begin Source File

SOURCE=.\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\convhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\cppobjhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\debugprint.cpp
# End Source File
# Begin Source File

SOURCE=.\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\excptns.cpp
# End Source File
# Begin Source File

SOURCE=.\fileutils.cpp
# End Source File
# Begin Source File

SOURCE=.\linkedstr.cpp
# End Source File
# Begin Source File

SOURCE=.\linklist.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\sprintf.cpp
# End Source File
# Begin Source File

SOURCE=.\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\strutils.cpp
# End Source File
# Begin Source File

SOURCE=.\symbols.cpp
# End Source File
# Begin Source File

SOURCE=.\text.cpp
# End Source File
# Begin Source File

SOURCE=.\verutils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\checkver.h
# End Source File
# Begin Source File

SOURCE=.\cmdline.h
# End Source File
# Begin Source File

SOURCE=.\convhelp.h
# End Source File
# Begin Source File

SOURCE=.\cppobjhelp.h
# End Source File
# Begin Source File

SOURCE=.\crc32.h
# End Source File
# Begin Source File

SOURCE=.\debugprint.h
# End Source File
# Begin Source File

SOURCE=.\errors.h
# End Source File
# Begin Source File

SOURCE=.\excptns.h
# End Source File
# Begin Source File

SOURCE=.\fileutils.h
# End Source File
# Begin Source File

SOURCE=.\linkedstr.h
# End Source File
# Begin Source File

SOURCE=.\linklist.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sprintf.h
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=.\strutils.h
# End Source File
# Begin Source File

SOURCE=.\symbols.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\verutils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\tncommon.rc
# End Source File
# End Group
# End Target
# End Project
