# Microsoft Developer Studio Generated NMAKE File, Based on Mtls.dsp
!IF "$(CFG)" == ""
CFG=mtl - Win32 Release
!MESSAGE No configuration specified. Defaulting to mtl - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "mtl - Win32 Release" && "$(CFG)" != "mtl - Win32 Debug" && "$(CFG)" != "mtl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mtls.mak" CFG="mtl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mtl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mtl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mtl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mtl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\mtl.dlt"


CLEAN :
	-@erase "$(INTDIR)\acubic.obj"
	-@erase "$(INTDIR)\bmtex.obj"
	-@erase "$(INTDIR)\checker.obj"
	-@erase "$(INTDIR)\cmtl.obj"
	-@erase "$(INTDIR)\composit.obj"
	-@erase "$(INTDIR)\composite_material.obj"
	-@erase "$(INTDIR)\dblsided.obj"
	-@erase "$(INTDIR)\falloff.obj"
	-@erase "$(INTDIR)\gradient.obj"
	-@erase "$(INTDIR)\marble.obj"
	-@erase "$(INTDIR)\mask.obj"
	-@erase "$(INTDIR)\matte.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mix.obj"
	-@erase "$(INTDIR)\mixmat.obj"
	-@erase "$(INTDIR)\mtl.res"
	-@erase "$(INTDIR)\mtlmain.obj"
	-@erase "$(INTDIR)\Mtls.pch"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\partage.obj"
	-@erase "$(INTDIR)\partblur.obj"
	-@erase "$(INTDIR)\plate.obj"
	-@erase "$(INTDIR)\rgbmult.obj"
	-@erase "$(INTDIR)\stdmtl.obj"
	-@erase "$(INTDIR)\stdmtl2.obj"
	-@erase "$(INTDIR)\stdShaders.obj"
	-@erase "$(INTDIR)\TexMaps.obj"
	-@erase "$(INTDIR)\tint.obj"
	-@erase "$(INTDIR)\vertcol.obj"
	-@erase "$(OUTDIR)\mtl.exp"
	-@erase "$(OUTDIR)\mtl.lib"
	-@erase "..\..\..\maxsdk\plugin\mtl.dlt"
	-@erase ".\mtl.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yu"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mtl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mtls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mtl.pdb" /machine:I386 /def:".\mtl.def" /out:"..\..\..\maxsdk\plugin\mtl.dlt" /implib:"$(OUTDIR)\mtl.lib" /release 
DEF_FILE= \
	".\mtl.def"
LINK32_OBJS= \
	"$(INTDIR)\acubic.obj" \
	"$(INTDIR)\bmtex.obj" \
	"$(INTDIR)\checker.obj" \
	"$(INTDIR)\cmtl.obj" \
	"$(INTDIR)\composit.obj" \
	"$(INTDIR)\composite_material.obj" \
	"$(INTDIR)\dblsided.obj" \
	"$(INTDIR)\falloff.obj" \
	"$(INTDIR)\gradient.obj" \
	"$(INTDIR)\marble.obj" \
	"$(INTDIR)\mask.obj" \
	"$(INTDIR)\matte.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mix.obj" \
	"$(INTDIR)\mixmat.obj" \
	"$(INTDIR)\mtlmain.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\noise.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\partage.obj" \
	"$(INTDIR)\partblur.obj" \
	"$(INTDIR)\plate.obj" \
	"$(INTDIR)\rgbmult.obj" \
	"$(INTDIR)\stdmtl.obj" \
	"$(INTDIR)\stdmtl2.obj" \
	"$(INTDIR)\stdShaders.obj" \
	"$(INTDIR)\TexMaps.obj" \
	"$(INTDIR)\tint.obj" \
	"$(INTDIR)\vertcol.obj" \
	"$(INTDIR)\mtl.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\mtl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mtl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\mtl.dlt"


