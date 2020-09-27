# CAFE Driver build
#
# Usage:
#
# nmake /f cafedrv.mk [all]         Build the EXE
# nmake /f cafedrv.mk ssync         Ssync the files needed to build this EXE
# nmake /f cafedrv.mk slmout        Check out the files that are built by this makefile
# nmake /f cafedrv.mk slmin         Check in the files that are built by this makefile
# nmake /f cafedrv.mk clean         Clean the project
# nmake /f cafedrv.mk auto.dep		Generate dependencies file (auto.dep)
#
#
# Additional command line defines: * indicates default
#
# DEBUG=	[ 0* | 1 | 2 ]			Build debug (0), retail (1), or retail with debug info (2)
# BROWSE=	[ 0* | 1 ]				Build with browse information (1)
# PLATFORM= [ X86*, MIPS, ALPHA ]	Specify the platform
# DEPEND=   [ 0 | 1* ]				Indicate whether to include dependent projects (1) or not (0)
#

# ---------------------------------------------
# ensure all defines are defined
# ---------------------------------------------
!ifndef DEBUG
!message DEBUG not defined; defaulting to DEBUG=0
DEBUG=0
!endif
!ifndef BROWSE
!message BROWSE not defined; defaulting to BROWSE=0
BROWSE=0
!endif
!ifndef DEPEND
!message DEPEND not defined; defaulting to DEPEND=1
DEPEND=1
!endif
!ifndef PLATFORM
!ifndef PROCESSOR_ARCHITECTURE
!message PLATFORM or PROCESSOR_ARCHITECTURE not defined; defaulting to PLATFORM=x86
PLATFORM=x86
!else
PLATFORM=$(PROCESSOR_ARCHITECTURE)
!endif
!endif
!ifndef V4LOW
!message V4LOW not defined; defaulting to V4LOW=0
V4LOW=0
!endif


# ---------------------------------------------
# define relative CAFE directories
# ---------------------------------------------
CAFE_INCLUDE=..\..\include
CAFE_TARGET=..\support
CAFE_GUI=..\support\guitools
CAFE_IDE=..\support\guitools\ide
CAFE_SYM=..\support\guitools\ide\sym
CAFE_SHL=..\support\guitools\ide\shl
CAFE_EDS=..\support\guitools\ide\eds
CAFE_PRJ=..\support\guitools\ide\prj
CAFE_DBG=..\support\guitools\ide\dbg
CAFE_TOOLSET=..\support
CAFE_SUBSUITE=..\subsuite

EXEDIR=..\..\bin
LIBDIR=..\..\lib

# ---------------------------------------------
# define toolset
# ---------------------------------------------
COMPILER=cl.exe
LINKER=link.exe
RC=rc.exe
BSC=bscmake.exe

# ---------------------------------------------
# set up debug/retail defines
# ---------------------------------------------
# OPTIMIZATIONS ARE TURNED OFF IN RETAIL SINCE WE GOT UTC COMPILER. (RANDOM GPFS) 
!if "$(DEBUG)" == "0"
OBJDIR=WinRel
TARGET=cafedrv
CPP_FLAGS=-Od -DNDEBUG -MD
LINK_FLAGS=-incremental:no
!elseif "$(DEBUG)" == "1"
OBJDIR=WinDebug
TARGET=cafedrvd
CPP_FLAGS=-Zi -Od -D_DEBUG -MDd
LINK_FLAGS=-incremental:yes -debug
!else
OBJDIR=WinRel
TARGET=cafedrv
CPP_FLAGS=-Zi -O2 -DNDEBUG -MD
LINK_FLAGS=-incremental:no -debug
!endif

# ---------------------------------------------
# set up platform-specific defines
# ---------------------------------------------
!if "$(PLATFORM)" == "x86"
!if "$(DEBUG)" != "0"
!message CAFE Driver: MR enabled
CPP_FLAGS=$(CPP_FLAGS) -Gm
!endif
!endif

