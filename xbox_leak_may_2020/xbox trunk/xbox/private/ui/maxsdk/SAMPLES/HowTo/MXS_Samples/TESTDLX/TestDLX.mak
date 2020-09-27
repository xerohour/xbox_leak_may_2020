# Microsoft Developer Studio Generated NMAKE File, Based on TestDLX.dsp
!IF "$(CFG)" == ""
CFG=TestDLX - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to TestDLX - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "TestDLX - Win32 Release" && "$(CFG)" != "TestDLX - Win32 Debug" && "$(CFG)" != "TestDLX - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestDLX.mak" CFG="TestDLX - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestDLX - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestDLX - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestDLX - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "TestDLX - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"


CLEAN :
	-@erase "$(INTDIR)\dlxClass.obj"
	-@erase "$(INTDIR)\testDLX.obj"
	-@erase "$(INTDIR)\testdlx.res"
	-@erase "$(INTDIR)\tester.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\testdlx.exp"
	-@erase "$(OUTDIR)\testdlx.lib"
	-@erase "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\testdlx.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestDLX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x0B160000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\testdlx.pdb" /machine:I386 /def:".\TestDLX.def" /out:"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" /implib:"$(OUTDIR)\testdlx.lib" /release 
DEF_FILE= \
	".\TestDLX.def"
LINK32_OBJS= \
	"$(INTDIR)\dlxClass.obj" \
	"$(INTDIR)\testDLX.obj" \
	"$(INTDIR)\tester.obj" \
	"$(INTDIR)\testdlx.res" \
	"..\..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\..\lib\core.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\geom.lib"

"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDLX - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"


CLEAN :
	-@erase "$(INTDIR)\dlxClass.obj"
	-@erase "$(INTDIR)\testDLX.obj"
	-@erase "$(INTDIR)\testdlx.res"
	-@erase "$(INTDIR)\tester.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\testdlx.exp"
	-@erase "$(OUTDIR)\testdlx.lib"
	-@erase "$(OUTDIR)\testdlx.pdb"
	-@erase "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"
	-@erase "..\..\..\..\..\maxsdk\plugin\testdlx.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\testdlx.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestDLX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x0B160000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\testdlx.pdb" /debug /machine:I386 /def:".\TestDLX.def" /out:"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" /implib:"$(OUTDIR)\testdlx.lib" /pdbtype:sept 
DEF_FILE= \
	".\TestDLX.def"
LINK32_OBJS= \
	"$(INTDIR)\dlxClass.obj" \
	"$(INTDIR)\testDLX.obj" \
	"$(INTDIR)\tester.obj" \
	"$(INTDIR)\testdlx.res" \
	"..\..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\..\lib\core.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\geom.lib"

"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDLX - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"


CLEAN :
	-@erase "$(INTDIR)\dlxClass.obj"
	-@erase "$(INTDIR)\testDLX.obj"
	-@erase "$(INTDIR)\testdlx.res"
	-@erase "$(INTDIR)\tester.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\testdlx.exp"
	-@erase "$(OUTDIR)\testdlx.lib"
	-@erase "$(OUTDIR)\testdlx.pdb"
	-@erase "..\..\..\..\..\maxsdk\plugin\testdlx.dlx"
	-@erase "..\..\..\..\..\maxsdk\plugin\testdlx.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\testdlx.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestDLX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x0B160000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\testdlx.pdb" /debug /machine:I386 /def:".\TestDLX.def" /out:"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" /implib:"$(OUTDIR)\testdlx.lib" /pdbtype:sept 
DEF_FILE= \
	".\TestDLX.def"
LINK32_OBJS= \
	"$(INTDIR)\dlxClass.obj" \
	"$(INTDIR)\testDLX.obj" \
	"$(INTDIR)\tester.obj" \
	"$(INTDIR)\testdlx.res" \
	"..\..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\..\lib\core.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\geom.lib"

"..\..\..\..\..\maxsdk\plugin\testdlx.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("TestDLX.dep")
!INCLUDE "TestDLX.dep"
!ELSE 
!MESSAGE Warning: cannot find "TestDLX.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TestDLX - Win32 Release" || "$(CFG)" == "TestDLX - Win32 Debug" || "$(CFG)" == "TestDLX - Win32 Hybrid"
SOURCE=.\dlxClass.cpp

"$(INTDIR)\dlxClass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\testDLX.cpp

"$(INTDIR)\testDLX.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\testdlx.rc

"$(INTDIR)\testdlx.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\tester.cpp

"$(INTDIR)\tester.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

