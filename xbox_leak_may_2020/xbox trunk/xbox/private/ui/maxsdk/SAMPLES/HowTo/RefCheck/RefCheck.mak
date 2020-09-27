# Microsoft Developer Studio Generated NMAKE File, Based on RefCheck.dsp
!IF "$(CFG)" == ""
CFG=RefCheck - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to RefCheck - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "RefCheck - Win32 Release" && "$(CFG)" !=\
 "RefCheck - Win32 Debug" && "$(CFG)" != "RefCheck - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RefCheck.mak" CFG="RefCheck - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RefCheck - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RefCheck - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RefCheck - Win32 Hybrid" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RefCheck - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ELSE 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\RefCheck.obj"
	-@erase "$(INTDIR)\RefCheck.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\RefCheck.exp"
	-@erase "$(OUTDIR)\RefCheck.lib"
	-@erase "..\..\..\plugin\RefCheck.dlu"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32"\
 /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RefCheck.pch" /YX /Fo"$(INTDIR)\\"\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RefCheck.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RefCheck.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /release /base:"0x08920000" /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\RefCheck.pdb" /machine:I386\
 /def:".\RefCheck.def" /out:"..\..\..\..\maxsdk\plugin\RefCheck.dlu"\
 /implib:"$(OUTDIR)\RefCheck.lib" 
DEF_FILE= \
	".\RefCheck.def"
LINK32_OBJS= \
	"$(INTDIR)\RefCheck.obj" \
	"$(INTDIR)\RefCheck.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\RefCheck.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RefCheck - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ELSE 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\RefCheck.obj"
	-@erase "$(INTDIR)\RefCheck.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\RefCheck.exp"
	-@erase "$(OUTDIR)\RefCheck.lib"
	-@erase "$(OUTDIR)\RefCheck.pdb"
	-@erase "..\..\..\plugin\RefCheck.dlu"
	-@erase "..\..\..\plugin\RefCheck.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\maxsdk\include"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RefCheck.pch" /YX\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RefCheck.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RefCheck.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /base:"0x08920000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\RefCheck.pdb" /debug /machine:I386\
 /def:".\RefCheck.def" /out:"..\..\..\..\maxsdk\plugin\RefCheck.dlu"\
 /implib:"$(OUTDIR)\RefCheck.lib" /pdbtype:sept 
DEF_FILE= \
	".\RefCheck.def"
LINK32_OBJS= \
	"$(INTDIR)\RefCheck.obj" \
	"$(INTDIR)\RefCheck.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\RefCheck.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RefCheck - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ELSE 

ALL : "..\..\..\plugin\RefCheck.dlu"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\RefCheck.obj"
	-@erase "$(INTDIR)\RefCheck.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\RefCheck.exp"
	-@erase "$(OUTDIR)\RefCheck.lib"
	-@erase "$(OUTDIR)\RefCheck.pdb"
	-@erase "..\..\..\plugin\RefCheck.dlu"
	-@erase "..\..\..\plugin\RefCheck.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "..\..\..\..\maxsdk\include" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RefCheck.pch" /YX\
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RefCheck.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RefCheck.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /base:"0x08920000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\RefCheck.pdb" /debug /machine:I386\
 /def:".\RefCheck.def" /out:"..\..\..\..\maxsdk\plugin\RefCheck.dlu"\
 /implib:"$(OUTDIR)\RefCheck.lib" /pdbtype:sept 
DEF_FILE= \
	".\RefCheck.def"
LINK32_OBJS= \
	"$(INTDIR)\RefCheck.obj" \
	"$(INTDIR)\RefCheck.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\plugin\RefCheck.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "RefCheck - Win32 Release" || "$(CFG)" ==\
 "RefCheck - Win32 Debug" || "$(CFG)" == "RefCheck - Win32 Hybrid"
SOURCE=.\RefCheck.cpp
DEP_CPP_REFCH=\
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
	"..\..\..\include\MAX.H"\
	"..\..\..\include\maxapi.h"\
	"..\..\..\include\maxcom.h"\
	"..\..\..\include\maxtess.h"\
	"..\..\..\include\maxtypes.h"\
	"..\..\..\include\mesh.h"\
	"..\..\..\include\meshlib.h"\
	"..\..\..\include\mouseman.h"\
	"..\..\..\include\mtl.h"\
	"..\..\..\include\nametab.h"\
	"..\..\..\include\object.h"\
	"..\..\..\include\objmode.h"\
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
	"..\..\..\include\utilapi.h"\
	"..\..\..\include\utilexp.h"\
	"..\..\..\include\utillib.h"\
	"..\..\..\include\vedge.h"\
	"..\..\..\include\winutil.h"\
	

"$(INTDIR)\RefCheck.obj" : $(SOURCE) $(DEP_CPP_REFCH) "$(INTDIR)"


SOURCE=.\RefCheck.rc

"$(INTDIR)\RefCheck.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