# ---------------------------------------------
# set up browse info defines
# ---------------------------------------------
!if "$(BROWSE)" == "0"
BSC_FLAGS=
!else
CPP_FLAGS=$(CPP_FLAGS) -FR$(OBJDIR)^\
BSC_FLAGS=-o$(OBJDIR)\$(TARGET).bsc
!endif

# ---------------------------------------------
# set up toolset flags
# ---------------------------------------------
CPP_FLAGS=$(CPP_FLAGS) -nologo -W3 -GR -GX -DWIN32 -D_WINDOWS -D_MBCS -D_AFXDLL -DMST_BUG -Fp$(OBJDIR)\$(TARGET).pch -Fo$(OBJDIR)^\ -Fd$(OBJDIR)\$(TARGET).pdb -c
CPP_CREATE_PCH_FLAGS=$(CPP_FLAGS) -Ycstdafx.h
CPP_FLAGS=$(CPP_FLAGS) -Yustdafx.h
LINK_FLAGS=$(LINK_FLAGS) -nologo -subsystem:windows -heap:4096 -out:$(EXEDIR)\$(TARGET).exe -pdb:$(EXEDIR)\$(TARGET).pdb -implib:$(LIBDIR)\$(TARGET).lib winmm.lib
RC_FLAGS=/l 0x409 /fo$(OBJDIR)\cafe.res /dNDEBUG
BSC_FLAGS=$(BSC_FLAGS) -nologo

# ---------------------------------------------
# set up V4Low flags
# ---------------------------------------------
!if "$(_LOGVIEW)" == "1"
CPP_FLAGS=$(CPP_FLAGS) -D_LOGVIEW
!endif


# ---------------------------------------------
# set up V4Low flags
# ---------------------------------------------
!if "$(V4LOW)" == "1"
CPP_FLAGS=$(CPP_FLAGS) -DV4LOW
!endif

# ---------------------------------------------
# define objs
# ---------------------------------------------
OBJFILES= \
	$(OBJDIR)\portdoc.obj  \
	$(OBJDIR)\portview.obj \
	$(OBJDIR)\logview.obj \
	$(OBJDIR)\cafemdi.obj \
	$(OBJDIR)\mainfrm.obj  \
	$(OBJDIR)\stdafx.obj   \
	$(OBJDIR)\cafe.res     \
	$(OBJDIR)\cafedrv.obj  \
	$(OBJDIR)\suitedoc.obj \
	$(OBJDIR)\suitevw.obj  \
	$(OBJDIR)\treectl.obj  \
	$(OBJDIR)\cafebar.obj  \
	$(OBJDIR)\suitebar.obj \
	$(OBJDIR)\vwprtbar.obj \
	$(OBJDIR)\cafepage.obj \
	$(OBJDIR)\logpage.obj  \
	$(OBJDIR)\cafexcpt.obj \
	$(OBJDIR)\hotkey.obj   \
	$(OBJDIR)\aplogdlg.obj \
	$(OBJDIR)\tbdlg.obj    \
	$(OBJDIR)\graph.obj    \
	$(OBJDIR)\suitepg.obj  \
	$(OBJDIR)\graphdlg.obj \
	$(OBJDIR)\graphwnd.obj \
	$(OBJDIR)\testspg.obj  \
	$(OBJDIR)\statepg.obj  



# ---------------------------------------------
# define libs
# ---------------------------------------------
LIBFILES= \
!if "$(DEBUG)"=="1"
    $(LIBDIR)\subsuitd.lib
!else
    $(LIBDIR)\subsuite.lib
!endif


