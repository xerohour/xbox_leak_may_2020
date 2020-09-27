# Microsoft Developer Studio Project File - Name="Installer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=Installer - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Installer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Installer.mak" CFG="Installer - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Installer - Xbox Release" (based on "Xbox Application")
!MESSAGE "Installer - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "Installer - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "SILVER" /D "NDEBUG" /YX /FD /Zvc6 /G6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Common\include" /D "WIN32" /D "_XBOX" /D "SILVER" /D "NDEBUG" /YX /FD /Zvc6 /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib xbdm.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib xbdm.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0xffff0030" /testname:"Installer" /testpubname:"Microsoft" /stack:0x10000 /INITFLAGS:0x00000000
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Installer - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "SILVER" /D "_DEBUG" /YX /FD /Zvc6 /G6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /D "WIN32" /D "_XBOX" /D "SILVER" /D "_DEBUG" /YX /FD /Zvc6 /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib xbdm.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib xbdm.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testid:"0xffff0030" /testname:"Installer" /testpubname:"Microsoft" /stack:0x10000 /debug /INITFLAGS:0x00000000
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=xe:\samples\installer
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Installer - Xbox Release"
# Name "Installer - Xbox Debug"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XBResource.cpp
# End Source File
# Begin Source File

SOURCE=.\XBUtil.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Installer.cpp
# End Source File
# End Target
# End Project
