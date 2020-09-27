# Microsoft Developer Studio Generated NMAKE File, Based on Sphere_c.dsp
!IF "$(CFG)" == ""
CFG=Sphere_c - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Sphere_c - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Sphere_c - Win32 Release" && "$(CFG)" != "Sphere_c - Win32 Debug" && "$(CFG)" != "Sphere_c - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sphere_c.mak" CFG="Sphere_c - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sphere_c - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Sphere_c - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Sphere_c - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Sphere_c - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\sphere_c.dlo"


CLEAN :
	-@erase "$(INTDIR)\Sphere_c.obj"
	-@erase "$(INTDIR)\Sphere_c.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sphere_c.exp"
	-@erase "$(OUTDIR)\sphere_c.lib"
	-@erase "..\..\..\plugin\sphere_c.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Sphere_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Sphere_c.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Sphere_c.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A10000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\sphere_c.pdb" /machine:I386 /def:".\Sphere_c.def" /out:"..\..\..\..\maxsdk\plugin\sphere_c.dlo" /implib:"$(OUTDIR)\sphere_c.lib" /release 
DEF_FILE= \
	".\Sphere_c.def"
LINK32_OBJS= \
	"$(INTDIR)\Sphere_c.obj" \
	"$(INTDIR)\Sphere_c.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\edmodel.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\gfx.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\sphere_c.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Sphere_c - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\sphere_c.dlo"


CLEAN :
	-@erase "$(INTDIR)\Sphere_c.obj"
	-@erase "$(INTDIR)\Sphere_c.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sphere_c.exp"
	-@erase "$(OUTDIR)\sphere_c.lib"
	-@erase "$(OUTDIR)\sphere_c.pdb"
	-@erase "..\..\..\plugin\sphere_c.dlo"
	-@erase "..\..\..\plugin\sphere_c.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Sphere_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Sphere_c.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Sphere_c.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\sphere_c.pdb" /debug /machine:I386 /def:".\Sphere_c.def" /out:"..\..\..\..\maxsdk\plugin\sphere_c.dlo" /implib:"$(OUTDIR)\sphere_c.lib" /pdbtype:sept 
DEF_FILE= \
	".\Sphere_c.def"
LINK32_OBJS= \
	"$(INTDIR)\Sphere_c.obj" \
	"$(INTDIR)\Sphere_c.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\edmodel.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\gfx.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\sphere_c.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Sphere_c - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\sphere_c.dlo"


CLEAN :
	-@erase "$(INTDIR)\Sphere_c.obj"
	-@erase "$(INTDIR)\Sphere_c.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sphere_c.exp"
	-@erase "$(OUTDIR)\sphere_c.lib"
	-@erase "$(OUTDIR)\sphere_c.pdb"
	-@erase "..\..\..\plugin\sphere_c.dlo"
	-@erase "..\..\..\plugin\sphere_c.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Sphere_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Sphere_c.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Sphere_c.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\sphere_c.pdb" /debug /machine:I386 /def:".\Sphere_c.def" /out:"..\..\..\..\maxsdk\plugin\sphere_c.dlo" /implib:"$(OUTDIR)\sphere_c.lib" /pdbtype:sept 
DEF_FILE= \
	".\Sphere_c.def"
LINK32_OBJS= \
	"$(INTDIR)\Sphere_c.obj" \
	"$(INTDIR)\Sphere_c.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\edmodel.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\gfx.lib" \
	"..\..\..\Lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\sphere_c.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Sphere_c.dep")
!INCLUDE "Sphere_c.dep"
!ELSE 
!MESSAGE Warning: cannot find "Sphere_c.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Sphere_c - Win32 Release" || "$(CFG)" == "Sphere_c - Win32 Debug" || "$(CFG)" == "Sphere_c - Win32 Hybrid"
SOURCE=.\Sphere_c.cpp

"$(INTDIR)\Sphere_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Sphere_c.rc

"$(INTDIR)\Sphere_c.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

