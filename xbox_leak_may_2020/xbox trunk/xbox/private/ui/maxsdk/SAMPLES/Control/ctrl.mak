# Microsoft Developer Studio Generated NMAKE File, Based on Ctrl.dsp
!IF "$(CFG)" == ""
CFG=ctrl - Win32 Release
!MESSAGE No configuration specified. Defaulting to ctrl - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "ctrl - Win32 Release" && "$(CFG)" != "ctrl - Win32 Debug" && "$(CFG)" != "ctrl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ctrl.mak" CFG="ctrl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ctrl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctrl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ctrl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\ctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\boolctrl.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\Ctrl.pch"
	-@erase "$(INTDIR)\eulrctrl.obj"
	-@erase "$(INTDIR)\exprctrl.obj"
	-@erase "$(INTDIR)\followu.obj"
	-@erase "$(INTDIR)\indepos.obj"
	-@erase "$(INTDIR)\indescale.obj"
	-@erase "$(INTDIR)\linkctrl.obj"
	-@erase "$(INTDIR)\loceulrc.obj"
	-@erase "$(INTDIR)\lodctrl.obj"
	-@erase "$(INTDIR)\noizctrl.obj"
	-@erase "$(INTDIR)\pathctrl.obj"
	-@erase "$(INTDIR)\perlin.obj"
	-@erase "$(INTDIR)\surfctrl.obj"
	-@erase "$(OUTDIR)\ctrl.exp"
	-@erase "$(OUTDIR)\ctrl.lib"
	-@erase "..\..\..\maxsdk\plugin\ctrl.dlc"
	-@erase ".\ctrl.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yu"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ctrl.pdb" /machine:I386 /def:".\ctrl.def" /out:"..\..\..\maxsdk\plugin\ctrl.dlc" /implib:"$(OUTDIR)\ctrl.lib" /release 
DEF_FILE= \
	".\ctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\boolctrl.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\eulrctrl.obj" \
	"$(INTDIR)\exprctrl.obj" \
	"$(INTDIR)\followu.obj" \
	"$(INTDIR)\indepos.obj" \
	"$(INTDIR)\indescale.obj" \
	"$(INTDIR)\linkctrl.obj" \
	"$(INTDIR)\loceulrc.obj" \
	"$(INTDIR)\lodctrl.obj" \
	"$(INTDIR)\noizctrl.obj" \
	"$(INTDIR)\pathctrl.obj" \
	"$(INTDIR)\perlin.obj" \
	"$(INTDIR)\surfctrl.obj" \
	"$(INTDIR)\control.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\EXPR.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\ctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ctrl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\ctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\boolctrl.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\Ctrl.pch"
	-@erase "$(INTDIR)\eulrctrl.obj"
	-@erase "$(INTDIR)\exprctrl.obj"
	-@erase "$(INTDIR)\followu.obj"
	-@erase "$(INTDIR)\indepos.obj"
	-@erase "$(INTDIR)\indescale.obj"
	-@erase "$(INTDIR)\linkctrl.obj"
	-@erase "$(INTDIR)\loceulrc.obj"
	-@erase "$(INTDIR)\lodctrl.obj"
	-@erase "$(INTDIR)\noizctrl.obj"
	-@erase "$(INTDIR)\pathctrl.obj"
	-@erase "$(INTDIR)\perlin.obj"
	-@erase "$(INTDIR)\surfctrl.obj"
	-@erase "$(OUTDIR)\ctrl.exp"
	-@erase "$(OUTDIR)\ctrl.lib"
	-@erase "$(OUTDIR)\ctrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\ctrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\ctrl.ilk"
	-@erase ".\ctrl.idb"
	-@erase ".\ctrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yu"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ctrl.pdb" /debug /machine:I386 /def:".\ctrl.def" /out:"..\..\..\maxsdk\plugin\ctrl.dlc" /implib:"$(OUTDIR)\ctrl.lib" 
