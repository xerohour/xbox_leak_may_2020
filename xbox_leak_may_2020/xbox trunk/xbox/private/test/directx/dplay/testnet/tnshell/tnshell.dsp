# Microsoft Developer Studio Project File - Name="tnshell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tnshell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tnshell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tnshell.mak" CFG="tnshell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tnshell - Win32 Debug" (based on "Win32 (x86) Application")
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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\inc" /I "..\..\..\common\testmgr\tmagent" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "BUILDENV_VISUALC" /D "XBOX" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\tncontrl\Debug\tncontrl.lib ..\tncommon\Debug\tncommon.lib gdi32.lib kernel32.lib user32.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	mapsym -o Debug\tnshell.sym Debug\tnshell.map	echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	echo Copying built binaries to C:\dplay\testnet...	copy Debug\tnshell.exe C:\dplay\testnet	copy Debug\tnshell.pdb C:\dplay\testnet	copy Debug\tnshell.sym C:\dplay\testnet
# End Special Build Tool
# Begin Target

# Name "tnshell - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\exe\main.cpp
# End Source File
# Begin Source File

SOURCE=.\exe\prefs.cpp
# End Source File
# Begin Source File

SOURCE=.\exe\select.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\exe\main.h
# End Source File
# Begin Source File

SOURCE=.\exe\prefs.h
# End Source File
# Begin Source File

SOURCE=.\exe\resource.h
# End Source File
# Begin Source File

SOURCE=.\exe\select.h
# End Source File
# Begin Source File

SOURCE=.\exe\tnshell.h
# End Source File
# Begin Source File

SOURCE=.\exe\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\tnshell.rc
# End Source File
# End Group
# End Target
# End Project
