# Microsoft Developer Studio Generated NMAKE File, Based on lag.dsp
!IF "$(CFG)" == ""
CFG=Lag - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Lag - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Lag - Win32 Release" && "$(CFG)" != "Lag - Win32 Hybrid" && "$(CFG)" != "Lag - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lag.mak" CFG="Lag - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lag - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Lag - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Lag - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Lag - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\lag.dlm"


CLEAN :
	-@erase "$(INTDIR)\Lag.obj"
	-@erase "$(INTDIR)\lag.pch"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(OUTDIR)\lag.exp"
	-@erase "$(OUTDIR)\lag.lib"
	-@erase "..\..\..\..\maxsdk\plugin\lag.dlm"
	-@erase ".\Lag.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lag.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x057C0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\lag.pdb" /machine:I386 /def:".\lag.def" /out:"..\..\..\..\maxsdk\plugin\lag.dlm" /implib:"$(OUTDIR)\lag.lib" /release 
DEF_FILE= \
	".\lag.def"
LINK32_OBJS= \
	"$(INTDIR)\Lag.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\lag.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Lag - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\lag.dlm"


CLEAN :
	-@erase "$(INTDIR)\Lag.obj"
	-@erase "$(INTDIR)\lag.pch"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(OUTDIR)\lag.exp"
	-@erase "$(OUTDIR)\lag.lib"
	-@erase "$(OUTDIR)\lag.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\lag.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\lag.ilk"
	-@erase ".\Lag.idb"
	-@erase ".\Lag.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lag.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x057C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\lag.pdb" /debug /machine:I386 /def:".\lag.def" /out:"..\..\..\..\maxsdk\plugin\lag.dlm" /implib:"$(OUTDIR)\lag.lib" 
DEF_FILE= \
	".\lag.def"
LINK32_OBJS= \
	"$(INTDIR)\Lag.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\lag.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Lag - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\lag.dlm"


CLEAN :
	-@erase "$(INTDIR)\Lag.obj"
	-@erase "$(INTDIR)\lag.pch"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(OUTDIR)\lag.exp"
	-@erase "$(OUTDIR)\lag.lib"
	-@erase "$(OUTDIR)\lag.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\lag.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\lag.ilk"
	-@erase ".\Lag.idb"
	-@erase ".\Lag.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lag.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x057C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\lag.pdb" /debug /machine:I386 /def:".\lag.def" /out:"..\..\..\..\maxsdk\plugin\lag.dlm" /implib:"$(OUTDIR)\lag.lib" 
DEF_FILE= \
	".\lag.def"
LINK32_OBJS= \
	"$(INTDIR)\Lag.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\lag.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("lag.dep")
!INCLUDE "lag.dep"
!ELSE 
!MESSAGE Warning: cannot find "lag.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Lag - Win32 Release" || "$(CFG)" == "Lag - Win32 Hybrid" || "$(CFG)" == "Lag - Win32 Debug"
SOURCE=.\Lag.CPP

"$(INTDIR)\Lag.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\lag.pch"


SOURCE=.\mods.cpp

!IF  "$(CFG)" == "Lag - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\lag.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Lag - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\lag.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Lag - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\lag.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"Lag.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\lag.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mods.rc

"$(INTDIR)\mods.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

