# Microsoft Developer Studio Generated NMAKE File, Based on motion.dsp
!IF "$(CFG)" == ""
CFG=motion - Win32 Release
!MESSAGE No configuration specified. Defaulting to motion - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "motion - Win32 Release" && "$(CFG)" != "motion - Win32 Debug" && "$(CFG)" != "motion - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "motion.mak" CFG="motion - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "motion - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "motion - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "motion - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "motion - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\motion.dlc"


CLEAN :
	-@erase "$(INTDIR)\floatmc.obj"
	-@erase "$(INTDIR)\joydev.obj"
	-@erase "$(INTDIR)\mcdevice.obj"
	-@erase "$(INTDIR)\mididev.obj"
	-@erase "$(INTDIR)\mmanager.obj"
	-@erase "$(INTDIR)\motion.obj"
	-@erase "$(INTDIR)\motion.pch"
	-@erase "$(INTDIR)\motion.res"
	-@erase "$(INTDIR)\point3mc.obj"
	-@erase "$(INTDIR)\posmc.obj"
	-@erase "$(INTDIR)\rotmc.obj"
	-@erase "$(INTDIR)\scalemc.obj"
	-@erase "$(OUTDIR)\motion.exp"
	-@erase "$(OUTDIR)\motion.lib"
	-@erase "..\..\..\maxsdk\plugin\motion.dlc"
	-@erase ".\motion.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yu"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\motion.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\motion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib winmm.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05E30000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\motion.pdb" /machine:I386 /def:".\motion.def" /out:"..\..\..\maxsdk\plugin\motion.dlc" /implib:"$(OUTDIR)\motion.lib" /release 
DEF_FILE= \
	".\motion.def"
LINK32_OBJS= \
	"$(INTDIR)\floatmc.obj" \
	"$(INTDIR)\joydev.obj" \
	"$(INTDIR)\mcdevice.obj" \
	"$(INTDIR)\mididev.obj" \
	"$(INTDIR)\mmanager.obj" \
	"$(INTDIR)\motion.obj" \
	"$(INTDIR)\point3mc.obj" \
	"$(INTDIR)\posmc.obj" \
	"$(INTDIR)\rotmc.obj" \
	"$(INTDIR)\scalemc.obj" \
	"$(INTDIR)\motion.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\motion.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "motion - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\motion.dlc"


CLEAN :
	-@erase "$(INTDIR)\floatmc.obj"
	-@erase "$(INTDIR)\joydev.obj"
	-@erase "$(INTDIR)\mcdevice.obj"
	-@erase "$(INTDIR)\mididev.obj"
	-@erase "$(INTDIR)\mmanager.obj"
	-@erase "$(INTDIR)\motion.obj"
	-@erase "$(INTDIR)\motion.pch"
	-@erase "$(INTDIR)\motion.res"
	-@erase "$(INTDIR)\point3mc.obj"
	-@erase "$(INTDIR)\posmc.obj"
	-@erase "$(INTDIR)\rotmc.obj"
	-@erase "$(INTDIR)\scalemc.obj"
	-@erase "$(OUTDIR)\motion.exp"
	-@erase "$(OUTDIR)\motion.lib"
	-@erase "$(OUTDIR)\motion.pdb"
	-@erase "..\..\..\maxsdk\plugin\motion.dlc"
	-@erase "..\..\..\maxsdk\plugin\motion.ilk"
	-@erase ".\motion.idb"
	-@erase ".\motion.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yu"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\motion.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\motion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib winmm.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05E30000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\motion.pdb" /debug /machine:I386 /def:".\motion.def" /out:"..\..\..\maxsdk\plugin\motion.dlc" /implib:"$(OUTDIR)\motion.lib" 
DEF_FILE= \
	".\motion.def"
