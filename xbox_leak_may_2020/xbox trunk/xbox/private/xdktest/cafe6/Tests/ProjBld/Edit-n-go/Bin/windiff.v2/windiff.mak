# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Release" && "$(CFG)" != "Win32 (MIPS) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "windiff.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Release"

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\windiff.exe .\WinRel\windiff.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /Bt /c
CPP_PROJ=/nologo /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"windiff.pch" /Fo$(INTDIR)/ /Bt /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"windiff.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"windiff.bsc" 
BSC32_SBRS= \
	.\WinRel\file.sbr \
	.\WinRel\scandir.sbr \
	.\WinRel\section.sbr \
	.\WinRel\windiff.sbr \
	.\WinRel\complist.sbr \
	.\WinRel\line.sbr \
	.\WinRel\view.sbr \
	.\WinRel\bar.sbr \
	.\WinRel\compitem.sbr

.\WinRel\windiff.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"windiff.pdb" /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:$(OUTDIR)/"windiff.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\file.obj \
	.\gutils.lib \
	.\WinRel\scandir.obj \
	.\WinRel\section.obj \
	.\WinRel\windiff.obj \
	.\WinRel\complist.obj \
	.\WinRel\line.obj \
	.\WinRel\view.obj \
	.\WinRel\bar.obj \
	.\WinRel\compitem.obj \
	.\WinRel\windiff.res

.\WinRel\windiff.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\windiff.exe .\WinDebug\windiff.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX"windows.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /W3 /GX /Zi /YX"windows.h" /Od /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /Bt /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX"windows.h" /Od /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"windiff.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"windiff.pdb" /Bt /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"windiff.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"windiff.bsc" 
BSC32_SBRS= \
	.\WinDebug\file.sbr \
	.\WinDebug\scandir.sbr \
	.\WinDebug\section.sbr \
	.\WinDebug\windiff.sbr \
	.\WinDebug\complist.sbr \
	.\WinDebug\line.sbr \
	.\WinDebug\view.sbr \
	.\WinDebug\bar.sbr \
	.\WinDebug\compitem.sbr

.\WinDebug\windiff.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"windiff.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:$(OUTDIR)/"windiff.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\file.obj \
	.\gutils.lib \
	.\WinDebug\scandir.obj \
	.\WinDebug\section.obj \
	.\WinDebug\windiff.obj \
	.\WinDebug\complist.obj \
	.\WinDebug\line.obj \
	.\WinDebug\view.obj \
	.\WinDebug\bar.obj \
	.\WinDebug\compitem.obj \
	.\WinDebug\windiff.res

.\WinDebug\windiff.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32__M"
# PROP BASE Intermediate_Dir "Win32__M"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\windiff.exe .\WinRel\windiff.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Bt /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"windiff.pch"\
 /Fo$(INTDIR)/ /Bt /c 
CPP_OBJS=.\WinRel/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"windiff.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"windiff.bsc" 
BSC32_SBRS= \
	.\WinRel\file.sbr \
	.\WinRel\scandir.sbr \
	.\WinRel\section.sbr \
	.\WinRel\windiff.sbr \
	.\WinRel\complist.sbr \
	.\WinRel\line.sbr \
	.\WinRel\view.sbr \
	.\WinRel\bar.sbr \
	.\WinRel\compitem.sbr

.\WinRel\windiff.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"windiff.pdb" /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"windiff.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\file.obj \
	.\gutils.lib \
	.\WinRel\scandir.obj \
	.\WinRel\section.obj \
	.\WinRel\windiff.obj \
	.\WinRel\complist.obj \
	.\WinRel\line.obj \
	.\WinRel\view.obj \
	.\WinRel\bar.obj \
	.\WinRel\compitem.obj \
	.\WinRel\windiff.res

.\WinRel\windiff.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32__0"
# PROP BASE Intermediate_Dir "Win32__0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\windiff.exe .\WinDebug\windiff.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Bt /c
CPP_PROJ=/nologo /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"windiff.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"windiff.pdb" /Bt /c 
CPP_OBJS=.\WinDebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"windiff.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"windiff.bsc" 
BSC32_SBRS= \
	.\WinDebug\file.sbr \
	.\WinDebug\scandir.sbr \
	.\WinDebug\section.sbr \
	.\WinDebug\windiff.sbr \
	.\WinDebug\complist.sbr \
	.\WinDebug\line.sbr \
	.\WinDebug\view.sbr \
	.\WinDebug\bar.sbr \
	.\WinDebug\compitem.sbr

