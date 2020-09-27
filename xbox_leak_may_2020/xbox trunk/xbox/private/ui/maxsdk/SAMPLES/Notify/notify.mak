# Microsoft Developer Studio Generated NMAKE File, Based on Notify.dsp
!IF "$(CFG)" == ""
CFG=Notify - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to Notify - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "Notify - Win32 Release" && "$(CFG)" != "Notify - Win32 Debug" && "$(CFG)" != "Notify - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Notify.mak" CFG="Notify - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Notify - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Notify - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Notify - Win32 Hybrid" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Notify - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\maxsdk\plugin\Notify.exe"


CLEAN :
	-@erase "$(INTDIR)\notifdlg.obj"
	-@erase "$(INTDIR)\Notify.obj"
	-@erase "$(INTDIR)\Notify.pch"
	-@erase "$(INTDIR)\Notify.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "..\..\..\maxsdk\plugin\Notify.exe"
	-@erase ".\Notify.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Notify.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Notify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib /nologo /base:"0x00400000" /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Notify.pdb" /machine:I386 /out:"..\..\..\maxsdk\plugin\Notify.exe" /release 
LINK32_OBJS= \
	"$(INTDIR)\notifdlg.obj" \
	"$(INTDIR)\Notify.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Notify.res" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\Notify.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Notify - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\maxsdk\plugin\Notify.exe"


CLEAN :
	-@erase "$(INTDIR)\notifdlg.obj"
	-@erase "$(INTDIR)\Notify.obj"
	-@erase "$(INTDIR)\Notify.pch"
	-@erase "$(INTDIR)\Notify.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\Notify.pdb"
	-@erase "..\..\..\maxsdk\plugin\Notify.exe"
	-@erase "..\..\..\maxsdk\plugin\Notify.ilk"
	-@erase ".\Notify.idb"
	-@erase ".\Notify.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Notify.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Notify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib /nologo /base:"0x00400000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Notify.pdb" /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\Notify.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\notifdlg.obj" \
	"$(INTDIR)\Notify.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Notify.res" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\Notify.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Notify - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\maxsdk\plugin\Notify.exe"


CLEAN :
	-@erase "$(INTDIR)\notifdlg.obj"
	-@erase "$(INTDIR)\Notify.obj"
	-@erase "$(INTDIR)\Notify.pch"
	-@erase "$(INTDIR)\Notify.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\Notify.pdb"
	-@erase "..\..\..\maxsdk\plugin\Notify.exe"
	-@erase "..\..\..\maxsdk\plugin\Notify.ilk"
	-@erase ".\Notify.idb"
	-@erase ".\Notify.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Notify.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Notify.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib /nologo /base:"0x00400000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Notify.pdb" /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\Notify.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\notifdlg.obj" \
	"$(INTDIR)\Notify.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Notify.res" \
	"..\..\lib\helpsys.lib"

"..\..\..\maxsdk\plugin\Notify.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Notify.dep")
!INCLUDE "Notify.dep"
!ELSE 
!MESSAGE Warning: cannot find "Notify.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Notify - Win32 Release" || "$(CFG)" == "Notify - Win32 Debug" || "$(CFG)" == "Notify - Win32 Hybrid"
SOURCE=.\notifdlg.cpp

"$(INTDIR)\notifdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Notify.pch"


SOURCE=.\Notify.cpp

"$(INTDIR)\Notify.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Notify.pch"


SOURCE=.\Notify.rc

"$(INTDIR)\Notify.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Notify - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Notify.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Notify - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Notify.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Notify - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Notify.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"Notify.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Notify.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

