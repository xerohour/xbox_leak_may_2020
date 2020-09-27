# Microsoft Developer Studio Generated NMAKE File, Based on block.dsp
!IF "$(CFG)" == ""
CFG=block - Win32 Release
!MESSAGE No configuration specified. Defaulting to block - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "block - Win32 Release" && "$(CFG)" != "block - Win32 Debug" && "$(CFG)" != "block - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "block.mak" CFG="block - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "block - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "block - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "block - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "block - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\block.dlc" "$(OUTDIR)\block.pch"


CLEAN :
	-@erase "$(INTDIR)\block.obj"
	-@erase "$(INTDIR)\block.pch"
	-@erase "$(INTDIR)\blockcontrol.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ControlContainer.obj"
	-@erase "$(INTDIR)\masterblock.obj"
	-@erase "$(INTDIR)\slave.obj"
	-@erase "$(OUTDIR)\block.exp"
	-@erase "$(OUTDIR)\block.lib"
	-@erase "..\..\..\..\maxsdk\plugin\block.dlc"
	-@erase ".\block.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\control.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\block.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2b260000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\block.pdb" /machine:I386 /def:".\block.def" /out:"..\..\..\..\maxsdk\plugin\block.dlc" /implib:"$(OUTDIR)\block.lib" /release 
DEF_FILE= \
	".\block.def"
LINK32_OBJS= \
	"$(INTDIR)\block.obj" \
	"$(INTDIR)\blockcontrol.obj" \
	"$(INTDIR)\ControlContainer.obj" \
	"$(INTDIR)\masterblock.obj" \
	"$(INTDIR)\slave.obj" \
	"$(INTDIR)\control.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\expr.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\block.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "block - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\block.dlc" "$(OUTDIR)\block.pch"


CLEAN :
	-@erase "$(INTDIR)\block.obj"
	-@erase "$(INTDIR)\block.pch"
	-@erase "$(INTDIR)\blockcontrol.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ControlContainer.obj"
	-@erase "$(INTDIR)\masterblock.obj"
	-@erase "$(INTDIR)\slave.obj"
	-@erase "$(OUTDIR)\block.exp"
	-@erase "$(OUTDIR)\block.lib"
	-@erase "$(OUTDIR)\block.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\block.dlc"
	-@erase "..\..\..\..\maxsdk\plugin\block.ilk"
	-@erase ".\block.idb"
	-@erase ".\block.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\control.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\block.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2b260000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\block.pdb" /debug /machine:I386 /def:".\block.def" /out:"..\..\..\..\maxsdk\plugin\block.dlc" /implib:"$(OUTDIR)\block.lib" 
DEF_FILE= \
	".\block.def"
LINK32_OBJS= \
	"$(INTDIR)\block.obj" \
	"$(INTDIR)\blockcontrol.obj" \
	"$(INTDIR)\ControlContainer.obj" \
	"$(INTDIR)\masterblock.obj" \
	"$(INTDIR)\slave.obj" \
	"$(INTDIR)\control.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\expr.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\block.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "block - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid
# Begin Custom Macros
OutDir=.\Hybrid
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\block.dlc" "$(OUTDIR)\block.pch"


CLEAN :
	-@erase "$(INTDIR)\block.obj"
	-@erase "$(INTDIR)\block.pch"
	-@erase "$(INTDIR)\blockcontrol.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ControlContainer.obj"
	-@erase "$(INTDIR)\masterblock.obj"
	-@erase "$(INTDIR)\slave.obj"
	-@erase "$(OUTDIR)\block.exp"
	-@erase "$(OUTDIR)\block.lib"
	-@erase "$(OUTDIR)\block.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\block.dlc"
	-@erase "..\..\..\..\maxsdk\plugin\block.ilk"
	-@erase ".\block.idb"
	-@erase ".\block.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\control.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\block.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2b260000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\block.pdb" /debug /machine:I386 /def:".\block.def" /out:"..\..\..\..\maxsdk\plugin\block.dlc" /implib:"$(OUTDIR)\block.lib" 
DEF_FILE= \
	".\block.def"
LINK32_OBJS= \
	"$(INTDIR)\block.obj" \
	"$(INTDIR)\blockcontrol.obj" \
	"$(INTDIR)\ControlContainer.obj" \
	"$(INTDIR)\masterblock.obj" \
	"$(INTDIR)\slave.obj" \
	"$(INTDIR)\control.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\expr.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\gfx.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\Paramblk2.lib"

"..\..\..\..\maxsdk\plugin\block.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("block.dep")
!INCLUDE "block.dep"
!ELSE 
!MESSAGE Warning: cannot find "block.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "block - Win32 Release" || "$(CFG)" == "block - Win32 Debug" || "$(CFG)" == "block - Win32 Hybrid"
SOURCE=.\block.cpp

!IF  "$(CFG)" == "block - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\block.pch" /Yc"block.h" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

"$(INTDIR)\block.obj"	"$(INTDIR)\block.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "block - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\block.pch" /Yc"block.h" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

"$(INTDIR)\block.obj"	"$(INTDIR)\block.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "block - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\block.pch" /Yc"block.h" /Fo"$(INTDIR)\\" /Fd"block.pdb" /FD /c 

"$(INTDIR)\block.obj"	"$(INTDIR)\block.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\blockcontrol.cpp

"$(INTDIR)\blockcontrol.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ControlContainer.cpp

"$(INTDIR)\ControlContainer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\masterblock.cpp

"$(INTDIR)\masterblock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\slave.cpp

"$(INTDIR)\slave.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\control.rc

"$(INTDIR)\control.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

