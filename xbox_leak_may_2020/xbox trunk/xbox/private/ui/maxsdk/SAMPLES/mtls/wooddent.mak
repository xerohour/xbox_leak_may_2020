# Microsoft Developer Studio Generated NMAKE File, Based on Wooddent.dsp
!IF "$(CFG)" == ""
CFG=wooddent - Win32 Release
!MESSAGE No configuration specified. Defaulting to wooddent - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "wooddent - Win32 Release" && "$(CFG)" != "wooddent - Win32 Debug" && "$(CFG)" != "wooddent - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wooddent.mak" CFG="wooddent - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wooddent - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wooddent - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wooddent - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wooddent - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\wooddent.dlt"


CLEAN :
	-@erase "$(INTDIR)\dent.obj"
	-@erase "$(INTDIR)\wood.obj"
	-@erase "$(INTDIR)\wooddent.obj"
	-@erase "$(INTDIR)\WOODDENT.res"
	-@erase "$(OUTDIR)\wooddent.exp"
	-@erase "$(OUTDIR)\wooddent.lib"
	-@erase "..\..\..\maxsdk\plugin\wooddent.dlt"
	-@erase ".\wooddent.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Wooddent.pch" /YX /Fo"$(INTDIR)\\" /Fd"wooddent.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WOODDENT.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/o"$(OUTDIR)\Wooddent.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02ED0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\wooddent.pdb" /machine:I386 /def:".\wooddent.def" /out:"..\..\..\maxsdk\plugin\wooddent.dlt" /implib:"$(OUTDIR)\wooddent.lib" /release 
DEF_FILE= \
	".\wooddent.def"
LINK32_OBJS= \
	"$(INTDIR)\dent.obj" \
	"$(INTDIR)\wood.obj" \
	"$(INTDIR)\wooddent.obj" \
	"$(INTDIR)\WOODDENT.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\wooddent.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wooddent - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\wooddent.dlt"


CLEAN :
	-@erase "$(INTDIR)\dent.obj"
	-@erase "$(INTDIR)\wood.obj"
	-@erase "$(INTDIR)\wooddent.obj"
	-@erase "$(INTDIR)\WOODDENT.res"
	-@erase "$(OUTDIR)\wooddent.exp"
	-@erase "$(OUTDIR)\wooddent.lib"
	-@erase "$(OUTDIR)\wooddent.pdb"
	-@erase "..\..\..\maxsdk\plugin\wooddent.dlt"
	-@erase "..\..\..\maxsdk\plugin\wooddent.ilk"
	-@erase ".\wooddent.idb"
	-@erase ".\wooddent.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Wooddent.pch" /YX /Fo"$(INTDIR)\\" /Fd"wooddent.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WOODDENT.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/o"$(OUTDIR)\Wooddent.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02ED0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\wooddent.pdb" /debug /machine:I386 /def:".\wooddent.def" /out:"..\..\..\maxsdk\plugin\wooddent.dlt" /implib:"$(OUTDIR)\wooddent.lib" 
DEF_FILE= \
	".\wooddent.def"
LINK32_OBJS= \
	"$(INTDIR)\dent.obj" \
	"$(INTDIR)\wood.obj" \
	"$(INTDIR)\wooddent.obj" \
	"$(INTDIR)\WOODDENT.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\wooddent.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wooddent - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\wooddent.dlt"


CLEAN :
	-@erase "$(INTDIR)\dent.obj"
	-@erase "$(INTDIR)\wood.obj"
	-@erase "$(INTDIR)\wooddent.obj"
	-@erase "$(INTDIR)\WOODDENT.res"
	-@erase "$(OUTDIR)\wooddent.exp"
	-@erase "$(OUTDIR)\wooddent.lib"
	-@erase "$(OUTDIR)\wooddent.pdb"
	-@erase "..\..\..\maxsdk\plugin\wooddent.dlt"
	-@erase "..\..\..\maxsdk\plugin\wooddent.ilk"
	-@erase ".\wooddent.idb"
	-@erase ".\wooddent.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Wooddent.pch" /YX /Fo"$(INTDIR)\\" /Fd"wooddent.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WOODDENT.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/o"$(OUTDIR)\Wooddent.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x02ED0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\wooddent.pdb" /debug /machine:I386 /def:".\wooddent.def" /out:"..\..\..\maxsdk\plugin\wooddent.dlt" /implib:"$(OUTDIR)\wooddent.lib" 
DEF_FILE= \
	".\wooddent.def"
LINK32_OBJS= \
	"$(INTDIR)\dent.obj" \
	"$(INTDIR)\wood.obj" \
	"$(INTDIR)\wooddent.obj" \
	"$(INTDIR)\WOODDENT.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\wooddent.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Wooddent.dep")
!INCLUDE "Wooddent.dep"
!ELSE 
!MESSAGE Warning: cannot find "Wooddent.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wooddent - Win32 Release" || "$(CFG)" == "wooddent - Win32 Debug" || "$(CFG)" == "wooddent - Win32 Hybrid"
SOURCE=.\dent.cpp

"$(INTDIR)\dent.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wood.cpp

"$(INTDIR)\wood.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wooddent.cpp

"$(INTDIR)\wooddent.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WOODDENT.RC

"$(INTDIR)\WOODDENT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

