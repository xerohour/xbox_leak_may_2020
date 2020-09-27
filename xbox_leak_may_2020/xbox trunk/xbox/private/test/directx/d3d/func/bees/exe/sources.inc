TARGETNAME=bees
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_bees.lib \
    $(TEST_LIB_PATH)\l_bees.lib

SOURCES=\
    include.cpp
