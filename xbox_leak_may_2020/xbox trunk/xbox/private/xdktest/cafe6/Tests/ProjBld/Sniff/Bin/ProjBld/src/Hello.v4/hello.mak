# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Dll - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Dll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Hello - Win32 Release" && "$(CFG)" != "Hello - Win32 Debug" &&\
 "$(CFG)" != "Dll - Win32 Release" && "$(CFG)" != "Dll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Hello.mak" CFG="Dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Hello - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Hello - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Dll - Win32 Debug"

!IF  "$(CFG)" == "Hello - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
TargetName=Hello
# End Custom Macros

ALL : "Dll - Win32 Release" "$(OUTDIR)\Hello.exe" "$(OUTDIR)\readme.txt"

CLEAN : 
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\readme.txt"
	-@erase "$(INTDIR)\shared.obj"
	-@erase "$(OUTDIR)\Hello.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W2 /WX /GX /O2 /I "\msdev\myinclude \msdev\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_DRELEASE" /YX /c
CPP_PROJ=/nologo /ML /W2 /WX /GX /O2 /I "\msdev\myinclude \msdev\inc" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_DRELEASE" /Fp"$(INTDIR)/Hello.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Hello.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 .\dll.lib /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=.\dll.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Hello.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/Hello.exe" 
LINK32_OBJS= \
	"$(INTDIR)\hello.obj" \
	"$(INTDIR)\shared.obj" \
	"..\Dll.lib" \
	".\Dll\Release\Dll.lib"

"$(OUTDIR)\Hello.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Release
TargetName=Hello
InputPath=.\Release\Hello.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   echo "this is a custom build rule for Hello project"

# End Custom Build

!ELSEIF  "$(CFG)" == "Hello - Win32 Debug"

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
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "Dll - Win32 Debug" ".\Debugx\Hello.exe" ".\foo"

CLEAN : 
	-@erase "$(INTDIR)\hello.obj"
	-@erase "$(INTDIR)\shared.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Hello.pdb"
	-@erase ".\Debugx\Hello.exe"
	-@erase ".\Debugx\Hello.ilk"
	-@erase ".\foo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W2 /WX /Gm /GX /Zi /Od /I "\msdev\myinclude \msdev\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W2 /WX /Gm /GX /Zi /Od /I "\msdev\myinclude \msdev\inc"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/Hello.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "\msdev\rcinclude" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Hello.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE) /nodefaultlib /out:"Debugx/Hello.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Hello.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /nodefaultlib\
 /out:"Debugx/Hello.exe" 
LINK32_OBJS= \
	"$(INTDIR)\hello.obj" \
	"$(INTDIR)\shared.obj" \
	".\Dll\Debug\Dll.lib"

".\Debugx\Hello.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
InputPath=.\Debugx\Hello.exe
SOURCE=$(InputPath)

"foo" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   echo "this is a custom build rule for Hello project"

# End Custom Build

!ELSEIF  "$(CFG)" == "Dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Dll\Release"
# PROP BASE Intermediate_Dir "Dll\Release"
# PROP BASE Target_Dir "Dll"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dll\Release"
# PROP Intermediate_Dir "Dll\Release"
# PROP Target_Dir "Dll"
OUTDIR=.\Dll\Release
INTDIR=.\Dll\Release

ALL : "$(OUTDIR)\Dll.dll" "..\Dll.lib"

CLEAN : 
	-@erase "$(INTDIR)\Dll.obj"
	-@erase "$(INTDIR)\shared.obj"
	-@erase "$(OUTDIR)\Dll.dll"
	-@erase "$(OUTDIR)\Dll.exp"
	-@erase "$(OUTDIR)\Dll.lib"
	-@erase "..\Dll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Dll.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Dll\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Dll.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/Dll.dll"\
 /implib:"$(OUTDIR)/Dll.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Dll.obj" \
	"$(INTDIR)\shared.obj"

"$(OUTDIR)\Dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Dll\Release
InputPath=.\Dll\Release\Dll.dll
InputName=Dll
SOURCE=$(InputPath)

"..\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   copy $(OutDir)\$(InputName).lib ..

# End Custom Build

