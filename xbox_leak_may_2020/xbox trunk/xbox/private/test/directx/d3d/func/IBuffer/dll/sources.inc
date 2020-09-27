!include ..\..\..\..\sources.inc

TARGETNAME=IBuffer
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_ibuffr.lib

SOURCES=\
    include.cpp
