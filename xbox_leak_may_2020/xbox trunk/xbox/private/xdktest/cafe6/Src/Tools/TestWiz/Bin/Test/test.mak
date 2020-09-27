# Microsoft Visual C++ Generated NMAKE File, Format Version 30003
# MSVCPRJD: version 3.00.4339
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=TEST - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to TEST - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TEST - Win32 Release" && "$(CFG)" != "TEST - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "TEST.mak" CFG="TEST - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TEST - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TEST - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "TEST - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "TEST - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/TEST.exe $(OUTDIR)/TEST.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"TEST.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /c 
CPP_OBJS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"test.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"TEST.bsc" 
BSC32_SBRS= \
	$(INTDIR)/testcase.sbr \
	$(INTDIR)/testtype.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/TEST.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:$(OUTDIR)/"TEST.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:$(OUTDIR)/"TEST.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/testcase.obj \
	$(INTDIR)/testtype.obj \
	$(INTDIR)/test.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/TEST.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TEST - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/TEST.exe $(OUTDIR)/TEST.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"TEST.pch" /Yu"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"TEST.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"test.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"TEST.bsc" 
BSC32_SBRS= \
	$(INTDIR)/testcase.sbr \
	$(INTDIR)/testtype.sbr \
	$(INTDIR)/stdafx.sbr

$(OUTDIR)/TEST.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:$(OUTDIR)/"TEST.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:$(OUTDIR)/"TEST.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/testcase.obj \
	$(INTDIR)/testtype.obj \
	$(INTDIR)/test.res \
	$(INTDIR)/stdafx.obj

$(OUTDIR)/TEST.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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
# Begin Target "TEST - Win32 Release"

# Name "TEST - Win32 Release"
# Name "TEST - Win32 Debug"
################################################################################
# Begin Source File

