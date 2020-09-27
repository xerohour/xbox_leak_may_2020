TARGETNAME=mstage4
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_mstag4.lib \
    $(TEST_LIB_PATH)\l_mstag4.lib

SOURCES=\
    include.cpp
