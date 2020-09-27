TARGETNAME=hightide
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_htide.lib \
    $(TEST_LIB_PATH)\l_htide.lib

SOURCES=\
    include.cpp
