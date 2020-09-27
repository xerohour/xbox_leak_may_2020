# Microsoft Developer Studio Generated NMAKE File, Based on wavectrl.dsp
!IF "$(CFG)" == ""
CFG=wavectrl - Win32 Release
!MESSAGE No configuration specified. Defaulting to wavectrl - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "wavectrl - Win32 Release" && "$(CFG)" != "wavectrl - Win32 Debug" && "$(CFG)" != "wavectrl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wavectrl.mak" CFG="wavectrl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wavectrl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wavectrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wavectrl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "wavectrl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\wavectrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\simpwave.obj"
	-@erase "$(INTDIR)\wavectrl.obj"
	-@erase "$(INTDIR)\wavectrl.res"
	-@erase "$(OUTDIR)\wavectrl.exp"
	-@erase "$(OUTDIR)\wavectrl.lib"
	-@erase "..\..\..\maxsdk\plugin\wavectrl.dlc"
	-@erase ".\wavectrl.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\wavectrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"wavectrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wavectrl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wavectrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05E80000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\wavectrl.pdb" /machine:I386 /def:".\wavectrl.def" /out:"..\..\..\maxsdk\plugin\wavectrl.dlc" /implib:"$(OUTDIR)\wavectrl.lib" /release 
DEF_FILE= \
	".\wavectrl.def"
LINK32_OBJS= \
	"$(INTDIR)\simpwave.obj" \
	"$(INTDIR)\wavectrl.obj" \
	"$(INTDIR)\wavectrl.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\expr.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\wavectrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wavectrl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\wavectrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\simpwave.obj"
	-@erase "$(INTDIR)\wavectrl.obj"
	-@erase "$(INTDIR)\wavectrl.res"
	-@erase "$(OUTDIR)\wavectrl.exp"
	-@erase "$(OUTDIR)\wavectrl.lib"
	-@erase "$(OUTDIR)\wavectrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\wavectrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\wavectrl.ilk"
	-@erase ".\wavectrl.idb"
	-@erase ".\wavectrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\wavectrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"wavectrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wavectrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wavectrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05E80000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\wavectrl.pdb" /debug /machine:I386 /def:".\wavectrl.def" /out:"..\..\..\maxsdk\plugin\wavectrl.dlc" /implib:"$(OUTDIR)\wavectrl.lib" 
DEF_FILE= \
	".\wavectrl.def"
LINK32_OBJS= \
	"$(INTDIR)\simpwave.obj" \
	"$(INTDIR)\wavectrl.obj" \
	"$(INTDIR)\wavectrl.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\expr.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\wavectrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wavectrl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\wavectrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\simpwave.obj"
	-@erase "$(INTDIR)\wavectrl.obj"
	-@erase "$(INTDIR)\wavectrl.res"
	-@erase "$(OUTDIR)\wavectrl.exp"
	-@erase "$(OUTDIR)\wavectrl.lib"
	-@erase "$(OUTDIR)\wavectrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\wavectrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\wavectrl.ilk"
	-@erase ".\wavectrl.idb"
	-@erase ".\wavectrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\wavectrl.pch" /YX /Fo"$(INTDIR)\\" /Fd"wavectrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wavectrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wavectrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05E80000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\wavectrl.pdb" /debug /machine:I386 /def:".\wavectrl.def" /out:"..\..\..\maxsdk\plugin\wavectrl.dlc" /implib:"$(OUTDIR)\wavectrl.lib" 
DEF_FILE= \
	".\wavectrl.def"
LINK32_OBJS= \
	"$(INTDIR)\simpwave.obj" \
	"$(INTDIR)\wavectrl.obj" \
	"$(INTDIR)\wavectrl.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\expr.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\wavectrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("wavectrl.dep")
!INCLUDE "wavectrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "wavectrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wavectrl - Win32 Release" || "$(CFG)" == "wavectrl - Win32 Debug" || "$(CFG)" == "wavectrl - Win32 Hybrid"
SOURCE=.\simpwave.cpp

"$(INTDIR)\simpwave.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wavectrl.cpp

"$(INTDIR)\wavectrl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wavectrl.rc

"$(INTDIR)\wavectrl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

