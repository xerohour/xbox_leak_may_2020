# Microsoft Developer Studio Generated NMAKE File, Based on ObjectSnap.dsp
!IF "$(CFG)" == ""
CFG=ObjectSnap - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to ObjectSnap - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "ObjectSnap - Win32 Release" && "$(CFG)" != "ObjectSnap - Win32 Debug" && "$(CFG)" != "ObjectSnap - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ObjectSnap.mak" CFG="ObjectSnap - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ObjectSnap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ObjectSnap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ObjectSnap - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ObjectSnap - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\ObjectSnap_skeleton.dls"


CLEAN :
	-@erase "$(INTDIR)\ObjectSnap.obj"
	-@erase "$(INTDIR)\ObjectSnap.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.exp"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.lib"
	-@erase "..\..\..\..\plugin\ObjectSnap_skeleton.dls"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTSNAP_EXPORTS" /Fp"$(INTDIR)\ObjectSnap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ObjectSnap.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ObjectSnap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08CB0000" /dll /incremental:no /pdb:"$(OUTDIR)\ObjectSnap_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ObjectSnap_skeleton.dls" /implib:"$(OUTDIR)\ObjectSnap_skeleton.lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ObjectSnap.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ObjectSnap.res" \
	"..\..\..\..\lib\geom.lib" \
	"..\..\..\..\lib\gfx.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\mesh.lib" \
	"..\..\..\..\lib\core.lib"

"..\..\..\..\plugin\ObjectSnap_skeleton.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ObjectSnap - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\ObjectSnap_skeleton.dls"


CLEAN :
	-@erase "$(INTDIR)\ObjectSnap.obj"
	-@erase "$(INTDIR)\ObjectSnap.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.exp"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.lib"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ObjectSnap_skeleton.dls"
	-@erase "..\..\..\..\plugin\ObjectSnap_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTSNAP_EXPORTS" /Fp"$(INTDIR)\ObjectSnap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ObjectSnap.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ObjectSnap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08CB0000" /dll /incremental:yes /pdb:"$(OUTDIR)\ObjectSnap_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ObjectSnap_skeleton.dls" /implib:"$(OUTDIR)\ObjectSnap_skeleton.lib" /pdbtype:sept 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ObjectSnap.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ObjectSnap.res" \
	"..\..\..\..\lib\geom.lib" \
	"..\..\..\..\lib\gfx.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\mesh.lib" \
	"..\..\..\..\lib\core.lib"

"..\..\..\..\plugin\ObjectSnap_skeleton.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ObjectSnap - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\ObjectSnap_skeleton.dls"


CLEAN :
	-@erase "$(INTDIR)\ObjectSnap.obj"
	-@erase "$(INTDIR)\ObjectSnap.res"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.exp"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.lib"
	-@erase "$(OUTDIR)\ObjectSnap_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ObjectSnap_skeleton.dls"
	-@erase "..\..\..\..\plugin\ObjectSnap_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTSNAP_EXPORTS" /Fp"$(INTDIR)\ObjectSnap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ObjectSnap.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ObjectSnap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08CB0000" /dll /incremental:yes /pdb:"$(OUTDIR)\ObjectSnap_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ObjectSnap_skeleton.dls" /implib:"$(OUTDIR)\ObjectSnap_skeleton.lib" /pdbtype:sept 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"..\..\..\..\lib\geom.lib" \
	"..\..\..\..\lib\gfx.lib" \
	"..\..\..\..\lib\maxutil.lib" \
	"..\..\..\..\lib\mesh.lib" \
	"..\..\..\..\lib\core.lib" \
	"$(INTDIR)\ObjectSnap.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ObjectSnap.res"

"..\..\..\..\plugin\ObjectSnap_skeleton.dls" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ObjectSnap.dep")
!INCLUDE "ObjectSnap.dep"
!ELSE 
!MESSAGE Warning: cannot find "ObjectSnap.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ObjectSnap - Win32 Release" || "$(CFG)" == "ObjectSnap - Win32 Debug" || "$(CFG)" == "ObjectSnap - Win32 Hybrid"
SOURCE=.\ObjectSnap.cpp

"$(INTDIR)\ObjectSnap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ObjectSnap.rc

"$(INTDIR)\ObjectSnap.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

