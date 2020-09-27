# Microsoft Developer Studio Project File - Name="BLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=BLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BLib.mak" CFG="BLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "BLib"
# PROP Scc_LocalPath "..\..\..\..\..\blib"
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "BLib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Release\BLib.lib
InputPath=.\Release\BLib.lib
SOURCE="$(InputPath)"

"..\lib\blib.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(TargetPath) 
	copy $(TargetPath) ..\lib\. 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "BLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Debug\BLib.lib
InputPath=.\Debug\BLib.lib
SOURCE="$(InputPath)"

"..\lib\blib.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(TargetPath) 
	copy $(TargetPath) ..\lib\. 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "BLib - Win32 Release"
# Name "BLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BConsole.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\inc\BArray.h
# End Source File
# Begin Source File

SOURCE=..\inc\BConsole.h
# End Source File
# Begin Source File

SOURCE=..\inc\BException.h
# End Source File
# Begin Source File

SOURCE=..\inc\BLib.h
# End Source File
# Begin Source File

SOURCE=..\inc\BList.h
# End Source File
# Begin Source File

SOURCE=..\inc\BString.h
# End Source File
# Begin Source File

SOURCE=..\inc\BTree.h
# End Source File
# End Group
# End Target
# End Project
