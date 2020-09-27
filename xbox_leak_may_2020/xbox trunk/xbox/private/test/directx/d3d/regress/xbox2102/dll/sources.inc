!include ..\..\..\..\sources.inc

TARGETNAME=mirror
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_mirror.lib

SOURCES=\
    include.cpp
