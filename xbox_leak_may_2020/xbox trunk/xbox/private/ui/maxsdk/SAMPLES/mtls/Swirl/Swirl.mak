# Microsoft Developer Studio Generated NMAKE File, Based on Swirl.dsp
!IF "$(CFG)" == ""
CFG=Swirl - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Swirl - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Swirl - Win32 Release" && "$(CFG)" != "Swirl - Win32 Debug" && "$(CFG)" != "Swirl - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Swirl.mak" CFG="Swirl - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Swirl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Swirl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Swirl - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Swirl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\maxsdk\plugin\Swirl.dlt"


CLEAN :
	-@erase "$(INTDIR)\Swirl.obj"
	-@erase "$(INTDIR)\Swirl.res"
	-@erase "$(INTDIR)\Tex.obj"
	-@erase "$(OUTDIR)\Swirl.exp"
	-@erase "$(OUTDIR)\Swirl.lib"
	-@erase "..\..\..\..\maxsdk\plugin\Swirl.dlt"
	-@erase ".\Swirl.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SWIRL_EXPORTS" /Fp"$(INTDIR)\Swirl.pch" /YX /Fo"$(INTDIR)\\" /Fd"Swirl.pdb" /FD /c 

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
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Swirl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swirl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02EA0000" /dll /incremental:no /pdb:"$(OUTDIR)\Swirl.pdb" /machine:I386 /def:".\Swirl.def" /out:"..\..\..\..\maxsdk\plugin\Swirl.dlt" /implib:"$(OUTDIR)\Swirl.lib" /libpath:"..\..\..\lib" /release 
DEF_FILE= \
	".\Swirl.def"
LINK32_OBJS= \
	"$(INTDIR)\Swirl.obj" \
	"$(INTDIR)\Tex.obj" \
	"$(INTDIR)\Swirl.res"

"..\..\..\..\maxsdk\plugin\Swirl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Swirl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\maxsdk\plugin\Swirl.dlt"


CLEAN :
	-@erase "$(INTDIR)\Swirl.obj"
	-@erase "$(INTDIR)\Swirl.res"
	-@erase "$(INTDIR)\Tex.obj"
	-@erase "$(OUTDIR)\Swirl.exp"
	-@erase "$(OUTDIR)\Swirl.lib"
	-@erase "$(OUTDIR)\Swirl.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Swirl.dlt"
	-@erase "..\..\..\..\maxsdk\plugin\Swirl.ilk"
	-@erase ".\Swirl.idb"
	-@erase ".\Swirl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SWIRL_EXPORTS" /Fp"$(INTDIR)\Swirl.pch" /YX /Fo"$(INTDIR)\\" /Fd"Swirl.pdb" /FD /GZ /c 

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
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Swirl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swirl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02EA0000" /dll /incremental:yes /pdb:"$(OUTDIR)\Swirl.pdb" /debug /machine:I386 /def:".\Swirl.def" /out:"..\..\..\..\maxsdk\plugin\Swirl.dlt" /implib:"$(OUTDIR)\Swirl.lib" /pdbtype:sept /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\Swirl.def"
LINK32_OBJS= \
	"$(INTDIR)\Swirl.obj" \
	"$(INTDIR)\Tex.obj" \
	"$(INTDIR)\Swirl.res"

"..\..\..\..\maxsdk\plugin\Swirl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Swirl - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\maxsdk\plugin\Swirl.dlt"


CLEAN :
	-@erase "$(INTDIR)\Swirl.obj"
	-@erase "$(INTDIR)\Swirl.res"
	-@erase "$(INTDIR)\Tex.obj"
	-@erase "$(OUTDIR)\Swirl.exp"
	-@erase "$(OUTDIR)\Swirl.lib"
	-@erase "$(OUTDIR)\Swirl.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\Swirl.dlt"
	-@erase "..\..\..\..\maxsdk\plugin\Swirl.ilk"
	-@erase ".\Swirl.idb"
	-@erase ".\Swirl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SWIRL_EXPORTS" /Fp"$(INTDIR)\Swirl.pch" /YX /Fo"$(INTDIR)\\" /Fd"Swirl.pdb" /FD /GZ /c 

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
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Swirl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swirl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=bmm.lib core.lib geom.lib maxutil.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x02EA0000" /dll /incremental:yes /pdb:"$(OUTDIR)\Swirl.pdb" /debug /machine:I386 /def:".\Swirl.def" /out:"..\..\..\..\maxsdk\plugin\Swirl.dlt" /implib:"$(OUTDIR)\Swirl.lib" /pdbtype:sept /libpath:"..\..\..\lib" 
DEF_FILE= \
	".\Swirl.def"
LINK32_OBJS= \
	"$(INTDIR)\Swirl.obj" \
	"$(INTDIR)\Tex.obj" \
	"$(INTDIR)\Swirl.res"

"..\..\..\..\maxsdk\plugin\Swirl.dlt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Swirl.dep")
!INCLUDE "Swirl.dep"
!ELSE 
!MESSAGE Warning: cannot find "Swirl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Swirl - Win32 Release" || "$(CFG)" == "Swirl - Win32 Debug" || "$(CFG)" == "Swirl - Win32 Hybrid"
SOURCE=.\Swirl.cpp

"$(INTDIR)\Swirl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Swirl.rc

"$(INTDIR)\Swirl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Tex.cpp

"$(INTDIR)\Tex.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

