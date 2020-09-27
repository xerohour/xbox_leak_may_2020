!include ..\..\sources.inc

TARGETNAME=common
TARGETPATH=obj
TARGETTYPE=LIBRARY

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\commoni.h
PRECOMPILED_PCH=commoni.pch
PRECOMPILED_OBJ=commoni.obj

!if $(FREEBUILD)
C_DEFINES=$(C_DEFINES) -DASSUME_VALID_PARAMETERS
!else
C_DEFINES=$(C_DEFINES) -DDEBUG
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Od
!endif

SOURCES=..\bilink.cpp \
        ..\comutil.cpp \
	..\dndbg.cpp \
	..\dneterrors.cpp \
	..\fpm.cpp \
	..\guidutil.cpp \
	..\osind.cpp \
	..\packbuff.cpp \
	..\queue.cpp \
	..\rcbuffer.cpp \
	..\strutils.cpp

USE_NOLIBS=1
