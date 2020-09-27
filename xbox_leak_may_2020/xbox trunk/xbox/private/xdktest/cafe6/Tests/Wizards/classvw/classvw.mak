# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=classvw - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to classvw - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "classvw - Win32 Release" && "$(CFG)" !=\
 "classvw - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "classvw.mak" CFG="classvw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "classvw - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "classvw - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "classvw - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "classvw - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\runtime\classvw.dll"

CLEAN : 
	-@erase ".\runtime\classvw.dll"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\sniff.obj"
	-@erase ".\Release\classvw.pch"
	-@erase ".\Release\classvw.obj"
	-@erase ".\Release\classvw.lib"
	-@erase ".\Release\classvw.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "Release/"
# ADD F90 /I "Release/"
F90_OBJS=.\Release/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/classvw.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"runtime\classvw.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/classvw.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"runtime\classvw.dll"\
 /implib:"$(OUTDIR)/classvw.lib" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/sniff.obj" \
	"$(INTDIR)/classvw.obj"

".\runtime\classvw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "classvw - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\runtime\classvwd.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\classvw.pch"
	-@erase ".\runtime\classvwd.dll"
	-@erase ".\Debug\classvw.obj"
	-@erase ".\Debug\sniff.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\runtime\classvwd.ilk"
	-@erase ".\Debug\classvwd.lib"
	-@erase ".\Debug\classvwd.exp"
	-@erase ".\Debug\classvwd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "Debug/"
# ADD F90 /I "Debug/"
F90_OBJS=.\Debug/
F90_PROJ=/I "Release/" /Fo"Release/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/classvw.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"runtime\classvwd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/classvwd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"runtime\classvwd.dll"\
 /implib:"$(OUTDIR)/classvwd.lib" 
LINK32_OBJS= \
	"$(INTDIR)/classvw.obj" \
	"$(INTDIR)/sniff.obj" \
	"$(INTDIR)/stdafx.obj"

".\runtime\classvwd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "classvw - Win32 Release"
# Name "classvw - Win32 Debug"

!IF  "$(CFG)" == "classvw - Win32 Release"

