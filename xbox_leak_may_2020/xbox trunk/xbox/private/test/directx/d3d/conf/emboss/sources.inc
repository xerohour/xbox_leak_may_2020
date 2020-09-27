!include ..\..\..\sources.inc

TARGETNAME=l_emboss
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	emboss.cpp \
	flags.cpp
