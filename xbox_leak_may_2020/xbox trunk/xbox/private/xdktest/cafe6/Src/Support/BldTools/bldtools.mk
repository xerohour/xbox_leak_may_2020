# Bldtools DLL build
#
# Usage:
#
# nmake /f bldtools.mk [all]        Build the DLL
# nmake /f bldtools.mk ssync        Ssync the files needed to build this DLL
# nmake /f bldtools.mk slmout       Check out the files that are built by this makefile
# nmake /f bldtools.mk slmin        Check in the files that are built by this makefile
# nmake /f bldtools.mk clean        Clean the project
# nmake /f bldtools.mk auto.dep			Generate dependencies file (auto.dep)
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
CAFE_INCLUDE=..\..\..\include
CAFE_TARGET=..
CAFE_GUI=..\guitools
CAFE_IDE=..\guitools\ide
CAFE_SYM=..\guitools\ide\sym
CAFE_SHL=..\guitools\ide\shl
CAFE_EDS=..\guitools\ide\eds
CAFE_PRJ=..\guitools\ide\prj
CAFE_DBG=..\guitools\ide\dbg
CAFE_TOOLSET=..
CAFE_LOG=..\..\log

EXEDIR=..\..\..\bin
LIBDIR=..\..\..\lib

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
!if "$(DEBUG)" == "0"
OBJDIR=WinRel
TARGET=bldtools
CPP_FLAGS=-Od -DNDEBUG -MD
LINK_FLAGS=-incremental:no
!elseif "$(DEBUG)" == "1"
OBJDIR=WinDebug
TARGET=bldtoold
CPP_FLAGS=-Zi -Od -D_DEBUG -MDd
LINK_FLAGS=-incremental:yes -debug
!else
OBJDIR=WinRel
TARGET=bldtools
CPP_FLAGS=-Zi -O2 -DNDEBUG -MD
LINK_FLAGS=-incremental:no -debug
!endif

# ---------------------------------------------
# set up platform-specific defines
# ---------------------------------------------
!if "$(PLATFORM)" == "x86"
!if "$(DEBUG)" != "0"
!message Build tools: MR enabled
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
CPP_FLAGS=$(CPP_FLAGS) -nologo -W3 -GR -GX -DWIN32 -D_WINDOWS -D_AFXDLL -D_MBCS -D_AFXEXT -DEXPORT_BLDTOOLS -Fp$(OBJDIR)\$(TARGET).pch -Fo$(OBJDIR)^\ -Fd$(OBJDIR)\$(TARGET).pdb -c
CPP_CREATE_PCH_FLAGS=$(CPP_FLAGS) -Ycstdafx.h
CPP_FLAGS=$(CPP_FLAGS) -Yustdafx.h
LINK_FLAGS=$(LINK_FLAGS) -nologo -subsystem:windows -dll -out:$(EXEDIR)\$(TARGET).dll -pdb:$(EXEDIR)\$(TARGET).pdb -implib:$(LIBDIR)\$(TARGET).lib
RC_FLAGS=/l 0x409 /fo$(OBJDIR)\bldtools.res /dNDEBUG /d_AFXDLL
BSC_FLAGS=$(BSC_FLAGS) -nologo
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
	$(OBJDIR)\compiler.obj \
	$(OBJDIR)\stdafx.obj   \
	$(OBJDIR)\bldtools.res \
	$(OBJDIR)\comppage.obj \
	$(OBJDIR)\bldtools.obj \
	$(OBJDIR)\execman.obj  \
	$(OBJDIR)\execpage.obj

# ---------------------------------------------
# define sbrs
# ---------------------------------------------
SBRFILES= \
	$(OBJDIR)\compiler.sbr \
	$(OBJDIR)\stdafx.sbr   \
	$(OBJDIR)\comppage.sbr \
	$(OBJDIR)\bldtools.sbr \
	$(OBJDIR)\execman.sbr  \
	$(OBJDIR)\execpage.sbr


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

