# Microsoft Developer Studio Generated NMAKE File, Based on MapPath.dsp
!IF "$(CFG)" == ""
CFG=MapPath - Win32 Release
!MESSAGE No configuration specified. Defaulting to MapPath - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "MapPath - Win32 Release" && "$(CFG)" != "MapPath - Win32 Debug" && "$(CFG)" != "MapPath - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MapPath.mak" CFG="MapPath - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MapPath - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MapPath - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MapPath - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MapPath - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\MapPath.dlu"


CLEAN :
	-@erase "$(INTDIR)\mappath.obj"
	-@erase "$(INTDIR)\mappath.res"
	-@erase "$(OUTDIR)\MapPath.exp"
	-@erase "$(OUTDIR)\MapPath.lib"
	-@erase "..\..\..\..\maxsdk\plugin\MapPath.dlu"
	-@erase ".\MapPath.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MapPath.pch" /YX"MapPath.h" /Fo"$(INTDIR)\\" /Fd"MapPath.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mappath.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MapPath.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08560000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\MapPath.pdb" /machine:I386 /def:".\mappath.def" /out:"..\..\..\..\maxsdk\plugin\MapPath.dlu" /implib:"$(OUTDIR)\MapPath.lib" /release 
DEF_FILE= \
	".\mappath.def"
LINK32_OBJS= \
	"$(INTDIR)\mappath.obj" \
	"$(INTDIR)\mappath.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\MapPath.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MapPath - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\MapPath.dlu"


CLEAN :
	-@erase "$(INTDIR)\mappath.obj"
	-@erase "$(INTDIR)\mappath.res"
	-@erase "$(OUTDIR)\MapPath.exp"
	-@erase "$(OUTDIR)\MapPath.lib"
	-@erase "$(OUTDIR)\MapPath.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\MapPath.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\MapPath.ilk"
	-@erase ".\MapPath.idb"
	-@erase ".\MapPath.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MapPath.pch" /YX"MapPath.h" /Fo"$(INTDIR)\\" /Fd"MapPath.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mappath.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MapPath.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08560000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\MapPath.pdb" /debug /machine:I386 /def:".\mappath.def" /out:"..\..\..\..\maxsdk\plugin\MapPath.dlu" /implib:"$(OUTDIR)\MapPath.lib" 
DEF_FILE= \
	".\mappath.def"
LINK32_OBJS= \
	"$(INTDIR)\mappath.obj" \
	"$(INTDIR)\mappath.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\MapPath.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MapPath - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\MapPath.dlu"


CLEAN :
	-@erase "$(INTDIR)\mappath.obj"
	-@erase "$(INTDIR)\mappath.res"
	-@erase "$(OUTDIR)\MapPath.exp"
	-@erase "$(OUTDIR)\MapPath.lib"
	-@erase "$(OUTDIR)\MapPath.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\MapPath.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\MapPath.ilk"
	-@erase ".\MapPath.idb"
	-@erase ".\MapPath.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MapPath.pch" /YX"MapPath.h" /Fo"$(INTDIR)\\" /Fd"MapPath.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mappath.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MapPath.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08560000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\MapPath.pdb" /debug /machine:I386 /def:".\mappath.def" /out:"..\..\..\..\maxsdk\plugin\MapPath.dlu" /implib:"$(OUTDIR)\MapPath.lib" 
DEF_FILE= \
	".\mappath.def"
LINK32_OBJS= \
	"$(INTDIR)\mappath.obj" \
	"$(INTDIR)\mappath.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\MapPath.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MapPath.dep")
!INCLUDE "MapPath.dep"
!ELSE 
!MESSAGE Warning: cannot find "MapPath.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MapPath - Win32 Release" || "$(CFG)" == "MapPath - Win32 Debug" || "$(CFG)" == "MapPath - Win32 Hybrid"
SOURCE=.\mappath.cpp

"$(INTDIR)\mappath.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mappath.rc

"$(INTDIR)\mappath.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

