# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=lfn - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to lfn - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lfn - Win32 Release" && "$(CFG)" != "lfn - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "lfn.mak" CFG="lfn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lfn - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lfn - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lfn - Win32 Release"

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

ALL : ".\bin\lfn.dll"

CLEAN : 
	-@erase ".\bin\lfn.dll"
	-@erase ".\Release\filecase.obj"
	-@erase ".\Release\filesub.obj"
	-@erase ".\Release\lfn.exp"
	-@erase ".\Release\lfn.lib"
	-@erase ".\Release\lfn.pch"
	-@erase ".\Release\stdafx.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/lfn.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lfn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin\lfn.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/lfn.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin\lfn.dll"\
 /implib:"$(OUTDIR)/lfn.lib" 
LINK32_OBJS= \
	".\Release\filecase.obj" \
	".\Release\filesub.obj" \
	".\Release\stdafx.obj"

".\bin\lfn.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lfn - Win32 Debug"

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

ALL : ".\bin\lfnd.dll"

CLEAN : 
	-@erase ".\bin\lfnd.dll"
	-@erase ".\bin\lfnd.ilk"
	-@erase ".\Debug\filecase.obj"
	-@erase ".\Debug\filesub.obj"
	-@erase ".\Debug\lfn.pch"
	-@erase ".\Debug\lfnd.exp"
	-@erase ".\Debug\lfnd.lib"
	-@erase ".\Debug\lfnd.pdb"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\vc40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/lfn.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lfn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin\lfnd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/lfnd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin\lfnd.dll"\
 /implib:"$(OUTDIR)/lfnd.lib" 
LINK32_OBJS= \
	".\Debug\filecase.obj" \
	".\Debug\filesub.obj" \
	".\Debug\stdafx.obj"

".\bin\lfnd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "lfn - Win32 Release"
# Name "lfn - Win32 Debug"

!IF  "$(CFG)" == "lfn - Win32 Release"

!ELSEIF  "$(CFG)" == "lfn - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	"..\..\..\cafe\include\guitarg.h"\
	"..\..\..\cafe\include\idetarg.h"\
	"..\..\..\cafe\include\log.h"\
	"..\..\..\cafe\include\platform.h"\
	"..\..\..\cafe\include\t3ctrl.h"\
	"..\..\..\cafe\include\t3scrn.h"\
	"..\..\..\cafe\include\target.h"\
	"..\..\..\cafe\include\targxprt.h"\
	"..\..\..\cafe\include\toolset.h"\
	"..\..\..\cafe\include\toolxprt.h"\
	"..\..\..\cafe\support\guitools\ide\dbg\dbgxprt.h"\
	"..\..\..\cafe\support\guitools\ide\eds\edsxprt.h"\
	"..\..\..\cafe\support\guitools\ide\prj\prjxprt.h"\
	"..\..\..\cafe\support\guitools\ide\sym\props.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coenv.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\ime32.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	{$(INCLUDE)}"\settings.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\subsuite.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\test.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	
NODEP_CPP_STDAF=\
	"..\..\..\cafe\support\guitools\ide\sym\slobwnd.h"\
	"..\..\..\cafe\support\guitools\ide\sym\undo.h"\
	

!IF  "$(CFG)" == "lfn - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/lfn.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

".\Release\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Release\lfn.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "lfn - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/lfn.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

".\Debug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\Debug\lfn.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filesub.cpp
DEP_CPP_FILES=\
	"..\..\..\cafe\include\guitarg.h"\
	"..\..\..\cafe\include\idetarg.h"\
	"..\..\..\cafe\include\log.h"\
	"..\..\..\cafe\include\platform.h"\
	"..\..\..\cafe\include\t3ctrl.h"\
	"..\..\..\cafe\include\t3scrn.h"\
	"..\..\..\cafe\include\target.h"\
	"..\..\..\cafe\include\targxprt.h"\
	"..\..\..\cafe\include\toolset.h"\
	"..\..\..\cafe\include\toolxprt.h"\
	"..\..\..\cafe\support\guitools\ide\dbg\dbgxprt.h"\
	"..\..\..\cafe\support\guitools\ide\eds\edsxprt.h"\
	"..\..\..\cafe\support\guitools\ide\prj\prjxprt.h"\
	"..\..\..\cafe\support\guitools\ide\sym\props.h"\
	".\FILEcase.h"\
	".\FILEsub.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coenv.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\ime32.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	{$(INCLUDE)}"\settings.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\subsuite.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\test.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	
