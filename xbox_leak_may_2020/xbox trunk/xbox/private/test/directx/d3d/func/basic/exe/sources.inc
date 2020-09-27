TARGETNAME=basic
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_basic.lib \
    $(TEST_LIB_PATH)\l_basic.lib

SOURCES=\
    include.cpp