# ---------------------------------------------
# define sbrs
# ---------------------------------------------
SBRFILES= \
	$(OBJDIR)\portdoc.sbr  \
	$(OBJDIR)\portview.sbr \
	$(OBJDIR)\logview.sbr \
	$(OBJDIR)\cafemdi.sbr \
	$(OBJDIR)\mainfrm.sbr  \
	$(OBJDIR)\stdafx.sbr   \
	$(OBJDIR)\cafedrv.sbr  \
	$(OBJDIR)\suitedoc.sbr \
	$(OBJDIR)\suitevw.sbr  \
	$(OBJDIR)\treectl.sbr  \
	$(OBJDIR)\cafebar.sbr  \
	$(OBJDIR)\suitebar.sbr \
	$(OBJDIR)\vwprtbar.sbr \
	$(OBJDIR)\cafepage.sbr \
	$(OBJDIR)\logpage.sbr  \
	$(OBJDIR)\cafexcpt.sbr \
	$(OBJDIR)\hotkey.sbr   \
	$(OBJDIR)\aplogdlg.sbr \
	$(OBJDIR)\tbdlg.sbr    \
	$(OBJDIR)\graph.sbr    \
	$(OBJDIR)\suitepg.sbr  \
	$(OBJDIR)\graphdlg.sbr \
	$(OBJDIR)\graphwnd.sbr \
	$(OBJDIR)\testspg.sbr  \
	$(OBJDIR)\statepg.sbr


# ---------------------------------------------
# ---------------------------------------------
# rules
# ---------------------------------------------

.c{$(OBJDIR)}.obj:
   @$(COMPILER) -FIstdafx.h -FIW32Repl.h $(CPP_FLAGS) $<

.cpp{$(OBJDIR)}.obj:
   @$(COMPILER) -FIstdafx.h -FIW32Repl.h $(CPP_FLAGS) $<

.cxx{$(OBJDIR)}.obj:
   @$(COMPILER) -FIstdafx.h -FIW32Repl.h $(CPP_FLAGS) $<


# ---------------------------------------------
# ---------------------------------------------
# targets
# ---------------------------------------------

all: dependencies $(EXEDIR)\$(TARGET).exe

# ---------------------------------------------
# build dependencies
# ---------------------------------------------
dependencies:
!if "$(DEPEND)" == "1"
	@echo CAFE driver: building dependencies...
# SubSuite LIB
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND)
	@cd $(MAKEDIR)
# Toolset DLL
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND)
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# build target EXE
# ---------------------------------------------
$(EXEDIR)\$(TARGET).exe: $(OBJDIR) $(OBJDIR)\$(TARGET).pch $(OBJFILES) $(LIBFILES)
	@echo CAFE driver: linking...
	@-if exist $(EXEDIR)\$(TARGET).exe attrib -r $(EXEDIR)\$(TARGET).exe
	@-if exist $(EXEDIR)\$(TARGET).pdb attrib -r $(EXEDIR)\$(TARGET).pdb
	@$(LINKER) @<<
		$(LINK_FLAGS) $(OBJFILES)
<<
# ---------------------------------------------
# build BSC
# ---------------------------------------------
!if "$(BROWSE)" == "1"
	@echo CAFE driver: creating BSC...
	@$(BSC) @<<
		$(BSC_FLAGS) $(SBRFILES)
<<
!endif

# ---------------------------------------------
# object file targets
# ---------------------------------------------
!if exist(auto.dep)
!include auto.dep
!else
!message Note: no AUTO.DEP currently exists
!endif

$(OBJDIR)\cafe.res:            \
	cafe.rc                    \
	caferes.h                  \
	res\CAFE.ICO               \
	res\UTENSILX.ICO           \
	res\PORTDOC.ICO            \
	res\cafebar.BMP            \
	res\fldrcls.bmp            \
	res\fldropen.bmp           \
	res\glyphcol.bmp           \
	res\glyphexp.bmp           \
	res\glyphtst.bmp           \
	res\suitebar.bmp           \
	res\vwprtbar.bmp           \
	res\subsuite.bmp           \
	res\cafe.rc2
	@echo CAFE driver: compiling resources...
	@$(RC) $(RC_FLAGS) cafe.rc

