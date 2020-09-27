# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=wizards - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to wizards - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wizards - Win32 Release" && "$(CFG)" !=\
 "wizards - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wizards.mak" CFG="wizards - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wizards - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wizards - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "wizards - Win32 Debug"
F90=fl32.exe
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wizards - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : ".\WinRel\wizsnap.dll" ".\WinRel\wizards.bsc"

CLEAN : 
	-@erase ".\WinRel\wizards.idb"
	-@erase ".\WinRel\wizards.pch"
	-@erase ".\WinRel\wizards.bsc"
	-@erase ".\WinRel\clwsnap.sbr"
	-@erase ".\WinRel\stdafx.sbr"
	-@erase ".\WinRel\appsnap.sbr"
	-@erase ".\WinRel\ctrsnap.sbr"
	-@erase ".\WinRel\sniff.sbr"
	-@erase ".\WinRel\wizsnap.dll"
	-@erase ".\WinRel\sniff.obj"
	-@erase ".\WinRel\clwsnap.obj"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\appsnap.obj"
	-@erase ".\WinRel\ctrsnap.obj"
	-@erase ".\WinRel\wizsnap.res"
	-@erase ".\WinRel\wizsnap.lib"
	-@erase ".\WinRel\wizsnap.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
F90_OBJS=.\WinRel/
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wizards.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wizards.pdb" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wizsnap.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wizards.bsc" 
BSC32_SBRS= \
	".\WinRel\clwsnap.sbr" \
	".\WinRel\stdafx.sbr" \
	".\WinRel\appsnap.sbr" \
	".\WinRel\ctrsnap.sbr" \
	".\WinRel\sniff.sbr"

".\WinRel\wizards.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinRel/wizsnap.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wizsnap.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/wizsnap.dll"\
 /implib:"$(OUTDIR)/wizsnap.lib" 
LINK32_OBJS= \
	".\WinRel\sniff.obj" \
	".\WinRel\clwsnap.obj" \
	".\WinRel\stdafx.obj" \
	".\WinRel\appsnap.obj" \
	".\WinRel\ctrsnap.obj" \
	".\WinRel\wizsnap.res"

".\WinRel\wizsnap.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : ".\WinDebug\wizsnapd.dll" ".\WinDebug\wizards.bsc"

CLEAN : 
	-@erase ".\WinDebug\wizards.pdb"
	-@erase ".\WinDebug\wizards.pch"
	-@erase ".\WinDebug\wizards.idb"
	-@erase ".\WinDebug\wizards.bsc"
	-@erase ".\WinDebug\sniff.sbr"
	-@erase ".\WinDebug\clwsnap.sbr"
	-@erase ".\WinDebug\appsnap.sbr"
	-@erase ".\WinDebug\ctrsnap.sbr"
	-@erase ".\WinDebug\stdafx.sbr"
	-@erase ".\WinDebug\wizsnapd.dll"
	-@erase ".\WinDebug\ctrsnap.obj"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\sniff.obj"
	-@erase ".\WinDebug\clwsnap.obj"
	-@erase ".\WinDebug\appsnap.obj"
	-@erase ".\WinDebug\wizsnap.res"
	-@erase ".\WinDebug\wizsnapd.ilk"
	-@erase ".\WinDebug\wizsnapd.lib"
	-@erase ".\WinDebug\wizsnapd.exp"
	-@erase ".\WinDebug\wizsnapd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinDebug/"
# ADD F90 /I "WinDebug/"
F90_OBJS=.\WinDebug/
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wizards.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wizards.pdb" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wizsnap.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wizards.bsc" 
BSC32_SBRS= \
	".\WinDebug\sniff.sbr" \
	".\WinDebug\clwsnap.sbr" \
	".\WinDebug\appsnap.sbr" \
	".\WinDebug\ctrsnap.sbr" \
	".\WinDebug\stdafx.sbr"

".\WinDebug\wizards.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"WinDebug/wizsnapd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wizsnapd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:"$(OUTDIR)/wizsnapd.dll" /implib:"$(OUTDIR)/wizsnapd.lib" 
LINK32_OBJS= \
	".\WinDebug\ctrsnap.obj" \
	".\WinDebug\stdafx.obj" \
	".\WinDebug\sniff.obj" \
	".\WinDebug\clwsnap.obj" \
	".\WinDebug\appsnap.obj" \
	".\WinDebug\wizsnap.res"

".\WinDebug\wizsnapd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

F90_PROJ=/I "WinRel/" /Fo"WinRel/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

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

# Name "wizards - Win32 Release"
# Name "wizards - Win32 Debug"

!IF  "$(CFG)" == "wizards - Win32 Release"

# PROP Classwizard_Name

!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"

# PROP Classwizard_Name

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\sniff.cpp
DEP_CPP_SNIFF=\
	".\appsnap.h"\
	".\sniff.h"\
	".\clwsnap.h"\
	".\ctrsnap.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\subsuite.h"\
	".\..\..\cafe\v3\include\log.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\settings.h"\
	".\..\..\cafe\v3\include\targxprt.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	".\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\cafe\v3\include\t3scrn.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\ime32.h"\
	".\..\..\cafe\v3\include\shlv1.h"\
	{$(INCLUDE)}"\test.h"\
	".\..\..\cafe\v3\include\idetarg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	".\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\cafe\v3\include\target.h"\
	".\..\..\cafe\v3\include\toolset.h"\
	".\..\..\cafe\v3\include\platform.h"\
	".\..\..\cafe\v3\include\toolxprt.h"\
	

