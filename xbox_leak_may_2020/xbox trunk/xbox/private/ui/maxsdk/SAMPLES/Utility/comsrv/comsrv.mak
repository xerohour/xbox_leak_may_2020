# Microsoft Developer Studio Generated NMAKE File, Based on comsrv.dsp
!IF "$(CFG)" == ""
CFG=comsrv - Win32 Release
!MESSAGE No configuration specified. Defaulting to comsrv - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "comsrv - Win32 Release" && "$(CFG)" != "comsrv - Win32 Debug" && "$(CFG)" != "comsrv - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "comsrv.mak" CFG="comsrv - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "comsrv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "comsrv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "comsrv - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "comsrv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\comsrvui.dlu"


CLEAN :
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(OUTDIR)\comsrvui.exp"
	-@erase "$(OUTDIR)\comsrvui.lib"
	-@erase "..\..\..\..\maxsdk\plugin\comsrvui.dlu"
	-@erase ".\comsrv.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\comsrv.pch" /YX"comsrv.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10530000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\comsrvui.pdb" /machine:I386 /def:".\comsrv.def" /out:"..\..\..\..\maxsdk\plugin\comsrvui.dlu" /implib:"$(OUTDIR)\comsrvui.lib" /release 
DEF_FILE= \
	".\comsrv.def"
LINK32_OBJS= \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrvui.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "comsrv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\comsrvui.dlu"


CLEAN :
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(OUTDIR)\comsrvui.exp"
	-@erase "$(OUTDIR)\comsrvui.lib"
	-@erase "$(OUTDIR)\comsrvui.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\comsrvui.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\comsrvui.ilk"
	-@erase ".\comsrv.idb"
	-@erase ".\comsrv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\comsrv.pch" /YX"comsrv.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10530000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\comsrvui.pdb" /debug /machine:I386 /def:".\comsrv.def" /out:"..\..\..\..\maxsdk\plugin\comsrvui.dlu" /implib:"$(OUTDIR)\comsrvui.lib" 
DEF_FILE= \
	".\comsrv.def"
LINK32_OBJS= \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrvui.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "comsrv - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\comsrvui.dlu"


CLEAN :
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(OUTDIR)\comsrvui.exp"
	-@erase "$(OUTDIR)\comsrvui.lib"
	-@erase "$(OUTDIR)\comsrvui.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\comsrvui.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\comsrvui.ilk"
	-@erase ".\comsrv.idb"
	-@erase ".\comsrv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\comsrv.pch" /YX"comsrv.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10530000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\comsrvui.pdb" /debug /machine:I386 /def:".\comsrv.def" /out:"..\..\..\..\maxsdk\plugin\comsrvui.dlu" /implib:"$(OUTDIR)\comsrvui.lib" 
DEF_FILE= \
	".\comsrv.def"
LINK32_OBJS= \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrvui.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("comsrv.dep")
!INCLUDE "comsrv.dep"
!ELSE 
!MESSAGE Warning: cannot find "comsrv.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "comsrv - Win32 Release" || "$(CFG)" == "comsrv - Win32 Debug" || "$(CFG)" == "comsrv - Win32 Hybrid"
SOURCE=.\comsrv.cpp

"$(INTDIR)\comsrv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\comsrv.rc

"$(INTDIR)\comsrv.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