CLEAN :
	-@erase "$(INTDIR)\acubic.obj"
	-@erase "$(INTDIR)\bmtex.obj"
	-@erase "$(INTDIR)\checker.obj"
	-@erase "$(INTDIR)\cmtl.obj"
	-@erase "$(INTDIR)\composit.obj"
	-@erase "$(INTDIR)\composite_material.obj"
	-@erase "$(INTDIR)\dblsided.obj"
	-@erase "$(INTDIR)\falloff.obj"
	-@erase "$(INTDIR)\gradient.obj"
	-@erase "$(INTDIR)\marble.obj"
	-@erase "$(INTDIR)\mask.obj"
	-@erase "$(INTDIR)\matte.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mix.obj"
	-@erase "$(INTDIR)\mixmat.obj"
	-@erase "$(INTDIR)\mtl.res"
	-@erase "$(INTDIR)\mtlmain.obj"
	-@erase "$(INTDIR)\Mtls.pch"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\partage.obj"
	-@erase "$(INTDIR)\partblur.obj"
	-@erase "$(INTDIR)\plate.obj"
	-@erase "$(INTDIR)\rgbmult.obj"
	-@erase "$(INTDIR)\stdmtl.obj"
	-@erase "$(INTDIR)\stdmtl2.obj"
	-@erase "$(INTDIR)\stdShaders.obj"
	-@erase "$(INTDIR)\TexMaps.obj"
	-@erase "$(INTDIR)\tint.obj"
	-@erase "$(INTDIR)\vertcol.obj"
	-@erase "$(OUTDIR)\mtl.exp"
	-@erase "$(OUTDIR)\mtl.lib"
	-@erase "$(OUTDIR)\mtl.pdb"
	-@erase "..\..\..\maxsdk\plugin\mtl.dlt"
	-@erase "..\..\..\maxsdk\plugin\mtl.ilk"
	-@erase ".\mtl.idb"
	-@erase ".\mtl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yu"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mtl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mtls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mtl.pdb" /debug /machine:I386 /def:".\mtl.def" /out:"..\..\..\maxsdk\plugin\mtl.dlt" /implib:"$(OUTDIR)\mtl.lib" 
DEF_FILE= \
	".\mtl.def"
LINK32_OBJS= \
	"$(INTDIR)\acubic.obj" \
	"$(INTDIR)\bmtex.obj" \
	"$(INTDIR)\checker.obj" \
	"$(INTDIR)\cmtl.obj" \
	"$(INTDIR)\composit.obj" \
	"$(INTDIR)\composite_material.obj" \
	"$(INTDIR)\dblsided.obj" \
	"$(INTDIR)\falloff.obj" \
	"$(INTDIR)\gradient.obj" \
	"$(INTDIR)\marble.obj" \
	"$(INTDIR)\mask.obj" \
	"$(INTDIR)\matte.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mix.obj" \
	"$(INTDIR)\mixmat.obj" \
	"$(INTDIR)\mtlmain.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\noise.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\partage.obj" \
	"$(INTDIR)\partblur.obj" \
	"$(INTDIR)\plate.obj" \
	"$(INTDIR)\rgbmult.obj" \
	"$(INTDIR)\stdmtl.obj" \
	"$(INTDIR)\stdmtl2.obj" \
	"$(INTDIR)\stdShaders.obj" \
	"$(INTDIR)\TexMaps.obj" \
	"$(INTDIR)\tint.obj" \
	"$(INTDIR)\vertcol.obj" \
	"$(INTDIR)\mtl.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\mtl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mtl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\mtl.dlt"


CLEAN :
	-@erase "$(INTDIR)\acubic.obj"
	-@erase "$(INTDIR)\bmtex.obj"
	-@erase "$(INTDIR)\checker.obj"
	-@erase "$(INTDIR)\cmtl.obj"
	-@erase "$(INTDIR)\composit.obj"
	-@erase "$(INTDIR)\composite_material.obj"
	-@erase "$(INTDIR)\dblsided.obj"
	-@erase "$(INTDIR)\falloff.obj"
	-@erase "$(INTDIR)\gradient.obj"
	-@erase "$(INTDIR)\marble.obj"
	-@erase "$(INTDIR)\mask.obj"
	-@erase "$(INTDIR)\matte.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mix.obj"
	-@erase "$(INTDIR)\mixmat.obj"
	-@erase "$(INTDIR)\mtl.res"
	-@erase "$(INTDIR)\mtlmain.obj"
	-@erase "$(INTDIR)\Mtls.pch"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\partage.obj"
	-@erase "$(INTDIR)\partblur.obj"
	-@erase "$(INTDIR)\plate.obj"
	-@erase "$(INTDIR)\rgbmult.obj"
	-@erase "$(INTDIR)\stdmtl.obj"
	-@erase "$(INTDIR)\stdmtl2.obj"
	-@erase "$(INTDIR)\stdShaders.obj"
	-@erase "$(INTDIR)\TexMaps.obj"
	-@erase "$(INTDIR)\tint.obj"
	-@erase "$(INTDIR)\vertcol.obj"
	-@erase "$(OUTDIR)\mtl.exp"
	-@erase "$(OUTDIR)\mtl.lib"
	-@erase "$(OUTDIR)\mtl.pdb"
	-@erase "..\..\..\maxsdk\plugin\mtl.dlt"
	-@erase "..\..\..\maxsdk\plugin\mtl.ilk"
	-@erase ".\mtl.idb"
	-@erase ".\mtl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yu"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mtl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mtls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2f2b0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mtl.pdb" /debug /machine:I386 /def:".\mtl.def" /out:"..\..\..\maxsdk\plugin\mtl.dlt" /implib:"$(OUTDIR)\mtl.lib" 
