TARGETNAME=skulls
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_skulls.lib \
    $(TEST_LIB_PATH)\l_skulls.lib

SOURCES=\
    include.cpp
