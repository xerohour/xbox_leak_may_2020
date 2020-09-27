!include ..\..\..\..\sources.inc

TARGETNAME=RearViewMirror
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rearvm.lib

SOURCES=\
    include.cpp
