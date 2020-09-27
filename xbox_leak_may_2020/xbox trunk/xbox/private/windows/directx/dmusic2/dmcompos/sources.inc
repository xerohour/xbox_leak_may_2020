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

TARGETNAME=dmcompos$(LIBEXT)

!IF !$(FREEBUILD) && defined( DIRECTX_REDIST )
TARGETNAME=dmcompod
!ENDIF


TARGETTYPE = LIBRARY
TARGETPATH = ..\..\obj
USE_MAPSYM=1
USE_NOLIBS=1

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"

!if "$(BuildMode)" == "internal"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!else
C_DEFINES = $(C_DEFINES) -DRDEBUG=1
!endif

!endif

C_DEFINES= $(C_DEFINES) -D_WIN32 -D_MT -DXBOX -DUNICODE

MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /W3 /WX

INCLUDES=$(INCLUDES);   \
    ..\..\shared; \
    ..\..\xprivate; \
    ..

	
PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchcompos.h
PRECOMPILED_PCH=pchcompos.pch
PRECOMPILED_OBJ=pchcompos.obj

SOURCES=\
dmcmpdll.cpp        \
dmcompos.cpp        \
dmpers.cpp          \
dmtempl.cpp         \
spsttrk.cpp         \
str.cpp             \
perstrk.cpp         \
dmcomp2.cpp         \
memstm.cpp


