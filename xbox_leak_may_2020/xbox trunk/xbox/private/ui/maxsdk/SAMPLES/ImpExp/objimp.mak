# Microsoft Developer Studio Generated NMAKE File, Based on Objimp.dsp
!IF "$(CFG)" == ""
CFG=objimp - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to objimp - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "objimp - Win32 Release" && "$(CFG)" != "objimp - Win32 Debug" && "$(CFG)" != "objimp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Objimp.mak" CFG="objimp - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "objimp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "objimp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "objimp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "objimp - Win32 Release"

OUTDIR=.\release
INTDIR=.\release

ALL : "..\..\..\maxsdk\plugin\objimp.dli"


CLEAN :
	-@erase "$(INTDIR)\objimp.obj"
	-@erase "$(INTDIR)\objimp.res"
	-@erase "$(OUTDIR)\objimp.exp"
	-@erase "$(OUTDIR)\objimp.lib"
	-@erase "..\..\..\maxsdk\plugin\objimp.dli"
	-@erase ".\objimp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"objimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\objimp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Objimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10510000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\objimp.pdb" /machine:I386 /def:".\objimp.def" /out:"..\..\..\maxsdk\plugin\objimp.dli" /implib:"$(OUTDIR)\objimp.lib" /release 
DEF_FILE= \
	".\objimp.def"
LINK32_OBJS= \
	"$(INTDIR)\objimp.obj" \
	"$(INTDIR)\objimp.res" \
	"..\..\Lib\CORE.lib" \
	"..\..\Lib\GEOM.lib" \
	"..\..\Lib\GFX.lib" \
	"..\..\Lib\MESH.lib"

"..\..\..\maxsdk\plugin\objimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "objimp - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug

ALL : "..\..\..\maxsdk\plugin\objimp.dli"


CLEAN :
	-@erase "$(INTDIR)\objimp.obj"
	-@erase "$(INTDIR)\objimp.res"
	-@erase "$(OUTDIR)\objimp.exp"
	-@erase "$(OUTDIR)\objimp.lib"
	-@erase "$(OUTDIR)\objimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\objimp.dli"
	-@erase "..\..\..\maxsdk\plugin\objimp.ilk"
	-@erase ".\objimp.idb"
	-@erase ".\objimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"objimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\objimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Objimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10510000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\objimp.pdb" /debug /machine:I386 /def:".\objimp.def" /out:"..\..\..\maxsdk\plugin\objimp.dli" /implib:"$(OUTDIR)\objimp.lib" 
DEF_FILE= \
	".\objimp.def"
LINK32_OBJS= \
	"$(INTDIR)\objimp.obj" \
	"$(INTDIR)\objimp.res" \
	"..\..\Lib\CORE.lib" \
	"..\..\Lib\GEOM.lib" \
	"..\..\Lib\GFX.lib" \
	"..\..\Lib\MESH.lib"

"..\..\..\maxsdk\plugin\objimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "objimp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\objimp.dli"


CLEAN :
	-@erase "$(INTDIR)\objimp.obj"
	-@erase "$(INTDIR)\objimp.res"
	-@erase "$(OUTDIR)\objimp.exp"
	-@erase "$(OUTDIR)\objimp.lib"
	-@erase "$(OUTDIR)\objimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\objimp.dli"
	-@erase "..\..\..\maxsdk\plugin\objimp.ilk"
	-@erase ".\objimp.idb"
	-@erase ".\objimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"objimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\objimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Objimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10510000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\objimp.pdb" /debug /machine:I386 /def:".\objimp.def" /out:"..\..\..\maxsdk\plugin\objimp.dli" /implib:"$(OUTDIR)\objimp.lib" 
DEF_FILE= \
	".\objimp.def"
LINK32_OBJS= \
	"$(INTDIR)\objimp.obj" \
	"$(INTDIR)\objimp.res" \
	"..\..\Lib\CORE.lib" \
	"..\..\Lib\GEOM.lib" \
	"..\..\Lib\GFX.lib" \
	"..\..\Lib\MESH.lib"

"..\..\..\maxsdk\plugin\objimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Objimp.dep")
!INCLUDE "Objimp.dep"
!ELSE 
!MESSAGE Warning: cannot find "Objimp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "objimp - Win32 Release" || "$(CFG)" == "objimp - Win32 Debug" || "$(CFG)" == "objimp - Win32 Hybrid"
SOURCE=.\objimp.cpp

"$(INTDIR)\objimp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objimp.rc

"$(INTDIR)\objimp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