LINK32_OBJS= \
	"$(INTDIR)\floatmc.obj" \
	"$(INTDIR)\joydev.obj" \
	"$(INTDIR)\mcdevice.obj" \
	"$(INTDIR)\mididev.obj" \
	"$(INTDIR)\mmanager.obj" \
	"$(INTDIR)\motion.obj" \
	"$(INTDIR)\point3mc.obj" \
	"$(INTDIR)\posmc.obj" \
	"$(INTDIR)\rotmc.obj" \
	"$(INTDIR)\scalemc.obj" \
	"$(INTDIR)\motion.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\motion.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "motion - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\motion.dlc"


CLEAN :
	-@erase "$(INTDIR)\floatmc.obj"
	-@erase "$(INTDIR)\joydev.obj"
	-@erase "$(INTDIR)\mcdevice.obj"
	-@erase "$(INTDIR)\mididev.obj"
	-@erase "$(INTDIR)\mmanager.obj"
	-@erase "$(INTDIR)\motion.obj"
	-@erase "$(INTDIR)\motion.pch"
	-@erase "$(INTDIR)\motion.res"
	-@erase "$(INTDIR)\point3mc.obj"
	-@erase "$(INTDIR)\posmc.obj"
	-@erase "$(INTDIR)\rotmc.obj"
	-@erase "$(INTDIR)\scalemc.obj"
	-@erase "$(OUTDIR)\motion.exp"
	-@erase "$(OUTDIR)\motion.lib"
	-@erase "$(OUTDIR)\motion.pdb"
	-@erase "..\..\..\maxsdk\plugin\motion.dlc"
	-@erase "..\..\..\maxsdk\plugin\motion.ilk"
	-@erase ".\motion.idb"
	-@erase ".\motion.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yu"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\motion.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\motion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib winmm.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05E30000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\motion.pdb" /debug /machine:I386 /def:".\motion.def" /out:"..\..\..\maxsdk\plugin\motion.dlc" /implib:"$(OUTDIR)\motion.lib" 
DEF_FILE= \
	".\motion.def"
LINK32_OBJS= \
	"$(INTDIR)\floatmc.obj" \
	"$(INTDIR)\joydev.obj" \
	"$(INTDIR)\mcdevice.obj" \
	"$(INTDIR)\mididev.obj" \
	"$(INTDIR)\mmanager.obj" \
	"$(INTDIR)\motion.obj" \
	"$(INTDIR)\point3mc.obj" \
	"$(INTDIR)\posmc.obj" \
	"$(INTDIR)\rotmc.obj" \
	"$(INTDIR)\scalemc.obj" \
	"$(INTDIR)\motion.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib"

"..\..\..\maxsdk\plugin\motion.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("motion.dep")
!INCLUDE "motion.dep"
!ELSE 
!MESSAGE Warning: cannot find "motion.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "motion - Win32 Release" || "$(CFG)" == "motion - Win32 Debug" || "$(CFG)" == "motion - Win32 Hybrid"
SOURCE=.\floatmc.cpp

"$(INTDIR)\floatmc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\joydev.cpp

"$(INTDIR)\joydev.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\mcdevice.cpp

"$(INTDIR)\mcdevice.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\mididev.cpp

"$(INTDIR)\mididev.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\mmanager.cpp

"$(INTDIR)\mmanager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\motion.cpp

!IF  "$(CFG)" == "motion - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yc"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 

"$(INTDIR)\motion.obj"	"$(INTDIR)\motion.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "motion - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yc"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 

"$(INTDIR)\motion.obj"	"$(INTDIR)\motion.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "motion - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\motion.pch" /Yc"motion.h" /Fo"$(INTDIR)\\" /Fd"motion.pdb" /FD /c 

"$(INTDIR)\motion.obj"	"$(INTDIR)\motion.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\motion.rc

"$(INTDIR)\motion.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\point3mc.cpp

"$(INTDIR)\point3mc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\posmc.cpp

"$(INTDIR)\posmc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\rotmc.cpp

"$(INTDIR)\rotmc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"


SOURCE=.\scalemc.cpp

"$(INTDIR)\scalemc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\motion.pch"



!ENDIF 

