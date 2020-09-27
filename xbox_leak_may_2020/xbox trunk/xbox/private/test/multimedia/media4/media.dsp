# Microsoft Developer Studio Project File - Name="media" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=media - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "media.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "media.mak" CFG="media - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "media - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "media - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "media - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f media.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "media.exe"
# PROP BASE Bsc_Name "media.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "go.bat"
# PROP Rebuild_Opt "/a"
# PROP Target_File "media.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "media - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f media.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "media.exe"
# PROP BASE Bsc_Name "media.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "%_NTDRIVE%%_NTROOT%\public\tools\razzle b.bat"
# PROP Rebuild_Opt ""
# PROP Target_File "media.xbe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "media - Win32 Release"
# Name "media - Win32 Debug"

!IF  "$(CFG)" == "media - Win32 Release"

!ELSEIF  "$(CFG)" == "media - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Ball.cpp
# End Source File
# Begin Source File

SOURCE=.\bitfont.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\Input.cpp
# End Source File
# Begin Source File

SOURCE=.\InvertedBall.cpp
# End Source File
# Begin Source File

SOURCE=.\media.cpp
# End Source File
# Begin Source File

SOURCE=.\Music.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicHeapFixed.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicHeapFixedCache.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicHeapTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\WireBall.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Ball.h
# End Source File
# Begin Source File

SOURCE=.\bitfont.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Helpers.h
# End Source File
# Begin Source File

SOURCE=.\Input.h
# End Source File
# Begin Source File

SOURCE=.\InvertedBall.h
# End Source File
# Begin Source File

SOURCE=.\media.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\MusicHeap.h
# End Source File
# Begin Source File

SOURCE=.\scene.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=.\WireBall.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files (External)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\public\sdk\inc\dmusici.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\public\sdk\inc\dsound.h
# End Source File
# Begin Source File

SOURCE=..\inc\macros.h
# End Source File
# End Group
# Begin Group "INI"

# PROP Default_Filter "*.ini"
# Begin Source File

SOURCE=.\TestParams\2Balls.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\30Balls.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\allparams.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\boing.txt
# End Source File
# Begin Source File

SOURCE=.\TestParams\default.txt
# End Source File
# Begin Source File

SOURCE=.\MediaFiles.ini
# End Source File
# End Group
# Begin Source File

SOURCE=.\b.bat
# End Source File
# Begin Source File

SOURCE=.\bc.bat
# End Source File
# Begin Source File

SOURCE=.\bcr.bat
# End Source File
# Begin Source File

SOURCE=.\br.bat
# End Source File
# Begin Source File

SOURCE=.\CopyMedia.bat
# End Source File
# Begin Source File

SOURCE=.\exe\makefile
# End Source File
# Begin Source File

SOURCE=.\r.bat
# End Source File
# Begin Source File

SOURCE=.\exe\sources
# End Source File
# Begin Source File

SOURCE=.\lib\sources
# End Source File
# Begin Source File

SOURCE=.\WipeMedia.bat
# End Source File
# End Target
# End Project
