!include ..\..\..\..\sources.inc

TARGETNAME=xbox2251
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx2251.lib

SOURCES=\
    include.cpp
