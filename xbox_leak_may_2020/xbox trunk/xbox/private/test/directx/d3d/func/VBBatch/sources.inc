!include ..\..\..\sources.inc

TARGETNAME=l_vbatch
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	VBBatch.cpp \
	Default.cpp \
	Vertices.cpp
