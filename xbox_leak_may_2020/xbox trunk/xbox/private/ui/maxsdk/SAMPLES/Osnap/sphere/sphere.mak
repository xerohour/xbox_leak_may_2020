# Microsoft Developer Studio Generated NMAKE File, Based on sphere.dsp
!IF "$(CFG)" == ""
CFG=sphere - Win32 Debug
!MESSAGE No configuration specified. Defaulting to sphere - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "sphere - Win32 Release" && "$(CFG)" != "sphere - Win32 Debug"\
 && "$(CFG)" != "sphere - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sphere.mak" CFG="sphere - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sphere - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sphere - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sphere - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "sphere - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\sphere.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\sphere.exp"
	-@erase "$(OUTDIR)\sphere.lib"
	-@erase "..\..\..\..\maxsdk\plugin\sphere.dls"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\include" /I "..\..\..\include"\
 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\sphere.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sphere.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /base:"0x02720000" /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\sphere.pdb" /machine:I386 /def:".\sphere.def"\
 /out:"..\..\..\..\maxsdk\plugin\sphere.dls" /implib:"$(OUTDIR)\sphere.lib"\
 /release 
DEF_FILE= \
	".\sphere.def"
LINK32_OBJS= \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\sphere.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sphere.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sphere - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\sphere.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\sphere.exp"
	-@erase "$(OUTDIR)\sphere.lib"
	-@erase "$(OUTDIR)\sphere.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\sphere.dls"
	-@erase "..\..\..\..\maxsdk\plugin\sphere.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\sphere.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sphere.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /base:"0x02720000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\sphere.pdb" /debug /machine:I386\
 /def:".\sphere.def" /out:"..\..\..\..\maxsdk\plugin\sphere.dls"\
 /implib:"$(OUTDIR)\sphere.lib" /pdbtype:sept 
DEF_FILE= \
	".\sphere.def"
LINK32_OBJS= \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\sphere.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sphere.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sphere - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\sphere.dls"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\sphere.obj"
	-@erase "$(INTDIR)\sphere.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\sphere.exp"
	-@erase "$(OUTDIR)\sphere.lib"
	-@erase "$(OUTDIR)\sphere.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\sphere.dls"
	-@erase "..\..\..\..\maxsdk\plugin\sphere.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\include" /I "..\..\..\include"\
 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\sphere.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sphere.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sphere.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /base:"0x02720000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\sphere.pdb" /debug /machine:I386\
 /def:".\sphere.def" /out:"..\..\..\..\maxsdk\plugin\sphere.dls"\
 /implib:"$(OUTDIR)\sphere.lib" 
DEF_FILE= \
	".\sphere.def"
LINK32_OBJS= \
	"$(INTDIR)\sphere.obj" \
	"$(INTDIR)\sphere.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sphere.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(CFG)" == "sphere - Win32 Release" || "$(CFG)" == "sphere - Win32 Debug"\
 || "$(CFG)" == "sphere - Win32 Hybrid"
SOURCE=.\sphere.cpp
DEP_CPP_SPHER=\
	"..\..\..\include\acolor.h"\
	"..\..\..\include\animtbl.h"\
	"..\..\..\include\appio.h"\
	"..\..\..\include\assert1.h"\
	"..\..\..\include\bitarray.h"\
	"..\..\..\include\box2.h"\
	"..\..\..\include\box3.h"\
	"..\..\..\include\buildver.h"\
	"..\..\..\include\channels.h"\
	"..\..\..\include\cmdmode.h"\
	"..\..\..\include\color.h"\
	"..\..\..\include\control.h"\
	"..\..\..\include\coreexp.h"\
	"..\..\..\include\custcont.h"\
	"..\..\..\include\dbgprint.h"\
	"..\..\..\include\dpoint3.h"\
	"..\..\..\include\euler.h"\
	"..\..\..\include\evuser.h"\
	"..\..\..\include\export.h"\
	"..\..\..\include\gencam.h"\
	"..\..\..\include\genhier.h"\
	"..\..\..\include\genlight.h"\
	"..\..\..\include\genshape.h"\
	"..\..\..\include\geom.h"\
	"..\..\..\include\geomlib.h"\
	"..\..\..\include\gfloat.h"\
	"..\..\..\include\gfx.h"\
	"..\..\..\include\gfxlib.h"\
	"..\..\..\include\gutil.h"\
	"..\..\..\include\hitdata.h"\
	"..\..\..\include\hold.h"\
	"..\..\..\include\impapi.h"\
	"..\..\..\include\impexp.h"\
	"..\..\..\include\imtl.h"\
	"..\..\..\include\inode.h"\
	"..\..\..\include\interval.h"\
	"..\..\..\include\ioapi.h"\
	"..\..\..\include\iparamb.h"\
	"..\..\..\include\ipoint2.h"\
	"..\..\..\include\ipoint3.h"\
	"..\..\..\include\lockid.h"\
	"..\..\..\include\log.h"\
	"..\..\..\include\matrix2.h"\
	"..\..\..\include\matrix3.h"\
	"..\..\..\include\max.h"\
	"..\..\..\include\maxapi.h"\
	"..\..\..\include\maxcom.h"\
	"..\..\..\include\maxtess.h"\
	"..\..\..\include\maxtypes.h"\
	"..\..\..\include\mesh.h"\
	"..\..\..\include\meshlib.h"\
	"..\..\..\include\mouseman.h"\
	"..\..\..\include\mtl.h"\
	"..\..\..\include\namesel.h"\
	"..\..\..\include\nametab.h"\
	"..\..\..\include\object.h"\
	"..\..\..\include\objmode.h"\
	"..\..\..\include\omanapi.h"\
	"..\..\..\include\osnap.h"\
	"..\..\..\include\osnapapi.h"\
	"..\..\..\include\osnaphit.h"\
	"..\..\..\include\osnapmk.h"\
	"..\..\..\include\partclib.h"\
	"..\..\..\include\particle.h"\
	"..\..\..\include\patch.h"\
	"..\..\..\include\patchlib.h"\
	"..\..\..\include\patchobj.h"\
	"..\..\..\include\plugapi.h"\
	"..\..\..\include\plugin.h"\
	"..\..\..\include\point2.h"\
	"..\..\..\include\point3.h"\
	"..\..\..\include\point4.h"\
	"..\..\..\include\ptrvec.h"\
	"..\..\..\include\quat.h"\
	"..\..\..\include\ref.h"\
	"..\..\..\include\render.h"\
	"..\..\..\include\rtclick.h"\
	"..\..\..\include\sceneapi.h"\
	"..\..\..\include\simpobj.h"\
	"..\..\..\include\snap.h"\
	"..\..\..\include\soundobj.h"\
	"..\..\..\include\stack.h"\
	"..\..\..\include\stack3.h"\
	"..\..\..\include\strbasic.h"\
	"..\..\..\include\strclass.h"\
	"..\..\..\include\tab.h"\
	"..\..\..\include\trig.h"\
	"..\..\..\include\triobj.h"\
	"..\..\..\include\units.h"\
	"..\..\..\include\utilexp.h"\
	"..\..\..\include\utillib.h"\
	"..\..\..\include\vedge.h"\
	"..\..\..\include\winutil.h"\
	

"$(INTDIR)\sphere.obj" : $(SOURCE) $(DEP_CPP_SPHER) "$(INTDIR)"


SOURCE=.\sphere.rc
DEP_RSC_SPHERE=\
	".\toolmask.bmp"\
	".\uitools.bmp"\
	

"$(INTDIR)\sphere.res" : $(SOURCE) $(DEP_RSC_SPHERE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