!ELSEIF  "$(CFG)" == "classvw - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	".\..\..\..\cafe\v3\include\test.h"\
	".\..\..\..\cafe\v3\include\subsuite.h"\
	".\..\..\..\cafe\v3\include\support.h"\
	".\..\..\..\Cafe\V3\include\log.h"\
	".\..\..\..\cafe\v3\include\settings.h"\
	".\..\..\..\cafe\v3\include\target.h"\
	".\..\..\..\cafe\v3\include\idetarg.h"\
	".\..\..\..\cafe\v3\include\toolset.h"\
	".\..\..\..\cafe\v3\include\guiv1.h"\
	".\..\..\..\cafe\v3\include\platform.h"\
	".\..\..\..\cafe\v3\include\testxcpt.h"\
	".\..\..\..\cafe\v3\include\mstwrap.h"\
	".\..\support\guitools\imewrap.h"\
	".\..\support\guitools\testutil.h"\
	".\..\support\guitools\uwindow.h"\
	".\..\support\guitools\udialog.h"\
	".\..\support\guitools\umsgbox.h"\
	".\..\support\guitools\coclip.h"\
	".\..\support\guitools\ide\sym\menuids.h"\
	".\..\support\guitools\ide\sym\dockids.h"\
	".\..\support\guitools\ide\sym\qcqp.h"\
	".\..\support\guitools\ide\sym\shrdres.h"\
	".\..\support\guitools\ide\sym\projprop.h"\
	".\..\support\guitools\ide\sym\slob.h"\
	".\..\support\guitools\ide\filetabl.h"\
	".\..\support\guitools\ide\shl\umainfrm.h"\
	".\..\support\guitools\ide\shl\wb_props.h"\
	".\..\support\guitools\ide\shl\wbutil.h"\
	".\..\support\guitools\ide\shl\uwbframe.h"\
	".\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\support\guitools\ide\shl\upropwnd.h"\
	".\..\support\guitools\ide\shl\udockwnd.h"\
	".\..\support\guitools\ide\shl\utbardlg.h"\
	".\..\support\guitools\ide\shl\utoolbar.h"\
	".\..\support\guitools\ide\shl\uitabdlg.h"\
	".\..\support\guitools\ide\shl\uioptdlg.h"\
	".\..\support\guitools\ide\shl\ucustdlg.h"\
	".\..\support\guitools\ide\shl\uioutput.h"\
	".\..\support\guitools\ide\shl\uiwbmsg.h"\
	".\..\support\guitools\ide\shl\uiwbdlg.h"\
	".\..\support\guitools\ide\shl\uiconnec.h"\
	".\..\support\guitools\ide\shl\uiconset.h"\
	".\..\support\guitools\ide\shl\coconnec.h"\
	".\..\support\guitools\ide\shl\cowrkspc.h"\
	".\..\support\guitools\ide\eds\ueditor.h"\
	".\..\support\guitools\ide\eds\uresedit.h"\
	".\..\support\guitools\ide\eds\urestabl.h"\
	".\..\support\guitools\ide\eds\uresbrws.h"\
	".\..\support\guitools\ide\eds\ustredit.h"\
	".\..\support\guitools\ide\eds\uaccedit.h"\
	".\..\support\guitools\ide\eds\uveredit.h"\
	".\..\support\guitools\ide\eds\udlgedit.h"\
	".\..\support\guitools\ide\eds\uimgedit.h"\
	".\..\support\guitools\ide\eds\umenedit.h"\
	".\..\support\guitools\ide\eds\usymdlg.h"\
	".\..\support\guitools\ide\eds\ufindrep.h"\
	".\..\support\guitools\ide\eds\ufindinf.h"\
	".\..\support\guitools\ide\eds\cofile.h"\
	".\..\support\guitools\ide\eds\cosource.h"\
	".\..\support\guitools\ide\eds\coresscr.h"\
	".\..\support\guitools\ide\prj\uprjfile.h"\
	".\..\support\guitools\ide\prj\uitarget.h"\
	".\..\support\guitools\ide\prj\uogaldlg.h"\
	".\..\support\guitools\ide\prj\uprjtdlg.h"\
	".\..\support\guitools\ide\prj\uprojwiz.h"\
	".\..\support\guitools\ide\prj\uiextwiz.h"\
	".\..\support\guitools\ide\prj\uappwiz.h"\
	".\..\support\guitools\ide\prj\ucwzdlg.h"\
	".\..\support\guitools\ide\prj\utstapwz.h"\
	".\..\support\guitools\ide\prj\odbcdlgs.h"\
	".\..\support\guitools\ide\prj\uictrlwz.h"\
	".\..\support\guitools\ide\prj\coprject.h"\
	".\..\support\guitools\ide\prj\uiwrkspc.h"\
	".\..\support\guitools\ide\dbg\parse.h"\
	".\..\support\guitools\ide\dbg\uiqw.h"\
	".\..\support\guitools\ide\dbg\uibp.h"\
	".\..\support\guitools\ide\dbg\uimem.h"\
	".\..\support\guitools\ide\dbg\uieewnd.h"\
	".\..\support\guitools\ide\dbg\uiwatch.h"\
	".\..\support\guitools\ide\dbg\uivar.h"\
	".\..\support\guitools\ide\dbg\uilocals.h"\
	".\..\support\guitools\ide\dbg\uireg.h"\
	".\..\support\guitools\ide\dbg\uicp.h"\
	".\..\support\guitools\ide\dbg\uidam.h"\
	".\..\support\guitools\ide\dbg\uiexcpt.h"\
	".\..\support\guitools\ide\dbg\uistack.h"\
	".\..\support\guitools\ide\dbg\uithread.h"\
	".\..\support\guitools\ide\dbg\uibrowse.h"\
	".\..\support\guitools\ide\dbg\uidebug.h"\
	".\..\support\guitools\ide\dbg\coee.h"\
	".\..\support\guitools\ide\dbg\cobp.h"\
	".\..\support\guitools\ide\dbg\comem.h"\
	".\..\support\guitools\ide\dbg\codebug.h"\
	".\..\support\guitools\ide\dbg\cocp.h"\
	".\..\support\guitools\ide\dbg\coreg.h"\
	".\..\support\guitools\ide\dbg\costack.h"\
	".\..\support\guitools\ide\dbg\cothread.h"\
	".\..\support\guitools\ide\dbg\coexcpt.h"\
	".\..\support\guitools\ide\dbg\codam.h"\
	".\..\support\guitools\ide\dbg\cobrowse.h"\
	".\..\..\..\Cafe\V3\include\targxprt.h"\
	".\..\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\..\cafe\v3\include\idexprt.h"\
	".\..\support\guitools\ide\connec.h"\
	".\..\..\..\cafe\v3\include\toolxprt.h"\
	".\..\..\..\cafe\v3\include\guixprt.h"\
	".\..\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\..\cafe\v3\include\t3scrn.h"\
	".\..\..\..\cafe\v3\include\ime32.h"\
	".\..\..\..\cafe\v3\include\shlxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	
NODEP_CPP_STDAF=\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\undo.h"\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\slobwnd.h"\
	

!IF  "$(CFG)" == "classvw - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\classvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "classvw - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\classvw.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sniff.cpp

!IF  "$(CFG)" == "classvw - Win32 Release"

