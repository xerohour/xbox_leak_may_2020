# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (MIPS) Application" 0x0501

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug" &&\
 "$(CFG)" != "Win32 (MIPS) Debug" && "$(CFG)" != "Win32 (MIPS) Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE
!MESSAGE NMAKE /f "multipad.mak" CFG="Win32 (80x86) Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Application")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Application")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (MIPS) Debug"

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

ALL : $(OUTDIR)/multipad.exe $(OUTDIR)/multipad.bsc

$(OUTDIR) :
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Bt  /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"multipad.pch" /Fo$(INTDIR)/ /Bt  /c
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"multipad.bsc"
BSC32_SBRS= \
	$(INTDIR)/multipad.sbr \
	$(INTDIR)/mpopen.sbr \
	$(INTDIR)/mpfind.sbr \
	$(INTDIR)/mpfile.sbr \
	$(INTDIR)/mpprint.sbr \
	$(INTDIR)/mpinit.sbr

$(OUTDIR)/multipad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"multipad.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /OUT:$(OUTDIR)/"multipad.exe"
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/multipad.obj \
	$(INTDIR)/mpopen.obj \
	$(INTDIR)/mpfind.obj \
	$(INTDIR)/mpfile.obj \
	$(INTDIR)/mpprint.obj \
	$(INTDIR)/mpinit.obj \
	$(INTDIR)/multipad.res

$(OUTDIR)/multipad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/multipad.exe $(OUTDIR)/multipad.bsc

$(OUTDIR) :
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Bt  /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"multipad.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"multipad.pdb" /Bt  /c
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"multipad.bsc"
BSC32_SBRS= \
	$(INTDIR)/multipad.sbr \
	$(INTDIR)/mpopen.sbr \
	$(INTDIR)/mpfind.sbr \
	$(INTDIR)/mpfile.sbr \
	$(INTDIR)/mpprint.sbr \
	$(INTDIR)/mpinit.sbr

$(OUTDIR)/multipad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"multipad.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:$(OUTDIR)/"multipad.exe"
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/multipad.obj \
	$(INTDIR)/mpopen.obj \
	$(INTDIR)/mpfind.obj \
	$(INTDIR)/mpfile.obj \
	$(INTDIR)/mpprint.obj \
	$(INTDIR)/mpinit.obj \
	$(INTDIR)/multipad.res

$(OUTDIR)/multipad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/multipad.exe $(OUTDIR)/multipad.bsc

$(OUTDIR) :
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D\
 "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"multipad.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"multipad.pdb" /c
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"multipad.bsc"
BSC32_SBRS= \
	$(INTDIR)/multipad.sbr \
	$(INTDIR)/mpopen.sbr \
	$(INTDIR)/mpfind.sbr \
	$(INTDIR)/mpfile.sbr \
	$(INTDIR)/mpprint.sbr \
	$(INTDIR)/mpinit.sbr

$(OUTDIR)/multipad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"multipad.pdb" /DEBUG\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"multipad.exe"
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/multipad.obj \
	$(INTDIR)/mpopen.obj \
	$(INTDIR)/mpfind.obj \
	$(INTDIR)/mpfile.obj \
	$(INTDIR)/mpprint.obj \
	$(INTDIR)/mpinit.obj \
	$(INTDIR)/multipad.res

$(OUTDIR)/multipad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/multipad.exe $(OUTDIR)/multipad.bsc

$(OUTDIR) :
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /c
CPP_PROJ=/nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX /O2 /D "NDEBUG" /D "_AFXDLL" /D\
 "_WINDOWS" /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"multipad.pch"\
 /Fo$(INTDIR)/ /c
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"multipad.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"multipad.bsc"
BSC32_SBRS= \
	$(INTDIR)/multipad.sbr \
	$(INTDIR)/mpopen.sbr \
	$(INTDIR)/mpfind.sbr \
	$(INTDIR)/mpfile.sbr \
	$(INTDIR)/mpprint.sbr \
	$(INTDIR)/mpinit.sbr

$(OUTDIR)/multipad.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:MIPS
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib /NOLOGO /SUBSYSTEM:windows /PDB:$(OUTDIR)/"multipad.pdb"\
 /MACHINE:MIPS /OUT:$(OUTDIR)/"multipad.exe"
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/multipad.obj \
	$(INTDIR)/mpopen.obj \
	$(INTDIR)/mpfind.obj \
	$(INTDIR)/mpfile.obj \
	$(INTDIR)/mpprint.obj \
	$(INTDIR)/mpinit.obj \
	$(INTDIR)/multipad.res

$(OUTDIR)/multipad.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\multipad.c
DEP_MULTI=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/multipad.obj :  $(SOURCE)  $(DEP_MULTI) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpopen.c
DEP_MPOPE=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mpopen.obj :  $(SOURCE)  $(DEP_MPOPE) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpfind.c
DEP_MPFIN=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mpfind.obj :  $(SOURCE)  $(DEP_MPFIN) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpfile.c
DEP_MPFIL=\
	.\multipad.h\

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mpfile.obj :  $(SOURCE)  $(DEP_MPFIL) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpprint.c
DEP_MPPRI=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mpprint.obj :  $(SOURCE)  $(DEP_MPPRI) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpinit.c
DEP_MPINI=\
	.\multipad.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/mpinit.obj :  $(SOURCE)  $(DEP_MPINI) $(INTDIR)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multipad.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/multipad.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE)

!ENDIF

# End Source File
# End Group
# End Project
################################################################################
