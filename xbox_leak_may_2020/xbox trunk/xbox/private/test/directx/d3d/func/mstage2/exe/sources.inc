TARGETNAME=mstage2
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_mstag2.lib \
    $(TEST_LIB_PATH)\l_mstag2.lib

SOURCES=\
    include.cpp
