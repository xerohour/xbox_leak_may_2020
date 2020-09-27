# Microsoft Developer Studio Generated NMAKE File, Based on VertexPaint.dsp
!IF "$(CFG)" == ""
CFG=VertexPaint - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to VertexPaint - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "VertexPaint - Win32 Release" && "$(CFG)" != "VertexPaint - Win32 Debug" && "$(CFG)" != "VertexPaint - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VertexPaint.mak" CFG="VertexPaint - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VertexPaint - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VertexPaint - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VertexPaint - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "VertexPaint - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"


CLEAN :
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\Paint.obj"
	-@erase "$(INTDIR)\VertexPaint.obj"
	-@erase "$(INTDIR)\VertexPaint.res"
	-@erase "$(OUTDIR)\VertexPaint.exp"
	-@erase "$(OUTDIR)\VertexPaint.lib"
	-@erase "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"
	-@erase ".\VertexPaint.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\VertexPaint.pch" /YX /Fo"$(INTDIR)\\" /Fd"VertexPaint.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\VertexPaint.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VertexPaint.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05D10000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\VertexPaint.pdb" /machine:I386 /def:".\VertexPaint.def" /out:"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" /implib:"$(OUTDIR)\VertexPaint.lib" /release 
DEF_FILE= \
	".\VertexPaint.def"
LINK32_OBJS= \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\Paint.obj" \
	"$(INTDIR)\VertexPaint.obj" \
	"$(INTDIR)\VertexPaint.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib"

"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "VertexPaint - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"


CLEAN :
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\Paint.obj"
	-@erase "$(INTDIR)\VertexPaint.obj"
	-@erase "$(INTDIR)\VertexPaint.res"
	-@erase "$(OUTDIR)\VertexPaint.exp"
	-@erase "$(OUTDIR)\VertexPaint.lib"
	-@erase "$(OUTDIR)\VertexPaint.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\VertexPaint.ilk"
	-@erase ".\VertexPaint.idb"
	-@erase ".\VertexPaint.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\VertexPaint.pch" /YX /Fo"$(INTDIR)\\" /Fd"VertexPaint.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\VertexPaint.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VertexPaint.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05D10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\VertexPaint.pdb" /debug /machine:I386 /def:".\VertexPaint.def" /out:"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" /implib:"$(OUTDIR)\VertexPaint.lib" /pdbtype:sept 
DEF_FILE= \
	".\VertexPaint.def"
LINK32_OBJS= \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\Paint.obj" \
	"$(INTDIR)\VertexPaint.obj" \
	"$(INTDIR)\VertexPaint.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib"

"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "VertexPaint - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"


CLEAN :
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\Paint.obj"
	-@erase "$(INTDIR)\VertexPaint.obj"
	-@erase "$(INTDIR)\VertexPaint.res"
	-@erase "$(OUTDIR)\VertexPaint.exp"
	-@erase "$(OUTDIR)\VertexPaint.lib"
	-@erase "$(OUTDIR)\VertexPaint.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\VertexPaint.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\VertexPaint.ilk"
	-@erase ".\VertexPaint.idb"
	-@erase ".\VertexPaint.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\VertexPaint.pch" /YX /Fo"$(INTDIR)\\" /Fd"VertexPaint.pdb" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\VertexPaint.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VertexPaint.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05D10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\VertexPaint.pdb" /debug /machine:I386 /def:".\VertexPaint.def" /out:"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" /implib:"$(OUTDIR)\VertexPaint.lib" /pdbtype:sept 
DEF_FILE= \
	".\VertexPaint.def"
LINK32_OBJS= \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\Paint.obj" \
	"$(INTDIR)\VertexPaint.obj" \
	"$(INTDIR)\VertexPaint.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\mesh.lib"

"..\..\..\..\maxsdk\plugin\VertexPaint.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("VertexPaint.dep")
!INCLUDE "VertexPaint.dep"
!ELSE 
!MESSAGE Warning: cannot find "VertexPaint.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "VertexPaint - Win32 Release" || "$(CFG)" == "VertexPaint - Win32 Debug" || "$(CFG)" == "VertexPaint - Win32 Hybrid"
SOURCE=.\dllmain.cpp

"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Paint.cpp

"$(INTDIR)\Paint.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\VertexPaint.cpp

"$(INTDIR)\VertexPaint.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\VertexPaint.rc

"$(INTDIR)\VertexPaint.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

