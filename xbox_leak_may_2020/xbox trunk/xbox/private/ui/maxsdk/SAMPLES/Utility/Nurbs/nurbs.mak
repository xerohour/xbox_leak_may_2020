# Microsoft Developer Studio Generated NMAKE File, Based on nurbs.dsp
!IF "$(CFG)" == ""
CFG=nurbs - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to nurbs - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "nurbs - Win32 Release" && "$(CFG)" != "nurbs - Win32 Debug" && "$(CFG)" != "nurbs - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nurbs.mak" CFG="nurbs - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nurbs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nurbs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nurbs - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "nurbs - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\nutil.dlu"


CLEAN :
	-@erase "$(INTDIR)\crt_crv.obj"
	-@erase "$(INTDIR)\crt_pnt.obj"
	-@erase "$(INTDIR)\crt_srf.obj"
	-@erase "$(INTDIR)\nutil.obj"
	-@erase "$(INTDIR)\nutil.res"
	-@erase "$(INTDIR)\srf_aprx.obj"
	-@erase "$(INTDIR)\srf_dump.obj"
	-@erase "$(INTDIR)\srf_test.obj"
	-@erase "$(OUTDIR)\nutil.exp"
	-@erase "$(OUTDIR)\nutil.lib"
	-@erase "..\..\..\..\maxsdk\plugin\nutil.dlu"
	-@erase ".\nurbs.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"nurbs.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nutil.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nurbs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib edmodel.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x04F40000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\nutil.pdb" /machine:I386 /def:".\nutil.def" /out:"..\..\..\..\maxsdk\plugin\nutil.dlu" /implib:"$(OUTDIR)\nutil.lib" /libpath:"..\..\..\lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\nutil.def"
LINK32_OBJS= \
	"$(INTDIR)\crt_crv.obj" \
	"$(INTDIR)\crt_pnt.obj" \
	"$(INTDIR)\crt_srf.obj" \
	"$(INTDIR)\nutil.obj" \
	"$(INTDIR)\srf_aprx.obj" \
	"$(INTDIR)\srf_dump.obj" \
	"$(INTDIR)\srf_test.obj" \
	"$(INTDIR)\nutil.res"

"..\..\..\..\maxsdk\plugin\nutil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nurbs - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\nutil.dlu"


CLEAN :
	-@erase "$(INTDIR)\crt_crv.obj"
	-@erase "$(INTDIR)\crt_pnt.obj"
	-@erase "$(INTDIR)\crt_srf.obj"
	-@erase "$(INTDIR)\nutil.obj"
	-@erase "$(INTDIR)\nutil.res"
	-@erase "$(INTDIR)\srf_aprx.obj"
	-@erase "$(INTDIR)\srf_dump.obj"
	-@erase "$(INTDIR)\srf_test.obj"
	-@erase "$(OUTDIR)\nutil.exp"
	-@erase "$(OUTDIR)\nutil.lib"
	-@erase "$(OUTDIR)\nutil.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\nutil.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\nutil.ilk"
	-@erase ".\nurbs.idb"
	-@erase ".\nurbs.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"nurbs.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nutil.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nurbs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib edmodel.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x04F40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\nutil.pdb" /debug /machine:I386 /def:".\nutil.def" /out:"..\..\..\..\maxsdk\plugin\nutil.dlu" /implib:"$(OUTDIR)\nutil.lib" /pdbtype:sept /libpath:"..\..\..\lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\nutil.def"
LINK32_OBJS= \
	"$(INTDIR)\crt_crv.obj" \
	"$(INTDIR)\crt_pnt.obj" \
	"$(INTDIR)\crt_srf.obj" \
	"$(INTDIR)\nutil.obj" \
	"$(INTDIR)\srf_aprx.obj" \
	"$(INTDIR)\srf_dump.obj" \
	"$(INTDIR)\srf_test.obj" \
	"$(INTDIR)\nutil.res"

"..\..\..\..\maxsdk\plugin\nutil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nurbs - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\nutil.dlu"


CLEAN :
	-@erase "$(INTDIR)\crt_crv.obj"
	-@erase "$(INTDIR)\crt_pnt.obj"
	-@erase "$(INTDIR)\crt_srf.obj"
	-@erase "$(INTDIR)\nutil.obj"
	-@erase "$(INTDIR)\nutil.res"
	-@erase "$(INTDIR)\srf_aprx.obj"
	-@erase "$(INTDIR)\srf_dump.obj"
	-@erase "$(INTDIR)\srf_test.obj"
	-@erase "$(OUTDIR)\nutil.exp"
	-@erase "$(OUTDIR)\nutil.lib"
	-@erase "$(OUTDIR)\nutil.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\nutil.dlu"
	-@erase "..\..\..\..\maxsdk\plugin\nutil.ilk"
	-@erase ".\nurbs.idb"
	-@erase ".\nurbs.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"nurbs.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nutil.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nurbs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib edmodel.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x04F40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\nutil.pdb" /debug /machine:I386 /def:".\nutil.def" /out:"..\..\..\..\maxsdk\plugin\nutil.dlu" /implib:"$(OUTDIR)\nutil.lib" /pdbtype:sept /libpath:"..\..\..\lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\nutil.def"
LINK32_OBJS= \
	"$(INTDIR)\crt_crv.obj" \
	"$(INTDIR)\crt_pnt.obj" \
	"$(INTDIR)\crt_srf.obj" \
	"$(INTDIR)\nutil.obj" \
	"$(INTDIR)\srf_aprx.obj" \
	"$(INTDIR)\srf_dump.obj" \
	"$(INTDIR)\srf_test.obj" \
	"$(INTDIR)\nutil.res"

"..\..\..\..\maxsdk\plugin\nutil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("nurbs.dep")
!INCLUDE "nurbs.dep"
!ELSE 
!MESSAGE Warning: cannot find "nurbs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "nurbs - Win32 Release" || "$(CFG)" == "nurbs - Win32 Debug" || "$(CFG)" == "nurbs - Win32 Hybrid"
SOURCE=.\crt_crv.cpp

"$(INTDIR)\crt_crv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\crt_pnt.cpp

"$(INTDIR)\crt_pnt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\crt_srf.cpp

"$(INTDIR)\crt_srf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nutil.cpp

"$(INTDIR)\nutil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nutil.rc

"$(INTDIR)\nutil.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\srf_aprx.cpp

"$(INTDIR)\srf_aprx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\srf_dump.cpp

"$(INTDIR)\srf_dump.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\srf_test.cpp

"$(INTDIR)\srf_test.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

