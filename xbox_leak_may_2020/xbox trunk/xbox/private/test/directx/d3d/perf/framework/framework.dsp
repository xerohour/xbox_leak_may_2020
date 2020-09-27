# Microsoft Developer Studio Project File - Name="framework" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=framework - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "framework.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "framework.mak" CFG="framework - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "framework - Xbox Release" (based on "Xbox Application")
!MESSAGE "framework - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "framework - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\inc" /I "..\..\..\..\..\test\inc" /I "..\..\..\..\..\..\Public\xdk\inc" /I "..\..\..\..\..\test\multimedia\inc" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "NOLOGGING" /D "XBOX" /D "UNICODE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# ADD LINK32 xtestlib.lib xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /stack:0x10000
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "framework - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /X /I "..\..\..\..\..\inc" /I "..\..\..\..\..\test\inc" /I "..\..\..\..\..\..\Public\xdk\inc" /I "..\..\..\..\..\test\multimedia\inc" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "NOLOGGING" /D "XBOX" /D "UNICODE" /FR /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xtestlib.lib xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /libpath:"..\..\..\..\..\..\Public\xdk\lib" /libpath:"..\..\..\..\..\test\lib\i386" /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /nodefaultlib
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x10000 /debug
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ENDIF 

# Begin Target

# Name "framework - Xbox Release"
# Name "framework - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Benmark5.cpp
# End Source File
# Begin Source File

SOURCE=.\bitfont.cpp
# End Source File
# Begin Source File

SOURCE=.\Fillrate.cpp
# End Source File
# Begin Source File

SOURCE=.\framework.cpp
# End Source File
# Begin Source File

SOURCE=.\FSAA.cpp
# End Source File
# Begin Source File

SOURCE=.\Ini.cpp
# End Source File
# Begin Source File

SOURCE=.\Test.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Benmark5.h
# End Source File
# Begin Source File

SOURCE=.\bitfont.h
# End Source File
# Begin Source File

SOURCE=.\Fillrate.h
# End Source File
# Begin Source File

SOURCE=.\framework.h
# End Source File
# Begin Source File

SOURCE=.\FSAA.h
# End Source File
# Begin Source File

SOURCE=.\Ini.h
# End Source File
# Begin Source File

SOURCE=.\Test.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# End Group
# Begin Group "Ini Files"

# PROP Default_Filter "*.ini"
# Begin Source File

SOURCE=.\benmark5.ini

!IF  "$(CFG)" == "framework - Xbox Release"

!ELSEIF  "$(CFG)" == "framework - Xbox Debug"

# Begin Custom Build
RemoteDir=xe:\framework
InputPath=.\benmark5.ini

"Debug\benmark5.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xbcp -r -y -d -t -f benmark5.ini $(RemoteDir)\benmark5.ini 
	copy benmark5.ini Debug 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fillrate.ini

!IF  "$(CFG)" == "framework - Xbox Release"

!ELSEIF  "$(CFG)" == "framework - Xbox Debug"

# Begin Custom Build
RemoteDir=xe:\framework
InputPath=.\fillrate.ini

"Debug\fillrate.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xbcp -r -y -d -t -f fillrate.ini $(RemoteDir)\fillrate.ini 
	copy fillrate.ini Debug 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\framework.ini

!IF  "$(CFG)" == "framework - Xbox Release"

!ELSEIF  "$(CFG)" == "framework - Xbox Debug"

# Begin Custom Build
RemoteDir=xe:\framework
InputPath=.\framework.ini

"Debug\framework.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xbcp -r -y -d -t -f framework.ini $(RemoteDir)\framework.ini 
	copy framework.ini Debug 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fsaa.ini

!IF  "$(CFG)" == "framework - Xbox Release"

!ELSEIF  "$(CFG)" == "framework - Xbox Debug"

# Begin Custom Build
RemoteDir=xe:\framework
InputPath=.\fsaa.ini

"Debug\fsaa.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xbcp -r -y -d -t -f fsaa.ini $(RemoteDir)\fsaa.ini 
	copy fsaa.ini Debug 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
