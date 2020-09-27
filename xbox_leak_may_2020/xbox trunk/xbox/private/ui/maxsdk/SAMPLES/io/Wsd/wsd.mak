# Microsoft Developer Studio Generated NMAKE File, Based on Wsd.dsp
!IF "$(CFG)" == ""
CFG=wsd - Win32 Release
!MESSAGE No configuration specified. Defaulting to wsd - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "wsd - Win32 Release" && "$(CFG)" != "wsd - Win32 Debug" &&\
 "$(CFG)" != "wsd - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wsd.mak" CFG="wsd - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wsd - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wsd - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wsd - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wsd - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\wsd.obj"
	-@erase "$(INTDIR)\wsd.res"
	-@erase "$(OUTDIR)\wsd.exp"
	-@erase "$(OUTDIR)\wsd.lib"
	-@erase "..\..\..\..\maxsdk\plugin\wsd.bmi"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\Include" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)\Wsd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wsd.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Wsd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /release /base:"0x078B0000" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\wsd.pdb" /machine:I386 /def:".\wsd.def"\
 /out:"..\..\..\..\maxsdk\plugin\wsd.bmi" /implib:"$(OUTDIR)\wsd.lib" 
DEF_FILE= \
	".\wsd.def"
LINK32_OBJS= \
	"$(INTDIR)\wsd.obj" \
	"$(INTDIR)\wsd.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\gcomm.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\wsd.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wsd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wsd.obj"
	-@erase "$(INTDIR)\wsd.res"
	-@erase "$(OUTDIR)\wsd.exp"
	-@erase "$(OUTDIR)\wsd.lib"
	-@erase "$(OUTDIR)\wsd.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\wsd.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\wsd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\..\Include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Wsd.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wsd.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Wsd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x078B0000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\wsd.pdb" /debug /machine:I386 /def:".\wsd.def"\
 /out:"..\..\..\..\maxsdk\plugin\wsd.bmi" /implib:"$(OUTDIR)\wsd.lib" 
DEF_FILE= \
	".\wsd.def"
LINK32_OBJS= \
	"$(INTDIR)\wsd.obj" \
	"$(INTDIR)\wsd.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\gcomm.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\wsd.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wsd - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\wsd.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wsd.obj"
	-@erase "$(INTDIR)\wsd.res"
	-@erase "$(OUTDIR)\wsd.exp"
	-@erase "$(OUTDIR)\wsd.lib"
	-@erase "$(OUTDIR)\wsd.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\wsd.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\wsd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /Zi /Od /I "..\..\..\Include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Wsd.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wsd.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Wsd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x078B0000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\wsd.pdb" /debug /machine:I386 /def:".\wsd.def"\
 /out:"..\..\..\..\maxsdk\plugin\wsd.bmi" /implib:"$(OUTDIR)\wsd.lib" 
DEF_FILE= \
	".\wsd.def"
LINK32_OBJS= \
	"$(INTDIR)\wsd.obj" \
	"$(INTDIR)\wsd.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\gcomm.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\wsd.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "wsd - Win32 Release" || "$(CFG)" == "wsd - Win32 Debug" ||\
 "$(CFG)" == "wsd - Win32 Hybrid"
SOURCE=.\wsd.cpp

!IF  "$(CFG)" == "wsd - Win32 Release"

