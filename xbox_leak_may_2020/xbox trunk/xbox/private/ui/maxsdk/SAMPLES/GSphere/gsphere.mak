# Microsoft Developer Studio Generated NMAKE File, Based on Gsphere.dsp
!IF "$(CFG)" == ""
CFG=gsphere - Win32 Release
!MESSAGE No configuration specified. Defaulting to gsphere - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "gsphere - Win32 Release" && "$(CFG)" != "gsphere - Win32 Debug" && "$(CFG)" != "gsphere - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gsphere.mak" CFG="gsphere - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gsphere - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gsphere - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gsphere - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "gsphere - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\gsphere.dlo"


CLEAN :
	-@erase "$(INTDIR)\GSPHERE.OBJ"
	-@erase "$(INTDIR)\PRIM.OBJ"
	-@erase "$(INTDIR)\PRIM.res"
	-@erase "$(OUTDIR)\gsphere.exp"
	-@erase "$(OUTDIR)\gsphere.lib"
	-@erase "..\..\..\maxsdk\plugin\gsphere.dlo"
	-@erase ".\gsphere.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"gsphere.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PRIM.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gsphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x05390000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\gsphere.pdb" /machine:I386 /def:".\PRIM.DEF" /out:"..\..\..\maxsdk\plugin\gsphere.dlo" /implib:"$(OUTDIR)\gsphere.lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\PRIM.DEF"
LINK32_OBJS= \
	"$(INTDIR)\GSPHERE.OBJ" \
	"$(INTDIR)\PRIM.OBJ" \
	"$(INTDIR)\PRIM.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\gsphere.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gsphere - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\gsphere.dlo"


CLEAN :
	-@erase "$(INTDIR)\GSPHERE.OBJ"
	-@erase "$(INTDIR)\PRIM.OBJ"
	-@erase "$(INTDIR)\PRIM.res"
	-@erase "$(OUTDIR)\gsphere.exp"
	-@erase "$(OUTDIR)\gsphere.lib"
	-@erase "$(OUTDIR)\gsphere.pdb"
	-@erase "..\..\..\maxsdk\plugin\gsphere.dlo"
	-@erase "..\..\..\maxsdk\plugin\gsphere.ilk"
	-@erase ".\gsphere.idb"
	-@erase ".\gsphere.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"gsphere.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PRIM.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gsphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x05390000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\gsphere.pdb" /debug /machine:I386 /def:".\PRIM.DEF" /out:"..\..\..\maxsdk\plugin\gsphere.dlo" /implib:"$(OUTDIR)\gsphere.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\PRIM.DEF"
LINK32_OBJS= \
	"$(INTDIR)\GSPHERE.OBJ" \
	"$(INTDIR)\PRIM.OBJ" \
	"$(INTDIR)\PRIM.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\gsphere.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gsphere - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\gsphere.dlo"


CLEAN :
	-@erase "$(INTDIR)\GSPHERE.OBJ"
	-@erase "$(INTDIR)\PRIM.OBJ"
	-@erase "$(INTDIR)\PRIM.res"
	-@erase "$(OUTDIR)\gsphere.exp"
	-@erase "$(OUTDIR)\gsphere.lib"
	-@erase "$(OUTDIR)\gsphere.pdb"
	-@erase "..\..\..\maxsdk\plugin\gsphere.dlo"
	-@erase "..\..\..\maxsdk\plugin\gsphere.ilk"
	-@erase ".\gsphere.idb"
	-@erase ".\gsphere.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"gsphere.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PRIM.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gsphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x05390000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\gsphere.pdb" /debug /machine:I386 /def:".\PRIM.DEF" /out:"..\..\..\maxsdk\plugin\gsphere.dlo" /implib:"$(OUTDIR)\gsphere.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\PRIM.DEF"
LINK32_OBJS= \
	"$(INTDIR)\GSPHERE.OBJ" \
	"$(INTDIR)\PRIM.OBJ" \
	"$(INTDIR)\PRIM.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\gsphere.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Gsphere.dep")
!INCLUDE "Gsphere.dep"
!ELSE 
!MESSAGE Warning: cannot find "Gsphere.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gsphere - Win32 Release" || "$(CFG)" == "gsphere - Win32 Debug" || "$(CFG)" == "gsphere - Win32 Hybrid"
SOURCE=.\GSPHERE.CPP

"$(INTDIR)\GSPHERE.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PRIM.CPP

"$(INTDIR)\PRIM.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PRIM.RC

"$(INTDIR)\PRIM.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

