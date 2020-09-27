# Microsoft Developer Studio Generated NMAKE File, Based on Dxfimp.dsp
!IF "$(CFG)" == ""
CFG=dxfimp - Win32 Release
!MESSAGE No configuration specified. Defaulting to dxfimp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dxfimp - Win32 Release" && "$(CFG)" != "dxfimp - Win32 Debug" && "$(CFG)" != "dxfimp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dxfimp.mak" CFG="dxfimp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dxfimp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dxfimp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dxfimp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dxfimp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\dxfimp.dli"


CLEAN :
	-@erase "$(INTDIR)\arbaxis.obj"
	-@erase "$(INTDIR)\dxfimp.obj"
	-@erase "$(INTDIR)\dxfimp.res"
	-@erase "$(OUTDIR)\dxfimp.exp"
	-@erase "$(OUTDIR)\dxfimp.lib"
	-@erase "..\..\..\maxsdk\plugin\dxfimp.dli"
	-@erase ".\dxfimp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfimp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05FA0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dxfimp.pdb" /machine:I386 /def:".\dxfimp.def" /out:"..\..\..\maxsdk\plugin\dxfimp.dli" /implib:"$(OUTDIR)\dxfimp.lib" /release 
DEF_FILE= \
	".\dxfimp.def"
LINK32_OBJS= \
	"$(INTDIR)\arbaxis.obj" \
	"$(INTDIR)\dxfimp.obj" \
	"$(INTDIR)\dxfimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dxfimp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\dxfimp.dli"


CLEAN :
	-@erase "$(INTDIR)\arbaxis.obj"
	-@erase "$(INTDIR)\dxfimp.obj"
	-@erase "$(INTDIR)\dxfimp.res"
	-@erase "$(OUTDIR)\dxfimp.exp"
	-@erase "$(OUTDIR)\dxfimp.lib"
	-@erase "$(OUTDIR)\dxfimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\dxfimp.dli"
	-@erase "..\..\..\maxsdk\plugin\dxfimp.ilk"
	-@erase ".\dxfimp.idb"
	-@erase ".\dxfimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05FA0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dxfimp.pdb" /debug /machine:I386 /def:".\dxfimp.def" /out:"..\..\..\maxsdk\plugin\dxfimp.dli" /implib:"$(OUTDIR)\dxfimp.lib" 
DEF_FILE= \
	".\dxfimp.def"
LINK32_OBJS= \
	"$(INTDIR)\arbaxis.obj" \
	"$(INTDIR)\dxfimp.obj" \
	"$(INTDIR)\dxfimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dxfimp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\dxfimp.dli"


CLEAN :
	-@erase "$(INTDIR)\arbaxis.obj"
	-@erase "$(INTDIR)\dxfimp.obj"
	-@erase "$(INTDIR)\dxfimp.res"
	-@erase "$(OUTDIR)\dxfimp.exp"
	-@erase "$(OUTDIR)\dxfimp.lib"
	-@erase "$(OUTDIR)\dxfimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\dxfimp.dli"
	-@erase "..\..\..\maxsdk\plugin\dxfimp.ilk"
	-@erase ".\dxfimp.idb"
	-@erase ".\dxfimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05FA0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dxfimp.pdb" /debug /machine:I386 /def:".\dxfimp.def" /out:"..\..\..\maxsdk\plugin\dxfimp.dli" /implib:"$(OUTDIR)\dxfimp.lib" 
DEF_FILE= \
	".\dxfimp.def"
LINK32_OBJS= \
	"$(INTDIR)\arbaxis.obj" \
	"$(INTDIR)\dxfimp.obj" \
	"$(INTDIR)\dxfimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Dxfimp.dep")
!INCLUDE "Dxfimp.dep"
!ELSE 
!MESSAGE Warning: cannot find "Dxfimp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dxfimp - Win32 Release" || "$(CFG)" == "dxfimp - Win32 Debug" || "$(CFG)" == "dxfimp - Win32 Hybrid"
SOURCE=.\arbaxis.cpp

"$(INTDIR)\arbaxis.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dxfimp.cpp

"$(INTDIR)\dxfimp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dxfimp.rc

"$(INTDIR)\dxfimp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