DEP_CPP_SNIFF=\
	".\classvw.h"\
	".\sniff.h"\
	".\stdafx.h"\
	".\..\..\..\cafe\v3\include\subsuite.h"\
	".\..\..\..\Cafe\V3\include\log.h"\
	".\..\..\..\cafe\v3\include\support.h"\
	".\..\support\guitools\coclip.h"\
	".\..\..\..\cafe\v3\include\guixprt.h"\
	".\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	".\..\support\guitools\ide\dbg\uibp.h"\
	".\..\support\guitools\udialog.h"\
	".\..\support\guitools\uwindow.h"\
	".\..\support\guitools\ide\shl\wbutil.h"\
	".\..\..\..\cafe\v3\include\shlxprt.h"\
	".\..\support\guitools\ide\shl\wb.h"\
	".\..\support\guitools\ide\dbg\cobrowse.h"\
	".\..\support\guitools\ide\dbg\uibrowse.h"\
	".\..\support\guitools\testutil.h"\
	".\..\support\guitools\ide\dbg\cocp.h"\
	".\..\support\guitools\ide\dbg\uicp.h"\
	".\..\support\guitools\ide\shl\udockwnd.h"\
	".\..\support\guitools\ide\dbg\codam.h"\
	".\..\support\guitools\ide\dbg\codebug.h"\
	".\..\support\guitools\ide\shl\uwbframe.h"\
	".\..\support\guitools\ide\shl\umainfrm.h"\
	".\..\support\guitools\ide\dbg\costack.h"\
	".\..\support\guitools\ide\dbg\uistack.h"\
	".\..\support\guitools\ide\sym\qcqp.h"\
	".\..\support\guitools\ide\sym\dockids.h"\
	".\..\support\guitools\ide\dbg\coee.h"\
	".\..\support\guitools\ide\dbg\uiqw.h"\
	".\..\support\guitools\ide\dbg\coexcpt.h"\
	".\..\support\guitools\ide\dbg\uiexcpt.h"\
	".\..\support\guitools\ide\dbg\comem.h"\
	".\..\support\guitools\ide\dbg\uimem.h"\
	".\..\support\guitools\ide\dbg\coreg.h"\
	".\..\support\guitools\ide\dbg\uireg.h"\
	".\..\support\guitools\ide\dbg\cothread.h"\
	".\..\support\guitools\ide\dbg\uithread.h"\
	".\..\support\guitools\ide\dbg\parse.h"\
	".\..\support\guitools\ide\dbg\uidebug.h"\
	".\..\support\guitools\ide\dbg\uieewnd.h"\
	".\..\support\guitools\ide\sym\cmdids.h"\
	".\..\..\..\cafe\v3\include\guiv1.h"\
	".\..\..\..\cafe\v3\include\settings.h"\
	".\..\..\..\Cafe\V3\include\targxprt.h"\
	".\..\..\..\cafe\v3\include\mstwrap.h"\
	".\..\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\..\cafe\v3\include\t3scrn.h"\
	".\..\..\..\cafe\v3\include\testxcpt.h"\
	".\..\support\guitools\ide\dbg\uivar.h"\
	".\..\support\guitools\ide\dbg\uiwatch.h"\
	".\..\support\guitools\ide\eds\cofile.h"\
	".\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	".\..\support\guitools\ide\eds\ueditor.h"\
	".\..\support\guitools\ide\eds\coresscr.h"\
	".\..\support\guitools\ide\eds\uresedit.h"\
	".\..\support\guitools\ide\eds\cosource.h"\
	".\..\support\guitools\ide\shl\uioptdlg.h"\
	".\..\support\guitools\ide\shl\uitabdlg.h"\
	".\..\support\guitools\ide\eds\uaccedit.h"\
	".\..\support\guitools\ide\eds\urestabl.h"\
	".\..\support\guitools\ide\eds\udlgedit.h"\
	".\..\support\guitools\ide\eds\ufindinf.h"\
	".\..\support\guitools\ide\eds\ufindrep.h"\
	".\..\support\guitools\ide\eds\uimgedit.h"\
	".\..\support\guitools\ide\eds\umenedit.h"\
	".\..\support\guitools\ide\eds\uresbrws.h"\
	".\..\support\guitools\ide\eds\ustredit.h"\
	".\..\support\guitools\ide\eds\usymdlg.h"\
	".\..\support\guitools\ide\eds\uveredit.h"\
	".\..\support\guitools\ide\filetabl.h"\
	".\..\..\..\cafe\v3\include\idexprt.h"\
	".\..\support\guitools\ide\prj\coprject.h"\
	".\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	".\..\support\guitools\ide\prj\uiwrkspc.h"\
	".\..\support\guitools\ide\prj\uprjtdlg.h"\
	".\..\support\guitools\ide\sym\optnrc.h"\
	".\..\support\guitools\ide\sym\vproj.h"\
	".\..\support\guitools\ide\prj\uprojwiz.h"\
	".\..\support\guitools\ide\prj\odbcdlgs.h"\
	".\..\support\guitools\ide\prj\ucwzdlg.h"\
	".\..\support\guitools\ide\prj\uictrlwz.h"\
	".\..\support\guitools\ide\prj\uiextwiz.h"\
	".\..\support\guitools\ide\prj\uitarget.h"\
	".\..\support\guitools\ide\prj\uprjfile.h"\
	".\..\support\guitools\ide\shl\coconnec.h"\
	".\..\support\guitools\ide\shl\uiconnec.h"\
	".\..\support\guitools\ide\shl\uiconset.h"\
	".\..\support\guitools\ide\shl\cowrkspc.h"\
	".\..\support\guitools\ide\shl\ucustdlg.h"\
	".\..\support\guitools\ide\sym\vshell.h"\
	".\..\support\guitools\ide\shl\uioutput.h"\
	".\..\support\guitools\ide\shl\uiwbdlg.h"\
	".\..\support\guitools\ide\shl\uiwbmsg.h"\
	".\..\support\guitools\ide\shl\upropwnd.h"\
	".\..\support\guitools\ide\shl\utbardlg.h"\
	".\..\support\guitools\ide\shl\utoolbar.h"\
	".\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	".\..\support\guitools\ide\sym\68k\optncplr.h"\
	".\..\support\guitools\ide\sym\68k\optnmrc.h"\
	".\..\support\guitools\ide\sym\appwz.h"\
	".\..\support\guitools\ide\sym\menuids.h"\
	".\..\support\guitools\ide\sym\optnbsc.h"\
	".\..\support\guitools\ide\sym\optnlink.h"\
	".\..\support\guitools\ide\sym\vres.h"\
	".\..\support\guitools\imewrap.h"\
	".\..\..\..\cafe\v3\include\ime32.h"\
	".\..\..\..\cafe\v3\include\test.h"\
	".\..\..\..\cafe\v3\include\idetarg.h"\
	".\..\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\..\cafe\v3\include\target.h"\
	".\..\..\..\cafe\v3\include\toolset.h"\
	".\..\..\..\cafe\v3\include\platform.h"\
	".\..\..\..\cafe\v3\include\toolxprt.h"\
	
