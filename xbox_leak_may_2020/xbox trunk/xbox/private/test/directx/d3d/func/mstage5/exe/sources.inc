TARGETNAME=mstage5
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_mstag5.lib \
    $(TEST_LIB_PATH)\l_mstag5.lib

SOURCES=\
    include.cpp
