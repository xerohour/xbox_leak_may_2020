!include ..\..\..\..\sources.inc

TARGETNAME=litetype
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_litetp.lib

SOURCES=\
    include.cpp
