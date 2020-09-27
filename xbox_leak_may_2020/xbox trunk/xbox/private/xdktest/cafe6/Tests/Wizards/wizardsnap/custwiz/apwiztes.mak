# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=apwiztes - Win32 Release
!MESSAGE No configuration specified.  Defaulting to apwiztes - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "apwiztes - Win32 Release" && "$(CFG)" !=\
 "apwiztes - Win32 Pseudo-Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "apwiztes.mak" CFG="apwiztes - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apwiztes - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "apwiztes - Win32 Pseudo-Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "apwiztes - Win32 Pseudo-Debug"
F90=fl32.exe
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "apwiztes - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP BASE Target_Ext awx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
# PROP Target_Ext awx
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\apwiztes.awx" "..\..\..\IDE\Template\apwiztes.awx"

CLEAN : 
	-@erase ".\Release\apwiztes.idb"
	-@erase ".\Release\apwiztes.pch"
	-@erase ".\Release\apwiztes.lib"
	-@erase ".\Release\cstm1dlg.obj"
	-@erase ".\Release\chooser.obj"
	-@erase ".\Release\apwizaw.obj"
	-@erase ".\Release\apwiztes.obj"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\apwiztes.res"
	-@erase ".\Release\apwiztes.exp"
	-@erase "..\..\..\IDE\Template\apwiztes.awx"
	-@erase ".\Release\apwiztes.awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /Ox /I "Release/" /c /nologo /MD
# ADD F90 /Ox /I "Release/" /c /nologo /MD
F90_OBJS=.\Release/
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/apwiztes.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/apwiztes.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/apwiztes.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/apwiztes.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/apwiztes.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/apwiztes.awx"\
 /implib:"$(OUTDIR)/apwiztes.lib" 
LINK32_OBJS= \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/apwizaw.obj" \
	"$(INTDIR)/apwiztes.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/apwiztes.res"

"$(OUTDIR)\apwiztes.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=Release
TargetPath=.\Release\apwiztes.awx
TargetName=apwiztes
InputPath=.\Release\apwiztes.awx
SOURCE=$(InputPath)

".\$(MSDevDir)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   if not exist $(MSDevDir)\Template\nul md $(MSDevDir)\Template
   copy "$(TargetPath)" "$(MSDevDir)\Template"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
    "$(MSDevDir)\Template"

# End Custom Build

!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP BASE Target_Ext awx
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
# PROP Target_Ext awx
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\apwiztes.awx" "..\..\..\IDE\Template\apwiztes.awx"

CLEAN : 
	-@erase ".\Debug\apwiztes.pdb"
	-@erase ".\Debug\apwiztes.pch"
	-@erase ".\Debug\cstm1dlg.obj"
	-@erase ".\Debug\chooser.obj"
	-@erase ".\Debug\apwizaw.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\apwiztes.obj"
	-@erase ".\Debug\apwiztes.res"
	-@erase ".\Debug\apwiztes.idb"
	-@erase ".\Debug\apwiztes.ilk"
	-@erase ".\Debug\apwiztes.lib"
	-@erase ".\Debug\apwiztes.exp"
	-@erase "..\..\..\IDE\Template\apwiztes.awx"
	-@erase ".\Debug\apwiztes.awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /Ox /I "Debug/" /c /nologo /MD
# ADD F90 /Ox /I "Debug/" /c /nologo /MD
F90_OBJS=.\Debug/
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/apwiztes.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/apwiztes.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/apwiztes.res" /d "NDEBUG" /d "_PSEUDO_DEBUG" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/apwiztes.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/apwiztes.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:"$(OUTDIR)/apwiztes.awx" /implib:"$(OUTDIR)/apwiztes.lib" 
LINK32_OBJS= \
	"$(INTDIR)/cstm1dlg.obj" \
	"$(INTDIR)/chooser.obj" \
	"$(INTDIR)/apwizaw.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/apwiztes.obj" \
	"$(INTDIR)/apwiztes.res"

"$(OUTDIR)\apwiztes.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=Debug
TargetPath=.\Debug\apwiztes.awx
TargetName=apwiztes
InputPath=.\Debug\apwiztes.awx
SOURCE=$(InputPath)

".\$(MSDevDir)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   if not exist $(MSDevDir)\Template\nul md $(MSDevDir)\Template
   copy "$(TargetPath)" "$(MSDevDir)\Template"
   if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb"\
    "$(MSDevDir)\Template"

# End Custom Build

!ENDIF 

F90_PROJ=/Ox /I "Debug/" /c /nologo /MD /Fo"Debug/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

MTL_PROJ=/nologo /D "NDEBUG" /win32 

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

# Name "apwiztes - Win32 Release"
# Name "apwiztes - Win32 Pseudo-Debug"

!IF  "$(CFG)" == "apwiztes - Win32 Release"

!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\apwiztes.cpp
DEP_CPP_APWIZ=\
	".\apwizaw.h"\
	".\apwiztes.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "apwiztes - Win32 Release"


"$(INTDIR)\apwiztes.obj" : $(SOURCE) $(DEP_CPP_APWIZ) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_APWIZ) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"


"$(INTDIR)\apwiztes.obj" : $(SOURCE) $(DEP_CPP_APWIZ) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.pdb" : $(SOURCE) $(DEP_CPP_APWIZ) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_APWIZ) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "apwiztes - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/apwiztes.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/apwiztes.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\apwiztes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_PSEUDO_DEBUG" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/apwiztes.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/apwiztes.pdb" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\apwiztes.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\apwiztes.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apwiztes.rc

"$(INTDIR)\apwiztes.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "apwiztes - Win32 Release"

!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chooser.cpp
DEP_CPP_CHOOS=\
	".\apwizaw.h"\
	".\apwiztes.h"\
	".\chooser.h"\
	".\cstm1dlg.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "apwiztes - Win32 Release"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"


"$(INTDIR)\chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.pdb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cstm1dlg.cpp
DEP_CPP_CSTM1=\
	".\apwizaw.h"\
	".\apwiztes.h"\
	".\cstm1dlg.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "apwiztes - Win32 Release"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"


"$(INTDIR)\cstm1dlg.obj" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.pdb" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_CSTM1) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apwizaw.cpp
DEP_CPP_APWIZA=\
	".\apwizaw.h"\
	".\apwiztes.h"\
	".\chooser.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "apwiztes - Win32 Release"


"$(INTDIR)\apwizaw.obj" : $(SOURCE) $(DEP_CPP_APWIZA) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_APWIZA) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ELSEIF  "$(CFG)" == "apwiztes - Win32 Pseudo-Debug"


"$(INTDIR)\apwizaw.obj" : $(SOURCE) $(DEP_CPP_APWIZA) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.pdb" : $(SOURCE) $(DEP_CPP_APWIZA) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"

"$(INTDIR)\apwiztes.idb" : $(SOURCE) $(DEP_CPP_APWIZA) "$(INTDIR)"\
 "$(INTDIR)\apwiztes.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
