# Microsoft Developer Studio Generated NMAKE File, Based on reactor.dsp
!IF "$(CFG)" == ""
CFG=reactor - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to reactor - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "reactor - Win32 Release" && "$(CFG)" != "reactor - Win32 Debug" && "$(CFG)" != "reactor - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "reactor.mak" CFG="reactor - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "reactor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "reactor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "reactor - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "reactor - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\reactor.dlc"


CLEAN :
	-@erase "$(INTDIR)\reactor.obj"
	-@erase "$(INTDIR)\reactor.res"
	-@erase "$(INTDIR)\reactorMain.obj"
	-@erase "$(INTDIR)\reactorui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\reactor.exp"
	-@erase "$(OUTDIR)\reactor.lib"
	-@erase "..\..\..\..\maxsdk\plugin\reactor.dlc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /Ob0 /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /I "..\..\..\..\dll\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\reactor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\reactor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB core.lib expr.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08680000" /subsystem:windows /dll /pdb:none /machine:I386 /def:".\reactor.def" /out:"..\..\..\..\maxsdk\plugin\reactor.dlc" /implib:"$(OUTDIR)\reactor.lib" /libpath:"..\..\..\lib" /release 
DEF_FILE= \
	".\reactor.def"
LINK32_OBJS= \
	"$(INTDIR)\reactor.obj" \
	"$(INTDIR)\reactorMain.obj" \
	"$(INTDIR)\reactorui.obj" \
	"$(INTDIR)\reactor.res"

"..\..\..\..\maxsdk\plugin\reactor.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "reactor - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\reactor.dlc"


CLEAN :
	-@erase "$(INTDIR)\reactor.obj"
	-@erase "$(INTDIR)\reactor.res"
	-@erase "$(INTDIR)\reactorMain.obj"
	-@erase "$(INTDIR)\reactorui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\reactor.exp"
	-@erase "$(OUTDIR)\reactor.lib"
	-@erase "$(OUTDIR)\reactor.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\reactor.dlc"
	-@erase "..\..\..\..\maxsdk\plugin\reactor.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /I "..\..\..\..\dll\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\reactor.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\reactor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB core.lib expr.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08680000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\reactor.pdb" /debug /machine:I386 /def:".\reactor.def" /out:"..\..\..\..\maxsdk\plugin\reactor.dlc" /implib:"$(OUTDIR)\reactor.lib" /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\reactor.def"
LINK32_OBJS= \
	"$(INTDIR)\reactor.obj" \
	"$(INTDIR)\reactorMain.obj" \
	"$(INTDIR)\reactorui.obj" \
	"$(INTDIR)\reactor.res"

"..\..\..\..\maxsdk\plugin\reactor.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "reactor - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\reactor.dlc"


CLEAN :
	-@erase "$(INTDIR)\reactor.obj"
	-@erase "$(INTDIR)\reactor.res"
	-@erase "$(INTDIR)\reactorMain.obj"
	-@erase "$(INTDIR)\reactorui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\reactor.exp"
	-@erase "$(OUTDIR)\reactor.lib"
	-@erase "$(OUTDIR)\reactor.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\reactor.dlc"
	-@erase "..\..\..\..\maxsdk\plugin\reactor.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\reactor.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\reactor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB core.lib expr.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08680000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\reactor.pdb" /debug /machine:I386 /def:".\reactor.def" /out:"..\..\..\..\maxsdk\plugin\reactor.dlc" /implib:"$(OUTDIR)\reactor.lib" /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\reactor.def"
LINK32_OBJS= \
	"$(INTDIR)\reactor.obj" \
	"$(INTDIR)\reactorMain.obj" \
	"$(INTDIR)\reactorui.obj" \
	"$(INTDIR)\reactor.res"

"..\..\..\..\maxsdk\plugin\reactor.dlc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("reactor.dep")
!INCLUDE "reactor.dep"
!ELSE 
!MESSAGE Warning: cannot find "reactor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "reactor - Win32 Release" || "$(CFG)" == "reactor - Win32 Debug" || "$(CFG)" == "reactor - Win32 Hybrid"
SOURCE=.\reactor.cpp

"$(INTDIR)\reactor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\reactorMain.cpp

"$(INTDIR)\reactorMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\reactorui.cpp

"$(INTDIR)\reactorui.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\reactor.rc

"$(INTDIR)\reactor.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

