# Microsoft Developer Studio Generated NMAKE File, Based on CCUtil.dsp
!IF "$(CFG)" == ""
CFG=CCUtil - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to CCUtil - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "CCUtil - Win32 Release" && "$(CFG)" != "CCUtil - Win32 Debug" && "$(CFG)" != "CCUtil - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CCUtil.mak" CFG="CCUtil - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CCUtil - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CCUtil - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CCUtil - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CCUtil - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\plugin\CCUtil.dlu"


CLEAN :
	-@erase "$(INTDIR)\CCUtil.obj"
	-@erase "$(INTDIR)\CCUtil.res"
	-@erase "$(INTDIR)\DllEntry.obj"
	-@erase "$(OUTDIR)\CCUtil.exp"
	-@erase "$(OUTDIR)\CCUtil.lib"
	-@erase "..\..\..\plugin\CCUtil.dlu"
	-@erase ".\CCUtil.idb"
	-@erase ".\CCUtil.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /Zi /O2 /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\CCUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"CCUtil.pdb" /FD /release /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CCUtil.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CCUtil.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08760000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CCUtil.pdb" /machine:I386 /def:".\CCUtil.def" /out:"..\..\..\..\maxsdk\plugin\CCUtil.dlu" /implib:"$(OUTDIR)\CCUtil.lib" /libpath:"..\..\..\..\maxsdk\lib" /release 
DEF_FILE= \
	".\CCUtil.def"
LINK32_OBJS= \
	"$(INTDIR)\CCUtil.obj" \
	"$(INTDIR)\DllEntry.obj" \
	"$(INTDIR)\CCUtil.res"

"..\..\..\plugin\CCUtil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CCUtil - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\plugin\CCUtil.dlu"


CLEAN :
	-@erase "$(INTDIR)\CCUtil.obj"
	-@erase "$(INTDIR)\CCUtil.res"
	-@erase "$(INTDIR)\DllEntry.obj"
	-@erase "$(OUTDIR)\CCUtil.exp"
	-@erase "$(OUTDIR)\CCUtil.lib"
	-@erase "$(OUTDIR)\CCUtil.pdb"
	-@erase "..\..\..\plugin\CCUtil.dlu"
	-@erase "..\..\..\plugin\CCUtil.ilk"
	-@erase ".\CCUtil.idb"
	-@erase ".\CCUtil.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\CCUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"CCUtil.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CCUtil.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CCUtil.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08760000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CCUtil.pdb" /debug /machine:I386 /def:".\CCUtil.def" /out:"..\..\..\..\maxsdk\plugin\CCUtil.dlu" /implib:"$(OUTDIR)\CCUtil.lib" /pdbtype:sept /libpath:"..\..\..\..\maxsdk\lib" 
DEF_FILE= \
	".\CCUtil.def"
LINK32_OBJS= \
	"$(INTDIR)\CCUtil.obj" \
	"$(INTDIR)\DllEntry.obj" \
	"$(INTDIR)\CCUtil.res"

"..\..\..\plugin\CCUtil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CCUtil - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\plugin\CCUtil.dlu"


CLEAN :
	-@erase "$(INTDIR)\CCUtil.obj"
	-@erase "$(INTDIR)\CCUtil.res"
	-@erase "$(INTDIR)\DllEntry.obj"
	-@erase "$(OUTDIR)\CCUtil.exp"
	-@erase "$(OUTDIR)\CCUtil.lib"
	-@erase "$(OUTDIR)\CCUtil.pdb"
	-@erase "..\..\..\plugin\CCUtil.dlu"
	-@erase "..\..\..\plugin\CCUtil.ilk"
	-@erase ".\CCUtil.idb"
	-@erase ".\CCUtil.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\CCUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"CCUtil.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CCUtil.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CCUtil.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08760000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CCUtil.pdb" /debug /machine:I386 /def:".\CCUtil.def" /out:"..\..\..\..\maxsdk\plugin\CCUtil.dlu" /implib:"$(OUTDIR)\CCUtil.lib" /pdbtype:sept /libpath:"..\..\..\..\maxsdk\lib" 
DEF_FILE= \
	".\CCUtil.def"
LINK32_OBJS= \
	"$(INTDIR)\CCUtil.obj" \
	"$(INTDIR)\DllEntry.obj" \
	"$(INTDIR)\CCUtil.res"

"..\..\..\plugin\CCUtil.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("CCUtil.dep")
!INCLUDE "CCUtil.dep"
!ELSE 
!MESSAGE Warning: cannot find "CCUtil.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CCUtil - Win32 Release" || "$(CFG)" == "CCUtil - Win32 Debug" || "$(CFG)" == "CCUtil - Win32 Hybrid"
SOURCE=.\CCUtil.cpp

"$(INTDIR)\CCUtil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CCUtil.rc

"$(INTDIR)\CCUtil.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\DllEntry.cpp

"$(INTDIR)\DllEntry.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

