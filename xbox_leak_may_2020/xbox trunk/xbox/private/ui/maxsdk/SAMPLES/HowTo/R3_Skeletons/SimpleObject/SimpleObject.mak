# Microsoft Developer Studio Generated NMAKE File, Based on SimpleObject.dsp
!IF "$(CFG)" == ""
CFG=SimpleObject - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to SimpleObject - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "SimpleObject - Win32 Release" && "$(CFG)" != "SimpleObject - Win32 Debug" && "$(CFG)" != "SimpleObject - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SimpleObject.mak" CFG="SimpleObject - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimpleObject - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SimpleObject - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SimpleObject - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "SimpleObject - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\SimpleObject_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SimpleMesh.obj"
	-@erase "$(INTDIR)\SimpleObj.obj"
	-@erase "$(INTDIR)\SimpleObject.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.exp"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.lib"
	-@erase "..\..\..\..\plugin\SimpleObject_skeleton.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLEOBJECT_EXPORTS" /Fp"$(INTDIR)\SimpleObject.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SimpleObject.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SimpleObject.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089E0000" /dll /incremental:no /pdb:"$(OUTDIR)\SimpleObject_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SimpleObject_skeleton.dlo" /implib:"$(OUTDIR)\SimpleObject_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SimpleMesh.obj" \
	"$(INTDIR)\SimpleObj.obj" \
	"$(INTDIR)\SimpleObject.res"

"..\..\..\..\plugin\SimpleObject_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SimpleObject - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\SimpleObject_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SimpleMesh.obj"
	-@erase "$(INTDIR)\SimpleObj.obj"
	-@erase "$(INTDIR)\SimpleObject.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.exp"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.lib"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.pdb"
	-@erase "..\..\..\..\plugin\SimpleObject_skeleton.dlo"
	-@erase "..\..\..\..\plugin\SimpleObject_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLEOBJECT_EXPORTS" /Fp"$(INTDIR)\SimpleObject.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SimpleObject.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SimpleObject.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089E0000" /dll /incremental:yes /pdb:"$(OUTDIR)\SimpleObject_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SimpleObject_skeleton.dlo" /implib:"$(OUTDIR)\SimpleObject_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SimpleMesh.obj" \
	"$(INTDIR)\SimpleObj.obj" \
	"$(INTDIR)\SimpleObject.res"

"..\..\..\..\plugin\SimpleObject_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SimpleObject - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\SimpleObject_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\SimpleMesh.obj"
	-@erase "$(INTDIR)\SimpleObj.obj"
	-@erase "$(INTDIR)\SimpleObject.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.exp"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.lib"
	-@erase "$(OUTDIR)\SimpleObject_skeleton.pdb"
	-@erase "..\..\..\..\plugin\SimpleObject_skeleton.dlo"
	-@erase "..\..\..\..\plugin\SimpleObject_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLEOBJECT_EXPORTS" /Fp"$(INTDIR)\SimpleObject.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\SimpleObject.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SimpleObject.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089E0000" /dll /incremental:yes /pdb:"$(OUTDIR)\SimpleObject_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\SimpleObject_skeleton.dlo" /implib:"$(OUTDIR)\SimpleObject_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\SimpleMesh.obj" \
	"$(INTDIR)\SimpleObj.obj" \
	"$(INTDIR)\SimpleObject.res"

"..\..\..\..\plugin\SimpleObject_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("SimpleObject.dep")
!INCLUDE "SimpleObject.dep"
!ELSE 
!MESSAGE Warning: cannot find "SimpleObject.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SimpleObject - Win32 Release" || "$(CFG)" == "SimpleObject - Win32 Debug" || "$(CFG)" == "SimpleObject - Win32 Hybrid"
SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SimpleMesh.cpp

"$(INTDIR)\SimpleMesh.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SimpleObj.cpp

"$(INTDIR)\SimpleObj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SimpleObject.rc

"$(INTDIR)\SimpleObject.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

