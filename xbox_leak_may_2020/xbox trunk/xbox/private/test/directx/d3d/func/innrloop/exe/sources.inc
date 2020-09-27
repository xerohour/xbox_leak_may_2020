TARGETNAME=innrloop
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_inloop.lib \
    $(TEST_LIB_PATH)\l_inloop.lib

SOURCES=\
    include.cpp
