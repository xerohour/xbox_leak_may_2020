!include ..\..\..\..\sources.inc

TARGETNAME=innrloop
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_inloop.lib

SOURCES=\
    include.cpp
