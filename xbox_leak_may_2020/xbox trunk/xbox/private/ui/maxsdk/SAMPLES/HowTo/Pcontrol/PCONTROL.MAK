# Microsoft Developer Studio Generated NMAKE File, Based on PControl.dsp
!IF "$(CFG)" == ""
CFG=PControl - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to PControl - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "PControl - Win32 Release" && "$(CFG)" != "PControl - Win32 Debug" && "$(CFG)" != "PControl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PControl.mak" CFG="PControl - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PControl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PControl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PControl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "PControl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\PControl.dlc"


CLEAN :
	-@erase "$(INTDIR)\PControl.obj"
	-@erase "$(INTDIR)\PControl.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PControl.exp"
	-@erase "$(OUTDIR)\PControl.lib"
	-@erase "..\..\..\plugin\PControl.dlc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PControl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PControl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PControl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08B20000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PControl.pdb" /machine:I386 /def:".\PControl.def" /out:"..\..\..\..\maxsdk\plugin\PControl.dlc" /implib:"$(OUTDIR)\PControl.lib" /release 
DEF_FILE= \
	".\PControl.def"
LINK32_OBJS= \
	"$(INTDIR)\PControl.obj" \
	"$(INTDIR)\PControl.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PControl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PControl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\PControl.dlc"


CLEAN :
	-@erase "$(INTDIR)\PControl.obj"
	-@erase "$(INTDIR)\PControl.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PControl.exp"
	-@erase "$(OUTDIR)\PControl.lib"
	-@erase "$(OUTDIR)\PControl.pdb"
	-@erase "..\..\..\plugin\PControl.dlc"
	-@erase "..\..\..\plugin\PControl.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PControl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PControl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PControl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08B20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PControl.pdb" /debug /machine:I386 /def:".\PControl.def" /out:"..\..\..\..\maxsdk\plugin\PControl.dlc" /implib:"$(OUTDIR)\PControl.lib" /pdbtype:sept 
DEF_FILE= \
	".\PControl.def"
LINK32_OBJS= \
	"$(INTDIR)\PControl.obj" \
	"$(INTDIR)\PControl.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PControl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PControl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\PControl.dlc"


CLEAN :
	-@erase "$(INTDIR)\PControl.obj"
	-@erase "$(INTDIR)\PControl.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PControl.exp"
	-@erase "$(OUTDIR)\PControl.lib"
	-@erase "$(OUTDIR)\PControl.pdb"
	-@erase "..\..\..\plugin\PControl.dlc"
	-@erase "..\..\..\plugin\PControl.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PControl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PControl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PControl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08B20000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PControl.pdb" /debug /machine:I386 /def:".\PControl.def" /out:"..\..\..\..\maxsdk\plugin\PControl.dlc" /implib:"$(OUTDIR)\PControl.lib" /pdbtype:sept 
DEF_FILE= \
	".\PControl.def"
LINK32_OBJS= \
	"$(INTDIR)\PControl.obj" \
	"$(INTDIR)\PControl.res" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\geom.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\plugin\PControl.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("PControl.dep")
!INCLUDE "PControl.dep"
!ELSE 
!MESSAGE Warning: cannot find "PControl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PControl - Win32 Release" || "$(CFG)" == "PControl - Win32 Debug" || "$(CFG)" == "PControl - Win32 Hybrid"
SOURCE=.\PControl.cpp

"$(INTDIR)\PControl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PControl.rc

"$(INTDIR)\PControl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

