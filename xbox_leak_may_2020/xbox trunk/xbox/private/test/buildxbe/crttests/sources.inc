!IF 0

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    sources

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

TARGETPATH=obj
TARGETTYPE=PROGRAM

UMTYPE=xboxapp

USE_MAPSYM=1

SOURCES=..\main.cpp

INCLUDES=$(SDK_INC_PATH); \
         $(PRIVATE_INC_PATH); \
         $(BASEDIR)\private\ntos\inc; \
         ..

CRTBASE=$(BASEDIR)\private\test\crttests\test

XE_FLAGS=/TESTID:0xA7049956 /INITFLAGS:3

PRECOMPILED_INCLUDE=..\crttestp.h
PRECOMPILED_CXX=1

SOURCES_USED=..\sources.inc

USE_STL=1
USE_IOSTREAM=1
USE_RTTI=1

# ignore multiply defined symbols warning
LINKER_FLAGS=/IGNORE:4006

TARGETLIBS=$(CRTBASE)\conformance\lib\*\plumhall$(MTLIB).lib \
           $(CRTBASE)\conformance\lib\*\slconfrm$(MTLIB).lib
