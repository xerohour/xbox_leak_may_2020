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

SOURCES=..\ph_util.c \
        ..\plumhall.c \
        ..\d41.c \
        ..\d410.c \
        ..\d410drv.c \
        ..\d411.c \
        ..\d411b.c \
        ..\d411drv.c \
        ..\d412.c \
        ..\d412drv.c \
        ..\d41a.c \
        ..\d41b.c \
        ..\d41c.c \
        ..\d41d.c \
        ..\d41drv.c \
        ..\d42.c \
        ..\d42a.c \
        ..\d42drv.c \
        ..\d43.c \
        ..\d43drv.c \
        ..\d44.c \
        ..\d44drv.c \
        ..\d45.c \
        ..\d45drv.c \
        ..\d46.c \
        ..\d46drv.c \
        ..\d47.c \
        ..\d47b.c \
        ..\d47drv.c \
        ..\d48.c \
        ..\d48drv.c \
        ..\d49.c \
        ..\d49a.c \
        ..\d49drv.c

MSC_WARNING_LEVEL=/W3

SOURCES_USED=..\sources.inc

