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

TARGETNAME=dmime$(LIBEXT)

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

# XMIX is (DSound-based) hardware mixing for Xbox
!if !defined(SILVER) && !defined(NOXMIX)
C_DEFINES = $(C_DEFINES) -DXMIX
!endif

C_DEFINES= $(C_DEFINES) -D_WIN32 -DINC_OLE2 -D_MT -DXBOX -DUNICODE

MSC_WARNING_LEVEL=/W3 /WX
MSC_OPTIMIZATION =  $(MSC_OPTIMIZATION) /Od

USER_C_FLAGS=$(USER_C_FLAGS) /FAcs

INCLUDES=$(INCLUDES);   \
    ..\..\shared;			\
    ..\..\xprivate; \
    ..

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchime.h
PRECOMPILED_PCH=pchime.pch
PRECOMPILED_OBJ=pchime.obj

SOURCES=\
alist.cpp			\
dmgraph.cpp         \
debug.cpp			\
dmperf.cpp          \
dmprfdll.cpp        \
dmsegobj.cpp        \
dmsstobj.cpp        \
medparam_i.c		\
queue.cpp           \
seqtrack.cpp        \
sysextrk.cpp        \
tempotrk.cpp        \
tsigtrk.cpp         \
dmstrm.cpp		   \
audpath.cpp         \
lyrictrk.cpp        \
marktrk.cpp         \
paramtrk.cpp        \
segtrtrk.cpp        \
smartref.cpp        \
song.cpp            \
trackhelp.cpp       \
wavtrack.cpp        \
phoneyds.cpp		\
dowork.cpp			\
dmcreate.cpp 		\
cmixbins.cpp

