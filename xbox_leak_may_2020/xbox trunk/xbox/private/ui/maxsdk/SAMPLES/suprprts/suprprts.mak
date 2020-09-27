# Microsoft Developer Studio Generated NMAKE File, Based on suprprts.dsp
!IF "$(CFG)" == ""
CFG=SuprPrts - Win32 Release
!MESSAGE No configuration specified. Defaulting to SuprPrts - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "SuprPrts - Win32 Release" && "$(CFG)" != "SuprPrts - Win32 Debug" && "$(CFG)" != "SuprPrts - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "suprprts.mak" CFG="SuprPrts - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SuprPrts - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SuprPrts - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SuprPrts - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "SuprPrts - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\suprprts.dlo"


CLEAN :
	-@erase "$(INTDIR)\BLIZZARD.OBJ"
	-@erase "$(INTDIR)\PArray.obj"
	-@erase "$(INTDIR)\pbomb.obj"
	-@erase "$(INTDIR)\PCloud.obj"
	-@erase "$(INTDIR)\pfollow.obj"
	-@erase "$(INTDIR)\sphered.obj"
	-@erase "$(INTDIR)\suprPrts.obj"
	-@erase "$(INTDIR)\SuprPrts.res"
	-@erase "$(INTDIR)\udeflect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\suprprts.exp"
	-@erase "$(OUTDIR)\suprprts.lib"
	-@erase "..\..\..\maxsdk\plugin\suprprts.dlo"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\suprprts.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SuprPrts.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\suprprts.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x05610000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\suprprts.pdb" /machine:I386 /def:".\SuprPrts.def" /out:"..\..\..\maxsdk\plugin\suprprts.dlo" /implib:"$(OUTDIR)\suprprts.lib" /release 
DEF_FILE= \
	".\SuprPrts.def"
LINK32_OBJS= \
	"$(INTDIR)\BLIZZARD.OBJ" \
	"$(INTDIR)\PArray.obj" \
	"$(INTDIR)\pbomb.obj" \
	"$(INTDIR)\PCloud.obj" \
	"$(INTDIR)\pfollow.obj" \
	"$(INTDIR)\sphered.obj" \
	"$(INTDIR)\suprPrts.obj" \
	"$(INTDIR)\udeflect.obj" \
	"$(INTDIR)\SuprPrts.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\suprprts.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuprPrts - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\suprprts.dlo"


CLEAN :
	-@erase "$(INTDIR)\BLIZZARD.OBJ"
	-@erase "$(INTDIR)\PArray.obj"
	-@erase "$(INTDIR)\pbomb.obj"
	-@erase "$(INTDIR)\PCloud.obj"
	-@erase "$(INTDIR)\pfollow.obj"
	-@erase "$(INTDIR)\sphered.obj"
	-@erase "$(INTDIR)\suprPrts.obj"
	-@erase "$(INTDIR)\SuprPrts.res"
	-@erase "$(INTDIR)\udeflect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\suprprts.exp"
	-@erase "$(OUTDIR)\suprprts.lib"
	-@erase "$(OUTDIR)\suprprts.pdb"
	-@erase "..\..\..\maxsdk\plugin\suprprts.dlo"
	-@erase "..\..\..\maxsdk\plugin\suprprts.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\suprprts.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SuprPrts.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\suprprts.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x05610000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\suprprts.pdb" /debug /machine:I386 /def:".\SuprPrts.def" /out:"..\..\..\maxsdk\plugin\suprprts.dlo" /implib:"$(OUTDIR)\suprprts.lib" 
DEF_FILE= \
	".\SuprPrts.def"
LINK32_OBJS= \
	"$(INTDIR)\BLIZZARD.OBJ" \
	"$(INTDIR)\PArray.obj" \
	"$(INTDIR)\pbomb.obj" \
	"$(INTDIR)\PCloud.obj" \
	"$(INTDIR)\pfollow.obj" \
	"$(INTDIR)\sphered.obj" \
	"$(INTDIR)\suprPrts.obj" \
	"$(INTDIR)\udeflect.obj" \
	"$(INTDIR)\SuprPrts.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\suprprts.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SuprPrts - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\suprprts.dlo"


CLEAN :
	-@erase "$(INTDIR)\BLIZZARD.OBJ"
	-@erase "$(INTDIR)\PArray.obj"
	-@erase "$(INTDIR)\pbomb.obj"
	-@erase "$(INTDIR)\PCloud.obj"
	-@erase "$(INTDIR)\pfollow.obj"
	-@erase "$(INTDIR)\sphered.obj"
	-@erase "$(INTDIR)\suprPrts.obj"
	-@erase "$(INTDIR)\SuprPrts.res"
	-@erase "$(INTDIR)\udeflect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\suprprts.exp"
	-@erase "$(OUTDIR)\suprprts.lib"
	-@erase "$(OUTDIR)\suprprts.pdb"
	-@erase "..\..\..\maxsdk\plugin\suprprts.dlo"
	-@erase "..\..\..\maxsdk\plugin\suprprts.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\suprprts.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SuprPrts.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\suprprts.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x05610000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\suprprts.pdb" /debug /machine:I386 /def:".\SuprPrts.def" /out:"..\..\..\maxsdk\plugin\suprprts.dlo" /implib:"$(OUTDIR)\suprprts.lib" 
DEF_FILE= \
	".\SuprPrts.def"
LINK32_OBJS= \
	"$(INTDIR)\BLIZZARD.OBJ" \
	"$(INTDIR)\PArray.obj" \
	"$(INTDIR)\pbomb.obj" \
	"$(INTDIR)\PCloud.obj" \
	"$(INTDIR)\pfollow.obj" \
	"$(INTDIR)\sphered.obj" \
	"$(INTDIR)\suprPrts.obj" \
	"$(INTDIR)\udeflect.obj" \
	"$(INTDIR)\SuprPrts.res" \
	"..\..\lib\bmm.lib" \
	"..\..\lib\core.lib" \
	"..\..\lib\geom.lib" \
	"..\..\lib\gfx.lib" \
	"..\..\lib\mesh.lib" \
	"..\..\lib\particle.lib" \
	"..\..\lib\maxutil.lib"

"..\..\..\maxsdk\plugin\suprprts.dlo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("suprprts.dep")
!INCLUDE "suprprts.dep"
!ELSE 
!MESSAGE Warning: cannot find "suprprts.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SuprPrts - Win32 Release" || "$(CFG)" == "SuprPrts - Win32 Debug" || "$(CFG)" == "SuprPrts - Win32 Hybrid"
SOURCE=.\BLIZZARD.CPP

"$(INTDIR)\BLIZZARD.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PArray.CPP

"$(INTDIR)\PArray.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pbomb.cpp

"$(INTDIR)\pbomb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PCloud.cpp

"$(INTDIR)\PCloud.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pfollow.cpp

"$(INTDIR)\pfollow.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sphered.cpp

"$(INTDIR)\sphered.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\suprPrts.cpp

"$(INTDIR)\suprPrts.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SuprPrts.rc

"$(INTDIR)\SuprPrts.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\udeflect.cpp

"$(INTDIR)\udeflect.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

