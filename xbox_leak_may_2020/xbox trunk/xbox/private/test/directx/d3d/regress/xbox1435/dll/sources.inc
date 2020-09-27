!include ..\..\..\..\sources.inc

TARGETNAME=xbox1435
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx1435.lib

SOURCES=\
    include.cpp
