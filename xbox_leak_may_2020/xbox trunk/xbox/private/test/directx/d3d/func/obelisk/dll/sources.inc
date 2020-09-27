!include ..\..\..\..\sources.inc

TARGETNAME=obelisk
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_obelsk.lib

SOURCES=\
    include.cpp
