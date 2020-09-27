!include ..\..\..\..\sources.inc

TARGETNAME=dragnfly
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_drgfly.lib

SOURCES=\
    include.cpp
