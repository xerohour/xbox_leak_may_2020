TARGETNAME=rteapot
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_rtpot.lib \
    $(TEST_LIB_PATH)\l_rtpot.lib

SOURCES=\
    include.cpp
