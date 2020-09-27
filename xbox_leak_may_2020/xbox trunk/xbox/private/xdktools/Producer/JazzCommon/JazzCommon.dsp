# Microsoft Developer Studio Project File - Name="JazzCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=JazzCommon - Win32 BoundsChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "JazzCommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "JazzCommon.mak" CFG="JazzCommon - Win32 BoundsChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "JazzCommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "JazzCommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "JazzCommon - Win32 BoundsChecker" (based on "Win32 (x86) Static Library")
!MESSAGE "JazzCommon - Win32 BBT" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/Jazz/JazzCommon", PAABAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "JazzCommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\JazzCommon.lib  ..\Libs\Release	copy          appbaseproppagemanager.h  ..\includes	copy  dllbaseproppagemanager.h          ..\includes	copy  baseproppagemanager.h  ..\includes	copy      staticproppagemanager.h ..\includes	copy  JazzEnumFormatEtc.h ..\includes	copy      JazzDataObject.h ..\includes	copy BaseJazzDataObject.h ..\includes	copy    AppJazzDataObject.h ..\includes	copy DllJazzDataObject.h ..\includes	copy  JazzUndoMan.h  ..\Includes
# End Special Build Tool

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\JazzCommond.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Debug\JazzCommond.lib  ..\Libs\Debug	copy          appbaseproppagemanager.h  ..\includes	copy  dllbaseproppagemanager.h          ..\includes	copy  baseproppagemanager.h  ..\includes	copy      staticproppagemanager.h ..\includes	copy  JazzEnumFormatEtc.h ..\includes	copy      JazzDataObject.h ..\includes	copy BaseJazzDataObject.h ..\includes	copy    AppJazzDataObject.h ..\includes	copy DllJazzDataObject.h ..\includes	copy  JazzUndoMan.h  ..\Includes
# End Special Build Tool

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 BoundsChecker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "JazzCommon___Win32_BoundsChecker"
# PROP BASE Intermediate_Dir "JazzCommon___Win32_BoundsChecker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BoundsChecker"
# PROP Intermediate_Dir "BoundsChecker"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\JazzCommond.lib"
# ADD LIB32 /nologo /out:"Debug\JazzCommond.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\BoundsChecker\JazzCommond.lib  ..\Libs\BoundsChecker	copy          appbaseproppagemanager.h  ..\includes	copy  dllbaseproppagemanager.h          ..\includes	copy  baseproppagemanager.h  ..\includes	copy      staticproppagemanager.h ..\includes	copy  JazzEnumFormatEtc.h ..\includes	copy      JazzDataObject.h ..\includes	copy BaseJazzDataObject.h ..\includes	copy    AppJazzDataObject.h ..\includes	copy DllJazzDataObject.h ..\includes	copy  JazzUndoMan.h  ..\Includes
# End Special Build Tool

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 BBT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "JazzCommon___Win32_BBT"
# PROP BASE Intermediate_Dir "JazzCommon___Win32_BBT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BBT"
# PROP Intermediate_Dir "BBT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\BBT\JazzCommon.lib  ..\Libs\BBT	copy          appbaseproppagemanager.h  ..\includes	copy  dllbaseproppagemanager.h          ..\includes	copy  baseproppagemanager.h  ..\includes	copy      staticproppagemanager.h ..\includes	copy  JazzEnumFormatEtc.h ..\includes	copy      JazzDataObject.h ..\includes	copy BaseJazzDataObject.h ..\includes	copy    AppJazzDataObject.h ..\includes	copy DllJazzDataObject.h ..\includes	copy  JazzUndoMan.h  ..\Includes
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "JazzCommon - Win32 Release"
# Name "JazzCommon - Win32 Debug"
# Name "JazzCommon - Win32 BoundsChecker"
# Name "JazzCommon - Win32 BBT"
# Begin Source File

SOURCE=.\AppBasePropPageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\AppBasePropPageManager.h
# End Source File
# Begin Source File

SOURCE=.\AppJazzDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AppJazzDataObject.h
# End Source File
# Begin Source File

SOURCE=.\BaseJazzDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseJazzDataObject.h
# End Source File
# Begin Source File

SOURCE=.\BasePropPageManager.cpp

!IF  "$(CFG)" == "JazzCommon - Win32 Release"

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 BoundsChecker"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "JazzCommon - Win32 BBT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BasePropPageManager.h
# End Source File
# Begin Source File

SOURCE=.\DllBasePropPageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DllBasePropPageManager.h
# End Source File
# Begin Source File

SOURCE=.\DllJazzDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\DllJazzDataObject.h
# End Source File
# Begin Source File

SOURCE=.\JazzDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzDataObject.h
# End Source File
# Begin Source File

SOURCE=.\JazzEnumFormatEtc.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzEnumFormatEtc.h
# End Source File
# Begin Source File

SOURCE=.\JazzUndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\JazzUndoMan.h
# End Source File
# Begin Source File

SOURCE=.\StaticPropPageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticPropPageManager.h
# End Source File
# End Target
# End Project
