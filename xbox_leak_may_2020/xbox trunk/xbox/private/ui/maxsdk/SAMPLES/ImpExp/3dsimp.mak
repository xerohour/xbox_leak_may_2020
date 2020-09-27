# Microsoft Developer Studio Generated NMAKE File, Based on 3dsimp.dsp
!IF "$(CFG)" == ""
CFG=3dsimp - Win32 Release
!MESSAGE No configuration specified. Defaulting to 3dsimp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "3dsimp - Win32 Release" && "$(CFG)" != "3dsimp - Win32 Debug" && "$(CFG)" != "3dsimp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "3dsimp.mak" CFG="3dsimp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "3dsimp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "3dsimp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "3dsimp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "3dsimp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\3dsimp.dli"


CLEAN :
	-@erase "$(INTDIR)\3dsimp.obj"
	-@erase "$(INTDIR)\3dsimp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\loadmli.obj"
	-@erase "$(OUTDIR)\3dsimp.exp"
	-@erase "$(OUTDIR)\3dsimp.lib"
	-@erase "..\..\..\maxsdk\plugin\3dsimp.dli"
	-@erase ".\3dsimp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsimp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05EB0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\3dsimp.pdb" /machine:I386 /def:".\3dsimp.def" /out:"..\..\..\maxsdk\plugin\3dsimp.dli" /implib:"$(OUTDIR)\3dsimp.lib" /release 
DEF_FILE= \
	".\3dsimp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsimp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\loadmli.obj" \
	"$(INTDIR)\3dsimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "3dsimp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\3dsimp.dli"


CLEAN :
	-@erase "$(INTDIR)\3dsimp.obj"
	-@erase "$(INTDIR)\3dsimp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\loadmli.obj"
	-@erase "$(OUTDIR)\3dsimp.exp"
	-@erase "$(OUTDIR)\3dsimp.lib"
	-@erase "$(OUTDIR)\3dsimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\3dsimp.dli"
	-@erase "..\..\..\maxsdk\plugin\3dsimp.ilk"
	-@erase ".\3dsimp.idb"
	-@erase ".\3dsimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05EB0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\3dsimp.pdb" /debug /machine:I386 /def:".\3dsimp.def" /out:"..\..\..\maxsdk\plugin\3dsimp.dli" /implib:"$(OUTDIR)\3dsimp.lib" 
DEF_FILE= \
	".\3dsimp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsimp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\loadmli.obj" \
	"$(INTDIR)\3dsimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "3dsimp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\3dsimp.dli"


CLEAN :
	-@erase "$(INTDIR)\3dsimp.obj"
	-@erase "$(INTDIR)\3dsimp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\loadmli.obj"
	-@erase "$(OUTDIR)\3dsimp.exp"
	-@erase "$(OUTDIR)\3dsimp.lib"
	-@erase "$(OUTDIR)\3dsimp.pdb"
	-@erase "..\..\..\maxsdk\plugin\3dsimp.dli"
	-@erase "..\..\..\maxsdk\plugin\3dsimp.ilk"
	-@erase ".\3dsimp.idb"
	-@erase ".\3dsimp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsimp.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsimp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05EB0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\3dsimp.pdb" /debug /machine:I386 /def:".\3dsimp.def" /out:"..\..\..\maxsdk\plugin\3dsimp.dli" /implib:"$(OUTDIR)\3dsimp.lib" 
DEF_FILE= \
	".\3dsimp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsimp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\loadmli.obj" \
	"$(INTDIR)\3dsimp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsimp.dli" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("3dsimp.dep")
!INCLUDE "3dsimp.dep"
!ELSE 
!MESSAGE Warning: cannot find "3dsimp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "3dsimp - Win32 Release" || "$(CFG)" == "3dsimp - Win32 Debug" || "$(CFG)" == "3dsimp - Win32 Hybrid"
SOURCE=.\3dsimp.cpp

"$(INTDIR)\3dsimp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\3dsimp.rc

"$(INTDIR)\3dsimp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\3dsmtl.cpp

"$(INTDIR)\3dsmtl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\loadmli.cpp

"$(INTDIR)\loadmli.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

