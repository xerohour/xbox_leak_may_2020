TARGETNAME=textr5
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_textr5.lib \
    $(TEST_LIB_PATH)\l_textr5.lib

SOURCES=\
    include.cpp
