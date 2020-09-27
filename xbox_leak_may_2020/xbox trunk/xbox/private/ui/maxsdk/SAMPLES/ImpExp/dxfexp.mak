# Microsoft Developer Studio Generated NMAKE File, Based on Dxfexp.dsp
!IF "$(CFG)" == ""
CFG=dxfexp - Win32 Release
!MESSAGE No configuration specified. Defaulting to dxfexp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dxfexp - Win32 Release" && "$(CFG)" != "dxfexp - Win32 Debug" && "$(CFG)" != "dxfexp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dxfexp.mak" CFG="dxfexp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dxfexp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dxfexp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dxfexp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dxfexp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\dxfexp.dle"


CLEAN :
	-@erase "$(INTDIR)\dxfexp.obj"
	-@erase "$(INTDIR)\dxfexp.res"
	-@erase "$(OUTDIR)\dxfexp.exp"
	-@erase "$(OUTDIR)\dxfexp.lib"
	-@erase "..\..\..\maxsdk\plugin\dxfexp.dle"
	-@erase ".\dxfexp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfexp.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfexp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03020000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dxfexp.pdb" /machine:I386 /def:".\dxfexp.def" /out:"..\..\..\maxsdk\plugin\dxfexp.dle" /implib:"$(OUTDIR)\dxfexp.lib" /release 
DEF_FILE= \
	".\dxfexp.def"
LINK32_OBJS= \
	"$(INTDIR)\dxfexp.obj" \
	"$(INTDIR)\dxfexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dxfexp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\dxfexp.dle"


CLEAN :
	-@erase "$(INTDIR)\dxfexp.obj"
	-@erase "$(INTDIR)\dxfexp.res"
	-@erase "$(OUTDIR)\dxfexp.exp"
	-@erase "$(OUTDIR)\dxfexp.lib"
	-@erase "$(OUTDIR)\dxfexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\dxfexp.dle"
	-@erase "..\..\..\maxsdk\plugin\dxfexp.ilk"
	-@erase ".\dxfexp.idb"
	-@erase ".\dxfexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfexp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03020000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dxfexp.pdb" /debug /machine:I386 /def:".\dxfexp.def" /out:"..\..\..\maxsdk\plugin\dxfexp.dle" /implib:"$(OUTDIR)\dxfexp.lib" 
DEF_FILE= \
	".\dxfexp.def"
LINK32_OBJS= \
	"$(INTDIR)\dxfexp.obj" \
	"$(INTDIR)\dxfexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dxfexp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\dxfexp.dle"


CLEAN :
	-@erase "$(INTDIR)\dxfexp.obj"
	-@erase "$(INTDIR)\dxfexp.res"
	-@erase "$(OUTDIR)\dxfexp.exp"
	-@erase "$(OUTDIR)\dxfexp.lib"
	-@erase "$(OUTDIR)\dxfexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\dxfexp.dle"
	-@erase "..\..\..\maxsdk\plugin\dxfexp.ilk"
	-@erase ".\dxfexp.idb"
	-@erase ".\dxfexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"dxfexp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dxfexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dxfexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03020000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dxfexp.pdb" /debug /machine:I386 /def:".\dxfexp.def" /out:"..\..\..\maxsdk\plugin\dxfexp.dle" /implib:"$(OUTDIR)\dxfexp.lib" 
DEF_FILE= \
	".\dxfexp.def"
LINK32_OBJS= \
	"$(INTDIR)\dxfexp.obj" \
	"$(INTDIR)\dxfexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\dxfexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Dxfexp.dep")
!INCLUDE "Dxfexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "Dxfexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dxfexp - Win32 Release" || "$(CFG)" == "dxfexp - Win32 Debug" || "$(CFG)" == "dxfexp - Win32 Hybrid"
SOURCE=.\dxfexp.cpp

"$(INTDIR)\dxfexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dxfexp.rc

"$(INTDIR)\dxfexp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