DEP_CPP_WSD_C=\
	"..\..\..\Include\acolor.h"\
	"..\..\..\Include\animtbl.h"\
	"..\..\..\Include\appio.h"\
	"..\..\..\Include\assert1.h"\
	"..\..\..\Include\bitarray.h"\
	"..\..\..\Include\bitmap.h"\
	"..\..\..\Include\bmmlib.h"\
	"..\..\..\Include\box2.h"\
	"..\..\..\Include\box3.h"\
	"..\..\..\Include\buildver.h"\
	"..\..\..\Include\channels.h"\
	"..\..\..\Include\cmdmode.h"\
	"..\..\..\Include\color.h"\
	"..\..\..\Include\control.h"\
	"..\..\..\Include\coreexp.h"\
	"..\..\..\Include\custcont.h"\
	"..\..\..\Include\dbgprint.h"\
	"..\..\..\Include\dpoint3.h"\
	"..\..\..\Include\euler.h"\
	"..\..\..\Include\evuser.h"\
	"..\..\..\Include\export.h"\
	"..\..\..\Include\gcomm.h"\
	"..\..\..\Include\gcommlib.h"\
	"..\..\..\Include\gencam.h"\
	"..\..\..\Include\genhier.h"\
	"..\..\..\Include\genlight.h"\
	"..\..\..\Include\genshape.h"\
	"..\..\..\Include\geom.h"\
	"..\..\..\Include\geomlib.h"\
	"..\..\..\Include\gfloat.h"\
	"..\..\..\Include\gfx.h"\
	"..\..\..\Include\gfxlib.h"\
	"..\..\..\Include\gutil.h"\
	"..\..\..\Include\hitdata.h"\
	"..\..\..\Include\hold.h"\
	"..\..\..\Include\impapi.h"\
	"..\..\..\Include\impexp.h"\
	"..\..\..\Include\imtl.h"\
	"..\..\..\Include\inode.h"\
	"..\..\..\Include\interval.h"\
	"..\..\..\Include\ioapi.h"\
	"..\..\..\Include\iparamb.h"\
	"..\..\..\Include\ipoint2.h"\
	"..\..\..\Include\ipoint3.h"\
	"..\..\..\Include\linklist.h"\
	"..\..\..\Include\lockid.h"\
	"..\..\..\Include\log.h"\
	"..\..\..\Include\matrix2.h"\
	"..\..\..\Include\matrix3.h"\
	"..\..\..\Include\Max.h"\
	"..\..\..\Include\maxapi.h"\
	"..\..\..\Include\maxcom.h"\
	"..\..\..\Include\maxtess.h"\
	"..\..\..\Include\maxtypes.h"\
	"..\..\..\Include\mesh.h"\
	"..\..\..\Include\meshlib.h"\
	"..\..\..\Include\mouseman.h"\
	"..\..\..\Include\mtl.h"\
	"..\..\..\Include\nametab.h"\
	"..\..\..\Include\object.h"\
	"..\..\..\Include\objmode.h"\
	"..\..\..\Include\palutil.h"\
	"..\..\..\Include\patch.h"\
	"..\..\..\Include\patchlib.h"\
	"..\..\..\Include\patchobj.h"\
	"..\..\..\Include\plugapi.h"\
	"..\..\..\Include\plugin.h"\
	"..\..\..\Include\point2.h"\
	"..\..\..\Include\point3.h"\
	"..\..\..\Include\point4.h"\
	"..\..\..\Include\ptrvec.h"\
	"..\..\..\Include\quat.h"\
	"..\..\..\Include\ref.h"\
	"..\..\..\Include\render.h"\
	"..\..\..\Include\rtclick.h"\
	"..\..\..\Include\sceneapi.h"\
	"..\..\..\Include\snap.h"\
	"..\..\..\Include\soundobj.h"\
	"..\..\..\Include\stack.h"\
	"..\..\..\Include\stack3.h"\
	"..\..\..\Include\strbasic.h"\
	"..\..\..\Include\strclass.h"\
	"..\..\..\Include\tab.h"\
	"..\..\..\Include\tcp.h"\
	"..\..\..\Include\trig.h"\
	"..\..\..\Include\triobj.h"\
	"..\..\..\Include\units.h"\
	"..\..\..\Include\utilexp.h"\
	"..\..\..\Include\utillib.h"\
	"..\..\..\Include\vedge.h"\
	"..\..\..\Include\winutil.h"\
	".\fmtspec.h"\
	".\wsd.h"\
	

