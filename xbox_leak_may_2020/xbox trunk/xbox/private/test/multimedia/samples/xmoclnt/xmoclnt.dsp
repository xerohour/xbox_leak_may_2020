# Microsoft Developer Studio Project File - Name="xmoclnt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=xmoclnt - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xmoclnt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xmoclnt.mak" CFG="xmoclnt - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xmoclnt - Xbox Release" (based on "Xbox Application")
!MESSAGE "xmoclnt - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmoclnt - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6  /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6  /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib xboxkrnl.lib /nologo /machine:I386 /ALIGN:32 /DRIVER
# ADD LINK32 xapilib.lib xboxkrnl.lib /nologo /machine:I386 /ALIGN:32 /DRIVER
XBE=imagebld.exe
# ADD BASE XBE /NOLOGO
# ADD XBE /NOLOGO

!ELSEIF  "$(CFG)" == "xmoclnt - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6  /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "OBJECT_IS_CHAR" /YX /FD /G6  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib xboxkrnl.lib /nologo /debug /machine:I386 /ALIGN:32 /DRIVER
# ADD LINK32 xapilibd.lib xboxkrnl.lib dsoundd.lib /nologo /debug /machine:I386 /ALIGN:32 /DRIVER
XBE=imagebld.exe
# ADD BASE XBE /NOLOGO /DEBUG
# ADD XBE /NOLOGO /DEBUG
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp /q C:\xbox\private\test\multimedia\samples\xmoclnt\debug\xmoclnt.xbe xc:\xbox\default.xbe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "xmoclnt - Xbox Release"
# Name "xmoclnt - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRCXMO.cpp
# End Source File
# Begin Source File

SOURCE=.\xmoclnt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRCXMO.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
