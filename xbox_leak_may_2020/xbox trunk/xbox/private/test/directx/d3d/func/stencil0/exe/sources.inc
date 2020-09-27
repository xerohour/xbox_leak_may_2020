TARGETNAME=stencil0
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_stncl0.lib \
    $(TEST_LIB_PATH)\l_stncl0.lib

SOURCES=\
    include.cpp
