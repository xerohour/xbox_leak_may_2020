TARGETNAME=pslights
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_pslite.lib \
    $(TEST_LIB_PATH)\l_pslite.lib

SOURCES=\
    include.cpp
