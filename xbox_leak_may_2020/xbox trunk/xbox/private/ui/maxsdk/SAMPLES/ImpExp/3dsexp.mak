# Microsoft Developer Studio Generated NMAKE File, Based on 3dsexp.dsp
!IF "$(CFG)" == ""
CFG=3dsexp - Win32 Release
!MESSAGE No configuration specified. Defaulting to 3dsexp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "3dsexp - Win32 Release" && "$(CFG)" != "3dsexp - Win32 Debug" && "$(CFG)" != "3dsexp - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "3dsexp.mak" CFG="3dsexp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "3dsexp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "3dsexp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "3dsexp - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "3dsexp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\3dsexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsexp.obj"
	-@erase "$(INTDIR)\3dsexp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\savemli.obj"
	-@erase "$(OUTDIR)\3dsexp.exp"
	-@erase "$(OUTDIR)\3dsexp.lib"
	-@erase "..\..\..\maxsdk\plugin\3dsexp.dle"
	-@erase ".\3dsexp.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsexp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F10000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\3dsexp.pdb" /machine:I386 /def:".\3dsexp.def" /out:"..\..\..\maxsdk\plugin\3dsexp.dle" /implib:"$(OUTDIR)\3dsexp.lib" /release 
DEF_FILE= \
	".\3dsexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsexp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\savemli.obj" \
	"$(INTDIR)\3dsexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "3dsexp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\3dsexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsexp.obj"
	-@erase "$(INTDIR)\3dsexp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\savemli.obj"
	-@erase "$(OUTDIR)\3dsexp.exp"
	-@erase "$(OUTDIR)\3dsexp.lib"
	-@erase "$(OUTDIR)\3dsexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\3dsexp.dle"
	-@erase "..\..\..\maxsdk\plugin\3dsexp.ilk"
	-@erase ".\3dsexp.idb"
	-@erase ".\3dsexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\3dsexp.pdb" /debug /machine:I386 /def:".\3dsexp.def" /out:"..\..\..\maxsdk\plugin\3dsexp.dle" /implib:"$(OUTDIR)\3dsexp.lib" 
DEF_FILE= \
	".\3dsexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsexp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\savemli.obj" \
	"$(INTDIR)\3dsexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "3dsexp - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\3dsexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsexp.obj"
	-@erase "$(INTDIR)\3dsexp.res"
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\savemli.obj"
	-@erase "$(OUTDIR)\3dsexp.exp"
	-@erase "$(OUTDIR)\3dsexp.lib"
	-@erase "$(OUTDIR)\3dsexp.pdb"
	-@erase "..\..\..\maxsdk\plugin\3dsexp.dle"
	-@erase "..\..\..\maxsdk\plugin\3dsexp.ilk"
	-@erase ".\3dsexp.idb"
	-@erase ".\3dsexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"3dsexp.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\3dsexp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\3dsexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x02F10000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\3dsexp.pdb" /debug /machine:I386 /def:".\3dsexp.def" /out:"..\..\..\maxsdk\plugin\3dsexp.dle" /implib:"$(OUTDIR)\3dsexp.lib" 
DEF_FILE= \
	".\3dsexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsexp.obj" \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\savemli.obj" \
	"$(INTDIR)\3dsexp.res" \
	"..\..\Lib\core.lib" \
	"..\..\Lib\geom.lib" \
	"..\..\Lib\gfx.lib" \
	"..\..\Lib\mesh.lib" \
	"..\..\Lib\maxutil.lib" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\3dsexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("3dsexp.dep")
!INCLUDE "3dsexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "3dsexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "3dsexp - Win32 Release" || "$(CFG)" == "3dsexp - Win32 Debug" || "$(CFG)" == "3dsexp - Win32 Hybrid"
SOURCE=.\3dsexp.cpp

"$(INTDIR)\3dsexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\3dsexp.rc

"$(INTDIR)\3dsexp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\3dsmtl.cpp

"$(INTDIR)\3dsmtl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\savemli.cpp

"$(INTDIR)\savemli.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

