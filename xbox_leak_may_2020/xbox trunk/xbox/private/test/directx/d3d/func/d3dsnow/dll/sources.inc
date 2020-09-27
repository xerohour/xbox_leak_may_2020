!include ..\..\..\..\sources.inc

TARGETNAME=d3dsnow
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_snow.lib

SOURCES=\
    include.cpp
