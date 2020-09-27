TARGETNAME=dxtnoise
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_dnoise.lib \
    $(TEST_LIB_PATH)\l_dnoise.lib

SOURCES=\
    include.cpp