SOURCE=.\testcase.cpp
DEP_TESTC=\
	.\stdafx.h\
	.\testcase.h\
	\v3slm\cafe\cafe\wb\wb.h\
	\v3slm\cafe\cafe\src\cafe.h\
	\v3slm\cafe\cafe\wb\wbdrv.h\
	\v3slm\cafe\cafe\wb\wbdebug.h\
	\v3slm\cafe\cafe\wb\regdlg.h\
	\v3slm\cafe\cafe\wb\envdlg.h\
	\v3slm\cafe\cafe\wb\wbutil.h\
	\v3slm\cafe\cafe\wb\filetabl.h\
	\v3slm\cafe\cafe\wb\registry.h\
	\v3slm\cafe\cafe\wb\sym\menuids.h\
	\v3slm\cafe\cafe\wb\sym\dockids.h\
	\v3slm\cafe\cafe\wb\sym\qcqp.h\
	\v3slm\cafe\cafe\wb\wb_props.h\
	\v3slm\cafe\cafe\wb\sym\shrdres.h\
	\v3slm\cafe\cafe\wb\sym\projprop.h\
	\v3slm\cafe\cafe\wb\sym\slob.h\
	\v3slm\cafe\cafe\wb\uwbframe.h\
	\v3slm\cafe\cafe\wb\ueditor.h\
	\v3slm\cafe\cafe\wb\uresedit.h\
	\v3slm\cafe\cafe\wb\urestabl.h\
	\v3slm\cafe\cafe\wb\uresbrws.h\
	\v3slm\cafe\cafe\wb\ustredit.h\
	\v3slm\cafe\cafe\wb\uaccedit.h\
	\v3slm\cafe\cafe\wb\uveredit.h\
	\v3slm\cafe\cafe\wb\udlgedit.h\
	\v3slm\cafe\cafe\wb\uimgedit.h\
	\v3slm\cafe\cafe\wb\umenedit.h\
	\v3slm\cafe\cafe\wb\usymdlg.h\
	\v3slm\cafe\cafe\wb\ucommdlg.h\
	\v3slm\cafe\cafe\wb\upropwnd.h\
	\v3slm\cafe\cafe\wb\uprjprop.h\
	\v3slm\cafe\cafe\wb\uprjfile.h\
	\v3slm\cafe\cafe\wb\udockwnd.h\
	\v3slm\cafe\cafe\wb\ufindinf.h\
	\v3slm\cafe\cafe\wb\ufindrep.h\
	\v3slm\cafe\cafe\wb\utbardlg.h\
	\v3slm\cafe\cafe\wb\utoolbar.h\
	\v3slm\cafe\cafe\wb\uitarget.h\
	\v3slm\cafe\cafe\wb\uitabdlg.h\
	\v3slm\cafe\cafe\wb\uioptdlg.h\
	\v3slm\cafe\cafe\wb\uprjtdlg.h\
	\v3slm\cafe\cafe\wb\ucustdlg.h\
	\v3slm\cafe\cafe\wb\uprojwiz.h\
	\v3slm\cafe\cafe\wb\uiextwiz.h\
	\v3slm\cafe\cafe\wb\uappwiz.h\
	\v3slm\cafe\cafe\wb\ucwzdlg.h\
	\v3slm\cafe\cafe\wb\parse.h\
	\v3slm\cafe\cafe\wb\uiqw.h\
	\v3slm\cafe\cafe\wb\uioutput.h\
	\v3slm\cafe\cafe\wb\uibp.h\
	\v3slm\cafe\cafe\wb\uimem.h\
	\v3slm\cafe\cafe\wb\uieewnd.h\
	\v3slm\cafe\cafe\wb\uiwatch.h\
	\v3slm\cafe\cafe\wb\uivar.h\
	\v3slm\cafe\cafe\wb\uilocals.h\
	\v3slm\cafe\cafe\wb\uireg.h\
	\v3slm\cafe\cafe\wb\uicp.h\
	\v3slm\cafe\cafe\wb\uidam.h\
	\v3slm\cafe\cafe\wb\uiexcpt.h\
	\v3slm\cafe\cafe\wb\uistack.h\
	\v3slm\cafe\cafe\wb\uithread.h\
	\v3slm\cafe\cafe\wb\uibrowse.h\
	\v3slm\cafe\cafe\wb\uiwbmsg.h\
	\v3slm\cafe\cafe\wb\uiwbdlg.h\
	\v3slm\cafe\cafe\wb\uiconnec.h\
	\v3slm\cafe\cafe\wb\uiconset.h\
	\v3slm\cafe\cafe\wb\odbcdlgs.h\
	\v3slm\cafe\cafe\wb\cofile.h\
	\v3slm\cafe\cafe\wb\coee.h\
	\v3slm\cafe\cafe\wb\cobp.h\
	\v3slm\cafe\cafe\wb\comem.h\
	\v3slm\cafe\cafe\wb\codebug.h\
	\v3slm\cafe\cafe\wb\cocp.h\
	\v3slm\cafe\cafe\wb\coreg.h\
	\v3slm\cafe\cafe\wb\costack.h\
	\v3slm\cafe\cafe\wb\cothread.h\
	\v3slm\cafe\cafe\wb\coexcpt.h\
	\v3slm\cafe\cafe\wb\codam.h\
	\v3slm\cafe\cafe\wb\cosource.h\
	\v3slm\cafe\cafe\wb\cowrkspc.h\
	\v3slm\cafe\cafe\wb\coprject.h\
	\v3slm\cafe\cafe\wb\coresscr.h\
	\v3slm\cafe\cafe\wb\cobrowse.h\
	\v3slm\cafe\cafe\wb\coconnec.h\
	\v3slm\cafe\cafe\src\mstwrap.h\
	\v3slm\cafe\cafe\src\imewrap.h\
	\v3slm\cafe\cafe\src\except.h\
	\v3slm\cafe\cafe\src\log.h\
	\v3slm\cafe\cafe\src\mainfrm.h\
	\v3slm\cafe\cafe\src\portdoc.h\
	\v3slm\cafe\cafe\src\portview.h\
	\v3slm\cafe\cafe\src\finddlg.h\
	\v3slm\cafe\cafe\src\testset.h\
	\v3slm\cafe\cafe\src\scripts.h\
	\v3slm\cafe\cafe\src\testdrv.h\
	\v3slm\cafe\cafe\src\testutil.h\
	\v3slm\cafe\cafe\src\perfdata.h\
	\v3slm\cafe\cafe\src\debug.h\
	\v3slm\cafe\cafe\src\hotkey.h\
	\v3slm\cafe\cafe\src\uwindow.h\
	\v3slm\cafe\cafe\src\udialog.h\
	\v3slm\cafe\cafe\src\umsgbox.h\
	\v3slm\cafe\cafe\src\umainfrm.h\
	\v3slm\cafe\cafe\src\coclip.h\
	\v3slm\cafe\cafe\wb\sym\props.h\
	\v3slm\ide\include\undo.h\
	\v3slm\ide\include\slobwnd.h\
	\v3slm\cafe\cafe\inc\t3ctrl.h\
	\v3slm\cafe\cafe\inc\t3scrn.h\
	\v3slm\cafe\cafe\inc\ime32.h\
	\v3slm\ide\include\slob.h

