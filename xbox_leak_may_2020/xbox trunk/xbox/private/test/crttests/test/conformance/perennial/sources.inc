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
USER_C_FLAGS=-TP -DQA_DBG_OUTPUT_DEBUGWIN=1 -DXBOX -DPEREN_TEST
USE_NATIVE_EH=1
LINKER_FLAGS=/IGNORE:4006

SOURCES=\
	..\a15039.c \
	..\p15038.c \
	..\p15071.c \
	..\a15040.c \
	..\a15050.c \
	..\a15051.c \
	..\a15052.c \
	..\a15053.c \
	..\a15054.c \
	..\p15036.c \
	..\p15037.c \
	..\filbuf_1.c \
	..\filbuf_2.c \
	..\filbuf_3.c \
	..\format1.c \
	..\format2.c \
	..\format3.c \
	..\format4.c \
	..\format5.c \
	..\format6.c \
	..\format7.c \
	..\fstream1.c \
	..\fstream2.c \
	..\fstream3.c \
	..\ios2.c \
	..\ios3.c \
	..\ios4.c \
	..\ios5.c \
	..\ios6.c \
	..\iosenum1.c \
	..\iosenum3.c \
	..\iosenum4.c \
	..\iosenum5.c \
	..\istream1.c \
	..\istreama.c \
	..\istreamb.c \
	..\istreamc.c \
	..\istream2.c \
	..\istream3.c \
	..\istream4.c \
	..\istream5.c \
	..\istream6.c \
	..\istream7.c \
	..\istream8.c \
	..\istream9.c \
	..\manip1.c \
	..\manip3.c \
	..\manip5.c \
	..\ofstream.c \
	..\osfx.c \
	..\osfx2.c \
	..\ostream1.c \
	..\ostream2.c \
	..\ostream3.c \
	..\ostream4.c \
	..\ostream5.c \
	..\ostream6.c \
	..\ostream7.c \
	..\ostream8.c \
	..\sbuf1.c \
	..\sbuf2.c \
	..\sbuf3.c \
	..\sbuf4.c \
	..\sbuf5.c \
	..\sbuf6.c \
	..\ssbuf1.c \
	..\ssbuf2.c \
	..\ssbuf3.c \
	..\ssbuf4.c \
	..\ssbuf5.c \
	..\ssbuf6.c \
	..\stdiobuf.c \
	..\stream.c \
	..\strm_wa.c \
	..\syncstd1.c \
	..\syncstd2.c \
	..\perennial.cpp

INCLUDES=$(XDK_INC_PATH);$(INCLUDES)

MSC_WARNING_LEVEL=/W2

SOURCES_USED=..\sources.inc
