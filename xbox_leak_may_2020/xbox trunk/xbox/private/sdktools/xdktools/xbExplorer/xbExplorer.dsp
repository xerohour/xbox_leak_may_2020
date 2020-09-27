# Microsoft Developer Studio Project File - Name="xbExplorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xbExplorer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xbExplorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xbExplorer.mak" CFG="xbExplorer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xbExplorer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xbExplorer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xbExplorer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "PC" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VCBUILD" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 \xbox\public\xdk\lib\xboxdbg.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xbExplorer - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\xbox\public\wsdk\inc" /I "\xbox\public\xdk\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VCBUILD" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "\xbox\public\wsdk\inc" /i "\xbox\public\xdk\inc" /i "\xbox\private\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 \xbox\public\xdk\lib\xboxdbg.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xbExplorer - Win32 Release"
# Name "xbExplorer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CDropSource.cpp
# End Source File
# Begin Source File

SOURCE=.\CDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\CFileContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\CFileContainerFormats.cpp
# End Source File
# Begin Source File

SOURCE=.\CProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\CProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\CXboxConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\CXboxStream.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProcs.cpp
# End Source File
# Begin Source File

SOURCE=.\DPA.cpp
# End Source File
# Begin Source File

SOURCE=.\History.cpp
# End Source File
# Begin Source File

SOURCE=.\ListView.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TargetMachine.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolView.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewOption.cpp
# End Source File
# Begin Source File

SOURCE=.\xbExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\xbExplorer.rc
# End Source File
# Begin Source File

SOURCE=.\XBFU.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cdropsource.h
# End Source File
# Begin Source File

SOURCE=.\CDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\CFileContainer.h
# End Source File
# Begin Source File

SOURCE=.\CFileContainerFormats.h
# End Source File
# Begin Source File

SOURCE=.\CProgress.h
# End Source File
# Begin Source File

SOURCE=.\CProperties.h
# End Source File
# Begin Source File

SOURCE=.\CXboxConnection.h
# End Source File
# Begin Source File

SOURCE=.\CXboxStream.h
# End Source File
# Begin Source File

SOURCE=.\DlgProcs.h
# End Source File
# Begin Source File

SOURCE=.\DPDA.h
# End Source File
# Begin Source File

SOURCE=.\History.h
# End Source File
# Begin Source File

SOURCE=.\ListView.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StatusView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TargetMachine.h
# End Source File
# Begin Source File

SOURCE=.\ToolView.h
# End Source File
# Begin Source File

SOURCE=.\ViewOption.h
# End Source File
# Begin Source File

SOURCE=.\xbExplorer.h
# End Source File
# Begin Source File

SOURCE=.\XBFU.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\file-copy-1.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbarbitmap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xbExplorer.ico
# End Source File
# End Group
# End Target
# End Project
