TARGETNAME=chessbrd
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_chess.lib \
    $(TEST_LIB_PATH)\l_chess.lib

SOURCES=\
    include.cpp