"$(INTDIR)\wsd.obj" : $(SOURCE) $(DEP_CPP_WSD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wsd - Win32 Debug"

DEP_CPP_WSD_C=\
	"..\..\..\Include\acolor.h"\
	"..\..\..\Include\animtbl.h"\
	"..\..\..\Include\appio.h"\
	"..\..\..\Include\assert1.h"\
	"..\..\..\Include\bitarray.h"\
	"..\..\..\Include\bitmap.h"\
	"..\..\..\Include\bmmlib.h"\
	"..\..\..\Include\box2.h"\
	"..\..\..\Include\box3.h"\
	"..\..\..\Include\buildver.h"\
	"..\..\..\Include\channels.h"\
	"..\..\..\Include\cmdmode.h"\
	"..\..\..\Include\color.h"\
	"..\..\..\Include\control.h"\
	"..\..\..\Include\coreexp.h"\
	"..\..\..\Include\custcont.h"\
	"..\..\..\Include\dbgprint.h"\
	"..\..\..\Include\dpoint3.h"\
	"..\..\..\Include\euler.h"\
	"..\..\..\Include\evuser.h"\
	"..\..\..\Include\export.h"\
	"..\..\..\Include\gcomm.h"\
	"..\..\..\Include\gcommlib.h"\
	"..\..\..\Include\gencam.h"\
	"..\..\..\Include\genhier.h"\
	"..\..\..\Include\genlight.h"\
	"..\..\..\Include\genshape.h"\
	"..\..\..\Include\geom.h"\
	"..\..\..\Include\geomlib.h"\
	"..\..\..\Include\gfloat.h"\
	"..\..\..\Include\gfx.h"\
	"..\..\..\Include\gfxlib.h"\
	"..\..\..\Include\gutil.h"\
	"..\..\..\Include\hitdata.h"\
	"..\..\..\Include\hold.h"\
	"..\..\..\Include\impapi.h"\
	"..\..\..\Include\impexp.h"\
	"..\..\..\Include\imtl.h"\
	"..\..\..\Include\inode.h"\
	"..\..\..\Include\interval.h"\
	"..\..\..\Include\ioapi.h"\
	"..\..\..\Include\iparamb.h"\
	"..\..\..\Include\ipoint2.h"\
	"..\..\..\Include\ipoint3.h"\
	"..\..\..\Include\linklist.h"\
	"..\..\..\Include\lockid.h"\
	"..\..\..\Include\log.h"\
	"..\..\..\Include\matrix2.h"\
	"..\..\..\Include\matrix3.h"\
	"..\..\..\Include\Max.h"\
	"..\..\..\Include\maxapi.h"\
	"..\..\..\Include\maxcom.h"\
	"..\..\..\Include\maxtess.h"\
	"..\..\..\Include\maxtypes.h"\
	"..\..\..\Include\mesh.h"\
	"..\..\..\Include\meshlib.h"\
	"..\..\..\Include\mouseman.h"\
	"..\..\..\Include\mtl.h"\
	"..\..\..\Include\nametab.h"\
	"..\..\..\Include\object.h"\
	"..\..\..\Include\objmode.h"\
	"..\..\..\Include\palutil.h"\
	"..\..\..\Include\patch.h"\
	"..\..\..\Include\patchlib.h"\
	"..\..\..\Include\patchobj.h"\
	"..\..\..\Include\plugapi.h"\
	"..\..\..\Include\plugin.h"\
	"..\..\..\Include\point2.h"\
	"..\..\..\Include\point3.h"\
	"..\..\..\Include\point4.h"\
	"..\..\..\Include\ptrvec.h"\
	"..\..\..\Include\quat.h"\
	"..\..\..\Include\ref.h"\
	"..\..\..\Include\render.h"\
	"..\..\..\Include\rtclick.h"\
	"..\..\..\Include\sceneapi.h"\
	"..\..\..\Include\snap.h"\
	"..\..\..\Include\soundobj.h"\
	"..\..\..\Include\stack.h"\
	"..\..\..\Include\stack3.h"\
	"..\..\..\Include\strbasic.h"\
	"..\..\..\Include\strclass.h"\
	"..\..\..\Include\tab.h"\
	"..\..\..\Include\tcp.h"\
	"..\..\..\Include\trig.h"\
	"..\..\..\Include\triobj.h"\
	"..\..\..\Include\units.h"\
	"..\..\..\Include\utilexp.h"\
	"..\..\..\Include\utillib.h"\
	"..\..\..\Include\vedge.h"\
	"..\..\..\Include\winutil.h"\
	".\fmtspec.h"\
	".\wsd.h"\
	

"$(INTDIR)\wsd.obj" : $(SOURCE) $(DEP_CPP_WSD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wsd - Win32 Hybrid"

DEP_CPP_WSD_C=\
	"..\..\..\Include\acolor.h"\
	"..\..\..\Include\animtbl.h"\
	"..\..\..\Include\appio.h"\
	"..\..\..\Include\assert1.h"\
	"..\..\..\Include\bitarray.h"\
	"..\..\..\Include\bitmap.h"\
	"..\..\..\Include\bmmlib.h"\
	"..\..\..\Include\box2.h"\
	"..\..\..\Include\box3.h"\
	"..\..\..\Include\buildver.h"\
	"..\..\..\Include\channels.h"\
	"..\..\..\Include\cmdmode.h"\
	"..\..\..\Include\color.h"\
	"..\..\..\Include\control.h"\
	"..\..\..\Include\coreexp.h"\
	"..\..\..\Include\custcont.h"\
	"..\..\..\Include\dbgprint.h"\
	"..\..\..\Include\dpoint3.h"\
	"..\..\..\Include\euler.h"\
	"..\..\..\Include\evuser.h"\
	"..\..\..\Include\export.h"\
	"..\..\..\Include\gcomm.h"\
	"..\..\..\Include\gcommlib.h"\
	"..\..\..\Include\gencam.h"\
	"..\..\..\Include\genhier.h"\
	"..\..\..\Include\genlight.h"\
	"..\..\..\Include\genshape.h"\
	"..\..\..\Include\geom.h"\
	"..\..\..\Include\geomlib.h"\
	"..\..\..\Include\gfloat.h"\
	"..\..\..\Include\gfx.h"\
	"..\..\..\Include\gfxlib.h"\
	"..\..\..\Include\gutil.h"\
	"..\..\..\Include\hitdata.h"\
	"..\..\..\Include\hold.h"\
	"..\..\..\Include\impapi.h"\
	"..\..\..\Include\impexp.h"\
	"..\..\..\Include\imtl.h"\
	"..\..\..\Include\inode.h"\
	"..\..\..\Include\interval.h"\
	"..\..\..\Include\ioapi.h"\
	"..\..\..\Include\iparamb.h"\
	"..\..\..\Include\ipoint2.h"\
	"..\..\..\Include\ipoint3.h"\
	"..\..\..\Include\linklist.h"\
	"..\..\..\Include\lockid.h"\
	"..\..\..\Include\log.h"\
	"..\..\..\Include\matrix2.h"\
	"..\..\..\Include\matrix3.h"\
	"..\..\..\Include\Max.h"\
	"..\..\..\Include\maxapi.h"\
	"..\..\..\Include\maxcom.h"\
	"..\..\..\Include\maxtess.h"\
	"..\..\..\Include\maxtypes.h"\
	"..\..\..\Include\mesh.h"\
	"..\..\..\Include\meshlib.h"\
	"..\..\..\Include\mouseman.h"\
	"..\..\..\Include\mtl.h"\
	"..\..\..\Include\nametab.h"\
	"..\..\..\Include\object.h"\
	"..\..\..\Include\objmode.h"\
	"..\..\..\Include\palutil.h"\
	"..\..\..\Include\patch.h"\
	"..\..\..\Include\patchlib.h"\
	"..\..\..\Include\patchobj.h"\
	"..\..\..\Include\plugapi.h"\
	"..\..\..\Include\plugin.h"\
	"..\..\..\Include\point2.h"\
	"..\..\..\Include\point3.h"\
	"..\..\..\Include\point4.h"\
	"..\..\..\Include\ptrvec.h"\
	"..\..\..\Include\quat.h"\
	"..\..\..\Include\ref.h"\
	"..\..\..\Include\render.h"\
	"..\..\..\Include\rtclick.h"\
	"..\..\..\Include\sceneapi.h"\
	"..\..\..\Include\snap.h"\
	"..\..\..\Include\soundobj.h"\
	"..\..\..\Include\stack.h"\
	"..\..\..\Include\stack3.h"\
	"..\..\..\Include\strbasic.h"\
	"..\..\..\Include\strclass.h"\
	"..\..\..\Include\tab.h"\
	"..\..\..\Include\tcp.h"\
	"..\..\..\Include\trig.h"\
	"..\..\..\Include\triobj.h"\
	"..\..\..\Include\units.h"\
	"..\..\..\Include\utilexp.h"\
	"..\..\..\Include\utillib.h"\
	"..\..\..\Include\vedge.h"\
	"..\..\..\Include\winutil.h"\
	".\fmtspec.h"\
	".\wsd.h"\
	

"$(INTDIR)\wsd.obj" : $(SOURCE) $(DEP_CPP_WSD_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\wsd.rc

"$(INTDIR)\wsd.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

