!include ..\..\..\..\sources.inc

TARGETNAME=VBuffer
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vbuffr.lib

SOURCES=\
    include.cpp
