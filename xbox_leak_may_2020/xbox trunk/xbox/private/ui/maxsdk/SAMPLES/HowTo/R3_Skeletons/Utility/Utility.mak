# Microsoft Developer Studio Generated NMAKE File, Based on Utility.dsp
!IF "$(CFG)" == ""
CFG=Utility - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Utility - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Utility - Win32 Release" && "$(CFG)" != "Utility - Win32 Debug" && "$(CFG)" != "Utility - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Utility.mak" CFG="Utility - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Utility - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Utility - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Utility - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Utility - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\Utility_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\Utility.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Utility_skeleton.exp"
	-@erase "$(OUTDIR)\Utility_skeleton.lib"
	-@erase "..\..\..\..\plugin\Utility_skeleton.dlu"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTILITY_EXPORTS" /Fp"$(INTDIR)\Utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Utility.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Utility.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08970000" /dll /incremental:no /pdb:"$(OUTDIR)\Utility_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Utility_skeleton.dlu" /implib:"$(OUTDIR)\Utility_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Utility.obj" \
	"$(INTDIR)\Utility.res"

"..\..\..\..\plugin\Utility_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Utility - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\Utility_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\Utility.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Utility_skeleton.exp"
	-@erase "$(OUTDIR)\Utility_skeleton.lib"
	-@erase "$(OUTDIR)\Utility_skeleton.pdb"
	-@erase "..\..\..\..\plugin\Utility_skeleton.dlu"
	-@erase "..\..\..\..\plugin\Utility_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTILITY_EXPORTS" /Fp"$(INTDIR)\Utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Utility.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Utility.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08970000" /dll /incremental:yes /pdb:"$(OUTDIR)\Utility_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Utility_skeleton.dlu" /implib:"$(OUTDIR)\Utility_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Utility.obj" \
	"$(INTDIR)\Utility.res"

"..\..\..\..\plugin\Utility_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Utility - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\Utility_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\Utility.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Utility_skeleton.exp"
	-@erase "$(OUTDIR)\Utility_skeleton.lib"
	-@erase "$(OUTDIR)\Utility_skeleton.pdb"
	-@erase "..\..\..\..\plugin\Utility_skeleton.dlu"
	-@erase "..\..\..\..\plugin\Utility_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTILITY_EXPORTS" /Fp"$(INTDIR)\Utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Utility.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Utility.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08970000" /dll /incremental:yes /pdb:"$(OUTDIR)\Utility_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Utility_skeleton.dlu" /implib:"$(OUTDIR)\Utility_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Utility.obj" \
	"$(INTDIR)\Utility.res"

"..\..\..\..\plugin\Utility_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Utility.dep")
!INCLUDE "Utility.dep"
!ELSE 
!MESSAGE Warning: cannot find "Utility.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Utility - Win32 Release" || "$(CFG)" == "Utility - Win32 Debug" || "$(CFG)" == "Utility - Win32 Hybrid"
SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Utility.cpp

"$(INTDIR)\Utility.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Utility.rc

"$(INTDIR)\Utility.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

