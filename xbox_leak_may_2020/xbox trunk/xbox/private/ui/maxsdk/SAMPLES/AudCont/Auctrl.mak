# Microsoft Developer Studio Generated NMAKE File, Based on Auctrl.dsp
!IF "$(CFG)" == ""
CFG=auctrl - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to auctrl - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "auctrl - Win32 Release" && "$(CFG)" != "auctrl - Win32 Debug" && "$(CFG)" != "auctrl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Auctrl.mak" CFG="auctrl - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "auctrl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "auctrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "auctrl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "auctrl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\auctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\auctrl.obj"
	-@erase "$(INTDIR)\auctrl.res"
	-@erase "$(INTDIR)\aufloat.obj"
	-@erase "$(INTDIR)\aup3base.obj"
	-@erase "$(INTDIR)\aup3dlg.obj"
	-@erase "$(INTDIR)\aupoint3.obj"
	-@erase "$(INTDIR)\aupos.obj"
	-@erase "$(INTDIR)\aurot.obj"
	-@erase "$(INTDIR)\auscale.obj"
	-@erase "$(INTDIR)\rtwave.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\auctrl.exp"
	-@erase "$(OUTDIR)\auctrl.lib"
	-@erase "..\..\..\maxsdk\plugin\auctrl.dlc"
	-@erase ".\auctrl.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\maxsdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Auctrl.pch" /YX"auctrl.h" /Fo"$(INTDIR)\\" /Fd"auctrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\auctrl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Auctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib vfw32.lib /nologo /base:"0x05D40000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\auctrl.pdb" /machine:I386 /def:".\auctrl.def" /out:"..\..\..\maxsdk\plugin\auctrl.dlc" /implib:"$(OUTDIR)\auctrl.lib" /release 
DEF_FILE= \
	".\auctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\auctrl.obj" \
	"$(INTDIR)\aufloat.obj" \
	"$(INTDIR)\aup3base.obj" \
	"$(INTDIR)\aup3dlg.obj" \
	"$(INTDIR)\aupoint3.obj" \
	"$(INTDIR)\aupos.obj" \
	"$(INTDIR)\aurot.obj" \
	"$(INTDIR)\auscale.obj" \
	"$(INTDIR)\rtwave.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\auctrl.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\auctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "auctrl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\auctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\auctrl.obj"
	-@erase "$(INTDIR)\auctrl.res"
	-@erase "$(INTDIR)\aufloat.obj"
	-@erase "$(INTDIR)\aup3base.obj"
	-@erase "$(INTDIR)\aup3dlg.obj"
	-@erase "$(INTDIR)\aupoint3.obj"
	-@erase "$(INTDIR)\aupos.obj"
	-@erase "$(INTDIR)\aurot.obj"
	-@erase "$(INTDIR)\auscale.obj"
	-@erase "$(INTDIR)\rtwave.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\auctrl.exp"
	-@erase "$(OUTDIR)\auctrl.lib"
	-@erase "$(OUTDIR)\auctrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\auctrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\auctrl.ilk"
	-@erase ".\auctrl.idb"
	-@erase ".\auctrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\maxsdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Auctrl.pch" /YX"auctrl.h" /Fo"$(INTDIR)\\" /Fd"auctrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\auctrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Auctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib vfw32.lib /nologo /base:"0x05D40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\auctrl.pdb" /debug /machine:I386 /def:".\auctrl.def" /out:"..\..\..\maxsdk\plugin\auctrl.dlc" /implib:"$(OUTDIR)\auctrl.lib" 
DEF_FILE= \
	".\auctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\auctrl.obj" \
	"$(INTDIR)\aufloat.obj" \
	"$(INTDIR)\aup3base.obj" \
	"$(INTDIR)\aup3dlg.obj" \
	"$(INTDIR)\aupoint3.obj" \
	"$(INTDIR)\aupos.obj" \
	"$(INTDIR)\aurot.obj" \
	"$(INTDIR)\auscale.obj" \
	"$(INTDIR)\rtwave.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\auctrl.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\auctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "auctrl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\auctrl.dlc"


CLEAN :
	-@erase "$(INTDIR)\auctrl.obj"
	-@erase "$(INTDIR)\auctrl.res"
	-@erase "$(INTDIR)\aufloat.obj"
	-@erase "$(INTDIR)\aup3base.obj"
	-@erase "$(INTDIR)\aup3dlg.obj"
	-@erase "$(INTDIR)\aupoint3.obj"
	-@erase "$(INTDIR)\aupos.obj"
	-@erase "$(INTDIR)\aurot.obj"
	-@erase "$(INTDIR)\auscale.obj"
	-@erase "$(INTDIR)\rtwave.obj"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(OUTDIR)\auctrl.exp"
	-@erase "$(OUTDIR)\auctrl.lib"
	-@erase "$(OUTDIR)\auctrl.pdb"
	-@erase "..\..\..\maxsdk\plugin\auctrl.dlc"
	-@erase "..\..\..\maxsdk\plugin\auctrl.ilk"
	-@erase ".\auctrl.idb"
	-@erase ".\auctrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\maxsdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Auctrl.pch" /YX"auctrl.h" /Fo"$(INTDIR)\\" /Fd"auctrl.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\auctrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Auctrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib vfw32.lib /nologo /base:"0x05D40000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\auctrl.pdb" /debug /machine:I386 /def:".\auctrl.def" /out:"..\..\..\maxsdk\plugin\auctrl.dlc" /implib:"$(OUTDIR)\auctrl.lib" 
DEF_FILE= \
	".\auctrl.def"
LINK32_OBJS= \
	"$(INTDIR)\auctrl.obj" \
	"$(INTDIR)\aufloat.obj" \
	"$(INTDIR)\aup3base.obj" \
	"$(INTDIR)\aup3dlg.obj" \
	"$(INTDIR)\aupoint3.obj" \
	"$(INTDIR)\aupos.obj" \
	"$(INTDIR)\aurot.obj" \
	"$(INTDIR)\auscale.obj" \
	"$(INTDIR)\rtwave.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\auctrl.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\auctrl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Auctrl.dep")
!INCLUDE "Auctrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "Auctrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "auctrl - Win32 Release" || "$(CFG)" == "auctrl - Win32 Debug" || "$(CFG)" == "auctrl - Win32 Hybrid"
SOURCE=.\auctrl.cpp

"$(INTDIR)\auctrl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\auctrl.rc

"$(INTDIR)\auctrl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\aufloat.cpp

"$(INTDIR)\aufloat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aup3base.cpp

"$(INTDIR)\aup3base.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aup3dlg.cpp

"$(INTDIR)\aup3dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aupoint3.cpp

"$(INTDIR)\aupoint3.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aupos.cpp

"$(INTDIR)\aupos.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aurot.cpp

"$(INTDIR)\aurot.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\auscale.cpp

"$(INTDIR)\auscale.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rtwave.cpp

"$(INTDIR)\rtwave.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wave.cpp

"$(INTDIR)\wave.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

