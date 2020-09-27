!include ..\..\..\..\sources.inc

TARGETNAME=Portal
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_portal.lib

SOURCES=\
    include.cpp
