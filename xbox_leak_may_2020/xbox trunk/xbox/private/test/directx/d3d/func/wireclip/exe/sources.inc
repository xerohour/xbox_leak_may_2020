TARGETNAME=Wirefram
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
    $(TEST_LIB_PATH)\m_wclip.lib \
    $(TEST_LIB_PATH)\l_wclip.lib

SOURCES=\
    include.cpp
