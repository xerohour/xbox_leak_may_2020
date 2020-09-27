# Microsoft Developer Studio Generated NMAKE File, Based on MaxFind.dsp
!IF "$(CFG)" == ""
CFG=MaxFind - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to MaxFind - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "MaxFind - Win32 Release" && "$(CFG)" != "MaxFind - Win32 Debug" && "$(CFG)" != "MaxFind - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxFind.mak" CFG="MaxFind - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxFind - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MaxFind - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MaxFind - Win32 Hybrid" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MaxFind - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\MaxFind.exe"


CLEAN :
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\gtypes.obj"
	-@erase "$(INTDIR)\MaxFind.obj"
	-@erase "$(INTDIR)\MaxFind.res"
	-@erase "$(INTDIR)\ShowPropDlg.obj"
	-@erase "$(INTDIR)\vtrackbl.obj"
	-@erase "..\..\..\maxsdk\plugin\MaxFind.exe"
	-@erase ".\MaxFind.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MaxFind.pch" /YX"pch.h" /Fo"$(INTDIR)\\" /Fd"MaxFind.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxFind.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxFind.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\MaxFind.pdb" /machine:I386 /out:"..\..\..\maxsdk\plugin\MaxFind.exe" /release 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\MaxFind.obj" \
	"$(INTDIR)\ShowPropDlg.obj" \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\gtypes.obj" \
	"$(INTDIR)\vtrackbl.obj" \
	"$(INTDIR)\MaxFind.res"

"..\..\..\maxsdk\plugin\MaxFind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MaxFind - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\MaxFind.exe"


CLEAN :
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\gtypes.obj"
	-@erase "$(INTDIR)\MaxFind.obj"
	-@erase "$(INTDIR)\MaxFind.res"
	-@erase "$(INTDIR)\ShowPropDlg.obj"
	-@erase "$(INTDIR)\vtrackbl.obj"
	-@erase "$(OUTDIR)\MaxFind.pdb"
	-@erase "..\..\..\maxsdk\plugin\MaxFind.exe"
	-@erase "..\..\..\maxsdk\plugin\MaxFind.ilk"
	-@erase ".\MaxFind.idb"
	-@erase ".\MaxFind.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MaxFind.pch" /YX"pch.h" /Fo"$(INTDIR)\\" /Fd"MaxFind.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxFind.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxFind.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\MaxFind.pdb" /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\MaxFind.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\MaxFind.obj" \
	"$(INTDIR)\ShowPropDlg.obj" \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\gtypes.obj" \
	"$(INTDIR)\vtrackbl.obj" \
	"$(INTDIR)\MaxFind.res"

"..\..\..\maxsdk\plugin\MaxFind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MaxFind - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\MaxFind.exe"


CLEAN :
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\gtypes.obj"
	-@erase "$(INTDIR)\MaxFind.obj"
	-@erase "$(INTDIR)\MaxFind.res"
	-@erase "$(INTDIR)\ShowPropDlg.obj"
	-@erase "$(INTDIR)\vtrackbl.obj"
	-@erase "$(OUTDIR)\MaxFind.pdb"
	-@erase "..\..\..\maxsdk\plugin\MaxFind.exe"
	-@erase "..\..\..\maxsdk\plugin\MaxFind.ilk"
	-@erase ".\MaxFind.idb"
	-@erase ".\MaxFind.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MaxFind.pch" /YX"pch.h" /Fo"$(INTDIR)\\" /Fd"MaxFind.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxFind.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxFind.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\MaxFind.pdb" /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\MaxFind.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\MaxFind.obj" \
	"$(INTDIR)\ShowPropDlg.obj" \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\gtypes.obj" \
	"$(INTDIR)\vtrackbl.obj" \
	"$(INTDIR)\MaxFind.res"

"..\..\..\maxsdk\plugin\MaxFind.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MaxFind.dep")
!INCLUDE "MaxFind.dep"
!ELSE 
!MESSAGE Warning: cannot find "MaxFind.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MaxFind - Win32 Release" || "$(CFG)" == "MaxFind - Win32 Debug" || "$(CFG)" == "MaxFind - Win32 Hybrid"
SOURCE=.\app.cpp

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MaxFind.cpp

"$(INTDIR)\MaxFind.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShowPropDlg.cpp

"$(INTDIR)\ShowPropDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MaxFind.rc

"$(INTDIR)\MaxFind.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\about.cpp

"$(INTDIR)\about.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gtypes.cpp

"$(INTDIR)\gtypes.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vtrackbl.cpp

"$(INTDIR)\vtrackbl.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

