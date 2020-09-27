# Microsoft Developer Studio Generated NMAKE File, Based on Util.dsp
!IF "$(CFG)" == ""
CFG=util - Win32 Release
!MESSAGE No configuration specified. Defaulting to util - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "util - Win32 Release" && "$(CFG)" != "util - Win32 Debug" && "$(CFG)" != "util - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Util.mak" CFG="util - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "util - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "util - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\utility.dlu"


CLEAN :
	-@erase "$(INTDIR)\appdata.obj"
	-@erase "$(INTDIR)\asciiout.obj"
	-@erase "$(INTDIR)\celltex.obj"
	-@erase "$(INTDIR)\colclip.obj"
	-@erase "$(INTDIR)\collapse.obj"
	-@erase "$(INTDIR)\linkinfo.obj"
	-@erase "$(INTDIR)\ortkeys.obj"
	-@erase "$(INTDIR)\randkeys.obj"
	-@erase "$(INTDIR)\refobj.obj"
	-@erase "$(INTDIR)\rescale.obj"
	-@erase "$(INTDIR)\selkeys.obj"
	-@erase "$(INTDIR)\shapechk.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\Util.pch"
	-@erase "$(INTDIR)\util.res"
	-@erase "$(INTDIR)\utiltest.obj"
	-@erase "$(OUTDIR)\utility.exp"
	-@erase "$(OUTDIR)\utility.lib"
	-@erase "..\..\..\maxsdk\plugin\utility.dlu"
	-@erase ".\util.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yu"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\util.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Util.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05010000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\utility.pdb" /machine:I386 /def:".\util.def" /out:"..\..\..\maxsdk\plugin\utility.dlu" /implib:"$(OUTDIR)\utility.lib" /release 
DEF_FILE= \
	".\util.def"
LINK32_OBJS= \
	"$(INTDIR)\appdata.obj" \
	"$(INTDIR)\asciiout.obj" \
	"$(INTDIR)\celltex.obj" \
	"$(INTDIR)\colclip.obj" \
	"$(INTDIR)\collapse.obj" \
	"$(INTDIR)\linkinfo.obj" \
	"$(INTDIR)\ortkeys.obj" \
	"$(INTDIR)\randkeys.obj" \
	"$(INTDIR)\refobj.obj" \
	"$(INTDIR)\rescale.obj" \
	"$(INTDIR)\selkeys.obj" \
	"$(INTDIR)\shapechk.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\utiltest.obj" \
	"$(INTDIR)\util.res" \
	"..\..\lib\bmm.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\utility.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\utility.dlu"


CLEAN :
	-@erase "$(INTDIR)\appdata.obj"
	-@erase "$(INTDIR)\asciiout.obj"
	-@erase "$(INTDIR)\celltex.obj"
	-@erase "$(INTDIR)\colclip.obj"
	-@erase "$(INTDIR)\collapse.obj"
	-@erase "$(INTDIR)\linkinfo.obj"
	-@erase "$(INTDIR)\ortkeys.obj"
	-@erase "$(INTDIR)\randkeys.obj"
	-@erase "$(INTDIR)\refobj.obj"
	-@erase "$(INTDIR)\rescale.obj"
	-@erase "$(INTDIR)\selkeys.obj"
	-@erase "$(INTDIR)\shapechk.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\Util.pch"
	-@erase "$(INTDIR)\util.res"
	-@erase "$(INTDIR)\utiltest.obj"
	-@erase "$(OUTDIR)\utility.exp"
	-@erase "$(OUTDIR)\utility.lib"
	-@erase "$(OUTDIR)\utility.pdb"
	-@erase "..\..\..\maxsdk\plugin\utility.dlu"
	-@erase "..\..\..\maxsdk\plugin\utility.ilk"
	-@erase ".\util.idb"
	-@erase ".\util.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yu"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\util.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Util.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05010000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\utility.pdb" /debug /machine:I386 /def:".\util.def" /out:"..\..\..\maxsdk\plugin\utility.dlu" /implib:"$(OUTDIR)\utility.lib" 
DEF_FILE= \
	".\util.def"
LINK32_OBJS= \
	"$(INTDIR)\appdata.obj" \
	"$(INTDIR)\asciiout.obj" \
	"$(INTDIR)\celltex.obj" \
	"$(INTDIR)\colclip.obj" \
	"$(INTDIR)\collapse.obj" \
	"$(INTDIR)\linkinfo.obj" \
	"$(INTDIR)\ortkeys.obj" \
	"$(INTDIR)\randkeys.obj" \
	"$(INTDIR)\refobj.obj" \
	"$(INTDIR)\rescale.obj" \
	"$(INTDIR)\selkeys.obj" \
	"$(INTDIR)\shapechk.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\utiltest.obj" \
	"$(INTDIR)\util.res" \
	"..\..\lib\bmm.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\utility.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "util - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\utility.dlu"