# ---------------------------------------------
# create object file directory
# ---------------------------------------------
$(OBJDIR):
	@if not exist $(OBJDIR)\nul mkdir $(OBJDIR)

# ---------------------------------------------
# ssync
# ---------------------------------------------
ssync: ssync.depend
	@echo CAFE driver: ssyncing...
	-ssync -f
	@cd $(CAFE_INCLUDE)
	-ssync -fr
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency ssync
# ---------------------------------------------
ssync.depend:
!if "$(DEPEND)" == "1"
	@echo CAFE driver: ssynching dependencies...
# SubSuite
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) ssync
	@cd $(MAKEDIR)
# Toolset
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) ssync
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# slmout
# ---------------------------------------------
slmout: slmout.depend
	@echo CAFE driver: checking out...
	@cd $(EXEDIR)
	-out -f $(TARGET).exe $(TARGET).pdb
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmout
# ---------------------------------------------
slmout.depend:
!if "$(DEPEND)" == "1"
	@echo CAFE driver: checking out dependencies...
# SubSuite
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmout
	@cd $(MAKEDIR)
# Toolset
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmout
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# slmin
# ---------------------------------------------
slmin: slmin.depend
	@echo CAFE driver: checking in...
	@cd $(EXEDIR)
	-in -c "Automated check-in" -f $(TARGET).exe $(TARGET).pdb
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmin
# ---------------------------------------------
slmin.depend:
!if "$(DEPEND)" == "1"
	@echo CAFE driver: checking in dependencies...
# SubSuite
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmin
	@cd $(MAKEDIR)
# Toolset
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmin
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# clean
# ---------------------------------------------
clean: clean.depend
	@echo CAFE driver: cleaning...
	@-if exist $(OBJDIR)\*.obj erase $(OBJDIR)\*.obj
	@-if exist $(OBJDIR)\*.pch erase $(OBJDIR)\*.pch
	@-if exist $(OBJDIR)\*.sbr erase $(OBJDIR)\*.sbr
	@-if exist $(OBJDIR)\*.pdb erase $(OBJDIR)\*.pdb
	@-if exist $(OBJDIR)\*.res erase $(OBJDIR)\*.res
	@-if exist $(OBJDIR)\*.bsc erase $(OBJDIR)\*.bsc
	@-if exist $(OBJDIR)\*.idb erase $(OBJDIR)\*.idb
	@-if exist $(OBJDIR) rmdir $(OBJDIR)
	@-if exist $(EXEDIR)\$(TARGET).* erase $(EXEDIR)\$(TARGET).*
	@-if exist $(LIBDIR)\$(TARGET).* erase $(LIBDIR)\$(TARGET).*

# ---------------------------------------------
# dependency clean
# ---------------------------------------------
clean.depend:
!if "$(DEPEND)" == "1"
	@echo CAFE driver: cleaning dependencies...
# SubSuite
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) clean
	@cd $(MAKEDIR)
# Toolset
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) clean
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# dependency generation
# ---------------------------------------------
auto.dep: auto.dep.depend
    @attrib -r auto.dep
    @mkdep  -I $(CAFE_INCLUDE) -P $$(OBJDIR)\ -s.obj -n *.cpp >auto.dep

# ---------------------------------------------
# dependencies for dependency generation
# ---------------------------------------------
auto.dep.depend: nul
!if "$(DEPEND)" == "1"
	@echo CAFE driver: Updating dependency files...
# SubSuite
	@cd $(CAFE_SUBSUITE)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) auto.dep
	@cd $(MAKEDIR)
# Toolset
	@cd $(CAFE_TOOLSET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) auto.dep
	@cd $(MAKEDIR)
!endif


# ---------------------------------------------
# PCH file generation
# ---------------------------------------------
$(OBJDIR)\$(TARGET).pch: stdafx.cpp stdafx.h
	@echo CAFE driver: compiling...
	@$(CPP) $(CPP_CREATE_PCH_FLAGS) stdafx.cpp

