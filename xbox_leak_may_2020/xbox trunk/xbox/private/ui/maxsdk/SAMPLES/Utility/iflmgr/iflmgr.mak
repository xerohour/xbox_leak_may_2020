# Microsoft Developer Studio Generated NMAKE File, Based on iflmgr.dsp
!IF "$(CFG)" == ""
CFG=iflmgr - Win32 Release
!MESSAGE No configuration specified. Defaulting to iflmgr - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "iflmgr - Win32 Release" && "$(CFG)" != "iflmgr - Win32 Debug" && "$(CFG)" != "iflmgr - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iflmgr.mak" CFG="iflmgr - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iflmgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iflmgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iflmgr - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "iflmgr - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\iflmgr.dlu"


CLEAN :
	-@erase "$(INTDIR)\iflmgr.obj"
	-@erase "$(INTDIR)\iflmgr.res"
	-@erase "$(OUTDIR)\iflmgr.exp"
	-@erase "$(OUTDIR)\iflmgr.lib"
	-@erase "..\..\..\..\maxsdk\plugin\iflmgr.dlu"
	-@erase ".\iflmgr.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iflmgr.pch" /YX"iflmgr.h" /Fo"$(INTDIR)\\" /Fd"iflmgr.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\iflmgr.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iflmgr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x04F20000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\iflmgr.pdb" /machine:I386 /def:".\iflmgr.def" /out:"..\..\..\..\maxsdk\plugin\iflmgr.dlu" /implib:"$(OUTDIR)\iflmgr.lib" /release 
DEF_FILE= \
	".\iflmgr.def"
LINK32_OBJS= \
	"$(INTDIR)\iflmgr.obj" \
	"$(INTDIR)\iflmgr.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\iflmgr.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iflmgr - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\iflmgr.dlu"


CLEAN :
	-@erase "$(INTDIR)\iflmgr.obj"
	-@erase "$(INTDIR)\iflmgr.res"
	-@erase "$(OUTDIR)\iflmgr.exp"
	-@erase "$(OUTDIR)\iflmgr.lib"
	-@erase "$(OUTDIR)\iflmgr.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\iflmgr.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\iflmgr.ilk"
	-@erase ".\iflmgr.idb"
	-@erase ".\iflmgr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iflmgr.pch" /YX"iflmgr.h" /Fo"$(INTDIR)\\" /Fd"iflmgr.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\iflmgr.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iflmgr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x04F20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\iflmgr.pdb" /debug /machine:I386 /def:".\iflmgr.def" /out:"..\..\..\..\maxsdk\plugin\iflmgr.dlu" /implib:"$(OUTDIR)\iflmgr.lib" 
DEF_FILE= \
	".\iflmgr.def"
LINK32_OBJS= \
	"$(INTDIR)\iflmgr.obj" \
	"$(INTDIR)\iflmgr.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\iflmgr.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iflmgr - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\iflmgr.dlu"


CLEAN :
	-@erase "$(INTDIR)\iflmgr.obj"
	-@erase "$(INTDIR)\iflmgr.res"
	-@erase "$(OUTDIR)\iflmgr.exp"
	-@erase "$(OUTDIR)\iflmgr.lib"
	-@erase "$(OUTDIR)\iflmgr.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\iflmgr.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\iflmgr.ilk"
	-@erase ".\iflmgr.idb"
	-@erase ".\iflmgr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iflmgr.pch" /YX"iflmgr.h" /Fo"$(INTDIR)\\" /Fd"iflmgr.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\iflmgr.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iflmgr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x04F20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\iflmgr.pdb" /debug /machine:I386 /def:".\iflmgr.def" /out:"..\..\..\..\maxsdk\plugin\iflmgr.dlu" /implib:"$(OUTDIR)\iflmgr.lib" 
DEF_FILE= \
	".\iflmgr.def"
LINK32_OBJS= \
	"$(INTDIR)\iflmgr.obj" \
	"$(INTDIR)\iflmgr.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\iflmgr.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("iflmgr.dep")
!INCLUDE "iflmgr.dep"
!ELSE 
!MESSAGE Warning: cannot find "iflmgr.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "iflmgr - Win32 Release" || "$(CFG)" == "iflmgr - Win32 Debug" || "$(CFG)" == "iflmgr - Win32 Hybrid"
SOURCE=.\iflmgr.cpp

"$(INTDIR)\iflmgr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\iflmgr.rc

"$(INTDIR)\iflmgr.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

