# Microsoft Developer Studio Generated NMAKE File, Based on Targa.dsp
!IF "$(CFG)" == ""
CFG=targa - Win32 Release
!MESSAGE No configuration specified. Defaulting to targa - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "targa - Win32 Release" && "$(CFG)" != "targa - Win32 Debug" && "$(CFG)" != "targa - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Targa.mak" CFG="targa - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "targa - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "targa - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "targa - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "targa - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\targa.bmi"


CLEAN :
	-@erase "$(INTDIR)\targa.obj"
	-@erase "$(INTDIR)\targa.res"
	-@erase "$(INTDIR)\targapch.obj"
	-@erase "$(OUTDIR)\targa.exp"
	-@erase "$(OUTDIR)\targa.lib"
	-@erase "..\..\..\..\maxsdk\plugin\targa.bmi"
	-@erase ".\targa.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Targa.pch" /YX /Fo"$(INTDIR)\\" /Fd"targa.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\targa.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Targa.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2d1f0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\targa.pdb" /machine:I386 /def:".\targa.def" /out:"..\..\..\..\maxsdk\plugin\targa.bmi" /implib:"$(OUTDIR)\targa.lib" /release 
DEF_FILE= \
	".\targa.def"
LINK32_OBJS= \
	"$(INTDIR)\targa.obj" \
	"$(INTDIR)\targapch.obj" \
	"$(INTDIR)\targa.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\targa.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "targa - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\targa.bmi"


CLEAN :
	-@erase "$(INTDIR)\targa.obj"
	-@erase "$(INTDIR)\targa.res"
	-@erase "$(INTDIR)\targapch.obj"
	-@erase "$(OUTDIR)\targa.exp"
	-@erase "$(OUTDIR)\targa.lib"
	-@erase "$(OUTDIR)\targa.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\targa.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\targa.ilk"
	-@erase ".\targa.idb"
	-@erase ".\targa.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Targa.pch" /YX /Fo"$(INTDIR)\\" /Fd"targa.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\targa.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Targa.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2d1f0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\targa.pdb" /debug /machine:I386 /def:".\targa.def" /out:"..\..\..\..\maxsdk\plugin\targa.bmi" /implib:"$(OUTDIR)\targa.lib" 
DEF_FILE= \
	".\targa.def"
LINK32_OBJS= \
	"$(INTDIR)\targa.obj" \
	"$(INTDIR)\targapch.obj" \
	"$(INTDIR)\targa.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\targa.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "targa - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\targa.bmi"


CLEAN :
	-@erase "$(INTDIR)\targa.obj"
	-@erase "$(INTDIR)\targa.res"
	-@erase "$(INTDIR)\targapch.obj"
	-@erase "$(OUTDIR)\targa.exp"
	-@erase "$(OUTDIR)\targa.lib"
	-@erase "$(OUTDIR)\targa.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\targa.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\targa.ilk"
	-@erase ".\targa.idb"
	-@erase ".\targa.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Targa.pch" /YX /Fo"$(INTDIR)\\" /Fd"targa.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\targa.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Targa.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x2d1f0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\targa.pdb" /debug /machine:I386 /def:".\targa.def" /out:"..\..\..\..\maxsdk\plugin\targa.bmi" /implib:"$(OUTDIR)\targa.lib" 
DEF_FILE= \
	".\targa.def"
LINK32_OBJS= \
	"$(INTDIR)\targa.obj" \
	"$(INTDIR)\targapch.obj" \
	"$(INTDIR)\targa.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\targa.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Targa.dep")
!INCLUDE "Targa.dep"
!ELSE 
!MESSAGE Warning: cannot find "Targa.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "targa - Win32 Release" || "$(CFG)" == "targa - Win32 Debug" || "$(CFG)" == "targa - Win32 Hybrid"
SOURCE=.\targa.cpp

"$(INTDIR)\targa.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\targa.rc

"$(INTDIR)\targa.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\targapch.cpp

"$(INTDIR)\targapch.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

