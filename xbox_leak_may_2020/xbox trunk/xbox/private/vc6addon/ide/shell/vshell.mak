# Builds the shell MFC extension DLL (MSVCSHL(D).DLL)
#
# Usage:     NMAKE option (build msvcshl(d).dll)
#    or:     NMAKE clean  (erase all compiled files)
#    or:	 NMAKE auto.dep (build dependencies)
#
# option:    DEBUG =[0|1]  (DEBUG not defined is equivalent to DEBUG=1)
#            CODEVIEW=[0|1](CODEVIEW not defined is equivalent to CODEVIEW=0. Not needed if DEBUG=1)
#            BROWSE=[0|1]  (BROWSE not defined is equivalent to BROWSE=0)
#			 NODEP=[0|1]   (do not use auto dependency information)
#            TEST=[0|1]    (TEST is defined by default for DEBUG)
#            JDATE=[0|1]   (JDATE is defined by default)
#			 ILINK=[0|1]   (ILINK is 0 by default, forces JDATE=0 if set)

!if "$(OLE)" == ""
OLE = 1
!endif

!if "$(LINKER)"==""
LINKER=link
!endif

#
# pick up platform dependent macros
#
!include ..\standard.mak

CMDFILE=vshell
MODEL=n
LIB = ..\$(SUSHILIB);$(LIB)

TARGET=w
DLL=2
LIBS=

!if "$(DBC)" == ""
DBC = 1
!endif

!if "$(DEBUG)"!="0"
# Debug DLL build
D=D
RCDEFINES=/D_DEBUG
CODEVIEW=1
LFLAGS=/DEBUG /DEBUGTYPE:cv$(LEGOLINK)
!if "$(CPU)" != "ALPHA"
LIBS=largeint.lib
!endif
!else
# Release DLL build
D=
RCDEFINES=
!if "$(CODEVIEW)" == "1"
# Release build with CodeView info
LFLAGS=/DEBUG /DEBUGTYPE:cv
!else
CODEVIEW=0
LFLAGS=
!endif
!endif

TARG=MSVCSHL$D

!ifndef NEWVIEW
NEWVIEW = 1
!endif

!ifndef ORDER
!if "$(DEBUG)" == "0"
ORDER = 1
!else
ORDER = 0
!endif
!endif

!if $(ORDER)
ORDER_FILE = msvcshl$(ORDER_FILE_EXT)
LFLAGS = $(LFLAGS) -order:@$(ORDER_FILE)
!endif

!if "$(REDEF)" == "1"
LFLAGS = $(LFLAGS) -incremental:no
!else
!if "$(DEBUG)" == "0"
!if "$(ILINK)" == "0"
# default when building release version is no ilink, remove dead functions
# must use -opt:noref because of brain-dead way of generating exports
LFLAGS = $(LFLAGS) -incremental:no -opt:noref
!else
LFLAGS = $(LFLAGS) -incremental:yes
!endif
!else
LFLAGS = $(LFLAGS) -incremental:yes
!endif
!endif

RCDEFINES=$(RCDEFINES) /D_WIN32
LFLAGS=$(LFLAGS) /nologo /dll /opt:noref /subsystem:windows /version:2,1\
!if "$(REDEF)"=="1"
	/map:$O\$(TARG).map\
!endif
	/base:@dllbase.txt,msvcshl /nodefaultlib:libc.lib


LIBS=$(LIBS) msvcrt$D.lib mfcs40$D.lib mfc40$D.lib \
	ole32.lib kernel32.lib gdi32.lib user32.lib winmm.lib \
	shell32.lib comdlg32.lib advapi32.lib version.lib \
	msf512$D.lib uuid3.lib compmgr$D.lib

!if "$(DEBUG)" == "1"
LIBS=$(LIBS) mfco40d.lib
!endif

!ifdef RELEASE # Release VERSION info
RCDEFINES=$(RCDEFINES) /DRELEASE
!endif

!if "$(REDEF)"=="1"
dll_goal: create2.dir ..\$(SUSHIBIN)\$(TARG).dll ..\$(SUSHILIB)\$(TARG).lib
!else
dll_goal: create2.dir ..\$(SUSHIBIN)\$(TARG).dll
!endif


