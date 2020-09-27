# Microsoft Developer Studio Generated NMAKE File, Based on Morpher.dsp
!IF "$(CFG)" == ""
CFG=Morpher - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Morpher - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Morpher - Win32 Release" && "$(CFG)" != "Morpher - Win32 Debug" && "$(CFG)" != "Morpher - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Morpher.mak" CFG="Morpher - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Morpher - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Morpher - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Morpher - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Morpher - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\Morpher.dlm"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wM3.res"
	-@erase "$(INTDIR)\wM3_cache.obj"
	-@erase "$(INTDIR)\wM3_channel.obj"
	-@erase "$(INTDIR)\wM3_core.obj"
	-@erase "$(INTDIR)\wM3_dlg-fns.obj"
	-@erase "$(INTDIR)\wM3_dlg.obj"
	-@erase "$(INTDIR)\wM3_impexp.obj"
	-@erase "$(INTDIR)\wM3_io.obj"
	-@erase "$(INTDIR)\wM3_main.obj"
	-@erase "$(INTDIR)\wM3_material.obj"
	-@erase "$(INTDIR)\wM3_material_dlg.obj"
	-@erase "$(INTDIR)\wM3_material_pick.obj"
	-@erase "$(INTDIR)\wM3_pick.obj"
	-@erase "$(INTDIR)\wM3_plugin.obj"
	-@erase "$(INTDIR)\wM3_subdlg.obj"
	-@erase "$(INTDIR)\wM3_undo.obj"
	-@erase "$(OUTDIR)\Morpher.exp"
	-@erase "$(OUTDIR)\Morpher.lib"
	-@erase "..\..\..\..\maxsdk\plugin\Morpher.dlm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Morpher.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\wM3.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Morpher.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Morpher.pdb" /machine:I386 /def:".\wM3.def" /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /implib:"$(OUTDIR)\Morpher.lib" /libpath:"..\..\..\lib" /release 
DEF_FILE= \
	".\wM3.def"
LINK32_OBJS= \
	"$(INTDIR)\wM3_cache.obj" \
	"$(INTDIR)\wM3_channel.obj" \
	"$(INTDIR)\wM3_dlg-fns.obj" \
	"$(INTDIR)\wM3_dlg.obj" \
	"$(INTDIR)\wM3_impexp.obj" \
	"$(INTDIR)\wM3_io.obj" \
	"$(INTDIR)\wM3_main.obj" \
	"$(INTDIR)\wM3_pick.obj" \
	"$(INTDIR)\wM3_plugin.obj" \
	"$(INTDIR)\wM3_subdlg.obj" \
	"$(INTDIR)\wM3_undo.obj" \
	"$(INTDIR)\wM3_core.obj" \
	"$(INTDIR)\wM3_material.obj" \
	"$(INTDIR)\wM3_material_dlg.obj" \
	"$(INTDIR)\wM3_material_pick.obj" \
	"$(INTDIR)\wM3.res"

"..\..\..\..\maxsdk\plugin\Morpher.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Morpher - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\Morpher.dlm"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wM3.res"
	-@erase "$(INTDIR)\wM3_cache.obj"
	-@erase "$(INTDIR)\wM3_channel.obj"
	-@erase "$(INTDIR)\wM3_core.obj"
	-@erase "$(INTDIR)\wM3_dlg-fns.obj"
	-@erase "$(INTDIR)\wM3_dlg.obj"
	-@erase "$(INTDIR)\wM3_impexp.obj"
	-@erase "$(INTDIR)\wM3_io.obj"
	-@erase "$(INTDIR)\wM3_main.obj"
	-@erase "$(INTDIR)\wM3_material.obj"
	-@erase "$(INTDIR)\wM3_material_dlg.obj"
	-@erase "$(INTDIR)\wM3_material_pick.obj"
	-@erase "$(INTDIR)\wM3_pick.obj"
	-@erase "$(INTDIR)\wM3_plugin.obj"
	-@erase "$(INTDIR)\wM3_subdlg.obj"
	-@erase "$(INTDIR)\wM3_undo.obj"
	-@erase "$(OUTDIR)\Morpher.exp"
	-@erase "$(OUTDIR)\Morpher.lib"
	-@erase "$(OUTDIR)\Morpher.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Morpher.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\Morpher.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Morpher.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\wM3.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Morpher.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Morpher.pdb" /debug /machine:I386 /def:".\wM3.def" /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /implib:"$(OUTDIR)\Morpher.lib" /pdbtype:sept /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\wM3.def"
