# Microsoft Developer Studio Generated NMAKE File, Based on Custctrl.dsp
!IF "$(CFG)" == ""
CFG=CUSTCTRL - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CUSTCTRL - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CUSTCTRL - Win32 Release" && "$(CFG)" != "CUSTCTRL - Win32 Hybrid" && "$(CFG)" != "CUSTCTRL - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Custctrl.mak" CFG="CUSTCTRL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CUSTCTRL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUSTCTRL - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUSTCTRL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CUSTCTRL - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\custctrl.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUSTCTRL.OBJ"
	-@erase "$(INTDIR)\CUSTCTRL.res"
	-@erase "$(OUTDIR)\custctrl.exp"
	-@erase "$(OUTDIR)\custctrl.lib"
	-@erase "..\..\..\plugin\custctrl.dlu"
	-@erase ".\CUSTCTRL.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Custctrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"CUSTCTRL.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUSTCTRL.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Custctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\custctrl.pdb" /machine:I386 /def:".\CUSTCTRL.DEF" /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu" /implib:"$(OUTDIR)\custctrl.lib" /release 
DEF_FILE= \
	".\CUSTCTRL.DEF"
LINK32_OBJS= \
	"$(INTDIR)\CUSTCTRL.OBJ" \
	"$(INTDIR)\CUSTCTRL.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\custctrl.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CUSTCTRL - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\custctrl.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUSTCTRL.OBJ"
	-@erase "$(INTDIR)\CUSTCTRL.res"
	-@erase "$(OUTDIR)\custctrl.exp"
	-@erase "$(OUTDIR)\custctrl.lib"
	-@erase "$(OUTDIR)\custctrl.pdb"
	-@erase "..\..\..\plugin\custctrl.dlu"
	-@erase "..\..\..\plugin\custctrl.ilk"
	-@erase ".\CUSTCTRL.idb"
	-@erase ".\CUSTCTRL.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Custctrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"CUSTCTRL.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUSTCTRL.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Custctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\custctrl.pdb" /debug /machine:I386 /def:".\CUSTCTRL.DEF" /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu" /implib:"$(OUTDIR)\custctrl.lib" 
DEF_FILE= \
	".\CUSTCTRL.DEF"
LINK32_OBJS= \
	"$(INTDIR)\CUSTCTRL.OBJ" \
	"$(INTDIR)\CUSTCTRL.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\custctrl.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CUSTCTRL - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\custctrl.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUSTCTRL.OBJ"
	-@erase "$(INTDIR)\CUSTCTRL.res"
	-@erase "$(OUTDIR)\custctrl.exp"
	-@erase "$(OUTDIR)\custctrl.lib"
	-@erase "$(OUTDIR)\custctrl.pdb"
	-@erase "..\..\..\plugin\custctrl.dlu"
	-@erase "..\..\..\plugin\custctrl.ilk"
	-@erase ".\CUSTCTRL.idb"
	-@erase ".\CUSTCTRL.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Custctrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"CUSTCTRL.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUSTCTRL.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Custctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\custctrl.pdb" /debug /machine:I386 /def:".\CUSTCTRL.DEF" /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu" /implib:"$(OUTDIR)\custctrl.lib" 
DEF_FILE= \
	".\CUSTCTRL.DEF"
LINK32_OBJS= \
	"$(INTDIR)\CUSTCTRL.OBJ" \
	"$(INTDIR)\CUSTCTRL.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\custctrl.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Custctrl.dep")
!INCLUDE "Custctrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "Custctrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CUSTCTRL - Win32 Release" || "$(CFG)" == "CUSTCTRL - Win32 Hybrid" || "$(CFG)" == "CUSTCTRL - Win32 Debug"
SOURCE=.\CUSTCTRL.CPP

"$(INTDIR)\CUSTCTRL.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CUSTCTRL.RC

"$(INTDIR)\CUSTCTRL.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

