!IF 0

Copyright (c) 1996-1999 Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.


Author:

    Jim Geist (JimGe) 29-Aug-97

NOTE:   Commented description of this file is in \nt\oak\bin\sources.tpl

NOTE:	The file ddksources in this directory structure is used to build the
        DDK sythesizer sample.  If you make changes to this file, make sure
        to keep ddksources in sync as well.  See ddkreadme.txt for more info.

!ENDIF

!include ..\..\sources.inc

MAJORCOMP=windows
MINORCOMP=directx

TARGETNAME=dmsynth$(LIBEXT)

!IF !$(FREEBUILD) && defined( DIRECTX_REDIST )
TARGETNAME=$(TARGETNAME)d
!ENDIF


TARGETTYPE = LIBRARY
TARGETPATH = ..\..\obj
USE_MAPSYM=1

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"

!if "$(BuildMode)" == "internal"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!else
C_DEFINES = $(C_DEFINES) -DRDEBUG=1
!endif

!endif

C_DEFINES= $(C_DEFINES) -D_WIN32 -DREVERB_ENABLED -D_DMUSIC_USER_MODE_ -D_MT -DXBOX -DUNICODE

USE_LIBCMT=1

INCLUDES= $(INCLUDES); \
        ..\..\xprivate;   \
        ..\..\shared; \
        ..

SOURCES=\
clist.cpp           \
control.cpp         \
csynth.cpp          \
instr.cpp           \
voice.cpp           \
guids_dmsynth.cpp   \
wave.cpp            \
dls.cpp             \
dmrand.cpp


# XMIX is (DSound-based) hardware mixing for Xbox
!if !defined(SILVER) && !defined(NOXMIX)

C_DEFINES = $(C_DEFINES) -DXMIX

INCLUDES= $(INCLUDES); \
    $(BASEDIR)\private\ntos\inc;\
    $(BASEDIR)\private\windows\directx\dsound\dsound;\
    $(BASEDIR)\private\windows\directx\dsound\ac97;\
    $(BASEDIR)\private\windows\directx\dsound\tools\inc

SOURCES = $(SOURCES) \
dsoundsequencer.cpp
!endif

