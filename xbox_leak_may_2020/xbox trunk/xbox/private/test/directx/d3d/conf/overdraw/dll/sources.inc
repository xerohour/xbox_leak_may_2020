!include ..\..\..\..\sources.inc

TARGETNAME=overdraw
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_ovrdrw.lib

SOURCES=\
    include.cpp
