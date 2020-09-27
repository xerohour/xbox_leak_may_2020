# Microsoft Visual C++ Generated NMAKE File, Format Version 30003
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "apitext.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
# PROP Source_Control_Project "<not controlled>"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/apitext.exe $(OUTDIR)/apitext.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apitext.pch" /Fo$(INTDIR)/"" /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"apitext.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/apitext.sbr

$(OUTDIR)/apitext.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:console /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:$(OUTDIR)/"apitext.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:$(OUTDIR)/"apitext.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/apitext.obj

$(OUTDIR)/apitext.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/apitextd.exe $(OUTDIR)/apitext.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /FR /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"apitext.pdb"\
 /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"apitext.bsc" 
BSC32_SBRS= \
	$(INTDIR)/stdafx.sbr \
	$(INTDIR)/apitext.sbr

$(OUTDIR)/apitext.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:console /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:".\WinDebug\apitextd.exe"
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:$(OUTDIR)/"apitextd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:$(OUTDIR)/"apitextd.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/stdafx.obj \
	$(INTDIR)/apitext.obj

$(OUTDIR)/apitextd.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Win32 Release"
# Name "Win32 Debug"
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "Win32 Release"

DEP_STDAF=\
	.\stdafx.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apitext.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

DEP_STDAF=\
	.\stdafx.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apitext.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"apitext.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apitext.cpp

!IF  "$(CFG)" == "Win32 Release"

DEP_APITE=\
	\ide\include\stdafx.h\
	.\apitext.h
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/apitext.obj :  $(SOURCE)  $(DEP_APITE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apitext.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

DEP_APITE=\
	\ide\include\stdafx.h\
	.\apitext.h
# ADD CPP /Yu"stdafx.h"

$(INTDIR)/apitext.obj :  $(SOURCE)  $(DEP_APITE) $(INTDIR) $(INTDIR)/stdafx.obj
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"apitext.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"apitext.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apitext.lst
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apitext.ls
# End Source File
################################################################################
# Begin Source File

SOURCE=.\apidata.txt
# End Source File
# End Target
# End Project
################################################################################
