MAJORCOMP=crt
MINORCOMP=direct

TARGETNAME=direct

!INCLUDE ..\..\crt32.inc

# drivfree is gone
# seterrm is gone

SOURCES=\
        ..\drivemap.c \
        ..\findf64.c  \
        ..\findfile.c \
        ..\findfi64.c \
        ..\slbeep.c   \
        ..\wfindfil.c \
        ..\wfndf64.c  \
        ..\wfndfi64.c

i386_SOURCES=\
        ..\i386\enable.asm