all: dependencies $(EXEDIR)\$(TARGET).dll

# ---------------------------------------------
# build dependencies
# ---------------------------------------------
dependencies:
!if "$(DEPEND)" == "1"
	@echo Build tools: building dependencies...
# Target DLL
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND)
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND)
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# build target DLL
# ---------------------------------------------
$(EXEDIR)\$(TARGET).dll: $(OBJDIR) $(OBJDIR)\$(TARGET).pch $(OBJFILES)
	@echo Build tools: linking DLL...
	@-if exist $(EXEDIR)\$(TARGET).dll attrib -r $(EXEDIR)\$(TARGET).dll
	@-if exist $(EXEDIR)\$(TARGET).pdb attrib -r $(EXEDIR)\$(TARGET).pdb
	@-if exist $(LIBDIR)\$(TARGET).lib attrib -r $(LIBDIR)\$(TARGET).lib
	@$(LINKER) @<<
		$(LINK_FLAGS) $(OBJFILES)
<<
# ---------------------------------------------
# build BSC
# ---------------------------------------------
!if "$(BROWSE)" == "1"
	@echo Build tools: creating BSC...
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

$(OBJDIR)\bldtools.res:         \
	bldtools.rc
	@echo Build tools: compiling resources...
	@$(RC) $(RC_FLAGS) bldtools.rc

# ---------------------------------------------
# create object file directory
# ---------------------------------------------
$(OBJDIR):
	@if not exist $(OBJDIR)\nul mkdir $(OBJDIR)

# ---------------------------------------------
# ssync
# ---------------------------------------------
ssync: ssync.depend
	@echo Build tools: ssyncing DLL...
	-ssync -f
	@cd $(CAFE_INCLUDE)
	-ssync -fr
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency ssync
# ---------------------------------------------
ssync.depend:
!if "$(DEPEND)" == "1"
	@echo Build tools: ssynching dependencies...
# Target
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) ssync
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) ssync
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# slmout
# ---------------------------------------------
slmout: slmout.depend
	@echo Build tools: checking out DLL...
	@cd $(EXEDIR)
	-out -f $(TARGET).dll $(TARGET).pdb
	@cd $(MAKEDIR)
	@cd $(LIBDIR)
	-out -f $(TARGET).lib
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmout
# ---------------------------------------------
slmout.depend:
!if "$(DEPEND)" == "1"
	@echo Build tools: checking out dependencies...
# Target
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmout
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmout
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# slmin
# ---------------------------------------------
slmin: slmin.depend
	@echo Build tools: checking in DLL...
	@cd $(EXEDIR)
	-in -c "Automated check-in" -f $(TARGET).dll $(TARGET).pdb
	@cd $(MAKEDIR)
	@cd $(LIBDIR)
	-in -c "Automated check-in" -f $(TARGET).lib
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmin
# ---------------------------------------------
slmin.depend:
!if "$(DEPEND)" == "1"
	@echo Build tools: checking in dependencies...
# Target
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmin
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) slmin
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# clean
# ---------------------------------------------
clean: clean.depend
	@echo Build tools: cleaning DLL...
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
	@echo Build tools: cleaning dependencies...
# Target
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) clean
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) clean
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
	@echo BldTools: Updating dependency files...
# Target
	@cd $(CAFE_TARGET)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) auto.dep
	@cd $(MAKEDIR)
# Log
	@cd $(CAFE_LOG)
	@$(MAKE) -nologo -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) PLATFORM=$(PLATFORM) DEPEND=$(DEPEND) auto.dep
	@cd $(MAKEDIR)
!endif

# ---------------------------------------------
# PCH file generation
# ---------------------------------------------
$(OBJDIR)\$(TARGET).pch: stdafx.cpp stdafx.h
	@echo BldTools: compiling...
	@$(CPP) $(CPP_CREATE_PCH_FLAGS) stdafx.cpp
