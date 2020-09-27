# Microsoft Developer Studio Generated NMAKE File, Based on spherify.dsp
!IF "$(CFG)" == ""
CFG=SPHERIFY - Win32 Release
!MESSAGE No configuration specified. Defaulting to SPHERIFY - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "SPHERIFY - Win32 Release" && "$(CFG)" != "SPHERIFY - Win32 Hybrid" && "$(CFG)" != "SPHERIFY - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "spherify.mak" CFG="SPHERIFY - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SPHERIFY - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SPHERIFY - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SPHERIFY - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "SPHERIFY - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\spherify.dlm"


CLEAN :
	-@erase "$(INTDIR)\SPHERIFY.OBJ"
	-@erase "$(INTDIR)\SPHERIFY.res"
	-@erase "$(OUTDIR)\spherify.exp"
	-@erase "$(OUTDIR)\spherify.lib"
	-@erase "..\..\..\maxsdk\plugin\spherify.dlm"
	-@erase ".\SPHERIFY.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\spherify.pch" /YX /Fo"$(INTDIR)\\" /Fd"SPHERIFY.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SPHERIFY.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\spherify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05C00000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\spherify.pdb" /machine:I386 /def:".\SPHERIFY.DEF" /out:"..\..\..\maxsdk\plugin\spherify.dlm" /implib:"$(OUTDIR)\spherify.lib" /release 
DEF_FILE= \
	".\SPHERIFY.DEF"
LINK32_OBJS= \
	"$(INTDIR)\SPHERIFY.OBJ" \
	"$(INTDIR)\SPHERIFY.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\spherify.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SPHERIFY - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\spherify.dlm"


CLEAN :
	-@erase "$(INTDIR)\SPHERIFY.OBJ"
	-@erase "$(INTDIR)\SPHERIFY.res"
	-@erase "$(OUTDIR)\spherify.exp"
	-@erase "$(OUTDIR)\spherify.lib"
	-@erase "$(OUTDIR)\spherify.pdb"
	-@erase "..\..\..\maxsdk\plugin\spherify.dlm"
	-@erase "..\..\..\maxsdk\plugin\spherify.ilk"
	-@erase ".\SPHERIFY.idb"
	-@erase ".\SPHERIFY.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\spherify.pch" /YX /Fo"$(INTDIR)\\" /Fd"SPHERIFY.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SPHERIFY.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\spherify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05C00000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\spherify.pdb" /debug /machine:I386 /def:".\SPHERIFY.DEF" /out:"..\..\..\maxsdk\plugin\spherify.dlm" /implib:"$(OUTDIR)\spherify.lib" 
DEF_FILE= \
	".\SPHERIFY.DEF"
LINK32_OBJS= \
	"$(INTDIR)\SPHERIFY.OBJ" \
	"$(INTDIR)\SPHERIFY.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\spherify.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SPHERIFY - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\spherify.dlm"


CLEAN :
	-@erase "$(INTDIR)\SPHERIFY.OBJ"
	-@erase "$(INTDIR)\SPHERIFY.res"
	-@erase "$(OUTDIR)\spherify.exp"
	-@erase "$(OUTDIR)\spherify.lib"
	-@erase "$(OUTDIR)\spherify.pdb"
	-@erase "..\..\..\maxsdk\plugin\spherify.dlm"
	-@erase "..\..\..\maxsdk\plugin\spherify.ilk"
	-@erase ".\SPHERIFY.idb"
	-@erase ".\SPHERIFY.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\spherify.pch" /YX /Fo"$(INTDIR)\\" /Fd"SPHERIFY.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SPHERIFY.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\spherify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05C00000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\spherify.pdb" /debug /machine:I386 /def:".\SPHERIFY.DEF" /out:"..\..\..\maxsdk\plugin\spherify.dlm" /implib:"$(OUTDIR)\spherify.lib" 
DEF_FILE= \
	".\SPHERIFY.DEF"
LINK32_OBJS= \
	"$(INTDIR)\SPHERIFY.OBJ" \
	"$(INTDIR)\SPHERIFY.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\spherify.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("spherify.dep")
!INCLUDE "spherify.dep"
!ELSE 
!MESSAGE Warning: cannot find "spherify.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SPHERIFY - Win32 Release" || "$(CFG)" == "SPHERIFY - Win32 Hybrid" || "$(CFG)" == "SPHERIFY - Win32 Debug"
SOURCE=.\SPHERIFY.CPP

"$(INTDIR)\SPHERIFY.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SPHERIFY.RC

"$(INTDIR)\SPHERIFY.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

