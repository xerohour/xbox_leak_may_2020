# Microsoft Developer Studio Generated NMAKE File, Based on blur.dsp
!IF "$(CFG)" == ""
CFG=blur - Win32 Release
!MESSAGE No configuration specified. Defaulting to blur - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "blur - Win32 Release" && "$(CFG)" != "blur - Win32 Debug" && "$(CFG)" != "blur - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "blur.mak" CFG="blur - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "blur - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "blur - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "blur - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "blur - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\blur.dlv"


CLEAN :
	-@erase "$(INTDIR)\blur.pch"
	-@erase "$(INTDIR)\blur.res"
	-@erase "$(INTDIR)\blurBase.obj"
	-@erase "$(INTDIR)\blurDirectional.obj"
	-@erase "$(INTDIR)\blurMgr.obj"
	-@erase "$(INTDIR)\blurRadial.obj"
	-@erase "$(INTDIR)\blurUniform.obj"
	-@erase "$(INTDIR)\dlgProcs.obj"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\scTex.obj"
	-@erase "$(INTDIR)\selIgnBack.obj"
	-@erase "$(INTDIR)\selImage.obj"
	-@erase "$(INTDIR)\selLum.obj"
	-@erase "$(INTDIR)\selMaps.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\blur.exp"
	-@erase "$(OUTDIR)\blur.lib"
	-@erase "..\..\..\..\maxsdk\plugin\blur.dlv"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

blur=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yu"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\blur.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\blur.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\blur.pdb" /machine:I386 /def:".\blur.def" /out:"..\..\..\..\maxsdk\plugin\blur.dlv" /implib:"$(OUTDIR)\blur.lib" /release 
DEF_FILE= \
	".\blur.def"
LINK32_OBJS= \
	"$(INTDIR)\blurBase.obj" \
	"$(INTDIR)\blurDirectional.obj" \
	"$(INTDIR)\blurRadial.obj" \
	"$(INTDIR)\blurUniform.obj" \
	"$(INTDIR)\scTex.obj" \
	"$(INTDIR)\selIgnBack.obj" \
	"$(INTDIR)\selImage.obj" \
	"$(INTDIR)\selLum.obj" \
	"$(INTDIR)\selMaps.obj" \
	"$(INTDIR)\blurMgr.obj" \
	"$(INTDIR)\dlgProcs.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\blur.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib"

"..\..\..\..\maxsdk\plugin\blur.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "blur - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\blur.dlv"


CLEAN :
	-@erase "$(INTDIR)\blur.pch"
	-@erase "$(INTDIR)\blur.res"
	-@erase "$(INTDIR)\blurBase.obj"
	-@erase "$(INTDIR)\blurDirectional.obj"
	-@erase "$(INTDIR)\blurMgr.obj"
	-@erase "$(INTDIR)\blurRadial.obj"
	-@erase "$(INTDIR)\blurUniform.obj"
	-@erase "$(INTDIR)\dlgProcs.obj"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\scTex.obj"
	-@erase "$(INTDIR)\selIgnBack.obj"
	-@erase "$(INTDIR)\selImage.obj"
	-@erase "$(INTDIR)\selLum.obj"
	-@erase "$(INTDIR)\selMaps.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\blur.exp"
	-@erase "$(OUTDIR)\blur.lib"
	-@erase "$(OUTDIR)\blur.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\blur.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\blur.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

blur=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yu"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\blur.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\blur.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\blur.pdb" /debug /machine:I386 /def:".\blur.def" /out:"..\..\..\..\maxsdk\plugin\blur.dlv" /implib:"$(OUTDIR)\blur.lib" 
DEF_FILE= \
	".\blur.def"
