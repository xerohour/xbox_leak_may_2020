!IF 0

Copyright (c) 1989-1999 Microsoft Corporation

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

!ENDIF

!include ..\..\sources.inc

MAJORCOMP=windows
MINORCOMP=directx

TARGETNAME=dmusic_core$(LIBEXT)

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

C_DEFINES= $(C_DEFINES) -D_WIN32 -D_DMUSIC_USER_MODE_ -DXBOX -DUNICODE

!if defined(INTERNAL)
C_DEFINES= $(C_DEFINES) -DDMUSIC_INTERNAL
!endif

USE_LIBCMT=1

#

INCLUDES= \
        ..\..\..\misc;  \
        ..\..\shared;   \
        ..\..\includes; \
        ..\..\xprivate; \
        ..

MSC_WARNING_LEVEL=/W3 /WX

SOURCES=\
alist.cpp   \
debug.cpp           \
dmart.cpp           \
dmbuffer.cpp        \
dmclock.cpp         \
dmcollec.cpp        \
dmdlinst.cpp        \
dmdll.cpp           \
dmdload.cpp         \
dminsobj.cpp        \
dminstru.cpp        \
dmportdl.cpp        \
dmregion.cpp        \
dmsport.cpp         \
dmsysclk.cpp        \
dmusic.cpp          \
dmwavobj.cpp        \
dmsport8.cpp        \
dswave.cpp          \
dmvoice.cpp         \
dmdsclk.cpp         \
dmport.cpp			\
sysaudio.cpp




