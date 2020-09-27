MSC_WARNING_LEVEL=/W3 /WX

INCLUDES=..;$(BASEDIR)\private\inc\crypto

SOURCES_COMMON=\
    ..\secure.c \
    ..\protocol.c\
    ..\filexfer.c\
    ..\fileapi.c\
    ..\xbconn.cpp\
    ..\threadbrk.c\
    ..\counters.c\
    ..\xbeutil.c

PRECOMPILED_INCLUDE=..\precomp.h
PRECOMPILED_PCH=..\precomp.pch
PRECOMPILED_OBJ=..\precomp.obj
