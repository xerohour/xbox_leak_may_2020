# Microsoft Developer Studio Project File - Name="Image" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=Image - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Image.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Image.mak" CFG="Image - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Image - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "Image - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XDemos/Image", SXCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "Image - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "png" /I "zlib" /D "NDEBUG" /D "WIN32" /D "_XBOX" /D "PNG_USER_MEM_SUPPORTED" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "png" /I "zlib" /D "_DEBUG" /D "WIN32" /D "_XBOX" /D "PNG_USER_MEM_SUPPORTED" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Image - Xbox Release"
# Name "Image - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "PNG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PNG\png.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngerror.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pnggccrd.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngget.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngmem.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngpread.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngread.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngrio.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngrtran.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngrutil.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngset.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngtrans.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngvcrd.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngwio.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngwrite.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngwtran.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNG\pngwutil.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# End Group
# Begin Group "ZLIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZLIB\adler32.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\compress.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\crc32.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\deflate.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\gzio.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\infblock.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\infcodes.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\inffast.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\inflate.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\inftrees.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\infutil.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\minigzip.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\trees.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\uncompr.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZLIB\zutil.c

!IF  "$(CFG)" == "Image - Xbox Release"

!ELSEIF  "$(CFG)" == "Image - Xbox Debug"

# ADD CPP /O2

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\XImage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "PNG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PNG\png.h
# End Source File
# Begin Source File

SOURCE=.\PNG\pngasmrd.h
# End Source File
# Begin Source File

SOURCE=.\PNG\pngconf.h
# End Source File
# End Group
# Begin Group "ZLIB Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZLIB\deflate.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\infblock.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inffast.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\infutil.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\trees.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\zconf.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\zlib.h
# End Source File
# Begin Source File

SOURCE=.\ZLIB\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\XImage.h
# End Source File
# End Group
# End Target
# End Project