!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Dll\Debug"
# PROP BASE Intermediate_Dir "Dll\Debug"
# PROP BASE Target_Dir "Dll"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Dll\Debug"
# PROP Intermediate_Dir "Dll\Debug"
# PROP Target_Dir "Dll"
OUTDIR=.\Dll\Debug
INTDIR=.\Dll\Debug

ALL : "$(OUTDIR)\Dll.dll"

CLEAN : 
	-@erase "$(INTDIR)\Dll.obj"
	-@erase "$(INTDIR)\shared.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Dll.dll"
	-@erase "$(OUTDIR)\Dll.exp"
	-@erase "$(OUTDIR)\Dll.ilk"
	-@erase "$(OUTDIR)\Dll.lib"
	-@erase "$(OUTDIR)\Dll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Dll.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Dll\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Dll.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/Dll.dll"\
 /implib:"$(OUTDIR)/Dll.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Dll.obj" \
	"$(INTDIR)\shared.obj"

"$(OUTDIR)\Dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "Hello - Win32 Release"
# Name "Hello - Win32 Debug"

!IF  "$(CFG)" == "Hello - Win32 Release"

!ELSEIF  "$(CFG)" == "Hello - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\shared.cpp
DEP_CPP_SHARE=\
	".\Dll\..\shared.h"\
	

"$(INTDIR)\shared.obj" : $(SOURCE) $(DEP_CPP_SHARE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hello.cpp
DEP_CPP_HELLO=\
	".\Dll\..\shared.h"\
	

!IF  "$(CFG)" == "Hello - Win32 Release"

# ADD CPP /I "\msdev\hellorelease \msdev\myinclude \msdev\inc"
# SUBTRACT CPP /I "\msdev\myinclude \msdev\inc"

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"
   $(CPP) /nologo /ML /W2 /WX /GX /O2 /I\
 "\msdev\hellorelease \msdev\myinclude \msdev\inc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_DRELEASE" /Fp"$(INTDIR)/Hello.pch" /YX /Fo"$(INTDIR)/" /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Hello - Win32 Debug"

# ADD CPP /I "\msdev\hellodebug \msdev\myinclude \msdev\inc" /D "_DDEBUG"
# SUBTRACT CPP /I "\msdev\myinclude \msdev\inc"

"$(INTDIR)\hello.obj" : $(SOURCE) $(DEP_CPP_HELLO) "$(INTDIR)"
   $(CPP) /nologo /MLd /W2 /WX /Gm /GX /Zi /Od /I\
 "\msdev\hellodebug \msdev\myinclude \msdev\inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_DDEBUG" /Fp"$(INTDIR)/Hello.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "Dll"

!IF  "$(CFG)" == "Hello - Win32 Release"

"Dll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Hello.mak" CFG="Dll - Win32 Release" 

!ELSEIF  "$(CFG)" == "Hello - Win32 Debug"

"Dll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\Hello.mak" CFG="Dll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Hello - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\readme.txt
InputName=readme

"$(OutDir)\$(InputName).txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   copy readme.txt $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "Hello - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Dll - Win32 Release"
# Name "Dll - Win32 Debug"

!IF  "$(CFG)" == "Dll - Win32 Release"

!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Dll\Dll.r

!IF  "$(CFG)" == "Dll - Win32 Release"

!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shared.cpp

!IF  "$(CFG)" == "Dll - Win32 Release"

DEP_CPP_SHARE=\
	".\Dll\..\shared.h"\
	

"$(INTDIR)\shared.obj" : $(SOURCE) $(DEP_CPP_SHARE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

DEP_CPP_SHARE=\
	".\Dll\..\shared.h"\
	

"$(INTDIR)\shared.obj" : $(SOURCE) $(DEP_CPP_SHARE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dll\Dll.cpp

!IF  "$(CFG)" == "Dll - Win32 Release"

DEP_CPP_DLL_C=\
	".\Dll\..\shared.h"\
	

"$(INTDIR)\Dll.obj" : $(SOURCE) $(DEP_CPP_DLL_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

DEP_CPP_DLL_C=\
	".\Dll\..\shared.h"\
	

"$(INTDIR)\Dll.obj" : $(SOURCE) $(DEP_CPP_DLL_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
