!IF 0

Copyright (c) 1989-2002  Microsoft Corporation

!ENDIF

MAJORCOMP=ntos
MINORCOMP=idex

TARGETPATH=$(BASEDIR)\private\ntos\obj
TARGETTYPE=LIBRARY

INCLUDES=..;$(BASEDIR)\private\ntos\inc;$(BASEDIR)\public\ddk\inc;$(BASEDIR)\private\inc\crypto

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

!if defined(ARCADE)
C_DEFINES=$(C_DEFINES) -DARCADE
!endif

SOURCES=..\cdrom.c    \
        ..\channel.c  \
        ..\disk.c     \
        ..\driver.c

PRECOMPILED_INCLUDE=..\idex.h
PRECOMPILED_PCH=idex.pch
PRECOMPILED_OBJ=idex.obj
