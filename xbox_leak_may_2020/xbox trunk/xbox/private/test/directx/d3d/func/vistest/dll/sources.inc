!include ..\..\..\..\sources.inc

TARGETNAME=vistest
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

INCLUDES=$(INCLUDES);$(BASEDIR)\private\ntos\inc

SOURCES=\
    include.cpp