DEF_FILE= \
	".\ctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\boolctrl.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\eulrctrl.obj" \
	"$(INTDIR)\exprctrl.obj" \
	"$(INTDIR)\followu.obj" \
	"$(INTDIR)\indepos.obj" \
	"$(INTDIR)\indescale.obj" \
	"$(INTDIR)\linkctrl.obj" \
	"$(INTDIR)\loceulrc.obj" \
	"$(INTDIR)\lodctrl.obj" \
	"$(INTDIR)\noizctrl.obj" \
	"$(INTDIR)\pathctrl.obj" \
	"$(INTDIR)\perlin.obj" \
	"$(INTDIR)\surfctrl.obj" \
	"$(INTDIR)\control.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\EXPR.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\ctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ctrl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\ctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\boolctrl.obj"
	-@erase "$(INTDIR)\control.res"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\Ctrl.pch"
	-@erase "$(INTDIR)\eulrctrl.obj"
	-@erase "$(INTDIR)\exprctrl.obj"
	-@erase "$(INTDIR)\followu.obj"
	-@erase "$(INTDIR)\indepos.obj"
	-@erase "$(INTDIR)\indescale.obj"
	-@erase "$(INTDIR)\linkctrl.obj"
	-@erase "$(INTDIR)\loceulrc.obj"
	-@erase "$(INTDIR)\lodctrl.obj"
	-@erase "$(INTDIR)\noizctrl.obj"
	-@erase "$(INTDIR)\pathctrl.obj"
	-@erase "$(INTDIR)\perlin.obj"
	-@erase "$(INTDIR)\surfctrl.obj"
	-@erase "$(OUTDIR)\ctrl.exp"
	-@erase "$(OUTDIR)\ctrl.lib"
	-@erase "$(OUTDIR)\ctrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\ctrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\ctrl.ilk"
	-@erase ".\ctrl.idb"
	-@erase ".\ctrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yu"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05D70000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ctrl.pdb" /debug /machine:I386 /def:".\ctrl.def" /out:"..\..\..\maxsdk\plugin\ctrl.dlc" /implib:"$(OUTDIR)\ctrl.lib" 
DEF_FILE= \
	".\ctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\boolctrl.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\eulrctrl.obj" \
	"$(INTDIR)\exprctrl.obj" \
	"$(INTDIR)\followu.obj" \
	"$(INTDIR)\indepos.obj" \
	"$(INTDIR)\indescale.obj" \
	"$(INTDIR)\linkctrl.obj" \
	"$(INTDIR)\loceulrc.obj" \
	"$(INTDIR)\lodctrl.obj" \
	"$(INTDIR)\noizctrl.obj" \
	"$(INTDIR)\pathctrl.obj" \
	"$(INTDIR)\perlin.obj" \
	"$(INTDIR)\surfctrl.obj" \
	"$(INTDIR)\control.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\EXPR.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\ctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Ctrl.dep")
!INCLUDE "Ctrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "Ctrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ctrl - Win32 Release" || "$(CFG)" == "ctrl - Win32 Debug" || "$(CFG)" == "ctrl - Win32 Hybrid"
SOURCE=.\attach.cpp

"$(INTDIR)\attach.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\boolctrl.cpp

"$(INTDIR)\boolctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\control.rc

"$(INTDIR)\control.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ctrl.cpp

!IF  "$(CFG)" == "ctrl - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yc"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\ctrl.obj"	"$(INTDIR)\Ctrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ctrl - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yc"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\ctrl.obj"	"$(INTDIR)\Ctrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ctrl - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Ctrl.pch" /Yc"ctrl.h" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\ctrl.obj"	"$(INTDIR)\Ctrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\eulrctrl.cpp

"$(INTDIR)\eulrctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\exprctrl.cpp

"$(INTDIR)\exprctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\followu.cpp

"$(INTDIR)\followu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\indepos.cpp

"$(INTDIR)\indepos.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\indescale.cpp

"$(INTDIR)\indescale.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\linkctrl.cpp

"$(INTDIR)\linkctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\loceulrc.cpp

"$(INTDIR)\loceulrc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\lodctrl.cpp

"$(INTDIR)\lodctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\noizctrl.cpp

"$(INTDIR)\noizctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\pathctrl.cpp

"$(INTDIR)\pathctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"


SOURCE=.\perlin.cpp

!IF  "$(CFG)" == "ctrl - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W1 /O2 /Ob0 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\perlin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ctrl - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W1 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\perlin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ctrl - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W1 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"ctrl.pdb" /FD /c 

"$(INTDIR)\perlin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\surfctrl.cpp

"$(INTDIR)\surfctrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Ctrl.pch"



!ENDIF 