$(INTDIR)/testcase.obj :  $(SOURCE)  $(DEP_TESTC) $(INTDIR)\
 .\WinDebug\stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testtype.cpp
DEP_TESTT=\
	.\stdafx.h\
	.\testtype.h\
	.\testcase.h\
	\v3slm\cafe\cafe\wb\wb.h\
	\v3slm\cafe\cafe\src\cafe.h\
	\v3slm\cafe\cafe\wb\wbdrv.h\
	\v3slm\cafe\cafe\wb\wbdebug.h\
	\v3slm\cafe\cafe\wb\regdlg.h\
	\v3slm\cafe\cafe\wb\envdlg.h\
	\v3slm\cafe\cafe\wb\wbutil.h\
	\v3slm\cafe\cafe\wb\filetabl.h\
	\v3slm\cafe\cafe\wb\registry.h\
	\v3slm\cafe\cafe\wb\sym\menuids.h\
	\v3slm\cafe\cafe\wb\sym\dockids.h\
	\v3slm\cafe\cafe\wb\sym\qcqp.h\
	\v3slm\cafe\cafe\wb\wb_props.h\
	\v3slm\cafe\cafe\wb\sym\shrdres.h\
	\v3slm\cafe\cafe\wb\sym\projprop.h\
	\v3slm\cafe\cafe\wb\sym\slob.h\
	\v3slm\cafe\cafe\wb\uwbframe.h\
	\v3slm\cafe\cafe\wb\ueditor.h\
	\v3slm\cafe\cafe\wb\uresedit.h\
	\v3slm\cafe\cafe\wb\urestabl.h\
	\v3slm\cafe\cafe\wb\uresbrws.h\
	\v3slm\cafe\cafe\wb\ustredit.h\
	\v3slm\cafe\cafe\wb\uaccedit.h\
	\v3slm\cafe\cafe\wb\uveredit.h\
	\v3slm\cafe\cafe\wb\udlgedit.h\
	\v3slm\cafe\cafe\wb\uimgedit.h\
	\v3slm\cafe\cafe\wb\umenedit.h\
	\v3slm\cafe\cafe\wb\usymdlg.h\
	\v3slm\cafe\cafe\wb\ucommdlg.h\
	\v3slm\cafe\cafe\wb\upropwnd.h\
	\v3slm\cafe\cafe\wb\uprjprop.h\
	\v3slm\cafe\cafe\wb\uprjfile.h\
	\v3slm\cafe\cafe\wb\udockwnd.h\
	\v3slm\cafe\cafe\wb\ufindinf.h\
	\v3slm\cafe\cafe\wb\ufindrep.h\
	\v3slm\cafe\cafe\wb\utbardlg.h\
	\v3slm\cafe\cafe\wb\utoolbar.h\
	\v3slm\cafe\cafe\wb\uitarget.h\
	\v3slm\cafe\cafe\wb\uitabdlg.h\
	\v3slm\cafe\cafe\wb\uioptdlg.h\
	\v3slm\cafe\cafe\wb\uprjtdlg.h\
	\v3slm\cafe\cafe\wb\ucustdlg.h\
	\v3slm\cafe\cafe\wb\uprojwiz.h\
	\v3slm\cafe\cafe\wb\uiextwiz.h\
	\v3slm\cafe\cafe\wb\uappwiz.h\
	\v3slm\cafe\cafe\wb\ucwzdlg.h\
	\v3slm\cafe\cafe\wb\parse.h\
	\v3slm\cafe\cafe\wb\uiqw.h\
	\v3slm\cafe\cafe\wb\uioutput.h\
	\v3slm\cafe\cafe\wb\uibp.h\
	\v3slm\cafe\cafe\wb\uimem.h\
	\v3slm\cafe\cafe\wb\uieewnd.h\
	\v3slm\cafe\cafe\wb\uiwatch.h\
	\v3slm\cafe\cafe\wb\uivar.h\
	\v3slm\cafe\cafe\wb\uilocals.h\
	\v3slm\cafe\cafe\wb\uireg.h\
	\v3slm\cafe\cafe\wb\uicp.h\
	\v3slm\cafe\cafe\wb\uidam.h\
	\v3slm\cafe\cafe\wb\uiexcpt.h\
	\v3slm\cafe\cafe\wb\uistack.h\
	\v3slm\cafe\cafe\wb\uithread.h\
	\v3slm\cafe\cafe\wb\uibrowse.h\
	\v3slm\cafe\cafe\wb\uiwbmsg.h\
	\v3slm\cafe\cafe\wb\uiwbdlg.h\
	\v3slm\cafe\cafe\wb\uiconnec.h\
	\v3slm\cafe\cafe\wb\uiconset.h\
	\v3slm\cafe\cafe\wb\odbcdlgs.h\
	\v3slm\cafe\cafe\wb\cofile.h\
	\v3slm\cafe\cafe\wb\coee.h\
	\v3slm\cafe\cafe\wb\cobp.h\
	\v3slm\cafe\cafe\wb\comem.h\
	\v3slm\cafe\cafe\wb\codebug.h\
	\v3slm\cafe\cafe\wb\cocp.h\
	\v3slm\cafe\cafe\wb\coreg.h\
	\v3slm\cafe\cafe\wb\costack.h\
	\v3slm\cafe\cafe\wb\cothread.h\
	\v3slm\cafe\cafe\wb\coexcpt.h\
	\v3slm\cafe\cafe\wb\codam.h\
	\v3slm\cafe\cafe\wb\cosource.h\
	\v3slm\cafe\cafe\wb\cowrkspc.h\
	\v3slm\cafe\cafe\wb\coprject.h\
	\v3slm\cafe\cafe\wb\coresscr.h\
	\v3slm\cafe\cafe\wb\cobrowse.h\
	\v3slm\cafe\cafe\wb\coconnec.h\
	\v3slm\cafe\cafe\src\mstwrap.h\
	\v3slm\cafe\cafe\src\imewrap.h\
	\v3slm\cafe\cafe\src\except.h\
	\v3slm\cafe\cafe\src\log.h\
	\v3slm\cafe\cafe\src\mainfrm.h\
	\v3slm\cafe\cafe\src\portdoc.h\
	\v3slm\cafe\cafe\src\portview.h\
	\v3slm\cafe\cafe\src\finddlg.h\
	\v3slm\cafe\cafe\src\testset.h\
	\v3slm\cafe\cafe\src\scripts.h\
	\v3slm\cafe\cafe\src\testdrv.h\
	\v3slm\cafe\cafe\src\testutil.h\
	\v3slm\cafe\cafe\src\perfdata.h\
	\v3slm\cafe\cafe\src\debug.h\
	\v3slm\cafe\cafe\src\hotkey.h\
	\v3slm\cafe\cafe\src\uwindow.h\
	\v3slm\cafe\cafe\src\udialog.h\
	\v3slm\cafe\cafe\src\umsgbox.h\
	\v3slm\cafe\cafe\src\umainfrm.h\
	\v3slm\cafe\cafe\src\coclip.h\
	\v3slm\cafe\cafe\wb\sym\props.h\
	\v3slm\ide\include\undo.h\
	\v3slm\ide\include\slobwnd.h\
	\v3slm\cafe\cafe\inc\t3ctrl.h\
	\v3slm\cafe\cafe\inc\t3scrn.h\
	\v3slm\cafe\cafe\inc\ime32.h\
	\v3slm\ide\include\slob.h

