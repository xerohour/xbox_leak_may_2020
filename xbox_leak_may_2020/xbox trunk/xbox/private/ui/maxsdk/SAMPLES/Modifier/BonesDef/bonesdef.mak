# Microsoft Developer Studio Generated NMAKE File, Based on bonesdef.dsp
!IF "$(CFG)" == ""
CFG=bonesdef - Win32 Release
!MESSAGE No configuration specified. Defaulting to bonesdef - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "bonesdef - Win32 Release" && "$(CFG)" != "bonesdef - Win32 Debug" && "$(CFG)" != "bonesdef - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bonesdef.mak" CFG="bonesdef - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bonesdef - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "bonesdef - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "bonesdef - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "bonesdef - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\bonesDef.DLM"


CLEAN :
	-@erase "$(INTDIR)\BonesDef.obj"
	-@erase "$(INTDIR)\bonesdef.pch"
	-@erase "$(INTDIR)\CommandModes.obj"
	-@erase "$(INTDIR)\DistanceStuff.obj"
	-@erase "$(INTDIR)\DlgProc.obj"
	-@erase "$(INTDIR)\DrawStuff.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\RightClickMenu.obj"
	-@erase "$(OUTDIR)\bonesDef.exp"
	-@erase "$(OUTDIR)\bonesDef.lib"
	-@erase "..\..\..\..\maxsdk\plugin\bonesDef.DLM"
	-@erase ".\bonesdef.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bonesdef.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\bonesDef.pdb" /machine:I386 /def:".\Bonesdef.def" /out:"..\..\..\..\maxsdk\plugin\bonesDef.DLM" /implib:"$(OUTDIR)\bonesDef.lib" /release 
DEF_FILE= \
	".\Bonesdef.def"
LINK32_OBJS= \
	"$(INTDIR)\BonesDef.obj" \
	"$(INTDIR)\CommandModes.obj" \
	"$(INTDIR)\DistanceStuff.obj" \
	"$(INTDIR)\DlgProc.obj" \
	"$(INTDIR)\DrawStuff.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\RightClickMenu.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\gfx.lib"

"..\..\..\..\maxsdk\plugin\bonesDef.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "bonesdef - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\bonesDef.DLM"


CLEAN :
	-@erase "$(INTDIR)\BonesDef.obj"
	-@erase "$(INTDIR)\bonesdef.pch"
	-@erase "$(INTDIR)\CommandModes.obj"
	-@erase "$(INTDIR)\DistanceStuff.obj"
	-@erase "$(INTDIR)\DlgProc.obj"
	-@erase "$(INTDIR)\DrawStuff.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\RightClickMenu.obj"
	-@erase "$(OUTDIR)\bonesDef.exp"
	-@erase "$(OUTDIR)\bonesDef.lib"
	-@erase "$(OUTDIR)\bonesDef.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\bonesDef.DLM"
	-@erase "..\..\..\..\maxsdk\plugin\bonesDef.ilk"
	-@erase ".\bonesdef.idb"
	-@erase ".\bonesdef.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bonesdef.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\bonesDef.pdb" /debug /machine:I386 /def:".\Bonesdef.def" /out:"..\..\..\..\maxsdk\plugin\bonesDef.DLM" /implib:"$(OUTDIR)\bonesDef.lib" 
DEF_FILE= \
	".\Bonesdef.def"
LINK32_OBJS= \
	"$(INTDIR)\BonesDef.obj" \
	"$(INTDIR)\CommandModes.obj" \
	"$(INTDIR)\DistanceStuff.obj" \
	"$(INTDIR)\DlgProc.obj" \
	"$(INTDIR)\DrawStuff.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\RightClickMenu.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\gfx.lib"

"..\..\..\..\maxsdk\plugin\bonesDef.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "bonesdef - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\bonesDef.DLM"


CLEAN :
	-@erase "$(INTDIR)\BonesDef.obj"
	-@erase "$(INTDIR)\bonesdef.pch"
	-@erase "$(INTDIR)\CommandModes.obj"
	-@erase "$(INTDIR)\DistanceStuff.obj"
	-@erase "$(INTDIR)\DlgProc.obj"
	-@erase "$(INTDIR)\DrawStuff.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\RightClickMenu.obj"
	-@erase "$(OUTDIR)\bonesDef.exp"
	-@erase "$(OUTDIR)\bonesDef.lib"
	-@erase "$(OUTDIR)\bonesDef.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\bonesDef.DLM"
	-@erase "..\..\..\..\maxsdk\plugin\bonesDef.ilk"
	-@erase ".\bonesdef.idb"
	-@erase ".\bonesdef.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yu"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bonesdef.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\bonesDef.pdb" /debug /machine:I386 /def:".\Bonesdef.def" /out:"..\..\..\..\maxsdk\plugin\bonesDef.DLM" /implib:"$(OUTDIR)\bonesDef.lib" 
DEF_FILE= \
	".\Bonesdef.def"
LINK32_OBJS= \
	"$(INTDIR)\BonesDef.obj" \
	"$(INTDIR)\CommandModes.obj" \
	"$(INTDIR)\DistanceStuff.obj" \
	"$(INTDIR)\DlgProc.obj" \
	"$(INTDIR)\DrawStuff.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\RightClickMenu.obj" \
	"$(INTDIR)\mods.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\geom.lib" \
	"..\..\..\lib\mesh.lib" \
	"..\..\..\lib\maxutil.lib" \
	"..\..\..\lib\Maxscrpt.lib" \
	"..\..\..\lib\gfx.lib"

"..\..\..\..\maxsdk\plugin\bonesDef.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("bonesdef.dep")
!INCLUDE "bonesdef.dep"
!ELSE 
!MESSAGE Warning: cannot find "bonesdef.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "bonesdef - Win32 Release" || "$(CFG)" == "bonesdef - Win32 Debug" || "$(CFG)" == "bonesdef - Win32 Hybrid"
SOURCE=.\BonesDef.cpp

"$(INTDIR)\BonesDef.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"


SOURCE=.\CommandModes.cpp

"$(INTDIR)\CommandModes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"


SOURCE=.\DistanceStuff.cpp

"$(INTDIR)\DistanceStuff.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"


SOURCE=.\DlgProc.cpp

"$(INTDIR)\DlgProc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"


SOURCE=.\DrawStuff.cpp

"$(INTDIR)\DrawStuff.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"


SOURCE=.\mods.cpp

!IF  "$(CFG)" == "bonesdef - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\bonesdef.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "bonesdef - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\bonesdef.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "bonesdef - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\maxscrpt" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\bonesdef.pch" /Yc"mods.h" /Fo"$(INTDIR)\\" /Fd"bonesdef.pdb" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\bonesdef.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mods.rc

"$(INTDIR)\mods.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\RightClickMenu.cpp

"$(INTDIR)\RightClickMenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\bonesdef.pch"



!ENDIF 

