# Microsoft Developer Studio Generated NMAKE File, Based on ApplyVC.dsp
!IF "$(CFG)" == ""
CFG=ApplyVC - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to ApplyVC - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "ApplyVC - Win32 Release" && "$(CFG)" != "ApplyVC - Win32 Debug" && "$(CFG)" != "ApplyVC - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ApplyVC.mak" CFG="ApplyVC - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ApplyVC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ApplyVC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ApplyVC - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ApplyVC - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"


CLEAN :
	-@erase "$(INTDIR)\ApplyVC.obj"
	-@erase "$(INTDIR)\ApplyVC.res"
	-@erase "$(INTDIR)\AVCMod.obj"
	-@erase "$(INTDIR)\AVCUtil.obj"
	-@erase "$(INTDIR)\EvalCol.obj"
	-@erase "$(OUTDIR)\ApplyVC.exp"
	-@erase "$(OUTDIR)\ApplyVC.lib"
	-@erase "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"
	-@erase ".\ApplyVC.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ApplyVC.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ApplyVC.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ApplyVC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x04E20000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ApplyVC.pdb" /machine:I386 /def:".\ApplyVC.def" /out:"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" /implib:"$(OUTDIR)\ApplyVC.lib" /release 
DEF_FILE= \
	".\ApplyVC.def"
LINK32_OBJS= \
	"$(INTDIR)\ApplyVC.obj" \
	"$(INTDIR)\AVCMod.obj" \
	"$(INTDIR)\AVCUtil.obj" \
	"$(INTDIR)\EvalCol.obj" \
	"$(INTDIR)\ApplyVC.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ApplyVC - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"


CLEAN :
	-@erase "$(INTDIR)\ApplyVC.obj"
	-@erase "$(INTDIR)\ApplyVC.res"
	-@erase "$(INTDIR)\AVCMod.obj"
	-@erase "$(INTDIR)\AVCUtil.obj"
	-@erase "$(INTDIR)\EvalCol.obj"
	-@erase "$(OUTDIR)\ApplyVC.exp"
	-@erase "$(OUTDIR)\ApplyVC.lib"
	-@erase "$(OUTDIR)\ApplyVC.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\ApplyVC.ilk"
	-@erase ".\ApplyVC.idb"
	-@erase ".\ApplyVC.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ApplyVC.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ApplyVC.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ApplyVC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x04E20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ApplyVC.pdb" /debug /machine:I386 /def:".\ApplyVC.def" /out:"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" /implib:"$(OUTDIR)\ApplyVC.lib" /pdbtype:sept 
DEF_FILE= \
	".\ApplyVC.def"
LINK32_OBJS= \
	"$(INTDIR)\ApplyVC.obj" \
	"$(INTDIR)\AVCMod.obj" \
	"$(INTDIR)\AVCUtil.obj" \
	"$(INTDIR)\EvalCol.obj" \
	"$(INTDIR)\ApplyVC.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ApplyVC - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"


CLEAN :
	-@erase "$(INTDIR)\ApplyVC.obj"
	-@erase "$(INTDIR)\ApplyVC.res"
	-@erase "$(INTDIR)\AVCMod.obj"
	-@erase "$(INTDIR)\AVCUtil.obj"
	-@erase "$(INTDIR)\EvalCol.obj"
	-@erase "$(OUTDIR)\ApplyVC.exp"
	-@erase "$(OUTDIR)\ApplyVC.lib"
	-@erase "$(OUTDIR)\ApplyVC.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\ApplyVC.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\ApplyVC.ilk"
	-@erase ".\ApplyVC.idb"
	-@erase ".\ApplyVC.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ApplyVC.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ApplyVC.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ApplyVC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x04E20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ApplyVC.pdb" /debug /machine:I386 /def:".\ApplyVC.def" /out:"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" /implib:"$(OUTDIR)\ApplyVC.lib" /pdbtype:sept 
DEF_FILE= \
	".\ApplyVC.def"
LINK32_OBJS= \
	"$(INTDIR)\ApplyVC.obj" \
	"$(INTDIR)\AVCMod.obj" \
	"$(INTDIR)\AVCUtil.obj" \
	"$(INTDIR)\EvalCol.obj" \
	"$(INTDIR)\ApplyVC.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\ApplyVC.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ApplyVC.dep")
!INCLUDE "ApplyVC.dep"
!ELSE 
!MESSAGE Warning: cannot find "ApplyVC.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ApplyVC - Win32 Release" || "$(CFG)" == "ApplyVC - Win32 Debug" || "$(CFG)" == "ApplyVC - Win32 Hybrid"
SOURCE=.\ApplyVC.cpp

"$(INTDIR)\ApplyVC.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AVCMod.cpp

"$(INTDIR)\AVCMod.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AVCUtil.cpp

"$(INTDIR)\AVCUtil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EvalCol.cpp

"$(INTDIR)\EvalCol.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ApplyVC.rc

"$(INTDIR)\ApplyVC.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

