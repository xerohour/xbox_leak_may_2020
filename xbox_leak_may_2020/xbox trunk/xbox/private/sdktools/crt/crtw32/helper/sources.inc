MAJORCOMP=crt
MINORCOMP=helper

TARGETNAME=helper

!INCLUDE ..\..\crt32.inc

C_DEFINES= $(C_DEFINES) -D_MIPSEL -D_WIN32

SOURCES=..\empty.c

i386_SOURCES= \
    ..\i386\lldiv.asm  \
    ..\i386\lldvrm.asm \
    ..\i386\llmul.asm  \
    ..\i386\llrem.asm  \
    ..\i386\llshl.asm  \
    ..\i386\llshr.asm  \
    ..\i386\ulldiv.asm \
    ..\i386\ulldvrm.asm \
    ..\i386\ullrem.asm \
    ..\i386\ullshr.asm
