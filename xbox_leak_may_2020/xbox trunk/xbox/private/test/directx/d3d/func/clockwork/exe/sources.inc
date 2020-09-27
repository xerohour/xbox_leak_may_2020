TARGETNAME=clockwrk
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_clwork.lib \
    $(TEST_LIB_PATH)\l_clwork.lib

SOURCES=\
    include.cpp
