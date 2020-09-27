# Microsoft Developer Studio Generated NMAKE File, Based on aiexp.dsp
!IF "$(CFG)" == ""
CFG=aiexp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to aiexp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "aiexp - Win32 Release" && "$(CFG)" != "aiexp - Win32 Debug" && "$(CFG)" != "aiexp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "aiexp.mak" CFG="aiexp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "aiexp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "aiexp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "aiexp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "aiexp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\aiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\aiexp.obj"
	-@erase "$(INTDIR)\aiexp.res"
	-@erase "$(OUTDIR)\aiexp.exp"
	-@erase "$(OUTDIR)\aiexp.lib"
	-@erase "..\..\..\maxsdk\plugin\aiexp.dle"
	-@erase ".\aiexp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"aiexp.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\aiexp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\aiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F40000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\aiexp.pdb" /machine:I386 /def:".\aiexp.def" /out:"..\..\..\maxsdk\plugin\aiexp.dle" /implib:"$(OUTDIR)\aiexp.lib" /release 
DEF_FILE= \
	".\aiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\aiexp.obj" \
	"$(INTDIR)\aiexp.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\aiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "aiexp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\aiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\aiexp.obj"
	-@erase "$(INTDIR)\aiexp.res"
	-@erase "$(OUTDIR)\aiexp.exp"
	-@erase "$(OUTDIR)\aiexp.lib"
	-@erase "$(OUTDIR)\aiexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\aiexp.dle"
	-@erase "..\..\..\maxsdk\plugin\aiexp.ilk"
	-@erase ".\aiexp.idb"
	-@erase ".\aiexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"aiexp.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\aiexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\aiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\aiexp.pdb" /debug /machine:I386 /def:".\aiexp.def" /out:"..\..\..\maxsdk\plugin\aiexp.dle" /implib:"$(OUTDIR)\aiexp.lib" /pdbtype:sept 
DEF_FILE= \
	".\aiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\aiexp.obj" \
	"$(INTDIR)\aiexp.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\aiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "aiexp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\aiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\aiexp.obj"
	-@erase "$(INTDIR)\aiexp.res"
	-@erase "$(OUTDIR)\aiexp.exp"
	-@erase "$(OUTDIR)\aiexp.lib"
	-@erase "$(OUTDIR)\aiexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\aiexp.dle"
	-@erase "..\..\..\maxsdk\plugin\aiexp.ilk"
	-@erase ".\aiexp.idb"
	-@erase ".\aiexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"aiexp.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\aiexp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\aiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\aiexp.pdb" /debug /machine:I386 /def:".\aiexp.def" /out:"..\..\..\maxsdk\plugin\aiexp.dle" /implib:"$(OUTDIR)\aiexp.lib" 
DEF_FILE= \
	".\aiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\aiexp.obj" \
	"$(INTDIR)\aiexp.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\aiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("aiexp.dep")
!INCLUDE "aiexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "aiexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "aiexp - Win32 Release" || "$(CFG)" == "aiexp - Win32 Debug" || "$(CFG)" == "aiexp - Win32 Hybrid"
SOURCE=.\aiexp.cpp

"$(INTDIR)\aiexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aiexp.rc

"$(INTDIR)\aiexp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

