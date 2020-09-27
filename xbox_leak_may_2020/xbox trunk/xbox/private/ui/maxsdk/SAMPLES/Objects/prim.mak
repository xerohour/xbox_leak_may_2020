# Microsoft Developer Studio Generated NMAKE File, Based on Prim.dsp
!IF "$(CFG)" == ""
CFG=prim - Win32 Release
!MESSAGE No configuration specified. Defaulting to prim - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "prim - Win32 Release" && "$(CFG)" != "prim - Win32 Debug" && "$(CFG)" != "prim - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Prim.mak" CFG="prim - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prim - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prim - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prim - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prim - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\prim.dlo"


CLEAN :
	-@erase "$(INTDIR)\3DSURFER.OBJ"
	-@erase "$(INTDIR)\arc.obj"
	-@erase "$(INTDIR)\bones.obj"
	-@erase "$(INTDIR)\boolobj.obj"
	-@erase "$(INTDIR)\boxobj.obj"
	-@erase "$(INTDIR)\camera.obj"
	-@erase "$(INTDIR)\circle.obj"
	-@erase "$(INTDIR)\cone.obj"
	-@erase "$(INTDIR)\cyl.obj"
	-@erase "$(INTDIR)\donut.obj"
	-@erase "$(INTDIR)\ellipse.obj"
	-@erase "$(INTDIR)\gridobj.obj"
	-@erase "$(INTDIR)\hedraobj.obj"
	-@erase "$(INTDIR)\helix.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\mkhedra.obj"
	-@erase "$(INTDIR)\morphcnt.obj"
	-@erase "$(INTDIR)\morphobj.obj"
	-@erase "$(INTDIR)\ngon.obj"
	-@erase "$(INTDIR)\patchgrd.obj"
	-@erase "$(INTDIR)\prim.obj"
	-@erase "$(INTDIR)\Prim.pch"
	-@erase "$(INTDIR)\prim.res"
	-@erase "$(INTDIR)\prothelp.obj"
	-@erase "$(INTDIR)\rain.obj"
	-@erase "$(INTDIR)\rectangl.obj"
	-@erase "$(INTDIR)\sample.obj"
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\star.obj"
	-@erase "$(INTDIR)\tapehelp.obj"
	-@erase "$(INTDIR)\target.obj"
	-@erase "$(INTDIR)\teapatch.obj"
	-@erase "$(INTDIR)\teapot.obj"
	-@erase "$(INTDIR)\teavert.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\torus.obj"
	-@erase "$(INTDIR)\tripatch.obj"
	-@erase "$(INTDIR)\tube.obj"
	-@erase "$(OUTDIR)\prim.exp"
	-@erase "$(OUTDIR)\prim.lib"
	-@erase "..\..\..\maxsdk\plugin\prim.dlo"
	-@erase ".\prim.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prim.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Prim.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x12000000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\prim.pdb" /machine:I386 /def:".\prim.def" /out:"..\..\..\maxsdk\plugin\prim.dlo" /implib:"$(OUTDIR)\prim.lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\prim.def"
LINK32_OBJS= \
	"$(INTDIR)\3DSURFER.OBJ" \
	"$(INTDIR)\arc.obj" \
	"$(INTDIR)\bones.obj" \
	"$(INTDIR)\boolobj.obj" \
	"$(INTDIR)\boxobj.obj" \
	"$(INTDIR)\camera.obj" \
	"$(INTDIR)\circle.obj" \
	"$(INTDIR)\cone.obj" \
	"$(INTDIR)\cyl.obj" \
	"$(INTDIR)\donut.obj" \
	"$(INTDIR)\ellipse.obj" \
	"$(INTDIR)\gridobj.obj" \
	"$(INTDIR)\hedraobj.obj" \
	"$(INTDIR)\helix.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\mkhedra.obj" \
	"$(INTDIR)\morphcnt.obj" \
	"$(INTDIR)\morphobj.obj" \
	"$(INTDIR)\ngon.obj" \
	"$(INTDIR)\patchgrd.obj" \
	"$(INTDIR)\prim.obj" \
	"$(INTDIR)\prothelp.obj" \
	"$(INTDIR)\rain.obj" \
	"$(INTDIR)\rectangl.obj" \
	"$(INTDIR)\sample.obj" \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\star.obj" \
	"$(INTDIR)\tapehelp.obj" \
	"$(INTDIR)\target.obj" \
	"$(INTDIR)\teapatch.obj" \
	"$(INTDIR)\teapot.obj" \
	"$(INTDIR)\teavert.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\torus.obj" \
	"$(INTDIR)\tripatch.obj" \
	"$(INTDIR)\tube.obj" \
	"$(INTDIR)\prim.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\MNMath.lib"