$(INTDIR)/testtype.obj :  $(SOURCE)  $(DEP_TESTT) $(INTDIR)\
 .\WinDebug\stdafx.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testcase.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\testtype.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\test.rc
DEP_TEST_=\
	\v3slm\cafe\cafe\wb\wb.rc\
	\v3slm\cafe\cafe\src\cafe.rc\
	\v3slm\cafe\cafe\src\RES\MAIN.ICO\
	\v3slm\cafe\cafe\src\RES\CATS.ICO\
	\v3slm\cafe\cafe\src\RES\AFX.ICO\
	\v3slm\cafe\cafe\src\RES\CAFE.ICO\
	\v3slm\cafe\cafe\src\RES\SNOUT.ICO\
	\v3slm\cafe\cafe\src\RES\PIG2.ICO\
	\v3slm\cafe\cafe\src\RES\UTENSIL.ICO\
	\v3slm\cafe\cafe\src\RES\UTENSILX.ICO\
	\v3slm\cafe\cafe\src\RES\UTENSIL3.ICO\
	\v3slm\cafe\cafe\src\RES\CHEESE.ICO\
	\v3slm\cafe\cafe\src\RES\PORTDOC.ICO\
	\v3slm\cafe\cafe\src\RES\TOOLBAR.BMP\
	\v3slm\cafe\cafe\src\res\cafe.rc2

