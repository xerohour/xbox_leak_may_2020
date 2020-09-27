# Microsoft Developer Studio Generated NMAKE File, Based on kernel.dsp
!IF "$(CFG)" == ""
CFG=kernel - Win32 Release
!MESSAGE No configuration specified. Defaulting to kernel - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "kernel - Win32 Release" && "$(CFG)" != "kernel - Win32 Debug" && "$(CFG)" != "kernel - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak" CFG="kernel - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kernel - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kernel - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kernel - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "kernel - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\kernel.dlk"


CLEAN :
	-@erase "$(INTDIR)\kernel.pch"
	-@erase "$(INTDIR)\kernel.res"
	-@erase "$(INTDIR)\kernelhdr.obj"
	-@erase "$(INTDIR)\kernelmain.obj"
	-@erase "$(INTDIR)\sampKernel.obj"
	-@erase "$(INTDIR)\stdKernels.obj"
	-@erase "$(OUTDIR)\kernel.exp"
	-@erase "$(OUTDIR)\kernel.lib"
	-@erase "..\..\..\maxsdk\plugin\kernel.dlk"
	-@erase ".\kernel.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

kernel=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yu"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\kernel.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kernel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076D0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\kernel.pdb" /machine:I386 /def:".\kernel.def" /out:"..\..\..\maxsdk\plugin\kernel.dlk" /implib:"$(OUTDIR)\kernel.lib" /release 
DEF_FILE= \
	".\kernel.def"
LINK32_OBJS= \
	"$(INTDIR)\kernelhdr.obj" \
	"$(INTDIR)\kernelmain.obj" \
	"$(INTDIR)\sampKernel.obj" \
	"$(INTDIR)\stdKernels.obj" \
	"$(INTDIR)\kernel.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\kernel.dlk" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "kernel - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\kernel.dlk"


CLEAN :
	-@erase "$(INTDIR)\kernel.pch"
	-@erase "$(INTDIR)\kernel.res"
	-@erase "$(INTDIR)\kernelhdr.obj"
	-@erase "$(INTDIR)\kernelmain.obj"
	-@erase "$(INTDIR)\sampKernel.obj"
	-@erase "$(INTDIR)\stdKernels.obj"
	-@erase "$(OUTDIR)\kernel.exp"
	-@erase "$(OUTDIR)\kernel.lib"
	-@erase "$(OUTDIR)\kernel.pdb"
	-@erase "..\..\..\maxsdk\plugin\kernel.dlk"
	-@erase "..\..\..\maxsdk\plugin\kernel.ilk"
	-@erase ".\kernel.idb"
	-@erase ".\kernel.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

kernel=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yu"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\kernel.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kernel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076D0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\kernel.pdb" /debug /machine:I386 /def:".\kernel.def" /out:"..\..\..\maxsdk\plugin\kernel.dlk" /implib:"$(OUTDIR)\kernel.lib" 
DEF_FILE= \
	".\kernel.def"
LINK32_OBJS= \
	"$(INTDIR)\kernelhdr.obj" \
	"$(INTDIR)\kernelmain.obj" \
	"$(INTDIR)\sampKernel.obj" \
	"$(INTDIR)\stdKernels.obj" \
	"$(INTDIR)\kernel.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\kernel.dlk" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "kernel - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\kernel.dlk"


CLEAN :
	-@erase "$(INTDIR)\kernel.pch"
	-@erase "$(INTDIR)\kernel.res"
	-@erase "$(INTDIR)\kernelhdr.obj"
	-@erase "$(INTDIR)\kernelmain.obj"
	-@erase "$(INTDIR)\sampKernel.obj"
	-@erase "$(INTDIR)\stdKernels.obj"
	-@erase "$(OUTDIR)\kernel.exp"
	-@erase "$(OUTDIR)\kernel.lib"
	-@erase "$(OUTDIR)\kernel.pdb"
	-@erase "..\..\..\maxsdk\plugin\kernel.dlk"
	-@erase "..\..\..\maxsdk\plugin\kernel.ilk"
	-@erase ".\kernel.idb"
	-@erase ".\kernel.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

kernel=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yu"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\kernel.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kernel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076D0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\kernel.pdb" /debug /machine:I386 /def:".\kernel.def" /out:"..\..\..\maxsdk\plugin\kernel.dlk" /implib:"$(OUTDIR)\kernel.lib" 
DEF_FILE= \
	".\kernel.def"
LINK32_OBJS= \
	"$(INTDIR)\kernelhdr.obj" \
	"$(INTDIR)\kernelmain.obj" \
	"$(INTDIR)\sampKernel.obj" \
	"$(INTDIR)\stdKernels.obj" \
	"$(INTDIR)\kernel.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\kernel.dlk" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("kernel.dep")
!INCLUDE "kernel.dep"
!ELSE 
!MESSAGE Warning: cannot find "kernel.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "kernel - Win32 Release" || "$(CFG)" == "kernel - Win32 Debug" || "$(CFG)" == "kernel - Win32 Hybrid"
SOURCE=.\kernel.rc

"$(INTDIR)\kernel.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\kernelhdr.cpp

!IF  "$(CFG)" == "kernel - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yc"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

"$(INTDIR)\kernelhdr.obj"	"$(INTDIR)\kernel.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kernel - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yc"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

"$(INTDIR)\kernelhdr.obj"	"$(INTDIR)\kernel.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kernel - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\kernel.pch" /Yc"kernelhdr.h" /Fo"$(INTDIR)\\" /Fd"kernel.pdb" /FD /c 

"$(INTDIR)\kernelhdr.obj"	"$(INTDIR)\kernel.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\kernelmain.cpp

"$(INTDIR)\kernelmain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kernel.pch"


SOURCE=.\sampKernel.cpp

"$(INTDIR)\sampKernel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kernel.pch"


SOURCE=.\stdKernels.cpp

"$(INTDIR)\stdKernels.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kernel.pch"



!ENDIF 

