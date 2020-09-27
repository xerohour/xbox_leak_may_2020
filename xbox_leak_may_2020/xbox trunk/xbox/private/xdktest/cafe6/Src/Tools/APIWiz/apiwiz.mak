# Microsoft Visual C++ Generated NMAKE File, Format Version 30003
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=apiwiz - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to apiwiz - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "apiwiz - Win32 Release" && "$(CFG)" != "apiwiz - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "apiwiz.mak" CFG="apiwiz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apiwiz - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "apiwiz - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "apiwiz - Win32 Debug"
# PROP Source_Control_Project "<not controlled>"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "apiwiz - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/cafebrws.exe $(OUTDIR)/apiwiz.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apiwiz.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /c 
CPP_OBJS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"apiwiz.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"apiwiz.bsc" 
BSC32_SBRS= \
	$(INTDIR)/apiwiz.sbr \
	$(INTDIR)/apiwidlg.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/apipage.sbr \
	$(INTDIR)/custpage.sbr \
	$(INTDIR)/areaslb.sbr \
	$(INTDIR)/data.sbr

$(OUTDIR)/apiwiz.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinRel/cafebrws.exe"
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:$(OUTDIR)/"cafebrws.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:$(OUTDIR)/"cafebrws.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/apiwiz.obj \
	$(INTDIR)/apiwidlg.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/apiwiz.res \
	$(INTDIR)/apipage.obj \
	$(INTDIR)/custpage.obj \
	$(INTDIR)/areaslb.obj \
	$(INTDIR)/data.obj

$(OUTDIR)/cafebrws.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "apiwiz - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/cafebrwsd.exe $(OUTDIR)/apiwiz.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apiwiz.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"apiwiz.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"apiwiz.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"apiwiz.bsc" 
BSC32_SBRS= \
	$(INTDIR)/apiwiz.sbr \
	$(INTDIR)/apiwidlg.sbr \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/apipage.sbr \
	$(INTDIR)/custpage.sbr \
	$(INTDIR)/areaslb.sbr \
	$(INTDIR)/data.sbr

$(OUTDIR)/apiwiz.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinDebug/cafebrwsd.exe"
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:$(OUTDIR)/"cafebrwsd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:$(OUTDIR)/"cafebrwsd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/apiwiz.obj \
	$(INTDIR)/apiwidlg.obj \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/apiwiz.res \
	$(INTDIR)/apipage.obj \
	$(INTDIR)/custpage.obj \
	$(INTDIR)/areaslb.obj \
	$(INTDIR)/data.obj

$(OUTDIR)/cafebrwsd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "apiwiz - Win32 Release"
# Name "apiwiz - Win32 Debug"
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apiwiz.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apiwiz.cpp
DEP_APIWI=\
	.\stdafx.h\
	.\apiwiz.h\
	.\apiwidlg.h

$(INTDIR)/apiwiz.obj :  $(SOURCE)  $(DEP_APIWI) $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apiwidlg.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apiwidlg.cpp
DEP_APIWID=\
	.\stdafx.h\
	.\apiwiz.h\
	.\apiwidlg.h

$(INTDIR)/apiwidlg.obj :  $(SOURCE)  $(DEP_APIWID) $(INTDIR)\
 $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "apiwiz - Win32 Release"

DEP_STDAF=\
	.\stdafx.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apiwiz.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "apiwiz - Win32 Debug"

DEP_STDAF=\
	.\stdafx.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apiwiz.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"apiwiz.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apiwiz.rc
DEP_APIWIZ=\
	.\res\apiwiz.ico\
	.\res\apiwiz.rc2

$(INTDIR)/apiwiz.res :  $(SOURCE)  $(DEP_APIWIZ) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apipage.cpp

$(INTDIR)/apipage.obj :  $(SOURCE)  $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\custpage.cpp

$(INTDIR)/custpage.obj :  $(SOURCE)  $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\data.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\areaslb.cpp

$(INTDIR)/areaslb.obj :  $(SOURCE)  $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\data.cpp

$(INTDIR)/data.obj :  $(SOURCE)  $(INTDIR) $(INTDIR)/stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=\ide\bin\appwiz\apidata.txt
# End Source File
# End Target
# End Project
################################################################################