$(INTDIR)/test.res :  $(SOURCE)  $(DEP_TEST_) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "TEST - Win32 Debug"

DEP_STDAF=\
	.\stdafx.h\
	\v3slm\cafe\cafe\wb\wb.h\
	\v3slm\cafe\cafe\src\cafe.h\
	\v3slm\cafe\cafe\wb\wbdrv.h\
	\v3slm\cafe\cafe\wb\wbdebug.h\
	\v3slm\cafe\cafe\wb\regdlg.h\
	\v3slm\cafe\cafe\wb\envdlg.h\
	\v3slm\cafe\cafe\wb\wbutil.h\
	\v3slm\cafe\cafe\wb\filetabl.h\
	\v3slm\cafe\cafe\wb\registry.h\
	\v3slm\cafe\cafe\wb\sym\menuids.h\
	\v3slm\cafe\cafe\wb\sym\dockids.h\
	\v3slm\cafe\cafe\wb\sym\qcqp.h\
	\v3slm\cafe\cafe\wb\wb_props.h\
	\v3slm\cafe\cafe\wb\sym\shrdres.h\
	\v3slm\cafe\cafe\wb\sym\projprop.h\
	\v3slm\cafe\cafe\wb\sym\slob.h\
	\v3slm\cafe\cafe\wb\uwbframe.h\
	\v3slm\cafe\cafe\wb\ueditor.h\
	\v3slm\cafe\cafe\wb\uresedit.h\
	\v3slm\cafe\cafe\wb\urestabl.h\
	\v3slm\cafe\cafe\wb\uresbrws.h\
	\v3slm\cafe\cafe\wb\ustredit.h\
	\v3slm\cafe\cafe\wb\uaccedit.h\
	\v3slm\cafe\cafe\wb\uveredit.h\
	\v3slm\cafe\cafe\wb\udlgedit.h\
	\v3slm\cafe\cafe\wb\uimgedit.h\
	\v3slm\cafe\cafe\wb\umenedit.h\
	\v3slm\cafe\cafe\wb\usymdlg.h\
	\v3slm\cafe\cafe\wb\ucommdlg.h\
	\v3slm\cafe\cafe\wb\upropwnd.h\
	\v3slm\cafe\cafe\wb\uprjprop.h\
	\v3slm\cafe\cafe\wb\uprjfile.h\
	\v3slm\cafe\cafe\wb\udockwnd.h\
	\v3slm\cafe\cafe\wb\ufindinf.h\
	\v3slm\cafe\cafe\wb\ufindrep.h\
	\v3slm\cafe\cafe\wb\utbardlg.h\
	\v3slm\cafe\cafe\wb\utoolbar.h\
	\v3slm\cafe\cafe\wb\uitarget.h\
	\v3slm\cafe\cafe\wb\uitabdlg.h\
	\v3slm\cafe\cafe\wb\uioptdlg.h\
	\v3slm\cafe\cafe\wb\uprjtdlg.h\
	\v3slm\cafe\cafe\wb\ucustdlg.h\
	\v3slm\cafe\cafe\wb\uprojwiz.h\
	\v3slm\cafe\cafe\wb\uiextwiz.h\
	\v3slm\cafe\cafe\wb\uappwiz.h\
	\v3slm\cafe\cafe\wb\ucwzdlg.h\
	\v3slm\cafe\cafe\wb\parse.h\
	\v3slm\cafe\cafe\wb\uiqw.h\
	\v3slm\cafe\cafe\wb\uioutput.h\
	\v3slm\cafe\cafe\wb\uibp.h\
	\v3slm\cafe\cafe\wb\uimem.h\
	\v3slm\cafe\cafe\wb\uieewnd.h\
	\v3slm\cafe\cafe\wb\uiwatch.h\
	\v3slm\cafe\cafe\wb\uivar.h\
	\v3slm\cafe\cafe\wb\uilocals.h\
	\v3slm\cafe\cafe\wb\uireg.h\
	\v3slm\cafe\cafe\wb\uicp.h\
	\v3slm\cafe\cafe\wb\uidam.h\
	\v3slm\cafe\cafe\wb\uiexcpt.h\
	\v3slm\cafe\cafe\wb\uistack.h\
	\v3slm\cafe\cafe\wb\uithread.h\
	\v3slm\cafe\cafe\wb\uibrowse.h\
	\v3slm\cafe\cafe\wb\uiwbmsg.h\
	\v3slm\cafe\cafe\wb\uiwbdlg.h\
	\v3slm\cafe\cafe\wb\uiconnec.h\
	\v3slm\cafe\cafe\wb\uiconset.h\
	\v3slm\cafe\cafe\wb\odbcdlgs.h\
	\v3slm\cafe\cafe\wb\cofile.h\
	\v3slm\cafe\cafe\wb\coee.h\
	\v3slm\cafe\cafe\wb\cobp.h\
	\v3slm\cafe\cafe\wb\comem.h\
	\v3slm\cafe\cafe\wb\codebug.h\
	\v3slm\cafe\cafe\wb\cocp.h\
	\v3slm\cafe\cafe\wb\coreg.h\
	\v3slm\cafe\cafe\wb\costack.h\
	\v3slm\cafe\cafe\wb\cothread.h\
	\v3slm\cafe\cafe\wb\coexcpt.h\
	\v3slm\cafe\cafe\wb\codam.h\
	\v3slm\cafe\cafe\wb\cosource.h\
	\v3slm\cafe\cafe\wb\cowrkspc.h\
	\v3slm\cafe\cafe\wb\coprject.h\
	\v3slm\cafe\cafe\wb\coresscr.h\
	\v3slm\cafe\cafe\wb\cobrowse.h\
	\v3slm\cafe\cafe\wb\coconnec.h\
	\v3slm\cafe\cafe\src\mstwrap.h\
	\v3slm\cafe\cafe\src\imewrap.h\
	\v3slm\cafe\cafe\src\except.h\
	\v3slm\cafe\cafe\src\log.h\
	\v3slm\cafe\cafe\src\mainfrm.h\
	\v3slm\cafe\cafe\src\portdoc.h\
	\v3slm\cafe\cafe\src\portview.h\
	\v3slm\cafe\cafe\src\finddlg.h\
	\v3slm\cafe\cafe\src\testset.h\
	\v3slm\cafe\cafe\src\scripts.h\
	\v3slm\cafe\cafe\src\testdrv.h\
	\v3slm\cafe\cafe\src\testutil.h\
	\v3slm\cafe\cafe\src\perfdata.h\
	\v3slm\cafe\cafe\src\debug.h\
	\v3slm\cafe\cafe\src\hotkey.h\
	\v3slm\cafe\cafe\src\uwindow.h\
	\v3slm\cafe\cafe\src\udialog.h\
	\v3slm\cafe\cafe\src\umsgbox.h\
	\v3slm\cafe\cafe\src\umainfrm.h\
	\v3slm\cafe\cafe\src\coclip.h\
	\v3slm\cafe\cafe\wb\sym\props.h\
	\v3slm\ide\include\undo.h\
	\v3slm\ide\include\slobwnd.h\
	\v3slm\cafe\cafe\inc\t3ctrl.h\
	\v3slm\cafe\cafe\inc\t3scrn.h\
	\v3slm\cafe\cafe\inc\ime32.h\
	\v3slm\ide\include\slob.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"TEST.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"TEST.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TEST - Win32 Release"