"..\..\..\maxsdk\plugin\prim.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\prim.dlo"


CLEAN :
	-@erase "$(INTDIR)\3DSURFER.OBJ"
	-@erase "$(INTDIR)\arc.obj"
	-@erase "$(INTDIR)\bones.obj"
	-@erase "$(INTDIR)\boolobj.obj"
	-@erase "$(INTDIR)\boxobj.obj"
	-@erase "$(INTDIR)\camera.obj"
	-@erase "$(INTDIR)\circle.obj"
	-@erase "$(INTDIR)\cone.obj"
	-@erase "$(INTDIR)\cyl.obj"
	-@erase "$(INTDIR)\donut.obj"
	-@erase "$(INTDIR)\ellipse.obj"
	-@erase "$(INTDIR)\gridobj.obj"
	-@erase "$(INTDIR)\hedraobj.obj"
	-@erase "$(INTDIR)\helix.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\mkhedra.obj"
	-@erase "$(INTDIR)\morphcnt.obj"
	-@erase "$(INTDIR)\morphobj.obj"
	-@erase "$(INTDIR)\ngon.obj"
	-@erase "$(INTDIR)\patchgrd.obj"
	-@erase "$(INTDIR)\prim.obj"
	-@erase "$(INTDIR)\Prim.pch"
	-@erase "$(INTDIR)\prim.res"
	-@erase "$(INTDIR)\prothelp.obj"
	-@erase "$(INTDIR)\rain.obj"
	-@erase "$(INTDIR)\rectangl.obj"
	-@erase "$(INTDIR)\sample.obj"
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\star.obj"
	-@erase "$(INTDIR)\tapehelp.obj"
	-@erase "$(INTDIR)\target.obj"
	-@erase "$(INTDIR)\teapatch.obj"
	-@erase "$(INTDIR)\teapot.obj"
	-@erase "$(INTDIR)\teavert.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\torus.obj"
	-@erase "$(INTDIR)\tripatch.obj"
	-@erase "$(INTDIR)\tube.obj"
	-@erase "$(OUTDIR)\prim.exp"
	-@erase "$(OUTDIR)\prim.lib"
	-@erase "$(OUTDIR)\prim.pdb"
	-@erase "..\..\..\maxsdk\plugin\prim.dlo"
	-@erase "..\..\..\maxsdk\plugin\prim.ilk"
	-@erase ".\prim.idb"
	-@erase ".\prim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prim.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Prim.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x12000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\prim.pdb" /debug /machine:I386 /def:".\prim.def" /out:"..\..\..\maxsdk\plugin\prim.dlo" /implib:"$(OUTDIR)\prim.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\prim.def"
