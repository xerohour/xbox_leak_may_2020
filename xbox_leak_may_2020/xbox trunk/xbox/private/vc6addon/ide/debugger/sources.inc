# Get all the other project paths.

VCROOT=$(BASEDIR)\private\vc6addon
#!include $(VCROOT)\sources.inc
!include $(VCROOT)\ide\sources.inc
DBG_ROOT=$(IDE_ROOT)\debugger

NOT_LEAN_AND_MEAN=
386_STDCALL = 1


DLLENTRY = _DllMainCRTStartup

MSC_WARNING_LEVEL=/WX /W3

C_DEFINES = $(C_DEFINES) -DWIN32 -DTARGET32 -DHOST32 -D_CROSS_PLATFORM_ -DOSDEBUG4

MIPS_FLAGS = -DHOST_MIPS
386_FLAGS  = -DHOST_i386
ALPHA_FLAGS= -DHOST_ALPHA
PPC_FLAGS  = -DHOST_PPC

!if $(FREEBUILD)
C_DEFINES = $(C_DEFINES) -DNDEBUG
D=
!else
C_DEFINES = $(C_DEFINES) -DDEBUGVER
D=D
!endif

DEBUG_INCLUDES=$(VC_INCLUDES);  \
               $(LANGAPI_ROOT)\debugger

DEBUG_LIB=$(IDE_ROOT)\lib
