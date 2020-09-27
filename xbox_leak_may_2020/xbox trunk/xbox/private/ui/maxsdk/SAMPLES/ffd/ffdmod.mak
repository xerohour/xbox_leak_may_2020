# Microsoft Developer Studio Generated NMAKE File, Based on ffdmod.dsp
!IF "$(CFG)" == ""
CFG=FFD - Win32 Release
!MESSAGE No configuration specified. Defaulting to FFD - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "FFD - Win32 Release" && "$(CFG)" != "FFD - Win32 Debug" && "$(CFG)" != "FFD - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ffdmod.mak" CFG="FFD - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FFD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FFD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FFD - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "FFD - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\ffdmod.dlo"


CLEAN :
	-@erase "$(INTDIR)\ffd.obj"
	-@erase "$(INTDIR)\ffdmod.obj"
	-@erase "$(INTDIR)\ffdmod.pch"
	-@erase "$(INTDIR)\ffdmod.res"
	-@erase "$(INTDIR)\ffdnm.obj"
	-@erase "$(INTDIR)\ffdui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ffdmod.exp"
	-@erase "$(OUTDIR)\ffdmod.lib"
	-@erase "..\..\..\maxsdk\plugin\ffdmod.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDMod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ffdmod.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ffdmod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ffdmod.pdb" /machine:I386 /def:".\ffdmod.def" /out:"..\..\..\maxsdk\plugin\ffdmod.dlo" /implib:"$(OUTDIR)\ffdmod.lib" /release 
DEF_FILE= \
	".\ffdmod.def"
LINK32_OBJS= \
	"$(INTDIR)\ffd.obj" \
	"$(INTDIR)\ffdmod.obj" \
	"$(INTDIR)\ffdnm.obj" \
	"$(INTDIR)\ffdui.obj" \
	"$(INTDIR)\ffdmod.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\ffdmod.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FFD - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\ffdmod.dlo"


CLEAN :
	-@erase "$(INTDIR)\ffd.obj"
	-@erase "$(INTDIR)\ffdmod.obj"
	-@erase "$(INTDIR)\ffdmod.pch"
	-@erase "$(INTDIR)\ffdmod.res"
	-@erase "$(INTDIR)\ffdnm.obj"
	-@erase "$(INTDIR)\ffdui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ffdmod.exp"
	-@erase "$(OUTDIR)\ffdmod.lib"
	-@erase "$(OUTDIR)\ffdmod.pdb"
	-@erase "..\..\..\maxsdk\plugin\ffdmod.dlo"
	-@erase "..\..\..\maxsdk\plugin\ffdmod.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDMod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ffdmod.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ffdmod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ffdmod.pdb" /debug /machine:I386 /def:".\ffdmod.def" /out:"..\..\..\maxsdk\plugin\ffdmod.dlo" /implib:"$(OUTDIR)\ffdmod.lib" 
DEF_FILE= \
	".\ffdmod.def"
LINK32_OBJS= \
	"$(INTDIR)\ffd.obj" \
	"$(INTDIR)\ffdmod.obj" \
	"$(INTDIR)\ffdnm.obj" \
	"$(INTDIR)\ffdui.obj" \
	"$(INTDIR)\ffdmod.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\ffdmod.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FFD - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\ffdmod.dlo"


CLEAN :
	-@erase "$(INTDIR)\ffd.obj"
	-@erase "$(INTDIR)\ffdmod.obj"
	-@erase "$(INTDIR)\ffdmod.pch"
	-@erase "$(INTDIR)\ffdmod.res"
	-@erase "$(INTDIR)\ffdnm.obj"
	-@erase "$(INTDIR)\ffdui.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ffdmod.exp"
	-@erase "$(OUTDIR)\ffdmod.lib"
	-@erase "$(OUTDIR)\ffdmod.pdb"
	-@erase "..\..\..\maxsdk\plugin\ffdmod.dlo"
	-@erase "..\..\..\maxsdk\plugin\ffdmod.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDMod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ffdmod.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ffdmod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ffdmod.pdb" /debug /machine:I386 /def:".\ffdmod.def" /out:"..\..\..\maxsdk\plugin\ffdmod.dlo" /implib:"$(OUTDIR)\ffdmod.lib" 
DEF_FILE= \
	".\ffdmod.def"
LINK32_OBJS= \
	"$(INTDIR)\ffd.obj" \
	"$(INTDIR)\ffdmod.obj" \
	"$(INTDIR)\ffdnm.obj" \
	"$(INTDIR)\ffdui.obj" \
	"$(INTDIR)\ffdmod.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib" \
	"..\..\lib\Maxscrpt.lib"

"..\..\..\maxsdk\plugin\ffdmod.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ffdmod.dep")
!INCLUDE "ffdmod.dep"
!ELSE 
!MESSAGE Warning: cannot find "ffdmod.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FFD - Win32 Release" || "$(CFG)" == "FFD - Win32 Debug" || "$(CFG)" == "FFD - Win32 Hybrid"
SOURCE=.\ffd.cpp

!IF  "$(CFG)" == "FFD - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"FFDmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ffdmod.cpp

!IF  "$(CFG)" == "FFD - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yc"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdmod.obj"	"$(INTDIR)\ffdmod.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yc"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdmod.obj"	"$(INTDIR)\ffdmod.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yc"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdmod.obj"	"$(INTDIR)\ffdmod.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ffdmod.rc

"$(INTDIR)\ffdmod.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ffdnm.cpp

!IF  "$(CFG)" == "FFD - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdnm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdnm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FFD - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\dll\maxscrpt" /I "..\..\..\dll\maxscrpt\mxsagni" /I "..\..\include" /I "..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ffdmod.pch" /Yu"ffdmod.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ffdnm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ffdui.cpp

"$(INTDIR)\ffdui.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ffdmod.pch"



!ENDIF 

