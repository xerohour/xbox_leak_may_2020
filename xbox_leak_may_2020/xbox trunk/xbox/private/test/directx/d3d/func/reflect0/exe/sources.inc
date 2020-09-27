TARGETNAME=reflect0
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_rflct0.lib \
    $(TEST_LIB_PATH)\l_rflct0.lib

SOURCES=\
    include.cpp
