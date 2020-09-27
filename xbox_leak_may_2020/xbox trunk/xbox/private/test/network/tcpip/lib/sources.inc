!IF 0

Copyright (c) 2000 Microsoft Corporation

Module Name:

    sources

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

Author:

    Steven Kehrli (steveke) 13-Mar-2000

Environment:

    XBox

Revision History:

  24-Apr-2000  steveke

      Build for xbox and nt.

  04-Apr-2000  steveke

      Removed xtestlib dependency.

!ENDIF



TARGETTYPE=DYNLINK
TARGETPATH=$(BASEDIR)\private\test\lib

DLLENTRY=DllMain

PRECOMPILED_INCLUDE=..\precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj

INCLUDES=\
    $(INCLUDES); \
    $(BASEDIR)\private\test\inc; \
    $(BASEDIR)\private\ntos\inc; \
    $(BASEDIR)\private\ntos\ke;  \
    $(BASEDIR)\private\ntos\ex;  \
    ..\;                         \

SOURCES=\
    ..\xlogging.rc \
    ..\dllmain.c   \
    ..\util.c      \
    ..\xlogging.c  \

