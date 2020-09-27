# Microsoft Developer Studio Generated NMAKE File, Based on Gif.dsp
!IF "$(CFG)" == ""
CFG=gif - Win32 Release
!MESSAGE No configuration specified. Defaulting to gif - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "gif - Win32 Release" && "$(CFG)" != "gif - Win32 Debug" && "$(CFG)" != "gif - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gif.mak" CFG="gif - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gif - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gif - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gif - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "gif - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\gif.bmi"


CLEAN :
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\gif.res"
	-@erase "$(INTDIR)\gifpch.obj"
	-@erase "$(OUTDIR)\gif.exp"
	-@erase "$(OUTDIR)\gif.lib"
	-@erase "..\..\..\..\maxsdk\plugin\gif.bmi"
	-@erase ".\gif.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Gif.pch" /YX /Fo"$(INTDIR)\\" /Fd"gif.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\gif.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x077E0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\gif.pdb" /machine:I386 /def:".\gif.def" /out:"..\..\..\..\maxsdk\plugin\gif.bmi" /implib:"$(OUTDIR)\gif.lib" /release 
DEF_FILE= \
	".\gif.def"
LINK32_OBJS= \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\gifpch.obj" \
	"$(INTDIR)\gif.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\gif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gif - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\gif.bmi"


CLEAN :
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\gif.res"
	-@erase "$(INTDIR)\gifpch.obj"
	-@erase "$(OUTDIR)\gif.exp"
	-@erase "$(OUTDIR)\gif.lib"
	-@erase "$(OUTDIR)\gif.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\gif.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\gif.ilk"
	-@erase ".\gif.idb"
	-@erase ".\gif.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Gif.pch" /YX /Fo"$(INTDIR)\\" /Fd"gif.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\gif.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x077E0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\gif.pdb" /debug /machine:I386 /def:".\gif.def" /out:"..\..\..\..\maxsdk\plugin\gif.bmi" /implib:"$(OUTDIR)\gif.lib" 
DEF_FILE= \
	".\gif.def"
LINK32_OBJS= \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\gifpch.obj" \
	"$(INTDIR)\gif.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\gif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gif - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\gif.bmi"


CLEAN :
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\gif.res"
	-@erase "$(INTDIR)\gifpch.obj"
	-@erase "$(OUTDIR)\gif.exp"
	-@erase "$(OUTDIR)\gif.lib"
	-@erase "$(OUTDIR)\gif.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\gif.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\gif.ilk"
	-@erase ".\gif.idb"
	-@erase ".\gif.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Gif.pch" /YX /Fo"$(INTDIR)\\" /Fd"gif.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\gif.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x077E0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\gif.pdb" /debug /machine:I386 /def:".\gif.def" /out:"..\..\..\..\maxsdk\plugin\gif.bmi" /implib:"$(OUTDIR)\gif.lib" 
DEF_FILE= \
	".\gif.def"
LINK32_OBJS= \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\gifpch.obj" \
	"$(INTDIR)\gif.res" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\gif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Gif.dep")
!INCLUDE "Gif.dep"
!ELSE 
!MESSAGE Warning: cannot find "Gif.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gif - Win32 Release" || "$(CFG)" == "gif - Win32 Debug" || "$(CFG)" == "gif - Win32 Hybrid"
SOURCE=.\gif.cpp

"$(INTDIR)\gif.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gif.rc

"$(INTDIR)\gif.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\gifpch.cpp

"$(INTDIR)\gifpch.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

