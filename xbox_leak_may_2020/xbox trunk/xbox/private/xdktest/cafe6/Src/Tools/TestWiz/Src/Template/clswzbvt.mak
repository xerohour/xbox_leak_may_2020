# Microsoft Visual C++ Generated NMAKE File, Format Version 20054
# MSVCPRJ: version 2.00.4210
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "$$root$$.MAK" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (80x86) Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel


ALL : .\WinDebug\$$root$$.exe .\WinDebug\$$root$$.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "ODBC_OLE" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "ODBC_OLE" /D "_WINDOWS" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"$$root$$.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x1 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"$$root$$.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# SUBTRACT BSC32 /nologo /Iu
BSC32_FLAGS=/o$(OUTDIR)/"$$root$$.bsc" 
BSC32_SBRS= \
	.\WinRel\STDAFX.SBR \
	.\WinRel\$$cases_cpp$$.SBR \
	.\WinRel\$$type_cpp$$.SBR

.\WinRel\$$root$$.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcd30.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib nafxcw.lib ctl3d32.lib cafe.lib wbdrv.lib t3bas32.lib t3run32.lib t3scrn32.lib t3ctrl32.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib nafxcw.lib ctl3d32.lib cafe.lib wbdrv.lib t3bas32.lib t3run32.lib\
 t3scrn32.lib t3ctrl32.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"$$root$$.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /OUT:$(OUTDIR)/"$$root$$.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\STDAFX.OBJ \
	.\WinRel\$$cases_cpp$$.obj \
	.\WinRel\$$type_cpp$$.obj \
	.\WinRel\$$root$$.res

.\WinRel\$$root$$.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\$$root$$.exe .\WinDebug\$$root$$.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "ODBC_OLE" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "_MBCS"\
 /D "ODBC_OLE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"$$root$$.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"$$root$$.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x1 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"$$root$$.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo				
# SUBTRACT BASE BSC32 /Iu				
# SUBTRACT BSC32 /nologo /Iu			
BSC32_FLAGS=/o$(OUTDIR)/"$$root$$.bsc" 
BSC32_SBRS= \
	.\WinDebug\STDAFX.SBR \		  	
	.\WinDebug\$$cases_cpp$$.SBR \	
	.\WinDebug\$$type_cpp$$.SBR		

.\WinDebug\$$root$$.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcd30d.lib compob32.lib storag32.lib ole2w32.lib ole232.lib ole2di32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib cafed.lib wbdrvd.lib t3bas32.lib t3run32.lib t3scrn32.lib t3ctrl32.lib nafxcwd.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib cafed.lib wbdrvd.lib \
 mstest40.lib nafxcwd.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"$$root$$.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:$(OUTDIR)/"$$root$$.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\STDAFX.OBJ \
	.\WinDebug\$$cases_cpp$$.OBJ \
	.\WinDebug\$$type_cpp$$.OBJ \
	.\WinDebug\$$root$$.res

.\WinDebug\$$root$$.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=

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

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\STDAFX.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\$$cases_cpp$$.CPP
DEP_CLWCA=\
	.\STDAFX.H\
	.\$$cases_cpp$$.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\$$cases_cpp$$.OBJ :  $(SOURCE)  $(DEP_CLWCA) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\$$cases_cpp$$.OBJ :  $(SOURCE)  $(DEP_CLWCA) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\$$type_cpp$$.CPP
DEP_CLWSN=\
	.\STDAFX.H\
	.\$$type_cpp$$.H\
	.\$$cases_cpp$$.H

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\$$type_cpp$$.OBJ :  $(SOURCE)  $(DEP_CLWSN) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\$$type_cpp$$.OBJ :  $(SOURCE)  $(DEP_CLWSN) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\$$root$$.RC
DEP_SNIFF=\
	wb.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

.\WinRel\$$root$$.res :  $(SOURCE)  $(DEP_SNIFF) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

.\WinDebug\$$root$$.res :  $(SOURCE)  $(DEP_SNIFF) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
