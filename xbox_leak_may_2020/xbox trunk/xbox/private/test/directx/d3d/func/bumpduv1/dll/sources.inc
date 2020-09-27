!include ..\..\..\..\sources.inc

TARGETNAME=bumpduv1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_bumpd1.lib

SOURCES=\
    include.cpp
