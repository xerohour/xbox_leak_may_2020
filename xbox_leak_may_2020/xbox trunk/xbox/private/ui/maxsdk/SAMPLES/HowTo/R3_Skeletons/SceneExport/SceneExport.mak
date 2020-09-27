# Microsoft Developer Studio Generated NMAKE File, Based on SceneExport.dsp
!IF "$(CFG)" == ""
CFG=SceneExport - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to SceneExport - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "SceneExport - Win32 Release" && "$(CFG)" != "SceneExport - Win32 Debug" && "$(CFG)" != "SceneExport - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SceneExport.mak" CFG="SceneExport - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SceneExport - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SceneExport - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SceneExport - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "SceneExport - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\SceneExport_skeleton.dle"


CLEAN :
	-@erase "$(INTDIR)\ConfigMgr.obj"
	-@erase "$(INTDIR)\DoExport.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SceneExport.obj"
	-@erase "$(INTDIR)\SceneExport.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SceneExport_skeleton.exp"
	-@erase "$(OUTDIR)\SceneExport_skeleton.lib"
	-@erase "..\..\..\..\plugin\SceneExport_skeleton.dle"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCENEEXPORT_EXPORTS" /Fp"$(INTDIR)\SceneExport.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SceneExport.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SceneExport.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib bmm.lib maxutil.lib mesh.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08BB0000" /dll /incremental:no /pdb:"$(OUTDIR)\SceneExport_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SceneExport_skeleton.dle" /implib:"$(OUTDIR)\SceneExport_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfigMgr.obj" \
	"$(INTDIR)\DoExport.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SceneExport.obj" \
	"$(INTDIR)\SceneExport.res"

"..\..\..\..\plugin\SceneExport_skeleton.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SceneExport - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\SceneExport_skeleton.dle"


CLEAN :
	-@erase "$(INTDIR)\ConfigMgr.obj"
	-@erase "$(INTDIR)\DoExport.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SceneExport.obj"
	-@erase "$(INTDIR)\SceneExport.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SceneExport_skeleton.exp"
	-@erase "$(OUTDIR)\SceneExport_skeleton.lib"
	-@erase "$(OUTDIR)\SceneExport_skeleton.pdb"
	-@erase "..\..\..\..\plugin\SceneExport_skeleton.dle"
	-@erase "..\..\..\..\plugin\SceneExport_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCENEEXPORT_EXPORTS" /Fp"$(INTDIR)\SceneExport.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SceneExport.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SceneExport.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib bmm.lib maxutil.lib mesh.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08BB0000" /dll /incremental:yes /pdb:"$(OUTDIR)\SceneExport_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SceneExport_skeleton.dle" /implib:"$(OUTDIR)\SceneExport_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfigMgr.obj" \
	"$(INTDIR)\DoExport.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SceneExport.obj" \
	"$(INTDIR)\SceneExport.res"

"..\..\..\..\plugin\SceneExport_skeleton.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SceneExport - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\SceneExport_skeleton.dle"


CLEAN :
	-@erase "$(INTDIR)\ConfigMgr.obj"
	-@erase "$(INTDIR)\DoExport.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SceneExport.obj"
	-@erase "$(INTDIR)\SceneExport.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SceneExport_skeleton.exp"
	-@erase "$(OUTDIR)\SceneExport_skeleton.lib"
	-@erase "$(OUTDIR)\SceneExport_skeleton.pdb"
	-@erase "..\..\..\..\plugin\SceneExport_skeleton.dle"
	-@erase "..\..\..\..\plugin\SceneExport_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCENEEXPORT_EXPORTS" /Fp"$(INTDIR)\SceneExport.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SceneExport.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SceneExport.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=core.lib geom.lib gfx.lib bmm.lib maxutil.lib mesh.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08BB0000" /dll /incremental:yes /pdb:"$(OUTDIR)\SceneExport_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SceneExport_skeleton.dle" /implib:"$(OUTDIR)\SceneExport_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfigMgr.obj" \
	"$(INTDIR)\DoExport.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SceneExport.obj" \
	"$(INTDIR)\SceneExport.res"

"..\..\..\..\plugin\SceneExport_skeleton.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("SceneExport.dep")
!INCLUDE "SceneExport.dep"
!ELSE 
!MESSAGE Warning: cannot find "SceneExport.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SceneExport - Win32 Release" || "$(CFG)" == "SceneExport - Win32 Debug" || "$(CFG)" == "SceneExport - Win32 Hybrid"
SOURCE=.\ConfigMgr.cpp

"$(INTDIR)\ConfigMgr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DoExport.cpp

"$(INTDIR)\DoExport.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SceneExport.cpp

"$(INTDIR)\SceneExport.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SceneExport.rc

"$(INTDIR)\SceneExport.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

