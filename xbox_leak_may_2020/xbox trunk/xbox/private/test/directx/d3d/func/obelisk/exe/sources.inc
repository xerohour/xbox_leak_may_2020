TARGETNAME=obelisk
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_obelsk.lib \
    $(TEST_LIB_PATH)\l_obelsk.lib

SOURCES=\
    include.cpp
