# Microsoft Developer Studio Generated NMAKE File, Based on PropertyTest.dsp
!IF "$(CFG)" == ""
CFG=PropertyTest - Win32 Release
!MESSAGE No configuration specified. Defaulting to PropertyTest - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "PropertyTest - Win32 Release" && "$(CFG)" != "PropertyTest - Win32 Debug" && "$(CFG)" != "PropertyTest - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PropertyTest.mak" CFG="PropertyTest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PropertyTest - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PropertyTest - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PropertyTest - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PropertyTest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\PropertyTest.dlu"


CLEAN :
	-@erase "$(INTDIR)\PropertyTest.obj"
	-@erase "$(INTDIR)\PropertyTest.res"
	-@erase "$(OUTDIR)\PropertyTest.exp"
	-@erase "$(OUTDIR)\PropertyTest.lib"
	-@erase "..\..\..\plugin\PropertyTest.dlu"
	-@erase ".\PropertyTest.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PropertyTest.pch" /YX"PropertyTest.h" /Fo"$(INTDIR)\\" /Fd"PropertyTest.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PropertyTest.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PropertyTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PropertyTest.pdb" /machine:I386 /def:".\PropertyTest.def" /out:"..\..\..\plugin\PropertyTest.dlu" /implib:"$(OUTDIR)\PropertyTest.lib" /release 
DEF_FILE= \
	".\PropertyTest.def"
LINK32_OBJS= \
	"$(INTDIR)\PropertyTest.obj" \
	"$(INTDIR)\PropertyTest.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PropertyTest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PropertyTest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\PropertyTest.dlu"


CLEAN :
	-@erase "$(INTDIR)\PropertyTest.obj"
	-@erase "$(INTDIR)\PropertyTest.res"
	-@erase "$(OUTDIR)\PropertyTest.exp"
	-@erase "$(OUTDIR)\PropertyTest.lib"
	-@erase "$(OUTDIR)\PropertyTest.pdb"
	-@erase "..\..\..\plugin\PropertyTest.dlu"
	-@erase "..\..\..\plugin\PropertyTest.ilk"
	-@erase ".\PropertyTest.idb"
	-@erase ".\PropertyTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PropertyTest.pch" /YX"PropertyTest.h" /Fo"$(INTDIR)\\" /Fd"PropertyTest.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PropertyTest.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PropertyTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PropertyTest.pdb" /debug /machine:I386 /def:".\PropertyTest.def" /out:"..\..\..\plugin\PropertyTest.dlu" /implib:"$(OUTDIR)\PropertyTest.lib" 
DEF_FILE= \
	".\PropertyTest.def"
LINK32_OBJS= \
	"$(INTDIR)\PropertyTest.obj" \
	"$(INTDIR)\PropertyTest.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PropertyTest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PropertyTest - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\PropertyTest.dlu"


CLEAN :
	-@erase "$(INTDIR)\PropertyTest.obj"
	-@erase "$(INTDIR)\PropertyTest.res"
	-@erase "$(OUTDIR)\PropertyTest.exp"
	-@erase "$(OUTDIR)\PropertyTest.lib"
	-@erase "$(OUTDIR)\PropertyTest.pdb"
	-@erase "..\..\..\plugin\PropertyTest.dlu"
	-@erase "..\..\..\plugin\PropertyTest.ilk"
	-@erase ".\PropertyTest.idb"
	-@erase ".\PropertyTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PropertyTest.pch" /YX"PropertyTest.h" /Fo"$(INTDIR)\\" /Fd"PropertyTest.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PropertyTest.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PropertyTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PropertyTest.pdb" /debug /machine:I386 /def:".\PropertyTest.def" /out:"..\..\..\plugin\PropertyTest.dlu" /implib:"$(OUTDIR)\PropertyTest.lib" 
DEF_FILE= \
	".\PropertyTest.def"
LINK32_OBJS= \
	"$(INTDIR)\PropertyTest.obj" \
	"$(INTDIR)\PropertyTest.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PropertyTest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("PropertyTest.dep")
!INCLUDE "PropertyTest.dep"
!ELSE 
!MESSAGE Warning: cannot find "PropertyTest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PropertyTest - Win32 Release" || "$(CFG)" == "PropertyTest - Win32 Debug" || "$(CFG)" == "PropertyTest - Win32 Hybrid"
SOURCE=.\PropertyTest.cpp

"$(INTDIR)\PropertyTest.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PropertyTest.rc

"$(INTDIR)\PropertyTest.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

