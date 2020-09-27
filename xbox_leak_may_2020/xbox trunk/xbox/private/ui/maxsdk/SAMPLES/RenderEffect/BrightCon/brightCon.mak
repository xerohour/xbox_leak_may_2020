# Microsoft Developer Studio Generated NMAKE File, Based on brightCon.dsp
!IF "$(CFG)" == ""
CFG=brightCon - Win32 Release
!MESSAGE No configuration specified. Defaulting to brightCon - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "brightCon - Win32 Release" && "$(CFG)" != "brightCon - Win32 Debug" && "$(CFG)" != "brightCon - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brightCon.mak" CFG="brightCon - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brightCon - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "brightCon - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "brightCon - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "brightCon - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\brightCon.dlv"


CLEAN :
	-@erase "$(INTDIR)\brightCon.obj"
	-@erase "$(INTDIR)\brightCon.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\brightCon.exp"
	-@erase "$(OUTDIR)\brightCon.lib"
	-@erase "..\..\..\..\maxsdk\plugin\brightCon.dlv"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

brightCon=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\brightCon.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\brightCon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075F0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\brightCon.pdb" /machine:I386 /def:".\brightCon.def" /out:"..\..\..\..\maxsdk\plugin\brightCon.dlv" /implib:"$(OUTDIR)\brightCon.lib" /release 
DEF_FILE= \
	".\brightCon.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\brightCon.obj" \
	"$(INTDIR)\brightCon.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\brightCon.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "brightCon - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\brightCon.dlv"


CLEAN :
	-@erase "$(INTDIR)\brightCon.obj"
	-@erase "$(INTDIR)\brightCon.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\brightCon.exp"
	-@erase "$(OUTDIR)\brightCon.lib"
	-@erase "$(OUTDIR)\brightCon.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\brightCon.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\brightCon.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

brightCon=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\brightCon.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\brightCon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075F0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\brightCon.pdb" /debug /machine:I386 /def:".\brightCon.def" /out:"..\..\..\..\maxsdk\plugin\brightCon.dlv" /implib:"$(OUTDIR)\brightCon.lib" 
DEF_FILE= \
	".\brightCon.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\brightCon.obj" \
	"$(INTDIR)\brightCon.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\brightCon.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "brightCon - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\brightCon.dlv"


CLEAN :
	-@erase "$(INTDIR)\brightCon.obj"
	-@erase "$(INTDIR)\brightCon.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\brightCon.exp"
	-@erase "$(OUTDIR)\brightCon.lib"
	-@erase "$(OUTDIR)\brightCon.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\brightCon.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\brightCon.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

brightCon=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\brightCon.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\brightCon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075F0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\brightCon.pdb" /debug /machine:I386 /def:".\brightCon.def" /out:"..\..\..\..\maxsdk\plugin\brightCon.dlv" /implib:"$(OUTDIR)\brightCon.lib" 
DEF_FILE= \
	".\brightCon.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\brightCon.obj" \
	"$(INTDIR)\brightCon.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\brightCon.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("brightCon.dep")
!INCLUDE "brightCon.dep"
!ELSE 
!MESSAGE Warning: cannot find "brightCon.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "brightCon - Win32 Release" || "$(CFG)" == "brightCon - Win32 Debug" || "$(CFG)" == "brightCon - Win32 Hybrid"
SOURCE=.\brightCon.cpp

"$(INTDIR)\brightCon.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\brightCon.rc

"$(INTDIR)\brightCon.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\dllMain.cpp

"$(INTDIR)\dllMain.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

