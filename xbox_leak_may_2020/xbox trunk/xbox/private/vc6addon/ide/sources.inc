#
# sources.inc
#
# Kent Forschmiedt (kentf) 11/30/95
#
# This is the global sources file for the MSVC project.
# The environment variable VCROOT must be defined as the
# directory where this file is found.
# The following environment variables may be defined, and will
# be assigned reasonable values if they are not:
#
# LANGAPI           $(VCROOT)\langapi
# CRTROOT
# CRT_INCLUDES      $(CRTROOT)\crtw32\h
# CRTLIB_DIR

VCROOT=$(BASEDIR)\private\vc6addon

#
# Pick up master directory definitions
#

!include $(VCROOT)\sources.inc

#
# Default settings:
#

DBC=1
TEST=1
#OSDEBUG4=1

!if $(FREEBUILD)
DEBUG=0
!else
DEBUG=1
!endif


!if "$(OLE)"!="0"
OLE=1
!endif


#
# if RCTOOL is not defined, default to "rc"
# this allows the use a different rc from NT-J
#

!if "$(RCTOOL)" == ""
RCTOOL=rc
!endif

#
# Standard includes:
#

DBGINCLUDES=$(LANGAPI_ROOT)\debugger

IDE_INCLUDES=$(IDE_ROOT)\include;$(DBGINCLUDES);$(VC_INCLUDES);$(IDE_ROOT)\idl;$(LANGAPI)\ncb

#
# compiler defines
#

C_DEFINES = $(C_DEFINES) \
            -DSHIP -D_SUSHI -DNEW_PROJ_VIEW -DCODEVIEW -DPPC_PLATFORM  \
            -DHOST32 -DTARGET32 -DADDR_MIXED -DCROSS_PLATFORM          \
            -D_WINDOWS -DNT_BUILD -D_NTWIN -DSTRICT -DBORDER_BUTTONS\
            -D__DEVHELP98__ -DCC_MULTIPLE_STORES=1

#
# Is this a ship build, or are chickens made of cheese?
#

!if "$(_SHIP)" == "1"
!message Building _SHIPping version, really, honest.
C_DEFINES = $(C_DEFINES) -D_SHIP
!endif

!ifdef OSDEBUG4
C_DEFINES=$(C_DEFINES) -DOSDEBUG4
!endif

!if "$(DEBUG)" == "1"
D=D
CFGNAME=Win32 Debug
C_DEFINES = $(C_DEFINES) -DDEBUG=1
PACKVERCHECK=$(IDE_ROOT)\include\$(TARGET_DIRECTORY)\pkgverd.h
MKTYPLIB_FLAGS=$(MKTYPLIB_FLAGS) /D_DEBUG
MSC_OPTIMIZATION=/Od
!else
D=
CFGNAME=Win32 Release
C_DEFINES = $(C_DEFINES) -DNDEBUG
PACKVERCHECK=$(IDE_ROOT)\include\$(TARGET_DIRECTORY)\pkgver.h
!endif

!if "$(DBC)" == "1"
# !message Building DBC enabled version!
# !message adding-D_DBCS=1 -D_MBCS to compile flags
C_DEFINES = $(C_DEFINES) -D_DBCS=1 -D_MBCS
!endif

!if "$(TEST)"=="1"
C_DEFINES=$(C_DEFINES) -D_TEST
!endif

!ifdef DMALLOC
C_DEFINES = $(C_DEFINES) -DUSE_DMALLOC
!endif

!if "$(SPY)" == "1"
C_DEFINES = $(C_DEFINES) -DSPY
!endif

#
# Misc compile and linker options
#
NOT_LEAN_AND_MEAN = 1
386_STDCALL=0

MIPS_FLAGS = -DHOST_MIPS
386_FLAGS  = -DHOST_i386 -Dx86
ALPHA_FLAGS= -DHOST_ALPHA
PPC_FLAGS  = -DHOST_PPC

#
# Every dll here (?) has a DllMain
#
DLLENTRY=_DllMainCRTStartup