#############################################################################
# import most rules and library files from normal makefile

###################  Begin include makefile #################

##### Builds VSHELL and VSHELLD DLL's and LIB's
#
# Usage:     NMAKE [flags]				Build all targets
#    or:     NMAKE clean				Prepare for clean build
#
# option:    DEBUG =[0|1]  (DEBUG not defined is equivalent to DEBUG=1)
#            BROWSE=[0|1]  (BROWSE not defined is equivalent to BROWSE=0)
#

##### Directory macros

!if "$(LANGAPI)"==""
LANGAPI=\langapi
!endif

INCLUDE = ..\include;$(INCLUDE);$(LANGAPI)\include;$(LANGAPI)\debugger
DEP_INCLUDE = /I..\include

LIB_DIR = ..\$(SUSHILIB)
OBJ_DIR = $$obj$D32$(OBJ_TARGET_EXT)

!if defined(TARGMAC68K)
OBJ_DIR=$(OBJ_DIR).mac
!else
!if defined(TARGMACPPC)
OBJ_DIR=$(OBJ_DIR).ppc
!endif
!endif

PDB_FILE = vshell$D.pdb

O=$(OBJ_DIR)

##### Tool flag macros

CDEFS		=	/D_WIN32 /D_AFXDLL
CFLAGS	    =	/W3 /WX /Zp8 /Gy /Gs /Fd$O\$(PDB_FILE) /Fo$O\ /nologo /ZB64 $(MFC_EH) $(EXTRA_CFLAGS)
PCHFLAGS	=	/Yustdafx.h /Fp$(OBJ_DIR)\stdafx.pch

!if "$(CPU)"=="ALPHA"
CFLAGS      =   $(CFLAGS)
!endif

CDEFS		=	$(CDEFS) /D_$(HOST_CPU)_ /D_WINDOWS /D_SUSHI /D_AFXEXT

!if "$(DEBUG)"!="0"
CFLAGS=$(CFLAGS) /Od /Zi /D_DEBUG /MDd
!else
CFLAGS=$(CFLAGS) $(RETAIL_OPT) /DNDEBUG	/MD
!if "$(CODEVIEW)"=="1"
CFLAGS=$(CFLAGS) /Zi
!endif
!endif

!if "$(BROWSE)"=="1"
CFLAGS=$(CFLAGS) /Zn /FR
!endif

!if "$(TEST)"!="0"
TEST=1
!endif

!if "$(TEST)"=="1"
CDEFS=$(CDEFS) /D_TEST
!endif

!if "$(COVER)"=="1"
CDEFS=$(CDEFS) /DCODECOVERAGE
!endif

!if "$(NEWVIEW)"=="1"
CDEFS=$(CDEFS) /DNEW_PROJ_VIEW
!endif

!if "$(DBC)" == "1"
!message Building DBC enabled version!
!message adding -D_MBCS to compile flags
CDEFS = $(CDEFS) -D_MBCS
!endif

!if "$(PERF_MEASUREMENTS)" != "0"
!message Building Perf instrumented version.
CDEFS = $(CDEFS) -DPERF_MEASUREMENTS
!endif


!ifdef TARGMAC68K
CDEFS=$(CDEFS) /DTARGMAC68K
!endif

# Build all platforms defining PPC_PLATFORM
# later remove the ifdefs PPC_PLATFORM in prjconfg.h prjconfg.cpp
#
PPC_PLATFORM= /DPPC_PLATFORM
CDEFS=$(CDEFS) $(PPC_PLATFORM)

!ifdef TARGMACPPC
CDEFS=$(CDEFS) /DTARGMACPPC
!endif

##### File list

PCH 	=	$O\stdafx.pch

OBJS	=	$O\stdafx.obj\
			$O\about.obj\
