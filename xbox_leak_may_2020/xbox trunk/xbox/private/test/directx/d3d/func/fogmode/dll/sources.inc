!include ..\..\..\..\sources.inc

TARGETNAME=fogmode
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_fogmod.lib

SOURCES=\
    include.cpp
