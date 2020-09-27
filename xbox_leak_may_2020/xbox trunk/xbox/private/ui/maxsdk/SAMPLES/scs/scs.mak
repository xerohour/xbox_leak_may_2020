# Microsoft Developer Studio Generated NMAKE File, Based on scs.dsp
!IF "$(CFG)" == ""
CFG=scs - Win32 Release
!MESSAGE No configuration specified. Defaulting to scs - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "scs - Win32 Release" && "$(CFG)" != "scs - Win32 Debug" && "$(CFG)" != "scs - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scs.mak" CFG="scs - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "scs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "scs - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "scs - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\scs.dlo"


CLEAN :
	-@erase "$(INTDIR)\cext.obj"
	-@erase "$(INTDIR)\chbox.obj"
	-@erase "$(INTDIR)\CHCYL.obj"
	-@erase "$(INTDIR)\damper.obj"
	-@erase "$(INTDIR)\gengon.obj"
	-@erase "$(INTDIR)\hose.obj"
	-@erase "$(INTDIR)\lext.obj"
	-@erase "$(INTDIR)\oiltnk.obj"
	-@erase "$(INTDIR)\prism.obj"
	-@erase "$(INTDIR)\pyramid.obj"
	-@erase "$(INTDIR)\scs.res"
	-@erase "$(INTDIR)\scuba.obj"
	-@erase "$(INTDIR)\solids.obj"
	-@erase "$(INTDIR)\spind.obj"
	-@erase "$(INTDIR)\spring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\scs.exp"
	-@erase "$(OUTDIR)\scs.lib"
	-@erase "..\..\..\maxsdk\plugin\scs.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\scs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scs.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib delayimp.lib /nologo /base:"0x054D0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\scs.pdb" /machine:I386 /def:".\scs.def" /out:"..\..\..\maxsdk\plugin\scs.dlo" /implib:"$(OUTDIR)\scs.lib" /release /delayload:"edmodel.dll" 
DEF_FILE= \
	".\scs.def"
LINK32_OBJS= \
	"$(INTDIR)\cext.obj" \
	"$(INTDIR)\chbox.obj" \
	"$(INTDIR)\CHCYL.obj" \
	"$(INTDIR)\damper.obj" \
	"$(INTDIR)\gengon.obj" \
	"$(INTDIR)\hose.obj" \
	"$(INTDIR)\lext.obj" \
	"$(INTDIR)\oiltnk.obj" \
	"$(INTDIR)\prism.obj" \
	"$(INTDIR)\pyramid.obj" \
	"$(INTDIR)\scuba.obj" \
	"$(INTDIR)\solids.obj" \
	"$(INTDIR)\spind.obj" \
	"$(INTDIR)\spring.obj" \
	"$(INTDIR)\scs.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\scs.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "scs - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\scs.dlo"


CLEAN :
	-@erase "$(INTDIR)\cext.obj"
	-@erase "$(INTDIR)\chbox.obj"
	-@erase "$(INTDIR)\CHCYL.obj"
	-@erase "$(INTDIR)\damper.obj"
	-@erase "$(INTDIR)\gengon.obj"
	-@erase "$(INTDIR)\hose.obj"
	-@erase "$(INTDIR)\lext.obj"
	-@erase "$(INTDIR)\oiltnk.obj"
	-@erase "$(INTDIR)\prism.obj"
	-@erase "$(INTDIR)\pyramid.obj"
	-@erase "$(INTDIR)\scs.res"
	-@erase "$(INTDIR)\scuba.obj"
	-@erase "$(INTDIR)\solids.obj"
	-@erase "$(INTDIR)\spind.obj"
	-@erase "$(INTDIR)\spring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\scs.exp"
	-@erase "$(OUTDIR)\scs.lib"
	-@erase "$(OUTDIR)\scs.pdb"
	-@erase "..\..\..\maxsdk\plugin\scs.dlo"
	-@erase "..\..\..\maxsdk\plugin\scs.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\scs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x054D0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\scs.pdb" /debug /machine:I386 /def:".\scs.def" /out:"..\..\..\maxsdk\plugin\scs.dlo" /implib:"$(OUTDIR)\scs.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\scs.def"
