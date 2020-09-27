!IF 0

Common sources file information.  The variable COMMON_SOURCES is defined
here to contain the source file names that are common across all platforms.

!ENDIF

DLLENTRY=_DllMainCRTStartup

!IF "$(NTNOPCH)" == ""
PRECOMPILED_INCLUDE=precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj
!ENDIF

SUBSYSTEM_VERSION=5.00

COMMON_SOURCES=    \
    ..\usbtree.cpp \
    ..\ohcd.cpp    \
    ..\dbgprint.c  \
    ..\device.c    \
    ..\driver.c    \
    ..\exceptn.c   \
    ..\fscache.c   \
    ..\gentable.c  \
    ..\gflag.c     \
    ..\handle.c    \
    ..\heap.c      \
    ..\image.c     \
    ..\iospace.c   \
    ..\irp.c       \
    ..\lists.c     \
    ..\mmx.c       \
    ..\object.c    \
    ..\patch.c     \
    ..\patchbld.c  \
    ..\physical.c  \
    ..\pool.c      \
    ..\process.c   \
    ..\ptov.c      \
    ..\ready.c     \
    ..\stacks.c    \
    ..\time.c      \
    ..\util.c      \
    ..\vad.c       \
    ..\kdexts.c    \
    ..\kdexts.rc   \
    help.c         \
    pcr.c          \
    trap.c         \
    cxr.c          \
    exsup.c

INCLUDES=\
    ..\;\
    $(BASEDIR)\private\inc;\
    $(BASEDIR)\public\ddk\inc\wdm; \
    $(BASEDIR)\private\ntos\inc;\
    $(BASEDIR)\private\ntos\cache;\
    $(BASEDIR)\private\ntos\cntfs;\
    $(BASEDIR)\private\ntos\config;\
    $(BASEDIR)\private\ntos\io;\
    $(BASEDIR)\private\ntos\ke;\
    $(BASEDIR)\private\ntos\mmx;\
    $(BASEDIR)\private\ntos\se;\
    $(BASEDIR)\private\ntos\po;\
    $(BASEDIR)\private\ntos\rtl;\
    $(BASEDIR)\private\ntos\dd\pci;\
    $(BASEDIR)\private\ntos\dd\usb\inc;\
    $(BASEDIR)\private\ntos\dd\usb\usbd;\
    $(BASEDIR)\private\ntos\dd\usb\ohcd;\
    $(BASEDIR)\private\ntos\nthals\inc;\
    $(DDK_INC_PATH);\
    $(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W3 /WX

TARGETLIBS=\
    $(LIBRARY_PATH)\kernel32.lib

USE_CRTDLL=1

DLLDEF=obj\$(TARGET_DIRECTORY)\$(TARGETNAME).def

UMTYPE=windows

NTTARGETFILE0=$(DLLDEF)

SOURCES_USED=..\sources.inc

C_DEFINES=$(C_DEFINES) -D_XBOX
