# Microsoft Developer Studio Generated NMAKE File, Based on Rla.dsp
!IF "$(CFG)" == ""
CFG=rla - Win32 Release
!MESSAGE No configuration specified. Defaulting to rla - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "rla - Win32 Release" && "$(CFG)" != "rla - Win32 Debug" && "$(CFG)" != "rla - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rla.mak" CFG="rla - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rla - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rla - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rla - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "rla - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\rla.bmi"


CLEAN :
	-@erase "$(INTDIR)\rla.obj"
	-@erase "$(INTDIR)\rla.res"
	-@erase "$(OUTDIR)\rla.exp"
	-@erase "$(OUTDIR)\rla.lib"
	-@erase "..\..\..\..\maxsdk\plugin\rla.bmi"
	-@erase ".\rla.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"rla.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rla.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rla.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10360000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\rla.pdb" /machine:I386 /def:".\rla.def" /out:"..\..\..\..\maxsdk\plugin\rla.bmi" /implib:"$(OUTDIR)\rla.lib" /release 
DEF_FILE= \
	".\rla.def"
LINK32_OBJS= \
	"$(INTDIR)\rla.obj" \
	"$(INTDIR)\rla.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\rla.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rla - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\rla.bmi"


CLEAN :
	-@erase "$(INTDIR)\rla.obj"
	-@erase "$(INTDIR)\rla.res"
	-@erase "$(OUTDIR)\rla.exp"
	-@erase "$(OUTDIR)\rla.lib"
	-@erase "$(OUTDIR)\rla.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\rla.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\rla.ilk"
	-@erase ".\rla.idb"
	-@erase ".\rla.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"rla.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rla.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rla.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10360000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\rla.pdb" /debug /machine:I386 /def:".\rla.def" /out:"..\..\..\..\maxsdk\plugin\rla.bmi" /implib:"$(OUTDIR)\rla.lib" 
DEF_FILE= \
	".\rla.def"
LINK32_OBJS= \
	"$(INTDIR)\rla.obj" \
	"$(INTDIR)\rla.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\rla.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rla - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\rla.bmi"


CLEAN :
	-@erase "$(INTDIR)\rla.obj"
	-@erase "$(INTDIR)\rla.res"
	-@erase "$(OUTDIR)\rla.exp"
	-@erase "$(OUTDIR)\rla.lib"
	-@erase "$(OUTDIR)\rla.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\rla.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\rla.ilk"
	-@erase ".\rla.idb"
	-@erase ".\rla.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"rla.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rla.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rla.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10360000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\rla.pdb" /debug /machine:I386 /def:".\rla.def" /out:"..\..\..\..\maxsdk\plugin\rla.bmi" /implib:"$(OUTDIR)\rla.lib" 
DEF_FILE= \
	".\rla.def"
LINK32_OBJS= \
	"$(INTDIR)\rla.obj" \
	"$(INTDIR)\rla.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\rla.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Rla.dep")
!INCLUDE "Rla.dep"
!ELSE 
!MESSAGE Warning: cannot find "Rla.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "rla - Win32 Release" || "$(CFG)" == "rla - Win32 Debug" || "$(CFG)" == "rla - Win32 Hybrid"
SOURCE=.\rla.cpp

"$(INTDIR)\rla.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rla.rc

"$(INTDIR)\rla.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

