TARGETNAME=palette0
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_palet0.lib \
    $(TEST_LIB_PATH)\l_palet0.lib

SOURCES=\
    include.cpp
