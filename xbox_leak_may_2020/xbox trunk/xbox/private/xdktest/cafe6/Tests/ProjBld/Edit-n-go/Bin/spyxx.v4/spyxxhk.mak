# Microsoft Visual C++ Generated NMAKE File, Format Version 30003
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spyxxhk.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "Win32 (80x86) Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)/SPYHK30.DLL" "$(OUTDIR)/SPYHK30.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
F90_OBJS=.\WinRel/
F90_PROJ=/I "WinDebug/" /Fo"WinDebug/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /Gi- /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "SPYXXHK_DLL" /D "_X86_" /FR /Yu"met.h" /c
CPP_PROJ=/nologo /MT /W3 /Gi- /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "SPYXXHK_DLL" /D "_X86_" /FR"$(INTDIR)/" /Fp"$(INTDIR)/spyxxhk.pch" /Yu"met.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spyxxhk.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WinRel/SPYHK30.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SPYHK30.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/spyxxhk.sbr"

"$(OUTDIR)/SPYHK30.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /base:0x20000000 /entry:"DLLEntryPoint" /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinRel/SPYHK30.DLL" /SECTION:.shdata,rws
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /nologo /base:0x20000000\
 /entry:"DLLEntryPoint" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/SPYHK30.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/SPYHK30.DLL"\
 /implib:"$(OUTDIR)/SPYHK30.lib" /SECTION:.shdata,rws 
DEF_FILE=
LINK32_OBJS= \
	"$(INTDIR)/spyxxhk.obj" \
	"$(INTDIR)/spyxxhk.res"

"$(OUTDIR)/SPYHK30.DLL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)/SPYHK30.DLL" "$(OUTDIR)/SPYHK30.map" "$(OUTDIR)/SPYHK30.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "WinDebug/"
# ADD F90 /I "WinDebug/"
F90_OBJS=.\WinDebug/
F90_PROJ=/I "WinDebug/" /Fo"WinDebug/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /FR /c
# ADD CPP /nologo /MT /W3 /Gi- /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "SPYXXHK_DLL" /D "_X86_" /FR /Yu"met.h" /Fd"WinDebug/SPYHK30.pdb" /Bt /c
CPP_PROJ=/nologo /MT /W3 /Gi- /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "SPYXXHK_DLL" /D "_X86_" /FR"$(INTDIR)/" /Fp"$(INTDIR)/spyxxhk.pch"\
 /Yu"met.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/SPYHK30.pdb" /Bt /c 
CPP_OBJS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips
# ADD MTL /nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips
MTL_PROJ=/nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spyxxhk.res" /d "_DEBUG" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WinDebug/SPYHK30.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SPYHK30.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/spyxxhk.sbr"

"$(OUTDIR)/SPYHK30.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib kernel32.lib user32.lib gdi32.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x20000000 /entry:"DLLEntryPoint" /subsystem:windows /dll /incremental:no /map /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinDebug/SPYHK30.DLL" /SECTION:.shdata,rws /SECTION:.shdata,rws
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib kernel32.lib user32.lib\
 gdi32.lib kernel32.lib user32.lib gdi32.lib /nologo /base:0x20000000\
 /entry:"DLLEntryPoint" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/SPYHK30.pdb" /map:"$(INTDIR)/SPYHK30.map" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:"$(OUTDIR)/SPYHK30.DLL" /implib:"$(OUTDIR)/SPYHK30.lib"\
 /SECTION:.shdata,rws /SECTION:.shdata,rws 
DEF_FILE=
LINK32_OBJS= \
	"$(INTDIR)/spyxxhk.obj" \
	"$(INTDIR)/spyxxhk.res"

"$(OUTDIR)/SPYHK30.map" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Win32 (80x86) Debug"
# Name "Win32 (80x86) Release"

!IF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\spyxxhk.c

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYXX=\
	".\spyxxhk.h"\
	".\met.h"\
	
# ADD CPP /Yc"met.h"

"$(INTDIR)/spyxxhk.obj" : $(SOURCE) $(DEP_CPP_SPYXX) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gi- /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "SPYXXHK_DLL" /D "_X86_" /FR"$(INTDIR)/" /Fp"$(INTDIR)/spyxxhk.pch" /Yc"met.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYXX=\
	".\spyxxhk.h"\
	".\met.h"\
	
# ADD CPP /Yc"met.h"

"$(INTDIR)/spyxxhk.obj" : $(SOURCE) $(DEP_CPP_SPYXX) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gi- /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "SPYXXHK_DLL" /D "_X86_" /FR"$(INTDIR)/" /Fp"$(INTDIR)/spyxxhk.pch"\
 /Yc"met.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/SPYHK30.pdb" /Bt /c $(SOURCE)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxhk.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

"$(INTDIR)/spyxxhk.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

"$(INTDIR)/spyxxhk.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
