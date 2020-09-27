# Microsoft Developer Studio Generated NMAKE File, Based on Mods.dsp
!IF "$(CFG)" == ""
CFG=mods - Win32 Debug Profiled
!MESSAGE No configuration specified. Defaulting to mods - Win32 Debug Profiled.
!ENDIF 

!IF "$(CFG)" != "mods - Win32 Release" && "$(CFG)" != "mods - Win32 Debug" && "$(CFG)" != "mods - Win32 Hybrid" && "$(CFG)" != "mods - Win32 Debug Profiled"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mods.mak" CFG="mods - Win32 Debug Profiled"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mods - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Debug Profiled" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mods - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\mods.dlm"


CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispApprox.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\nspline.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wind.obj"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "..\..\..\maxsdk\plugin\mods.dlm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mods.pdb" /machine:I386 /def:".\mods.def" /out:"..\..\..\maxsdk\plugin\mods.dlm" /implib:"$(OUTDIR)\mods.lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispApprox.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\nspline.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"$(INTDIR)\wind.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\acap.lib"

"..\..\..\maxsdk\plugin\mods.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\maxsdk\plugin\mods.dlm" "$(OUTDIR)\Mods.pch"


CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispApprox.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\nspline.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wind.obj"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "$(OUTDIR)\mods.pdb"
	-@erase "..\..\..\maxsdk\plugin\mods.dlm"
	-@erase "..\..\..\maxsdk\plugin\mods.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mods.pdb" /debug /machine:I386 /def:".\mods.def" /out:"..\..\..\maxsdk\plugin\mods.dlm" /implib:"$(OUTDIR)\mods.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispApprox.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\nspline.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"$(INTDIR)\wind.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\acap.lib"

"..\..\..\maxsdk\plugin\mods.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\mods.dlm"


CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispApprox.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\nspline.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wind.obj"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "$(OUTDIR)\mods.pdb"
	-@erase "..\..\..\maxsdk\plugin\mods.dlm"
	-@erase "..\..\..\maxsdk\plugin\mods.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mods.pdb" /debug /machine:I386 /def:".\mods.def" /out:"..\..\..\maxsdk\plugin\mods.dlm" /implib:"$(OUTDIR)\mods.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispApprox.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\nspline.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"$(INTDIR)\wind.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\acap.lib"

"..\..\..\maxsdk\plugin\mods.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mods - Win32 Debug Profiled"

OUTDIR=.\DbgProf
INTDIR=.\DbgProf
# Begin Custom Macros
OutDir=.\DbgProf
# End Custom Macros

ALL : "..\..\..\maxsdk\plugin\mods.dlm" "$(OUTDIR)\Mods.pch"


CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispApprox.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\nspline.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wind.obj"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "$(OUTDIR)\mods.pdb"
	-@erase "..\..\..\maxsdk\plugin\mods.dlm"
	-@erase "..\..\..\maxsdk\plugin\mods.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gh /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mods.pdb" /debug /machine:I386 /def:".\mods.def" /out:"..\..\..\maxsdk\plugin\mods.dlm" /implib:"$(OUTDIR)\mods.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispApprox.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\nspline.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"$(INTDIR)\wind.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\acap.lib"

"..\..\..\maxsdk\plugin\mods.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Mods.dep")
!INCLUDE "Mods.dep"
!ELSE 
!MESSAGE Warning: cannot find "Mods.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mods - Win32 Release" || "$(CFG)" == "mods - Win32 Debug" || "$(CFG)" == "mods - Win32 Hybrid" || "$(CFG)" == "mods - Win32 Debug Profiled"
SOURCE=.\afregion.cpp

"$(INTDIR)\afregion.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\bend.cpp

"$(INTDIR)\bend.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\bomb.cpp

"$(INTDIR)\bomb.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\clstnode.cpp

"$(INTDIR)\clstnode.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\clustmod.cpp

"$(INTDIR)\clustmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\deflect.cpp

"$(INTDIR)\deflect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\delmod.cpp

"$(INTDIR)\delmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\delsmod.cpp

"$(INTDIR)\delsmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\dispApprox.cpp

"$(INTDIR)\dispApprox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\dispmod.cpp

"$(INTDIR)\dispmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editmesh.cpp

"$(INTDIR)\editmesh.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editmops.cpp

"$(INTDIR)\editmops.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editpat.cpp

"$(INTDIR)\editpat.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editpops.cpp

"$(INTDIR)\editpops.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editsops.cpp

"$(INTDIR)\editsops.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\editspl.cpp

"$(INTDIR)\editspl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\edmdata.cpp

"$(INTDIR)\edmdata.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\edmrest.cpp

"$(INTDIR)\edmrest.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\edmui.cpp

"$(INTDIR)\edmui.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\extrude.cpp

"$(INTDIR)\extrude.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\fextrude.cpp

"$(INTDIR)\fextrude.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\gravity.cpp

"$(INTDIR)\gravity.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\mapmod.cpp

"$(INTDIR)\mapmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\meshsel.cpp

"$(INTDIR)\meshsel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\mirror.cpp

"$(INTDIR)\mirror.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\mods.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mods - Win32 Debug Profiled"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gh /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mods.rc

"$(INTDIR)\mods.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\noisemod.cpp

"$(INTDIR)\noisemod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\nspline.cpp

"$(INTDIR)\nspline.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\optmod.cpp

"$(INTDIR)\optmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\resettm.cpp

"$(INTDIR)\resettm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\sctex.cpp

"$(INTDIR)\sctex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\selmod.cpp

"$(INTDIR)\selmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\sinwave.cpp

"$(INTDIR)\sinwave.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\skew.cpp

"$(INTDIR)\skew.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\surfmod.cpp

"$(INTDIR)\surfmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\surfrev.cpp

"$(INTDIR)\surfrev.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\taper.cpp

"$(INTDIR)\taper.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\tessmod.cpp

"$(INTDIR)\tessmod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\TWIST.CPP
SOURCE=.\twist1.cpp

"$(INTDIR)\twist1.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\twist2.cpp

"$(INTDIR)\twist2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\unwrap.cpp

"$(INTDIR)\unwrap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\uvwxform.cpp

"$(INTDIR)\uvwxform.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"


SOURCE=.\wind.cpp

"$(INTDIR)\wind.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mods.pch"



!ENDIF 