LINK32_OBJS= \
	"$(INTDIR)\3DSURFER.OBJ" \
	"$(INTDIR)\arc.obj" \
	"$(INTDIR)\bones.obj" \
	"$(INTDIR)\boolobj.obj" \
	"$(INTDIR)\boxobj.obj" \
	"$(INTDIR)\camera.obj" \
	"$(INTDIR)\circle.obj" \
	"$(INTDIR)\cone.obj" \
	"$(INTDIR)\cyl.obj" \
	"$(INTDIR)\donut.obj" \
	"$(INTDIR)\ellipse.obj" \
	"$(INTDIR)\gridobj.obj" \
	"$(INTDIR)\hedraobj.obj" \
	"$(INTDIR)\helix.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\mkhedra.obj" \
	"$(INTDIR)\morphcnt.obj" \
	"$(INTDIR)\morphobj.obj" \
	"$(INTDIR)\ngon.obj" \
	"$(INTDIR)\patchgrd.obj" \
	"$(INTDIR)\prim.obj" \
	"$(INTDIR)\prothelp.obj" \
	"$(INTDIR)\rain.obj" \
	"$(INTDIR)\rectangl.obj" \
	"$(INTDIR)\sample.obj" \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\star.obj" \
	"$(INTDIR)\tapehelp.obj" \
	"$(INTDIR)\target.obj" \
	"$(INTDIR)\teapatch.obj" \
	"$(INTDIR)\teapot.obj" \
	"$(INTDIR)\teavert.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\torus.obj" \
	"$(INTDIR)\tripatch.obj" \
	"$(INTDIR)\tube.obj" \
	"$(INTDIR)\prim.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\MNMath.lib"

"..\..\..\maxsdk\plugin\prim.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\prim.dlo"


CLEAN :
	-@erase "$(INTDIR)\3DSURFER.OBJ"
	-@erase "$(INTDIR)\arc.obj"
	-@erase "$(INTDIR)\bones.obj"
	-@erase "$(INTDIR)\boolobj.obj"
	-@erase "$(INTDIR)\boxobj.obj"
	-@erase "$(INTDIR)\camera.obj"
	-@erase "$(INTDIR)\circle.obj"
	-@erase "$(INTDIR)\cone.obj"
	-@erase "$(INTDIR)\cyl.obj"
	-@erase "$(INTDIR)\donut.obj"
	-@erase "$(INTDIR)\ellipse.obj"
	-@erase "$(INTDIR)\gridobj.obj"
	-@erase "$(INTDIR)\hedraobj.obj"
	-@erase "$(INTDIR)\helix.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\mkhedra.obj"
	-@erase "$(INTDIR)\morphcnt.obj"
	-@erase "$(INTDIR)\morphobj.obj"
	-@erase "$(INTDIR)\ngon.obj"
	-@erase "$(INTDIR)\patchgrd.obj"
	-@erase "$(INTDIR)\prim.obj"
	-@erase "$(INTDIR)\Prim.pch"
	-@erase "$(INTDIR)\prim.res"
	-@erase "$(INTDIR)\prothelp.obj"
	-@erase "$(INTDIR)\rain.obj"
	-@erase "$(INTDIR)\rectangl.obj"
	-@erase "$(INTDIR)\sample.obj"
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\star.obj"
	-@erase "$(INTDIR)\tapehelp.obj"
	-@erase "$(INTDIR)\target.obj"
	-@erase "$(INTDIR)\teapatch.obj"
	-@erase "$(INTDIR)\teapot.obj"
	-@erase "$(INTDIR)\teavert.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\torus.obj"
	-@erase "$(INTDIR)\tripatch.obj"
	-@erase "$(INTDIR)\tube.obj"
	-@erase "$(OUTDIR)\prim.exp"
	-@erase "$(OUTDIR)\prim.lib"
	-@erase "$(OUTDIR)\prim.pdb"
	-@erase "..\..\..\maxsdk\plugin\prim.dlo"
	-@erase "..\..\..\maxsdk\plugin\prim.ilk"
	-@erase ".\prim.idb"
	-@erase ".\prim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prim.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Prim.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x12000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\prim.pdb" /debug /machine:I386 /def:".\prim.def" /out:"..\..\..\maxsdk\plugin\prim.dlo" /implib:"$(OUTDIR)\prim.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\prim.def"
