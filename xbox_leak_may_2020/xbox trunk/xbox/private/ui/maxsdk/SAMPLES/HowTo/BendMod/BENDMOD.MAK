# Microsoft Developer Studio Generated NMAKE File, Based on BendMod.dsp
!IF "$(CFG)" == ""
CFG=BendMod - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to BendMod - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "BendMod - Win32 Release" && "$(CFG)" != "BendMod - Win32 Debug" && "$(CFG)" != "BendMod - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BendMod.mak" CFG="BendMod - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BendMod - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BendMod - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BendMod - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "BendMod - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\bendmod.dlm"


CLEAN :
	-@erase "$(INTDIR)\BendMod.obj"
	-@erase "$(INTDIR)\BendMod.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\bendmod.exp"
	-@erase "$(OUTDIR)\bendmod.lib"
	-@erase "..\..\..\plugin\bendmod.dlm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\BendMod.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\BendMod.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BendMod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A40000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\bendmod.pdb" /machine:I386 /def:".\BendMod.def" /out:"..\..\..\..\maxsdk\plugin\bendmod.dlm" /implib:"$(OUTDIR)\bendmod.lib" /release 
DEF_FILE= \
	".\BendMod.def"
LINK32_OBJS= \
	"$(INTDIR)\BendMod.obj" \
	"$(INTDIR)\BendMod.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\bendmod.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BendMod - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\bendmod.dlm"


CLEAN :
	-@erase "$(INTDIR)\BendMod.obj"
	-@erase "$(INTDIR)\BendMod.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\bendmod.exp"
	-@erase "$(OUTDIR)\bendmod.lib"
	-@erase "$(OUTDIR)\bendmod.pdb"
	-@erase "..\..\..\plugin\bendmod.dlm"
	-@erase "..\..\..\plugin\bendmod.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\BendMod.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\BendMod.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BendMod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\bendmod.pdb" /debug /machine:I386 /def:".\BendMod.def" /out:"..\..\..\..\maxsdk\plugin\bendmod.dlm" /implib:"$(OUTDIR)\bendmod.lib" /pdbtype:sept 
DEF_FILE= \
	".\BendMod.def"
LINK32_OBJS= \
	"$(INTDIR)\BendMod.obj" \
	"$(INTDIR)\BendMod.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\bendmod.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BendMod - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\bendmod.dlm"


CLEAN :
	-@erase "$(INTDIR)\BendMod.obj"
	-@erase "$(INTDIR)\BendMod.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\bendmod.exp"
	-@erase "$(OUTDIR)\bendmod.lib"
	-@erase "$(OUTDIR)\bendmod.pdb"
	-@erase "..\..\..\plugin\bendmod.dlm"
	-@erase "..\..\..\plugin\bendmod.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\BendMod.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\BendMod.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BendMod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\bendmod.pdb" /debug /machine:I386 /def:".\BendMod.def" /out:"..\..\..\..\maxsdk\plugin\bendmod.dlm" /implib:"$(OUTDIR)\bendmod.lib" /pdbtype:sept 
DEF_FILE= \
	".\BendMod.def"
LINK32_OBJS= \
	"$(INTDIR)\BendMod.obj" \
	"$(INTDIR)\BendMod.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\bendmod.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("BendMod.dep")
!INCLUDE "BendMod.dep"
!ELSE 
!MESSAGE Warning: cannot find "BendMod.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "BendMod - Win32 Release" || "$(CFG)" == "BendMod - Win32 Debug" || "$(CFG)" == "BendMod - Win32 Hybrid"
SOURCE=.\BendMod.cpp

"$(INTDIR)\BendMod.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\BendMod.rc

"$(INTDIR)\BendMod.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

