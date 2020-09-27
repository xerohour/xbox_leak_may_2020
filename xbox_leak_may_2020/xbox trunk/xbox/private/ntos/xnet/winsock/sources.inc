!include ..\..\sources.inc

SYNCHRONIZE_DRAIN=1
C_DEFINES=$(C_DEFINES) -D_XNET_SERVICE

INCLUDES=..; \
    $(INCLUDES); \
    ..\..\tcp

PRECOMPILED_INCLUDE=..\precomp.h

SOURCES=\
    ..\sockinit.c \
    ..\socket.c \
    ..\sockopt.c \
    ..\recv.c \
    ..\send.c \
    ..\connect.c \
    ..\select.c \
    ..\sockmisc.c \
    ..\enumprot.c \
    ..\getxbyy.c

USE_NOLIBS=1

