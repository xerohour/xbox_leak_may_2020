# Microsoft Developer Studio Project File - Name="onlinehelper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=onlinehelper - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "onlinehelper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "onlinehelper.mak" CFG="onlinehelper - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "onlinehelper - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "onlinehelper - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "onlinehelper - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "onlinehelper - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "onlinehelper - Xbox Release"
# Name "onlinehelper - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AsyncTask.cpp
# End Source File
# Begin Source File

SOURCE=.\ContentInstallTask.cpp
# End Source File
# Begin Source File

SOURCE=.\ContentRemoveTask.cpp
# End Source File
# Begin Source File

SOURCE=.\ContentVerifyTask.cpp
# End Source File
# Begin Source File

SOURCE=.\FriendEnumTask.cpp
# End Source File
# Begin Source File

SOURCE=.\FriendOpTask.cpp
# End Source File
# Begin Source File

SOURCE=.\LogonTask.cpp
# End Source File
# Begin Source File

SOURCE=.\MatchSearchTask.cpp
# End Source File
# Begin Source File

SOURCE=.\MatchSessionCreateTask.cpp
# End Source File
# Begin Source File

SOURCE=.\OfferingEnumerateTask.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineTimer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AsyncTask.h
# End Source File
# Begin Source File

SOURCE=.\ContentInstallTask.h
# End Source File
# Begin Source File

SOURCE=.\ContentRemoveTask.h
# End Source File
# Begin Source File

SOURCE=.\ContentVerifyTask.h
# End Source File
# Begin Source File

SOURCE=.\FriendEnumTask.h
# End Source File
# Begin Source File

SOURCE=.\FriendOpTask.h
# End Source File
# Begin Source File

SOURCE=.\LogonTask.h
# End Source File
# Begin Source File

SOURCE=.\MatchSearchTask.h
# End Source File
# Begin Source File

SOURCE=.\MatchSessionCreateTask.h
# End Source File
# Begin Source File

SOURCE=.\OfferingEnumerateTask.h
# End Source File
# Begin Source File

SOURCE=.\OnlineTimer.h
# End Source File
# End Group
# End Target
# End Project
