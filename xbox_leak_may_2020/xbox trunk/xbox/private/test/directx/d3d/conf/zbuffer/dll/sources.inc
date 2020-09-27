!include ..\..\..\..\sources.inc

TARGETNAME=zbuffer
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_zbuffr.lib

SOURCES=\
    include.cpp
