# Microsoft Developer Studio Generated NMAKE File, Based on procmap.dsp
!IF "$(CFG)" == ""
CFG=procmap - Win32 Release
!MESSAGE No configuration specified. Defaulting to procmap - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "procmap - Win32 Release" && "$(CFG)" != "procmap - Win32 Debug" && "$(CFG)" != "procmap - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "procmap.mak" CFG="procmap - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "procmap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "procmap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "procmap - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "procmap - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\procmap.dlt"


CLEAN :
	-@erase "$(INTDIR)\PLANET.OBJ"
	-@erase "$(INTDIR)\procmap.obj"
	-@erase "$(INTDIR)\procmap.pch"
	-@erase "$(INTDIR)\procmaps.res"
	-@erase "$(INTDIR)\SMOKE.OBJ"
	-@erase "$(INTDIR)\SPECKLE.OBJ"
	-@erase "$(INTDIR)\SPLAT.OBJ"
	-@erase "$(INTDIR)\STUCCO.OBJ"
	-@erase "$(INTDIR)\WATER.OBJ"
	-@erase "$(OUTDIR)\procmap.exp"
	-@erase "$(OUTDIR)\procmap.lib"
	-@erase "..\..\..\maxsdk\plugin\procmap.dlt"
	-@erase ".\procmap.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yu"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\procmaps.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\procmap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02BB0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\procmap.pdb" /machine:I386 /def:".\procmaps.def" /out:"..\..\..\maxsdk\plugin\procmap.dlt" /implib:"$(OUTDIR)\procmap.lib" /release 
DEF_FILE= \
	".\procmaps.def"
LINK32_OBJS= \
	"$(INTDIR)\PLANET.OBJ" \
	"$(INTDIR)\procmap.obj" \
	"$(INTDIR)\SMOKE.OBJ" \
	"$(INTDIR)\SPECKLE.OBJ" \
	"$(INTDIR)\SPLAT.OBJ" \
	"$(INTDIR)\STUCCO.OBJ" \
	"$(INTDIR)\WATER.OBJ" \
	"$(INTDIR)\procmaps.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\procmap.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "procmap - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\procmap.dlt"


CLEAN :
	-@erase "$(INTDIR)\PLANET.OBJ"
	-@erase "$(INTDIR)\procmap.obj"
	-@erase "$(INTDIR)\procmap.pch"
	-@erase "$(INTDIR)\procmaps.res"
	-@erase "$(INTDIR)\SMOKE.OBJ"
	-@erase "$(INTDIR)\SPECKLE.OBJ"
	-@erase "$(INTDIR)\SPLAT.OBJ"
	-@erase "$(INTDIR)\STUCCO.OBJ"
	-@erase "$(INTDIR)\WATER.OBJ"
	-@erase "$(OUTDIR)\procmap.exp"
	-@erase "$(OUTDIR)\procmap.lib"
	-@erase "$(OUTDIR)\procmap.pdb"
	-@erase "..\..\..\maxsdk\plugin\procmap.dlt"
	-@erase "..\..\..\maxsdk\plugin\procmap.ilk"
	-@erase ".\procmap.idb"
	-@erase ".\procmap.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yu"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\procmaps.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\procmap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02BB0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\procmap.pdb" /debug /machine:I386 /def:".\procmaps.def" /out:"..\..\..\maxsdk\plugin\procmap.dlt" /implib:"$(OUTDIR)\procmap.lib" 
DEF_FILE= \
	".\procmaps.def"
LINK32_OBJS= \
	"$(INTDIR)\PLANET.OBJ" \
	"$(INTDIR)\procmap.obj" \
	"$(INTDIR)\SMOKE.OBJ" \
	"$(INTDIR)\SPECKLE.OBJ" \
	"$(INTDIR)\SPLAT.OBJ" \
	"$(INTDIR)\STUCCO.OBJ" \
	"$(INTDIR)\WATER.OBJ" \
	"$(INTDIR)\procmaps.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\procmap.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "procmap - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\procmap.dlt"


CLEAN :
	-@erase "$(INTDIR)\PLANET.OBJ"
	-@erase "$(INTDIR)\procmap.obj"
	-@erase "$(INTDIR)\procmap.pch"
	-@erase "$(INTDIR)\procmaps.res"
	-@erase "$(INTDIR)\SMOKE.OBJ"
	-@erase "$(INTDIR)\SPECKLE.OBJ"
	-@erase "$(INTDIR)\SPLAT.OBJ"
	-@erase "$(INTDIR)\STUCCO.OBJ"
	-@erase "$(INTDIR)\WATER.OBJ"
	-@erase "$(OUTDIR)\procmap.exp"
	-@erase "$(OUTDIR)\procmap.lib"
	-@erase "$(OUTDIR)\procmap.pdb"
	-@erase "..\..\..\maxsdk\plugin\procmap.dlt"
	-@erase "..\..\..\maxsdk\plugin\procmap.ilk"
	-@erase ".\procmap.idb"
	-@erase ".\procmap.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yu"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\procmaps.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\procmap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02BB0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\procmap.pdb" /debug /machine:I386 /def:".\procmaps.def" /out:"..\..\..\maxsdk\plugin\procmap.dlt" /implib:"$(OUTDIR)\procmap.lib" 
DEF_FILE= \
	".\procmaps.def"
LINK32_OBJS= \
	"$(INTDIR)\PLANET.OBJ" \
	"$(INTDIR)\procmap.obj" \
	"$(INTDIR)\SMOKE.OBJ" \
	"$(INTDIR)\SPECKLE.OBJ" \
	"$(INTDIR)\SPLAT.OBJ" \
	"$(INTDIR)\STUCCO.OBJ" \
	"$(INTDIR)\WATER.OBJ" \
	"$(INTDIR)\procmaps.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\procmap.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("procmap.dep")
!INCLUDE "procmap.dep"
!ELSE 
!MESSAGE Warning: cannot find "procmap.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "procmap - Win32 Release" || "$(CFG)" == "procmap - Win32 Debug" || "$(CFG)" == "procmap - Win32 Hybrid"
SOURCE=.\PLANET.CPP

"$(INTDIR)\PLANET.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"


SOURCE=.\procmap.cpp

!IF  "$(CFG)" == "procmap - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yc"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

"$(INTDIR)\procmap.obj"	"$(INTDIR)\procmap.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "procmap - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yc"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

"$(INTDIR)\procmap.obj"	"$(INTDIR)\procmap.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "procmap - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\procmap.pch" /Yc"procmaps.h" /Fo"$(INTDIR)\\" /Fd"procmap.pdb" /FD /c 

"$(INTDIR)\procmap.obj"	"$(INTDIR)\procmap.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\procmaps.rc

"$(INTDIR)\procmaps.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\SMOKE.CPP

"$(INTDIR)\SMOKE.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"


SOURCE=.\SPECKLE.CPP

"$(INTDIR)\SPECKLE.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"


SOURCE=.\SPLAT.CPP

"$(INTDIR)\SPLAT.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"


SOURCE=.\STUCCO.CPP

"$(INTDIR)\STUCCO.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"


SOURCE=.\WATER.CPP

"$(INTDIR)\WATER.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\procmap.pch"



!ENDIF 

