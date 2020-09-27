# Microsoft Developer Studio Generated NMAKE File, Based on samplers.dsp
!IF "$(CFG)" == ""
CFG=samplers - Win32 Release
!MESSAGE No configuration specified. Defaulting to samplers - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "samplers - Win32 Release" && "$(CFG)" != "samplers - Win32 Debug" && "$(CFG)" != "samplers - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "samplers.mak" CFG="samplers - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "samplers - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "samplers - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "samplers - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "samplers - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\samplers.dlh"


CLEAN :
	-@erase "$(INTDIR)\samplers.pch"
	-@erase "$(INTDIR)\samplers.res"
	-@erase "$(INTDIR)\samplersHdr.obj"
	-@erase "$(INTDIR)\samplersMain.obj"
	-@erase "$(INTDIR)\samplerUtil.obj"
	-@erase "$(INTDIR)\sampSampler.obj"
	-@erase "$(INTDIR)\stdSamplers.obj"
	-@erase "$(OUTDIR)\samplers.exp"
	-@erase "$(OUTDIR)\samplers.lib"
	-@erase "..\..\..\maxsdk\plugin\samplers.dlh"
	-@erase ".\samplers.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

samplers=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yu"samplershdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\samplers.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\samplers.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x084E0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\samplers.pdb" /machine:I386 /def:".\samplers.def" /out:"..\..\..\maxsdk\plugin\samplers.dlh" /implib:"$(OUTDIR)\samplers.lib" /release 
DEF_FILE= \
	".\samplers.def"
LINK32_OBJS= \
	"$(INTDIR)\samplersHdr.obj" \
	"$(INTDIR)\samplersMain.obj" \
	"$(INTDIR)\samplerUtil.obj" \
	"$(INTDIR)\sampSampler.obj" \
	"$(INTDIR)\stdSamplers.obj" \
	"$(INTDIR)\samplers.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib"

"..\..\..\maxsdk\plugin\samplers.dlh" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "samplers - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\samplers.dlh"


CLEAN :
	-@erase "$(INTDIR)\samplers.pch"
	-@erase "$(INTDIR)\samplers.res"
	-@erase "$(INTDIR)\samplersHdr.obj"
	-@erase "$(INTDIR)\samplersMain.obj"
	-@erase "$(INTDIR)\samplerUtil.obj"
	-@erase "$(INTDIR)\sampSampler.obj"
	-@erase "$(INTDIR)\stdSamplers.obj"
	-@erase "$(OUTDIR)\samplers.exp"
	-@erase "$(OUTDIR)\samplers.lib"
	-@erase "$(OUTDIR)\samplers.pdb"
	-@erase "..\..\..\maxsdk\plugin\samplers.dlh"
	-@erase "..\..\..\maxsdk\plugin\samplers.ilk"
	-@erase ".\samplers.idb"
	-@erase ".\samplers.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

samplers=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yu"samplershdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\samplers.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\samplers.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x084E0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\samplers.pdb" /debug /machine:I386 /def:".\samplers.def" /out:"..\..\..\maxsdk\plugin\samplers.dlh" /implib:"$(OUTDIR)\samplers.lib" 
DEF_FILE= \
	".\samplers.def"
LINK32_OBJS= \
	"$(INTDIR)\samplersHdr.obj" \
	"$(INTDIR)\samplersMain.obj" \
	"$(INTDIR)\samplerUtil.obj" \
	"$(INTDIR)\sampSampler.obj" \
	"$(INTDIR)\stdSamplers.obj" \
	"$(INTDIR)\samplers.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib"

"..\..\..\maxsdk\plugin\samplers.dlh" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "samplers - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\samplers.dlh"


CLEAN :
	-@erase "$(INTDIR)\samplers.pch"
	-@erase "$(INTDIR)\samplers.res"
	-@erase "$(INTDIR)\samplersHdr.obj"
	-@erase "$(INTDIR)\samplersMain.obj"
	-@erase "$(INTDIR)\samplerUtil.obj"
	-@erase "$(INTDIR)\sampSampler.obj"
	-@erase "$(INTDIR)\stdSamplers.obj"
	-@erase "$(OUTDIR)\samplers.exp"
	-@erase "$(OUTDIR)\samplers.lib"
	-@erase "$(OUTDIR)\samplers.pdb"
	-@erase "..\..\..\maxsdk\plugin\samplers.dlh"
	-@erase "..\..\..\maxsdk\plugin\samplers.ilk"
	-@erase ".\samplers.idb"
	-@erase ".\samplers.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

samplers=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yu"samplershdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\samplers.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\samplers.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x084E0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\samplers.pdb" /debug /machine:I386 /def:".\samplers.def" /out:"..\..\..\maxsdk\plugin\samplers.dlh" /implib:"$(OUTDIR)\samplers.lib" 
DEF_FILE= \
	".\samplers.def"
LINK32_OBJS= \
	"$(INTDIR)\samplersHdr.obj" \
	"$(INTDIR)\samplersMain.obj" \
	"$(INTDIR)\samplerUtil.obj" \
	"$(INTDIR)\sampSampler.obj" \
	"$(INTDIR)\stdSamplers.obj" \
	"$(INTDIR)\samplers.res" \
	"..\..\lib\core.lib" \
	"..\..\lib\maxutil.lib" \
	"..\..\lib\geom.lib"

"..\..\..\maxsdk\plugin\samplers.dlh" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("samplers.dep")
!INCLUDE "samplers.dep"
!ELSE 
!MESSAGE Warning: cannot find "samplers.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "samplers - Win32 Release" || "$(CFG)" == "samplers - Win32 Debug" || "$(CFG)" == "samplers - Win32 Hybrid"
SOURCE=.\samplers.rc

"$(INTDIR)\samplers.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\samplersHdr.cpp

!IF  "$(CFG)" == "samplers - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yc"samplersHdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

"$(INTDIR)\samplersHdr.obj"	"$(INTDIR)\samplers.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "samplers - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yc"samplersHdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

"$(INTDIR)\samplersHdr.obj"	"$(INTDIR)\samplers.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "samplers - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\samplers.pch" /Yc"samplersHdr.h" /Fo"$(INTDIR)\\" /Fd"samplers.pdb" /FD /c 

"$(INTDIR)\samplersHdr.obj"	"$(INTDIR)\samplers.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\samplersMain.cpp

"$(INTDIR)\samplersMain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\samplers.pch"


SOURCE=.\samplerUtil.cpp

"$(INTDIR)\samplerUtil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\samplers.pch"


SOURCE=.\sampSampler.cpp

"$(INTDIR)\sampSampler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\samplers.pch"


SOURCE=.\stdSamplers.cpp

"$(INTDIR)\stdSamplers.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\samplers.pch"



!ENDIF 

