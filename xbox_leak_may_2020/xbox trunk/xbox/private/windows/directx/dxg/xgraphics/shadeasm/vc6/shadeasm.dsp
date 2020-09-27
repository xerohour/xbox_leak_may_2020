# Microsoft Developer Studio Project File - Name="shadeasm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=shadeasm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shadeasm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shadeasm.mak" CFG="shadeasm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shadeasm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "shadeasm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "shadeasm"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shadeasm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release_shadeasm"
# PROP Intermediate_Dir "release_shadeasm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\inc" /I "..\..\..\..\..\..\..\public\wsdk\inc" /I "..\..\..\..\..\..\inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"release_shadeasm\shadeasm.lib"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "shadeasm___Win32_Debug0"
# PROP BASE Intermediate_Dir "shadeasm___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug_shadeasm"
# PROP Intermediate_Dir "debug_shadeasm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\inc" /I "..\..\..\..\..\..\..\public\wsdk\inc" /I "..\..\..\..\..\..\inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DBG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"debug_shadeasm\shadeasm.lib"

!ENDIF 

# Begin Target

# Name "shadeasm - Win32 Release"
# Name "shadeasm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\api.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CD3DXAssembler.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CD3DXStack.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\misc\debug.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pixelshader.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\preprocessor.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pshdrval.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\stdafx.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\valbase.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\vshdrval.cpp

!IF  "$(CFG)" == "shadeasm - Win32 Release"

!ELSEIF  "$(CFG)" == "shadeasm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CD3DXAssembler.h
# End Source File
# Begin Source File

SOURCE=..\CD3DXStack.h
# End Source File
# Begin Source File

SOURCE=..\errorcodes.h
# End Source File
# Begin Source File

SOURCE=..\microcodeformat.h
# End Source File
# Begin Source File

SOURCE=..\pchshadeasm.h
# End Source File
# Begin Source File

SOURCE=..\pshdrval.h
# End Source File
# Begin Source File

SOURCE=..\shadeasm.h
# End Source File
# Begin Source File

SOURCE=..\valbase.h
# End Source File
# Begin Source File

SOURCE=..\vshdrval.h
# End Source File
# End Group
# End Target
# End Project
