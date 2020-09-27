!include ..\..\..\..\sources.inc

TARGETNAME=xbox2083
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx2083.lib

SOURCES=\
    include.cpp
