# Microsoft Developer Studio Generated NMAKE File, Based on Avi.dsp
!IF "$(CFG)" == ""
CFG=avi - Win32 Release
!MESSAGE No configuration specified. Defaulting to avi - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "avi - Win32 Release" && "$(CFG)" != "avi - Win32 Debug" && "$(CFG)" != "avi - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avi.mak" CFG="avi - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avi - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avi - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\avi.bmi"


CLEAN :
	-@erase "$(INTDIR)\avi.obj"
	-@erase "$(INTDIR)\avi.res"
	-@erase "$(OUTDIR)\avi.exp"
	-@erase "$(OUTDIR)\avi.lib"
	-@erase "..\..\..\..\maxsdk\plugin\avi.bmi"
	-@erase ".\avi.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Avi.pch" /YX /Fo"$(INTDIR)\\" /Fd"avi.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avi.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x101d0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\avi.pdb" /machine:I386 /def:".\avi.def" /out:"..\..\..\..\maxsdk\plugin\avi.bmi" /implib:"$(OUTDIR)\avi.lib" /release 
DEF_FILE= \
	".\avi.def"
LINK32_OBJS= \
	"$(INTDIR)\avi.obj" \
	"$(INTDIR)\avi.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\avi.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "avi - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\avi.bmi"


CLEAN :
	-@erase "$(INTDIR)\avi.obj"
	-@erase "$(INTDIR)\avi.res"
	-@erase "$(OUTDIR)\avi.exp"
	-@erase "$(OUTDIR)\avi.lib"
	-@erase "$(OUTDIR)\avi.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\avi.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\avi.ilk"
	-@erase ".\avi.idb"
	-@erase ".\avi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Avi.pch" /YX /Fo"$(INTDIR)\\" /Fd"avi.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avi.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x101d0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\avi.pdb" /debug /machine:I386 /def:".\avi.def" /out:"..\..\..\..\maxsdk\plugin\avi.bmi" /implib:"$(OUTDIR)\avi.lib" 
DEF_FILE= \
	".\avi.def"
LINK32_OBJS= \
	"$(INTDIR)\avi.obj" \
	"$(INTDIR)\avi.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\avi.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "avi - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\avi.bmi"


CLEAN :
	-@erase "$(INTDIR)\avi.obj"
	-@erase "$(INTDIR)\avi.res"
	-@erase "$(OUTDIR)\avi.exp"
	-@erase "$(OUTDIR)\avi.lib"
	-@erase "$(OUTDIR)\avi.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\avi.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\avi.ilk"
	-@erase ".\avi.idb"
	-@erase ".\avi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Avi.pch" /YX /Fo"$(INTDIR)\\" /Fd"avi.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avi.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x101d0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\avi.pdb" /debug /machine:I386 /def:".\avi.def" /out:"..\..\..\..\maxsdk\plugin\avi.bmi" /implib:"$(OUTDIR)\avi.lib" 
DEF_FILE= \
	".\avi.def"
LINK32_OBJS= \
	"$(INTDIR)\avi.obj" \
	"$(INTDIR)\avi.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\avi.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Avi.dep")
!INCLUDE "Avi.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avi.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avi - Win32 Release" || "$(CFG)" == "avi - Win32 Debug" || "$(CFG)" == "avi - Win32 Hybrid"
SOURCE=.\avi.cpp

"$(INTDIR)\avi.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avi.rc

"$(INTDIR)\avi.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

