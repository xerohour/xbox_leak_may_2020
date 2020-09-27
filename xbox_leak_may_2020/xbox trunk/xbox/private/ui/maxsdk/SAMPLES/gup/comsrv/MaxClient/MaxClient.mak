# Microsoft Developer Studio Generated NMAKE File, Based on MaxClient.dsp
!IF "$(CFG)" == ""
CFG=MaxClient - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to MaxClient - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "MaxClient - Win32 Release" && "$(CFG)" != "MaxClient - Win32 Debug" && "$(CFG)" != "MaxClient - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxClient.mak" CFG="MaxClient - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MaxClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MaxClient - Win32 Hybrid" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MaxClient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\plugin\MaxClient.exe"


CLEAN :
	-@erase "$(INTDIR)\MaxClient.obj"
	-@erase "$(INTDIR)\MaxClient.pch"
	-@erase "$(INTDIR)\MaxClient.res"
	-@erase "$(INTDIR)\MaxClientDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "..\..\..\..\plugin\MaxClient.exe"
	-@erase ".\MaxClient.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxClient.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxClient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x10000000" /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\MaxClient.pdb" /machine:I386 /out:"..\..\..\..\plugin\MaxClient.exe" /release 
LINK32_OBJS= \
	"$(INTDIR)\MaxClient.obj" \
	"$(INTDIR)\MaxClientDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\MaxClient.res"

"..\..\..\..\plugin\MaxClient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MaxClient - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\plugin\MaxClient.exe"


CLEAN :
	-@erase "$(INTDIR)\MaxClient.obj"
	-@erase "$(INTDIR)\MaxClient.pch"
	-@erase "$(INTDIR)\MaxClient.res"
	-@erase "$(INTDIR)\MaxClientDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\MaxClient.pdb"
	-@erase "..\..\..\..\plugin\MaxClient.exe"
	-@erase "..\..\..\..\plugin\MaxClient.ilk"
	-@erase ".\MaxClient.idb"
	-@erase ".\MaxClient.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxClient.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxClient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x10000000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\MaxClient.pdb" /debug /machine:I386 /out:"..\..\..\..\plugin\MaxClient.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\MaxClient.obj" \
	"$(INTDIR)\MaxClientDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\MaxClient.res"

"..\..\..\..\plugin\MaxClient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MaxClient - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "..\..\..\..\plugin\MaxClient.exe"


CLEAN :
	-@erase "$(INTDIR)\MaxClient.obj"
	-@erase "$(INTDIR)\MaxClient.pch"
	-@erase "$(INTDIR)\MaxClient.res"
	-@erase "$(INTDIR)\MaxClientDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\MaxClient.pdb"
	-@erase "..\..\..\..\plugin\MaxClient.exe"
	-@erase "..\..\..\..\plugin\MaxClient.ilk"
	-@erase ".\MaxClient.idb"
	-@erase ".\MaxClient.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MaxClient.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MaxClient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x10000000" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\MaxClient.pdb" /debug /machine:I386 /nodefaultlib:"libc msvcrt" /nodefaultlib:"msvcrt" /out:"..\..\..\..\plugin\MaxClient.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\MaxClient.obj" \
	"$(INTDIR)\MaxClientDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\MaxClient.res"

"..\..\..\..\plugin\MaxClient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MaxClient.dep")
!INCLUDE "MaxClient.dep"
!ELSE 
!MESSAGE Warning: cannot find "MaxClient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MaxClient - Win32 Release" || "$(CFG)" == "MaxClient - Win32 Debug" || "$(CFG)" == "MaxClient - Win32 Hybrid"
SOURCE=.\MaxClient.cpp

"$(INTDIR)\MaxClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MaxClient.pch"


SOURCE=.\MaxClient.rc

"$(INTDIR)\MaxClient.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\MaxClientDlg.cpp

"$(INTDIR)\MaxClientDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MaxClient.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "MaxClient - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\MaxClient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MaxClient - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\MaxClient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MaxClient - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MaxClient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"MaxClient.pdb" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\MaxClient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

