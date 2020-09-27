# Microsoft Developer Studio Generated NMAKE File, Based on Atmospheric.dsp
!IF "$(CFG)" == ""
CFG=Atmospheric - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Atmospheric - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Atmospheric - Win32 Release" && "$(CFG)" != "Atmospheric - Win32 Debug" && "$(CFG)" != "Atmospheric - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Atmospheric.mak" CFG="Atmospheric - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Atmospheric - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Atmospheric - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Atmospheric - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Atmospheric - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\Atmospheric_skeleton.dlr"


CLEAN :
	-@erase "$(INTDIR)\Atmospheric.obj"
	-@erase "$(INTDIR)\Atmospheric.res"
	-@erase "$(INTDIR)\AtmosShade.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.exp"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.lib"
	-@erase "..\..\..\..\plugin\Atmospheric_skeleton.dlr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATMOSPHERIC_EXPORTS" /Fp"$(INTDIR)\Atmospheric.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Atmospheric.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Atmospheric.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=paramblk2.lib maxutil.lib core.lib geom.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08C40000" /dll /incremental:no /pdb:"$(OUTDIR)\Atmospheric_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Atmospheric_skeleton.dlr" /implib:"$(OUTDIR)\Atmospheric_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Atmospheric.obj" \
	"$(INTDIR)\AtmosShade.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Atmospheric.res"

"..\..\..\..\plugin\Atmospheric_skeleton.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Atmospheric - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\Atmospheric_skeleton.dlr"


CLEAN :
	-@erase "$(INTDIR)\Atmospheric.obj"
	-@erase "$(INTDIR)\Atmospheric.res"
	-@erase "$(INTDIR)\AtmosShade.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.exp"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.lib"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.pdb"
	-@erase "..\..\..\..\plugin\Atmospheric_skeleton.dlr"
	-@erase "..\..\..\..\plugin\Atmospheric_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATMOSPHERIC_EXPORTS" /Fp"$(INTDIR)\Atmospheric.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Atmospheric.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Atmospheric.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=paramblk2.lib maxutil.lib core.lib geom.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08C40000" /dll /incremental:yes /pdb:"$(OUTDIR)\Atmospheric_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Atmospheric_skeleton.dlr" /implib:"$(OUTDIR)\Atmospheric_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Atmospheric.obj" \
	"$(INTDIR)\AtmosShade.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Atmospheric.res"

"..\..\..\..\plugin\Atmospheric_skeleton.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Atmospheric - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\Atmospheric_skeleton.dlr"


CLEAN :
	-@erase "$(INTDIR)\Atmospheric.obj"
	-@erase "$(INTDIR)\Atmospheric.res"
	-@erase "$(INTDIR)\AtmosShade.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.exp"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.lib"
	-@erase "$(OUTDIR)\Atmospheric_skeleton.pdb"
	-@erase "..\..\..\..\plugin\Atmospheric_skeleton.dlr"
	-@erase "..\..\..\..\plugin\Atmospheric_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATMOSPHERIC_EXPORTS" /Fp"$(INTDIR)\Atmospheric.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Atmospheric.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Atmospheric.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=paramblk2.lib maxutil.lib core.lib geom.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08C40000" /dll /incremental:yes /pdb:"$(OUTDIR)\Atmospheric_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\Atmospheric_skeleton.dlr" /implib:"$(OUTDIR)\Atmospheric_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\Atmospheric.obj" \
	"$(INTDIR)\AtmosShade.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\Atmospheric.res"

"..\..\..\..\plugin\Atmospheric_skeleton.dlr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Atmospheric.dep")
!INCLUDE "Atmospheric.dep"
!ELSE 
!MESSAGE Warning: cannot find "Atmospheric.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Atmospheric - Win32 Release" || "$(CFG)" == "Atmospheric - Win32 Debug" || "$(CFG)" == "Atmospheric - Win32 Hybrid"
SOURCE=.\Atmospheric.cpp

"$(INTDIR)\Atmospheric.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AtmosShade.cpp

"$(INTDIR)\AtmosShade.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Atmospheric.rc

"$(INTDIR)\Atmospheric.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

