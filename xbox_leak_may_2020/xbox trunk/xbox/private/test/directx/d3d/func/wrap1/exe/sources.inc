TARGETNAME=wrap1
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_wrap1.lib \
    $(TEST_LIB_PATH)\l_wrap1.lib

SOURCES=\
    include.cpp
