!IF 0

Copyright (c) 1989  Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.


Author:

    Steve Wood (stevewo) 12-Apr-1990

NOTE:   Commented description of this file is in \nt\bak\bin\sources.tpl

!ENDIF

MAJORCOMP=ntos
MINORCOMP=mm

TARGETPATH=$(BASEDIR)\private\ntos\obj
TARGETTYPE=LIBRARY

INCLUDES=..;$(BASEDIR)\private\ntos\inc;$(BASEDIR)\private\ntos\ke;$(BASEDIR)\private\inc

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

!if defined(ARCADE)
C_DEFINES=$(C_DEFINES) -DARCADE
!endif

SOURCES=..\addrsup.c  \
        ..\debugsup.c \
        ..\fscache.c  \
        ..\mmfault.c  \
        ..\mminit.c   \
        ..\mmsup.c    \
        ..\mmsupa.asm \
        ..\pfnsup.c   \
        ..\physical.c \
        ..\sysptes.c  \
        ..\vadtree.c  \
        ..\virtual.c

PRECOMPILED_INCLUDE=..\mi.h
PRECOMPILED_PCH=mi.pch
PRECOMPILED_OBJ=mi.obj
