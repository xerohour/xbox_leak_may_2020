!include ..\..\..\sources.inc

TARGETNAME=l_texgen
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	texgen.cpp \
	default.cpp \
	properties.cpp
