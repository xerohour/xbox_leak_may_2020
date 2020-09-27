# Microsoft Developer Studio Project File - Name="xbounce" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=xbounce - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xbounce.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xbounce.mak" CFG="xbounce - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xbounce - Xbox Release" (based on "Xbox Application")
!MESSAGE "xbounce - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xbounce - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib libcmt.lib ntoskrnl.lib /nologo /machine:I386 /nodefaultlib /ALIGN:32 /DRIVER /SUBSYSTEM:WINDOWS /base:0x400000 /entry:XapiEntryPoint@12
# ADD LINK32 xapilib.lib libcmt.lib ntoskrnl.lib d3d8.lib /nologo /machine:I386 /nodefaultlib /ALIGN:32 /DRIVER /SUBSYSTEM:WINDOWS /base:0x400000 /entry:XapiEntryPointCRT@12
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe

!ELSEIF  "$(CFG)" == "xbounce - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\common" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib libcmtd.lib ntoskrnl.lib /nologo /incremental:no /debug /machine:I386 /nodefaultlib /ALIGN:32 /DRIVER /SUBSYSTEM:WINDOWS /base:0x400000 /entry:XapiEntryPoint@12
# ADD LINK32 xapilibd.lib libcmtd.lib ntoskrnl.lib d3d8d.lib /nologo /incremental:no /debug /machine:I386 /nodefaultlib /ALIGN:32 /DRIVER /SUBSYSTEM:WINDOWS /base:0x400000 /entry:XapiEntryPointCRT@12
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /DEBUG
# ADD XBE /DEBUG

!ENDIF 

# Begin Target

# Name "xbounce - Xbox Release"
# Name "xbounce - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\bitfont.cpp
# End Source File
# Begin Source File

SOURCE=..\common\bounce.cpp
# End Source File
# Begin Source File

SOURCE=..\common\draw.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\bitfont.h
# End Source File
# Begin Source File

SOURCE=..\common\bounce.h
# End Source File
# Begin Source File

SOURCE=..\common\draw.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
