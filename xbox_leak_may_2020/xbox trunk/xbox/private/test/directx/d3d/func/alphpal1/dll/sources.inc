!include ..\..\..\..\sources.inc

TARGETNAME=alphpal1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_alpal1.lib

SOURCES=\
    include.cpp
