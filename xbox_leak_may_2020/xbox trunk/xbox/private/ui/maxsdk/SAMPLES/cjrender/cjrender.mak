# Microsoft Developer Studio Generated NMAKE File, Based on cjrender.dsp
!IF "$(CFG)" == ""
CFG=cjrender - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to cjrender - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "cjrender - Win32 Release" && "$(CFG)" !=\
 "cjrender - Win32 Debug" && "$(CFG)" != "cjrender - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cjrender.mak" CFG="cjrender - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cjrender - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cjrender - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cjrender - Win32 Hybrid" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cjrender - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ELSE 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\cjrender.obj"
	-@erase "$(INTDIR)\cjrender.res"
	-@erase "$(INTDIR)\cjrmain.obj"
	-@erase "$(INTDIR)\cjrui.obj"
	-@erase "$(INTDIR)\dummtl.obj"
	-@erase "$(INTDIR)\instance.obj"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\raytrace.obj"
	-@erase "$(INTDIR)\refenum.obj"
	-@erase "$(INTDIR)\rendutil.obj"
	-@erase "$(INTDIR)\scontext.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\cjrender.exp"
	-@erase "$(OUTDIR)\cjrender.lib"
	-@erase "..\..\..\maxsdk\plugin\cjrender.dlr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)\cjrender.pch" /YX"maxincl.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cjrender.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cjrender.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /release /base:"0x086E0000" /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\cjrender.pdb" /machine:I386\
 /def:".\cjrender.def" /out:"..\..\..\maxsdk\plugin\cjrender.dlr"\
 /implib:"$(OUTDIR)\cjrender.lib" 
DEF_FILE= \
	".\cjrender.def"
LINK32_OBJS= \
	"$(INTDIR)\cjrender.obj" \
	"$(INTDIR)\cjrender.res" \
	"$(INTDIR)\cjrmain.obj" \
	"$(INTDIR)\cjrui.obj" \
	"$(INTDIR)\dummtl.obj" \
	"$(INTDIR)\instance.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\raytrace.obj" \
	"$(INTDIR)\refenum.obj" \
	"$(INTDIR)\rendutil.obj" \
	"$(INTDIR)\scontext.obj" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cjrender.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cjrender - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ELSE 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\cjrender.obj"
	-@erase "$(INTDIR)\cjrender.res"
	-@erase "$(INTDIR)\cjrmain.obj"
	-@erase "$(INTDIR)\cjrui.obj"
	-@erase "$(INTDIR)\dummtl.obj"
	-@erase "$(INTDIR)\instance.obj"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\raytrace.obj"
	-@erase "$(INTDIR)\refenum.obj"
	-@erase "$(INTDIR)\rendutil.obj"
	-@erase "$(INTDIR)\scontext.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\cjrender.exp"
	-@erase "$(OUTDIR)\cjrender.lib"
	-@erase "..\..\..\maxsdk\plugin\cjrender.dlr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cjrender.pch" /YX"maxincl.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cjrender.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cjrender.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /base:"0x086E0000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\cjrender.pdb" /debug /machine:I386\
 /def:".\cjrender.def" /out:"..\..\..\maxsdk\plugin\cjrender.dlr"\
 /implib:"$(OUTDIR)\cjrender.lib" 
DEF_FILE= \
	".\cjrender.def"
LINK32_OBJS= \
	"$(INTDIR)\cjrender.obj" \
	"$(INTDIR)\cjrender.res" \
	"$(INTDIR)\cjrmain.obj" \
	"$(INTDIR)\cjrui.obj" \
	"$(INTDIR)\dummtl.obj" \
	"$(INTDIR)\instance.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\raytrace.obj" \
	"$(INTDIR)\refenum.obj" \
	"$(INTDIR)\rendutil.obj" \
	"$(INTDIR)\scontext.obj" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cjrender.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cjrender - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ELSE 

