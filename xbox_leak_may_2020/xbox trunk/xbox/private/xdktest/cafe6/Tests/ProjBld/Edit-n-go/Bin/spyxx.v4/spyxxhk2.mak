# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spyxxhk2.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/spyhk30.dll $(OUTDIR)/spyhk30.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "SPYXXHK_DLL" /D "_X86_" /FR /c
# SUBTRACT CPP /Yu
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "SPYXXHK_DLL" /D "_X86_" /FR$(INTDIR)/ /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"WinRel./spyhk30.res" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"WinRel./spyhk30.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WinRel./spyhk30.bsc"
BSC32_FLAGS=/nologo /o"WinRel./spyhk30.bsc" 
BSC32_SBRS= \
	$(INTDIR)/spyxxhk.sbr

$(OUTDIR)/spyhk30.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /PDB:"WinRel/spyhk30.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /OUT:"WinRel/spyhk30.dll" /IMPLIB:"WinRel/spyhk30.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /NOLOGO /SUBSYSTEM:windows /DLL\
 /INCREMENTAL:no /PDB:"WinRel/spyhk30.pdb" /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:"WinRel/spyhk30.dll" /IMPLIB:"WinRel/spyhk30.lib" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/spyxxhk.obj \
	$(INTDIR)/spyhk30.res

$(OUTDIR)/spyhk30.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/spyxxhk30.dll $(OUTDIR)/spyhk30.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "SPYXXHK_DLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR"" /Fd"WinDebug/spyxxhk30.pdb" /Bt  /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "SPYXXHK_DLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR /Fo$(INTDIR)/\
 /Fd"WinDebug/spyxxhk30.pdb" /Bt  /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"WinDebug/spyhk30.res" /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"WinDebug/spyhk30.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WinDebug/spyhk30.bsc"
BSC32_FLAGS=/nologo /o"WinDebug/spyhk30.bsc" 
BSC32_SBRS= \
	.\spyxxhk.sbr

$(OUTDIR)/spyhk30.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:"WinDebug/spyxxhk30.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE) /OUT:"WinDebug/spyxxhk30.dll" /IMPLIB:"WinDebug/spyxxhk30.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /NOLOGO /SUBSYSTEM:windows /DLL\
 /INCREMENTAL:no /PDB:"WinDebug/spyxxhk30.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:"WinDebug/spyxxhk30.dll" /IMPLIB:"WinDebug/spyxxhk30.lib" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/spyxxhk.obj \
	$(INTDIR)/spyhk30.res

$(OUTDIR)/spyxxhk30.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\spyxxhk.c
DEP_SPYXX=\
	.\spyxxhk.h\
	.\met.h

$(INTDIR)/spyxxhk.obj :  $(SOURCE)  $(DEP_SPYXX) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxhk.rc
DEP_SPYXXH=\
	.\reshk.h

$(INTDIR)/spyhk30.res :  $(SOURCE)  $(DEP_SPYXXH) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
