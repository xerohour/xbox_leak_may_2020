# Microsoft Developer Studio Generated NMAKE File, Based on fileOut.dsp
!IF "$(CFG)" == ""
CFG=fileOut - Win32 Release
!MESSAGE No configuration specified. Defaulting to fileOut - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "fileOut - Win32 Release" && "$(CFG)" != "fileOut - Win32 Debug" && "$(CFG)" != "fileOut - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fileOut.mak" CFG="fileOut - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fileOut - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileOut - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileOut - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "fileOut - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\fileOut.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\fileOut.obj"
	-@erase "$(INTDIR)\fileOut.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\fileOut.exp"
	-@erase "$(OUTDIR)\fileOut.lib"
	-@erase "..\..\..\..\maxsdk\plugin\fileOut.dlv"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

fileOut=midl.exe
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\fileOut.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileOut.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07660000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\fileOut.pdb" /machine:I386 /def:".\fileOut.def" /out:"..\..\..\..\maxsdk\plugin\fileOut.dlv" /implib:"$(OUTDIR)\fileOut.lib" /release 
DEF_FILE= \
	".\fileOut.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\fileOut.obj" \
	"$(INTDIR)\fileOut.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\fileOut.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fileOut - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\fileOut.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\fileOut.obj"
	-@erase "$(INTDIR)\fileOut.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\fileOut.exp"
	-@erase "$(OUTDIR)\fileOut.lib"
	-@erase "$(OUTDIR)\fileOut.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\fileOut.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\fileOut.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

fileOut=midl.exe
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\fileOut.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileOut.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07660000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\fileOut.pdb" /debug /machine:I386 /def:".\fileOut.def" /out:"..\..\..\..\maxsdk\plugin\fileOut.dlv" /implib:"$(OUTDIR)\fileOut.lib" 
DEF_FILE= \
	".\fileOut.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\fileOut.obj" \
	"$(INTDIR)\fileOut.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\fileOut.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fileOut - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\fileOut.dlv"


CLEAN :
	-@erase "$(INTDIR)\dllMain.obj"
	-@erase "$(INTDIR)\fileOut.obj"
	-@erase "$(INTDIR)\fileOut.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\fileOut.exp"
	-@erase "$(OUTDIR)\fileOut.lib"
	-@erase "$(OUTDIR)\fileOut.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\fileOut.dlv"
	-@erase "..\..\..\..\maxsdk\plugin\fileOut.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

fileOut=midl.exe
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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\fileOut.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileOut.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x07660000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\fileOut.pdb" /debug /machine:I386 /def:".\fileOut.def" /out:"..\..\..\..\maxsdk\plugin\fileOut.dlv" /implib:"$(OUTDIR)\fileOut.lib" 
DEF_FILE= \
	".\fileOut.def"
LINK32_OBJS= \
	"$(INTDIR)\dllMain.obj" \
	"$(INTDIR)\fileOut.obj" \
	"$(INTDIR)\fileOut.res" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\Paramblk2.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\fileOut.dlv" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("fileOut.dep")
!INCLUDE "fileOut.dep"
!ELSE 
!MESSAGE Warning: cannot find "fileOut.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "fileOut - Win32 Release" || "$(CFG)" == "fileOut - Win32 Debug" || "$(CFG)" == "fileOut - Win32 Hybrid"
SOURCE=.\dllMain.cpp

"$(INTDIR)\dllMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fileOut.cpp

"$(INTDIR)\fileOut.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fileOut.rc

"$(INTDIR)\fileOut.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

