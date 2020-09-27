# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=cafebld - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to cafebld - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cafebld - Win32 Release" && "$(CFG)" !=\
 "cafebld - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cafebld.mak" CFG="cafebld - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cafebld - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cafebld - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "cafebld - Win32 Debug"
F90=fl32.exe
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cafebld - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\cafebld.exe"

CLEAN : 
	-@erase ".\Release\cafebld.idb"
	-@erase ".\Release\cafebld.pch"
	-@erase ".\Release\cafebld.exe"
	-@erase ".\Release\STDAFX.OBJ"
	-@erase ".\Release\settings.obj"
	-@erase ".\Release\CAFEBLD.OBJ"
	-@erase ".\Release\cafedlg.obj"
	-@erase ".\Release\cafebld.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\cafebld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE F90 /Ox /I "Release/" /c /nologo
# ADD F90 /Ox /I "Release/" /c /nologo
F90_PROJ=/Ox /I "Release/" /c /nologo /Fo"Release/" 
F90_OBJS=.\Release/
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/cafebld.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/cafebld.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cafebld.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cafebld.bsc" 
BSC32_SBRS=

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/cafebld.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/cafebld.exe" 
LINK32_OBJS= \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/CAFEBLD.OBJ" \
	"$(INTDIR)/cafedlg.obj" \
	"$(INTDIR)/cafebld.res"

"$(OUTDIR)\cafebld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\cafebld.exe"

CLEAN : 
	-@erase ".\Debug\cafebld.pdb"
	-@erase ".\Debug\STDAFX.OBJ"
	-@erase ".\Debug\CAFEBLD.OBJ"
	-@erase ".\Debug\cafebld.pch"
	-@erase ".\Debug\settings.obj"
	-@erase ".\Debug\cafedlg.obj"
	-@erase ".\Debug\cafebld.res"
	-@erase ".\Debug\cafebld.idb"
	-@erase ".\Debug\cafebld.exe"
	-@erase ".\Debug\cafebld.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)

"$(OUTDIR)\cafebld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE F90 /Zi /I "Debug/" /c /nologo
# ADD F90 /Zi /I "Debug/" /c /nologo
F90_PROJ=/Zi /I "Debug/" /c /nologo /Fo"Debug/" /Fd"Debug/cafebld.pdb" 
F90_OBJS=.\Debug/
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/cafebld.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/cafebld.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cafebld.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cafebld.bsc" 
BSC32_SBRS=

"$(OUTDIR)\cafebld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/cafebld.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/cafebld.exe" 
LINK32_OBJS= \
	"$(INTDIR)/STDAFX.OBJ" \
	"$(INTDIR)/CAFEBLD.OBJ" \
	"$(INTDIR)/settings.obj" \
	"$(INTDIR)/cafedlg.obj" \
	"$(INTDIR)/cafebld.res"

"$(OUTDIR)\cafebld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "cafebld - Win32 Release"
# Name "cafebld - Win32 Debug"

!IF  "$(CFG)" == "cafebld - Win32 Release"

!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\CAFEBLD.CPP
DEP_CPP_CAFEB=\
	".\CAFEBLD.H"\
	".\resource.h"\
	".\cafedlg.h"\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "cafebld - Win32 Release"


"$(INTDIR)\CAFEBLD.OBJ" : $(SOURCE) $(DEP_CPP_CAFEB) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_CAFEB) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"


"$(INTDIR)\CAFEBLD.OBJ" : $(SOURCE) $(DEP_CPP_CAFEB) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.pdb" : $(SOURCE) $(DEP_CPP_CAFEB) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_CAFEB) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cafebld.rc
DEP_RSC_CAFEBL=\
	".\res\cafebld.ico"\
	".\res\CAFEBLD.RC2"\
	

"$(INTDIR)\cafebld.res" : $(SOURCE) $(DEP_RSC_CAFEBL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cafedlg.cpp
DEP_CPP_CAFED=\
	".\CAFEBLD.H"\
	".\resource.h"\
	".\cafedlg.h"\
	".\settings.h"\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "cafebld - Win32 Release"


"$(INTDIR)\cafedlg.obj" : $(SOURCE) $(DEP_CPP_CAFED) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_CAFED) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"


"$(INTDIR)\cafedlg.obj" : $(SOURCE) $(DEP_CPP_CAFED) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.pdb" : $(SOURCE) $(DEP_CPP_CAFED) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_CAFED) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\settings.cpp
DEP_CPP_SETTI=\
	".\settings.h"\
	".\CAFEBLD.H"\
	".\resource.h"\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "cafebld - Win32 Release"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"


"$(INTDIR)\settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.pdb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\cafebld.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "cafebld - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/cafebld.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/cafebld.pdb" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cafebld.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cafebld - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/cafebld.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/cafebld.pdb" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cafebld.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cafebld.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cafebld.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