LINK32_OBJS= \
	"$(INTDIR)\3DSURFER.OBJ" \
	"$(INTDIR)\arc.obj" \
	"$(INTDIR)\bones.obj" \
	"$(INTDIR)\boolobj.obj" \
	"$(INTDIR)\boxobj.obj" \
	"$(INTDIR)\camera.obj" \
	"$(INTDIR)\circle.obj" \
	"$(INTDIR)\cone.obj" \
	"$(INTDIR)\cyl.obj" \
	"$(INTDIR)\donut.obj" \
	"$(INTDIR)\ellipse.obj" \
	"$(INTDIR)\gridobj.obj" \
	"$(INTDIR)\hedraobj.obj" \
	"$(INTDIR)\helix.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\mkhedra.obj" \
	"$(INTDIR)\morphcnt.obj" \
	"$(INTDIR)\morphobj.obj" \
	"$(INTDIR)\ngon.obj" \
	"$(INTDIR)\patchgrd.obj" \
	"$(INTDIR)\prim.obj" \
	"$(INTDIR)\prothelp.obj" \
	"$(INTDIR)\rain.obj" \
	"$(INTDIR)\rectangl.obj" \
	"$(INTDIR)\sample.obj" \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\star.obj" \
	"$(INTDIR)\tapehelp.obj" \
	"$(INTDIR)\target.obj" \
	"$(INTDIR)\teapatch.obj" \
	"$(INTDIR)\teapot.obj" \
	"$(INTDIR)\teavert.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\torus.obj" \
	"$(INTDIR)\tripatch.obj" \
	"$(INTDIR)\tube.obj" \
	"$(INTDIR)\prim.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\MNMath.lib"

"..\..\..\maxsdk\plugin\prim.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Prim.dep")
!INCLUDE "Prim.dep"
!ELSE 
!MESSAGE Warning: cannot find "Prim.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prim - Win32 Release" || "$(CFG)" == "prim - Win32 Debug" || "$(CFG)" == "prim - Win32 Hybrid"
SOURCE=.\3DSURFER.CPP

"$(INTDIR)\3DSURFER.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"


SOURCE=.\arc.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\arc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\arc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\arc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\bones.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\bones.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\bones.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\bones.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\boolobj.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boolobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boolobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boolobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\boxobj.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boxobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boxobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\boxobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\camera.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\camera.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\camera.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\camera.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\circle.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\circle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\circle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\circle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\cone.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cone.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cone.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cone.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\cyl.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cyl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cyl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\cyl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\donut.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\donut.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\donut.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\donut.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ellipse.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ellipse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ellipse.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ellipse.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\gridobj.cpp

"$(INTDIR)\gridobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"


SOURCE=.\hedraobj.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\hedraobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\hedraobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\hedraobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\helix.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\helix.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\helix.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\helix.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\light.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\light.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\light.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\light.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mkhedra.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\mkhedra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\mkhedra.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\mkhedra.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\morphcnt.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphcnt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphcnt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphcnt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\morphobj.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\morphobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ngon.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ngon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ngon.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\ngon.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\patchgrd.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\patchgrd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\patchgrd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\patchgrd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\prim.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yc"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prim.obj"	"$(INTDIR)\Prim.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yc"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prim.obj"	"$(INTDIR)\Prim.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yc"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prim.obj"	"$(INTDIR)\Prim.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\prim.rc

"$(INTDIR)\prim.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\prothelp.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prothelp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prothelp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\prothelp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\rain.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\rectangl.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rectangl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rectangl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\rectangl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\sample.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sample.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sample.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sample.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\sphere.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sphere.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sphere.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\sphere.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\spline.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\spline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\spline.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\spline.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\star.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\star.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\star.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\star.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\tapehelp.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tapehelp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tapehelp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tapehelp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\target.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\target.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\target.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\target.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\teapatch.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\teapot.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapot.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teapot.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\teavert.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teavert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teavert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\teavert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\text.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\text.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\text.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\text.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\torus.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\torus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\torus.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\torus.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\tripatch.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tripatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tripatch.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tripatch.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\tube.cpp

!IF  "$(CFG)" == "prim - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tube.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tube.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "prim - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Prim.pch" /Yu"prim.h" /Fo"$(INTDIR)\\" /Fd"prim.pdb" /FD /c 

"$(INTDIR)\tube.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Prim.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

