TARGETNAME=snowfall
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_snow.lib \
    $(TEST_LIB_PATH)\l_snow.lib

SOURCES=\
    include.cpp
