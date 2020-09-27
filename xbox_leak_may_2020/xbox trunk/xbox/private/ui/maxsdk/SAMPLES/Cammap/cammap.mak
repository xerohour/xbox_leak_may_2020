# Microsoft Developer Studio Generated NMAKE File, Based on cammap.dsp
!IF "$(CFG)" == ""
CFG=cammap - Win32 Release
!MESSAGE No configuration specified. Defaulting to cammap - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "cammap - Win32 Release" && "$(CFG)" != "cammap - Win32 Debug" && "$(CFG)" != "cammap - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cammap.mak" CFG="cammap - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cammap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cammap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cammap - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cammap - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\cammap.dlm"


CLEAN :
	-@erase "$(INTDIR)\cammap.obj"
	-@erase "$(INTDIR)\cammap.res"
	-@erase "$(OUTDIR)\cammap.exp"
	-@erase "$(OUTDIR)\cammap.lib"
	-@erase "..\..\..\maxsdk\plugin\cammap.dlm"
	-@erase ".\cammap.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\cammap.pch" /YX /Fo"$(INTDIR)\\" /Fd"cammap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cammap.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cammap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05760000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cammap.pdb" /machine:I386 /def:".\cammap.def" /out:"..\..\..\maxsdk\plugin\cammap.dlm" /implib:"$(OUTDIR)\cammap.lib" /release 
DEF_FILE= \
	".\cammap.def"
LINK32_OBJS= \
	"$(INTDIR)\cammap.obj" \
	"$(INTDIR)\cammap.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cammap.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cammap - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\cammap.dlm"


CLEAN :
	-@erase "$(INTDIR)\cammap.obj"
	-@erase "$(INTDIR)\cammap.res"
	-@erase "$(OUTDIR)\cammap.exp"
	-@erase "$(OUTDIR)\cammap.lib"
	-@erase "$(OUTDIR)\cammap.pdb"
	-@erase "..\..\..\maxsdk\plugin\cammap.dlm"
	-@erase "..\..\..\maxsdk\plugin\cammap.ilk"
	-@erase ".\cammap.idb"
	-@erase ".\cammap.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\cammap.pch" /YX /Fo"$(INTDIR)\\" /Fd"cammap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cammap.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cammap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05760000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cammap.pdb" /debug /machine:I386 /def:".\cammap.def" /out:"..\..\..\maxsdk\plugin\cammap.dlm" /implib:"$(OUTDIR)\cammap.lib" 
DEF_FILE= \
	".\cammap.def"
LINK32_OBJS= \
	"$(INTDIR)\cammap.obj" \
	"$(INTDIR)\cammap.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cammap.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cammap - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\cammap.dlm"


CLEAN :
	-@erase "$(INTDIR)\cammap.obj"
	-@erase "$(INTDIR)\cammap.res"
	-@erase "$(OUTDIR)\cammap.exp"
	-@erase "$(OUTDIR)\cammap.lib"
	-@erase "$(OUTDIR)\cammap.pdb"
	-@erase "..\..\..\maxsdk\plugin\cammap.dlm"
	-@erase "..\..\..\maxsdk\plugin\cammap.ilk"
	-@erase ".\cammap.idb"
	-@erase ".\cammap.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\cammap.pch" /YX /Fo"$(INTDIR)\\" /Fd"cammap.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cammap.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cammap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05760000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cammap.pdb" /debug /machine:I386 /def:".\cammap.def" /out:"..\..\..\maxsdk\plugin\cammap.dlm" /implib:"$(OUTDIR)\cammap.lib" 
DEF_FILE= \
	".\cammap.def"
LINK32_OBJS= \
	"$(INTDIR)\cammap.obj" \
	"$(INTDIR)\cammap.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\cammap.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cammap.dep")
!INCLUDE "cammap.dep"
!ELSE 
!MESSAGE Warning: cannot find "cammap.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cammap - Win32 Release" || "$(CFG)" == "cammap - Win32 Debug" || "$(CFG)" == "cammap - Win32 Hybrid"
SOURCE=.\cammap.cpp

"$(INTDIR)\cammap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cammap.rc

"$(INTDIR)\cammap.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

