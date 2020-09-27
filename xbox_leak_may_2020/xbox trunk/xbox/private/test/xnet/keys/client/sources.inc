!IF 0

Copyright (c) Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF



TARGETPATH=obj
TARGETTYPE=PROGRAM

UMTYPE=xboxapp

PRECOMPILED_INCLUDE=..\precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj
PRECOMPILED_CXX=1

USECXX_FLAG=/Tp

INCLUDES=\
    $(INCLUDES); \
    ..\; \
    ..\..\; \

TARGETLIBS=\
    $(LIBRARY_PATH)\xnet$(S)$(D).lib \
    $(BASEDIR)\private\test\lib\*\xtestlib.lib    \
    $(BASEDIR)\private\test\lib\*\xmem.lib        \
    $(BASEDIR)\private\test\lib\*\xnetref$(D).lib \
    $(BASEDIR)\private\test\lib\*\xlog.lib        \
    $(BASEDIR)\private\test\lib\*\netsync.lib     \
    ..\..\lib\*\xboxkeys.lib                      \

SOURCES=\
    main.c      \
