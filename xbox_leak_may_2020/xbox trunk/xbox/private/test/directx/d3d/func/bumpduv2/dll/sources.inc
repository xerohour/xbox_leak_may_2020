!include ..\..\..\..\sources.inc

TARGETNAME=bumpduv2
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_bumpd2.lib

SOURCES=\
    include.cpp
