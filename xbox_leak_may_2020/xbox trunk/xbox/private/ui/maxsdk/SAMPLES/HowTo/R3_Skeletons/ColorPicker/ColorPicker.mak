# Microsoft Developer Studio Generated NMAKE File, Based on ColorPicker.dsp
!IF "$(CFG)" == ""
CFG=ColorPicker - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to ColorPicker - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "ColorPicker - Win32 Release" && "$(CFG)" != "ColorPicker - Win32 Debug" && "$(CFG)" != "ColorPicker - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ColorPicker.mak" CFG="ColorPicker - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ColorPicker - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ColorPicker - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ColorPicker - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ColorPicker - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\ColorPicker_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\ColorPicker.obj"
	-@erase "$(INTDIR)\ColorPicker.res"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.exp"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.lib"
	-@erase "..\..\..\..\plugin\ColorPicker_skeleton.dlu"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\ColorPicker.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /D /release"COLORPICKER_EXPORTS" /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ColorPicker.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ColorPicker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08790000" /dll /incremental:no /pdb:"$(OUTDIR)\ColorPicker_skeleton.pdb" /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ColorPicker_skeleton.dlu" /implib:"$(OUTDIR)\ColorPicker_skeleton.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ColorPicker.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ColorPicker.res"

"..\..\..\..\plugin\ColorPicker_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ColorPicker - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\ColorPicker_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\ColorPicker.obj"
	-@erase "$(INTDIR)\ColorPicker.res"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.exp"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.lib"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ColorPicker_skeleton.dlu"
	-@erase "..\..\..\..\plugin\ColorPicker_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COLORPICKER_EXPORTS" /Fp"$(INTDIR)\ColorPicker.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ColorPicker.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ColorPicker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08790000" /dll /incremental:yes /pdb:"$(OUTDIR)\ColorPicker_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ColorPicker_skeleton.dlu" /implib:"$(OUTDIR)\ColorPicker_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ColorPicker.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ColorPicker.res"

"..\..\..\..\plugin\ColorPicker_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ColorPicker - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\ColorPicker_skeleton.dlu"


CLEAN :
	-@erase "$(INTDIR)\ColorPicker.obj"
	-@erase "$(INTDIR)\ColorPicker.res"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\Plugin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.exp"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.lib"
	-@erase "$(OUTDIR)\ColorPicker_skeleton.pdb"
	-@erase "..\..\..\..\plugin\ColorPicker_skeleton.dlu"
	-@erase "..\..\..\..\plugin\ColorPicker_skeleton.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COLORPICKER_EXPORTS" /Fp"$(INTDIR)\ColorPicker.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ColorPicker.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ColorPicker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x08790000" /dll /incremental:yes /pdb:"$(OUTDIR)\ColorPicker_skeleton.pdb" /debug /machine:I386 /def:".\Plugin.def" /out:"..\..\..\..\plugin\ColorPicker_skeleton.dlu" /implib:"$(OUTDIR)\ColorPicker_skeleton.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\Plugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ColorPicker.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\Plugin.obj" \
	"$(INTDIR)\ColorPicker.res"

"..\..\..\..\plugin\ColorPicker_skeleton.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ColorPicker.dep")
!INCLUDE "ColorPicker.dep"
!ELSE 
!MESSAGE Warning: cannot find "ColorPicker.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ColorPicker - Win32 Release" || "$(CFG)" == "ColorPicker - Win32 Debug" || "$(CFG)" == "ColorPicker - Win32 Hybrid"
SOURCE=.\ColorPicker.cpp

"$(INTDIR)\ColorPicker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Dialog.cpp

"$(INTDIR)\Dialog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Plugin.cpp

"$(INTDIR)\Plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ColorPicker.rc

"$(INTDIR)\ColorPicker.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