# ADD CPP /Yu"stdafx.h"

"$(INTDIR)\sniff.obj" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "classvw - Win32 Debug"

DEP_CPP_SNIFF=\
	".\stdafx.h"\
	".\sniff.h"\
	".\classvw.h"\
	".\..\..\..\cafe\v3\include\test.h"\
	".\..\..\..\cafe\v3\include\subsuite.h"\
	".\..\..\..\cafe\v3\include\support.h"\
	".\..\..\..\Cafe\V3\include\log.h"\
	".\..\..\..\cafe\v3\include\settings.h"\
	".\..\..\..\cafe\v3\include\target.h"\
	".\..\..\..\cafe\v3\include\idetarg.h"\
	".\..\..\..\cafe\v3\include\toolset.h"\
	".\..\..\..\cafe\v3\include\guiv1.h"\
	".\..\..\..\cafe\v3\include\platform.h"\
	".\..\..\..\cafe\v3\include\testxcpt.h"\
	".\..\..\..\cafe\v3\include\mstwrap.h"\
	".\..\support\guitools\imewrap.h"\
	".\..\support\guitools\testutil.h"\
	".\..\support\guitools\uwindow.h"\
	".\..\support\guitools\udialog.h"\
	".\..\support\guitools\umsgbox.h"\
	".\..\support\guitools\coclip.h"\
	".\..\support\guitools\ide\sym\menuids.h"\
	".\..\support\guitools\ide\sym\dockids.h"\
	".\..\support\guitools\ide\sym\qcqp.h"\
	".\..\support\guitools\ide\sym\shrdres.h"\
	".\..\support\guitools\ide\sym\projprop.h"\
	".\..\support\guitools\ide\sym\slob.h"\
	".\..\support\guitools\ide\filetabl.h"\
	".\..\support\guitools\ide\shl\umainfrm.h"\
	".\..\support\guitools\ide\shl\wb_props.h"\
	".\..\support\guitools\ide\shl\wbutil.h"\
	".\..\support\guitools\ide\shl\uwbframe.h"\
	".\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\support\guitools\ide\shl\upropwnd.h"\
	".\..\support\guitools\ide\shl\udockwnd.h"\
	".\..\support\guitools\ide\shl\utbardlg.h"\
	".\..\support\guitools\ide\shl\utoolbar.h"\
	".\..\support\guitools\ide\shl\uitabdlg.h"\
	".\..\support\guitools\ide\shl\uioptdlg.h"\
	".\..\support\guitools\ide\shl\ucustdlg.h"\
	".\..\support\guitools\ide\shl\uioutput.h"\
	".\..\support\guitools\ide\shl\uiwbmsg.h"\
	".\..\support\guitools\ide\shl\uiwbdlg.h"\
	".\..\support\guitools\ide\shl\uiconnec.h"\
	".\..\support\guitools\ide\shl\uiconset.h"\
	".\..\support\guitools\ide\shl\coconnec.h"\
	".\..\support\guitools\ide\shl\cowrkspc.h"\
	".\..\support\guitools\ide\eds\ueditor.h"\
	".\..\support\guitools\ide\eds\uresedit.h"\
	".\..\support\guitools\ide\eds\urestabl.h"\
	".\..\support\guitools\ide\eds\uresbrws.h"\
	".\..\support\guitools\ide\eds\ustredit.h"\
	".\..\support\guitools\ide\eds\uaccedit.h"\
	".\..\support\guitools\ide\eds\uveredit.h"\
	".\..\support\guitools\ide\eds\udlgedit.h"\
	".\..\support\guitools\ide\eds\uimgedit.h"\
	".\..\support\guitools\ide\eds\umenedit.h"\
	".\..\support\guitools\ide\eds\usymdlg.h"\
	".\..\support\guitools\ide\eds\ufindrep.h"\
	".\..\support\guitools\ide\eds\ufindinf.h"\
	".\..\support\guitools\ide\eds\cofile.h"\
	".\..\support\guitools\ide\eds\cosource.h"\
	".\..\support\guitools\ide\eds\coresscr.h"\
	".\..\support\guitools\ide\prj\uprjfile.h"\
	".\..\support\guitools\ide\prj\uitarget.h"\
	".\..\support\guitools\ide\prj\uogaldlg.h"\
	".\..\support\guitools\ide\prj\uprjtdlg.h"\
	".\..\support\guitools\ide\prj\uprojwiz.h"\
	".\..\support\guitools\ide\prj\uiextwiz.h"\
	".\..\support\guitools\ide\prj\uappwiz.h"\
	".\..\support\guitools\ide\prj\ucwzdlg.h"\
	".\..\support\guitools\ide\prj\utstapwz.h"\
	".\..\support\guitools\ide\prj\odbcdlgs.h"\
	".\..\support\guitools\ide\prj\uictrlwz.h"\
	".\..\support\guitools\ide\prj\coprject.h"\
	".\..\support\guitools\ide\prj\uiwrkspc.h"\
	".\..\support\guitools\ide\dbg\parse.h"\
	".\..\support\guitools\ide\dbg\uiqw.h"\
	".\..\support\guitools\ide\dbg\uibp.h"\
	".\..\support\guitools\ide\dbg\uimem.h"\
	".\..\support\guitools\ide\dbg\uieewnd.h"\
	".\..\support\guitools\ide\dbg\uiwatch.h"\
	".\..\support\guitools\ide\dbg\uivar.h"\
	".\..\support\guitools\ide\dbg\uilocals.h"\
	".\..\support\guitools\ide\dbg\uireg.h"\
	".\..\support\guitools\ide\dbg\uicp.h"\
	".\..\support\guitools\ide\dbg\uidam.h"\
	".\..\support\guitools\ide\dbg\uiexcpt.h"\
	".\..\support\guitools\ide\dbg\uistack.h"\
	".\..\support\guitools\ide\dbg\uithread.h"\
	".\..\support\guitools\ide\dbg\uibrowse.h"\
	".\..\support\guitools\ide\dbg\uidebug.h"\
	".\..\support\guitools\ide\dbg\coee.h"\
	".\..\support\guitools\ide\dbg\cobp.h"\
	".\..\support\guitools\ide\dbg\comem.h"\
	".\..\support\guitools\ide\dbg\codebug.h"\
	".\..\support\guitools\ide\dbg\cocp.h"\
	".\..\support\guitools\ide\dbg\coreg.h"\
	".\..\support\guitools\ide\dbg\costack.h"\
	".\..\support\guitools\ide\dbg\cothread.h"\
	".\..\support\guitools\ide\dbg\coexcpt.h"\
	".\..\support\guitools\ide\dbg\codam.h"\
	".\..\support\guitools\ide\dbg\cobrowse.h"\
	".\..\..\..\Cafe\V3\include\targxprt.h"\
	".\..\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\..\cafe\v3\include\idexprt.h"\
	".\..\support\guitools\ide\connec.h"\
	".\..\..\..\cafe\v3\include\toolxprt.h"\
	".\..\..\..\cafe\v3\include\guixprt.h"\
	".\..\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\..\cafe\v3\include\t3scrn.h"\
	".\..\..\..\cafe\v3\include\ime32.h"\
	".\..\..\..\cafe\v3\include\shlxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	