DEF_FILE= \
	".\mtl.def"
LINK32_OBJS= \
	"$(INTDIR)\acubic.obj" \
	"$(INTDIR)\bmtex.obj" \
	"$(INTDIR)\checker.obj" \
	"$(INTDIR)\cmtl.obj" \
	"$(INTDIR)\composit.obj" \
	"$(INTDIR)\composite_material.obj" \
	"$(INTDIR)\dblsided.obj" \
	"$(INTDIR)\falloff.obj" \
	"$(INTDIR)\gradient.obj" \
	"$(INTDIR)\marble.obj" \
	"$(INTDIR)\mask.obj" \
	"$(INTDIR)\matte.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mix.obj" \
	"$(INTDIR)\mixmat.obj" \
	"$(INTDIR)\mtlmain.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\noise.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\partage.obj" \
	"$(INTDIR)\partblur.obj" \
	"$(INTDIR)\plate.obj" \
	"$(INTDIR)\rgbmult.obj" \
	"$(INTDIR)\stdmtl.obj" \
	"$(INTDIR)\stdmtl2.obj" \
	"$(INTDIR)\stdShaders.obj" \
	"$(INTDIR)\TexMaps.obj" \
	"$(INTDIR)\tint.obj" \
	"$(INTDIR)\vertcol.obj" \
	"$(INTDIR)\mtl.res" \
	"..\..\Lib\BMM.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\mtl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Mtls.dep")
!INCLUDE "Mtls.dep"
!ELSE 
!MESSAGE Warning: cannot find "Mtls.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mtl - Win32 Release" || "$(CFG)" == "mtl - Win32 Debug" || "$(CFG)" == "mtl - Win32 Hybrid"
SOURCE=.\acubic.cpp

"$(INTDIR)\acubic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\bmtex.cpp

"$(INTDIR)\bmtex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\checker.cpp

"$(INTDIR)\checker.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\cmtl.cpp

"$(INTDIR)\cmtl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\composit.cpp

"$(INTDIR)\composit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\composite_material.cpp

"$(INTDIR)\composite_material.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\dblsided.cpp

"$(INTDIR)\dblsided.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\falloff.cpp

"$(INTDIR)\falloff.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\gradient.cpp

"$(INTDIR)\gradient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\marble.cpp

"$(INTDIR)\marble.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\mask.cpp

"$(INTDIR)\mask.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\matte.cpp

"$(INTDIR)\matte.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\mirror.cpp

"$(INTDIR)\mirror.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\mix.cpp

"$(INTDIR)\mix.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\mixmat.cpp

"$(INTDIR)\mixmat.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\mtl.rc

"$(INTDIR)\mtl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\mtlmain.cpp

!IF  "$(CFG)" == "mtl - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yc"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

"$(INTDIR)\mtlmain.obj"	"$(INTDIR)\Mtls.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mtl - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yc"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

"$(INTDIR)\mtlmain.obj"	"$(INTDIR)\Mtls.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mtl - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mtls.pch" /Yc"mtlhdr.h" /Fo"$(INTDIR)\\" /Fd"mtl.pdb" /FD /c 

"$(INTDIR)\mtlmain.obj"	"$(INTDIR)\Mtls.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\multi.cpp

"$(INTDIR)\multi.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\noise.cpp

"$(INTDIR)\noise.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\output.cpp

"$(INTDIR)\output.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\partage.cpp

"$(INTDIR)\partage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\partblur.cpp

"$(INTDIR)\partblur.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\plate.cpp

"$(INTDIR)\plate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\rgbmult.cpp

"$(INTDIR)\rgbmult.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\stdmtl.cpp

"$(INTDIR)\stdmtl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\stdmtl2.cpp

"$(INTDIR)\stdmtl2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\stdShaders.cpp

"$(INTDIR)\stdShaders.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\TexMaps.cpp

"$(INTDIR)\TexMaps.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\tint.cpp

"$(INTDIR)\tint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"


SOURCE=.\vertcol.cpp

"$(INTDIR)\vertcol.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Mtls.pch"



!ENDIF 