#			$O\assert.obj\
			$O\autoapp.obj\
			$O\autodocd.obj\
			$O\autodocs.obj\
			$O\autofrm.obj\
			$O\autosave.obj\
			$0\autostat.obj\
			$O\autowind.obj\
			$O\autowins.obj\
			$O\barbdr.obj\
			$O\barctrl.obj\
			$O\barcust.obj\
			$O\bardlgs.obj\
			$O\bardockx.obj\
			$O\barglob.obj\
			$O\cbitem.obj\
			$O\cbmenu.obj\
			$O\cbpopup.obj\
			$O\cbverb.obj\
			$O\checklis.obj\
			$O\cmdtable.obj\
			$O\customiz.obj\
			$O\dirmgr.obj\
			$O\dlgbase.obj\
			$O\docauto.obj\
			$O\dockapi.obj\
			$O\dockman.obj\
			$O\dockwnds.obj\
			$O\docobctr.obj\
			$O\docobdoc.obj\
			$O\docobtpl.obj\
			$O\docobvw.obj\
			$O\drptcoll.obj\
			$O\envrvar.obj\
			$O\errcont.obj\
			$O\fcdialog.obj\
			$O\filechng.obj\
			$O\filecmd.obj\
			$O\filefind.obj\
			$O\filesave.obj\
			$O\findctrl.obj\
			$O\floatwnd.obj\
			$O\fmtinfo.obj\
			$O\gotodlg.obj\
			$O\grid.obj\
			$O\help.obj\
			$O\helpkey.obj\
			$O\iconlist.obj\
			$O\imeutil.obj\
			$O\imgwell.obj\
			$O\initexit.obj\
			$O\ipcfrmhk.obj\
			$O\ipcmpctr.obj\
			$O\ipcmpdoc.obj\
			$O\ipcmpvw.obj\
			$O\ipcslob.obj\
			$O\keycust.obj\
			$O\keymap.obj\
			$O\cmdcache.obj\
			$O\main.obj\
			$O\mainfrm.obj\
			$O\menubtn.obj\
			$O\msgboxes.obj\
			$O\multslob.obj\
			$O\newwsdlg.obj\
			$O\oldbar.obj\
			$O\oldstat.obj\
			$O\oleref.obj\
			$O\opendir.obj\
			$O\opendocs.obj\
			$O\package.obj\
			$O\partdoc.obj\
			$O\partfrm.obj\
			$O\partmpl.obj\
			$O\partvw.obj\
			$O\path.obj\
			$O\prodinfo.obj\
			$O\profile.obj\
			$O\progdlg.obj\
			$O\propbag.obj\
			$O\proppage.obj\
			$O\propsite.obj\
			$O\provider.obj\
			$O\sbar.obj\
			$O\sfind.obj\
			$O\shbrowse.obj\
			$O\sheet.obj\
			$O\slob.obj\
			$O\slobdrag.obj\
			$O\slobwnd.obj\
			$O\smenu.obj\
			$O\spopup.obj\
			$O\stdfont.obj\
			$O\stgfile.obj\
			$O\tabdlg.obj\
			$O\tabpage.obj\
			$O\tap.obj\
			$O\test.obj\
			$O\toolexpt.obj\
			$O\tools.obj\
			$O\toolsdlg.obj\
			$O\tipmgr.obj\
			$O\tipdlg.obj\
			$O\tipctrl.obj\
			$O\treectl.obj\
			$O\undo.obj\
			$O\undoctl.obj\
			$O\util.obj\
			$O\widgetsb.obj\
			$O\workspc.obj\

TARGET	=	shr$(DLL)$D.lib

##### Create final targets

all:	hack $O $(LIB_DIR)\$(TARGET)
	@if exist $O\$(PDB_FILE) ren $O\$(PDB_FILE) $(PDB_FILE:PDB=PDX)
!if "$(DLL)"==""
	@$(MAKE) /$(MAKEFLAGS) DLL=dll
!endif

# This is a hack.  When we're building this lib, we want the PDB file to have the
# extension .PDB.  When we're NOT building it (i.e., when we're done building it),
# we want it to have the extension .PDX.  This is so that there's no chance that
# CVPACKs from other directories will pick up this one instead of the one from
# the LIB directory.