LINK32_OBJS= \
	"$(INTDIR)\wM3_cache.obj" \
	"$(INTDIR)\wM3_channel.obj" \
	"$(INTDIR)\wM3_dlg-fns.obj" \
	"$(INTDIR)\wM3_dlg.obj" \
	"$(INTDIR)\wM3_impexp.obj" \
	"$(INTDIR)\wM3_io.obj" \
	"$(INTDIR)\wM3_main.obj" \
	"$(INTDIR)\wM3_pick.obj" \
	"$(INTDIR)\wM3_plugin.obj" \
	"$(INTDIR)\wM3_subdlg.obj" \
	"$(INTDIR)\wM3_undo.obj" \
	"$(INTDIR)\wM3_core.obj" \
	"$(INTDIR)\wM3_material.obj" \
	"$(INTDIR)\wM3_material_dlg.obj" \
	"$(INTDIR)\wM3_material_pick.obj" \
	"$(INTDIR)\wM3.res"

"..\..\..\..\maxsdk\plugin\Morpher.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Morpher - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\Morpher.dlm"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wM3.res"
	-@erase "$(INTDIR)\wM3_cache.obj"
	-@erase "$(INTDIR)\wM3_channel.obj"
	-@erase "$(INTDIR)\wM3_core.obj"
	-@erase "$(INTDIR)\wM3_dlg-fns.obj"
	-@erase "$(INTDIR)\wM3_dlg.obj"
	-@erase "$(INTDIR)\wM3_impexp.obj"
	-@erase "$(INTDIR)\wM3_io.obj"
	-@erase "$(INTDIR)\wM3_main.obj"
	-@erase "$(INTDIR)\wM3_material.obj"
	-@erase "$(INTDIR)\wM3_material_dlg.obj"
	-@erase "$(INTDIR)\wM3_material_pick.obj"
	-@erase "$(INTDIR)\wM3_pick.obj"
	-@erase "$(INTDIR)\wM3_plugin.obj"
	-@erase "$(INTDIR)\wM3_subdlg.obj"
	-@erase "$(INTDIR)\wM3_undo.obj"
	-@erase "$(OUTDIR)\Morpher.exp"
	-@erase "$(OUTDIR)\Morpher.lib"
	-@erase "$(OUTDIR)\Morpher.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Morpher.dlm"
	-@erase "..\..\..\..\maxsdk\plugin\Morpher.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Morpher.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\wM3.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Morpher.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=maxscrpt.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib paramblk2.lib comctl32.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x3fa30000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Morpher.pdb" /debug /machine:I386 /def:".\wM3.def" /out:"..\..\..\..\maxsdk\plugin\Morpher.dlm" /implib:"$(OUTDIR)\Morpher.lib" /pdbtype:sept /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\wM3.def"
LINK32_OBJS= \
	"$(INTDIR)\wM3_cache.obj" \
	"$(INTDIR)\wM3_channel.obj" \
	"$(INTDIR)\wM3_dlg-fns.obj" \
	"$(INTDIR)\wM3_dlg.obj" \
	"$(INTDIR)\wM3_impexp.obj" \
	"$(INTDIR)\wM3_io.obj" \
	"$(INTDIR)\wM3_main.obj" \
	"$(INTDIR)\wM3_pick.obj" \
	"$(INTDIR)\wM3_plugin.obj" \
	"$(INTDIR)\wM3_subdlg.obj" \
	"$(INTDIR)\wM3_undo.obj" \
	"$(INTDIR)\wM3_core.obj" \
	"$(INTDIR)\wM3_material.obj" \
	"$(INTDIR)\wM3_material_dlg.obj" \
	"$(INTDIR)\wM3_material_pick.obj" \
	"$(INTDIR)\wM3.res"

"..\..\..\..\maxsdk\plugin\Morpher.dlm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Morpher.dep")
!INCLUDE "Morpher.dep"
!ELSE 
!MESSAGE Warning: cannot find "Morpher.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Morpher - Win32 Release" || "$(CFG)" == "Morpher - Win32 Debug" || "$(CFG)" == "Morpher - Win32 Hybrid"
SOURCE=.\wM3.rc

"$(INTDIR)\wM3.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\wM3_cache.cpp

"$(INTDIR)\wM3_cache.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_channel.cpp

"$(INTDIR)\wM3_channel.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\wM3_dlg-fns.cpp"

"$(INTDIR)\wM3_dlg-fns.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_dlg.cpp

"$(INTDIR)\wM3_dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_impexp.cpp

"$(INTDIR)\wM3_impexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_io.cpp

"$(INTDIR)\wM3_io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_main.cpp

"$(INTDIR)\wM3_main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_pick.cpp

"$(INTDIR)\wM3_pick.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_plugin.cpp

"$(INTDIR)\wM3_plugin.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_subdlg.cpp

"$(INTDIR)\wM3_subdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_undo.cpp

"$(INTDIR)\wM3_undo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_core.cpp

"$(INTDIR)\wM3_core.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_material.cpp

"$(INTDIR)\wM3_material.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_material_dlg.cpp

"$(INTDIR)\wM3_material_dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wM3_material_pick.cpp

"$(INTDIR)\wM3_material_pick.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

