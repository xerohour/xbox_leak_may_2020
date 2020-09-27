# Microsoft Developer Studio Generated NMAKE File, Based on Negative.dsp
!IF "$(CFG)" == ""
CFG=negative - Win32 Release
!MESSAGE No configuration specified. Defaulting to negative - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "negative - Win32 Release" && "$(CFG)" != "negative - Win32 Debug" && "$(CFG)" != "negative - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Negative.mak" CFG="negative - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "negative - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "negative - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "negative - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "negative - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\negative.flt"


CLEAN :
	-@erase "$(INTDIR)\negative.obj"
	-@erase "$(INTDIR)\negative.res"
	-@erase "$(OUTDIR)\negative.exp"
	-@erase "$(OUTDIR)\negative.lib"
	-@erase "..\..\..\..\maxsdk\plugin\negative.flt"
	-@erase ".\negative.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Negative.pch" /YX /Fo"$(INTDIR)\\" /Fd"negative.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\negative.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Negative.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x08460000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\negative.pdb" /machine:I386 /def:".\negative.def" /out:"..\..\..\..\maxsdk\plugin\negative.flt" /implib:"$(OUTDIR)\negative.lib" /release 
DEF_FILE= \
	".\negative.def"
LINK32_OBJS= \
	"$(INTDIR)\negative.obj" \
	"$(INTDIR)\negative.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\flt.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\negative.flt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "negative - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\negative.flt"


CLEAN :
	-@erase "$(INTDIR)\negative.obj"
	-@erase "$(INTDIR)\negative.res"
	-@erase "$(OUTDIR)\negative.exp"
	-@erase "$(OUTDIR)\negative.lib"
	-@erase "$(OUTDIR)\negative.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\negative.flt"
	-@erase "..\..\..\..\maxsdk\plugin\negative.ilk"
	-@erase ".\negative.idb"
	-@erase ".\negative.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Negative.pch" /YX /Fo"$(INTDIR)\\" /Fd"negative.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\negative.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Negative.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x08460000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\negative.pdb" /debug /machine:I386 /def:".\negative.def" /out:"..\..\..\..\maxsdk\plugin\negative.flt" /implib:"$(OUTDIR)\negative.lib" 
DEF_FILE= \
	".\negative.def"
LINK32_OBJS= \
	"$(INTDIR)\negative.obj" \
	"$(INTDIR)\negative.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\flt.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\negative.flt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "negative - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\negative.flt"


CLEAN :
	-@erase "$(INTDIR)\negative.obj"
	-@erase "$(INTDIR)\negative.res"
	-@erase "$(OUTDIR)\negative.exp"
	-@erase "$(OUTDIR)\negative.lib"
	-@erase "$(OUTDIR)\negative.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\negative.flt"
	-@erase "..\..\..\..\maxsdk\plugin\negative.ilk"
	-@erase ".\negative.idb"
	-@erase ".\negative.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Negative.pch" /YX /Fo"$(INTDIR)\\" /Fd"negative.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\negative.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Negative.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x08460000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\negative.pdb" /debug /machine:I386 /def:".\negative.def" /out:"..\..\..\..\maxsdk\plugin\negative.flt" /implib:"$(OUTDIR)\negative.lib" 
DEF_FILE= \
	".\negative.def"
LINK32_OBJS= \
	"$(INTDIR)\negative.obj" \
	"$(INTDIR)\negative.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\flt.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\negative.flt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Negative.dep")
!INCLUDE "Negative.dep"
!ELSE 
!MESSAGE Warning: cannot find "Negative.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "negative - Win32 Release" || "$(CFG)" == "negative - Win32 Debug" || "$(CFG)" == "negative - Win32 Hybrid"
SOURCE=.\negative.cpp

"$(INTDIR)\negative.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\negative.rc

"$(INTDIR)\negative.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

