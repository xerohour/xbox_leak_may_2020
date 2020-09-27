!include ..\..\..\..\sources.inc

TARGETNAME=wbuffer
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_wbuffr.lib

SOURCES=\
    include.cpp
