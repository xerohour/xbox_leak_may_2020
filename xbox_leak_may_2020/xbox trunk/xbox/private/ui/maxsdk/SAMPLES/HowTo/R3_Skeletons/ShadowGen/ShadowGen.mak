# Microsoft Developer Studio Generated NMAKE File, Based on ShadowGen.dsp
!IF "$(CFG)" == ""
CFG=ShadowGen - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to ShadowGen - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "ShadowGen - Win32 Release" && "$(CFG)" != "ShadowGen - Win32 Debug" && "$(CFG)" != "ShadowGen - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShadowGen.mak" CFG="ShadowGen - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShadowGen - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShadowGen - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShadowGen - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ShadowGen - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\ShadowGen_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\ShadowGen.obj"
	-@erase "$(INTDIR)\ShadowGen.res"
	-@erase "$(INTDIR)\ShadowShader.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.exp"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.lib"
	-@erase "..\..\..\..\plugin\ShadowGen_skeleton.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADOWGEN_EXPORTS" /Fp"$(INTDIR)\ShadowGen.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ShadowGen.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ShadowGen.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib core.lib geom.lib gfx.lib maxutil.lib bmm.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089C0000" /dll /incremental:no /pdb:"$(OUTDIR)\ShadowGen_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ShadowGen_skeleton.dlo" /implib:"$(OUTDIR)\ShadowGen_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ShadowGen.obj" \
	"$(INTDIR)\ShadowShader.obj" \
	"$(INTDIR)\ShadowGen.res"

"..\..\..\..\plugin\ShadowGen_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ShadowGen - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\ShadowGen_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\ShadowGen.obj"
	-@erase "$(INTDIR)\ShadowGen.res"
	-@erase "$(INTDIR)\ShadowShader.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.exp"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.lib"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ShadowGen_skeleton.dlo"
	-@erase "..\..\..\..\plugin\ShadowGen_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADOWGEN_EXPORTS" /Fp"$(INTDIR)\ShadowGen.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ShadowGen.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ShadowGen.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib core.lib geom.lib gfx.lib maxutil.lib bmm.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089C0000" /dll /incremental:yes /pdb:"$(OUTDIR)\ShadowGen_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ShadowGen_skeleton.dlo" /implib:"$(OUTDIR)\ShadowGen_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ShadowGen.obj" \
	"$(INTDIR)\ShadowShader.obj" \
	"$(INTDIR)\ShadowGen.res"

"..\..\..\..\plugin\ShadowGen_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ShadowGen - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\ShadowGen_skeleton.dlo"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\ShadowGen.obj"
	-@erase "$(INTDIR)\ShadowGen.res"
	-@erase "$(INTDIR)\ShadowShader.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.exp"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.lib"
	-@erase "$(OUTDIR)\ShadowGen_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ShadowGen_skeleton.dlo"
	-@erase "..\..\..\..\plugin\ShadowGen_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADOWGEN_EXPORTS" /Fp"$(INTDIR)\ShadowGen.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ShadowGen.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ShadowGen.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib core.lib geom.lib gfx.lib maxutil.lib bmm.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x089C0000" /dll /incremental:yes /pdb:"$(OUTDIR)\ShadowGen_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ShadowGen_skeleton.dlo" /implib:"$(OUTDIR)\ShadowGen_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ShadowGen.obj" \
	"$(INTDIR)\ShadowShader.obj" \
	"$(INTDIR)\ShadowGen.res"

"..\..\..\..\plugin\ShadowGen_skeleton.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ShadowGen.dep")
!INCLUDE "ShadowGen.dep"
!ELSE 
!MESSAGE Warning: cannot find "ShadowGen.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ShadowGen - Win32 Release" || "$(CFG)" == "ShadowGen - Win32 Debug" || "$(CFG)" == "ShadowGen - Win32 Hybrid"
SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShadowGen.cpp

"$(INTDIR)\ShadowGen.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShadowShader.cpp

"$(INTDIR)\ShadowShader.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShadowGen.rc

"$(INTDIR)\ShadowGen.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