NODEP_CPP_FILES=\
	"..\..\..\cafe\support\guitools\ide\sym\slobwnd.h"\
	"..\..\..\cafe\support\guitools\ide\sym\undo.h"\
	

!IF  "$(CFG)" == "lfn - Win32 Release"


".\Release\filesub.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"\
 ".\Release\lfn.pch"


!ELSEIF  "$(CFG)" == "lfn - Win32 Debug"


".\Debug\filesub.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"\
 ".\Debug\lfn.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filecase.cpp
DEP_CPP_FILEC=\
	"..\..\..\cafe\include\guitarg.h"\
	"..\..\..\cafe\include\idetarg.h"\
	"..\..\..\cafe\include\log.h"\
	"..\..\..\cafe\include\platform.h"\
	"..\..\..\cafe\include\t3ctrl.h"\
	"..\..\..\cafe\include\t3scrn.h"\
	"..\..\..\cafe\include\target.h"\
	"..\..\..\cafe\include\targxprt.h"\
	"..\..\..\cafe\include\toolset.h"\
	"..\..\..\cafe\include\toolxprt.h"\
	"..\..\..\cafe\support\guitools\ide\dbg\dbgxprt.h"\
	"..\..\..\cafe\support\guitools\ide\eds\edsxprt.h"\
	"..\..\..\cafe\support\guitools\ide\prj\prjxprt.h"\
	"..\..\..\cafe\support\guitools\ide\sym\props.h"\
	".\FILEcase.h"\
	".\FILEsub.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"\..\support\guitools\coclip.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\connec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cobrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cocp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\codebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coee.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\comem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\coreg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\costack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\cothread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\parse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uibrowse.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uicp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidam.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uidebug.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uieewnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiexcpt.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uilocals.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uimem.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiqw.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uireg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uistack.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uithread.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uivar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\dbg\uiwatch.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cofile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\coresscr.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\cosource.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uaccedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\udlgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ueditor.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindinf.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ufindrep.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uimgedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\umenedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresbrws.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uresedit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\urestabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\ustredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\usymdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\eds\uveredit.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\filetabl.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\coprject.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\odbcdlgs.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uappwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\ucwzdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uictrlwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiextwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uitarget.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uiwrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uogaldlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjfile.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprjtdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\uprojwiz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\prj\utstapwz.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\coenv.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\cowrkspc.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucommdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\ucustdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\udockwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconnec.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiconset.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioptdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uioutput.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uitabdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbdlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uiwbmsg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\umainfrm.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\upropwnd.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utbardlg.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\utoolbar.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\uwbframe.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wb_props.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\shl\wbutil.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\dockids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\menuids.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\projprop.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\qcqp.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\shrdres.h"\
	{$(INCLUDE)}"\..\support\guitools\ide\sym\slob.h"\
	{$(INCLUDE)}"\..\support\guitools\imewrap.h"\
	{$(INCLUDE)}"\..\support\guitools\testutil.h"\
	{$(INCLUDE)}"\..\support\guitools\udialog.h"\
	{$(INCLUDE)}"\..\support\guitools\umsgbox.h"\
	{$(INCLUDE)}"\..\support\guitools\uwindow.h"\
	{$(INCLUDE)}"\guiv1.h"\
	{$(INCLUDE)}"\guixprt.h"\
	{$(INCLUDE)}"\idexprt.h"\
	{$(INCLUDE)}"\ime32.h"\
	{$(INCLUDE)}"\mstwrap.h"\
	{$(INCLUDE)}"\settings.h"\
	{$(INCLUDE)}"\shlxprt.h"\
	{$(INCLUDE)}"\subsuite.h"\
	{$(INCLUDE)}"\support.h"\
	{$(INCLUDE)}"\test.h"\
	{$(INCLUDE)}"\testxcpt.h"\
	
NODEP_CPP_FILEC=\
	"..\..\..\cafe\support\guitools\ide\sym\slobwnd.h"\
	"..\..\..\cafe\support\guitools\ide\sym\undo.h"\
	

!IF  "$(CFG)" == "lfn - Win32 Release"


".\Release\filecase.obj" : $(SOURCE) $(DEP_CPP_FILEC) "$(INTDIR)"\
 ".\Release\lfn.pch"


!ELSEIF  "$(CFG)" == "lfn - Win32 Debug"


".\Debug\filecase.obj" : $(SOURCE) $(DEP_CPP_FILEC) "$(INTDIR)"\
 ".\Debug\lfn.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
