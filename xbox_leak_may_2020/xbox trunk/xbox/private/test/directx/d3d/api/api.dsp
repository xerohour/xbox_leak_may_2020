# Microsoft Developer Studio Project File - Name="api" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=api - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "api.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "api.mak" CFG="api - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "api - Xbox Release" (based on "Xbox Application")
!MESSAGE "api - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "api - Xbox Release"

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
# ADD CPP /nologo /Gz /W3 /O2 /I "\xb\public\sdk\inc" /I "\xb\public\sdk\inc\crt" /I "\xb\private\ntos\inc" /I "\xb\private\test\inc" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /YX /FD /G6 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib xboxkrnl.lib /nologo /machine:I386 /ALIGN:32 /DRIVER
# ADD LINK32 xapilib.lib xboxkrnl.lib xtestlib.lib xnet.lib d3d8.lib d3dx8.lib xgraphics.lib xlog.lib guidgen.lib c:\xb\private\lib\i386\rsa32.lib /nologo /machine:I386 /libpath:"c:\xb\private\test\lib\i386" /libpath:"c:\xb\public\lib\i386" /ALIGN:32 /DRIVER -merge:.rdata=.text /optidata
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /NOLOGO
# ADD XBE /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=move to xb
PostBuild_Cmds=xbcp -q -x a-jasgoux2 release\api.xbe xe:\d3dapi\api.xbe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "api - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "api___Xbox_Debug"
# PROP BASE Intermediate_Dir "api___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "api___Xbox_Debug"
# PROP Intermediate_Dir "api___Xbox_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /G6 /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /X /I "\xb\public\sdk\inc" /I "\xb\public\sdk\inc\crt" /I "\xb\private\ntos\inc" /I "\xb\private\test\inc" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /FR /YX /FD /G6 /c
# SUBTRACT CPP /Gf
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib xboxkrnl.lib /nologo /debug /machine:I386 /ALIGN:32 /DRIVER
# ADD LINK32 xapilibd.lib xboxkrnl.lib xtestlib.lib xnetd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib xlog.lib guidgen.lib c:\xb\private\lib\i386\rsa32.lib /nologo /incremental:no /map /debug /debugtype:both /machine:I386 /nodefaultlib /libpath:"c:\xb\private\test\lib\i386" /libpath:"c:\xb\public\lib\i386" /ALIGN:32 /driver -align:0x20 -driver -STACK:65536,65536 -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -IGNORE:4001,4037,4039,4044,4065,4070,4078,4087,4089,4198 -FULLBUILD -FORCE:MULTIPLE -NOCOMMENT /release -debug:FULL -version:5.00 -osversion:5.00 -merge:.rdata=.text -subsystem:windows,4.00 -entry:wWinMainCRTStartup -optidata
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /NOLOGO /DEBUG
# ADD XBE /NOLOGO /DEBUG
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=move to xb
PostBuild_Cmds=xbcp -q -x a-jasgoux2 api___xbox_debug\api.xbe xe:\d3dapi\api.xbe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "api - Xbox Release"
# Name "api - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cubelock.cpp
# End Source File
# Begin Source File

SOURCE=.\cubetexture.cpp
# End Source File
# Begin Source File

SOURCE=.\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dapi.cpp
# End Source File
# Begin Source File

SOURCE=.\direct3d8api.cpp
# End Source File
# Begin Source File

SOURCE=.\direct3d8apiinit.cpp
# End Source File
# Begin Source File

SOURCE=.\direct3ddevice8.cpp
# End Source File
# Begin Source File

SOURCE=.\direct3ddevice8init.cpp
# End Source File
# Begin Source File

SOURCE=.\linkme.cpp
# End Source File
# Begin Source File

SOURCE=.\lock.cpp
# End Source File
# Begin Source File

SOURCE=.\exe\main.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\uma.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\volumelock.cpp
# End Source File
# Begin Source File

SOURCE=.\volumetexture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\d3dapi.hpp
# End Source File
# Begin Source File

SOURCE=.\d3dapiMMstats.h
# End Source File
# Begin Source File

SOURCE=.\linkme.hpp
# End Source File
# Begin Source File

SOURCE=.\swizzler.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\sources.inc
# End Source File
# End Group
# Begin Group "external headers"

# PROP Default_Filter "*.h"
# End Group
# End Target
# End Project
