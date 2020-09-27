!include ..\..\sources.inc

TARGETNAME=enet
TARGETPATH=obj
TARGETTYPE=LIBRARY

INCLUDES=..\;$(INCLUDES)

PRECOMPILED_INCLUDE=..\precomp.h

SOURCES=\
    ..\arp.c \
    ..\enet.c \
    ..\i82558.c \
    ..\xnic.c


