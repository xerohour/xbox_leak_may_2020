# Microsoft Developer Studio Generated NMAKE File, Based on Melt.dsp
!IF "$(CFG)" == ""
CFG=Melt - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Melt - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Melt - Win32 Release" && "$(CFG)" != "Melt - Win32 Debug" && "$(CFG)" != "Melt - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Melt.mak" CFG="Melt - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Melt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Melt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Melt - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Melt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\melt.dlm"


CLEAN :
	-@erase "$(INTDIR)\MELT.OBJ"
	-@erase "$(INTDIR)\Melt.res"
	-@erase "$(OUTDIR)\melt.exp"
	-@erase "$(OUTDIR)\melt.lib"
	-@erase "..\..\..\maxsdk\plugin\melt.dlm"
	-@erase ".\Melt.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Melt.pch" /YX /Fo"$(INTDIR)\\" /Fd"Melt.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Melt.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Melt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\melt.pdb" /machine:I386 /def:".\MELT.DEF" /out:"..\..\..\maxsdk\plugin\melt.dlm" /implib:"$(OUTDIR)\melt.lib" /release 
DEF_FILE= \
	".\MELT.DEF"
LINK32_OBJS= \
	"$(INTDIR)\MELT.OBJ" \
	"$(INTDIR)\Melt.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\viewfile.lib"

"..\..\..\maxsdk\plugin\melt.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Melt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\melt.dlm"


CLEAN :
	-@erase "$(INTDIR)\MELT.OBJ"
	-@erase "$(INTDIR)\Melt.res"
	-@erase "$(OUTDIR)\melt.exp"
	-@erase "$(OUTDIR)\melt.lib"
	-@erase "$(OUTDIR)\melt.pdb"
	-@erase "..\..\..\maxsdk\plugin\melt.dlm"
	-@erase "..\..\..\maxsdk\plugin\melt.ilk"
	-@erase ".\Melt.idb"
	-@erase ".\Melt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Melt.pch" /YX /Fo"$(INTDIR)\\" /Fd"Melt.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Melt.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Melt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\melt.pdb" /debug /machine:I386 /def:".\MELT.DEF" /out:"..\..\..\maxsdk\plugin\melt.dlm" /implib:"$(OUTDIR)\melt.lib" /pdbtype:sept 
DEF_FILE= \
	".\MELT.DEF"
LINK32_OBJS= \
	"$(INTDIR)\MELT.OBJ" \
	"$(INTDIR)\Melt.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\viewfile.lib"

"..\..\..\maxsdk\plugin\melt.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Melt - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\melt.dlm"


CLEAN :
	-@erase "$(INTDIR)\MELT.OBJ"
	-@erase "$(INTDIR)\Melt.res"
	-@erase "$(OUTDIR)\melt.exp"
	-@erase "$(OUTDIR)\melt.lib"
	-@erase "$(OUTDIR)\melt.pdb"
	-@erase "..\..\..\maxsdk\plugin\melt.dlm"
	-@erase "..\..\..\maxsdk\plugin\melt.ilk"
	-@erase ".\Melt.idb"
	-@erase ".\Melt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Melt.pch" /YX /Fo"$(INTDIR)\\" /Fd"Melt.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Melt.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Melt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08660000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\melt.pdb" /debug /machine:I386 /def:".\MELT.DEF" /out:"..\..\..\maxsdk\plugin\melt.dlm" /implib:"$(OUTDIR)\melt.lib" /pdbtype:sept 
DEF_FILE= \
	".\MELT.DEF"
LINK32_OBJS= \
	"$(INTDIR)\MELT.OBJ" \
	"$(INTDIR)\Melt.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\viewfile.lib"

"..\..\..\maxsdk\plugin\melt.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Melt.dep")
!INCLUDE "Melt.dep"
!ELSE 
!MESSAGE Warning: cannot find "Melt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Melt - Win32 Release" || "$(CFG)" == "Melt - Win32 Debug" || "$(CFG)" == "Melt - Win32 Hybrid"
SOURCE=.\MELT.CPP

"$(INTDIR)\MELT.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Melt.rc

"$(INTDIR)\Melt.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

