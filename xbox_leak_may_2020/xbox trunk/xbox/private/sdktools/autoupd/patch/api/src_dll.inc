
#
#  Must set TARGETNAME and DLLBASE before including this file
#

TARGETTYPE=DYNLINK

UMTYPE=windows

TARGETLIBS=$(BASEDIR)\public\sdk\lib\*\kernel32.lib \
           $(SDK_LIB_PATH)\user32.lib \
           ..\..\lzx\encoder\obj\*\lzxpc.lib \
           ..\..\lzx\decoder\obj\*\lzxpd.lib

C_DEFINES=$(C_DEFINES) -DBUILDING_PATCHAPI_DLL=1

SOURCES=$(SOURCES) $(TARGETNAME).rc

#
#  This is a hacky way to get the PE file version stamp to be something
#  other than what NT's makefile.def tries to force it to be.  We're assuming
#  here that DLLBASE gets used in the linker command line AFTER makefile.def
#  inserts its own -version option.
#

DLLBASE=$(DLLBASE) -version:5.2

#
#  To avoid dragging in bloated c-runtime exception handling code, we build
#  our own scaled down version for x86 only.  We have to include our own
#  handler for x86 because we can't link with msvcrt.dll since it isn't
#  available on Win95 systems.  For non-x86 platforms, we can use ntdll.dll
#  directly, which also buys us the imports for intrinsic memory operations
#  without pulling in msvcrt.dll.
#

i386_SOURCES=..\i386\exsup.asm  \
             ..\i386\exsup3.asm

!if "$(TARGET_DIRECTORY)" != "i386"
!undef NO_NTDLL
USE_NTDLL=1
!endif

LINKER_FLAGS=$(LINKER_FLAGS) /OPT:NOWIN98 /PDBPATH:NONE

NTTARGETFILES=DELEXPFILE
