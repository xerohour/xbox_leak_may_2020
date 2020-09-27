# Microsoft Developer Studio Project File - Name="idl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=idl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "idl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "idl.mak" CFG="idl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "idl - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "idl - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "idl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f idl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "idl.exe"
# PROP BASE Bsc_Name "idl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f vcidl.mak"
# PROP Rebuild_Opt ""
# PROP Target_File "..\lib\i386\vcidl.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "idl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f idl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "idl.exe"
# PROP BASE Bsc_Name "idl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f vcidl.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\lib\i386\vcidl.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "idl - Win32 Release"
# Name "idl - Win32 Debug"

!IF  "$(CFG)" == "idl - Win32 Release"

!ELSEIF  "$(CFG)" == "idl - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bined.idl
# End Source File
# Begin Source File

SOURCE=.\ncb.idl
# End Source File
# Begin Source File

SOURCE=.\ProjBld.idl
# End Source File
# Begin Source File

SOURCE=.\respkg.idl
# End Source File
# Begin Source File

SOURCE=.\vc.idl
# End Source File
# Begin Source File

SOURCE=.\vcclsvw.idl
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\makefile.inc

!IF  "$(CFG)" == "idl - Win32 Release"

!ELSEIF  "$(CFG)" == "idl - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sources

!IF  "$(CFG)" == "idl - Win32 Release"

!ELSEIF  "$(CFG)" == "idl - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vcidl.mak

!IF  "$(CFG)" == "idl - Win32 Release"

!ELSEIF  "$(CFG)" == "idl - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
