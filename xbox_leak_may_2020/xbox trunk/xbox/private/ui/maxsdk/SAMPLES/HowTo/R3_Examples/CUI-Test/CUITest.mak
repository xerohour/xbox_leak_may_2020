# Microsoft Developer Studio Generated NMAKE File, Based on CUITest.dsp
!IF "$(CFG)" == ""
CFG=CUITest - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to CUITest - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "CUITest - Win32 Release" && "$(CFG)" != "CUITest - Win32 Debug" && "$(CFG)" != "CUITest - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CUITest.mak" CFG="CUITest - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CUITest - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUITest - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUITest - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "CUITest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\CUITest.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUITest.obj"
	-@erase "$(INTDIR)\CUITest.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CUITest.exp"
	-@erase "$(OUTDIR)\CUITest.lib"
	-@erase "..\..\..\..\plugin\CUITest.dlu"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /Zi /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\CUITest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUITest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CUITest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib /nologo /base:"0x087B0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CUITest.pdb" /machine:I386 /def:".\CUITest.def" /out:"..\..\..\..\plugin\CUITest.dlu" /implib:"$(OUTDIR)\CUITest.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\CUITest.def"
LINK32_OBJS= \
	"$(INTDIR)\CUITest.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\CUITest.res"

"..\..\..\..\plugin\CUITest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CUITest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\CUITest.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUITest.obj"
	-@erase "$(INTDIR)\CUITest.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CUITest.exp"
	-@erase "$(OUTDIR)\CUITest.lib"
	-@erase "$(OUTDIR)\CUITest.pdb"
	-@erase "..\..\..\..\plugin\CUITest.dlu"
	-@erase "..\..\..\..\plugin\CUITest.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\CUITest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUITest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CUITest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib /nologo /base:"0x087B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CUITest.pdb" /debug /machine:I386 /def:".\CUITest.def" /out:"..\..\..\..\plugin\CUITest.dlu" /implib:"$(OUTDIR)\CUITest.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\CUITest.def"
LINK32_OBJS= \
	"$(INTDIR)\CUITest.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\CUITest.res"

"..\..\..\..\plugin\CUITest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CUITest - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\CUITest.dlu"


CLEAN :
	-@erase "$(INTDIR)\CUITest.obj"
	-@erase "$(INTDIR)\CUITest.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CUITest.exp"
	-@erase "$(OUTDIR)\CUITest.lib"
	-@erase "$(OUTDIR)\CUITest.pdb"
	-@erase "..\..\..\..\plugin\CUITest.dlu"
	-@erase "..\..\..\..\plugin\CUITest.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\CUITest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CUITest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CUITest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib /nologo /base:"0x087B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CUITest.pdb" /debug /machine:I386 /def:".\CUITest.def" /out:"..\..\..\..\plugin\CUITest.dlu" /implib:"$(OUTDIR)\CUITest.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\CUITest.def"
LINK32_OBJS= \
	"$(INTDIR)\CUITest.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\CUITest.res"

"..\..\..\..\plugin\CUITest.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("CUITest.dep")
!INCLUDE "CUITest.dep"
!ELSE 
!MESSAGE Warning: cannot find "CUITest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CUITest - Win32 Release" || "$(CFG)" == "CUITest - Win32 Debug" || "$(CFG)" == "CUITest - Win32 Hybrid"
SOURCE=.\CUITest.cpp

"$(INTDIR)\CUITest.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CUITest.rc

"$(INTDIR)\CUITest.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