DEP_STDAF=\
	.\stdafx.h\
	\v3slm\cafe\cafe\wb\wb.h\
	\v3slm\cafe\cafe\src\cafe.h\
	\v3slm\cafe\cafe\wb\wbdrv.h\
	\v3slm\cafe\cafe\wb\wbdebug.h\
	\v3slm\cafe\cafe\wb\regdlg.h\
	\v3slm\cafe\cafe\wb\envdlg.h\
	\v3slm\cafe\cafe\wb\wbutil.h\
	\v3slm\cafe\cafe\wb\filetabl.h\
	\v3slm\cafe\cafe\wb\registry.h\
	\v3slm\cafe\cafe\wb\sym\menuids.h\
	\v3slm\cafe\cafe\wb\sym\dockids.h\
	\v3slm\cafe\cafe\wb\sym\qcqp.h\
	\v3slm\cafe\cafe\wb\wb_props.h\
	\v3slm\cafe\cafe\wb\sym\shrdres.h\
	\v3slm\cafe\cafe\wb\sym\projprop.h\
	\v3slm\cafe\cafe\wb\sym\slob.h\
	\v3slm\cafe\cafe\wb\uwbframe.h\
	\v3slm\cafe\cafe\wb\ueditor.h\
	\v3slm\cafe\cafe\wb\uresedit.h\
	\v3slm\cafe\cafe\wb\urestabl.h\
	\v3slm\cafe\cafe\wb\uresbrws.h\
	\v3slm\cafe\cafe\wb\ustredit.h\
	\v3slm\cafe\cafe\wb\uaccedit.h\
	\v3slm\cafe\cafe\wb\uveredit.h\
	\v3slm\cafe\cafe\wb\udlgedit.h\
	\v3slm\cafe\cafe\wb\uimgedit.h\
	\v3slm\cafe\cafe\wb\umenedit.h\
	\v3slm\cafe\cafe\wb\usymdlg.h\
	\v3slm\cafe\cafe\wb\ucommdlg.h\
	\v3slm\cafe\cafe\wb\upropwnd.h\
	\v3slm\cafe\cafe\wb\uprjprop.h\
	\v3slm\cafe\cafe\wb\uprjfile.h\
	\v3slm\cafe\cafe\wb\udockwnd.h\
	\v3slm\cafe\cafe\wb\ufindinf.h\
	\v3slm\cafe\cafe\wb\ufindrep.h\
	\v3slm\cafe\cafe\wb\utbardlg.h\
	\v3slm\cafe\cafe\wb\utoolbar.h\
	\v3slm\cafe\cafe\wb\uitarget.h\
	\v3slm\cafe\cafe\wb\uitabdlg.h\
	\v3slm\cafe\cafe\wb\uioptdlg.h\
	\v3slm\cafe\cafe\wb\uprjtdlg.h\
	\v3slm\cafe\cafe\wb\ucustdlg.h\
	\v3slm\cafe\cafe\wb\uprojwiz.h\
	\v3slm\cafe\cafe\wb\uiextwiz.h\
	\v3slm\cafe\cafe\wb\uappwiz.h\
	\v3slm\cafe\cafe\wb\ucwzdlg.h\
	\v3slm\cafe\cafe\wb\parse.h\
	\v3slm\cafe\cafe\wb\uiqw.h\
	\v3slm\cafe\cafe\wb\uioutput.h\
	\v3slm\cafe\cafe\wb\uibp.h\
	\v3slm\cafe\cafe\wb\uimem.h\
	\v3slm\cafe\cafe\wb\uieewnd.h\
	\v3slm\cafe\cafe\wb\uiwatch.h\
	\v3slm\cafe\cafe\wb\uivar.h\
	\v3slm\cafe\cafe\wb\uilocals.h\
	\v3slm\cafe\cafe\wb\uireg.h\
	\v3slm\cafe\cafe\wb\uicp.h\
	\v3slm\cafe\cafe\wb\uidam.h\
	\v3slm\cafe\cafe\wb\uiexcpt.h\
	\v3slm\cafe\cafe\wb\uistack.h\
	\v3slm\cafe\cafe\wb\uithread.h\
	\v3slm\cafe\cafe\wb\uibrowse.h\
	\v3slm\cafe\cafe\wb\uiwbmsg.h\
	\v3slm\cafe\cafe\wb\uiwbdlg.h\
	\v3slm\cafe\cafe\wb\uiconnec.h\
	\v3slm\cafe\cafe\wb\uiconset.h\
	\v3slm\cafe\cafe\wb\odbcdlgs.h\
	\v3slm\cafe\cafe\wb\cofile.h\
	\v3slm\cafe\cafe\wb\coee.h\
	\v3slm\cafe\cafe\wb\cobp.h\
	\v3slm\cafe\cafe\wb\comem.h\
	\v3slm\cafe\cafe\wb\codebug.h\
	\v3slm\cafe\cafe\wb\cocp.h\
	\v3slm\cafe\cafe\wb\coreg.h\
	\v3slm\cafe\cafe\wb\costack.h\
	\v3slm\cafe\cafe\wb\cothread.h\
	\v3slm\cafe\cafe\wb\coexcpt.h\
	\v3slm\cafe\cafe\wb\codam.h\
	\v3slm\cafe\cafe\wb\cosource.h\
	\v3slm\cafe\cafe\wb\cowrkspc.h\
	\v3slm\cafe\cafe\wb\coprject.h\
	\v3slm\cafe\cafe\wb\coresscr.h\
	\v3slm\cafe\cafe\wb\cobrowse.h\
	\v3slm\cafe\cafe\wb\coconnec.h\
	\v3slm\cafe\cafe\src\mstwrap.h\
	\v3slm\cafe\cafe\src\imewrap.h\
	\v3slm\cafe\cafe\src\except.h\
	\v3slm\cafe\cafe\src\log.h\
	\v3slm\cafe\cafe\src\mainfrm.h\
	\v3slm\cafe\cafe\src\portdoc.h\
	\v3slm\cafe\cafe\src\portview.h\
	\v3slm\cafe\cafe\src\finddlg.h\
	\v3slm\cafe\cafe\src\testset.h\
	\v3slm\cafe\cafe\src\scripts.h\
	\v3slm\cafe\cafe\src\testdrv.h\
	\v3slm\cafe\cafe\src\testutil.h\
	\v3slm\cafe\cafe\src\perfdata.h\
	\v3slm\cafe\cafe\src\debug.h\
	\v3slm\cafe\cafe\src\hotkey.h\
	\v3slm\cafe\cafe\src\uwindow.h\
	\v3slm\cafe\cafe\src\udialog.h\
	\v3slm\cafe\cafe\src\umsgbox.h\
	\v3slm\cafe\cafe\src\umainfrm.h\
	\v3slm\cafe\cafe\src\coclip.h\
	\v3slm\cafe\cafe\wb\sym\props.h\
	\v3slm\ide\include\undo.h\
	\v3slm\ide\include\slobwnd.h\
	\v3slm\cafe\cafe\inc\t3ctrl.h\
	\v3slm\cafe\cafe\inc\t3scrn.h\
	\v3slm\cafe\cafe\inc\ime32.h\
	\v3slm\ide\include\slob.h
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR$(INTDIR)/"" /Fp$(OUTDIR)/"TEST.pch" /Yc"stdafx.h"\
 /Fo$(INTDIR)/"" /Fd$(OUTDIR)/"TEST.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TEST.mak
# End Source File
# End Target
# End Project
################################################################################
