# Microsoft Developer Studio Generated NMAKE File, Based on colorBalance.dsp
!IF "$(CFG)" == ""
CFG=colorBalance - Win32 Release
!MESSAGE No configuration specified. Defaulting to colorBalance - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "colorBalance - Win32 Release" && "$(CFG)" != "colorBalance - Win32 Debug" && "$(CFG)" != "colorBalance - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "colorBalance.mak" CFG="colorBalance - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "colorBalance - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "colorBalance - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "colorBalance - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "colorBalance - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\colorBalance.dlv"


CLEAN :
	-@erase "$(INTDIR)\colorBalance.obj"
	-@erase "$(INTDIR)\colorBalance.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(OUTDIR)\colorBalance.exp"
	-@erase "$(OUTDIR)\colorBalance.lib"
	-@erase "..\..\..\..\maxsdk\plugin\colorBalance.dlv"
	-@erase ".\colorBalance.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

colorBalance=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"colorBalance.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\colorBalance.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\colorBalance.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07610000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\colorBalance.pdb" /machine:I386 /def:".\colorBalance.def" /out:"..\..\..\..\maxsdk\plugin\colorBalance.dlv" /implib:"$(OUTDIR)\colorBalance.lib" /release 
DEF_FILE= \
	".\colorBalance.def"
LINK32_OBJS= \
	"$(INTDIR)\colorBalance.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\colorBalance.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\colorBalance.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "colorBalance - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\colorBalance.dlv"


CLEAN :
	-@erase "$(INTDIR)\colorBalance.obj"
	-@erase "$(INTDIR)\colorBalance.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(OUTDIR)\colorBalance.exp"
	-@erase "$(OUTDIR)\colorBalance.lib"
	-@erase "$(OUTDIR)\colorBalance.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\colorBalance.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\colorBalance.ilk"
	-@erase ".\colorBalance.idb"
	-@erase ".\colorBalance.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

colorBalance=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"colorBalance.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\colorBalance.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\colorBalance.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07610000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\colorBalance.pdb" /debug /machine:I386 /def:".\colorBalance.def" /out:"..\..\..\..\maxsdk\plugin\colorBalance.dlv" /implib:"$(OUTDIR)\colorBalance.lib" 
DEF_FILE= \
	".\colorBalance.def"
LINK32_OBJS= \
	"$(INTDIR)\colorBalance.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\colorBalance.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\colorBalance.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "colorBalance - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\colorBalance.dlv"


CLEAN :
	-@erase "$(INTDIR)\colorBalance.obj"
	-@erase "$(INTDIR)\colorBalance.res"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(OUTDIR)\colorBalance.exp"
	-@erase "$(OUTDIR)\colorBalance.lib"
	-@erase "$(OUTDIR)\colorBalance.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\colorBalance.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\colorBalance.ilk"
	-@erase ".\colorBalance.idb"
	-@erase ".\colorBalance.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

colorBalance=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"colorBalance.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\colorBalance.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\colorBalance.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07610000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\colorBalance.pdb" /debug /machine:I386 /def:".\colorBalance.def" /out:"..\..\..\..\maxsdk\plugin\colorBalance.dlv" /implib:"$(OUTDIR)\colorBalance.lib" 
DEF_FILE= \
	".\colorBalance.def"
LINK32_OBJS= \
	"$(INTDIR)\colorBalance.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\colorBalance.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\colorBalance.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("colorBalance.dep")
!INCLUDE "colorBalance.dep"
!ELSE 
!MESSAGE Warning: cannot find "colorBalance.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "colorBalance - Win32 Release" || "$(CFG)" == "colorBalance - Win32 Debug" || "$(CFG)" == "colorBalance - Win32 Hybrid"
SOURCE=.\colorBalance.cpp

"$(INTDIR)\colorBalance.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\colorBalance.rc

"$(INTDIR)\colorBalance.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\dllMain.cpp

"$(INTDIR)\dllMain.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

