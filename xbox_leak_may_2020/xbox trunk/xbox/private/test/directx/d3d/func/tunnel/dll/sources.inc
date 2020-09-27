!include ..\..\..\..\sources.inc

TARGETNAME=tunnel
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_tunnel.lib

SOURCES=\
    include.cpp
