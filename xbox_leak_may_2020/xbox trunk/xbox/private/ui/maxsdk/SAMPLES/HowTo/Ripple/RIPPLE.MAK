# Microsoft Developer Studio Generated NMAKE File, Based on Ripple.dsp
!IF "$(CFG)" == ""
CFG=Ripple - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Ripple - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Ripple - Win32 Release" && "$(CFG)" != "Ripple - Win32 Debug" && "$(CFG)" != "Ripple - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ripple.mak" CFG="Ripple - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Ripple - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ripple - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ripple - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Ripple - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\ripple.dlm"


CLEAN :
	-@erase "$(INTDIR)\Ripple.obj"
	-@erase "$(INTDIR)\Ripple.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ripple.exp"
	-@erase "$(OUTDIR)\ripple.lib"
	-@erase "..\..\..\plugin\ripple.dlm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Ripple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Ripple.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ripple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A90000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ripple.pdb" /machine:I386 /def:".\RIPPLE.DEF" /out:"..\..\..\..\maxsdk\plugin\ripple.dlm" /implib:"$(OUTDIR)\ripple.lib" /release 
DEF_FILE= \
	".\RIPPLE.DEF"
LINK32_OBJS= \
	"$(INTDIR)\Ripple.obj" \
	"$(INTDIR)\Ripple.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\ripple.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Ripple - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\ripple.dlm"


CLEAN :
	-@erase "$(INTDIR)\Ripple.obj"
	-@erase "$(INTDIR)\Ripple.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ripple.exp"
	-@erase "$(OUTDIR)\ripple.lib"
	-@erase "$(OUTDIR)\ripple.pdb"
	-@erase "..\..\..\plugin\ripple.dlm"
	-@erase "..\..\..\plugin\ripple.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Ripple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Ripple.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ripple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A90000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ripple.pdb" /debug /machine:I386 /def:".\RIPPLE.DEF" /out:"..\..\..\..\maxsdk\plugin\ripple.dlm" /implib:"$(OUTDIR)\ripple.lib" /pdbtype:sept 
DEF_FILE= \
	".\RIPPLE.DEF"
LINK32_OBJS= \
	"$(INTDIR)\Ripple.obj" \
	"$(INTDIR)\Ripple.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\ripple.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Ripple - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\ripple.dlm"


CLEAN :
	-@erase "$(INTDIR)\Ripple.obj"
	-@erase "$(INTDIR)\Ripple.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ripple.exp"
	-@erase "$(OUTDIR)\ripple.lib"
	-@erase "$(OUTDIR)\ripple.pdb"
	-@erase "..\..\..\plugin\ripple.dlm"
	-@erase "..\..\..\plugin\ripple.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Ripple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Ripple.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ripple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08A90000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ripple.pdb" /debug /machine:I386 /def:".\RIPPLE.DEF" /out:"..\..\..\..\maxsdk\plugin\ripple.dlm" /implib:"$(OUTDIR)\ripple.lib" /pdbtype:sept 
DEF_FILE= \
	".\RIPPLE.DEF"
LINK32_OBJS= \
	"$(INTDIR)\Ripple.obj" \
	"$(INTDIR)\Ripple.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\ripple.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Ripple.dep")
!INCLUDE "Ripple.dep"
!ELSE 
!MESSAGE Warning: cannot find "Ripple.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Ripple - Win32 Release" || "$(CFG)" == "Ripple - Win32 Debug" || "$(CFG)" == "Ripple - Win32 Hybrid"
SOURCE=.\Ripple.cpp

"$(INTDIR)\Ripple.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Ripple.rc

"$(INTDIR)\Ripple.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