ALL : "..\..\..\maxsdk\plugin\cjrender.dlr"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\cjrender.obj"
	-@erase "$(INTDIR)\cjrender.res"
	-@erase "$(INTDIR)\cjrmain.obj"
	-@erase "$(INTDIR)\cjrui.obj"
	-@erase "$(INTDIR)\dummtl.obj"
	-@erase "$(INTDIR)\instance.obj"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\raytrace.obj"
	-@erase "$(INTDIR)\refenum.obj"
	-@erase "$(INTDIR)\rendutil.obj"
	-@erase "$(INTDIR)\scontext.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\cjrender.exp"
	-@erase "$(OUTDIR)\cjrender.lib"
	-@erase "$(OUTDIR)\cjrender.pdb"
	-@erase "..\..\..\maxsdk\plugin\cjrender.dlr"
	-@erase "..\..\..\maxsdk\plugin\cjrender.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /Zi /Od /I "..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cjrender.pch" /YX"maxincl.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cjrender.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cjrender.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /base:"0x086E0000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\cjrender.pdb" /debug /machine:I386\
 /def:".\cjrender.def" /out:"..\..\..\maxsdk\plugin\cjrender.dlr"\
 /implib:"$(OUTDIR)\cjrender.lib" /pdbtype:sept 
DEF_FILE= \
	".\cjrender.def"
LINK32_OBJS= \
	"$(INTDIR)\cjrender.obj" \
	"$(INTDIR)\cjrender.res" \
	"$(INTDIR)\cjrmain.obj" \
	"$(INTDIR)\cjrui.obj" \
	"$(INTDIR)\dummtl.obj" \
	"$(INTDIR)\instance.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\raytrace.obj" \
	"$(INTDIR)\refenum.obj" \
	"$(INTDIR)\rendutil.obj" \
	"$(INTDIR)\scontext.obj" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cjrender.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "cjrender - Win32 Release" || "$(CFG)" ==\
 "cjrender - Win32 Debug" || "$(CFG)" == "cjrender - Win32 Hybrid"
SOURCE=.\cjrender.cpp
DEP_CPP_CJREN=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\refenum.h"\
	".\rendutil.h"\
	

"$(INTDIR)\cjrender.obj" : $(SOURCE) $(DEP_CPP_CJREN) "$(INTDIR)"


SOURCE=.\cjrmain.cpp
DEP_CPP_CJRMA=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	

"$(INTDIR)\cjrmain.obj" : $(SOURCE) $(DEP_CPP_CJRMA) "$(INTDIR)"


SOURCE=.\cjrui.cpp
DEP_CPP_CJRUI=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	

"$(INTDIR)\cjrui.obj" : $(SOURCE) $(DEP_CPP_CJRUI) "$(INTDIR)"


SOURCE=.\dummtl.cpp
DEP_CPP_DUMMT=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	

"$(INTDIR)\dummtl.obj" : $(SOURCE) $(DEP_CPP_DUMMT) "$(INTDIR)"


SOURCE=.\instance.cpp
DEP_CPP_INSTA=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\refenum.h"\
	".\rendutil.h"\
	

"$(INTDIR)\instance.obj" : $(SOURCE) $(DEP_CPP_INSTA) "$(INTDIR)"


SOURCE=.\lights.cpp
DEP_CPP_LIGHT=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\refenum.h"\
	".\rendutil.h"\
	

"$(INTDIR)\lights.obj" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


SOURCE=.\raytrace.cpp
DEP_CPP_RAYTR=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\rendutil.h"\
	".\scontext.h"\
	

"$(INTDIR)\raytrace.obj" : $(SOURCE) $(DEP_CPP_RAYTR) "$(INTDIR)"


SOURCE=.\refenum.cpp
DEP_CPP_REFEN=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\refenum.h"\
	

"$(INTDIR)\refenum.obj" : $(SOURCE) $(DEP_CPP_REFEN) "$(INTDIR)"


SOURCE=.\rendutil.cpp
DEP_CPP_RENDU=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\rendutil.h"\
	

"$(INTDIR)\rendutil.obj" : $(SOURCE) $(DEP_CPP_RENDU) "$(INTDIR)"


SOURCE=.\scontext.cpp
DEP_CPP_SCONT=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\stdmat.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\templt.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\cjrender.h"\
	".\maxincl.h"\
	".\rendutil.h"\
	".\scontext.h"\
	

"$(INTDIR)\scontext.obj" : $(SOURCE) $(DEP_CPP_SCONT) "$(INTDIR)"


SOURCE=.\cjrender.rc

"$(INTDIR)\cjrender.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

