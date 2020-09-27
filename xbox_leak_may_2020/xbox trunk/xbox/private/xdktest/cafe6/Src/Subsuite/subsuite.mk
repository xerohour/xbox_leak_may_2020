# SubSuite LIB build
#
# Usage:
#
# nmake /f subsuite.mk [all]        Build the LIB
# nmake /f subsuite.mk ssync        Ssync the files needed to build this LIB
# nmake /f subsuite.mk slmout       Check out the files that are built by this makefile
# nmake /f subsuite.mk slmin        Check in the files that are built by this makefile
# nmake /f subsuite.mk clean        Clean the project
# nmake /f subsuite.mk auto.dep     Generate dependencies file (auto.dep)
#
#
# Additional command line defines: * indicates default
#
# DEBUG=	[ 0* | 1 | 2 ]			Build debug (0), retail (1), or retail with debug info (2)
# BROWSE=	[ 0 | 1* ]				Build with browse information (1)
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
!message BROWSE not defined; defaulting to BROWSE=1
BROWSE=1
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

EXEDIR=..\..\bin
LIBDIR=..\..\lib

# ---------------------------------------------
# define toolset
# ---------------------------------------------
COMPILER=cl.exe
LINKER=link.exe
LIB=lib.exe
RC=rc.exe
BSC=bscmake.exe

# ---------------------------------------------
# set up debug/retail defines
# ---------------------------------------------
# OPTIMIZATIONS ARE TURNED OFF IN RETAIL SINCE WE GOT UTC COMPILER. (RANDOM GPFS) 

!if "$(DEBUG)" == "0"
OBJDIR=WinRel
TARGET=subsuite
CPP_FLAGS=-Od -DNDEBUG -MD
LINK_FLAGS=
!elseif "$(DEBUG)" == "1"
OBJDIR=WinDebug
TARGET=subsuitd
CPP_FLAGS=-Zi -Od -D_DEBUG -MDd
LINK_FLAGS=
!else
OBJDIR=WinRel
TARGET=subsuite
CPP_FLAGS=-Zi -O2 -DNDEBUG -MD
LINK_FLAGS=
!endif

# ---------------------------------------------
# set up platform-specific defines
# ---------------------------------------------
!if "$(PLATFORM)" == "x86"
!if "$(DEBUG)" != "0"
!message SubSuite: MR enabled
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
CPP_FLAGS=$(CPP_FLAGS) -nologo -W4 -GR -GX -DWIN32 -D_WINDOWS -D_AFXDLL -D_MBCS -Fp$(OBJDIR)\$(TARGET).pch -Fo$(OBJDIR)^\ -Fd$(LIBDIR)\$(TARGET).pdb -c
CPP_CREATE_PCH_FLAGS=$(CPP_FLAGS) -Ycstdafx.h
CPP_FLAGS=$(CPP_FLAGS) -Yustdafx.h
LINK_FLAGS=
LIB_FLAGS=-nologo -out:$(LIBDIR)\$(TARGET).lib
RC_FLAGS=/l 0x409 /fo$(OBJDIR)\subsuite.res /dNDEBUG /d_AFXDLL
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
	$(OBJDIR)\stdafx.obj   \
	$(OBJDIR)\subsuite.obj \
	$(OBJDIR)\test.obj     \
    $(OBJDIR)\rawstrng.obj \
	$(OBJDIR)\randtest.obj \
	$(OBJDIR)\vertex.obj   \
	$(OBJDIR)\set.obj

# ---------------------------------------------
# define sbrs
# ---------------------------------------------
SBRFILES= \
	$(OBJDIR)\stdafx.sbr   \
	$(OBJDIR)\subsuite.sbr \
	$(OBJDIR)\test.sbr     \
    $(OBJDIR)\rawstrng.sbr \
	$(OBJDIR)\randtest.sbr \
	$(OBJDIR)\vertex.sbr   \
	$(OBJDIR)\set.sbr


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

all: dependencies $(LIBDIR)\$(TARGET).lib

# ---------------------------------------------
# build dependencies
# ---------------------------------------------
dependencies:
!if "$(DEPEND)" == "1"
	@echo SubSuite: building dependencies...
!endif

