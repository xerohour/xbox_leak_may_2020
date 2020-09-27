# Microsoft Developer Studio Project File - Name="xbLayout" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xbLayout - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xbLayout.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xbLayout.mak" CFG="xbLayout - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xbLayout - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xbLayout - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xbLayout - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xbLayout - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\xbox\private\inc" /I "C:\xbox\private\ntos\gdfx" /I "C:\xbox\private\cert\lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DVDEMU" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "C:\xbox\private\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 \xbox\private\lib\i386\AmcLayoutEngineD.lib Msimg32.lib comctl32.lib Rpcrt4.lib msvcrtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xbLayout - Win32 Release"
# Name "xbLayout - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CColorBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CDVD.cpp
# End Source File
# Begin Source File

SOURCE=.\CDVD_DirEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\CDVD_Insert.cpp
# End Source File
# Begin Source File

SOURCE=.\CDVD_Persist.cpp
# End Source File
# Begin Source File

SOURCE=.\CFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CListview.cpp
# End Source File
# Begin Source File

SOURCE=.\CListView_OwnerDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\CObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\CWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CWindow_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\CWindow_Main_Drag.cpp
# End Source File
# Begin Source File

SOURCE=.\CWindow_Scratch.cpp
# End Source File
# Begin Source File

SOURCE=.\CWorkspace.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\xbLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\xbLayout.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CColorBar.h
# End Source File
# Begin Source File

SOURCE=.\CDVD.h
# End Source File
# Begin Source File

SOURCE=.\CFile.h
# End Source File
# Begin Source File

SOURCE=.\CInitedObject.h
# End Source File
# Begin Source File

SOURCE=.\CListView.h
# End Source File
# Begin Source File

SOURCE=.\CObject.h
# End Source File
# Begin Source File

SOURCE=.\CScratch.h
# End Source File
# Begin Source File

SOURCE=.\CStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\CToolbar.h
# End Source File
# Begin Source File

SOURCE=.\CWaitCursor.h
# End Source File
# Begin Source File

SOURCE=.\CWindow.h
# End Source File
# Begin Source File

SOURCE=.\CWindow_Main.h
# End Source File
# Begin Source File

SOURCE=.\CWindow_Scratch.h
# End Source File
# Begin Source File

SOURCE=.\CWorkspace.h
# End Source File
# Begin Source File

SOURCE=.\FLDVersion.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Media\about.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Media\Folder.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\xbLayout.ico
# End Source File
# End Group
# End Target
# End Project