LINK32_OBJS= \
	"$(INTDIR)\cext.obj" \
	"$(INTDIR)\chbox.obj" \
	"$(INTDIR)\CHCYL.obj" \
	"$(INTDIR)\damper.obj" \
	"$(INTDIR)\gengon.obj" \
	"$(INTDIR)\hose.obj" \
	"$(INTDIR)\lext.obj" \
	"$(INTDIR)\oiltnk.obj" \
	"$(INTDIR)\prism.obj" \
	"$(INTDIR)\pyramid.obj" \
	"$(INTDIR)\scuba.obj" \
	"$(INTDIR)\solids.obj" \
	"$(INTDIR)\spind.obj" \
	"$(INTDIR)\spring.obj" \
	"$(INTDIR)\scs.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\scs.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "scs - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\scs.dlo"


CLEAN :
	-@erase "$(INTDIR)\cext.obj"
	-@erase "$(INTDIR)\chbox.obj"
	-@erase "$(INTDIR)\CHCYL.obj"
	-@erase "$(INTDIR)\damper.obj"
	-@erase "$(INTDIR)\gengon.obj"
	-@erase "$(INTDIR)\hose.obj"
	-@erase "$(INTDIR)\lext.obj"
	-@erase "$(INTDIR)\oiltnk.obj"
	-@erase "$(INTDIR)\prism.obj"
	-@erase "$(INTDIR)\pyramid.obj"
	-@erase "$(INTDIR)\scs.res"
	-@erase "$(INTDIR)\scuba.obj"
	-@erase "$(INTDIR)\solids.obj"
	-@erase "$(INTDIR)\spind.obj"
	-@erase "$(INTDIR)\spring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\scs.exp"
	-@erase "$(OUTDIR)\scs.lib"
	-@erase "$(OUTDIR)\scs.pdb"
	-@erase "..\..\..\maxsdk\plugin\scs.dlo"
	-@erase "..\..\..\maxsdk\plugin\scs.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\scs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x054D0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\scs.pdb" /debug /machine:I386 /def:".\scs.def" /out:"..\..\..\maxsdk\plugin\scs.dlo" /implib:"$(OUTDIR)\scs.lib" /delayload:"edmodel.dll" 
DEF_FILE= \
	".\scs.def"
LINK32_OBJS= \
	"$(INTDIR)\cext.obj" \
	"$(INTDIR)\chbox.obj" \
	"$(INTDIR)\CHCYL.obj" \
	"$(INTDIR)\damper.obj" \
	"$(INTDIR)\gengon.obj" \
	"$(INTDIR)\hose.obj" \
	"$(INTDIR)\lext.obj" \
	"$(INTDIR)\oiltnk.obj" \
	"$(INTDIR)\prism.obj" \
	"$(INTDIR)\pyramid.obj" \
	"$(INTDIR)\scuba.obj" \
	"$(INTDIR)\solids.obj" \
	"$(INTDIR)\spind.obj" \
	"$(INTDIR)\spring.obj" \
	"$(INTDIR)\scs.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\edmodel.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\scs.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("scs.dep")
!INCLUDE "scs.dep"
!ELSE 
!MESSAGE Warning: cannot find "scs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "scs - Win32 Release" || "$(CFG)" == "scs - Win32 Debug" || "$(CFG)" == "scs - Win32 Hybrid"
SOURCE=.\cext.cpp

"$(INTDIR)\cext.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\chbox.CPP

"$(INTDIR)\chbox.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CHCYL.cpp

"$(INTDIR)\CHCYL.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\damper.cpp

"$(INTDIR)\damper.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gengon.cpp

"$(INTDIR)\gengon.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hose.cpp

"$(INTDIR)\hose.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lext.cpp

"$(INTDIR)\lext.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\oiltnk.cpp

"$(INTDIR)\oiltnk.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\prism.cpp

"$(INTDIR)\prism.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pyramid.cpp

"$(INTDIR)\pyramid.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scs.rc

"$(INTDIR)\scs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\scuba.cpp

"$(INTDIR)\scuba.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\solids.cpp

"$(INTDIR)\solids.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spind.cpp

"$(INTDIR)\spind.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spring.cpp

"$(INTDIR)\spring.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

