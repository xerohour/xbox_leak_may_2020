# Microsoft Developer Studio Generated NMAKE File, Based on sampleEff.dsp
!IF "$(CFG)" == ""
CFG=sampleEff - Win32 Release
!MESSAGE No configuration specified. Defaulting to sampleEff - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "sampleEff - Win32 Release" && "$(CFG)" != "sampleEff - Win32 Debug" && "$(CFG)" != "sampleEff - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sampleEff.mak" CFG="sampleEff - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sampleEff - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sampleEff - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sampleEff - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "sampleEff - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\sampleEff.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\sampleEff.obj"
	-@erase "$(INTDIR)\sampleEff.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sampleEff.exp"
	-@erase "$(OUTDIR)\sampleEff.lib"
	-@erase "..\..\..\..\maxsdk\plugin\sampleEff.dlv"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

sampleEff=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sampleEff.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sampleEff.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076B0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\sampleEff.pdb" /machine:I386 /def:".\sampleEff.def" /out:"..\..\..\..\maxsdk\plugin\sampleEff.dlv" /implib:"$(OUTDIR)\sampleEff.lib" /release 
DEF_FILE= \
	".\sampleEff.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\sampleEff.obj" \
	"$(INTDIR)\sampleEff.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sampleEff.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sampleEff - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\sampleEff.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\sampleEff.obj"
	-@erase "$(INTDIR)\sampleEff.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sampleEff.exp"
	-@erase "$(OUTDIR)\sampleEff.lib"
	-@erase "$(OUTDIR)\sampleEff.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\sampleEff.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\sampleEff.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

sampleEff=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sampleEff.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sampleEff.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\sampleEff.pdb" /debug /machine:I386 /def:".\sampleEff.def" /out:"..\..\..\..\maxsdk\plugin\sampleEff.dlv" /implib:"$(OUTDIR)\sampleEff.lib" 
DEF_FILE= \
	".\sampleEff.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\sampleEff.obj" \
	"$(INTDIR)\sampleEff.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sampleEff.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sampleEff - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\sampleEff.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\sampleEff.obj"
	-@erase "$(INTDIR)\sampleEff.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sampleEff.exp"
	-@erase "$(OUTDIR)\sampleEff.lib"
	-@erase "$(OUTDIR)\sampleEff.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\sampleEff.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\sampleEff.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

sampleEff=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\sampleEff.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sampleEff.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x076B0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\sampleEff.pdb" /debug /machine:I386 /def:".\sampleEff.def" /out:"..\..\..\..\maxsdk\plugin\sampleEff.dlv" /implib:"$(OUTDIR)\sampleEff.lib" 
DEF_FILE= \
	".\sampleEff.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\sampleEff.obj" \
	"$(INTDIR)\sampleEff.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\sampleEff.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("sampleEff.dep")
!INCLUDE "sampleEff.dep"
!ELSE 
!MESSAGE Warning: cannot find "sampleEff.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sampleEff - Win32 Release" || "$(CFG)" == "sampleEff - Win32 Debug" || "$(CFG)" == "sampleEff - Win32 Hybrid"
SOURCE=.\dllMain.cpp

"$(INTDIR)\dllMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sampleEff.cpp

"$(INTDIR)\sampleEff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sampleEff.rc

"$(INTDIR)\sampleEff.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

