TARGETNAME=textr6
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_textr6.lib \
    $(TEST_LIB_PATH)\l_textr6.lib

SOURCES=\
    include.cpp