NODEP_CPP_SNIFF=\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\undo.h"\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\slobwnd.h"\
	
# ADD CPP /Yu"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\sniff.obj" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SNIFF) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\classvw.cpp

!IF  "$(CFG)" == "classvw - Win32 Release"

DEP_CPP_CLASS=\
	".\classvw.h"\
	".\sniff.h"\
	".\..\..\..\cafe\v3\include\rawstrng.h"\
	".\stdafx.h"\
	".\..\..\..\cafe\v3\include\subsuite.h"\
	".\..\..\..\Cafe\V3\include\log.h"\
	".\..\..\..\cafe\v3\include\support.h"\
	".\..\support\guitools\coclip.h"\
	".\..\..\..\cafe\v3\include\guixprt.h"\
	".\..\support\guitools\ide\dbg\cobp.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	".\..\support\guitools\ide\dbg\uibp.h"\
	".\..\support\guitools\udialog.h"\
	".\..\support\guitools\uwindow.h"\
	".\..\support\guitools\ide\shl\wbutil.h"\
	".\..\..\..\cafe\v3\include\shlxprt.h"\
	".\..\support\guitools\ide\shl\wb.h"\
	".\..\support\guitools\ide\dbg\cobrowse.h"\
	".\..\support\guitools\ide\dbg\uibrowse.h"\
	".\..\support\guitools\testutil.h"\
	".\..\support\guitools\ide\dbg\cocp.h"\
	".\..\support\guitools\ide\dbg\uicp.h"\
	".\..\support\guitools\ide\shl\udockwnd.h"\
	".\..\support\guitools\ide\dbg\codam.h"\
	".\..\support\guitools\ide\dbg\codebug.h"\
	".\..\support\guitools\ide\shl\uwbframe.h"\
	".\..\support\guitools\ide\shl\umainfrm.h"\
	".\..\support\guitools\ide\dbg\costack.h"\
	".\..\support\guitools\ide\dbg\uistack.h"\
	".\..\support\guitools\ide\sym\qcqp.h"\
	".\..\support\guitools\ide\sym\dockids.h"\
	".\..\support\guitools\ide\dbg\coee.h"\
	".\..\support\guitools\ide\dbg\uiqw.h"\
	".\..\support\guitools\ide\dbg\coexcpt.h"\
	".\..\support\guitools\ide\dbg\uiexcpt.h"\
	".\..\support\guitools\ide\dbg\comem.h"\
	".\..\support\guitools\ide\dbg\uimem.h"\
	".\..\support\guitools\ide\dbg\coreg.h"\
	".\..\support\guitools\ide\dbg\uireg.h"\
	".\..\support\guitools\ide\dbg\cothread.h"\
	".\..\support\guitools\ide\dbg\uithread.h"\
	".\..\support\guitools\ide\dbg\parse.h"\
	".\..\support\guitools\ide\dbg\uidebug.h"\
	".\..\support\guitools\ide\dbg\uieewnd.h"\
	".\..\support\guitools\ide\sym\cmdids.h"\
	".\..\..\..\cafe\v3\include\guiv1.h"\
	".\..\..\..\cafe\v3\include\settings.h"\
	".\..\..\..\Cafe\V3\include\targxprt.h"\
	".\..\..\..\cafe\v3\include\mstwrap.h"\
	".\..\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\..\cafe\v3\include\t3scrn.h"\
	".\..\..\..\cafe\v3\include\testxcpt.h"\
	".\..\support\guitools\ide\dbg\uivar.h"\
	".\..\support\guitools\ide\dbg\uiwatch.h"\
	".\..\support\guitools\ide\eds\cofile.h"\
	".\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	".\..\support\guitools\ide\eds\ueditor.h"\
	".\..\support\guitools\ide\eds\coresscr.h"\
	".\..\support\guitools\ide\eds\uresedit.h"\
	".\..\support\guitools\ide\eds\cosource.h"\
	".\..\support\guitools\ide\shl\uioptdlg.h"\
	".\..\support\guitools\ide\shl\uitabdlg.h"\
	".\..\support\guitools\ide\eds\uaccedit.h"\
	".\..\support\guitools\ide\eds\urestabl.h"\
	".\..\support\guitools\ide\eds\udlgedit.h"\
	".\..\support\guitools\ide\eds\ufindinf.h"\
	".\..\support\guitools\ide\eds\ufindrep.h"\
	".\..\support\guitools\ide\eds\uimgedit.h"\
	".\..\support\guitools\ide\eds\umenedit.h"\
	".\..\support\guitools\ide\eds\uresbrws.h"\
	".\..\support\guitools\ide\eds\ustredit.h"\
	".\..\support\guitools\ide\eds\usymdlg.h"\
	".\..\support\guitools\ide\eds\uveredit.h"\
	".\..\support\guitools\ide\filetabl.h"\
	".\..\..\..\cafe\v3\include\idexprt.h"\
	".\..\support\guitools\ide\prj\coprject.h"\
	".\..\support\guitools\ide\sym\projprop.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	".\..\support\guitools\ide\prj\uiwrkspc.h"\
	".\..\support\guitools\ide\prj\uprjtdlg.h"\
	".\..\support\guitools\ide\sym\optnrc.h"\
	".\..\support\guitools\ide\sym\vproj.h"\
	".\..\support\guitools\ide\prj\uprojwiz.h"\
	".\..\support\guitools\ide\prj\odbcdlgs.h"\
	".\..\support\guitools\ide\prj\ucwzdlg.h"\
	".\..\support\guitools\ide\prj\uictrlwz.h"\
	".\..\support\guitools\ide\prj\uiextwiz.h"\
	".\..\support\guitools\ide\prj\uitarget.h"\
	".\..\support\guitools\ide\prj\uprjfile.h"\
	".\..\support\guitools\ide\shl\coconnec.h"\
	".\..\support\guitools\ide\shl\uiconnec.h"\
	".\..\support\guitools\ide\shl\uiconset.h"\
	".\..\support\guitools\ide\shl\cowrkspc.h"\
	".\..\support\guitools\ide\shl\ucustdlg.h"\
	".\..\support\guitools\ide\sym\vshell.h"\
	".\..\support\guitools\ide\shl\uioutput.h"\
	".\..\support\guitools\ide\shl\uiwbdlg.h"\
	".\..\support\guitools\ide\shl\uiwbmsg.h"\
	".\..\support\guitools\ide\shl\upropwnd.h"\
	".\..\support\guitools\ide\shl\utbardlg.h"\
	".\..\support\guitools\ide\shl\utoolbar.h"\
	".\..\support\guitools\ide\shl\wb_props.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	".\..\support\guitools\ide\sym\68k\optncplr.h"\
	".\..\support\guitools\ide\sym\68k\optnmrc.h"\
	".\..\support\guitools\ide\sym\appwz.h"\
	".\..\support\guitools\ide\sym\menuids.h"\
	".\..\support\guitools\ide\sym\optnbsc.h"\
	".\..\support\guitools\ide\sym\optnlink.h"\
	".\..\support\guitools\ide\sym\vres.h"\
	".\..\support\guitools\imewrap.h"\
	".\..\..\..\cafe\v3\include\ime32.h"\
	".\..\..\..\cafe\v3\include\test.h"\
	".\..\..\..\cafe\v3\include\idetarg.h"\
	".\..\..\..\cafe\v3\include\guitarg.h"\
	".\..\..\..\cafe\v3\include\target.h"\
	".\..\..\..\cafe\v3\include\toolset.h"\
	".\..\..\..\cafe\v3\include\platform.h"\
	".\..\..\..\cafe\v3\include\toolxprt.h"\
	
