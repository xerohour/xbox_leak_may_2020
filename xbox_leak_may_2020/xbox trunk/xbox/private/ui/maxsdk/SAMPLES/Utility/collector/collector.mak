# Microsoft Developer Studio Generated NMAKE File, Based on collector.dsp
!IF "$(CFG)" == ""
CFG=Collector - Win32 Release
!MESSAGE No configuration specified. Defaulting to Collector - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Collector - Win32 Release" && "$(CFG)" != "Collector - Win32 Debug" && "$(CFG)" != "Collector - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "collector.mak" CFG="Collector - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Collector - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Collector - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Collector - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Collector - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\Collector.dlu"


CLEAN :
	-@erase "$(INTDIR)\Collector.obj"
	-@erase "$(INTDIR)\collector.res"
	-@erase "$(OUTDIR)\Collector.exp"
	-@erase "$(OUTDIR)\Collector.lib"
	-@erase "..\..\..\..\maxsdk\plugin\Collector.dlu"
	-@erase ".\Collector.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\collector.pch" /YX"Collector.h" /Fo"$(INTDIR)\\" /Fd"Collector.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\collector.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\collector.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08550000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Collector.pdb" /machine:I386 /def:".\Collector.def" /out:"..\..\..\..\maxsdk\plugin\Collector.dlu" /implib:"$(OUTDIR)\Collector.lib" /release 
DEF_FILE= \
	".\Collector.def"
LINK32_OBJS= \
	"$(INTDIR)\Collector.obj" \
	"$(INTDIR)\collector.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\Collector.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Collector - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\Collector.dlu"


CLEAN :
	-@erase "$(INTDIR)\Collector.obj"
	-@erase "$(INTDIR)\collector.res"
	-@erase "$(OUTDIR)\Collector.exp"
	-@erase "$(OUTDIR)\Collector.lib"
	-@erase "$(OUTDIR)\Collector.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Collector.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\Collector.ilk"
	-@erase ".\Collector.idb"
	-@erase ".\Collector.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\collector.pch" /YX"Collector.h" /Fo"$(INTDIR)\\" /Fd"Collector.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\collector.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\collector.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08550000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Collector.pdb" /debug /machine:I386 /def:".\Collector.def" /out:"..\..\..\..\maxsdk\plugin\Collector.dlu" /implib:"$(OUTDIR)\Collector.lib" 
DEF_FILE= \
	".\Collector.def"
LINK32_OBJS= \
	"$(INTDIR)\Collector.obj" \
	"$(INTDIR)\collector.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\Collector.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Collector - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\Collector.dlu"


CLEAN :
	-@erase "$(INTDIR)\Collector.obj"
	-@erase "$(INTDIR)\collector.res"
	-@erase "$(OUTDIR)\Collector.exp"
	-@erase "$(OUTDIR)\Collector.lib"
	-@erase "$(OUTDIR)\Collector.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Collector.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\Collector.ilk"
	-@erase ".\Collector.idb"
	-@erase ".\Collector.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\collector.pch" /YX"Collector.h" /Fo"$(INTDIR)\\" /Fd"Collector.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\collector.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\collector.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x08550000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Collector.pdb" /debug /machine:I386 /def:".\Collector.def" /out:"..\..\..\..\maxsdk\plugin\Collector.dlu" /implib:"$(OUTDIR)\Collector.lib" 
DEF_FILE= \
	".\Collector.def"
LINK32_OBJS= \
	"$(INTDIR)\Collector.obj" \
	"$(INTDIR)\collector.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\Collector.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("collector.dep")
!INCLUDE "collector.dep"
!ELSE 
!MESSAGE Warning: cannot find "collector.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Collector - Win32 Release" || "$(CFG)" == "Collector - Win32 Debug" || "$(CFG)" == "Collector - Win32 Hybrid"
SOURCE=.\Collector.cpp

"$(INTDIR)\Collector.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\collector.rc

"$(INTDIR)\collector.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