!IF  "$(CFG)" == "wizards - Win32 Release"


".\WinRel\sniff.obj" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\sniff.sbr" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\wizards.idb" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinRel\wizards.pch"


!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"


".\WinDebug\sniff.obj" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\sniff.sbr" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.pdb" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.idb" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	{$(INCLUDE)}"\subsuite.h"\
	".\..\..\cafe\v3\include\log.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\settings.h"\
	".\..\..\cafe\v3\include\targxprt.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	".\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\cafe\v3\include\t3scrn.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\ime32.h"\
	".\..\..\cafe\v3\include\shlv1.h"\
	{$(INCLUDE)}"\test.h"\
	".\..\..\cafe\v3\include\idetarg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	".\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\cafe\v3\include\target.h"\
	".\..\..\cafe\v3\include\toolset.h"\
	".\..\..\cafe\v3\include\platform.h"\
	".\..\..\cafe\v3\include\toolxprt.h"\
	

!IF  "$(CFG)" == "wizards - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wizards.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wizards.pdb" /c $(SOURCE) \
	

".\WinRel\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\wizards.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\wizards.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wizards.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/wizards.pdb" /c $(SOURCE) \
	

".\WinDebug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\wizards.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\wizards.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\wizards.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\appsnap.cpp
DEP_CPP_APPSN=\
	".\appsnap.h"\
	".\sniff.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\subsuite.h"\
	".\..\..\cafe\v3\include\log.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\settings.h"\
	".\..\..\cafe\v3\include\targxprt.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	".\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\cafe\v3\include\t3scrn.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\ime32.h"\
	".\..\..\cafe\v3\include\shlv1.h"\
	{$(INCLUDE)}"\test.h"\
	".\..\..\cafe\v3\include\idetarg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	".\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\cafe\v3\include\target.h"\
	".\..\..\cafe\v3\include\toolset.h"\
	".\..\..\cafe\v3\include\platform.h"\
	".\..\..\cafe\v3\include\toolxprt.h"\
	

!IF  "$(CFG)" == "wizards - Win32 Release"


".\WinRel\appsnap.obj" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\appsnap.sbr" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\wizards.idb" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"


!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"


".\WinDebug\appsnap.obj" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\appsnap.sbr" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.pdb" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.idb" : $(SOURCE) $(DEP_CPP_APPSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\clwsnap.cpp
DEP_CPP_CLWSN=\
	".\clwsnap.h"\
	".\sniff.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\subsuite.h"\
	".\..\..\cafe\v3\include\log.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\settings.h"\
	".\..\..\cafe\v3\include\targxprt.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	".\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\cafe\v3\include\t3scrn.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\ime32.h"\
	".\..\..\cafe\v3\include\shlv1.h"\
	{$(INCLUDE)}"\test.h"\
	".\..\..\cafe\v3\include\idetarg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	".\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\cafe\v3\include\target.h"\
	".\..\..\cafe\v3\include\toolset.h"\
	".\..\..\cafe\v3\include\platform.h"\
	".\..\..\cafe\v3\include\toolxprt.h"\
	

!IF  "$(CFG)" == "wizards - Win32 Release"


".\WinRel\clwsnap.obj" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\clwsnap.sbr" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\wizards.idb" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"


!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"


".\WinDebug\clwsnap.obj" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\clwsnap.sbr" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.pdb" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.idb" : $(SOURCE) $(DEP_CPP_CLWSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wizsnap.rc

!IF  "$(CFG)" == "wizards - Win32 Release"


".\WinRel\wizsnap.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"


".\WinDebug\wizsnap.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ctrsnap.cpp
DEP_CPP_CTRSN=\
	".\ctrsnap.h"\
	".\sniff.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\subsuite.h"\
	".\..\..\cafe\v3\include\log.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\settings.h"\
	".\..\..\cafe\v3\include\targxprt.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	".\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\cafe\v3\include\t3scrn.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\ime32.h"\
	".\..\..\cafe\v3\include\shlv1.h"\
	{$(INCLUDE)}"\test.h"\
	".\..\..\cafe\v3\include\idetarg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	".\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\cafe\v3\include\target.h"\
	".\..\..\cafe\v3\include\toolset.h"\
	".\..\..\cafe\v3\include\platform.h"\
	".\..\..\cafe\v3\include\toolxprt.h"\
	

!IF  "$(CFG)" == "wizards - Win32 Release"


".\WinRel\ctrsnap.obj" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\ctrsnap.sbr" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"

".\WinRel\wizards.idb" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinRel\wizards.pch"


!ELSEIF  "$(CFG)" == "wizards - Win32 Debug"


".\WinDebug\ctrsnap.obj" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\ctrsnap.sbr" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.pdb" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"

".\WinDebug\wizards.idb" : $(SOURCE) $(DEP_CPP_CTRSN) "$(INTDIR)"\
 ".\WinDebug\wizards.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
