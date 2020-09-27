TARGETNAME=tunnel
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\m_tunnel.lib \
    $(TEST_LIB_PATH)\l_tunnel.lib

SOURCES=\
    include.cpp
