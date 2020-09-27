TARGETNAME=persistd
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_prsist.lib \
    $(TEST_LIB_PATH)\l_prsist.lib

SOURCES=\
    include.cpp
