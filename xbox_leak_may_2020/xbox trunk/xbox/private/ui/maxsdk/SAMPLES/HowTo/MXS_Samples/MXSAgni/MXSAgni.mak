# Microsoft Developer Studio Generated NMAKE File, Based on MXSAgni.dsp
!IF "$(CFG)" == ""
CFG=MXSAgni - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to MXSAgni - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "MXSAgni - Win32 Release" && "$(CFG)" != "MXSAgni - Win32 Debug" && "$(CFG)" != "MXSAgni - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MXSAgni.mak" CFG="MXSAgni - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MXSAgni - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MXSAgni - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MXSAgni - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MXSAgni - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"


CLEAN :
	-@erase "$(INTDIR)\DllMain.obj"
	-@erase "$(INTDIR)\ExtClass.obj"
	-@erase "$(INTDIR)\MXSAgni.obj"
	-@erase "$(INTDIR)\MXSAgni.res"
	-@erase "$(INTDIR)\PhyExprt.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MXSAgni.exp"
	-@erase "$(OUTDIR)\MXSAgni.lib"
	-@erase "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GR /GX /O2 /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Zm150 /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MXSAgni.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MXSAgni.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib MNMath.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\MXSAgni.pdb" /machine:I386 /def:".\MXSAgni.def" /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /implib:"$(OUTDIR)\MXSAgni.lib" /libpath:"..\..\..\..\lib" /release 
DEF_FILE= \
	".\MXSAgni.def"
LINK32_OBJS= \
	"$(INTDIR)\DllMain.obj" \
	"$(INTDIR)\ExtClass.obj" \
	"$(INTDIR)\MXSAgni.obj" \
	"$(INTDIR)\PhyExprt.obj" \
	"$(INTDIR)\MXSAgni.res"

"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MXSAgni - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"


CLEAN :
	-@erase "$(INTDIR)\DllMain.obj"
	-@erase "$(INTDIR)\ExtClass.obj"
	-@erase "$(INTDIR)\MXSAgni.obj"
	-@erase "$(INTDIR)\MXSAgni.res"
	-@erase "$(INTDIR)\PhyExprt.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MXSAgni.exp"
	-@erase "$(OUTDIR)\MXSAgni.lib"
	-@erase "$(OUTDIR)\MXSAgni.pdb"
	-@erase "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"
	-@erase "..\..\..\..\..\maxsdk\plugin\MXSAgni.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MXSAgni.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MXSAgni.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib bmm.lib MNMath.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\MXSAgni.pdb" /debug /machine:I386 /def:".\MXSAgni.def" /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /implib:"$(OUTDIR)\MXSAgni.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\MXSAgni.def"
LINK32_OBJS= \
	"$(INTDIR)\DllMain.obj" \
	"$(INTDIR)\ExtClass.obj" \
	"$(INTDIR)\MXSAgni.obj" \
	"$(INTDIR)\PhyExprt.obj" \
	"$(INTDIR)\MXSAgni.res"

"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MXSAgni - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"


CLEAN :
	-@erase "$(INTDIR)\DllMain.obj"
	-@erase "$(INTDIR)\ExtClass.obj"
	-@erase "$(INTDIR)\MXSAgni.obj"
	-@erase "$(INTDIR)\MXSAgni.res"
	-@erase "$(INTDIR)\PhyExprt.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MXSAgni.exp"
	-@erase "$(OUTDIR)\MXSAgni.lib"
	-@erase "$(OUTDIR)\MXSAgni.pdb"
	-@erase "..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx"
	-@erase "..\..\..\..\..\maxsdk\plugin\MXSAgni.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\include\maxscrpt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Zm150 /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MXSAgni.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MXSAgni.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib MNMath.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib core.lib maxutil.lib maxscrpt.lib geom.lib /nologo /base:"0x0B0F0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\MXSAgni.pdb" /debug /machine:I386 /def:".\MXSAgni.def" /out:"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" /implib:"$(OUTDIR)\MXSAgni.lib" /pdbtype:sept /libpath:"..\..\..\..\lib" 
DEF_FILE= \
	".\MXSAgni.def"
LINK32_OBJS= \
	"$(INTDIR)\DllMain.obj" \
	"$(INTDIR)\ExtClass.obj" \
	"$(INTDIR)\MXSAgni.obj" \
	"$(INTDIR)\PhyExprt.obj" \
	"$(INTDIR)\MXSAgni.res"

"..\..\..\..\..\maxsdk\plugin\MXSAgni.dlx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MXSAgni.dep")
!INCLUDE "MXSAgni.dep"
!ELSE 
!MESSAGE Warning: cannot find "MXSAgni.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MXSAgni - Win32 Release" || "$(CFG)" == "MXSAgni - Win32 Debug" || "$(CFG)" == "MXSAgni - Win32 Hybrid"
SOURCE=.\DllMain.cpp

"$(INTDIR)\DllMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ExtClass.cpp

"$(INTDIR)\ExtClass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MXSAgni.cpp

"$(INTDIR)\MXSAgni.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MXSAgni.rc

"$(INTDIR)\MXSAgni.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\PhyExprt.cpp

"$(INTDIR)\PhyExprt.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

