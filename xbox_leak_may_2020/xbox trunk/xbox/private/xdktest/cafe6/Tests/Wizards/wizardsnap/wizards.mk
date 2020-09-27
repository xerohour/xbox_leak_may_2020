# Wizards Snap build
#
# Usage:
#
# nmake /f wizards.mk [all]         Build the DLL
# nmake /f wizards.mk clean         Clean the project
#
#
# Additional command line defines: * indicates default
#
# DEBUG=	[ 0* | 1 | 2 ]			Build debug (0), retail (1), or retail with debug info (2)
# BROWSE=	[ 0* | 1 ]				Build with browse information (1)
# PLATFORM= [ X86*, MIPS, ALPHA ]	Specify the platform
# DEPEND=   [ 0* | 1 ]				Indicate whether to include dependent projects (1) or not (0)
# EXEDIR=   directory for DLL		Specify the directory for the DLL
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
!message DEPEND not defined; defaulting to DEPEND=0
DEPEND=0
!endif
!ifndef PLATFORM
!ifndef PROCESSOR_ARCHITECTURE
!message PLATFORM or PROCESSOR_ARCHITECTURE not defined; defaulting to PLATFORM=x86
PLATFORM=x86
!else
PLATFORM=$(PROCESSOR_ARCHITECTURE)
!endif
!endif
!ifndef CAFEDIR
!message CAFEDIR not defined; defaulting to CAFEDIR=\cafe
CAFEDIR=\cafe
!endif

# ---------------------------------------------
# define relative CAFE directories
# ---------------------------------------------
CAFE_INCLUDE=$(CAFEDIR)\v3\include
CAFE_TARGET=$(CAFEDIR)\v3\support
CAFE_GUI=$(CAFEDIR)\v3\support\guitools
CAFE_IDE=$(CAFEDIR)\v3\support\guitools\ide
CAFE_SYM=$(CAFEDIR)\v3\support\guitools\ide\sym
CAFE_SHL=$(CAFEDIR)\v3\support\guitools\ide\shl
CAFE_EDS=$(CAFEDIR)\v3\support\guitools\ide\eds
CAFE_PRJ=$(CAFEDIR)\v3\support\guitools\ide\prj
CAFE_DBG=$(CAFEDIR)\v3\support\guitools\ide\dbg
CAFE_TOOLSET=$(CAFEDIR)\v3\support
CAFE_SUBSUITE=$(CAFEDIR)\v3\subsuite

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
TARGET=wizsnap
CPP_FLAGS=-O2 -DNDEBUG
LINK_FLAGS=-incremental:no
!elseif "$(DEBUG)" == "1"
OBJDIR=WinDebug
TARGET=wizsnapd
CPP_FLAGS=-Zi -Od -D_DEBUG
LINK_FLAGS=-incremental:yes -debug
!else
OBJDIR=WinRel
TARGET=wizsnap
CPP_FLAGS=-Zi -O2 -DNDEBUG
LINK_FLAGS=-incremental:no -debug
!endif

!ifndef EXEDIR
EXEDIR=$(OBJDIR)
!endif

# ---------------------------------------------
# set up platform-specific defines
# ---------------------------------------------
!if "$(PLATFORM)" == "x86"
!if "$(DEBUG)" != "0"
!message WizSnap: MR enabled
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
CPP_FLAGS=$(CPP_FLAGS) -nologo -MD -W3 -GR -GX -DWIN32 -D_WINDOWS -D_AFXDLL -D_MBCS -D_AFXEXT -Fp$(OBJDIR)\$(TARGET).pch -Fo$(OBJDIR)^\ -Fd$(OBJDIR)\$(TARGET).pdb -c
CPP_CREATE_PCH_FLAGS=$(CPP_FLAGS) -Ycstdafx.h
CPP_FLAGS=$(CPP_FLAGS) -Yustdafx.h
LINK_FLAGS=$(LINK_FLAGS) -nologo -subsystem:windows -dll -out:$(EXEDIR)\$(TARGET).dll -pdb:$(EXEDIR)\$(TARGET).pdb
RC_FLAGS=/l 0x409 /fo$(OBJDIR)\wizsnap.res /dNDEBUG /d_AFXDLL
BSC_FLAGS=$(BSC_FLAGS) -nologo

# ---------------------------------------------
# define objs
# ---------------------------------------------
OBJFILES= \
	$(OBJDIR)\wizsnap.res  \
	$(OBJDIR)\appsnap.obj  \
	$(OBJDIR)\clwsnap.obj  \
	$(OBJDIR)\ctrsnap.obj  \
	$(OBJDIR)\sniff.obj    \
	$(OBJDIR)\stdafx.obj

