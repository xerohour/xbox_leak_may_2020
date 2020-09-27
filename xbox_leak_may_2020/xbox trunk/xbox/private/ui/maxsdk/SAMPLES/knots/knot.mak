# Microsoft Developer Studio Generated NMAKE File, Based on knot.dsp
!IF "$(CFG)" == ""
CFG=knot - Win32 Release
!MESSAGE No configuration specified. Defaulting to knot - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "knot - Win32 Release" && "$(CFG)" != "knot - Win32 Debug" && "$(CFG)" != "knot - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "knot.mak" CFG="knot - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "knot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "knot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "knot - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "knot - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\knot.dlo"


CLEAN :
	-@erase "$(INTDIR)\knot.obj"
	-@erase "$(INTDIR)\knot.res"
	-@erase "$(INTDIR)\knotbase.obj"
	-@erase "$(INTDIR)\ringwave.obj"
	-@erase "$(OUTDIR)\knot.exp"
	-@erase "$(OUTDIR)\knot.lib"
	-@erase "..\..\..\maxsdk\plugin\knot.dlo"
	-@erase ".\knot.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\knot.pch" /YX /Fo"$(INTDIR)\\" /Fd"knot.pdb" /FD /QIfdiv- /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\knot.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\knot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib mesh.lib maxutil.lib edmodel.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x053C0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\knot.pdb" /machine:I386 /def:".\knot.def" /out:"..\..\..\maxsdk\plugin\knot.dlo" /implib:"$(OUTDIR)\knot.lib" /libpath:"..\..\lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\knot.def"
LINK32_OBJS= \
	"$(INTDIR)\knot.obj" \
	"$(INTDIR)\knotbase.obj" \
	"$(INTDIR)\ringwave.obj" \
	"$(INTDIR)\knot.res"

"..\..\..\maxsdk\plugin\knot.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "knot - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\knot.dlo"


CLEAN :
	-@erase "$(INTDIR)\knot.obj"
	-@erase "$(INTDIR)\knot.res"
	-@erase "$(INTDIR)\knotbase.obj"
	-@erase "$(INTDIR)\ringwave.obj"
	-@erase "$(OUTDIR)\knot.exp"
	-@erase "$(OUTDIR)\knot.lib"
	-@erase "$(OUTDIR)\knot.pdb"
	-@erase "..\..\..\maxsdk\plugin\knot.dlo"
	-@erase "..\..\..\maxsdk\plugin\knot.ilk"
	-@erase ".\knot.idb"
	-@erase ".\knot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\knot.pch" /YX /Fo"$(INTDIR)\\" /Fd"knot.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\knot.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\knot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib mesh.lib maxutil.lib edmodel.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x053C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\knot.pdb" /debug /machine:I386 /def:".\knot.def" /out:"..\..\..\maxsdk\plugin\knot.dlo" /implib:"$(OUTDIR)\knot.lib" /libpath:"..\..\lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\knot.def"
LINK32_OBJS= \
	"$(INTDIR)\knot.obj" \
	"$(INTDIR)\knotbase.obj" \
	"$(INTDIR)\ringwave.obj" \
	"$(INTDIR)\knot.res"

"..\..\..\maxsdk\plugin\knot.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "knot - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\knot.dlo"


CLEAN :
	-@erase "$(INTDIR)\knot.obj"
	-@erase "$(INTDIR)\knot.res"
	-@erase "$(INTDIR)\knotbase.obj"
	-@erase "$(INTDIR)\ringwave.obj"
	-@erase "$(OUTDIR)\knot.exp"
	-@erase "$(OUTDIR)\knot.lib"
	-@erase "$(OUTDIR)\knot.pdb"
	-@erase "..\..\..\maxsdk\plugin\knot.dlo"
	-@erase "..\..\..\maxsdk\plugin\knot.ilk"
	-@erase ".\knot.idb"
	-@erase ".\knot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\knot.pch" /YX /Fo"$(INTDIR)\\" /Fd"knot.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\knot.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\knot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib mesh.lib maxutil.lib edmodel.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x053C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\knot.pdb" /debug /machine:I386 /def:".\knot.def" /out:"..\..\..\maxsdk\plugin\knot.dlo" /implib:"$(OUTDIR)\knot.lib" /libpath:"..\..\lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\knot.def"
LINK32_OBJS= \
	"$(INTDIR)\knot.obj" \
	"$(INTDIR)\knotbase.obj" \
	"$(INTDIR)\ringwave.obj" \
	"$(INTDIR)\knot.res"

"..\..\..\maxsdk\plugin\knot.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("knot.dep")
!INCLUDE "knot.dep"
!ELSE 
!MESSAGE Warning: cannot find "knot.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "knot - Win32 Release" || "$(CFG)" == "knot - Win32 Debug" || "$(CFG)" == "knot - Win32 Hybrid"
SOURCE=.\knot.cpp

"$(INTDIR)\knot.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\knot.rc

"$(INTDIR)\knot.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\knotbase.cpp

"$(INTDIR)\knotbase.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ringwave.cpp

"$(INTDIR)\ringwave.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

