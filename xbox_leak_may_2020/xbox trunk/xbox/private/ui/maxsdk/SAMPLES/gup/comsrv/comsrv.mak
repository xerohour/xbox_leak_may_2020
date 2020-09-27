# Microsoft Developer Studio Generated NMAKE File, Based on comsrv.dsp
!IF "$(CFG)" == ""
CFG=comsrv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to comsrv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "comsrv - Win32 Debug" && "$(CFG)" != "comsrv - Win32 Release" && "$(CFG)" != "comsrv - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "comsrv.mak" CFG="comsrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "comsrv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "comsrv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "comsrv - Win32 Hybrid" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "comsrv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\comsrv.gup" ".\comsrv.tlb" ".\comsrv.h" ".\comsrv_i.c" "$(OUTDIR)\comsrv.pch"


CLEAN :
	-@erase "$(INTDIR)\BitmapInfo.obj"
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.pch"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(INTDIR)\MaxRenderer.obj"
	-@erase "$(INTDIR)\mscom.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\comsrv.exp"
	-@erase "$(OUTDIR)\comsrv.lib"
	-@erase "$(OUTDIR)\comsrv.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\comsrv.gup"
	-@erase "..\..\..\..\maxsdk\plugin\comsrv.ilk"
	-@erase ".\comsrv.h"
	-@erase ".\comsrv.idb"
	-@erase ".\comsrv.pdb"
	-@erase ".\comsrv.tlb"
	-@erase ".\comsrv_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x084A0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\comsrv.pdb" /debug /machine:I386 /def:".\mscom.def" /out:"..\..\..\..\maxsdk\plugin\comsrv.gup" /implib:"$(OUTDIR)\comsrv.lib" /pdbtype:sept 
DEF_FILE= \
	".\mscom.def"
LINK32_OBJS= \
	"$(INTDIR)\BitmapInfo.obj" \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\MaxRenderer.obj" \
	"$(INTDIR)\mscom.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrv.gup" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "comsrv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\comsrv.gup" ".\comsrv.tlb" ".\comsrv.h" ".\comsrv_i.c" "$(OUTDIR)\comsrv.pch"


CLEAN :
	-@erase "$(INTDIR)\BitmapInfo.obj"
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.pch"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(INTDIR)\MaxRenderer.obj"
	-@erase "$(INTDIR)\mscom.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\comsrv.exp"
	-@erase "$(OUTDIR)\comsrv.lib"
	-@erase "..\..\..\..\maxsdk\plugin\comsrv.gup"
	-@erase ".\comsrv.h"
	-@erase ".\comsrv.idb"
	-@erase ".\comsrv.tlb"
	-@erase ".\comsrv_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x084A0000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\comsrv.pdb" /machine:I386 /def:".\mscom.def" /out:"..\..\..\..\maxsdk\plugin\comsrv.gup" /implib:"$(OUTDIR)\comsrv.lib" /release 
DEF_FILE= \
	".\mscom.def"
LINK32_OBJS= \
	"$(INTDIR)\BitmapInfo.obj" \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\MaxRenderer.obj" \
	"$(INTDIR)\mscom.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrv.gup" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "comsrv - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid
# Begin Custom Macros
OutDir=.\Hybrid
# End Custom Macros

ALL : "..\..\..\..\maxsdk\plugin\comsrv.gup" ".\comsrv.tlb" ".\comsrv.h" ".\comsrv_i.c" "$(OUTDIR)\comsrv.pch"


CLEAN :
	-@erase "$(INTDIR)\BitmapInfo.obj"
	-@erase "$(INTDIR)\comsrv.obj"
	-@erase "$(INTDIR)\comsrv.pch"
	-@erase "$(INTDIR)\comsrv.res"
	-@erase "$(INTDIR)\MaxRenderer.obj"
	-@erase "$(INTDIR)\mscom.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\comsrv.exp"
	-@erase "$(OUTDIR)\comsrv.lib"
	-@erase "$(OUTDIR)\comsrv.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\comsrv.gup"
	-@erase "..\..\..\..\maxsdk\plugin\comsrv.ilk"
	-@erase ".\comsrv.h"
	-@erase ".\comsrv.idb"
	-@erase ".\comsrv.pdb"
	-@erase ".\comsrv.tlb"
	-@erase ".\comsrv_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /ZI /Od /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\comsrv.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comsrv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x084A0000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\comsrv.pdb" /debug /machine:I386 /def:".\mscom.def" /out:"..\..\..\..\maxsdk\plugin\comsrv.gup" /implib:"$(OUTDIR)\comsrv.lib" 
DEF_FILE= \
	".\mscom.def"
LINK32_OBJS= \
	"$(INTDIR)\BitmapInfo.obj" \
	"$(INTDIR)\comsrv.obj" \
	"$(INTDIR)\MaxRenderer.obj" \
	"$(INTDIR)\mscom.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\comsrv.res" \
	"..\..\..\lib\core.lib" \
	"..\..\..\lib\gup.lib" \
	"..\..\..\lib\bmm.lib" \
	"..\..\..\lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\comsrv.gup" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("comsrv.dep")
!INCLUDE "comsrv.dep"
!ELSE 
!MESSAGE Warning: cannot find "comsrv.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "comsrv - Win32 Debug" || "$(CFG)" == "comsrv - Win32 Release" || "$(CFG)" == "comsrv - Win32 Hybrid"
SOURCE=.\BitmapInfo.cpp

"$(INTDIR)\BitmapInfo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\comsrv.cpp

"$(INTDIR)\comsrv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\comsrv.idl

!IF  "$(CFG)" == "comsrv - Win32 Debug"

MTL_SWITCHES=/tlb ".\comsrv.tlb" /h "comsrv.h" /iid "comsrv_i.c" /Oicf 

".\comsrv.tlb"	".\comsrv.h"	".\comsrv_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "comsrv - Win32 Release"

MTL_SWITCHES=/tlb ".\comsrv.tlb" /h "comsrv.h" /iid "comsrv_i.c" /Oicf 

".\comsrv.tlb"	".\comsrv.h"	".\comsrv_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "comsrv - Win32 Hybrid"

MTL_SWITCHES=/tlb ".\comsrv.tlb" /h "comsrv.h" /iid "comsrv_i.c" /Oicf 

".\comsrv.tlb"	".\comsrv.h"	".\comsrv_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\comsrv.rc

"$(INTDIR)\comsrv.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\MaxRenderer.cpp

"$(INTDIR)\MaxRenderer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mscom.cpp

"$(INTDIR)\mscom.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "comsrv - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\comsrv.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "comsrv - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\comsrv.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "comsrv - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /ZI /Od /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\comsrv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"comsrv.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\comsrv.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

