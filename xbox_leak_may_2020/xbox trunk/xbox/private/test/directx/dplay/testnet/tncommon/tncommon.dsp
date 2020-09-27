# Microsoft Developer Studio Project File - Name="tncommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tncommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tncommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tncommon.mak" CFG="tncommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tncommon - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNCOMMON_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\DirectX\dplay\dvoice\inc" /I "..\..\..\..\DirectX\dplay\dnet\inc" /I "C:\Program Files\Microsoft Platform SDK\Include" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TNCOMMON_EXPORTS" /D "BUILDENV_VISUALC" /D "XBOX" /D "NO_ERRORS_DVOICE" /D "NO_SPRINTF_DPLAY8" /D "NO_SPRINTF_DVOICE" /D "NO_SPRINTF_REGISTRY" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib version.lib wsock32.lib comctl32.lib advapi32.lib gdi32.lib user32.lib kernel32.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	mapsym -o Debug\tncommon.sym Debug\tncommon.map	echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	echo Copying built binaries to C:\dplay\testnet...	copy Debug\tncommon.dll C:\dplay\testnet	copy Debug\tncommon.pdb C:\dplay\testnet	copy Debug\tncommon.sym C:\dplay\testnet
# End Special Build Tool
# Begin Target

# Name "tncommon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\checkver.c
# End Source File
# Begin Source File

SOURCE=.\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\convhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\cppobjhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\debugprint.cpp
# End Source File
# Begin Source File

SOURCE=.\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\excptns.cpp
# End Source File
# Begin Source File

SOURCE=.\fileutils.cpp
# End Source File
# Begin Source File

SOURCE=.\linkedstr.cpp
# End Source File
# Begin Source File

SOURCE=.\linklist.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\sprintf.cpp
# End Source File
# Begin Source File

SOURCE=.\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\strutils.cpp
# End Source File
# Begin Source File

SOURCE=.\symbols.cpp
# End Source File
# Begin Source File

SOURCE=.\text.cpp
# End Source File
# Begin Source File

SOURCE=.\verutils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\checkver.h
# End Source File
# Begin Source File

SOURCE=.\cmdline.h
# End Source File
# Begin Source File

SOURCE=.\convhelp.h
# End Source File
# Begin Source File

SOURCE=.\cppobjhelp.h
# End Source File
# Begin Source File

SOURCE=.\crc32.h
# End Source File
# Begin Source File

SOURCE=.\debugprint.h
# End Source File
# Begin Source File

SOURCE=.\errors.h
# End Source File
# Begin Source File

SOURCE=.\excptns.h
# End Source File
# Begin Source File

SOURCE=.\fileutils.h
# End Source File
# Begin Source File

SOURCE=.\linkedstr.h
# End Source File
# Begin Source File

SOURCE=.\linklist.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sprintf.h
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=.\strutils.h
# End Source File
# Begin Source File

SOURCE=.\symbols.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\verutils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\tncommon.def
# End Source File
# Begin Source File

SOURCE=.\tncommon.rc
# End Source File
# End Group
# End Target
# End Project
