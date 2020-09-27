!include ..\..\..\..\sources.inc

TARGETNAME=terrain
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_terain.lib

SOURCES=\
    include.cpp