LINK32_OBJS= \
	"$(INTDIR)\blurBase.obj" \
	"$(INTDIR)\blurDirectional.obj" \
	"$(INTDIR)\blurRadial.obj" \
	"$(INTDIR)\blurUniform.obj" \
	"$(INTDIR)\scTex.obj" \
	"$(INTDIR)\selIgnBack.obj" \
	"$(INTDIR)\selImage.obj" \
	"$(INTDIR)\selLum.obj" \
	"$(INTDIR)\selMaps.obj" \
	"$(INTDIR)\blurMgr.obj" \
	"$(INTDIR)\dlgProcs.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\blur.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib"

"..\..\..\..\maxsdk\plugin\blur.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "blur - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\blur.dlv"


CLEAN :
	-@erase "$(INTDIR)\blur.pch"
	-@erase "$(INTDIR)\blur.res"
	-@erase "$(INTDIR)\blurBase.obj"
	-@erase "$(INTDIR)\blurDirectional.obj"
	-@erase "$(INTDIR)\blurMgr.obj"
	-@erase "$(INTDIR)\blurRadial.obj"
	-@erase "$(INTDIR)\blurUniform.obj"
	-@erase "$(INTDIR)\dlgProcs.obj"
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\scTex.obj"
	-@erase "$(INTDIR)\selIgnBack.obj"
	-@erase "$(INTDIR)\selImage.obj"
	-@erase "$(INTDIR)\selLum.obj"
	-@erase "$(INTDIR)\selMaps.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\blur.exp"
	-@erase "$(OUTDIR)\blur.lib"
	-@erase "$(OUTDIR)\blur.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\blur.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\blur.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

blur=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yu"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\blur.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\blur.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\blur.pdb" /debug /machine:I386 /def:".\blur.def" /out:"..\..\..\..\maxsdk\plugin\blur.dlv" /implib:"$(OUTDIR)\blur.lib" 
DEF_FILE= \
	".\blur.def"
LINK32_OBJS= \
	"$(INTDIR)\blurBase.obj" \
	"$(INTDIR)\blurDirectional.obj" \
	"$(INTDIR)\blurRadial.obj" \
	"$(INTDIR)\blurUniform.obj" \
	"$(INTDIR)\scTex.obj" \
	"$(INTDIR)\selIgnBack.obj" \
	"$(INTDIR)\selImage.obj" \
	"$(INTDIR)\selLum.obj" \
	"$(INTDIR)\selMaps.obj" \
	"$(INTDIR)\blurMgr.obj" \
	"$(INTDIR)\dlgProcs.obj" \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\blur.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\bmm.lib"

"..\..\..\..\maxsdk\plugin\blur.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("blur.dep")
!INCLUDE "blur.dep"
!ELSE 
!MESSAGE Warning: cannot find "blur.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "blur - Win32 Release" || "$(CFG)" == "blur - Win32 Debug" || "$(CFG)" == "blur - Win32 Hybrid"
SOURCE=.\_blurTypes\blurBase.cpp

"$(INTDIR)\blurBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_blurTypes\blurDirectional.cpp

"$(INTDIR)\blurDirectional.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_blurTypes\blurRadial.cpp

"$(INTDIR)\blurRadial.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_blurTypes\blurUniform.cpp

"$(INTDIR)\blurUniform.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_selectTypes\scTex.cpp

"$(INTDIR)\scTex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_selectTypes\selIgnBack.cpp

"$(INTDIR)\selIgnBack.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_selectTypes\selImage.cpp

"$(INTDIR)\selImage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_selectTypes\selLum.cpp

"$(INTDIR)\selLum.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_selectTypes\selMaps.cpp

"$(INTDIR)\selMaps.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\blur.rc

"$(INTDIR)\blur.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\blurMgr.cpp

"$(INTDIR)\blurMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"


SOURCE=.\dlgProcs.cpp

"$(INTDIR)\dlgProcs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\blur.pch"


SOURCE=.\dllMain.cpp

!IF  "$(CFG)" == "blur - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yc"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dllMain.obj"	"$(INTDIR)\blur.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "blur - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yc"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dllMain.obj"	"$(INTDIR)\blur.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "blur - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\blur.pch" /Yc"pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dllMain.obj"	"$(INTDIR)\blur.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

