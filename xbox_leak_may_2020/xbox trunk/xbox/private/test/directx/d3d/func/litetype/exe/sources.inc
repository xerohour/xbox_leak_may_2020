TARGETNAME=litetype
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_litetp.lib \
    $(TEST_LIB_PATH)\l_litetp.lib

SOURCES=\
    include.cpp