CLEAN :
	-@erase "$(INTDIR)\appdata.obj"
	-@erase "$(INTDIR)\asciiout.obj"
	-@erase "$(INTDIR)\celltex.obj"
	-@erase "$(INTDIR)\colclip.obj"
	-@erase "$(INTDIR)\collapse.obj"
	-@erase "$(INTDIR)\linkinfo.obj"
	-@erase "$(INTDIR)\ortkeys.obj"
	-@erase "$(INTDIR)\randkeys.obj"
	-@erase "$(INTDIR)\refobj.obj"
	-@erase "$(INTDIR)\rescale.obj"
	-@erase "$(INTDIR)\selkeys.obj"
	-@erase "$(INTDIR)\shapechk.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\Util.pch"
	-@erase "$(INTDIR)\util.res"
	-@erase "$(INTDIR)\utiltest.obj"
	-@erase "$(OUTDIR)\utility.exp"
	-@erase "$(OUTDIR)\utility.lib"
	-@erase "$(OUTDIR)\utility.pdb"
	-@erase "..\..\..\maxsdk\plugin\utility.dlu"
	-@erase "..\..\..\maxsdk\plugin\utility.ilk"
	-@erase ".\util.idb"
	-@erase ".\util.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yu"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\util.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Util.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05010000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\utility.pdb" /debug /machine:I386 /def:".\util.def" /out:"..\..\..\maxsdk\plugin\utility.dlu" /implib:"$(OUTDIR)\utility.lib" 
DEF_FILE= \
	".\util.def"
LINK32_OBJS= \
	"$(INTDIR)\appdata.obj" \
	"$(INTDIR)\asciiout.obj" \
	"$(INTDIR)\celltex.obj" \
	"$(INTDIR)\colclip.obj" \
	"$(INTDIR)\collapse.obj" \
	"$(INTDIR)\linkinfo.obj" \
	"$(INTDIR)\ortkeys.obj" \
	"$(INTDIR)\randkeys.obj" \
	"$(INTDIR)\refobj.obj" \
	"$(INTDIR)\rescale.obj" \
	"$(INTDIR)\selkeys.obj" \
	"$(INTDIR)\shapechk.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\utiltest.obj" \
	"$(INTDIR)\util.res" \
	"..\..\lib\bmm.lib" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\lib\MNMath.lib" \
	"..\..\Lib\Particle.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\Paramblk2.lib"

"..\..\..\maxsdk\plugin\utility.dlu" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Util.dep")
!INCLUDE "Util.dep"
!ELSE 
!MESSAGE Warning: cannot find "Util.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "util - Win32 Release" || "$(CFG)" == "util - Win32 Debug" || "$(CFG)" == "util - Win32 Hybrid"
SOURCE=.\appdata.cpp

"$(INTDIR)\appdata.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\asciiout.cpp

"$(INTDIR)\asciiout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\celltex.cpp

"$(INTDIR)\celltex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\colclip.cpp

"$(INTDIR)\colclip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\collapse.cpp

"$(INTDIR)\collapse.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\linkinfo.cpp

"$(INTDIR)\linkinfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\ortkeys.cpp

"$(INTDIR)\ortkeys.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\randkeys.cpp

"$(INTDIR)\randkeys.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\refobj.cpp

"$(INTDIR)\refobj.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\rescale.cpp

"$(INTDIR)\rescale.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\selkeys.cpp

"$(INTDIR)\selkeys.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\shapechk.cpp

"$(INTDIR)\shapechk.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"


SOURCE=.\util.cpp

!IF  "$(CFG)" == "util - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yc"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 

"$(INTDIR)\util.obj"	"$(INTDIR)\Util.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "util - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yc"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 

"$(INTDIR)\util.obj"	"$(INTDIR)\Util.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "util - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Util.pch" /Yc"util.h" /Fo"$(INTDIR)\\" /Fd"util.pdb" /FD /c 

"$(INTDIR)\util.obj"	"$(INTDIR)\Util.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\util.rc

"$(INTDIR)\util.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\utiltest.cpp

"$(INTDIR)\utiltest.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Util.pch"



!ENDIF 

