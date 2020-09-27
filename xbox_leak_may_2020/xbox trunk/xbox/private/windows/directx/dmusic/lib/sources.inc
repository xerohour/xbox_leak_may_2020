!IF 0

Copyright (c) 2001  Microsoft Corporation

Module Name:

    sources.inc

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

    This file assumes that LIBEXT has been set to blank, 'd', or 'i',
    as apropriate.

!ENDIF

!include ..\..\sources.inc

MAJORCOMP=windows
MINORCOMP=directx

TARGETTYPE=LIBRARY
TARGETNAME=dmusic$(LIBEXT)
TARGETPATH=$(BASEDIR)\public\sdk\lib

NTTARGETFILES=\
    $(XDK_LIB_PATH)\$(TARGETNAME).lib


OBJLIBFILES= \
		..\..\obj\*\dmband$(LIBEXT).lib                         \
		..\..\obj\*\dmcompos$(LIBEXT).lib                       \
		..\..\obj\*\dmime$(LIBEXT).lib                          \
		..\..\obj\*\dmloader$(LIBEXT).lib                       \
		..\..\obj\*\dmscript$(LIBEXT).lib                       \
		..\..\obj\*\dmstyle$(LIBEXT).lib                        \
		..\..\obj\*\dmsynth$(LIBEXT).lib                        \
		..\..\obj\*\dmguids$(LIBEXT).lib                        \

# Make sure the libs get built before we try to do this
SYNCHRONIZE_DRAIN=1

SOURCES=