hack:
	@if exist $O\$(PDB_FILE:PDB=PDX) ren $O\$(PDB_FILE:PDB=PDX) $(PDB_FILE)

$O:
	@if not exist $O mkdir $O

$(LIB_DIR)\$(TARGET): $(OBJS)
	@lib -nologo -debugtype:cv -machine:$(PROCESSOR_ARCHITECTURE) -out:$O\$(TARGET) @<<
$(OBJS: =^
)
<<
	@-del /f $(LIB_DIR)\$(TARGET)
	@copy $O\$(TARGET) $(LIB_DIR)
	@-del /f $(LIB_DIR)\$(PDB_FILE)
	@copy $O\$(PDB_FILE) $(LIB_DIR)

##### Special rules

$O\stdafx.obj $O\stdafx.pch: ..\include\stdafx.h
	@$(CPP) $(CFLAGS) $(CDEFS) /Ycstdafx.h /Fp$O\stdafx.pch /c stdafx.cpp

*.obj: $O\stdafx.pch

.cpp{$O}.obj:
	@$(CPP) $(CFLAGS) $(CDEFS) $(PCHFLAGS) /c $<

.c{$O}.obj:
	@$(CC) $(CFLAGS) $(CDEFS) /c $<

$(CMDFILE).rcs: $(CMDFILE).cmd
	cmdcomp $(CMDFILE)

$O\initexit.obj: initexit.cpp

###################  End include makefile #################



create2.dir:
	@-if not exist $O\*.* mkdir $O

#############################################################################
# Debug target

# if RCFILE is not defined, default to "vshell"
# this allows the use of a localized version of the resource file
!if "$(RCFILE)" == ""
RCFILE=vshell
!endif

# if RCTOOL is not defined, default to "rc"
# this allows the use a different rc from NT-J
!if "$(RCTOOL)" == ""
RCTOOL=rc
!endif

shell.tlb: shell.odl
	mktyplib shell.odl

$O\$(TARG).res: $(RCFILE).rc res\*.* $(CMDFILE).rcs shell.tlb
	$(RCTOOL) /r $(RCDEFINES) /fo $O\$(TARG).res $(RCFILE).rc

DLL_OBJS=$(OBJS) $O\dllinit.obj

..\$(SUSHIBIN)\$(TARG).dll ..\$(SUSHILIB)\$(TARG).lib: $(DLL_OBJS) $(TARG).def $O\$(TARG).res $(ORDER_FILE)
        -@attrib -r ..\$(SUSHIBIN)\$(TARG).DLL
        -@attrib -r ..\$(SUSHIBIN)\$(TARG).PDB
        -@attrib -r ..\$(SUSHILIB)\$(TARG).LIB
	$(LINKER) @<<$O\vshell.rsp
$(LFLAGS)
$(LIBS: =^
)
$(DLL_OBJS: =^
)
$O\$(TARG).res
/def:$(TARG).def
/out:..\$(SUSHIBIN)\$(TARG).dll
/implib:..\$(SUSHILIB)\$(TARG).lib
$(EXTRA_LFLAGS)
<<keep

auto.dep: nul
	copy << auto.dep
#--------------------------------------------------------------------
# AUTOMATICALLY GENERATED BY MKDEP
#
# To regenerate dependencies, check out this file and then type
#     nmake auto.dep
#
# Does not add resource.h to dep lists
#--------------------------------------------------------------------

<<
	mkdep $(DEP_INCLUDE) -P $$(O)\ -s .obj -n *.c *.cpp \
	| sed -e "s!	!   !" \
		-e "s!../vproj/resource.h!!g" \
		-e "s!resource.h!!g" >> auto.dep

# --[Project dependencies]-----------------

# N.B. no resource dependencies

!if "$(NODEP)"!="1"
!if exist(auto.dep)	
!include auto.dep
!else
!message Note: no AUTO.DEP currently exists
!endif									
!endif

#############################################################################

clean:
	@-if exist $O\*.* delnode /q $O
	@-if exist $(CMDFILE).rcs erase $(CMDFILE).rcs
