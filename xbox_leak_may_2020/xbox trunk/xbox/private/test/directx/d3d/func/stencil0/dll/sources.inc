!include ..\..\..\..\sources.inc

TARGETNAME=stencil0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_stncl0.lib

SOURCES=\
    include.cpp
