# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Dynamic-Link Library" 0x0502
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

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
!MESSAGE NMAKE /f "gutils.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 (MIPS) Release" (based on "Win32 (MIPS) Dynamic-Link Library")
!MESSAGE "Win32 (MIPS) Debug" (based on "Win32 (MIPS) Dynamic-Link Library")
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

ALL : $(OUTDIR)/gutils.dll $(OUTDIR)/gutils.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /MT /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /Bt /c
CPP_PROJ=/nologo /MT /W3 /GX /YX"windows.h" /O2 /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gutils.pch" /Fo$(INTDIR)/ /Bt /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gutils.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gutils.bsc" 
BSC32_SBRS= \
	$(INTDIR)/gmem.sbr \
	$(INTDIR)/tree.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/gdate.sbr \
	$(INTDIR)/tpaint.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/table.sbr \
	$(INTDIR)/tscroll.sbr \
	$(INTDIR)/gbit.sbr \
	$(INTDIR)/utils.sbr \
	$(INTDIR)/gutils.sbr \
	$(INTDIR)/tprint.sbr

$(OUTDIR)/gutils.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30.lib mfco30.lib mfcd30.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /machine:$(PROCESSOR_ARCHITECTURE) /IMPLIB:"gutils.lib"
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"gutils.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /DEF:".\gutils.def"\
 /OUT:$(OUTDIR)/"gutils.dll" /IMPLIB:"gutils.lib" 
DEF_FILE=.\gutils.def
LINK32_OBJS= \
	$(INTDIR)/gmem.obj \
	$(INTDIR)/tree.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/gdate.obj \
	$(INTDIR)/tpaint.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/table.obj \
	$(INTDIR)/tscroll.obj \
	$(INTDIR)/gbit.obj \
	$(INTDIR)/utils.obj \
	$(INTDIR)/gutils.res \
	$(INTDIR)/gutils.obj \
	$(INTDIR)/tprint.obj

$(OUTDIR)/gutils.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gutils.dll $(OUTDIR)/gutils.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX"windows.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX"windows.h" /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /FR /Bt /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX"windows.h" /Od /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /D "WIN32" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gutils.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"gutils.pdb" /Bt /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gutils.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gutils.bsc" 
BSC32_SBRS= \
	$(INTDIR)/gmem.sbr \
	$(INTDIR)/tree.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/gdate.sbr \
	$(INTDIR)/tpaint.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/table.sbr \
	$(INTDIR)/tscroll.sbr \
	$(INTDIR)/gbit.sbr \
	$(INTDIR)/utils.sbr \
	$(INTDIR)/gutils.sbr \
	$(INTDIR)/tprint.sbr

$(OUTDIR)/gutils.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib odbc32.lib mfc30d.lib mfco30d.lib mfcd30d.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /machine:$(PROCESSOR_ARCHITECTURE) /IMPLIB:"gutils.lib"
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"gutils.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE) /DEF:".\gutils.def"\
 /OUT:$(OUTDIR)/"gutils.dll" /IMPLIB:"gutils.lib" 
DEF_FILE=.\gutils.def
LINK32_OBJS= \
	$(INTDIR)/gmem.obj \
	$(INTDIR)/tree.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/gdate.obj \
	$(INTDIR)/tpaint.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/table.obj \
	$(INTDIR)/tscroll.obj \
	$(INTDIR)/gbit.obj \
	$(INTDIR)/utils.obj \
	$(INTDIR)/gutils.res \
	$(INTDIR)/gutils.obj \
	$(INTDIR)/tprint.obj

$(OUTDIR)/gutils.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gutils.dll $(OUTDIR)/gutils.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Bt /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /YX"windows.h" /O2 /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gutils.pch"\
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gutils.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gutils.bsc" 
BSC32_SBRS= \
	$(INTDIR)/gmem.sbr \
	$(INTDIR)/tree.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/gdate.sbr \
	$(INTDIR)/tpaint.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/table.sbr \
	$(INTDIR)/tscroll.sbr \
	$(INTDIR)/gbit.sbr \
	$(INTDIR)/utils.sbr \
	$(INTDIR)/gutils.sbr \
	$(INTDIR)/tprint.sbr

$(OUTDIR)/gutils.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:MIPS /IMPLIB:"gutils.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"gutils.pdb" /MACHINE:MIPS\
 /DEF:".\gutils.def" /OUT:$(OUTDIR)/"gutils.dll" /IMPLIB:"gutils.lib" 
DEF_FILE=.\gutils.def
LINK32_OBJS= \
	$(INTDIR)/gmem.obj \
	$(INTDIR)/tree.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/gdate.obj \
	$(INTDIR)/tpaint.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/table.obj \
	$(INTDIR)/tscroll.obj \
	$(INTDIR)/gbit.obj \
	$(INTDIR)/utils.obj \
	$(INTDIR)/gutils.res \
	$(INTDIR)/gutils.obj \
	$(INTDIR)/tprint.obj

$(OUTDIR)/gutils.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/gutils.dll $(OUTDIR)/gutils.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Bt /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Bt /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /YX"windows.h" /Od /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"gutils.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"gutils.pdb" /Bt /c 
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
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"gutils.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o$(OUTDIR)/"gutils.bsc" 
BSC32_SBRS= \
	$(INTDIR)/gmem.sbr \
	$(INTDIR)/tree.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/gdate.sbr \
	$(INTDIR)/tpaint.sbr \
	$(INTDIR)/status.sbr \
	$(INTDIR)/table.sbr \
	$(INTDIR)/tscroll.sbr \
	$(INTDIR)/gbit.sbr \
	$(INTDIR)/utils.sbr \
	$(INTDIR)/gutils.sbr \
	$(INTDIR)/tprint.sbr

