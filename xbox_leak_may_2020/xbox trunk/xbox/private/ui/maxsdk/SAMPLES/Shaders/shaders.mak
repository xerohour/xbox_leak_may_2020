# Microsoft Developer Studio Generated NMAKE File, Based on shaders.dsp
!IF "$(CFG)" == ""
CFG=shaders - Win32 Release
!MESSAGE No configuration specified. Defaulting to shaders - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "shaders - Win32 Release" && "$(CFG)" != "shaders - Win32 Debug" && "$(CFG)" != "shaders - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shaders.mak" CFG="shaders - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shaders - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shaders - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shaders - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "shaders - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\shaders.dlb"


CLEAN :
	-@erase "$(INTDIR)\shaderAniso.obj"
	-@erase "$(INTDIR)\shaderMultilayer.obj"
	-@erase "$(INTDIR)\shaderONB.obj"
	-@erase "$(INTDIR)\shaders.pch"
	-@erase "$(INTDIR)\shaders.res"
	-@erase "$(INTDIR)\shadersMain.obj"
	-@erase "$(INTDIR)\shadersPch.obj"
	-@erase "$(INTDIR)\shaderStrauss.obj"
	-@erase "$(INTDIR)\shaderUtil.obj"
	-@erase "$(INTDIR)\shaderWard.obj"
	-@erase "$(OUTDIR)\shaders.exp"
	-@erase "$(OUTDIR)\shaders.lib"
	-@erase "..\..\..\maxsdk\plugin\shaders.dlb"
	-@erase ".\shaders.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

shaders=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yu"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\shaders.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\shaders.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07710000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\shaders.pdb" /machine:I386 /def:".\shaders.def" /out:"..\..\..\maxsdk\plugin\shaders.dlb" /implib:"$(OUTDIR)\shaders.lib" /release 
DEF_FILE= \
	".\shaders.def"
LINK32_OBJS= \
	"$(INTDIR)\shaderAniso.obj" \
	"$(INTDIR)\shaderMultilayer.obj" \
	"$(INTDIR)\shaderONB.obj" \
	"$(INTDIR)\shadersMain.obj" \
	"$(INTDIR)\shadersPch.obj" \
	"$(INTDIR)\shaderStrauss.obj" \
	"$(INTDIR)\shaderUtil.obj" \
	"$(INTDIR)\shaderWard.obj" \
	"$(INTDIR)\shaders.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\shaders.dlb" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "shaders - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\shaders.dlb"


CLEAN :
	-@erase "$(INTDIR)\shaderAniso.obj"
	-@erase "$(INTDIR)\shaderMultilayer.obj"
	-@erase "$(INTDIR)\shaderONB.obj"
	-@erase "$(INTDIR)\shaders.pch"
	-@erase "$(INTDIR)\shaders.res"
	-@erase "$(INTDIR)\shadersMain.obj"
	-@erase "$(INTDIR)\shadersPch.obj"
	-@erase "$(INTDIR)\shaderStrauss.obj"
	-@erase "$(INTDIR)\shaderUtil.obj"
	-@erase "$(INTDIR)\shaderWard.obj"
	-@erase "$(OUTDIR)\shaders.exp"
	-@erase "$(OUTDIR)\shaders.lib"
	-@erase "$(OUTDIR)\shaders.pdb"
	-@erase "..\..\..\maxsdk\plugin\shaders.dlb"
	-@erase "..\..\..\maxsdk\plugin\shaders.ilk"
	-@erase ".\shaders.idb"
	-@erase ".\shaders.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

shaders=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yu"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\shaders.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\shaders.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07710000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\shaders.pdb" /debug /machine:I386 /def:".\shaders.def" /out:"..\..\..\maxsdk\plugin\shaders.dlb" /implib:"$(OUTDIR)\shaders.lib" 
DEF_FILE= \
	".\shaders.def"
