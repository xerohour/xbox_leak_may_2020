!include ..\..\..\..\sources.inc

TARGETNAME=basic
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_basic.lib

SOURCES=\
    include.cpp
