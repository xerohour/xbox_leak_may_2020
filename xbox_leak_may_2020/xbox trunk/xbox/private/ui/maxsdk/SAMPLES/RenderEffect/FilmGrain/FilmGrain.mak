# Microsoft Developer Studio Generated NMAKE File, Based on FilmGrain.dsp
!IF "$(CFG)" == ""
CFG=FilmGrain - Win32 Release
!MESSAGE No configuration specified. Defaulting to FilmGrain - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "FilmGrain - Win32 Release" && "$(CFG)" != "FilmGrain - Win32 Debug" && "$(CFG)" != "FilmGrain - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FilmGrain.mak" CFG="FilmGrain - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FilmGrain - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FilmGrain - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FilmGrain - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "FilmGrain - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"


CLEAN :
	-@erase "$(INTDIR)\FilmGrain.obj"
	-@erase "$(INTDIR)\FilmGrain.res"
	-@erase "$(OUTDIR)\FilmGrain.exp"
	-@erase "$(OUTDIR)\FilmGrain.lib"
	-@erase "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"
	-@erase ".\FilmGrain.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

FilmGrain=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\FilmGrain.pch" /YX /Fo"$(INTDIR)\\" /Fd"FilmGrain.pdb" /FD /c 

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

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\FilmGrain.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilmGrain.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07690000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\FilmGrain.pdb" /machine:I386 /def:".\FilmGrain.def" /out:"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" /implib:"$(OUTDIR)\FilmGrain.lib" /release 
DEF_FILE= \
	".\FilmGrain.def"
LINK32_OBJS= \
	"$(INTDIR)\FilmGrain.obj" \
	"$(INTDIR)\FilmGrain.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FilmGrain - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"


CLEAN :
	-@erase "$(INTDIR)\FilmGrain.obj"
	-@erase "$(INTDIR)\FilmGrain.res"
	-@erase "$(OUTDIR)\FilmGrain.exp"
	-@erase "$(OUTDIR)\FilmGrain.lib"
	-@erase "$(OUTDIR)\FilmGrain.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\FilmGrain.ilk"
	-@erase ".\FilmGrain.idb"
	-@erase ".\FilmGrain.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

FilmGrain=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\FilmGrain.pch" /YX /Fo"$(INTDIR)\\" /Fd"FilmGrain.pdb" /FD /c 

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

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\FilmGrain.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilmGrain.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07690000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\FilmGrain.pdb" /debug /machine:I386 /def:".\FilmGrain.def" /out:"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" /implib:"$(OUTDIR)\FilmGrain.lib" 
DEF_FILE= \
	".\FilmGrain.def"
LINK32_OBJS= \
	"$(INTDIR)\FilmGrain.obj" \
	"$(INTDIR)\FilmGrain.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FilmGrain - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"


CLEAN :
	-@erase "$(INTDIR)\FilmGrain.obj"
	-@erase "$(INTDIR)\FilmGrain.res"
	-@erase "$(OUTDIR)\FilmGrain.exp"
	-@erase "$(OUTDIR)\FilmGrain.lib"
	-@erase "$(OUTDIR)\FilmGrain.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\FilmGrain.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\FilmGrain.ilk"
	-@erase ".\FilmGrain.idb"
	-@erase ".\FilmGrain.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

FilmGrain=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\FilmGrain.pch" /YX /Fo"$(INTDIR)\\" /Fd"FilmGrain.pdb" /FD /c 

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

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\FilmGrain.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilmGrain.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07690000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\FilmGrain.pdb" /debug /machine:I386 /def:".\FilmGrain.def" /out:"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" /implib:"$(OUTDIR)\FilmGrain.lib" 
DEF_FILE= \
	".\FilmGrain.def"
LINK32_OBJS= \
	"$(INTDIR)\FilmGrain.obj" \
	"$(INTDIR)\FilmGrain.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\FilmGrain.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("FilmGrain.dep")
!INCLUDE "FilmGrain.dep"
!ELSE 
!MESSAGE Warning: cannot find "FilmGrain.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FilmGrain - Win32 Release" || "$(CFG)" == "FilmGrain - Win32 Debug" || "$(CFG)" == "FilmGrain - Win32 Hybrid"
SOURCE=.\FilmGrain.cpp

"$(INTDIR)\FilmGrain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FilmGrain.rc

"$(INTDIR)\FilmGrain.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

