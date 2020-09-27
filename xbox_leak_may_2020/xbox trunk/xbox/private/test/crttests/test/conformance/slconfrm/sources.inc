!IF 0

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    sources

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built

!ENDIF

TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

386_STDCALL=0
USER_C_FLAGS=-TP -DQA_DBG_OUTPUT_DEBUGWIN=1 -DXBOX
USE_NATIVE_EH=1
LINKER_FLAGS=/IGNORE:4006

SOURCES=\
	..\t170.cpp \
	..\t180.cpp \
	..\t182.cpp \
	..\t183.cpp \
	..\t190.cpp \
	..\t212.cpp \
	..\t217.cpp \
	..\t230.cpp \
	..\t238.cpp \
	..\t251.cpp \
	..\t252.cpp \
	..\t258.cpp \
	..\t260.cpp \
	..\t262.cpp \
	..\t270.cpp \
	..\t273.cpp \
	..\t27e.cpp \
	..\t27f.cpp \
	..\t27j.cpp \
	..\txd0.cpp \
	..\txd2.cpp \
	..\slconfrm.cpp

INCLUDES=$(XDK_INC_PATH);$(INCLUDES)

MSC_WARNING_LEVEL=/W2

SOURCES_USED=..\sources.inc