# ---------------------------------------------
# build target LIB
# ---------------------------------------------
$(LIBDIR)\$(TARGET).lib: $(OBJDIR) $(OBJDIR)\$(TARGET).pch $(OBJFILES)
	@echo SubSuite: creating LIB...
	@-if exist $(LIBDIR)\$(TARGET).pdb attrib -r $(LIBDIR)\$(TARGET).pdb
	@-if exist $(LIBDIR)\$(TARGET).lib attrib -r $(LIBDIR)\$(TARGET).lib
	@$(LIB) @<<
		$(LIB_FLAGS) $(OBJFILES)
<<
# ---------------------------------------------
# build BSC
# ---------------------------------------------
!if "$(BROWSE)" == "1"
	@echo SubSuite: creating BSC...
	@$(BSC) @<<
		$(BSC_FLAGS) $(SBRFILES)
<<
!endif
# copy the .pdb

# ---------------------------------------------
# object file targets
# ---------------------------------------------
!if exist(auto.dep)
!include auto.dep
!else
!message Note: no AUTO.DEP currently exists
!endif

# ---------------------------------------------
# create object file directory
# ---------------------------------------------
$(OBJDIR):
	@if not exist $(OBJDIR)\nul mkdir $(OBJDIR)

# ---------------------------------------------
# ssync
# ---------------------------------------------
ssync: ssync.depend
	@echo SubSuite: ssyncing LIB...
	-ssync -f
	@cd $(CAFE_INCLUDE)
	-ssync -fr
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency ssync
# ---------------------------------------------
ssync.depend:
!if "$(DEPEND)" == "1"
	@echo SubSuite: ssynching dependencies...
!endif

# ---------------------------------------------
# slmout
# ---------------------------------------------
slmout: slmout.depend
	@echo SubSuite: checking out LIB...
	@cd $(LIBDIR)
	-out -f $(TARGET).lib $(TARGET).pdb
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmout
# ---------------------------------------------
slmout.depend:
!if "$(DEPEND)" == "1"
	@echo SubSuite: checking out dependencies...
!endif

# ---------------------------------------------
# slmin
# ---------------------------------------------
slmin: slmin.depend
	@echo SubSuite: checking in LIB...
	@cd $(LIBDIR)
	-in -c "Automated check-in" -f $(TARGET).lib $(TARGET).pdb
	@cd $(MAKEDIR)

# ---------------------------------------------
# dependency slmin
# ---------------------------------------------
slmin.depend:
!if "$(DEPEND)" == "1"
	@echo SubSuite: checking in dependencies...
!endif

# ---------------------------------------------
# clean
# ---------------------------------------------
clean: clean.depend
	@echo SubSuite: cleaning LIB...
	@-if exist $(OBJDIR)\*.obj erase $(OBJDIR)\*.obj
	@-if exist $(OBJDIR)\*.pch erase $(OBJDIR)\*.pch
	@-if exist $(OBJDIR)\*.sbr erase $(OBJDIR)\*.sbr
	@-if exist $(OBJDIR)\*.pdb erase $(OBJDIR)\*.pdb
	@-if exist $(OBJDIR)\*.res erase $(OBJDIR)\*.res
	@-if exist $(OBJDIR)\*.bsc erase $(OBJDIR)\*.bsc
	@-if exist $(OBJDIR)\*.idb erase $(OBJDIR)\*.idb
	@-if exist $(OBJDIR) rmdir $(OBJDIR)
	@-if exist $(LIBDIR)\$(TARGET).* erase $(LIBDIR)\$(TARGET).*

# ---------------------------------------------
# dependency clean
# ---------------------------------------------
clean.depend:
!if "$(DEPEND)" == "1"
	@echo SubSuite: cleaning dependencies...
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
	@echo SubSuite: Updating dependency files...
!endif

# ---------------------------------------------
# PCH file generation
# ---------------------------------------------
$(OBJDIR)\$(TARGET).pch: stdafx.cpp stdafx.h
	@echo SubSuite: compiling...
	@-if exist $(LIBDIR)\$(TARGET).pdb attrib -r $(LIBDIR)\$(TARGET).pdb
	@$(CPP) $(CPP_CREATE_PCH_FLAGS) stdafx.cpp