$(OUTDIR)/gutils.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:MIPS /IMPLIB:"gutils.lib"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /DLL /PDB:$(OUTDIR)/"gutils.pdb" /DEBUG /MACHINE:MIPS\
 /DEF:".\gutils.def" /OUT:$(OUTDIR)/"gutils.dll" /IMPLIB:"gutils.lib" 
DEF_FILE=.\gutils.def
LINK32_OBJS= \
	$(INTDIR)/gmem.obj \
	$(INTDIR)/tree.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/gdate.obj \
	$(INTDIR)/tpaint.obj \
	$(INTDIR)/status.obj \
	$(INTDIR)/table.obj \
	$(INTDIR)/tscroll.obj \
	$(INTDIR)/gbit.obj \
	$(INTDIR)/utils.obj \
	$(INTDIR)/gutils.res \
	$(INTDIR)/gutils.obj \
	$(INTDIR)/tprint.obj

$(OUTDIR)/gutils.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\gmem.c
DEP_GMEM_=\
	.\gutils.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gmem.obj :  $(SOURCE)  $(DEP_GMEM_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gmem.obj :  $(SOURCE)  $(DEP_GMEM_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gmem.obj :  $(SOURCE)  $(DEP_GMEM_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gmem.obj :  $(SOURCE)  $(DEP_GMEM_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tree.c
DEP_TREE_=\
	.\gutils.h\
	.\tree.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/tree.obj :  $(SOURCE)  $(DEP_TREE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/tree.obj :  $(SOURCE)  $(DEP_TREE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/tree.obj :  $(SOURCE)  $(DEP_TREE_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/tree.obj :  $(SOURCE)  $(DEP_TREE_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\list.c
DEP_LIST_=\
	.\gutils.h\
	.\list.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gutils.def
# End Source File
################################################################################
# Begin Source File

SOURCE=.\gdate.c
DEP_GDATE=\
	.\gutils.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gdate.obj :  $(SOURCE)  $(DEP_GDATE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gdate.obj :  $(SOURCE)  $(DEP_GDATE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gdate.obj :  $(SOURCE)  $(DEP_GDATE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gdate.obj :  $(SOURCE)  $(DEP_GDATE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tpaint.c
DEP_TPAIN=\
	.\gutils.h\
	.\table.h\
	.\tpriv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/tpaint.obj :  $(SOURCE)  $(DEP_TPAIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/tpaint.obj :  $(SOURCE)  $(DEP_TPAIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/tpaint.obj :  $(SOURCE)  $(DEP_TPAIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/tpaint.obj :  $(SOURCE)  $(DEP_TPAIN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\status.c
DEP_STATU=\
	.\gutils.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/status.obj :  $(SOURCE)  $(DEP_STATU) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\table.c
DEP_TABLE=\
	.\gutils.h\
	.\table.h\
	.\tpriv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/table.obj :  $(SOURCE)  $(DEP_TABLE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/table.obj :  $(SOURCE)  $(DEP_TABLE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/table.obj :  $(SOURCE)  $(DEP_TABLE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/table.obj :  $(SOURCE)  $(DEP_TABLE) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tscroll.c
DEP_TSCRO=\
	.\gutils.h\
	.\table.h\
	.\tpriv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/tscroll.obj :  $(SOURCE)  $(DEP_TSCRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/tscroll.obj :  $(SOURCE)  $(DEP_TSCRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/tscroll.obj :  $(SOURCE)  $(DEP_TSCRO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/tscroll.obj :  $(SOURCE)  $(DEP_TSCRO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gbit.c
DEP_GBIT_=\
	.\gutils.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gbit.obj :  $(SOURCE)  $(DEP_GBIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gbit.obj :  $(SOURCE)  $(DEP_GBIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gbit.obj :  $(SOURCE)  $(DEP_GBIT_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gbit.obj :  $(SOURCE)  $(DEP_GBIT_) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\utils.c
DEP_UTILS=\
	.\gutils.h\
	.\gutilsrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/utils.obj :  $(SOURCE)  $(DEP_UTILS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/utils.obj :  $(SOURCE)  $(DEP_UTILS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/utils.obj :  $(SOURCE)  $(DEP_UTILS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/utils.obj :  $(SOURCE)  $(DEP_UTILS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gutils.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gutils.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gutils.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gutils.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gutils.res :  $(SOURCE)  $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gutils.c
DEP_GUTIL=\
	.\gutils.h\
	.\gutilsrc.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/gutils.obj :  $(SOURCE)  $(DEP_GUTIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/gutils.obj :  $(SOURCE)  $(DEP_GUTIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/gutils.obj :  $(SOURCE)  $(DEP_GUTIL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/gutils.obj :  $(SOURCE)  $(DEP_GUTIL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tprint.c
DEP_TPRIN=\
	.\gutils.h\
	.\gutilsrc.h\
	.\table.h\
	.\tpriv.h

!IF  "$(CFG)" == "Win32 (80x86) Release"

$(INTDIR)/tprint.obj :  $(SOURCE)  $(DEP_TPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

$(INTDIR)/tprint.obj :  $(SOURCE)  $(DEP_TPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Release"

$(INTDIR)/tprint.obj :  $(SOURCE)  $(DEP_TPRIN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (MIPS) Debug"

$(INTDIR)/tprint.obj :  $(SOURCE)  $(DEP_TPRIN) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
