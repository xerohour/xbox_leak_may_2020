# Microsoft Developer Studio Generated NMAKE File, Based on RingArry.dsp
!IF "$(CFG)" == ""
CFG=RingArry - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to RingArry - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "RingArry - Win32 Release" && "$(CFG)" != "RingArry - Win32 Debug" && "$(CFG)" != "RingArry - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RingArry.mak" CFG="RingArry - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RingArry - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RingArry - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RingArry - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "RingArry - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\RingArry.dlo"


CLEAN :
	-@erase "$(INTDIR)\RingArry.obj"
	-@erase "$(INTDIR)\RingArry.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\RingArry.exp"
	-@erase "$(OUTDIR)\RingArry.lib"
	-@erase "..\..\..\plugin\RingArry.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RingArry.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RingArry.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RingArry.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\RingArry.pdb" /machine:I386 /def:".\RingArry.def" /out:"..\..\..\..\maxsdk\plugin\RingArry.dlo" /implib:"$(OUTDIR)\RingArry.lib" /release 
DEF_FILE= \
	".\RingArry.def"
LINK32_OBJS= \
	"$(INTDIR)\RingArry.obj" \
	"$(INTDIR)\RingArry.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\RingArry.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RingArry - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\RingArry.dlo"


CLEAN :
	-@erase "$(INTDIR)\RingArry.obj"
	-@erase "$(INTDIR)\RingArry.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\RingArry.exp"
	-@erase "$(OUTDIR)\RingArry.lib"
	-@erase "$(OUTDIR)\RingArry.pdb"
	-@erase "..\..\..\plugin\RingArry.dlo"
	-@erase "..\..\..\plugin\RingArry.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RingArry.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RingArry.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RingArry.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\RingArry.pdb" /debug /machine:I386 /def:".\RingArry.def" /out:"..\..\..\..\maxsdk\plugin\RingArry.dlo" /implib:"$(OUTDIR)\RingArry.lib" /pdbtype:sept 
DEF_FILE= \
	".\RingArry.def"
LINK32_OBJS= \
	"$(INTDIR)\RingArry.obj" \
	"$(INTDIR)\RingArry.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\RingArry.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RingArry - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\RingArry.dlo"


CLEAN :
	-@erase "$(INTDIR)\RingArry.obj"
	-@erase "$(INTDIR)\RingArry.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\RingArry.exp"
	-@erase "$(OUTDIR)\RingArry.lib"
	-@erase "$(OUTDIR)\RingArry.pdb"
	-@erase "..\..\..\plugin\RingArry.dlo"
	-@erase "..\..\..\plugin\RingArry.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\RingArry.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RingArry.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RingArry.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\RingArry.pdb" /debug /machine:I386 /def:".\RingArry.def" /out:"..\..\..\..\maxsdk\plugin\RingArry.dlo" /implib:"$(OUTDIR)\RingArry.lib" /pdbtype:sept 
DEF_FILE= \
	".\RingArry.def"
LINK32_OBJS= \
	"$(INTDIR)\RingArry.obj" \
	"$(INTDIR)\RingArry.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\RingArry.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("RingArry.dep")
!INCLUDE "RingArry.dep"
!ELSE 
!MESSAGE Warning: cannot find "RingArry.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "RingArry - Win32 Release" || "$(CFG)" == "RingArry - Win32 Debug" || "$(CFG)" == "RingArry - Win32 Hybrid"
SOURCE=.\RingArry.cpp

"$(INTDIR)\RingArry.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RingArry.rc

"$(INTDIR)\RingArry.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

