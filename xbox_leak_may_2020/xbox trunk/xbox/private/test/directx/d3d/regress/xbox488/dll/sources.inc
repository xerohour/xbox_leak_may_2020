TARGETNAME=xbox488
TARGETTYPE=DYNLINK
TARGETPATH=obj
DLLDEF=..\..\..\..\frame\gentest\dll\gentest.def

!include ..\..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\l_rx488.lib

SOURCES=\
    include.cpp
