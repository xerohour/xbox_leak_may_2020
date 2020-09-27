# Microsoft Developer Studio Generated NMAKE File, Based on asciiexp.dsp
!IF "$(CFG)" == ""
CFG=asciiexp - Win32 Release
!MESSAGE No configuration specified. Defaulting to asciiexp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "asciiexp - Win32 Release" && "$(CFG)" != "asciiexp - Win32 Debug" && "$(CFG)" != "asciiexp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "asciiexp.mak" CFG="asciiexp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "asciiexp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "asciiexp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "asciiexp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "asciiexp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\asciiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\animout.obj"
	-@erase "$(INTDIR)\asciiexp.obj"
	-@erase "$(INTDIR)\asciiexp.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\asciiexp.exp"
	-@erase "$(OUTDIR)\asciiexp.lib"
	-@erase "..\..\..\..\maxsdk\plugin\asciiexp.dle"
	-@erase ".\asciiexp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"asciiexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\asciiexp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\asciiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02F60000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\asciiexp.pdb" /machine:I386 /def:".\asciiexp.def" /out:"..\..\..\..\maxsdk\plugin\asciiexp.dle" /implib:"$(OUTDIR)\asciiexp.lib" /release 
DEF_FILE= \
	".\asciiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\animout.obj" \
	"$(INTDIR)\asciiexp.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\asciiexp.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\asciiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "asciiexp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\asciiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\animout.obj"
	-@erase "$(INTDIR)\asciiexp.obj"
	-@erase "$(INTDIR)\asciiexp.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\asciiexp.exp"
	-@erase "$(OUTDIR)\asciiexp.lib"
	-@erase "$(OUTDIR)\asciiexp.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\asciiexp.dle"
	-@erase "..\..\..\..\maxsdk\plugin\asciiexp.ilk"
	-@erase ".\asciiexp.idb"
	-@erase ".\asciiexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"asciiexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\asciiexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\asciiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02F60000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\asciiexp.pdb" /debug /machine:I386 /def:".\asciiexp.def" /out:"..\..\..\..\maxsdk\plugin\asciiexp.dle" /implib:"$(OUTDIR)\asciiexp.lib" 
DEF_FILE= \
	".\asciiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\animout.obj" \
	"$(INTDIR)\asciiexp.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\asciiexp.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\asciiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "asciiexp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\asciiexp.dle"


CLEAN :
	-@erase "$(INTDIR)\animout.obj"
	-@erase "$(INTDIR)\asciiexp.obj"
	-@erase "$(INTDIR)\asciiexp.res"
	-@erase "$(INTDIR)\export.obj"
	-@erase "$(OUTDIR)\asciiexp.exp"
	-@erase "$(OUTDIR)\asciiexp.lib"
	-@erase "$(OUTDIR)\asciiexp.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\asciiexp.dle"
	-@erase "..\..\..\..\maxsdk\plugin\asciiexp.ilk"
	-@erase ".\asciiexp.idb"
	-@erase ".\asciiexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"asciiexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\asciiexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\asciiexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02F60000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\asciiexp.pdb" /debug /machine:I386 /def:".\asciiexp.def" /out:"..\..\..\..\maxsdk\plugin\asciiexp.dle" /implib:"$(OUTDIR)\asciiexp.lib" 
DEF_FILE= \
	".\asciiexp.def"
LINK32_OBJS= \
	"$(INTDIR)\animout.obj" \
	"$(INTDIR)\asciiexp.obj" \
	"$(INTDIR)\export.obj" \
	"$(INTDIR)\asciiexp.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\asciiexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("asciiexp.dep")
!INCLUDE "asciiexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "asciiexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "asciiexp - Win32 Release" || "$(CFG)" == "asciiexp - Win32 Debug" || "$(CFG)" == "asciiexp - Win32 Hybrid"
SOURCE=.\animout.cpp

"$(INTDIR)\animout.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\asciiexp.cpp

"$(INTDIR)\asciiexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\export.cpp

"$(INTDIR)\export.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\asciiexp.rc

"$(INTDIR)\asciiexp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