# ---------------------------------------------
# define sbrs
# ---------------------------------------------
SBRFILES= \
	$(OBJDIR)\appsnap.sbr  \
	$(OBJDIR)\clwsnap.sbr  \
	$(OBJDIR)\ctrsnap.sbr  \
	$(OBJDIR)\sniff.sbr    \
	$(OBJDIR)\stdafx.sbr


# ---------------------------------------------
# ---------------------------------------------
# rules
# ---------------------------------------------

.c{$(OBJDIR)}.obj:
   @$(COMPILER) $(CPP_FLAGS) $<

.cpp{$(OBJDIR)}.obj:
   @$(COMPILER) $(CPP_FLAGS) $<

.cxx{$(OBJDIR)}.obj:
   @$(COMPILER) $(CPP_FLAGS) $<


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
!endif

# ---------------------------------------------
# build target DLL
# ---------------------------------------------
$(EXEDIR)\$(TARGET).dll: $(OBJDIR) $(OBJFILES)
	@echo WizSnap: linking DLL...
	@-if exist $(EXEDIR)\$(TARGET).dll attrib -r $(EXEDIR)\$(TARGET).dll
	@$(LINKER) @<<
		$(LINK_FLAGS) $(OBJFILES)
<<
# ---------------------------------------------
# build BSC
# ---------------------------------------------
!if "$(BROWSE)" == "1"
	@echo WizSnap: creating BSC...
	@$(BSC) @<<
		$(BSC_FLAGS) $(SBRFILES)
<<
!endif

# ---------------------------------------------
# object file targets
# ---------------------------------------------

$(OBJDIR)\appsnap.obj:         \
	appsnap.cpp                \
	appsnap.h                  \
	stdafx.h                   \
	$(OBJDIR)\stdafx.obj

$(OBJDIR)\clwsnap.obj:         \
	clwsnap.cpp                \
	clwsnap.h                  \
	stdafx.h                   \
	$(OBJDIR)\stdafx.obj

$(OBJDIR)\ctrsnap.obj:         \
	ctrsnap.cpp                \
	ctrsnap.h                  \
	stdafx.h                   \
	$(OBJDIR)\stdafx.obj

$(OBJDIR)\sniff.obj:           \
	sniff.cpp                  \
	sniff.h                    \
	stdafx.h                   \
	appsnap.h                  \
	clwsnap.h                  \
	ctrsnap.h                  \
	$(OBJDIR)\stdafx.obj

$(OBJDIR)\stdafx.obj:          \
	stdafx.cpp                 \
	stdafx.h                   \
	$(CAFE_INCLUDE)\test.h     \
	$(CAFE_INCLUDE)\subsuite.h \
	$(CAFE_INCLUDE)\support.h
	@echo WizSnap: compiling DLL...
	@$(CPP) $(CPP_CREATE_PCH_FLAGS) stdafx.cpp

$(OBJDIR)\wizsnap.res:         \
	wizsnap.rc                 \
	resource.h
	@echo WizSnap: compiling resources...
	@$(RC) $(RC_FLAGS) wizsnap.rc

# ---------------------------------------------
# create object file directory
# ---------------------------------------------
$(OBJDIR):
	@if not exist $(OBJDIR)\nul mkdir $(OBJDIR)

# ---------------------------------------------
# clean
# ---------------------------------------------
clean: clean.depend
	@echo WizSnap: cleaning DLL...
	@-if exist $(OBJDIR)\*.obj erase $(OBJDIR)\*.obj
	@-if exist $(OBJDIR)\*.pch erase $(OBJDIR)\*.pch
	@-if exist $(OBJDIR)\*.sbr erase $(OBJDIR)\*.sbr
	@-if exist $(OBJDIR)\*.pdb erase $(OBJDIR)\*.pdb
	@-if exist $(OBJDIR)\*.res erase $(OBJDIR)\*.res
	@-if exist $(OBJDIR)\*.bsc erase $(OBJDIR)\*.bsc
	@-if exist $(OBJDIR)\*.ilk erase $(OBJDIR)\*.ilk
	@-if exist $(OBJDIR)\*.lib erase $(OBJDIR)\*.lib
	@-if exist $(OBJDIR)\*.exp erase $(OBJDIR)\*.exp
	@-if exist $(OBJDIR)\*.sym erase $(OBJDIR)\*.sym
	@-if exist $(OBJDIR)\*.dll erase $(OBJDIR)\*.dll
	@-if exist $(OBJDIR)\*.log erase $(OBJDIR)\*.log
	@-if exist $(OBJDIR)\*.txt erase $(OBJDIR)\*.txt
	@-if exist $(OBJDIR) rmdir $(OBJDIR)
	@-if exist $(EXEDIR)\$(TARGET).dll erase $(EXEDIR)\$(TARGET).dll

# ---------------------------------------------
# dependency clean
# ---------------------------------------------
clean.depend:
!if "$(DEPEND)" == "1"
!endif
