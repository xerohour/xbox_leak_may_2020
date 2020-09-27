# Microsoft Developer Studio Generated NMAKE File, Based on dynwarps.dsp
!IF "$(CFG)" == ""
CFG=dynwarps - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to dynwarps - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "dynwarps - Win32 Release" && "$(CFG)" != "dynwarps - Win32 Debug" && "$(CFG)" != "dynwarps - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynwarps.mak" CFG="dynwarps - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dynwarps - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dynwarps - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dynwarps - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dynwarps - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\dynwarps.dlo"


CLEAN :
	-@erase "$(INTDIR)\dynw.obj"
	-@erase "$(INTDIR)\dynwarps.res"
	-@erase "$(INTDIR)\force.obj"
	-@erase "$(INTDIR)\motor.obj"
	-@erase "$(INTDIR)\pin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dynwarps.exp"
	-@erase "$(OUTDIR)\dynwarps.lib"
	-@erase "..\..\..\maxsdk\plugin\dynwarps.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dynwarps.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dynwarps.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dynwarps.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x051B0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dynwarps.pdb" /machine:I386 /def:".\dynwarps.def" /out:"..\..\..\maxsdk\plugin\dynwarps.dlo" /implib:"$(OUTDIR)\dynwarps.lib" /release 
DEF_FILE= \
	".\dynwarps.def"
LINK32_OBJS= \
	"$(INTDIR)\dynw.obj" \
	"$(INTDIR)\force.obj" \
	"$(INTDIR)\motor.obj" \
	"$(INTDIR)\pin.obj" \
	"$(INTDIR)\dynwarps.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\expr.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dynwarps.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dynwarps - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\dynwarps.dlo"


CLEAN :
	-@erase "$(INTDIR)\dynw.obj"
	-@erase "$(INTDIR)\dynwarps.res"
	-@erase "$(INTDIR)\force.obj"
	-@erase "$(INTDIR)\motor.obj"
	-@erase "$(INTDIR)\pin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dynwarps.exp"
	-@erase "$(OUTDIR)\dynwarps.lib"
	-@erase "$(OUTDIR)\dynwarps.pdb"
	-@erase "..\..\..\maxsdk\plugin\dynwarps.dlo"
	-@erase "..\..\..\maxsdk\plugin\dynwarps.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dynwarps.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dynwarps.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dynwarps.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x051B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dynwarps.pdb" /debug /machine:I386 /def:".\dynwarps.def" /out:"..\..\..\maxsdk\plugin\dynwarps.dlo" /implib:"$(OUTDIR)\dynwarps.lib" /pdbtype:sept 
DEF_FILE= \
	".\dynwarps.def"
LINK32_OBJS= \
	"$(INTDIR)\dynw.obj" \
	"$(INTDIR)\force.obj" \
	"$(INTDIR)\motor.obj" \
	"$(INTDIR)\pin.obj" \
	"$(INTDIR)\dynwarps.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\expr.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dynwarps.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dynwarps - Win32 Hybrid"

OUTDIR=.\hybrid
INTDIR=.\hybrid

ALL : "..\..\..\maxsdk\plugin\dynwarps.dlo"


CLEAN :
	-@erase "$(INTDIR)\dynw.obj"
	-@erase "$(INTDIR)\dynwarps.res"
	-@erase "$(INTDIR)\force.obj"
	-@erase "$(INTDIR)\motor.obj"
	-@erase "$(INTDIR)\pin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dynwarps.exp"
	-@erase "$(OUTDIR)\dynwarps.lib"
	-@erase "$(OUTDIR)\dynwarps.pdb"
	-@erase "..\..\..\maxsdk\plugin\dynwarps.dlo"
	-@erase "..\..\..\maxsdk\plugin\dynwarps.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dynwarps.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dynwarps.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dynwarps.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x051B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dynwarps.pdb" /debug /machine:I386 /def:".\dynwarps.def" /out:"..\..\..\maxsdk\plugin\dynwarps.dlo" /implib:"$(OUTDIR)\dynwarps.lib" /pdbtype:sept 
DEF_FILE= \
	".\dynwarps.def"
LINK32_OBJS= \
	"$(INTDIR)\dynw.obj" \
	"$(INTDIR)\force.obj" \
	"$(INTDIR)\motor.obj" \
	"$(INTDIR)\pin.obj" \
	"$(INTDIR)\dynwarps.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\expr.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dynwarps.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dynwarps.dep")
!INCLUDE "dynwarps.dep"
!ELSE 
!MESSAGE Warning: cannot find "dynwarps.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dynwarps - Win32 Release" || "$(CFG)" == "dynwarps - Win32 Debug" || "$(CFG)" == "dynwarps - Win32 Hybrid"
SOURCE=.\dynw.cpp

"$(INTDIR)\dynw.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dynwarps.rc

"$(INTDIR)\dynwarps.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\force.cpp

"$(INTDIR)\force.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\motor.cpp

"$(INTDIR)\motor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pin.cpp

"$(INTDIR)\pin.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

