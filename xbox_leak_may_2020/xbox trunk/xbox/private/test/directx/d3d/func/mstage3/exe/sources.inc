TARGETNAME=mstage3
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_mstag3.lib \
    $(TEST_LIB_PATH)\l_mstag3.lib

SOURCES=\
    include.cpp
