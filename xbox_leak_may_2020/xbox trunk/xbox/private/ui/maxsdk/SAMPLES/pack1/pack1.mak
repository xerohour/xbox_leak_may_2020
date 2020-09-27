# Microsoft Developer Studio Generated NMAKE File, Based on Pack1.dsp
!IF "$(CFG)" == ""
CFG=pack1 - Win32 Release
!MESSAGE No configuration specified. Defaulting to pack1 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "pack1 - Win32 Release" && "$(CFG)" != "pack1 - Win32 Debug" && "$(CFG)" != "pack1 - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pack1.mak" CFG="pack1 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pack1 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pack1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pack1 - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pack1 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\update1.dlo"


CLEAN :
	-@erase "$(INTDIR)\approxui.obj"
	-@erase "$(INTDIR)\pack1.obj"
	-@erase "$(INTDIR)\Pack1.pch"
	-@erase "$(INTDIR)\pack1.res"
	-@erase "$(INTDIR)\triedui.obj"
	-@erase "$(INTDIR)\triobjed.obj"
	-@erase "$(INTDIR)\triops.obj"
	-@erase "$(OUTDIR)\update1.exp"
	-@erase "$(OUTDIR)\update1.lib"
	-@erase "..\..\..\maxsdk\plugin\update1.dlo"
	-@erase ".\pack1.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Pack1.pch" /Yu"pack1.h" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pack1.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Pack1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x232a0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\update1.pdb" /machine:I386 /def:".\pack1.def" /out:"..\..\..\maxsdk\plugin\update1.dlo" /implib:"$(OUTDIR)\update1.lib" /release 
DEF_FILE= \
	".\pack1.def"
LINK32_OBJS= \
	"$(INTDIR)\approxui.obj" \
	"$(INTDIR)\pack1.obj" \
	"$(INTDIR)\triedui.obj" \
	"$(INTDIR)\triobjed.obj" \
	"$(INTDIR)\triops.obj" \
	"$(INTDIR)\pack1.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\update1.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pack1 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\maxsdk\plugin\update1.dlo" "$(OUTDIR)\Pack1.pch"


CLEAN :
	-@erase "$(INTDIR)\approxui.obj"
	-@erase "$(INTDIR)\pack1.obj"
	-@erase "$(INTDIR)\Pack1.pch"
	-@erase "$(INTDIR)\pack1.res"
	-@erase "$(INTDIR)\triedui.obj"
	-@erase "$(INTDIR)\triobjed.obj"
	-@erase "$(INTDIR)\triops.obj"
	-@erase "$(OUTDIR)\update1.exp"
	-@erase "$(OUTDIR)\update1.lib"
	-@erase "$(OUTDIR)\update1.pdb"
	-@erase "..\..\..\maxsdk\plugin\update1.dlo"
	-@erase "..\..\..\maxsdk\plugin\update1.ilk"
	-@erase ".\pack1.idb"
	-@erase ".\pack1.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pack1.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Pack1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x232a0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\update1.pdb" /debug /machine:I386 /def:".\pack1.def" /out:"..\..\..\maxsdk\plugin\update1.dlo" /implib:"$(OUTDIR)\update1.lib" 
DEF_FILE= \
	".\pack1.def"
LINK32_OBJS= \
	"$(INTDIR)\approxui.obj" \
	"$(INTDIR)\pack1.obj" \
	"$(INTDIR)\triedui.obj" \
	"$(INTDIR)\triobjed.obj" \
	"$(INTDIR)\triops.obj" \
	"$(INTDIR)\pack1.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\update1.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pack1 - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\update1.dlo"


CLEAN :
	-@erase "$(INTDIR)\approxui.obj"
	-@erase "$(INTDIR)\pack1.obj"
	-@erase "$(INTDIR)\Pack1.pch"
	-@erase "$(INTDIR)\pack1.res"
	-@erase "$(INTDIR)\triedui.obj"
	-@erase "$(INTDIR)\triobjed.obj"
	-@erase "$(INTDIR)\triops.obj"
	-@erase "$(OUTDIR)\update1.exp"
	-@erase "$(OUTDIR)\update1.lib"
	-@erase "$(OUTDIR)\update1.pdb"
	-@erase "..\..\..\maxsdk\plugin\update1.dlo"
	-@erase "..\..\..\maxsdk\plugin\update1.ilk"
	-@erase ".\pack1.idb"
	-@erase ".\pack1.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Pack1.pch" /Yu"pack1.h" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pack1.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Pack1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x232a0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\update1.pdb" /debug /machine:I386 /def:".\pack1.def" /out:"..\..\..\maxsdk\plugin\update1.dlo" /implib:"$(OUTDIR)\update1.lib" 
DEF_FILE= \
	".\pack1.def"
LINK32_OBJS= \
	"$(INTDIR)\approxui.obj" \
	"$(INTDIR)\pack1.obj" \
	"$(INTDIR)\triedui.obj" \
	"$(INTDIR)\triobjed.obj" \
	"$(INTDIR)\triops.obj" \
	"$(INTDIR)\pack1.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\Maxscrpt.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\update1.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Pack1.dep")
!INCLUDE "Pack1.dep"
!ELSE 
!MESSAGE Warning: cannot find "Pack1.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pack1 - Win32 Release" || "$(CFG)" == "pack1 - Win32 Debug" || "$(CFG)" == "pack1 - Win32 Hybrid"
SOURCE=.\approxui.cpp

"$(INTDIR)\approxui.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Pack1.pch"


SOURCE=.\pack1.cpp

!IF  "$(CFG)" == "pack1 - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Pack1.pch" /Yc"pack1.h" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 

"$(INTDIR)\pack1.obj"	"$(INTDIR)\Pack1.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "pack1 - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Pack1.pch" /Yc"pack1.h" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 

"$(INTDIR)\pack1.obj"	"$(INTDIR)\Pack1.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "pack1 - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Pack1.pch" /Yc"pack1.h" /Fo"$(INTDIR)\\" /Fd"pack1.pdb" /FD /c 

"$(INTDIR)\pack1.obj"	"$(INTDIR)\Pack1.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\pack1.rc

"$(INTDIR)\pack1.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\triedui.cpp

"$(INTDIR)\triedui.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Pack1.pch"


SOURCE=.\triobjed.cpp

"$(INTDIR)\triobjed.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Pack1.pch"


SOURCE=.\triops.cpp

"$(INTDIR)\triops.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Pack1.pch"



!ENDIF 