LINK32_OBJS= \
	"$(INTDIR)\shaderAniso.obj" \
	"$(INTDIR)\shaderMultilayer.obj" \
	"$(INTDIR)\shaderONB.obj" \
	"$(INTDIR)\shadersMain.obj" \
	"$(INTDIR)\shadersPch.obj" \
	"$(INTDIR)\shaderStrauss.obj" \
	"$(INTDIR)\shaderUtil.obj" \
	"$(INTDIR)\shaderWard.obj" \
	"$(INTDIR)\shaders.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\shaders.dlb" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "shaders - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\shaders.dlb"


CLEAN :
	-@erase "$(INTDIR)\shaderAniso.obj"
	-@erase "$(INTDIR)\shaderMultilayer.obj"
	-@erase "$(INTDIR)\shaderONB.obj"
	-@erase "$(INTDIR)\shaders.pch"
	-@erase "$(INTDIR)\shaders.res"
	-@erase "$(INTDIR)\shadersMain.obj"
	-@erase "$(INTDIR)\shadersPch.obj"
	-@erase "$(INTDIR)\shaderStrauss.obj"
	-@erase "$(INTDIR)\shaderUtil.obj"
	-@erase "$(INTDIR)\shaderWard.obj"
	-@erase "$(OUTDIR)\shaders.exp"
	-@erase "$(OUTDIR)\shaders.lib"
	-@erase "$(OUTDIR)\shaders.pdb"
	-@erase "..\..\..\maxsdk\plugin\shaders.dlb"
	-@erase "..\..\..\maxsdk\plugin\shaders.ilk"
	-@erase ".\shaders.idb"
	-@erase ".\shaders.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

shaders=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yu"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\shaders.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\shaders.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07710000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\shaders.pdb" /debug /machine:I386 /def:".\shaders.def" /out:"..\..\..\maxsdk\plugin\shaders.dlb" /implib:"$(OUTDIR)\shaders.lib" 
DEF_FILE= \
	".\shaders.def"
LINK32_OBJS= \
	"$(INTDIR)\shaderAniso.obj" \
	"$(INTDIR)\shaderMultilayer.obj" \
	"$(INTDIR)\shaderONB.obj" \
	"$(INTDIR)\shadersMain.obj" \
	"$(INTDIR)\shadersPch.obj" \
	"$(INTDIR)\shaderStrauss.obj" \
	"$(INTDIR)\shaderUtil.obj" \
	"$(INTDIR)\shaderWard.obj" \
	"$(INTDIR)\shaders.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\shaders.dlb" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("shaders.dep")
!INCLUDE "shaders.dep"
!ELSE 
!MESSAGE Warning: cannot find "shaders.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "shaders - Win32 Release" || "$(CFG)" == "shaders - Win32 Debug" || "$(CFG)" == "shaders - Win32 Hybrid"
SOURCE=.\shaderAniso.cpp

"$(INTDIR)\shaderAniso.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shaderMultilayer.cpp

"$(INTDIR)\shaderMultilayer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shaderONB.cpp

"$(INTDIR)\shaderONB.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shaders.rc

"$(INTDIR)\shaders.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\shadersMain.cpp

"$(INTDIR)\shadersMain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shadersPch.cpp

!IF  "$(CFG)" == "shaders - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yc"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

"$(INTDIR)\shadersPch.obj"	"$(INTDIR)\shaders.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "shaders - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yc"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

"$(INTDIR)\shadersPch.obj"	"$(INTDIR)\shaders.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "shaders - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\shaders.pch" /Yc"shadersPch.h" /Fo"$(INTDIR)\\" /Fd"shaders.pdb" /FD /c 

"$(INTDIR)\shadersPch.obj"	"$(INTDIR)\shaders.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\shaderStrauss.cpp

"$(INTDIR)\shaderStrauss.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shaderUtil.cpp

"$(INTDIR)\shaderUtil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"


SOURCE=.\shaderWard.cpp

"$(INTDIR)\shaderWard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\shaders.pch"



!ENDIF 

