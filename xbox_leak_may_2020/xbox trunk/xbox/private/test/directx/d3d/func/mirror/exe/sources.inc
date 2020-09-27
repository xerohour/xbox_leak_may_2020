TARGETNAME=mirror
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_mirror.lib \
    $(TEST_LIB_PATH)\l_mirror.lib

SOURCES=\
    include.cpp
