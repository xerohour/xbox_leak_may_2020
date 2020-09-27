# Microsoft Developer Studio Generated NMAKE File, Based on ascout.dsp
!IF "$(CFG)" == ""
CFG=ascout - Win32 Release
!MESSAGE No configuration specified. Defaulting to ascout - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "ascout - Win32 Release" && "$(CFG)" != "ascout - Win32 Debug" && "$(CFG)" != "ascout - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ascout.mak" CFG="ascout - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ascout - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ascout - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ascout - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ascout - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\ascout.dle"


CLEAN :
	-@erase "$(INTDIR)\ascout.obj"
	-@erase "$(INTDIR)\ascout.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\ascout.exp"
	-@erase "$(OUTDIR)\ascout.lib"
	-@erase "..\..\..\..\maxsdk\plugin\ascout.dle"
	-@erase ".\ascout.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ascout.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ascout.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ascout.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x086C0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ascout.pdb" /machine:I386 /def:".\ascout.def" /out:"..\..\..\..\maxsdk\plugin\ascout.dle" /implib:"$(OUTDIR)\ascout.lib" /release 
DEF_FILE= \
	".\ascout.def"
LINK32_OBJS= \
	"$(INTDIR)\ascout.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\ascout.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ascout.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ascout - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\ascout.dle"


CLEAN :
	-@erase "$(INTDIR)\ascout.obj"
	-@erase "$(INTDIR)\ascout.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\ascout.exp"
	-@erase "$(OUTDIR)\ascout.lib"
	-@erase "$(OUTDIR)\ascout.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\ascout.dle"
	-@erase "..\..\..\..\maxsdk\plugin\ascout.ilk"
	-@erase ".\ascout.idb"
	-@erase ".\ascout.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ascout.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ascout.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ascout.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x086C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ascout.pdb" /debug /machine:I386 /def:".\ascout.def" /out:"..\..\..\..\maxsdk\plugin\ascout.dle" /implib:"$(OUTDIR)\ascout.lib" 
DEF_FILE= \
	".\ascout.def"
LINK32_OBJS= \
	"$(INTDIR)\ascout.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\ascout.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ascout.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ascout - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\ascout.dle"


CLEAN :
	-@erase "$(INTDIR)\ascout.obj"
	-@erase "$(INTDIR)\ascout.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\ascout.exp"
	-@erase "$(OUTDIR)\ascout.lib"
	-@erase "$(OUTDIR)\ascout.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\ascout.dle"
	-@erase "..\..\..\..\maxsdk\plugin\ascout.ilk"
	-@erase ".\ascout.idb"
	-@erase ".\ascout.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ascout.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ascout.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ascout.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x086C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ascout.pdb" /debug /machine:I386 /def:".\ascout.def" /out:"..\..\..\..\maxsdk\plugin\ascout.dle" /implib:"$(OUTDIR)\ascout.lib" 
DEF_FILE= \
	".\ascout.def"
LINK32_OBJS= \
	"$(INTDIR)\ascout.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\ascout.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ascout.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ascout.dep")
!INCLUDE "ascout.dep"
!ELSE 
!MESSAGE Warning: cannot find "ascout.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ascout - Win32 Release" || "$(CFG)" == "ascout - Win32 Debug" || "$(CFG)" == "ascout - Win32 Hybrid"
SOURCE=.\ascout.cpp

"$(INTDIR)\ascout.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\export.cpp

"$(INTDIR)\export.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ascout.rc

"$(INTDIR)\ascout.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

