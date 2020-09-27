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

TARGETNAME=dmstyle$(LIBEXT)

!IF !$(FREEBUILD) && defined( DIRECTX_REDIST )
TARGETNAME=$(TARGETNAME)d
!ENDIF

!if $(USE_VC7)
ENABLE_LINK_TIME_CODE_GENERATION=0
!endif

TARGETPATH = ..\..\obj
TARGETTYPE=LIBRARY
UMTYPE=xboxdll
DLLDEF=dmstyle.def
DLLENTRY=DllMain
USE_MAPSYM=1

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"

!if "$(BuildMode)" == "internal"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!else
C_DEFINES = $(C_DEFINES) -DRDEBUG=1
!endif

!endif

C_DEFINES= $(C_DEFINES) -D_WIN32 -DINC_OLE2 -D_MT -DXBOX -DUNICODE

INCLUDES=$(INCLUDES);   \
    $(BASEDIR)\public\sdk\inc;\
    $(BASEDIR)\private\inc;\
    ..\..\shared;\
    ..\..\xprivate;\
    ..

SOURCES=\
chordtrk.cpp                \
cmmdtrk.cpp                 \
dmsect.cpp                  \
dmstydll.cpp                \
dmstyle.cpp                 \
motiftrk.cpp                \
ptrntrk.cpp                 \
styletrk.cpp                \
audtrk.cpp                  \
mutetrk.cpp                 \
dmstyle2.cpp                \
filter.cpp                  \
mgentrk.cpp

