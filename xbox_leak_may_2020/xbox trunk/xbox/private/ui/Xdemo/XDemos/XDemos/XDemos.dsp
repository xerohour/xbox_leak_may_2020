# Microsoft Developer Studio Project File - Name="XDemos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=XDemos - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XDemos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XDemos.mak" CFG="XDemos - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XDemos - Xbox Release" (based on "Xbox Application")
!MESSAGE "XDemos - Xbox Debug" (based on "Xbox Application")
!MESSAGE "XDemos - Xbox Profile" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XDemos/XDemos", DSCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "XDemos - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /ML /W3 /GX /Zi /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /FR /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilib.lib image\release\image.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib  wmvdec.lib /nologo /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /verbose
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0x00000000" /testname:"Xbox Demos" /stack:0x10000 /debug /limitmem /titleimage:"$(OutDir)\titleimage.xbx"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=xe:\xdemos
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media	xbcp -r -y -d -t -f xdemos.xdi $(RemoteDir)	xbcp -r -y -d -t -f softball\*.* $(RemoteDir)\softball	xbcp -r -y -d -t -f dolphin\*.png $(RemoteDir)\dolphin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XDemos - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /ML /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /FR /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib image\debug\image.lib xbdm.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib libcd.lib  wmvdecd.lib /nologo /incremental:no /debug /machine:I386 /nodefaultlib /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /verbose
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x10000 /debug /limitmem
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=xe:\xdemos
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media	xbcp -r -y -d -t -f xdemos.xdi $(RemoteDir)	xbcp -r -y -d -t -f softball\*.* $(RemoteDir)\softball	xbcp -r -y -d -t -f dolphin\*.png $(RemoteDir)\dolphin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XDemos - Xbox Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XDemos___Xbox_Profile"
# PROP BASE Intermediate_Dir "XDemos___Xbox_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /ML /W3 /GX /Zi /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /FR /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /ML /W3 /GX /Zi /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "PROFILE" /FR /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib image\release\image.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 .\XDIXLib\Release\XDIXLib.lib xapilib.lib image\release\image.lib d3d8i.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /verbose
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x10000 /debug
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media	xbcp -r -y -d -t -f xdemos.xdi $(RemoteDir)	xbcp -r -y -d -t -f softball\*.* $(RemoteDir)\softball	xbcp -r -y -d -t -f dolphin\*.png $(RemoteDir)\dolphin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "XDemos - Xbox Release"
# Name "XDemos - Xbox Debug"
# Name "XDemos - Xbox Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XbFont.Cpp
# End Source File
# Begin Source File

SOURCE=.\XbResource.Cpp
# End Source File
# Begin Source File

SOURCE=.\XbUtil.Cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Input.cpp
# End Source File
# Begin Source File

SOURCE=.\xdemos.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Framework Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XbFont.h
# End Source File
# Begin Source File

SOURCE=.\XbResource.h
# End Source File
# Begin Source File

SOURCE=.\XbUtil.h
# End Source File
# Begin Source File

SOURCE=.\XDIFile\XDIFile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\XDemos.h
# End Source File
# Begin Source File

SOURCE=.\Image\XImage.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\TitleImage.rdf
# End Source File
# Begin Source File

SOURCE=.\Xbox_Demo_UI_KioskTitleImage.bmp

!IF  "$(CFG)" == "XDemos - Xbox Release"

USERDEP__XBOX_="TitleImage.rdf"	
# Begin Custom Build
OutDir=.\Release
InputPath=.\Xbox_Demo_UI_KioskTitleImage.bmp

"$(OutDir)\titleimage.xbx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bundler TitleImage.rdf -o $(OutDir)\titleimage.xbx

# End Custom Build

!ELSEIF  "$(CFG)" == "XDemos - Xbox Debug"

!ELSEIF  "$(CFG)" == "XDemos - Xbox Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xdemos.xdi
# End Source File
# End Group
# Begin Group "Media"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\media\ambient.wma
# End Source File
# Begin Source File

SOURCE=.\media\Dn.png
# End Source File
# Begin Source File

SOURCE=.\media\fontb.xpr
# End Source File
# Begin Source File

SOURCE=.\media\fontn.xpr
# End Source File
# Begin Source File

SOURCE=.\media\launch.wav
# End Source File
# Begin Source File

SOURCE=.\media\menu.wav
# End Source File
# Begin Source File

SOURCE=.\media\screen.png
# End Source File
# Begin Source File

SOURCE=.\media\screenT.png
# End Source File
# Begin Source File

SOURCE=.\media\Up.png
# End Source File
# Begin Source File

SOURCE=.\media\wait.png
# End Source File
# End Group
# Begin Group "Bink"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bink\BINK.H
# End Source File
# Begin Source File

SOURCE=.\Bink\RAD.H
# End Source File
# Begin Source File

SOURCE=.\Bink\binkxbox.lib
# End Source File
# End Group
# Begin Group "Files needed by sample demos"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dolphin\dol.png
# End Source File
# Begin Source File

SOURCE=.\Softball\mike.bik
# End Source File
# Begin Source File

SOURCE=.\Softball\mike.png
# End Source File
# End Group
# Begin Group "wmv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WMV\debugstats.cpp
# End Source File
# Begin Source File

SOURCE=.\WMV\graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\WMV\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\WMV\wmvplay.cpp
# End Source File
# Begin Source File

SOURCE=.\WMV\wmvplay.h
# End Source File
# End Group
# End Target
# End Project
