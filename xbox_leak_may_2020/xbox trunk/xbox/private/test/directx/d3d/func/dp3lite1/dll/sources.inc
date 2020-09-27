!include ..\..\..\..\sources.inc

TARGETNAME=dp3lite
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_dp3lit.lib

SOURCES=\
    include.cpp
