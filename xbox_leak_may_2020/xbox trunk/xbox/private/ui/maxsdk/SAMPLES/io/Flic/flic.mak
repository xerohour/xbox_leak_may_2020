# Microsoft Developer Studio Generated NMAKE File, Based on Flic.dsp
!IF "$(CFG)" == ""
CFG=flic - Win32 Release
!MESSAGE No configuration specified. Defaulting to flic - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "flic - Win32 Release" && "$(CFG)" != "flic - Win32 Debug" && "$(CFG)" != "flic - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Flic.mak" CFG="flic - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "flic - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flic - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flic - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "flic - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\flic.bmi"


CLEAN :
	-@erase "$(INTDIR)\flic.obj"
	-@erase "$(INTDIR)\flic.res"
	-@erase "$(INTDIR)\flicpch.obj"
	-@erase "$(INTDIR)\mbox.obj"
	-@erase "$(OUTDIR)\flic.exp"
	-@erase "$(OUTDIR)\flic.lib"
	-@erase "..\..\..\..\maxsdk\plugin\flic.bmi"
	-@erase ".\flic.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Flic.pch" /YX /Fo"$(INTDIR)\\" /Fd"flic.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\flic.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Flic.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\flilibr.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10330000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\flic.pdb" /machine:I386 /def:".\flic.def" /out:"..\..\..\..\maxsdk\plugin\flic.bmi" /implib:"$(OUTDIR)\flic.lib" /release 
DEF_FILE= \
	".\flic.def"
LINK32_OBJS= \
	"$(INTDIR)\flic.obj" \
	"$(INTDIR)\flicpch.obj" \
	"$(INTDIR)\mbox.obj" \
	"$(INTDIR)\flic.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\flic.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "flic - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\flic.bmi"


CLEAN :
	-@erase "$(INTDIR)\flic.obj"
	-@erase "$(INTDIR)\flic.res"
	-@erase "$(INTDIR)\flicpch.obj"
	-@erase "$(INTDIR)\mbox.obj"
	-@erase "$(OUTDIR)\flic.exp"
	-@erase "$(OUTDIR)\flic.lib"
	-@erase "$(OUTDIR)\flic.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\flic.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\flic.ilk"
	-@erase ".\flic.idb"
	-@erase ".\flic.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Flic.pch" /YX /Fo"$(INTDIR)\\" /Fd"flic.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\flic.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Flic.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\flilibd.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10330000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\flic.pdb" /debug /machine:I386 /def:".\flic.def" /out:"..\..\..\..\maxsdk\plugin\flic.bmi" /implib:"$(OUTDIR)\flic.lib" 
DEF_FILE= \
	".\flic.def"
LINK32_OBJS= \
	"$(INTDIR)\flic.obj" \
	"$(INTDIR)\flicpch.obj" \
	"$(INTDIR)\mbox.obj" \
	"$(INTDIR)\flic.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\flic.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "flic - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\flic.bmi"


CLEAN :
	-@erase "$(INTDIR)\flic.obj"
	-@erase "$(INTDIR)\flic.res"
	-@erase "$(INTDIR)\flicpch.obj"
	-@erase "$(INTDIR)\mbox.obj"
	-@erase "$(OUTDIR)\flic.exp"
	-@erase "$(OUTDIR)\flic.lib"
	-@erase "$(OUTDIR)\flic.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\flic.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\flic.ilk"
	-@erase ".\flic.idb"
	-@erase ".\flic.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Flic.pch" /YX /Fo"$(INTDIR)\\" /Fd"flic.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\flic.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Flic.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\flilibh.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x10330000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\flic.pdb" /debug /machine:I386 /def:".\flic.def" /out:"..\..\..\..\maxsdk\plugin\flic.bmi" /implib:"$(OUTDIR)\flic.lib" 
DEF_FILE= \
	".\flic.def"
LINK32_OBJS= \
	"$(INTDIR)\flic.obj" \
	"$(INTDIR)\flicpch.obj" \
	"$(INTDIR)\mbox.obj" \
	"$(INTDIR)\flic.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\flic.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Flic.dep")
!INCLUDE "Flic.dep"
!ELSE 
!MESSAGE Warning: cannot find "Flic.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "flic - Win32 Release" || "$(CFG)" == "flic - Win32 Debug" || "$(CFG)" == "flic - Win32 Hybrid"
SOURCE=.\flic.cpp

"$(INTDIR)\flic.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\flic.rc

"$(INTDIR)\flic.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\flicpch.cpp

"$(INTDIR)\flicpch.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mbox.c

"$(INTDIR)\mbox.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

