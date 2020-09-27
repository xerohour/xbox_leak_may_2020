# Microsoft Developer Studio Project File - Name="xbPremaster" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xbPremaster - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xbPremaster.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xbPremaster.mak" CFG="xbPremaster - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xbPremaster - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xbPremaster - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xbPremaster - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "C:\xbox\private\inc" /I "C:\xbox\private\cert\lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "DVDEMU" /Yu"stdafx.h" /FD /c
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

!ELSEIF  "$(CFG)" == "xbPremaster - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\xbox\private\cert\lib" /I "C:\Xbox\public\wsdk\inc" /I "C:\xbox\private\inc" /I "C:\xbox\private\inc\crypto" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DVDEMU" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "C:\xbox\private\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 \xbox\private\lib\i386\cklibver.lib Rpcrt4.lib msvcrtd.lib \xbox\private\lib\i386\dvdemu.lib \xbox\private\lib\i386\lci.lib \xbox\private\lib\i386\ldi.lib \xbox\private\lib\i386\dvdemu.lib \xbox\private\lib\i386\AmcLayoutEngineD.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xbPremaster - Win32 Release"
# Name "xbPremaster - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Steps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CStep.cpp
# End Source File
# Begin Source File

SOURCE=.\CStep_Completion.cpp
# End Source File
# Begin Source File

SOURCE=.\CStep_DestSource.cpp
# End Source File
# Begin Source File

SOURCE=.\CStep_Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\CStep_UserAction.cpp
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\xbPremaster.cpp
# End Source File
# Begin Source File

SOURCE=.\xbPremaster.rc
# End Source File
# End Group
# Begin Group "WriteThread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CFLD.cpp
# End Source File
# Begin Source File

SOURCE=.\CFST.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateDx2mlFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateXControlFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateXImageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteThread.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\lzss.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CDlg.h
# End Source File
# Begin Source File

SOURCE=.\CFile.h
# End Source File
# Begin Source File

SOURCE=.\CFLD.h
# End Source File
# Begin Source File

SOURCE=.\CFST.h
# End Source File
# Begin Source File

SOURCE=.\CInitedObject.h
# End Source File
# Begin Source File

SOURCE=.\CStep.h
# End Source File
# Begin Source File

SOURCE=.\CXmlNode.h
# End Source File
# Begin Source File

SOURCE=.\filestm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tape.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\add.bmp
# End Source File
# Begin Source File

SOURCE=.\small.bmp
# End Source File
# Begin Source File

SOURCE=.\small2.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\SmallImage.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\TitlePage.bmp
# End Source File
# Begin Source File

SOURCE=.\Media\xbPremaster.ico
# End Source File
# End Target
# End Project