# ADD CPP /Yu"stdafx.h"

"$(INTDIR)\classvw.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "classvw - Win32 Debug"

DEP_CPP_CLASS=\
	".\stdafx.h"\
	".\classvw.h"\
	".\..\..\..\cafe\v3\include\rawstrng.h"\
	".\..\..\..\cafe\v3\include\support.h"\
	".\..\..\..\cafe\v3\include\test.h"\
	".\..\..\..\cafe\v3\include\subsuite.h"\
	".\sniff.h"\
	".\..\..\..\Cafe\V3\include\log.h"\
	".\..\..\..\cafe\v3\include\settings.h"\
	".\..\..\..\cafe\v3\include\testxcpt.h"\
	".\..\..\..\cafe\v3\include\mstwrap.h"\
	".\..\support\guitools\imewrap.h"\
	".\..\support\guitools\testutil.h"\
	".\..\support\guitools\uwindow.h"\
	".\..\support\guitools\udialog.h"\
	".\..\support\guitools\umsgbox.h"\
	".\..\support\guitools\coclip.h"\
	".\..\support\guitools\ide\sym\menuids.h"\
	".\..\support\guitools\ide\sym\dockids.h"\
	".\..\support\guitools\ide\sym\qcqp.h"\
	".\..\support\guitools\ide\sym\shrdres.h"\
	".\..\support\guitools\ide\sym\projprop.h"\
	".\..\support\guitools\ide\sym\slob.h"\
	".\..\support\guitools\ide\filetabl.h"\
	".\..\support\guitools\ide\shl\umainfrm.h"\
	".\..\support\guitools\ide\shl\wb_props.h"\
	".\..\support\guitools\ide\shl\wbutil.h"\
	".\..\support\guitools\ide\shl\uwbframe.h"\
	".\..\support\guitools\ide\shl\ucommdlg.h"\
	".\..\support\guitools\ide\shl\upropwnd.h"\
	".\..\support\guitools\ide\shl\udockwnd.h"\
	".\..\support\guitools\ide\shl\utbardlg.h"\
	".\..\support\guitools\ide\shl\utoolbar.h"\
	".\..\support\guitools\ide\shl\uitabdlg.h"\
	".\..\support\guitools\ide\shl\uioptdlg.h"\
	".\..\support\guitools\ide\shl\ucustdlg.h"\
	".\..\support\guitools\ide\shl\uioutput.h"\
	".\..\support\guitools\ide\shl\uiwbmsg.h"\
	".\..\support\guitools\ide\shl\uiwbdlg.h"\
	".\..\support\guitools\ide\shl\uiconnec.h"\
	".\..\support\guitools\ide\shl\uiconset.h"\
	".\..\support\guitools\ide\shl\coconnec.h"\
	".\..\support\guitools\ide\shl\cowrkspc.h"\
	".\..\support\guitools\ide\eds\ueditor.h"\
	".\..\support\guitools\ide\eds\uresedit.h"\
	".\..\support\guitools\ide\eds\urestabl.h"\
	".\..\support\guitools\ide\eds\uresbrws.h"\
	".\..\support\guitools\ide\eds\ustredit.h"\
	".\..\support\guitools\ide\eds\uaccedit.h"\
	".\..\support\guitools\ide\eds\uveredit.h"\
	".\..\support\guitools\ide\eds\udlgedit.h"\
	".\..\support\guitools\ide\eds\uimgedit.h"\
	".\..\support\guitools\ide\eds\umenedit.h"\
	".\..\support\guitools\ide\eds\usymdlg.h"\
	".\..\support\guitools\ide\eds\ufindrep.h"\
	".\..\support\guitools\ide\eds\ufindinf.h"\
	".\..\support\guitools\ide\eds\cofile.h"\
	".\..\support\guitools\ide\eds\cosource.h"\
	".\..\support\guitools\ide\eds\coresscr.h"\
	".\..\support\guitools\ide\prj\uprjfile.h"\
	".\..\support\guitools\ide\prj\uitarget.h"\
	".\..\support\guitools\ide\prj\uogaldlg.h"\
	".\..\support\guitools\ide\prj\uprjtdlg.h"\
	".\..\support\guitools\ide\prj\uprojwiz.h"\
	".\..\support\guitools\ide\prj\uiextwiz.h"\
	".\..\support\guitools\ide\prj\uappwiz.h"\
	".\..\support\guitools\ide\prj\ucwzdlg.h"\
	".\..\support\guitools\ide\prj\utstapwz.h"\
	".\..\support\guitools\ide\prj\odbcdlgs.h"\
	".\..\support\guitools\ide\prj\uictrlwz.h"\
	".\..\support\guitools\ide\prj\coprject.h"\
	".\..\support\guitools\ide\prj\uiwrkspc.h"\
	".\..\support\guitools\ide\dbg\parse.h"\
	".\..\support\guitools\ide\dbg\uiqw.h"\
	".\..\support\guitools\ide\dbg\uibp.h"\
	".\..\support\guitools\ide\dbg\uimem.h"\
	".\..\support\guitools\ide\dbg\uieewnd.h"\
	".\..\support\guitools\ide\dbg\uiwatch.h"\
	".\..\support\guitools\ide\dbg\uivar.h"\
	".\..\support\guitools\ide\dbg\uilocals.h"\
	".\..\support\guitools\ide\dbg\uireg.h"\
	".\..\support\guitools\ide\dbg\uicp.h"\
	".\..\support\guitools\ide\dbg\uidam.h"\
	".\..\support\guitools\ide\dbg\uiexcpt.h"\
	".\..\support\guitools\ide\dbg\uistack.h"\
	".\..\support\guitools\ide\dbg\uithread.h"\
	".\..\support\guitools\ide\dbg\uibrowse.h"\
	".\..\support\guitools\ide\dbg\uidebug.h"\
	".\..\support\guitools\ide\dbg\coee.h"\
	".\..\support\guitools\ide\dbg\cobp.h"\
	".\..\support\guitools\ide\dbg\comem.h"\
	".\..\support\guitools\ide\dbg\codebug.h"\
	".\..\support\guitools\ide\dbg\cocp.h"\
	".\..\support\guitools\ide\dbg\coreg.h"\
	".\..\support\guitools\ide\dbg\costack.h"\
	".\..\support\guitools\ide\dbg\cothread.h"\
	".\..\support\guitools\ide\dbg\coexcpt.h"\
	".\..\support\guitools\ide\dbg\codam.h"\
	".\..\support\guitools\ide\dbg\cobrowse.h"\
	".\..\..\..\cafe\v3\include\guiv1.h"\
	".\..\..\..\cafe\v3\include\target.h"\
	".\..\..\..\cafe\v3\include\idetarg.h"\
	".\..\..\..\cafe\v3\include\toolset.h"\
	".\..\..\..\cafe\v3\include\platform.h"\
	".\..\..\..\Cafe\V3\include\targxprt.h"\
	".\..\..\..\cafe\v3\include\t3ctrl.h"\
	".\..\..\..\cafe\v3\include\t3scrn.h"\
	".\..\..\..\cafe\v3\include\guixprt.h"\
	".\..\..\..\cafe\v3\include\ime32.h"\
	".\..\..\..\cafe\v3\include\idexprt.h"\
	".\..\..\..\cafe\v3\include\shlxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\sym\props.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\eds\edsxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\prj\prjxprt.h"\
	".\..\..\..\cafe\v3\support\guitools\ide\dbg\dbgxprt.h"\
	".\..\..\..\cafe\v3\include\guitarg.h"\
	".\..\support\guitools\ide\connec.h"\
	".\..\..\..\cafe\v3\include\toolxprt.h"\
	
NODEP_CPP_CLASS=\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\undo.h"\
	".\..\..\..\Cafe\V3\support\guitools\ide\sym\slobwnd.h"\
	
# ADD CPP /Yu"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/classvw.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\classvw.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"\
 "$(INTDIR)\classvw.pch"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
