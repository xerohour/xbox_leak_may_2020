!include ..\..\..\..\sources.inc

TARGETNAME=xbox2271
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx2271.lib

SOURCES=\
    include.cpp
