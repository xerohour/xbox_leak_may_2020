TARGETNAME=xbox548
TARGETTYPE=DYNLINK
TARGETPATH=obj
DLLDEF=..\..\..\..\frame\gentest\dll\gentest.def

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx548.lib

SOURCES=\
    include.cpp