.\WinDebug\windiff.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /PDB:$(OUTDIR)/"windiff.pdb" /DEBUG /MACHINE:MIPS\
 /OUT:$(OUTDIR)/"windiff.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\file.obj \
	.\gutils.lib \
	.\WinDebug\scandir.obj \
	.\WinDebug\section.obj \
	.\WinDebug\windiff.obj \
	.\WinDebug\complist.obj \
	.\WinDebug\line.obj \
	.\WinDebug\view.obj \
	.\WinDebug\bar.obj \
	.\WinDebug\compitem.obj \
	.\WinDebug\windiff.res

.\WinDebug\windiff.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\file.c
DEP_FILE_=\
	.\gutils.h\
	.\windiff.h\
	.\list.h\
	.\line.h\
	.\scandir.h\
	.\file.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\file.obj :  $(SOURCE)  $(DEP_FILE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gutils.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\scandir.c
DEP_SCAND=\
	.\gutils.h\
	.\list.h\
	.\scandir.h\
	.\windiff.h\
	.\wdiffrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\scandir.obj :  $(SOURCE)  $(DEP_SCAND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\scandir.obj :  $(SOURCE)  $(DEP_SCAND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\scandir.obj :  $(SOURCE)  $(DEP_SCAND) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\scandir.obj :  $(SOURCE)  $(DEP_SCAND) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\section.c
DEP_SECTI=\
	.\gutils.h\
	.\tree.h\
	.\state.h\
	.\windiff.h\
	.\wdiffrc.h\
	.\list.h\
	.\line.h\
	.\section.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\section.obj :  $(SOURCE)  $(DEP_SECTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\section.obj :  $(SOURCE)  $(DEP_SECTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\section.obj :  $(SOURCE)  $(DEP_SECTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\section.obj :  $(SOURCE)  $(DEP_SECTI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windiff.c
DEP_WINDI=\
	.\gutils.h\
	.\table.h\
	.\list.h\
	.\scandir.h\
	.\file.h\
	.\compitem.h\
	.\complist.h\
	.\view.h\
	.\state.h\
	.\windiff.h\
	.\wdiffrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\windiff.obj :  $(SOURCE)  $(DEP_WINDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\windiff.obj :  $(SOURCE)  $(DEP_WINDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\windiff.obj :  $(SOURCE)  $(DEP_WINDI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\windiff.obj :  $(SOURCE)  $(DEP_WINDI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\complist.c
DEP_COMPL=\
	.\gutils.h\
	.\state.h\
	.\windiff.h\
	.\wdiffrc.h\
	.\list.h\
	.\line.h\
	.\scandir.h\
	.\file.h\
	.\section.h\
	.\compitem.h\
	.\complist.h\
	.\view.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\complist.obj :  $(SOURCE)  $(DEP_COMPL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\complist.obj :  $(SOURCE)  $(DEP_COMPL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\complist.obj :  $(SOURCE)  $(DEP_COMPL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\complist.obj :  $(SOURCE)  $(DEP_COMPL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\line.c
DEP_LINE_=\
	.\gutils.h\
	.\windiff.h\
	.\list.h\
	.\line.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\line.obj :  $(SOURCE)  $(DEP_LINE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.c
DEP_VIEW_=\
	.\gutils.h\
	.\table.h\
	.\state.h\
	.\windiff.h\
	.\wdiffrc.h\
	.\list.h\
	.\line.h\
	.\scandir.h\
	.\file.h\
	.\section.h\
	.\compitem.h\
	.\complist.h\
	.\view.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\view.obj :  $(SOURCE)  $(DEP_VIEW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\view.obj :  $(SOURCE)  $(DEP_VIEW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\view.obj :  $(SOURCE)  $(DEP_VIEW_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\view.obj :  $(SOURCE)  $(DEP_VIEW_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bar.c
DEP_BAR_C=\
	.\gutils.h\
	.\table.h\
	.\state.h\
	.\wdiffrc.h\
	.\windiff.h\
	.\list.h\
	.\line.h\
	.\scandir.h\
	.\file.h\
	.\section.h\
	.\compitem.h\
	.\complist.h\
	.\view.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\bar.obj :  $(SOURCE)  $(DEP_BAR_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\bar.obj :  $(SOURCE)  $(DEP_BAR_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\bar.obj :  $(SOURCE)  $(DEP_BAR_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\bar.obj :  $(SOURCE)  $(DEP_BAR_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\compitem.c
DEP_COMPI=\
	.\gutils.h\
	.\state.h\
	.\windiff.h\
	.\wdiffrc.h\
	.\list.h\
	.\line.h\
	.\scandir.h\
	.\file.h\
	.\section.h\
	.\compitem.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\compitem.obj :  $(SOURCE)  $(DEP_COMPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\compitem.obj :  $(SOURCE)  $(DEP_COMPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\compitem.obj :  $(SOURCE)  $(DEP_COMPI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\compitem.obj :  $(SOURCE)  $(DEP_COMPI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windiff.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\windiff.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\windiff.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

.\WinRel\windiff.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

.\WinDebug\windiff.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
