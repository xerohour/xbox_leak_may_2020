# Microsoft Developer Studio Generated NMAKE File, Based on Tif.dsp
!IF "$(CFG)" == ""
CFG=tif - Win32 Release
!MESSAGE No configuration specified. Defaulting to tif - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "tif - Win32 Release" && "$(CFG)" != "tif - Win32 Debug" && "$(CFG)" != "tif - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tif.mak" CFG="tif - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tif - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tif - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tif - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tif - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\tif.bmi"


CLEAN :
	-@erase "$(INTDIR)\tif.obj"
	-@erase "$(INTDIR)\tif.res"
	-@erase "$(INTDIR)\tifpch.obj"
	-@erase "$(OUTDIR)\tif.exp"
	-@erase "$(OUTDIR)\tif.lib"
	-@erase "..\..\..\..\maxsdk\plugin\tif.bmi"
	-@erase ".\tif.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\tif.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Release\tiflib.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib geom.lib /nologo /base:"0x103a0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\tif.pdb" /machine:I386 /def:".\tif.def" /out:"..\..\..\..\maxsdk\plugin\tif.bmi" /implib:"$(OUTDIR)\tif.lib" /libpath:"..\..\..\lib" /release 
DEF_FILE= \
	".\tif.def"
LINK32_OBJS= \
	"$(INTDIR)\tif.obj" \
	"$(INTDIR)\tifpch.obj" \
	"$(INTDIR)\tif.res" \
	"..\..\..\Lib\bmm.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\tif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tif - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\tif.bmi"


CLEAN :
	-@erase "$(INTDIR)\tif.obj"
	-@erase "$(INTDIR)\tif.res"
	-@erase "$(INTDIR)\tifpch.obj"
	-@erase "$(OUTDIR)\tif.exp"
	-@erase "$(OUTDIR)\tif.lib"
	-@erase "$(OUTDIR)\tif.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\tif.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\tif.ilk"
	-@erase ".\tif.idb"
	-@erase ".\tif.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\tif.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Debug\tiflib.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib geom.lib /nologo /base:"0x103a0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\tif.pdb" /debug /machine:I386 /def:".\tif.def" /out:"..\..\..\..\maxsdk\plugin\tif.bmi" /implib:"$(OUTDIR)\tif.lib" /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\tif.def"
LINK32_OBJS= \
	"$(INTDIR)\tif.obj" \
	"$(INTDIR)\tifpch.obj" \
	"$(INTDIR)\tif.res" \
	"..\..\..\Lib\bmm.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\tif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tif - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\tif.bmi"


CLEAN :
	-@erase "$(INTDIR)\tif.obj"
	-@erase "$(INTDIR)\tif.res"
	-@erase "$(INTDIR)\tifpch.obj"
	-@erase "$(OUTDIR)\tif.exp"
	-@erase "$(OUTDIR)\tif.lib"
	-@erase "$(OUTDIR)\tif.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\tif.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\tif.ilk"
	-@erase ".\tif.idb"
	-@erase ".\tif.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\tif.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tif.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Hybrid\tiflib.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib geom.lib /nologo /base:"0x103a0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\tif.pdb" /debug /machine:I386 /def:".\tif.def" /out:"..\..\..\..\maxsdk\plugin\tif.bmi" /implib:"$(OUTDIR)\tif.lib" /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\tif.def"
LINK32_OBJS= \
	"$(INTDIR)\tif.obj" \
	"$(INTDIR)\tifpch.obj" \
	"$(INTDIR)\tif.res" \
	"..\..\..\Lib\bmm.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\tif.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Tif.dep")
!INCLUDE "Tif.dep"
!ELSE 
!MESSAGE Warning: cannot find "Tif.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tif - Win32 Release" || "$(CFG)" == "tif - Win32 Debug" || "$(CFG)" == "tif - Win32 Hybrid"
SOURCE=.\tif.cpp

!IF  "$(CFG)" == "tif - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tif - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tif - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\tif.rc

"$(INTDIR)\tif.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\tifpch.cpp

!IF  "$(CFG)" == "tif - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tifpch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tif - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tifpch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tif - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"tif.pdb" /FD /c 

"$(INTDIR)\tifpch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

